#ifndef ENGINETHREAD_H_
#define ENGINETHREAD_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: EngineThread.h,v 1.7 2011-04-22 18:51:48 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                  when                       What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  14/01/2009      Creation                                                  */

//#define FW_DEBUG

#include <acsThread.h>
#include <IRA>
#include <DataCollection.h>
#ifdef FW_DEBUG
#include <iostream>
#include <fstream>
#else
#include "FitsWriter.h"
#endif
#include "Configuration.h"

namespace FitsWriter_private {

/**
 * This class implements a working thread. This thread is in charge of collecting the data from the backend(sender)
 * and mark them with the telescope coordinates. The whole packet is then added with corrateral information and
 * sent to the fits engine in order to produce the output file  
*/
class CEngineThread : public ACS::Thread
{
public:
	/**
     * Constructor().
     * @param name thread name
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
    */
	CEngineThread(const ACE_CString& name,CSecureArea<CDataCollection> *param, 
			const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
			const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

	/**
	 * Destructor.
    */
    ~CEngineThread();

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
     
     /**
      * It sets the slice of time the thread is allowed to run.
      * @param timeSlice time slice given in 100 ns  
      */
     void setTimeSlice(const long& timeSlice) { m_timeSlice=timeSlice; }
     
     void setConfiguration(CConfiguration * const conf) { m_config=conf; }
     
     void setServices(maci::ContainerServices * const service) { m_service=service; }
     
private:
	CSecureArea<CDataCollection> *m_dataWrapper;
#ifdef FW_DEBUG
	ofstream m_file;
#else
	CFitsWriter *m_file;
#endif
	CConfiguration *m_config;
	maci::ContainerServices * m_service;
	bool m_fileOpened;
	long m_timeSlice;
	double *m_ptsys;
	bool antennaBossError;
	Antenna::AntennaBoss_var m_antennaBoss;
	bool receiverBossError;
	Receivers::ReceiversBoss_var m_receiversBoss;
	bool checkTime(const ACS::Time& currentTime);
	bool checkTimeSlot(const ACS::Time& slotStart);
	bool processData();
	/**
	 * It gets and updates the receiver code, and all the required data from the receviers subsystem 
	 */
	void collectReceiversData();
	/**
	 * It gets and updates the all the required data from the antenna subsystem 
	 */
	void collectAntennaData();

};

};

#endif /*ENGINETHREAD_H_*/

