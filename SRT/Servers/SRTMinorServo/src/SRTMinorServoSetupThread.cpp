#include "SRTMinorServoSetupThread.h"

using namespace MinorServo;

SRTMinorServoSetupThread::SRTMinorServoSetupThread(const ACE_CString& name, SRTMinorServoBossCore& core, const ACS::TimeInterval& response_time, const ACS::TimeInterval& sleep_time) :
    ACS::Thread(name, response_time, sleep_time),
    m_core(core)
{
    AUTO_TRACE("SRTMinorServoSetupThread::SRTMinorServoSetupThread()");
}

SRTMinorServoSetupThread::~SRTMinorServoSetupThread()
{
    AUTO_TRACE("SRTMinorServoSetupThread::~SRTMinorServoSetupThread()");
}

void SRTMinorServoSetupThread::onStart()
{
    AUTO_TRACE("SRTMinorServoSetupThread::onStart()");

    this->setSleepTime(500000);   // 50 milliseconds
    m_start_time = IRA::CIRATools::getUNIXEpoch();

    m_status = 0;

    SRTMinorServoFocalConfiguration commanded_configuration = m_core.m_commanded_configuration.load();
    m_LDO_configuration = LDOConfigurationNameTable.left.at(commanded_configuration);
    m_gregorian_cover_position = commanded_configuration == CONFIGURATION_PRIMARY ? COVER_STATUS_CLOSED : COVER_STATUS_OPEN;

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoSetupThread::onStart()", (LM_NOTICE, ("Setting up MinorServos with '" + m_core.m_commanded_setup + "' configuration").c_str()));
}

void SRTMinorServoSetupThread::onStop()
{
    AUTO_TRACE("SRTMinorServoSetupThread::onStop()");

    if(m_core.m_motion_status.load() == MOTION_STATUS_TRACKING)
    {
        try
        {
            m_core.startThread(m_core.m_tracking_thread);
        }
        catch(ComponentErrors::ComponentErrorsEx& ex)
        {
            ACS_SHORT_LOG((LM_ERROR, ex.errorTrace.routine));
        }
    }

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoSetupThread::onStop()", (LM_NOTICE, "MinorServos configured"));
}

void SRTMinorServoSetupThread::runLoop()
{
    AUTO_TRACE("SRTMinorServoSetupThread::runLoop()");

    try
    {
        m_core.checkLineStatus();
    }
    catch(MinorServoErrors::StatusErrorEx& ex)
    {
        ACS_SHORT_LOG((LM_ERROR, ex.errorTrace.routine));
        this->setStopped();
        return;
    }

    if(IRA::CIRATools::getUNIXEpoch() - m_start_time >= SETUP_TIMEOUT)
    {
        ACS_LOG(LM_FULL_INFO, "SRTMinorServoSetupThread::runLoop()", (LM_CRITICAL, std::string("Timeout while configuring the system. Stopped at status " + std::to_string(m_status)).c_str()));
        m_core.setError(ERROR_CONFIG_ERROR);
        this->setStopped();
        return;
    }

    switch(m_status)
    {
        case 0: // Check if all the servos stopped
        {
            if(std::all_of(m_core.m_servos.begin(), m_core.m_servos.end(), [](const std::pair<std::string, SRTBaseMinorServo_ptr>& servo) -> bool
            {
                ACSErr::Completion_var comp;
                return servo.second->operative_mode()->get_sync(comp.out()) == OPERATIVE_MODE_STOP ? true : false;
            }))
            {
                m_status = 1;
            }

            break;
        }
        case 1: // Set all the servo offsets to 0
        {
            for(const auto& [name, servo] : m_core.m_servos)
            {
                // Not sure about this
                servo->clearSystemOffsets();
                servo->clearUserOffsets();
            }

            m_status = 2;
            break;
        }
        case 2: // Send the SETUP command
        {
            try
            {
                if(!m_core.m_socket.sendCommand(SRTMinorServoCommandLibrary::setup(m_LDO_configuration)).checkOutput())
                {
                    ACS_LOG(LM_FULL_INFO, "SRTMinorServoSetupThread::runLoop()", (LM_CRITICAL, "Received NAK in response to a SETUP command."));
                    m_core.setError(ERROR_CONFIG_ERROR);
                    this->setStopped();
                    return;
                }
                else
                {
                    m_status = 3;
                }
            }
            catch(...)
            {
                ACS_LOG(LM_FULL_INFO, "SRTMinorServoSetupThread::runLoop()", (LM_CRITICAL, "Communication error while sending a SETUP command."));
                m_core.setError(ERROR_CONFIG_ERROR);
                this->setStopped();
                return;
            }

            break;
        }
        case 3: // Wait for the system to show the commanded configuration
        {
            if(m_core.m_status.getFocalConfiguration() == m_core.m_commanded_configuration.load())
            {
                m_status = 4;
            }

            break;
        }
        case 4: // Wait for all the servos to reach the desired configuration
        {
            // Then we cycle through all the servos and make sure their operative mode is SETUP
            if(std::all_of(m_core.m_servos.begin(), m_core.m_servos.end(), [](const std::pair<std::string, SRTBaseMinorServo_ptr>& servo) -> bool
            {
                ACSErr::Completion_var comp;
                return servo.second->operative_mode()->get_sync(comp.out()) == OPERATIVE_MODE_SETUP;
            }))
            {
                m_status = 5;
            }

            break;
        }
        case 5: // Load the servos coefficients
        {
            for(const auto& [servo_name, servo] : m_core.m_servos)
            {
                try
                {
                    if(servo->setup(m_core.m_commanded_setup.c_str(), m_core.m_as_configuration.load() == Management::MNG_TRUE ? false : true))
                    {
                        m_core.m_current_servos[servo_name] = servo;
                        try
                        {
                            m_core.m_current_tracking_servos[servo_name] = m_core.m_tracking_servos.at(servo_name);
                        }
                        catch(...)
                        {
                            // Not a tracking servo, ignore
                        }
                    }
                }
                catch(...)
                {
                    ACS_LOG(LM_FULL_INFO, "SRTMinorServoSetupThread::runLoop()", (LM_CRITICAL, ("Error while loading a SETUP to servo'" + servo_name + "'.").c_str()));
                    m_core.setError(ERROR_CONFIG_ERROR);
                    this->setStopped();
                    return;
                }
            }

            if(m_core.m_as_configuration.load() == Management::MNG_FALSE)
            {
                // We commanded a configuration which does not use the active surface, therefore we need to send some slightly different coordinates with a preset command

                for(const auto& [servo_name, servo] : m_core.m_current_servos)
                {
                    try
                    {
                        servo->preset(*servo->calcCoordinates(45));
                    }
                    catch(MinorServoErrors::MinorServoErrorsEx& ex)
                    {
                        ACS_SHORT_LOG((LM_ERROR, ex.errorTrace.routine));
                        m_core.setError(ERROR_COMMAND_ERROR);
                        this->setStopped();
                        return;
                    }
                }
                m_status = 6;
            }
            else
            {
                // AS configuration, jump directly to state 7
                m_status = 7;
            }

            break;
        }
        case 6: // Wait for the used servos to reach the PRESET configuration
        {
            if(std::all_of(m_core.m_current_servos.begin(), m_core.m_current_servos.end(), [this](const std::pair<std::string, SRTBaseMinorServo_ptr>& servo) -> bool
            {
                ACSErr::Completion_var comp;
                return servo.second->operative_mode()->get_sync(comp.out()) == OPERATIVE_MODE_PRESET ? true : false;
            }))
            {
                m_status = 7;
            }

            break;
        }
        case 7: // Check the status of the gregorian cover
        {
            if(m_core.m_status.getGregorianCoverPosition() == m_gregorian_cover_position)
            {
                m_status = 8;
            }

            break;
        }
        case 8: // Finally set all the variables values and eventually start the elevation tracking thread
        {
            m_core.m_actual_setup = m_core.m_commanded_setup;
            m_core.m_starting.store(Management::MNG_FALSE);
            m_core.m_ready.store(Management::MNG_TRUE);
            m_core.m_subsystem_status.store(Management::MNG_OK);

            if(m_core.m_elevation_tracking_enabled.load() == Management::MNG_TRUE)
            {
                m_core.m_motion_status.store(MOTION_STATUS_TRACKING);
            }
            else
            {
                m_core.m_motion_status.store(MOTION_STATUS_CONFIGURED);
            }

            this->setStopped();
            break;
        }
    }
}
