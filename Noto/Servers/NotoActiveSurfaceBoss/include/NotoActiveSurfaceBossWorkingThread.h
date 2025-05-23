#ifndef _NOTOACTIVESURFACEBOSSWORKINGTHREAD_H_
#define _NOTOACTIVESURFACEBOSSWORKINGTHREAD_H_

/* ********************************************************************************* */
/* OAC Osservatorio Astronomico di Cagliari                                          */
/* $Id: NotoActiveSurfaceBossWorkingThread.h,v 1.1 2010-07-26 12:36:49 c.migoni Exp $ */
/*                                                                                   */
/* This code is under GNU General Public Licence (GPL).                              */
/*                                                                                   */
/* Who                                  when        What                             */
/* Carlo Migoni (carlo.migoni@inaf.it)  24/02/2021  Creation                          */

#include <acsThread.h>
#include <IRA>
#include "NotoActiveSurfaceBossCore.h"
#include <ComponentErrors.h>

/**
 * This class implements a working thread. This thread is in charge of updating the active surface
*/
class CNotoActiveSurfaceBossWorkingThread : public ACS::Thread
{
public:
	/**
     * Constructor().
     * @param name thread name
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
    */
	CNotoActiveSurfaceBossWorkingThread(const ACE_CString& name,IRA::CSecureArea<CNotoActiveSurfaceBossCore>  *param, 
			const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

	/**
	 * Destructor.
    */
    ~CNotoActiveSurfaceBossWorkingThread();

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

	ACS::TimeInterval m_sleepTime;
	IRA::CSecureArea<CNotoActiveSurfaceBossCore> *m_core;
};

#endif /*_NotoACTIVESURFACEBOSSWORKINGTHREAD_H_*/
