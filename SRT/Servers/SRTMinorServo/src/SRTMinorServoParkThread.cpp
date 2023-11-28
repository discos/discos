#include "SRTMinorServoParkThread.h"

SRTMinorServoParkThread::SRTMinorServoParkThread(const ACE_CString& name, SRTMinorServoBossCore* core, const ACS::TimeInterval& responseTime, const ACS::TimeInterval& sleepTime):
    ACS::Thread(name, responseTime, sleepTime),
    m_core(core)
{
    m_thread_name = std::string(name.c_str());
    AUTO_TRACE(m_thread_name + "::SRTMinorServoParkThread()");
}

SRTMinorServoParkThread::~SRTMinorServoParkThread()
{
    AUTO_TRACE(m_thread_name + "::~SRTMinorServoParkThread()");
}

void SRTMinorServoParkThread::onStart()
{
    AUTO_TRACE(m_thread_name + "::onStart()");
}

void SRTMinorServoParkThread::onStop()
{
    AUTO_TRACE(m_thread_name + "::onStop()");
}

void SRTMinorServoParkThread::run()
{
    AUTO_TRACE(m_thread_name + "::run()");
    ACSErr::Completion_var comp;
    double start_time = CIRATools::getUNIXEpoch();

    while(true)
    {
        // First we check if the gregorian cover has closed
        bool completed = m_core->m_component->gregorian_cover()->get_sync(comp.out()) == MinorServo::COVER_STATUS_CLOSED ? true : false;

        // Then we cycle through all the servos and make sure their operative mode is STOP
        if(completed && std::all_of(m_core->m_servos.begin(), m_core->m_servos.end(), [](const MinorServo::SRTBaseMinorServo_ptr& servo) -> bool
        {
            ACSErr::Completion_var comp;
            return servo->operative_mode()->get_sync(comp.out()) == MinorServo::OPERATIVE_MODE_STOP ? true : false;
        }))
        {
            break;
        }

        if(CIRATools::getUNIXEpoch() - start_time >= PARK_TIMEOUT)
        {
            m_core->m_boss_status = MinorServo::BOSS_STATUS_ERROR;
            m_core->m_current_configuration = MinorServo::CONFIGURATION_UNKNOWN;
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    m_core->m_current_configuration = MinorServo::CONFIGURATION_PARK;
    m_core->m_boss_status = MinorServo::BOSS_STATUS_CONFIGURED;
}
