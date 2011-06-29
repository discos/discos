#ifndef _SCHEDULEEXECUTOR_H_
#define _SCHEDULEEXECUTOR_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: ScheduleExecutor.h,v 1.10 2011-05-20 16:53:09 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  13/04/2010      Creation                                                  */
/* Andrea Orlati(aorlati@ira.inaf.it)  20/09/2010      StartRecording not done if scan duration is 0.0                                                  */

#include <acsThread.h>
#include <IRA>
#include <acsContainerServices.h>
#include <DataReceiverC.h>
#include <GenericBackendC.h>
#include "Schedule.h"

#include "Core.h"

/**
 * This class implements a watching thread. This thread is in charge of update all the obseved coordinates. It is started when the component
 *  goes up and it continues to run until the component dies. 
*/
class CScheduleExecutor : public ACS::Thread
{
public:
	/**
     * Constructor().
     * @param name thread name
     * @param par parameter passed to the thread
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
    */
	CScheduleExecutor(const ACE_CString& name,CCore *par,const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
			const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

	/**
	 * Destructor.
    */
    ~CScheduleExecutor();

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
 	 * This method loads a schedule file and starts the execution of the schedule form the given line.
 	 * @param scheduleFile string that reports the file name of the schedule to load
 	 * @param lineNumber line inside the schedule to start from
 	 * @throw ComponentErrors::MemoryAllocationExImpl
 	 * @throw ManagementErrors::ScheduleErrorExImpl
 	 * @throw ComponentErrors::AlreadyRunningExImpl
 	 * @throw ManagementErrors::SubscanErrorExImpl
 	 * @throw ComponentErrors::CouldntGetComponentExImpl
 	*/
     void startSchedule(const char* scheduleFile,const long& lineNumber) throw (ManagementErrors::ScheduleErrorExImpl,
    		 ManagementErrors::AlreadyRunningExImpl,ComponentErrors::MemoryAllocationExImpl,ComponentErrors::CouldntGetComponentExImpl);
     
     /**
      * initialize the schedule executor.
      * @param services pointer to the container services
      * @param dut1 delta UT UT1
      * @param site site information structure
      */
     void initialize(maci::ContainerServices *services,const double& dut1,const IRA::CSite& site);
     
     /**
      * Called to stop the schedule execution.
      * @param if forse is true the schedule is immediately stopped; if flase the schedule is stopped as soon as the current scan is completed.
      */
     void stopSchedule(bool force);
     
     /**
      * @return the schedule name
     */
     const IRA::CString& getScheduleName() { baci::ThreadSyncGuard guard(&m_mutex); return m_scheduleName; }
     
     /**
      * @return the schedule name
     */
     const IRA::CString& getScheduleOwner() { baci::ThreadSyncGuard guard(&m_mutex); return m_scheduleOwner; }     
     
     /**
      * @return the currently executed schedule line
      */
     const DWORD& getCurrentScheduleLine() {  baci::ThreadSyncGuard guard(&m_mutex); return m_currentLine; }
     
     /**
      * @return true if a schedule is currently running
      */
     const bool& isScheduleActive() { baci::ThreadSyncGuard guard(&m_mutex); return m_active; }
     
     /**
      * Get a reference to the backend used by the schedule. Attention must be payed to the fact that the currently used backend may change without advice.
      * So the caller must ask for another reference to the backend itself. 
      * @return the reference of backend currently used by the schedule
      */
     Backends::GenericBackend_ptr getBackendReference();
     
private:
	enum TScheduleStages {
		LIMBO,
		INIT,
		STARTTIME_WAIT,
		SCAN_SELECTION,
		SCAN_CHECK,
		SCAN_PREPARATION,
		WRITING_INITIALIZATION,
		PRE_SCAN_PROC,
		SCAN_START,
		SCHEDULE_STOP,
		SCAN_EXECUTION,
		POST_SCAN_PROC
	};
	/**
	 * Stores the present stage of the scan
	 */
	TScheduleStages m_stage;
	/**
	 * Flags used to set the wait status of the execution thread on and off
	 */
	bool m_goAhead;
	/**
	 * Thread sync mutex
	 */
	BACIMutex m_mutex;
	/**
	 * pointer to the container serivices
	 */
	CCore* m_core;
	/**
	 * Container services pointer
	 */
	maci::ContainerServices *m_services;
	/**
	 * Name of the running schedule
	 */
	IRA::CString m_scheduleName;
	
	/**
	 * Name of the owner of the current schedule
	 */
	IRA::CString m_scheduleOwner;
	/**
	 * true if the schedule has been loaded succesfully
	 */
	bool m_scheduleLoaded;
	/**
	 * Pointer to the current schedule, if NULL no schedule is empty at present 
	*/
	CSchedule *m_schedule;
	/**
	 * Instance name of the current writer
	 */
	IRA::CString m_currentWriterInstance;
	/**
	 * Instance name of the current backend
	 */
	IRA::CString m_currentBackendInstance;
	/**
	 * true if an error was detected when communicating to the data receiver
	 */	
	bool m_writerError;
	/**
	 * DataReceiver Reference
	 */
	Management::DataReceiver_var m_writer;
	/**
	 * true if an error was detected when communicating to the backend
	 */
	bool m_backendError;
	/**
	 * Backend reference
	 */
	Backends::GenericBackend_var m_backend;
	/**
	 * true if an error was detected when communicating to the antenna boss
	 */
	bool m_antennaBossError;
	/**
	 * Antenna boss reference
	 */
	Antenna::AntennaBoss_var m_antennaBoss;
	/**
	 * Site information object
	 */
	IRA::CSite m_site;
	/**
	 * UTC-UT1
	 */
	double m_dut1;
	/**
	 * true is there is a schedule runnig at present
	 */
	bool m_active;	
	/**
	 * schedule pointer
	 */
	DWORD m_currentLine;
	/**
	 * This flags are used to free things up when the data transfer is started
	 */
	bool m_streamPrepared; bool m_streamStarted; bool m_streamConnected;
	/**
	 * Scan informations
	 */
	CSchedule::TRecord m_currentScan;
	/**
	 * Scan description
	 */
	Schedule::CScanList::TRecord m_currentScanRec;
	/**
	 * Stores the line number of the first scan selecteed by the scheduler
	 */
	DWORD m_firstScan;
	/**
	 * Line number selected by the user as starting scan of the schedule
	*/
	DWORD m_startScan;
	/**
	* This stores the number of scans completed during a repetition
	*/
	long m_scansCounter;
	/**
	 * stores the number of repetitions performed up to now
	 */
	DWORD m_repetition;
	/**
	 * Name of the present backend procedure
	 */ 
	IRA::CString m_currentBackendProcedure;
	/**
	 * This stores the time of the start of the recording
	*/
	ACS::Time m_startRecordTime;
	/**
	 * The last time programmed for the timer
	 */
	ACS::Time m_lastScheduledTime;
	
	/**
	 * false if the scan is executing
	 */
	bool m_scanDone;
	/**
	 * true if an error was detected after the stop recording command was issued
	 */
	bool m_scanStopError;
	/**
	 * If set to true the thread stops itself and the schedule immediately closed
	 */
	bool m_stopMe;
	/**
	 * if set to true the thread completes the current scan and than close the schedule
	 */
	bool m_haltMe;
	
	/**
	 * Extract the next scan from a schedule.
	 * @param currentLine this is the current scan identifier
	 * @param rec structure that stores the selected schedule line
	 * @param scanRec structure that contains the scan parameters
	 * @throw ManagementErrors::ScheduleErrorExImpl
	 */
	void getNextScan(const DWORD& currentLine,CSchedule::TRecord& rec,Schedule::CScanList::TRecord& scanRec) throw (ManagementErrors::ScheduleErrorExImpl);
	
	/**
	 * Setup the commands to start data acquisition
	 * @param rec structure that stores the selected schedule line
	 */
	void startRecording(const CSchedule::TRecord& rec) throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl);
	
	/**
	 * Stops the data recording. It sets the <i>m_scanDone</i> flag to true.
	 */
	void stopRecording();
	
	/**
	 * get the reference to the current writer
	 */
	void openWriter(const IRA::CString& wrt) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl);
	
	/**
	 * Release the current writer
	 */
	void closeWriter() throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl);
	
	/**
	 * Get the reference to the current backend
	 */
	void openBackend(const IRA::CString& bck) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl);

	/**
	 * Release the current backend
	*/
	void closeBackend() throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl);

	/**
	 * Enable the transfer between the backend and the receiver. In practice it connects the backend with the recevier and calls sendHeader().
	 */
	//void enableFileWriting() throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl);
	
	/**
	 * Close and frees the resources allocated to enabled the transmission between backend and recevier. In practice it calls backend sendStop(), terminate(),
	 * disconnect() and receiver closeReceiver()
	 */
	//void disableFileWriting() throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl);

	/*
	 * It forwards the whole configuration procedure to the backend, one command at a time
	 * @param procedure sequence of commands contained by the configuration procedure
	 * @throw ManagementErrors::ProcedureErrorExImpl
	 * @thorw ComponentErrors::CORBAProblemExImpl
	 * @thorw ComponentErrors::UnexpectedExImpl
	 */
	void configureBackend(const std::vector<IRA::CString>& procedure) throw (ManagementErrors::ProcedureErrorExImpl,
			ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl);
	
	/**
	 * This will start the data transfer from the backend to the receiver. It will takes all the required operation or nothing if the schedule does not
	 * requires changes from the previuos scan
	 */
	void prepareFileWriting(const CSchedule::TRecord& rec,const Schedule::CScanList::TRecord& scanRec) throw (ManagementErrors::ScheduleErrorExImpl,
			ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl,
			ComponentErrors::CouldntReleaseComponentExImpl,ManagementErrors::CommandLineErrorExImpl,ManagementErrors::ProcedureErrorExImpl);
	
	/**
	 * This method will abort the current scan and force the executor to start from the next one
	 */
	void cleanScan();

	/**
	 * this method will stop the current schedule and reset the executor status.
	 * @param error true if the schedule close is due to an error.
	 */
	void cleanSchedule(bool error);
	
	/**
	 * This is a callback function called by the parser when a procedure completes.
	 */
	static void procedureCallBack(const void *param,const IRA::CString& name,const bool& result);
	
	/**
	 * This is the timer handler that reacts to the stop recording event
	 */
	static void stopRecordingEventHandler(const ACS::Time& time,const void *par);
	
	/**
	 * This is the timer handler that reacts to the re-try schedule event
	 */
	static void restartEventHandler(const ACS::Time& time,const void *par);

	
};

#endif /*_SCHEDULEEXECUTOR_H_*/

