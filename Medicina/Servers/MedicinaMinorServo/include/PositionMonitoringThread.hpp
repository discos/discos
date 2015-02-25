#ifndef MEDMINORSERVOREADTHREAD_HPP
#define MEDMINORSERVOREADTHREAD_HPP

#include <acsThread.h>

#include <IRA>
#include "MedMinorServoControl.hpp"

#define POSITION_SLEEP_TIME 1000000 // 100 mseconds

class PositionMonitoringThread : public ACS::Thread
{
    public:
        PositionMonitoringThread(const ACE_CString& name, 
                                 MedMinorServoControl_sp control,
                                 const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
                                 const ACS::TimeInterval& sleepTime=POSITION_SLEEP_TIME,
                                 const bool del=true);
        virtual ~PositionMonitoringThread();
        virtual void onStart();
        virtual void onStop();
        virtual void runLoop();
    private:
        MedMinorServoControl_sp m_control;
};

#endif

