#ifndef MEDICINAACUTHREAD_H
#define MEDICINAACUTHREAD_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: MedicinaMountThread.h,v 1.2 2010-10-14 12:08:28 a.orlati Exp $									           */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                 when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  20/07/2005      Creation                                         */
/* Andrea Orlati(aorlati@ira.inaf.it)  20/09/2005      Implemented the long job monitoring              */

#include "MedicinaMountSocket.h"
#include <acsThread.h>
#include <IRA>

using ACS::ThreadBase;

//class MedicinaMountImpl;

/**
 * This class inheriths from the ACS thread base class. It allows to run separate thread that performs a continuos refresh of ACU data
 * even if no client is connected asking for properties; this causes indeed the Notification Channel to be supplied with new data. 
 * It also monitors the long job executions.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
 */
class CMedicinaMountControlThread : public ACS::Thread
{
public:
	typedef struct {
		ACSErr::Completion comp;
		bool done;
	} TLongJobResult;
	
	typedef struct {
		TLongJobResult* result;
		WORD jobID;
	} TLongJobData;
		
	/**
	 * Constructor().
	 * @param name thread name
	 * @param ACUControl pointer to the MedicinaMount socket object 
	 * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime	thread's sleep time in 100ns unit. Default value is 100ms.
	*/
	CMedicinaMountControlThread(const ACE_CString& name,CSecureArea<CMedicinaMountSocket> *ACUControl,
                        const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
                        const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime); 

	/**
	 * Destructor. 
	*/
    ~CMedicinaMountControlThread();
	
	/**
	 * This method is executed once when the thread starts.
	*/
	virtual void onStart();
	

	/**
	 * This method is executed once when the thread stops.
	*/
	virtual void onStop();

	/** 
	 * This function is used to inform the thread about the long job that is actually executed by the component so that the thread can monitor
     * it and check if and when it finishes. 
	 * @param job job identification, a zero means that no long job is running at present.
	 * @return the pointer to the TLongJobResult structure where the job result will be stored. The user of this structure is in charge off
	 *         freeing it.
	*/
	TLongJobResult *setLongJob(WORD job);
	
	/** 
	 * This method overrides the thread implementation class. The method is executed in a loop until the thread is alive. 
	 * The thread can be exited by calling ACS::ThreadBase::stop or ACS::ThreadBase::exit command. 
	*/
    virtual void runLoop();

protected:
	CSecureArea<CMedicinaMountSocket> * m_pACUControl;
	CSecureArea<TLongJobData>* m_pData;
};

	



#endif
