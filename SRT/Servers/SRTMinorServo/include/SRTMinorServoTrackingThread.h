#ifndef _SRTMINORSERVOTRACKINGTHREAD_H_
#define _SRTMINORSERVOTRACKINGTHREAD_H_

/*************************************************************************************/
/* OAC Osservatorio Astronomico di Cagliari                                          */
/* $Id: SRTMinorServoTrackingThread.h                                                */
/*                                                                                   */
/* This code is under GNU General Public Licence (GPL).                              */
/*                                                                                   */
/* Who                                            When        What                   */
/* Giuseppe Carboni (giuseppe.carboni@inaf.it)    30/12/2023    Creation             */
/*************************************************************************************/

#include "Common.h"
#include <IRA>
#include <acsThread.h>
#include <ComponentErrors.h>
#include "SRTMinorServoBossCore.h"
#include <AntennaBossC.h>
#include <LogFilter.h>
#include <slamac.h>

_IRA_LOGFILTER_IMPORT;


#define TRACKING_FUTURE_TIME    2   //2 seconds
#define TRACKING_TIMEGAP        0.2 //200 milliseconds

class SRTMinorServoBossCore;


/**
 * This class implements a tracking thread. This thread is in charge of positioning the minor servos in time.
*/
class SRTMinorServoTrackingThread : public ACS::Thread
{
public:
    /**
     * Constructor().
     * @param name thread name
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
    */
    SRTMinorServoTrackingThread(const ACE_CString& name, SRTMinorServoBossCore* core, const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime, const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

    /**
     * Destructor.
    */
    ~SRTMinorServoTrackingThread();

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
    double getElevation(double time);
    void resetTracking();

    std::string m_thread_name;
    SRTMinorServoBossCore* m_core;
    Antenna::AntennaBoss_var m_antennaBoss;
    unsigned int m_point_id, m_trajectory_id;
    double m_point_time;
};

#endif /*_SRTMINORSERVOTRACKINGTHREAD_H_*/
