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

    //SRTMinorServoFocalConfiguration commanded_configuration = m_core.m_commanded_configuration.load();
    //m_LDO_configuration = LDOConfigurationNameTable.left.at(commanded_configuration);
    //m_gregorian_cover_position = commanded_configuration == CONFIGURATION_PRIMARY ? COVER_STATUS_CLOSED : COVER_STATUS_OPEN;

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoSetupThread::onStart()", (LM_NOTICE, ("SETUP THREAD STARTED WITH '" + m_core.m_commanded_setup + "' CONFIGURATION").c_str()));
}

void SRTMinorServoSetupThread::onStop()
{
    AUTO_TRACE("SRTMinorServoSetupThread::onStop()");

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoSetupThread::onStop()", (LM_NOTICE, "SETUP THREAD STOPPED"));
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
        m_core.setFailure();
        this->setStopped();
        return;
    }

    if(IRA::CIRATools::getUNIXEpoch() - m_start_time >= SETUP_TIMEOUT)
    {
        ACS_LOG(LM_FULL_INFO, "SRTMinorServoSetupThread::runLoop()", (LM_CRITICAL, "Timeout while performing a setup operation."));
        m_core.setFailure();
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

            //m_status = 2;
            //m_status = 100;
            m_status = 5;
            break;
        }
        /*case 100: // Send the STOW command to the gregorian cover
        {
            if(!m_core.m_socket.sendCommand(SRTMinorServoCommandLibrary::stow("Gregoriano", m_gregorian_cover_position)).checkOutput())
            {
                ACS_LOG(LM_FULL_INFO, "SRTMinorServoSetupThread::runLoop()", (LM_CRITICAL, "Received NAK when setting the gregorian cover position."));
                m_core.setFailure();
                this->setStopped();
                return;
            }

            m_status = 5;
            break;
        }*/
        /*case 2: // Send the SETUP command
        {
            try
            {
                if(!m_core.m_socket.sendCommand(SRTMinorServoCommandLibrary::setup(m_LDO_configuration)).checkOutput())
                {
                    ACS_LOG(LM_FULL_INFO, "SRTMinorServoSetupThread::runLoop()", (LM_CRITICAL, "Received NAK in response to a SETUP command."));
                    m_core.setFailure();
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
                m_core.setFailure();
                this->setStopped();
                return;
            }

            break;
        }
        case 3: // Wait for the system to show the commanded configuration
        {
            ACS::Time comp;
            if(m_core.m_component.m_current_configuration_devio->read(comp) == m_core.m_commanded_configuration.load())
            {
                m_status = 4;
            }

            break;
        }
        case 4: // Wait for the whole system to reach the desired configuration
        {
            ACSErr::Completion_var comp;
            // First we check the status of the gregorian cover
            bool completed = m_core.m_component.m_gregorian_cover_devio->read(comp) == m_gregorian_cover_position ? true : false;

            // Then we cycle through all the servos and make sure their operative mode is SETUP
            if(completed && std::all_of(m_core.m_servos.begin(), m_core.m_servos.end(), [](const std::pair<std::string, SRTBaseMinorServo_ptr>& servo) -> bool
            {
                ACSErr::Completion_var comp;
                return servo.second->operative_mode()->get_sync(comp.out()) == OPERATIVE_MODE_SETUP ? true : false;
            }))
            {
                m_status = 5;
            }

            break;
        }*/
        case 5: // Load the servos coefficients and send a PRESET command
        {
            for(const auto& [servo_name, servo] : m_core.m_servos)
            {
                try
                {
                    if(servo->setup(m_core.m_commanded_setup.c_str()))
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
                    m_core.setFailure();
                    this->setStopped();
                    return;
                }
            }

            // This step is necessary because we have _ASACTIVE configurations that have a slightly different position from the commanded one
            // Unfortunately, the Leonardo implementation accepts a fixed number of configurations, therefore we share the _ASACTIVE and AS not active configurations for each focal position
            for(const auto& [servo_name, servo] : m_core.m_current_servos)
            {
                try
                {
                    servo->preset(*servo->calcCoordinates(45));
                }
                catch(MinorServoErrors::MinorServoErrorsEx& ex)
                {
                    ACS_SHORT_LOG((LM_ERROR, ex.errorTrace.routine));
                    m_core.setFailure();
                    this->setStopped();
                    return;
                }
            }

            m_status = 6;
            break;
        }
        case 6: // Wait for the whole system to reach the PRESET configuration
        {
            //ACSErr::Completion_var comp;
            // First we check the status of the gregorian cover
            //bool completed = m_core.m_component.m_gregorian_cover_devio->read(comp) == m_gregorian_cover_position ? true : false;

            if(/*completed && */std::all_of(m_core.m_current_servos.begin(), m_core.m_current_servos.end(), [this](const std::pair<std::string, SRTBaseMinorServo_ptr>& servo) -> bool
            {
                ACSErr::Completion_var comp;
                return servo.second->operative_mode()->get_sync(comp.out()) == OPERATIVE_MODE_PRESET ? true : false;
            }))
            {
                m_status = 7;
            }

            break;
        }
        case 7: // Finally set all the variables values and eventually start the elevation tracking thread
        {
            m_core.m_actual_setup = m_core.m_commanded_setup;
            m_core.m_starting.store(Management::MNG_FALSE);
            m_core.m_ready.store(Management::MNG_TRUE);
            m_core.m_subsystem_status.store(Management::MNG_OK);

            if(m_core.m_elevation_tracking_enabled.load() == Management::MNG_TRUE)
            {
                m_core.m_motion_status.store(MOTION_STATUS_TRACKING);
                try
                {
                    m_core.startThread(m_core.m_tracking_thread);
                }
                catch(ComponentErrors::ComponentErrorsEx& ex)
                {
                    ACS_SHORT_LOG((LM_ERROR, ex.errorTrace.routine));
                    m_core.setFailure();
                    this->setStopped();
                    return;
                }
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
