#include "SRTMinorServoParkThread.h"

using namespace MinorServo;

SRTMinorServoParkThread::SRTMinorServoParkThread(const ACE_CString& name, SRTMinorServoBossCore& core, const ACS::TimeInterval& response_time, const ACS::TimeInterval& sleep_time) :
    ACS::Thread(name, response_time, sleep_time),
    m_core(core),
    m_status(0)
{
    AUTO_TRACE("SRTMinorServoParkThread::SRTMinorServoParkThread()");
}

SRTMinorServoParkThread::~SRTMinorServoParkThread()
{
    AUTO_TRACE("SRTMinorServoParkThread::~SRTMinorServoParkThread()");
}

void SRTMinorServoParkThread::onStart()
{
    AUTO_TRACE("SRTMinorServoParkThread::onStart()");
    this->setSleepTime(500000);   // 50 milliseconds
    m_start_time = IRA::CIRATools::getUNIXEpoch();

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoParkThread::onStart()", (LM_INFO, "PARK THREAD STARTED"));
}

void SRTMinorServoParkThread::onStop()
{
    AUTO_TRACE("SRTMinorServoParkThread::onStop()");

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoParkThread::onStop()", (LM_INFO, "PARK THREAD STOPPED"));
}

void SRTMinorServoParkThread::runLoop()
{
    AUTO_TRACE("SRTMinorServoParkThread::runLoop()");

    try
    {
        m_core.checkLineStatus();
    }
    catch(MinorServoErrors::MinorServoErrorsEx& ex)
    {
        ACS_SHORT_LOG((LM_ERROR, ex.errorTrace.routine));
        m_core.setFailure();
        this->setStopped();
        return;
    }

    if(IRA::CIRATools::getUNIXEpoch() - m_start_time >= PARK_TIMEOUT)
    {
        ACS_LOG(LM_FULL_INFO, "SRTMinorServoParkThread::runLoop()", (LM_ERROR, "Timeout while performing a park operation."));
        m_core.setFailure();
        this->setStopped();
        return;
    }

    ACSErr::Completion_var comp;

    switch(m_status)
    {
        case 0:
        {
            // First we check if the gregorian cover has closed
            bool completed = m_core.m_component.gregorian_cover()->get_sync(comp.out()) == COVER_STATUS_CLOSED? true : false;

            // Then we cycle through all the servos and make sure their operative mode is STOP
            if(completed && std::all_of(m_core.m_servos.begin(), m_core.m_servos.end(), [](const std::pair<std::string, SRTBaseMinorServo_ptr>& servo) -> bool
            {
                ACSErr::Completion_var comp;
                return servo.second->operative_mode()->get_sync(comp.out()) == OPERATIVE_MODE_STOP ? true : false;
            }))
            {
                m_status = 1;
            }

            break;
        }
        case 1:
        {
            for(const auto& [name, servo] : m_core.m_servos)
            {
                servo->setup("");
            }

            m_core.m_actual_setup = m_core.m_commanded_setup;
            m_core.m_starting.store(Management::MNG_FALSE);
            m_core.m_motion_status.store(MOTION_STATUS_PARKED);
            m_core.m_subsystem_status.store(Management::MNG_OK);
            this->setStopped();
            break;
        }
    }
}