#include "PositionMonitoringThread.hpp"

PositionMonitoringThread::PositionMonitoringThread(const ACE_CString& name, 
                         MedMinorServoControl_sp control,
                         const ACS::TimeInterval& responseTime,
                         const ACS::TimeInterval& sleepTime,
                         const bool del
                         ) : ACS::Thread(name, responseTime, sleepTime, del),
                             m_control(control)
{}

PositionMonitoringThread::~PositionMonitoringThread(){}

void
PositionMonitoringThread::onStart(){
    AUTO_TRACE("PositionMonitoringThread::onStart()");
}

void
PositionMonitoringThread::onStop(){
    AUTO_TRACE("PositionMonitoringThread::onStop()");
}

void 
PositionMonitoringThread::runLoop()
{
    try{
        if(m_control)
            m_control->update_position();
        else
            //CUSTOM_LOG(LM_FULL_INFO, "PositionMonitoringThread::runLoop()",
            //  (LM_WARNING, "Position Monitoring Thread cannot access servo control"));
            return;
    }catch(...) {
        CUSTOM_LOG(LM_FULL_INFO, "PositionMonitoringThread::runLoop()",
              (LM_WARNING, "Position Monitoring Thread cannot update position"));
    }
}

