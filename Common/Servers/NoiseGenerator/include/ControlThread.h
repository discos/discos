#ifndef CONTROLTHREAD_H_
#define CONTROLTHREAD_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: ControlThread.h,v 1.1 2011-03-14 15:16:22 a.orlati Exp $									           */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                 when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  03/11/2009      Creation                                         */

#include <acsThread.h>
#include <IRA>
#include "CommandLine.h"

/**
 * This class inheriths from the ACS thread base class. It allows to run separate thread that performs a continuos check of the backend status
 * even if no client is connected  
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
 */
class CControlThread : public ACS::Thread
{
public:
	/**
	 * Constructor().
	 * @param name thread name
	 * @param parameter  pointer to the object that is the thread running argument 
	 * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime	thread's sleep time in 100ns unit. Default value is 100ms.
	*/
	CControlThread(const ACE_CString& name,CSecureArea<CCommandLine> *parameter,
                        const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
                        const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime); 

	/**
	 * Destructor. 
	*/
    ~CControlThread();
	
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

protected:
	CSecureArea<CCommandLine>* m_pData;
};










#endif /*CONTROLTHREAD_H_*/
