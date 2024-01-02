#ifndef _SRTMINORSERVOSTATUSTHREAD_H_
#define _SRTMINORSERVOSTATUSTHREAD_H_

/*************************************************************************************/
/* OAC Osservatorio Astronomico di Cagliari                                          */
/* $Id: SRTMinorServoStatusThread.h                                                  */
/*                                                                                   */
/* This code is under GNU General Public Licence (GPL).                              */
/*                                                                                   */
/* Who                                            When        What                   */
/* Giuseppe Carboni (giuseppe.carboni@inaf.it)    14/09/2023    Creation             */
/*************************************************************************************/

#include "Common.h"
#include <IRA>
#include <acsThread.h>
#include <ComponentErrors.h>
#include "SRTMinorServoSocket.h"
#include "SRTMinorServoBossCore.h"

class SRTMinorServoBossCore;


/**
 * This class implements a status thread. This thread is in charge of checking the status of 
*/
class SRTMinorServoStatusThread : public ACS::Thread
{
public:
    /**
     * Constructor().
     * @param name thread name
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
    */
    SRTMinorServoStatusThread(const ACE_CString& name, SRTMinorServoBossCore* core, const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime, const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

    /**
     * Destructor.
    */
    ~SRTMinorServoStatusThread();

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
    std::string m_thread_name;
    SRTMinorServoBossCore* m_core;
    double m_sleep_time;
};

#endif /*_SRTMINORSERVOSTATUSTHREAD_H_*/
