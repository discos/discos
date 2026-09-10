#ifndef ENGINETHREAD_H_
#define ENGINETHREAD_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                  when                       What                          */
/* Andrea Orlati(aorlati@ira.inaf.it)  31/08/2026      Creation                                                  */


#include <acsThread.h>
#include <IRA>
#include <DataCollection.h>
#include <iostream>
#include <fstream>

#include "Configuration.h"
#include "MetaData.h"

namespace DataWriter_private {

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
	CEngineThread(const ACE_CString& name, CDataCollection *param, 
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
      * @param timeSlice time slice given in microseconds
      */
     void setTimeSlice(const long& timeSlice) {
    	 m_timeSlice=(ACS::TimeInterval)timeSlice*10;
    	 /*setResponseTime(m_timeSlice*2);*/
     }
     
     void setConfiguration(CConfiguration * const conf) { m_config=conf; }
     
     void setServices(maci::ContainerServices * const service) { m_service=service; }
     
private:
	DataWriter_private::CDataCollection *m_data;
	ofstream m_file;
	CConfiguration *m_config;
	CMetaData m_info;
	maci::ContainerServices * m_service;
	bool m_fileOpened;
	//bool m_summaryOpened;
	ACS::TimeInterval m_timeSlice;
	double *m_ptsys;
	/*bool antennaBossError;
	Antenna::AntennaBoss_var m_antennaBoss;
	bool receiverBossError;
	Receivers::ReceiversBoss_var m_receiversBoss;
	bool minorServoBossError;
	MinorServo::MinorServoBoss_var m_minorServoBoss;
	bool m_schedulerError;
	Management::Scheduler_var m_scheduler;
	ACS::Time m_lastMinorServoEnquireTime;*/
	bool checkTime(const ACS::Time& jobStartTime);
	bool checkTimeSlot(const ACS::Time& slotStart);
	bool processData();
};

};

#endif /*ENGINETHREAD_H_*/

