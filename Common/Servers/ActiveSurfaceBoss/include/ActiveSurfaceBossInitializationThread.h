#ifndef _ACTIVESURFACEBOSSINITIALIZATIONTHREAD_H_
#define _ACTIVESURFACEBOSSINITIALIZATIONTHREAD_H_

/********************************************************************************************/
/* OAC Osservatorio Astronomico di Cagliari                                                 */
/* $Id: ActiveSurfaceBossInitializationThread.h,v 1.1 2010-07-26 12:36:49 c.migoni Exp $    */
/*                                                                                          */
/* This code is under GNU General Public Licence (GPL).                                     */
/*                                                                                          */
/* Who                                            When        What                          */
/* Giuseppe Carboni (giuseppe.carboni@inaf.it)    09/01/2025    Creation                    */
/********************************************************************************************/

#include <acsThread.h>
#include <IRA>
#include "ActiveSurfaceBossCore.h"
#include "ActiveSurfaceBossSectorThread.h"
#include <ComponentErrors.h>

#define SECTORTIME 1000000 // 0.1 seconds

/**
 * This class implements an initialization thread. This thread is in charge of setting up the active surface
*/
class CActiveSurfaceBossInitializationThread : public ACS::Thread
{
public:
    /**
     * Constructor().
     * @param name thread name
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
    */
    CActiveSurfaceBossInitializationThread(const ACE_CString& name, CActiveSurfaceBossCore *param,
            const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

    /**
     * Destructor.
    */
    ~CActiveSurfaceBossInitializationThread();

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
    CActiveSurfaceBossCore *m_boss;
    std::string m_thread_name;
    ACS::Time timestart;
	std::vector<CActiveSurfaceBossSectorThread*> m_sectorThread;
};

#endif /*_ACTIVESURFACEBOSSINITIALIZATIONTHREAD_H_*/
