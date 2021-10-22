#ifndef _MEDICINAACTIVESURFACEBOSSSECTORTHREAD_H_
#define _MEDICINAACTIVESURFACEBOSSSECTORTHREAD_H_

/* ********************************************************************************* */
/* OAC Osservatorio Astronomico di Cagliari                                          */
/* $Id: MedicinaActiveSurfaceBossSectorThread.h,v 1.1 2010-07-26 12:36:49 c.migoni Exp $ */
/*                                                                                   */
/* This code is under GNU General Public Licence (GPL).                              */
/*                                                                                   */
/* Who                                  when        What                     */
/* Carlo Migoni (carlo.migoni@inaf.it)  24/02/2021  Creation                 */

#include <acsThread.h>
#include <IRA>
#include "MedicinaActiveSurfaceBossCore.h"
#include <ComponentErrors.h>

/**
 * This class implements a Sector thread. This thread is in charge of updating the active surface
*/
class CMedicinaActiveSurfaceBossSectorThread : public ACS::Thread
{
public:
    /**
     * Constructor().
     * @param name thread name
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
    */
    CMedicinaActiveSurfaceBossSectorThread(const ACE_CString& name, CMedicinaActiveSurfaceBossCore *param,
            const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

    /**
     * Destructor.
    */
    ~CMedicinaActiveSurfaceBossSectorThread();

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
     virtual void run();

     /**
      * This method is used to set the sector to be initialized. It MUST be called before starting the thread execution
     */
     virtual void setSector(int sector);

private:

    IRA::CSecureArea<CMedicinaActiveSurfaceBossCore> *m_core;
    CMedicinaActiveSurfaceBossCore *boss;
    int m_sector;
};

#endif /*_MedicinaACTIVESURFACEBOSSSECTORTHREAD_H_*/
