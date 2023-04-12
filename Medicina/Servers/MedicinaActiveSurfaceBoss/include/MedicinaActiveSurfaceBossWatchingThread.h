#ifndef _MEDICINAACTIVESURFACEBOSSWATCHINGTHREAD_H_
#define _MEDICINAACTIVESURFACEBOSSWATCHINGTHREAD_H_

/* ********************************************************************************** */
/* OAC Osservatorio Astronomico di Cagliari                                           */
/* $Id: MedicinaActiveSurfaceBossWatchingThread.h,v 1.1 2009-05-21 15:33:19 c.migoni Exp $ */
/*                                                                                    */
/* This code is under GNU General Public Licence (GPL).                               */
/*                                                                                    */
/* Who                                  when        What                              */
/* Carlo Migoni (carlo.migoni@inaf.it)  24/02/2021  Creation                          */

#include <acsThread.h>
#include <IRA>
#include "MedicinaActiveSurfaceBossCore.h"

/**
 * This class implements a watching thread. This thread is in charge of checking the status of the active surface
*/
class CMedicinaActiveSurfaceBossWatchingThread : public ACS::Thread
{
public:
	/**
     * Constructor().
     * @param name thread name
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
    */
	CMedicinaActiveSurfaceBossWatchingThread(const ACE_CString& name,IRA::CSecureArea<CMedicinaActiveSurfaceBossCore>  *param, 
			const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

	/**
	 * Destructor.
    */
    ~CMedicinaActiveSurfaceBossWatchingThread();

     /**
     * This method is executed once when the thread starts.
    */
    virtual void onStart();

    /**
     * This method is executed once when the thread stops.
     */
     virtual void onStop();

     /**
      * This method overrides the thread implementation class.
      * The thread can be exited by calling ACS::ThreadBase::stop or ACS::ThreadBase::exit command.
     */
     virtual void runLoop();

private:

	IRA::CSecureArea<CMedicinaActiveSurfaceBossCore> *m_core;
};

#endif /*MEDICINAACTIVESURFACEBOSSWORKINGTHREAD_H_*/
