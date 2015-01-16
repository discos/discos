#include "SetupThread.hpp"

SetupThreadParameters::SetupThreadParameters(MedMinorServoControl_sp control,
                                             MedMinorServoStatus* status) : 
                                             m_control(control),
                                             m_status(status)
{}

SetupThreadParameters::~SetupThreadParameters(){}

SetupThread::SetupThread(const ACE_CString& name, 
                         SetupThreadParameters& params
                         ) : ACS::Thread(name),
                             m_control(params.m_control),
                             m_status(params.m_status)
{}

SetupThread::~SetupThread(){}

void
SetupThread::onStart(){
    AUTO_TRACE("SetupThread::onStart()");
}

void
SetupThread::onStop(){
    AUTO_TRACE("SetupThread::onStop()");
}

void 
SetupThread::run()
{
    IRA::CTimer timer;
    bool tracking = false;
    while(!(tracking))
    {
        if(timer.elapsed() > SETUP_MAX_WAIT)
        {
            throw ServoTimeoutError("Timeout wating for tracking flag");
        }
        tracking = m_control->is_tracking();
    }
    m_status->starting = false;
    m_status->ready = true;
    CUSTOM_LOG(LM_FULL_INFO, "SetupThread::run",
               (LM_DEBUG, "Setup complete"));
}

