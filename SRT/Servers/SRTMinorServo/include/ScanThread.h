/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/
#ifndef __SCAN_THREAD_H__
#define __SCAN_THREAD_H__

#include <acsThread.h>
#include <LogFilter.h>
#include <IRA>
#include <MinorServoS.h>
#include "MSParameters.h"
#include "SetupThread.h"
#include <string>
#include <map>
#include "utils.h"

// The Delta Time is the time between two axis positioning (Time in 100ns)
const ACS::Time SCAN_DELTA_TIME = 1000000; // 100ms, 
// Sleep Time in 100ns
const unsigned int SCAN_SLEEP_TIME = static_cast<unsigned int>(0.9 * SCAN_DELTA_TIME);
const double GUARD_COEFF = 0.2;

// This class updates the status property of minor servos.
class ScanThread : public ACS::Thread
{
public:

	ScanThread(
        const ACE_CString& name, 
        MSThreadParameters& params,
        const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
        const ACS::TimeInterval& sleepTime=SCAN_SLEEP_TIME,
        const bool del=true
    );

    ~ScanThread();

    /// This method is executed once when the thread starts.
    virtual void onStart();

    /// This method is executed once when the thread stops.
    virtual void onStop();

    /// This method overrides the thread implementation class.
    virtual void runLoop();

private:
    MSThreadParameters *m_params;
     
};

#endif 

