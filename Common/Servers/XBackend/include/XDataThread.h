#ifndef XDATATHREAD_H_
#define XDATATHREAD_H_

/* ************************************************************************************************************* */
/* 						                                            				                            */
/* $Id: XDataThread.h																					         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                         					       when            What                                                       */
/* Liliana Branciforti(bliliana@arcetri.astro.it)  30/07/2009     Creation                                       */
#include "XaThread.h"
#include "XaSemaphore.h"
#include "GroupSpectrometer.h"
#include <IRA>


using ACS::ThreadBase;

class XarcosThread;

class XDataThread : public virtual Thread
{
public:
	struct DataParameter {
		XarcosThread *sender;
		GroupSpectrometer *group;
	};
	/**
	 * Constructor().
	 * @param name thread name
	 * @param time in  
	 * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime	thread's sleep time in 100ns unit. Default value is 100ms.
	*/
	XDataThread(const ACE_CString& name,DataParameter *par,
                        const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
                        const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime); 

	/**
	 * Destructor. 
	*/
    ~XDataThread();
	
	/**
	 * This method is executed once when the thread starts.
	*/
	void Start();
	
	/**
	 * This method is executed once when the thread stops.
	*/
	void Stop();

	
	/** 
	 * This method overrides the thread implementation class. The method is executed in a loop until the thread is alive. 
	 * The thread can be exited by calling ACS::ThreadBase::stop or ACS::ThreadBase::exit command. 
	*/
    void run();

	Semaphore StartSem,StopSem;
	
protected:
	bool ciclo;
	XarcosThread *m_sender;	
	GroupSpectrometer *groupS;
};

#endif /*TIMER_H_*/
