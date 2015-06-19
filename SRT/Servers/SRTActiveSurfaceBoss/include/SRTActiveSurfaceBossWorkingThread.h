#ifndef _SRTACTIVESURFACEBOSSWORKINGTHREAD_H_
#define _SRTACTIVESURFACEBOSSWORKINGTHREAD_H_

/* ********************************************************************************* */
/* OAC Osservatorio Astronomico di Cagliari                                          */
/* $Id: SRTActiveSurfaceBossWorkingThread.h,v 1.1 2010-07-26 12:36:49 c.migoni Exp $ */
/*                                                                                   */
/* This code is under GNU General Public Licence (GPL).                              */
/*                                                                                   */
/* Who                                  when        What                             */
/* Carlo Migoni (migoni@ca.astro.it)   09/07/2010  Creation                          */

#include <acsThread.h>
#include <IRA>
#include "SRTActiveSurfaceBossCore.h"
#include <ComponentErrors.h>

/**
 * This class implements a working thread. This thread is in charge of updating the active surface
*/
class CSRTActiveSurfaceBossWorkingThread : public ACS::Thread
{
public:
	/**
     * Constructor().
     * @param name thread name
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
    */
	CSRTActiveSurfaceBossWorkingThread(const ACE_CString& name,IRA::CSecureArea<CSRTActiveSurfaceBossCore>  *param, 
			const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

	/**
	 * Destructor.
    */
    ~CSRTActiveSurfaceBossWorkingThread();

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

	IRA::CSecureArea<CSRTActiveSurfaceBossCore> *m_core;
};

#endif /*_SRTACTIVESURFACEBOSSWORKINGTHREAD_H_*/
