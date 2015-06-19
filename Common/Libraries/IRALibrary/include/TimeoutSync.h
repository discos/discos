#ifndef TIMEOUTSYNC_H_
#define TIMEOUTSYNC_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: TimeoutSync.h,v 1.1 2010-11-05 11:28:17 a.orlati Exp $	    												  */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 05/1/2010      Creation                                          */


#include <acstimeEpochHelper.h>
#include "Definitions.h"

namespace IRA {

/**
 * This class can be used to synchornize threads that want to acces the same peace of code concurrently. Given an ACE recursive mutex it takes charge of
 * locking it and finally makes sure it is released. It also support for timeout, if the resourse is locked it trys to lock it for a given time before failing.  
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */
class CTimeoutSync {
public:
	/**
	 * Constructor. The resource is not acquired at construction time, but is must be acquired esplicitally.
	 * @param mutex pointer to the mutex
	 * @param timeout time to wait before declaring timeout (microseconds), if a zero is passed the subsequent call to <i>acquire()</i> is blocking.
	 * @param sleepTime time the thread will sleep before try to acquire the resource again (microseconds). Default is 25 milliseconds.
	 */
	CTimeoutSync(ACE_Recursive_Thread_Mutex * mutex,const DDWORD& timeout,const DDWORD& sleepTime=25000);
	/**
	 * Destructor. It takes charge to unlock the resource if still blocked
	 */
	~CTimeoutSync();
	/**
	 * Explicitly release the resource, if the resource was not locked it does nothing.
	*/ 
	void release();
	/**
	 * Explicitly lock the resource, this call is blocking but if the resource could not be acquired before the timeout expires it exists.
	 * @return true if the lock could be taken before the timeout expires. 
	 */
	bool acquire();
	/**
	 * @return true if the mutex is locked by this class;
	 */
	bool isLocked() const;
private:
	ACE_Recursive_Thread_Mutex * m_mutex;
	DDWORD m_timeout;
	DDWORD m_sleepTime;
	bool m_locked;
};

};


#endif
