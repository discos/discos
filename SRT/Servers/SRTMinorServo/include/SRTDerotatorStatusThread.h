#ifndef _SRTDEROTATORSTATUSTHREAD_H_
#define _SRTDEROTATORSTATUSTHREAD_H_

/**
 * SRTDerotatorStatusThread.h
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "SuppressWarnings.h"
#include <acsThread.h>
#include <ComponentErrors.h>
#include "SRTMinorServoSocket.h"
#include "SRTDerotatorImpl.h"

class SRTDerotatorImpl;


/**
 * This class implements a status thread. This thread is in charge of updating the status of the relative derotator component.
 */
class SRTDerotatorStatusThread : public ACS::Thread
{
public:
    /**
     * Constructor.
     * @param name thread name
     * @param response_time thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleep_time thread's sleep time in 100ns unit. Default value is 100ms.
     */
    SRTDerotatorStatusThread(const ACE_CString& name, SRTDerotatorImpl& component, const ACS::TimeInterval& response_time=ThreadBase::defaultResponseTime, const ACS::TimeInterval& sleep_time=ThreadBase::defaultSleepTime);

    /**
     * Destructor.
     */
    ~SRTDerotatorStatusThread();

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
    SRTDerotatorImpl& m_component;

    /**
     * The sleeping time of the thread.
     * The thread should be cycling at a constant rate, therefore the inner sleeping time is always updated taking into account this and the thread execution time.
     */
    ACS::TimeInterval m_sleep_time;
};

#endif /*_SRTDEROTATORSTATUSTHREAD_H_*/
