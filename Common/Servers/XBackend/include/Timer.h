#ifndef TIMER_H_
#define TIMER_H_
 
/** **************************************************************************************************** */
/*                                                                                                      */
/*  $Id:Timer.h                                                                                */
/*                                                                                                     */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                     					           when            What                                              */
/* Liliana Branciforti(bliliana@arcetri.astro.it)  30/07/2009     Creation 								*/

#include <acsThread.h>
#include "GroupSpectrometer.h"
#include <IRA>


using ACS::ThreadBase;

class XarcosThread;

class Timer : public ACS::Thread
{
public:
	struct TimerParameter {
		XarcosThread *sender;
		long TimerId;
		long interval;
		bool tp;
	};
	/**
	 * Constructor().
	 * @param name thread name
	 * @param par  
	 * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime	thread's sleep time in 100ns unit. Default value is 100ms.
	*/
	Timer(const ACE_CString& name,TimerParameter *par,
                        const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
                        const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime); 

	/**
	 * Destructor. 
	*/
    ~Timer();
	
	/**
	 * This method is executed once when the thread starts.
	*/
	virtual void Start();
	
	/**
	 * This method is executed once when the thread stops.
	*/
	virtual void Stop();

	
	/** 
	 * This method overrides the thread implementation class. The method is executed in a loop until the thread is alive. 
	 * The thread can be exited by calling ACS::ThreadBase::stop or ACS::ThreadBase::exit command. 
	*/
    virtual void runLoop();
	bool event;
	
protected:
	long TimerId;
	ACS::TimeInterval v_interval;
	ACS::Time timeStart;
	bool ok;
	XarcosThread *m_sender;	
	bool tp;
};

#endif /*TIMER_H_*/
