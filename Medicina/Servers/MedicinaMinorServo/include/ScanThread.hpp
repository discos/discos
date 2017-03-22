#ifndef SCANTHREAD_HPP
#define SCANTHREAD_HPP

#include <acsThread.h>

#include <IRA>
#include "MedMinorServoControl.hpp"
#include "MedMinorServoStatus.hpp"
#include "MedMinorServoScan.hpp"

#define SCAN_MAX_WAIT 6000000000 // 10 minutes
#define SCAN_DELTA 100000 //10ms.

class ScanThreadParameters
{
    public:
        ScanThreadParameters(MedMinorServoControl_sp,
                              MedMinorServoStatus*,
                              MedMinorServoScan&);
        virtual ~ScanThreadParameters();
        MedMinorServoControl_sp m_control;
        MedMinorServoStatus *m_status;
        MedMinorServoScan m_scan;
};

class ScanThread : public ACS::Thread
{
    public:
        ScanThread(const ACE_CString& name, 
                    ScanThreadParameters& params);
        virtual ~ScanThread();
        virtual void onStart();
        virtual void onStop();
        virtual void run();
    private:
        MedMinorServoControl_sp m_control;
        MedMinorServoStatus* m_status;
        MedMinorServoScan m_scan;
};

#endif

