#include "SRTMinorServoSetupThread.h"

SRTMinorServoSetupThread::SRTMinorServoSetupThread(const ACE_CString& name, SRTMinorServoBossCore* core, const ACS::TimeInterval& responseTime, const ACS::TimeInterval& sleepTime):
    ACS::Thread(name, responseTime, sleepTime),
    m_core(core)
{
    m_thread_name = std::string(name.c_str());
    AUTO_TRACE(m_thread_name + "::SRTMinorServoSetupThread()");
}

SRTMinorServoSetupThread::~SRTMinorServoSetupThread()
{
    AUTO_TRACE(m_thread_name + "::~SRTMinorServoSetupThread()");
}

void SRTMinorServoSetupThread::onStart()
{
    AUTO_TRACE(m_thread_name + "::onStart()");
    this->setSleepTime(500000);   // 50 milliseconds
    m_start_time = CIRATools::getUNIXEpoch();

    MinorServo::SRTMinorServoFocalConfiguration commanded_configuration = m_core->m_commanded_configuration;
    m_LDO_configuration = MinorServo::LDOConfigurationNameTable.left.at(commanded_configuration);
    m_gregorian_cover_position = commanded_configuration == MinorServo::CONFIGURATION_PRIMARY ? MinorServo::COVER_STATUS_CLOSED : MinorServo::COVER_STATUS_OPEN;
    m_status = 0;
}

void SRTMinorServoSetupThread::onStop()
{
    AUTO_TRACE(m_thread_name + "::onStop()");
}

void SRTMinorServoSetupThread::runLoop()
{
    AUTO_TRACE(m_thread_name + "::runLoop()");

    try
    {
        m_core->checkControl();
        m_core->checkEmergency();
    }
    catch(MinorServoErrors::StatusErrorExImpl& ex)
    {
        ACS_LOG(LM_FULL_INFO, m_thread_name + "::runLoop()", (LM_ERROR, ex.getData("Reason").c_str()));
        this->setStopped();
        return;
    }

    if(CIRATools::getUNIXEpoch() - m_start_time >= SETUP_TIMEOUT)
    {
        m_core->m_starting = Management::MNG_FALSE;
        m_core->m_motion_status = MinorServo::MOTION_STATUS_ERROR;
        m_core->m_subsystem_status = Management::MNG_FAILURE;
        ACS_LOG(LM_FULL_INFO, m_thread_name + "::runLoop()", (LM_ERROR, "Timeout while performing a setup operation."));
        this->setStopped();
    }

    ACSErr::Completion_var comp;

    switch(m_status)
    {
        case 0: // Check if all the servos stopped
        {
            if(std::all_of(m_core->m_servos.begin(), m_core->m_servos.end(), [](const std::pair<std::string, MinorServo::SRTBaseMinorServo_ptr>& servo) -> bool
            {
                ACSErr::Completion_var comp;
                return servo.second->operative_mode()->get_sync(comp.out()) == MinorServo::OPERATIVE_MODE_STOP ? true : false;
            }))
            {
                // Move to phase 1
                m_status = 1;
            }

            break;
        }
        case 1: // Send the SETUP command
        {
            if(std::get<std::string>(m_core->m_socket->sendCommand(SRTMinorServoCommandLibrary::setup(m_LDO_configuration))["OUTPUT"]) != "GOOD")
            {
                m_core->m_starting = Management::MNG_FALSE;
                m_core->m_motion_status = MinorServo::MOTION_STATUS_ERROR;
                m_core->m_subsystem_status = Management::MNG_FAILURE;
                this->setStopped();
            }
            else
            {
                m_status = 2;
            }

            break;
        }
        case 2: // Wait for the system to show the commanded configuration
        {
            if(m_core->m_component->current_configuration()->get_sync(comp.out()) == m_core->m_commanded_configuration)
            {
                m_status = 3;
            }

            break;
        }
        case 3: // Wait for the whole system to reach the desired configuration
        {
            // First we check the status of the gregorian cover
            bool completed = m_core->m_component->gregorian_cover()->get_sync(comp.out()) == m_gregorian_cover_position ? true : false;

            // Then we cycle through all the servos and make sure their operative mode is SETUP
            if(completed && std::all_of(m_core->m_servos.begin(), m_core->m_servos.end(), [](const std::pair<std::string, MinorServo::SRTBaseMinorServo_ptr>& servo) -> bool
            {
                ACSErr::Completion_var comp;
                return servo.second->operative_mode()->get_sync(comp.out()) == MinorServo::OPERATIVE_MODE_SETUP ? true : false;
            }))
            {
                m_status = 4;
            }

            break;
        }
        case 4: // Finally load the servos coefficients
        {
            m_core->m_actual_setup = m_core->m_commanded_setup;

            for(const auto& [name, servo] : m_core->m_servos)
            {
                servo->setup(m_core->m_actual_setup.c_str());
            }

            m_core->m_starting = Management::MNG_FALSE;
            m_core->m_ready = Management::MNG_TRUE;
            m_core->m_subsystem_status = Management::MNG_OK;
            m_core->m_motion_status = m_core->m_elevation_tracking_enabled == Management::MNG_TRUE ? MinorServo::MOTION_STATUS_TRACKING : MinorServo::MOTION_STATUS_CONFIGURED;
            this->setStopped();
            break;
        }
    }
}
