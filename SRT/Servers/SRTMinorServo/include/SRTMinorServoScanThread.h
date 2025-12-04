#ifndef _SRTMINORSERVOSCANTHREAD_H_
#define _SRTMINORSERVOSCANTHREAD_H_

/**
 * SRTMinorServoScanThread.h
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "SuppressWarnings.h"
#include <acsThread.h>
#include <ComponentErrors.h>
#include "SRTMinorServoBossCore.h"
#include "SRTMinorServoContainers.h"


class SRTMinorServoBossCore;


/**
 * This class implements a scan thread. This thread is in charge of positioning the minor servos during a scan operation.
 */
class SRTMinorServoScanThread : public ACS::Thread
{
public:
    /**
     * Constructor.
     * @param name thread name
     * @param response_time thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleep_time thread's sleep time in 100ns unit. Default value is 100ms.
     */
    SRTMinorServoScanThread(const ACE_CString& name, SRTMinorServoBossCore& core, const ACS::TimeInterval& response_time=ThreadBase::defaultResponseTime, const ACS::TimeInterval& sleep_time=ThreadBase::defaultSleepTime);

    /**
     * Destructor.
     */
    ~SRTMinorServoScanThread();

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
    static constexpr const char* c_thread_name = "SRTMinorServoScanThread";

private:
    /**
     * The reference to the SRTMinorServoBossCore object.
     */
    SRTMinorServoBossCore& m_core;

    /**
     * The current trajectory ID.
     */
    unsigned int m_trajectory_id;

    /**
     * The current trajectory point ID.
     */
    unsigned int m_point_id;

    /**
     * The current trajectory point time.
     */
    ACS::Time m_point_time;

    /**
     * Boolean which indicates if the scan thread exited with an error or not.
     */
    bool m_error;

    /**
     * The queue of offsets to be added to the original tracking coordinates.
     */
    MinorServo::SRTMinorServoPositionsQueue m_scan_offsets;

    /**
     * The coordinates to which the minor servo involved in the scan was positioned before starting the scan itself.
     */
    ACS::doubleSeq m_starting_coordinates;
};

#endif /*_SRTMINORSERVOSCANTHREAD_H_*/
