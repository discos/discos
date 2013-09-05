/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/
#ifndef __PARK_THREAD_H__
#define __PARK_THREAD_H__

#include <acsThread.h>
#include <acsncSimpleSupplier.h>
#include <LogFilter.h>
#include <IRA>
#include <MinorServoS.h>
#include "MSParameters.h"
#include <string>
#include <map>
#include <pthread.h>
#include "utils.h"
#include "MSBossConfiguration.h"

using namespace maci;

// Time in 100ns: 10000000 == 1sec
const unsigned int PARK_ITER_SLEEP_TIME = 30000000; // Iteration sleep time: 3 seconds
const unsigned int PARK_INCR_SLEEP_TIME = 20000000; // Increment sleep time: 2 seconds
const unsigned int PARK_MAX_ACTION_TIME = 450000000; // Maximum action (stop, positioning, ecc.) time: 45 seconds
const unsigned int MAX_PARK_TIME = 1800000000; // Maximum park time: 180 seconds


// This class disables the minor servos.
class ParkThread : public ACS::Thread
{
public:

	ParkThread(
        const ACE_CString& name, 
        MSBossConfiguration * configuration,
        const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
        const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime,
        const bool del=false
    );

    ~ParkThread();

    /// This method is executed once when the thread starts.
    virtual void onStart();

    /// This method is executed once when the thread stops.
    virtual void onStop();

    /// This method overrides the thread implementation class. The method is executed only once.
    virtual void run();

private:
    /** Names of positioning components **/
    vector<string> m_positioning;

    MSBossConfiguration * m_configuration;
     
};

#endif 

