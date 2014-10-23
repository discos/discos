#ifndef CORE_COMMON_H_
#define CORE_COMMON_H_

#define COMMON_INIT

/**
 * This is not thread safe but we can consider it almost atomic.
 * It clear the tracking flag, issued when a new scan is commanded in order to prevent the scheduler to consider the tracking when it is not the case
 */
inline void clearAntennaTracking() { m_isAntennaTracking=false; }

/**
 * This is not thread safe but we can consider it almost atomic.
 * It clear the tracking flag, issued when a new scan is commanded in order to prevent the scheduler to consider the tracking when it is not the case
 */
inline void clearReceiversTracking() { m_isReceiversTracking=false; }

/**
 * This is not thread safe but we can consider it almost atomic.
 * It clear the tracking flag from minor servo, issued when a new scan is commanded in order to prevent the scheduler to consider the tracking when it is not the case
 */
inline void clearMinorServoTracking() { if (MINOR_SERVO_AVAILABLE) m_isMinorServoTracking=false; }

/**
 * This is not thread safe but we can consider it almost atomic.
 * This method will cause the tracking flag to be false for a certain amount of time.
 */
void clearTracking();

/**
 * It allows to change the current status of the scheduler.
 * @param status new status of the scheduler
 */
 void changeSchedulerStatus(const Management::TSystemStatus& status);

/**
 * check is the stream or data recording has already started or not
 */
bool isStreamStarted() const;

/*
*/
int status() const;


#endif /*CORE_COMMON_H_*/

