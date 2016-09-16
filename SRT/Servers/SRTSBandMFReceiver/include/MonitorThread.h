#ifndef _MONITORTHREAD_H_
#define _MONITORTHREAD_H_

/*******************************************************************************\
 *  Author Info
 *  ===========
 *  Andrea Orlati <orlati@ira.inaf.it>
\*******************************************************************************/

#include <acsThread.h>
#include <SRTSBandMFCore.h>

/**
 * This class implements a component monitoring thread . This thread is in charge 
 * of update all the attributes of the component in order to make sure the receiver is correctly working.
 */
class CMonitorThread : public ACS::Thread
{

public:

    /**
     * Constructor().
     * @param name thread name
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
     */
    CMonitorThread(
            const ACE_CString& name,
            SRTSBandMFCore  *param,
            const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
            const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime
    );

    ~CMonitorThread();


    /**
     * This method is executed once when the thread starts.
     */
    virtual void onStart();


    /**
     * This method is executed once when the thread stops.
     */
     virtual void onStop();


     /**
      * This method overrides the thread implementation class. The method is executed in a loop until the thread is alive.
      * The thread can be exited by calling ACS::ThreadBase::stop or ACS::ThreadBase::exit command.
      */
     virtual void runLoop();


     /**
      * This method allows to inform the watch dog on how long the LNA sampling time takes.
      */
     void setLNASamplingTime(const DDWORD& time);
     

private:

     enum TStage {
         VACUUM,
         STATUS,
         LNA_VD,
         LNA_ID,
         LNA_VG,
         CTEMPCOOLHEAD,
         CTEMPCOOLHEADW,
         CTEMPLNA,
         CTEMPLNAW,
         REMOTE,
         COOLHEAD,
         VACUUMPUMP,
         VACUUMVALVE,
         NOISEMARK,
         UNLOCKED,
         ENVTEMP
     };


     SRTSBandMFCore *m_core;
     ACS::TimeInterval m_currentResponseTime;
     ACS::TimeInterval m_currentSampling;
     TStage m_currentStage;
};

#endif

