/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu, Marco Bartolini
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/
#ifndef __MSBOSS_PUBLISHER_H__
#define __MSBOSS_PUBLISHER_H__

#include <acsThread.h>
#include <acsncSimpleSupplier.h>
#include <IRA>
#include <MinorServoS.h>
#include <MinorServoBossS.h>
#include <string>
#include <ManagmentDefinitionsS.h>

#include "MedMinorServoStatus.hpp"
#include "MedMinorServoControl.hpp"

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

class PublisherThreadParameters
{
    public:
        PublisherThreadParameters(MedMinorServoStatus*, nc::SimpleSupplier*,
                                  MedMinorServoControl_sp);
        virtual ~PublisherThreadParameters();
        MedMinorServoStatus *m_status;
        nc::SimpleSupplier *m_nc;
        MedMinorServoControl_sp m_control;
};

// This class updates the status property of minor servos.
class MSBossPublisher : public ACS::Thread
{
public:

	MSBossPublisher(
        const ACE_CString& name, 
        PublisherThreadParameters& params,
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
    MedMinorServoStatus *m_status, m_old_status;
    nc::SimpleSupplier *m_nc;
    MedMinorServoControl_sp m_control;
    bool m_old_tracking;
    TIMEVALUE m_last_event;
	
	/** The status of the whole minor servo subsystem */
	//Management::TSystemStatus m_status;
     
};

#endif 

