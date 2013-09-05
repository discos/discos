#ifndef COLLECTORTHREAD_H_
#define COLLECTORTHREAD_H_

/* **************************************************************************/
/* OAC - Osservatorio Astronomico di Cagliari                               */
/* $Id: CollectorThread.h,v 1.2 2011-01-18 10:48:00 c.migoni Exp $      */
/*                                                                          */
/* This code is under GNU General Public Licence (GPL).                     */
/*                                                                          */
/* Who                                when            What                  */
/* Carlo Migoni (migoni@ca.astro.it)  23/11/2010      Creation              */

#include <acsThread.h>
#include <IRA>
#include <DataCollection.h>

namespace CalibrationTool_private {

/**
 * This class implements a working thread. This thread is in charge of collecting the "slow" data from all other
 * component in the system.
 */
class CCollectorThread : public ACS::Thread
{
public:
	/**
     * Constructor().
     * @param name thread name
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
    */
	CCollectorThread(const ACE_CString& name,CSecureArea<CDataCollection> *param, 
			const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
			const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

	/**
	 * Destructor.
    */
    ~CCollectorThread();

     /**
     * This method is executed once when the thread starts.
    */
    virtual void onStart();

    /**
     * This method is executed once when the thread stops.
     */
     virtual void onStop();

     /**
      * This method overrides the thread implementation class. The method is executed in a loop until the thread is alive.
      * The thread can be exited by calling ACS::ThreadBase::stop or ACS::ThreadBase::exit command.
     */
     virtual void runLoop();
     
     /**
      * This allows to specify the duty cycle for tracking flag requests
      * @param duty meteo request cycle duty time in milliseconds
      */
     void setTrackingFlagDuty(const DDWORD& duty) { m_trackingDuty=duty*10; }     
     
     void setConfiguration(CConfiguration * const conf) { m_config=conf; }
     
     void setServices(maci::ContainerServices * const service) { m_services=service; }
          
private:
	CSecureArea<CDataCollection> *m_dataWrapper;
	CConfiguration *m_config;
	maci::ContainerServices * m_services;
	DDWORD m_trackingDuty; // duty cycle of the tracking request in 100 ns
	ACS::Time m_trackingTime; // last request to the scheduler component
	bool m_schedulerError;
	Management::Scheduler_var m_scheduler;
	Management::ROTBoolean_var m_schedulerTracking; // tracking property of the scheduler
	/**
	 * It checks if the component is ready to receive data from the backend....so it is time to collect auxiliary data.
	 */
	bool checkReadyFlag();
};

};

#endif /*COLLECTORTHREAD_H_*/
