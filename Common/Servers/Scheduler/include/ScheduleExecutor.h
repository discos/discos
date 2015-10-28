#ifndef _SCHEDULEEXECUTOR_H_
#define _SCHEDULEEXECUTOR_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  13/04/2010      Creation                                                  */
/* Andrea Orlati(aorlati@ira.inaf.it)  20/09/2010      StartRecording not done if scan duration is 0.0                                                  */
/* Andrea Orlati(aorlati@ira.inaf.it)  29/05/2012       the start of a schedule now cause the opening of a new log file*/

#include <acsThread.h>
#include <IRA>
#include <acsContainerServices.h>
#include <DataReceiverC.h>
#include <GenericBackendC.h>
#include "Schedule.h"
#include "Configuration.h"

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
 	 * This method loads a schedule file and starts the execution of the schedule form the given subscan.
 	 * @param scheduleFile string that reports the file name of the schedule to load
 	 * @param subScanidentifier identifies the subscan to start from
 	 * @throw ComponentErrors::MemoryAllocationExImpl
 	 * @throw ManagementErrors::ScheduleErrorExImpl
 	 * @throw ComponentErrors::AlreadyRunningExImpl
 	 * @throw ManagementErrors::SubscanErrorExImpl
 	 * @throw ComponentErrors::CouldntGetComponentExImpl
 	 * @throw ComponentErrors::CORBAProblemExImpl
 	 * @throw ManagementErrors::LogFileErrorExImpl
 	*/
     void startSchedule(const char* scheduleFile,const char * subScanidentifier) throw (ManagementErrors::ScheduleErrorExImpl,
    		 ManagementErrors::AlreadyRunningExImpl,ComponentErrors::MemoryAllocationExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ManagementErrors::LogFileErrorExImpl);
     
     /**
      * initialize the schedule executor.
      * @param services pointer to the container services
      * @param dut1 delta UT UT1
      * @param site site information structure
      * @param config pointer to the configuration object
      */
     void initialize(maci::ContainerServices *services,const double& dut1,const IRA::CSite& site, CConfiguration* config);
     
     /**
      * Called to stop the schedule execution.
      * @param if force is true the schedule is immediately stopped; if flase the schedule is stopped as soon as the current scan is completed.
      */
     void stopSchedule(bool force);
     
     /**
      * @return the schedule name
     */
     const IRA::CString& getScheduleName() { baci::ThreadSyncGuard guard(&m_mutex); return m_scheduleName; }
     
     /**
      * @return the code of the project
     */
     const IRA::CString& getProjectCode() { baci::ThreadSyncGuard guard(&m_mutex); return m_projectCode; }
     
     /**
      * Set the current project code, the code could be overridden by a call to startSchedule
      */
     void setProjectCode(const IRA::CString& code) { baci::ThreadSyncGuard guard(&m_mutex); m_projectCode=code;	 }

     /**
      * Method is almost atomic, no sync required
      * @return the currently executed subscan
      */
     const DWORD& getCurrentScheduleCounter() {  /*baci::ThreadSyncGuard guard(&m_mutex);*/ return m_scheduleCounter; }
     
     /**
      * @return the currently executed scan/subscan identifiers
      */
     void getCurrentScanIdentifers(DWORD& scanID,DWORD& subScanID);

     /**
      * Method is almost atomic, no sync required
      * @return true if a schedule is currently running
      */
     const bool& isScheduleActive() { /*baci::ThreadSyncGuard guard(&m_mutex);*/ return m_active; }
     
     /**
      * Get a reference to the backend used by the schedule. Attention must be payed to the fact that the currently used backend may change without advice.
      * So the caller must ask for another reference to the backend itself. 
      * @return the reference of backend currently used by the schedule, it must be freed
      */
     //Backends::GenericBackend_ptr getBackendReference();

     /**
      * Get a reference to the writer used by the schedule. Attention must be payed to the fact that the currently used writer may change without advice.
      * So the caller must ask for another reference to the writer itself.
      * @return the reference of writer currently used by the schedule, it must be freed
      */
     //Management::DataReceiver_ptr  getWriterReference();
     
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
		STOP_SCHEDULING,
		SCAN_EXECUTION,
		POST_SCAN_PROC,
		SCAN_CLOSEUP,
		CLOSEUP_WAIT,
		RECORDING_FINALIZE
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
	 * pointer to the container services
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
	 * The name of the current running project
	 */
	IRA::CString m_projectCode;
	/**
	 * true if the schedule has been loaded successfully
	 */
	bool m_scheduleLoaded;
	/**
	 * Pointer to the current schedule, if NULL no schedule is empty at present 
	*/
	CSchedule *m_schedule;
	/**
	 * Instance name of the current writer
	 */
	//IRA::CString m_currentWriterInstance;
	/**
	 * Instance name of the current backend
	 */
	//IRA::CString m_currentBackendInstance;
	/**
	 * true if an error was detected when communicating to the data receiver
	 */	
	//bool m_writerError;
	/**
	 * DataReceiver Reference
	 */
	//Management::DataReceiver_var m_writer;
	/**
	 * true if an error was detected when communicating to the backend
	 */
	//bool m_backendError;
	/**
	 * Backend reference
	 */
	//Backends::GenericBackend_var m_backend;
	/**
	 * true if an error was detected when communicating to the antenna boss
	 */
	//bool m_antennaBossError;
	/**
	 * Antenna boss reference
	 */
	//Antenna::AntennaBoss_var m_antennaBoss;
	/**
	 * Minor Servo boss reference
	 */
	//MinorServo::MinorServoBoss_var m_minorServoBoss;
	/**
	 * true if an error was detected when communicating to the minor servo boss
	 */
	//bool m_minorServoBossError;
	/**
	 * Site information object
	 */
	IRA::CSite m_site;
	/**
	 * UTC-UT1
	 */
	double m_dut1;
	/**
	 * true is there is a schedule running at present
	 */
	bool m_active;	
	/**
	 * schedule pointer
	 */
	DWORD m_scheduleCounter;
	/**
	 * This flags are used to free things up when the data transfer is started
	 */
	//bool m_streamPrepared; bool m_streamStarted; bool m_streamConnected;
	/**
	 * true if the schedule is already in the middle of a scan
	 */
	 //bool m_scanStarted;
	 /**
	  * true when the schedule has arrived to the end and it is restarted from the beginning, status is persistent through subscans
	  */
	 bool m_scheduleRewound;
	/**
	 * Scan informations
	 */
	CSchedule::TRecord m_currentScan;
	/**
	 * Scan description
	 */
	Schedule::CScanList::TRecord m_currentScanRec;
	/**
	 * Stores the subscan selected as first one by the scheduler (not necessary equal to the one selected by the user)
	 */
	DWORD m_firstSubScan;
	/**
	 * counter of the subscan selected by user as start point of the schedule
	*/
	DWORD m_startSubScan;
	/**
	* This stores the number of scans completed during a repetition
	*/
	long m_scansCounter;
	/**
	 * identifier of the current scan
	 */
	DWORD m_lastScanID;
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
	 * false if the sub scan is executing
	 */
	bool m_subScanDone;
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
	 * represent the time stamp at which the current scan is supposed to be closed and a new one can be initialized
	 */
	ACS::Time m_closeScanTimer;

	/**
	 * pointer to the configuration object
	 */
	 CConfiguration *m_config;

	/**
	 * Extract the next scan from a schedule.
	 * @param currentLine this is the current scan identifier
	 * @param rec structure that stores the selected schedule line
	 * @param scanRec structure that contains the scan parameters
	 * @throw ManagementErrors::ScheduleErrorExImpl
	 */
	void getNextScan(const DWORD& currentLine,CSchedule::TRecord& rec,Schedule::CScanList::TRecord& scanRec) throw (ManagementErrors::ScheduleErrorExImpl);
	
	/**
	 * Stops the data recording. It sets the <i>m_scanDone</i> flag to true.
	 */
	void stopRecording();
	
	/**
	 * get the reference to the current writer
	 */
	//void openWriter(const IRA::CString& wrt) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl);
	
	/**
	 * Release the current writer
	 */
	//void closeWriter() throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl);
	
	/**
	 * Get the reference to the current backend
	 */
	//void openBackend(const IRA::CString& bck) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl);

	/**
	 * Release the current backend
	*/
	//void closeBackend() throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl);


	/**
	 * This will start the data transfer from the backend to the receiver. It will takes all the required operation or nothing if the schedule does not
	 * requires changes from the previous scan
	 */
	void prepareFileWriting(const CSchedule::TRecord& rec) throw (ManagementErrors::ScheduleErrorExImpl,
			ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl,
			ComponentErrors::CouldntReleaseComponentExImpl,ManagementErrors::BackendProcedureErrorExImpl,
			ManagementErrors::BackendNotAvailableExImpl);
	
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

