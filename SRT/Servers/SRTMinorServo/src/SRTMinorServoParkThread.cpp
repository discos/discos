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
    this->setSleepTime(500000);   // 50 milliseconds
    m_start_time = CIRATools::getUNIXEpoch();

    m_status = 0;
}

void SRTMinorServoParkThread::onStop()
{
    AUTO_TRACE(m_thread_name + "::onStop()");
}

void SRTMinorServoParkThread::runLoop()
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

    if(CIRATools::getUNIXEpoch() - m_start_time >= PARK_TIMEOUT)
    {
        m_core->m_starting = Management::MNG_FALSE;
        m_core->m_motion_status = MinorServo::MOTION_STATUS_ERROR;
        m_core->m_subsystem_status = Management::MNG_FAILURE;
        ACS_LOG(LM_FULL_INFO, m_thread_name + "::runLoop()", (LM_ERROR, "Timeout while performing a park operation."));
        this->setStopped();
    }

    ACSErr::Completion_var comp;

    switch(m_status)
    {
        case 0:
        {
            // First we check if the gregorian cover has closed
            bool completed = m_core->m_component->gregorian_cover()->get_sync(comp.out()) == MinorServo::COVER_STATUS_CLOSED ? true : false;

            // Then we cycle through all the servos and make sure their operative mode is STOP
            if(completed && std::all_of(m_core->m_servos.begin(), m_core->m_servos.end(), [](const std::pair<std::string, MinorServo::SRTBaseMinorServo_ptr>& servo) -> bool
            {
                ACSErr::Completion_var comp;
                return servo.second->operative_mode()->get_sync(comp.out()) == MinorServo::OPERATIVE_MODE_STOP ? true : false;
            }))
            {
                m_status = 1;
            }

            break;
        }
        case 1:
        {
            for(const auto& [name, servo] : m_core->m_servos)
            {
                servo->setup("");
            }

            m_core->m_actual_setup = m_core->m_commanded_setup;
            m_core->m_starting = Management::MNG_FALSE;

            m_core->m_subsystem_status = Management::MNG_OK;
            m_core->m_motion_status = MinorServo::MOTION_STATUS_PARK;
            this->setStopped();
            break;
        }
    }
}
