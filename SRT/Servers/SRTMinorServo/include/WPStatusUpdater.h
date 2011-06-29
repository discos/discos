/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/
#ifndef __WPSTATUS_UPDATER_H__
#define __WPSTATUS_UPDATER_H__

#include <acsThread.h>
#include <LogFilter.h>
#include <IRA>
#include "MSParameters.h"
#include "RequestScheduler.h"
 
const ACS::Time MAX_TIME_DIFF = 1000000; // One second

// This class updates the status property of minor servos.
class WPStatusUpdater : public ACS::Thread
{
public:

	WPStatusUpdater(
            const ACE_CString& name, 
            ThreadParameters& params,
            const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
            const ACS::TimeInterval& sleepTime=status_sleep_time
    );

    ~WPStatusUpdater();

    /// This method is executed once when the thread starts.
    virtual void onStart();

     /// This method is executed once when the thread stops.
     virtual void onStop();

     /**
     * This method overrides the thread implementation class. The method is executed in a 
     * loop until the thread is alive. The thread can be exited by calling 
     * ACS::ThreadBase::stop or ACS::ThreadBase::exit command.
     */
     virtual void runLoop();

private:
     ThreadParameters *m_params;

     unsigned long m_counter;

};

#endif 

