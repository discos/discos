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
}

void SRTMinorServoSetupThread::onStop()
{
    AUTO_TRACE(m_thread_name + "::onStop()");
}

void SRTMinorServoSetupThread::run()
{
    AUTO_TRACE(m_thread_name + "::run()");
    double start_time = CIRATools::getUNIXEpoch();
    std::string configuration_name = m_core->m_focal_configurations_table.right.at(m_core->m_requested_configuration);
    MinorServo::SRTMinorServoGregorianCoverStatus gregorian_cover_position = m_core->m_requested_configuration == MinorServo::CONFIGURATION_PRIMARY ? MinorServo::COVER_STATUS_CLOSED : MinorServo::COVER_STATUS_OPEN;
    ACSErr::Completion_var comp;

    // Check if all the servos stopped
    while(!std::all_of(m_core->m_servos.begin(), m_core->m_servos.end(), [](const MinorServo::SRTBaseMinorServo_ptr& servo) -> bool
    {
        ACSErr::Completion_var comp;
        return servo->operative_mode()->get_sync(comp.out()) == MinorServo::OPERATIVE_MODE_STOP ? true : false;
    }))
    {
        if(CIRATools::getUNIXEpoch() - start_time >= SETUP_TIMEOUT)
        {
            m_core->m_boss_status = MinorServo::BOSS_STATUS_ERROR;
            m_core->m_current_configuration = MinorServo::CONFIGURATION_UNKNOWN;
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // Send the SETUP command
    if(std::get<std::string>(m_core->m_socket->sendCommand(SRTMinorServoCommandLibrary::setup(configuration_name))["OUTPUT"]) != "GOOD")
    {
        _THROW_EXCPT(MinorServoErrors::SetupErrorExImpl, "SRTMinorServoBossCore::setup()");
    }

    // Wait for the system to show the commanded configuration
    while(m_core->m_component->current_configuration()->get_sync(comp.out()) != m_core->m_current_configuration)
    {
        if(CIRATools::getUNIXEpoch() - start_time >= SETUP_TIMEOUT)
        {
            m_core->m_boss_status = MinorServo::BOSS_STATUS_ERROR;
            m_core->m_current_configuration = MinorServo::CONFIGURATION_UNKNOWN;
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // Wait for the whole system to reach the desired configuration
    while(true)
    {
        // First we check the status of the gregorian cover
        bool completed = m_core->m_component->gregorian_cover()->get_sync(comp.out()) == gregorian_cover_position ? true : false;

        // Then we cycle through all the servos and make sure their operative mode is SETUP
        if(completed && std::all_of(m_core->m_servos.begin(), m_core->m_servos.end(), [](const MinorServo::SRTBaseMinorServo_ptr& servo) -> bool
        {
            ACSErr::Completion_var comp;
            return servo->operative_mode()->get_sync(comp.out()) == MinorServo::OPERATIVE_MODE_SETUP ? true : false;
        }))
        {
            break;
        }

        if(CIRATools::getUNIXEpoch() - start_time >= SETUP_TIMEOUT)
        {
            m_core->m_boss_status = MinorServo::BOSS_STATUS_ERROR;
            m_core->m_current_configuration = MinorServo::CONFIGURATION_UNKNOWN;
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    if(m_core->m_component->current_configuration()->get_sync(comp.out()) != m_core->m_requested_configuration)
    {
        m_core->m_boss_status = MinorServo::BOSS_STATUS_ERROR;
        m_core->m_current_configuration = MinorServo::CONFIGURATION_UNKNOWN;
        return;
    }

    // Finally load the servos coefficients
    for(auto& servo : m_core->m_servos)
    {
        servo->setup(configuration_name.c_str());
    }

    m_core->m_current_configuration = m_requested_configuration;
    m_core->m_boss_status = MinorServo::BOSS_STATUS_CONFIGURED;
}
