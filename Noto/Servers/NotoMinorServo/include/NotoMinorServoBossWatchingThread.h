#ifndef _NOTOMINORSERVOBOSSWATCHINGTHREAD_H_
#define _NOTOMINORSERVOBOSSWATCHINGTHREAD_H_

/* ********************************************************************************** */
/* OAC Osservatorio Astronomico di Cagliari                                           */
/* $Id: NotoMinorServoBossWatchingThread.h,v 1.1 2009-05-21 15:33:19 c.migoni Exp $ */
/*                                                                                    */
/* This code is under GNU General Public Licence (GPL).                               */
/*                                                                                    */
/* Who                                  when        What                              */
/* Carlo Migoni (migoni@ca.astro.it)   25/02/2009  Creation                           */

#include <acsThread.h>
#include <IRA>
#include "NotoMinorServoBossCore.h"

/**
 * This class implements a watching thread. This thread is in charge of checking the status of the active surface
*/
class CNotoMinorServoBossWatchingThread : public ACS::Thread
{
public:
	/**
     * Constructor().
     * @param name thread name
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
    */
	CNotoMinorServoBossWatchingThread(const ACE_CString& name,IRA::CSecureArea<CNotoMinorServoBossCore>  *param, 
			const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

	/**
	 * Destructor.
    */
    ~CNotoMinorServoBossWatchingThread();

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

	IRA::CSecureArea<CNotoMinorServoBossCore> *m_core;
};

#endif /*NOTOMinorServoWATCHINGTHREAD_H_*/
