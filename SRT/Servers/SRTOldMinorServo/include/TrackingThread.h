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
#include <MSBossConfiguration.h>
#include <string>
#include <map>
#include "utils.h"
#include <MountC.h>


// Sleep Time in 50ms (N * 10 = N us)
#define TT_SLEEP_TIME 50000 * 10 // 50 ms
#define ELEVATION_FUTURE_TIME 1700000 // 150 ms

// This class updates the minor servos positions depending of the antenna elevation
class TrackingThread : public ACS::Thread
{
public:

	TrackingThread(
        const ACE_CString& name, 
        MSBossConfiguration * configuration,
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
    MSBossConfiguration *m_configuration;
    Antenna::Mount_var m_antennaMount;
    bool m_ready_error;
    bool m_failure;
     
};

#endif 

