#ifndef _SRTMINORSERVOPARKTHREAD_H_
#define _SRTMINORSERVOPARKTHREAD_H_

/**
 * SRTMinorServoParkThread.h
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "SRTMinorServoCommon.h"
#include <acsThread.h>
#include <ComponentErrors.h>
#include "SRTMinorServoBossCore.h"

// TODO: fine-tune this with the real hardware. The correct procedure can easily exceed this timeout, it should be increased accordingly.
#define PARK_TIMEOUT 60

class SRTMinorServoBossCore;


/**
 * This class implements a parking thread. This thread is in charge of checking the status of the minor servos parking procedure.
 */
class SRTMinorServoParkThread : public ACS::Thread
{
public:
    /**
     * Constructor.
     * @param name thread name
     * @param core a reference to the SRTMinorServoBossCore object.
     * @param response_time thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleep_time thread's sleep time in 100ns unit. Default value is 100ms.
    */
    SRTMinorServoParkThread(const ACE_CString& name, SRTMinorServoBossCore& core, const ACS::TimeInterval& response_time=ThreadBase::defaultResponseTime, const ACS::TimeInterval& sleep_time=ThreadBase::defaultSleepTime);

    /**
     * Destructor.
     */
    ~SRTMinorServoParkThread();

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
    static constexpr const char* c_thread_name = "SRTMinorServoParkThread";

private:
    /**
     * The reference to the SRTMinorServoBossCore object.
     */
    SRTMinorServoBossCore& m_core;

    /**
     * The current status of the finite-state machine.
     */
    unsigned int m_status;

    /**
     * The starting time of the park procedure. Used to check if we ran out of time.
     */
    double m_start_time;
};

#endif /*_SRTMINORSERVOSETUPTHREAD_H_*/
