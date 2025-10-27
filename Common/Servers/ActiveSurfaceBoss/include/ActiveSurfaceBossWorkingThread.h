#ifndef _ACTIVESURFACEBOSSWORKINGTHREAD_H_
#define _ACTIVESURFACEBOSSWORKINGTHREAD_H_

/* ********************************************************************************* */
/* OAC Osservatorio Astronomico di Cagliari                                          */
/* $Id: ActiveSurfaceBossWorkingThread.h,v 1.1 2010-07-26 12:36:49 c.migoni Exp $    */
/*                                                                                   */
/* This code is under GNU General Public Licence (GPL).                              */
/*                                                                                   */
/* Who                                   when        What                            */
/* Carlo Migoni (migoni@ca.astro.it)     09/07/2010  Creation                        */
/* G. Carboni (giuseppe.carboni@inaf.it) 10/10/2025  Added ZMQ publishing            */

#include <acsThread.h>
#include <IRA>
#include "ActiveSurfaceBossCore.h"
#include <ComponentErrors.h>

/**
 * This class implements a working thread. This thread is in charge of updating the active surface
 */
class CActiveSurfaceBossWorkingThread : public ACS::Thread
{
public:
    /**
     * Constructor().
     * @param name thread name
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
     */
    CActiveSurfaceBossWorkingThread(const ACE_CString& name, IRA::CSecureArea<CActiveSurfaceBossCore> *core, const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

    /**
     * Destructor.
     */
    ~CActiveSurfaceBossWorkingThread();

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
    ACS::Time m_nextTime;
    IRA::CSecureArea<CActiveSurfaceBossCore> *m_core;
    unsigned int m_status;
};

#endif /*_ACTIVESURFACEBOSSWORKINGTHREAD_H_*/
