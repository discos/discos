#ifndef _WORKINGTHREAD_H_
#define _WORKINGTHREAD_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: WorkingThread.h,v 1.2 2008-03-07 17:12:52 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  28/01/2008      Creation                                                  */

#include <acsThread.h>
#include <IRA>
#include <BossCore.h>

/**
 * This class implements a working thread. This thread is in charge of loading the raw coordinates into the ACU in order to exploit the 
 * <i>>ACU_PROGRAMTRACK</i> mode. It tries to keep the ammount of coordinates loaded between the minimum and maximum 
 * number of points (@sa the configuration database). 
*/
class CWorkingThread : public ACS::Thread
{
public:
	/**
     * Constructor().
     * @param name thread name
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
    */
	CWorkingThread(const ACE_CString& name,IRA::CSecureArea<CBossCore>  *param, 
			const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

	/**
	 * Destructor.
    */
    ~CWorkingThread();

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
private:
	IRA::CSecureArea<CBossCore> *m_core;
	TIMEDIFFERENCE m_slice;
	long m_gap;
	long m_maxPoints;
	long m_minPoints;
	long m_sleepTime;
	bool m_trackingOk;
};

#endif /*WORKINGTHREAD_H_*/
