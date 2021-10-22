#ifndef _MEDICINAACTIVESURFACEBOSSWORKINGTHREAD_H_
#define _MEDICINAACTIVESURFACEBOSSWORKINGTHREAD_H_

/* ********************************************************************************* */
/* OAC Osservatorio Astronomico di Cagliari                                          */
/* $Id: MedicinaActiveSurfaceBossWorkingThread.h,v 1.1 2010-07-26 12:36:49 c.migoni Exp $ */
/*                                                                                   */
/* This code is under GNU General Public Licence (GPL).                              */
/*                                                                                   */
/* Who                                  when        What                             */
/* Carlo Migoni (carlo.migoni@inaf.it)  24/02/2021  Creation                          */

#include <acsThread.h>
#include <IRA>
#include "MedicinaActiveSurfaceBossCore.h"
#include <ComponentErrors.h>

/**
 * This class implements a working thread. This thread is in charge of updating the active surface
*/
class CMedicinaActiveSurfaceBossWorkingThread : public ACS::Thread
{
public:
	/**
     * Constructor().
     * @param name thread name
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
    */
	CMedicinaActiveSurfaceBossWorkingThread(const ACE_CString& name,IRA::CSecureArea<CMedicinaActiveSurfaceBossCore>  *param, 
			const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

	/**
	 * Destructor.
    */
    ~CMedicinaActiveSurfaceBossWorkingThread();

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

#endif /*_MedicinaACTIVESURFACEBOSSWORKINGTHREAD_H_*/
