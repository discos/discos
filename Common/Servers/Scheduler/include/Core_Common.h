#ifndef CORE_COMMON_H_
#define CORE_COMMON_H_

// $Id: Core_Common.h,v 1.2 2011-05-20 16:53:09 a.orlati Exp $

#define COMMON_INIT 


/**
 * Check the scan against the instrumentation. If it could be executed or not. The result could depend also on schedule mode, scan type, instrument configuration and so on.
 * @param mode schedule mode
 * @param rec structure that stores the selected schedule line
 * @param scanRec structure that contains the scan parameters
 * @param antBoss reference to the antenna boss
 * @param antBossErr flags the status of the antenna boss reference
 * @throw ComponentErrors::UnexpectedExImpl
 * @throw ComponentErrors::OperationErrorExImpl
 * @throw ComponentErrors::ComponentNotActiveExImpl
 * @thorw ComponentErrors::CORBAProblemExImpl
 * @return true if the scan has been checked succesfully
 */ 	
static bool checkScan(const CSchedule::TScheduleMode& mode,const CSchedule::TRecord& scanInfo,const Schedule::CScanList::TRecord& scanData,Antenna::AntennaBoss_ptr antBoss,bool& antBossError) 
	throw (ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl);


/**
 * Send to the antenna boss the commands required to start scan.
 * @thorw ComponentErrors::OperationErrorExImpl
 * @throw ComponentErrors::CORBAProblemExImpl
 * @throw ComponentErrors::UnexpectedExImpl
 * @throw ComponentErrors::ComponentNotActiveExImpl
 * @param scan structure that contains the scan to be executed. The ut field can be modified. 
 * @param scanRec structure that contains the scan parameters
 * @param par structure that describes the scan
 * @param antBoss reference to the antenna boss
 * @param antBossErr flags the status of the antenna boss reference
*/
static void doScan(CSchedule::TRecord& scanInfo,const Schedule::CScanList::TRecord& scanData,Antenna::AntennaBoss_ptr antBoss,bool& antBossError) throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::ComponentNotActiveExImpl);


/**
 * This method will close (if necessary) the enstablished connection between a backend and a data receiver.It closes and frees the resources allocated to enable the trasmission. In practice it calls sendStop()
 * terminate(), disconnect() and closeReceiver()
 * @param backend reference to the backend 
 * @param backendError will be returned back true if an error occurred in the communication to backend component
 * @param writer reference to the writer or data recorder or data dealer 
 * @param writerError will be returned back true if an error occurred in the communication to writer component
 * @param streamStarted this argument indicates that the data transfer has been started, on exit it is false if the closeup is succesful
 * @param streamPrepared this argument indicates that the data transfer has been setup, on exit it is false if the closeup is succesful
 * @param streamconnected this argument indicates that connection between the sender and receiver component was enstablished, on exit it is false if the closeup is succesful 
 */
static void disableDataTransfer(Backends::GenericBackend_ptr backend,bool& backendError,Management::DataReceiver_ptr writer,bool& writerError,bool& streamStarted,bool& streamPrepared,
		bool& streamConnected) throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl);

/**
 * Enable the transfer between the backend and the receiver. In practice it connects the backend with the recevier and calls sendHeader().
 * @param backend reference to the backend 
 * @param backendError will be returned back true if an error occurred in the communication to backend component
 * @param writer reference to the writer or data recorder or data dealer 
 * @param writerError will be returned back true if an error occurred in the communication to writer component
 * @param streamPrepared this argument indicates that the data transfer has been setup, on exit it is false if the closeup is succesful
 * @param streamconnected this argument indicates that connection between the sender and receiver component was enstablished, on exit it is false if the closeup is succesful  
 */
static void enableDataTransfer(Backends::GenericBackend_ptr backend,bool& backendError,Management::DataReceiver_ptr writer,bool& writerError,bool& streamPrepared,bool& streamConnected) throw (
		ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl);

/**
 * This command set up the data receiver with some parameters.
 * @param writer reference to the writer or data recorder or data dealer 
 * @param writerError will be returned back true if an error occurred in the communication to writer component
 * @param obsName name of the current observer
 * @param prj name of the project currently running
 * @param scanId numerical identifier of the scan
 * @param device identifier of the device currently in use
 * @param axis indicates which axis is currently used by the telescope.
 * @param fileName name of the file to be created
 */
static void setupDataTransfer(Management::DataReceiver_ptr writer,bool& writerError,const IRA::CString& obsName,const IRA::CString& prj,const long& scanId,const long& device,
		const Management::TScanAxis& axis,const IRA::CString& fileName) throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl);

/**
 * This static method starts the data transfer between the backend and the configured data recorder.
 * @param backend reference to the backend 
 * @param backendError will be returned back true if an error occurred in the communication to backend component
 * òparam startTime this is the exact time the data transfer should start
 * @param streamStarted this argument indicates that the data transfer has been started, on exit it is false if the closeup is succesful
 * @param streamPrepared this argument indicates that the data transfer has been setup, on exit it is false if the closeup is succesful
 * @param streamconnected this argument indicates that connection between the sender and receiver component was enstablished, on exit it is false if the closeup is succesful 
 */
static void startDataTansfer(Backends::GenericBackend_ptr backend,bool& backendError,const ACS::Time& startTime,bool& streamStarted,bool& streamPrepared,bool& streamConnected) throw (
		ComponentErrors::OperationErrorExImpl,ComponentErrors::UnexpectedExImpl,ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl);

/**
 * This statci method dtops the data transfer between the backend and the configured data recorder.
 * @param backend reference to the backend 
 * @param backendError will be returned back true if an error occurred in the communication to backend component
 * @param streamStarted this argument indicates that the data transfer has been started, on exit it is false if the closeup is succesful
 * @param streamPrepared this argument indicates that the data transfer has been setup, on exit it is false if the closeup is succesful
 * @param streamconnected this argument indicates that connection between the sender and receiver component was enstablished, on exit it is false if the closeup is succesful
 */ 
 static void stopDataTransfer(Backends::GenericBackend_ptr backend,bool& backendError,bool& streamStarted,bool& streamPrepared,bool& streamConnected) throw (ComponentErrors::OperationErrorExImpl,
		ManagementErrors::BackendNotAvailableExImpl);

/**
 * Given an UT time, this function returns the next ut at which the provided lst takes place. It takes into
 * account that two different UT instants can correspond to the same lst. 
 * @param currentUT current UT time used for recursion
 * @param checkUT current UT time used for comparison
 * @param lst local sidereal time to be converted in UT time (radians)
 * @param site information about the site
 * @param dut1 difference between UTC and UT1 
 * @return UT corresponding to the input lst
 */ 
static ACS::Time getUTFromLST(const IRA::CDateTime& currentUT,const IRA::CDateTime& checkUT,const ACS::TimeInterval& lst,const IRA::CSite& site,const double& dut1);

/**
 * Computes the name of the output file, including the lst time
 * @param ut universal time
 * @param lst localsidereal time
 * @param prj name of the project
 * @param suffix string that will be appended to the end of file name
 * @return the name of the file to be created
*/
static IRA::CString computeOutputFileName(const ACS::Time& ut,const ACS::TimeInterval& lst,const IRA::CString& prj,const IRA::CString& suffix);

/**
 * Computes the name of the output file, not including the lst time
 * @param ut universal time
 * @param lst localsidereal time
 * @param prj name of the project
 * @param suffix string that will be appended to the end of file name
 * @return the name of the file to be created
*/	
static IRA::CString computeOutputFileName(const ACS::Time& ut,const IRA::CString& prj,const IRA::CString& suffix);

/**
 * This is the handler of the events coming from the notification channel published by the Antenna subsystem
 */
static void antennaNCHandler(Antenna::AntennaDataBlock antennaData,void *handlerParam);

/**
 * used as wrapper to function that are in charge of forwarding commands to other packages  
*/
IRA::CString remoteCall(const IRA::CString& command,const IRA::CString& package,const long& par) throw (ParserErrors::ExecutionErrorExImpl,ParserErrors::PackageErrorExImpl); 


#endif /*CORE_COMMON_H_*/

