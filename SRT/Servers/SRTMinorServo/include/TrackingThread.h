/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/
#ifndef __TRACKING_THREAD_H__
#define __TRACKING_THREAD_H__

#include <acsThread.h>
#include <LogFilter.h>
#include <IRA>
#include <MinorServoS.h>
#include "MSParameters.h"
#include "SetupThread.h"
#include <string>
#include <map>
#include "utils.h"

// Sleep Time in 100ns
#define TT_SLEEP_TIME 50000000

// This class updates the status property of minor servos.
class TrackingThread : public ACS::Thread
{
public:

	TrackingThread(
        const ACE_CString& name, 
        MSThreadParameters& params,
        const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
        const ACS::TimeInterval& sleepTime=TT_SLEEP_TIME,
        const bool del=true
    );

    ~TrackingThread();

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

