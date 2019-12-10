#ifndef _SRTACTIVESURFACEBOSSSECTORTHREAD_H_
#define _SRTACTIVESURFACEBOSSSECTORTHREAD_H_

/*************************************************************************************/
/* OAC Osservatorio Astronomico di Cagliari                                          */
/* $Id: SRTActiveSurfaceBossSector1Thread.h,v 1.1 2010-07-26 12:36:49 c.migoni Exp $ */
/*                                                                                   */
/* This code is under GNU General Public Licence (GPL).                              */
/*                                                                                   */
/* Who                                            When        What                   */
/* Giuseppe Carboni (giuseppe.carboni@inaf.it)    02/07/2019    Creation             */
/*************************************************************************************/

#include <acsThread.h>
#include <IRA>
#include "SRTActiveSurfaceBossCore.h"
#include <ComponentErrors.h>

/**
 * This class implements a Sector thread. This thread is in charge of updating the active surface
*/
class CSRTActiveSurfaceBossSectorThread : public ACS::Thread
{
public:
    /**
     * Constructor().
     * @param name thread name
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
    */
    CSRTActiveSurfaceBossSectorThread(const ACE_CString& name, CSRTActiveSurfaceBossCore *param,
            const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

    /**
     * Destructor.
    */
    ~CSRTActiveSurfaceBossSectorThread();

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
    CSRTActiveSurfaceBossCore *m_boss;
    int m_sector;
    std::string m_thread_name;
    std::ifstream m_usdTable;
    ACS::Time timestart;
};

#endif /*_SRTACTIVESURFACEBOSSSECTORTHREAD_H_*/
