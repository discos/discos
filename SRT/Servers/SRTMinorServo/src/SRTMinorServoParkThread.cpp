#include "SRTMinorServoParkThread.h"

using namespace MinorServo;

SRTMinorServoParkThread::SRTMinorServoParkThread(const ACE_CString& name, SRTMinorServoBossCore& core, const ACS::TimeInterval& response_time, const ACS::TimeInterval& sleep_time) :
    ACS::Thread(name, response_time, sleep_time),
    m_core(core)
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

    m_status = 0;

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoParkThread::onStart()", (LM_NOTICE, "PARK THREAD STARTED"));
}

void SRTMinorServoParkThread::onStop()
{
    AUTO_TRACE("SRTMinorServoParkThread::onStop()");

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoParkThread::onStop()", (LM_NOTICE, "PARK THREAD STOPPED"));
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
        this->setStopped();
        return;
    }

    if(IRA::CIRATools::getUNIXEpoch() - m_start_time >= PARK_TIMEOUT)
    {
        ACS_LOG(LM_FULL_INFO, "SRTMinorServoParkThread::runLoop()", (LM_CRITICAL, "Timeout while performing a park operation."));
        m_core.setError(ERROR_CONFIG_ERROR);
        this->setStopped();
        return;
    }

    switch(m_status)
    {
        case 0:
        {
            // First we check if the gregorian cover has closed
            // TEMPORARY: we skip the gregorian cover closing, therefore we don't check
            //bool completed = m_core.m_status.getGregorianCoverPosition() == COVER_STATUS_CLOSED;
            bool completed = true;

            // Then we cycle through all the servos and make sure their operative mode is STOP
            if(completed && std::all_of(m_core.m_servos.begin(), m_core.m_servos.end(), [](const std::pair<std::string, SRTBaseMinorServo_ptr>& servo) -> bool
            {
                ACSErr::Completion_var comp;
                return servo.second->operative_mode()->get_sync(comp.out()) == OPERATIVE_MODE_STOP;
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
