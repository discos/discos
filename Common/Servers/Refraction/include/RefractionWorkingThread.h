#ifndef _REFRACTIONWORKINGTHREAD_H_
#define _REFRACTIONWORKINGTHREAD_H_

/* ************************************************************************ */
/* OAC Osservatorio Astronomico di Cagliari                                 */
/* $Id: RefractionWorkingThread.h,v 1.1 2009-01-29 09:34:47 c.migoni Exp $  */
/*                                                                          */
/* This code is under GNU General Public Licence (GPL).                     */
/*                                                                          */
/* Who                                  when        What                    */
/* Carlo Migoni (migoni@ca.astro.it)   26/01/2009  Creation                 */

#include "RefractionCore.h"
#include <acsThread.h>
#include <IRA>

/**
 * This class implements a working thread. This thread is in charge of loading the meteo parameters from the metrology component every one second
*/
class CRefractionWorkingThread : public ACS::Thread
{
public:
	/**
     * Constructor().
     * @param name thread name
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
    */
	//CRefractionWorkingThread(const ACE_CString& name,IRA::CSecureArea<CRefractionCore>  *param, 
	//		const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);
	CRefractionWorkingThread(const ACE_CString& name,CRefractionCore *param, 
			const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

	/**
	 * Destructor.
    */
    ~CRefractionWorkingThread();

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

private:

	IRA::CSecureArea<CRefractionCore> *m_core;
	CRefractionCore *boss;
};

#endif /*REFRACTIONWORKINGTHREAD_H_*/
