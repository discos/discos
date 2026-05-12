#ifndef _SRTMINORSERVOSTATUSTHREAD_H_
#define _SRTMINORSERVOSTATUSTHREAD_H_

/**
 * SRTMinorServoStatusThread.h
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "SuppressWarnings.h"
#include <acsThread.h>
#include <ComponentErrors.h>
#include "SRTMinorServoSocket.h"
#include "SRTMinorServoBossCore.h"

class SRTMinorServoBossCore;


/**
 * This class implements a status thread. This thread is in charge of updating the status of the minor servo components.
 */
class SRTMinorServoStatusThread : public ACS::Thread
{
public:
    /**
     * Constructor.
     * @param name thread name
     * @param response_time thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleep_time thread's sleep time in 100ns unit. Default value is 100ms.
     */
    SRTMinorServoStatusThread(const ACE_CString& name, SRTMinorServoBossCore& core, const ACS::TimeInterval& response_time=ThreadBase::defaultResponseTime, const ACS::TimeInterval& sleep_time=ThreadBase::defaultSleepTime);

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

    /**
     * The name of this class of threads. Since a single instance of this thread class can only run it is ok to reuse the same name.
     */
    static constexpr const char* c_thread_name = "SRTMinorServoStatusThread";

private:
    /**
     * The reference to the SRTMinorServoBossCore object.
     */
    SRTMinorServoBossCore& m_core;

    /**
     * The sleeping time of the thread.
     * The thread should be cycling at a constant rate, therefore the inner sleeping time is always updated taking into account this and the thread execution time.
     */
    const ACS::TimeInterval m_sleep_time;

    /**
     * Variable containing the time of the next planned thread iteration.
     */
    ACS::Time m_next_time;
};

#endif /*_SRTMINORSERVOSTATUSTHREAD_H_*/
