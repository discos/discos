/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/
#ifndef __MSBOSS_PUBLISHER_H__
#define __MSBOSS_PUBLISHER_H__

#include <acsThread.h>
#include <LogFilter.h>
#include <IRA>
#include <MinorServoS.h>
#include "MSParameters.h"
#include "MSBossConfiguration.h"
#include "SetupThread.h"
#include <string>
#include <map>
#include "utils.h"

// Sleep Time in 100ns
#define PUBLISHER_SLEEP_TIME 1000000 // 100 mseconds
#define VSTATUS_LENGTH 7
#define VS_OK 0
#define VS_TRACKING 1
#define VS_CONFIGURING 2
#define VS_PARKING 3
#define VS_PARKED 4
#define VS_WARNING 5
#define VS_FAILURE 6

// This class updates the status property of minor servos.
class MSBossPublisher : public ACS::Thread
{
public:

	MSBossPublisher(
        const ACE_CString& name, 
        MSBossConfiguration * configuration,
        const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
        const ACS::TimeInterval& sleepTime=PUBLISHER_SLEEP_TIME,
        const bool del=true
    );

    ~MSBossPublisher();

    /// This method is executed once when the thread starts.
    virtual void onStart();

    /// This method is executed once when the thread stops.
    virtual void onStop();

    /// This method overrides the thread implementation class.
    virtual void runLoop();

private:
    MSBossConfiguration * m_configuration;
    TIMEVALUE m_last_event;
	
	/** The status of the whole minor servo subsystem */
	Management::TSystemStatus m_status;
     
};

#endif 

