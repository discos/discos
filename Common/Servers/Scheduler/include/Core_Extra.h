#ifndef CORE_EXTRA_H_
#define CORE_EXTRA_H_

// $Id: Core_Extra.h,v 1.5 2011-04-15 09:08:08 a.orlati Exp $

#define EXTRA_INIT

/**
 * Make the current thread wait for the given number of seconds. It can be used stop execution.
 * @param seconds number of seconds to wait, expressed as integer number plus fraction of seconds.
*/
void wait(const double& seconds) const;

/**
 * The implementation of No Operation command. It does nothing
 */
void nop() const;

/**
 * Make the current thread wait for the tracking flag to be true. It can be used to stop the schedule execution until the telecope is tracking
 */
void waitOnSource() const;

/**
 * 
 */
int status() const;

#endif

