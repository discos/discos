#ifndef CORE_COMMON_H_
#define CORE_COMMON_H_

#define COMMON_INIT 

/**
 * Check the scan against the instrumentation. If it could be executed or not. The result could depend also on schedule mode, scan type, instrument configuration and so on.
 * @param mode schedule mode
 * @param rec structure that stores the selected schedule line
 * @param scanRec structure that contains the scan parameters
 * @param minEl elevation lower limit
 * @param maxEl elevation upper limit
 * @param antBoss reference to the antenna boss
 * @param antBossErr flags the status of the antenna boss reference
 * @throw ComponentErrors::UnexpectedExImpl
 * @throw ComponentErrors::OperationErrorExImpl
 * @throw ComponentErrors::ComponentNotActiveExImpl
 * @throw ComponentErrors::CORBAProblemExImpl
 * @return true if the scan has been checked successfully
 */ 	
static bool checkScan(const CSchedule::TScheduleMode& mode,const CSchedule::TRecord& scanInfo,const Schedule::CScanList::TRecord& scanData,const double& minEl,const double& maxEl,Antenna::AntennaBoss_ptr antBoss,bool& antBossError)
	throw (ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl);


/**
 * Send to the antenna boss the commands required to start scan.
 * @throw ComponentErrors::OperationErrorExImpl
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
 * This method will close (if necessary) the established connection between a backend and a data receiver.It closes and frees the resources allocated to enable the trasmission. In practice it calls sendStop()
 * terminate(), disconnect() and closeReceiver()
 * @param backend reference to the backend 
 * @param backendError will be returned back true if an error occurred in the communication to backend component
 * @param writer reference to the writer or data recorder or data dealer 
 * @param writerError will be returned back true if an error occurred in the communication to writer component
 * @param streamStarted this argument indicates that the data transfer has been started, on exit it is false if the closeup is succesful
 * @param streamPrepared this argument indicates that the data transfer has been setup, on exit it is false if the closeup is succesful
 * @param streamConnected this argument indicates that connection between the sender and receiver component was enstablished, on exit it is false if the closeup is succesful
 * @param scanStarted this argument indicates that the system has already started a scan,in that case it closes it,  on exit it is false
 */
static void disableDataTransfer(Backends::GenericBackend_ptr backend,bool& backendError,Management::DataReceiver_ptr writer,bool& writerError,bool& streamStarted,bool& streamPrepared,
		bool& streamConnected,bool& scanStarted) throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl);

/**
 * Enable the transfer between the backend and the receiver. In practice it connects the backend with the receiver itself
 * @param backend reference to the backend 
 * @param backendError will be returned back true if an error occurred in the communication to backend component
 * @param writer reference to the writer or data recorder or data dealer 
 * @param streamconnected this argument indicates that connection between the sender and receiver component was established, on exit it is false if the closeup is successful
 * @param streamPrepared if false the backend is given command to send headers to the writer, then it is set to true
 */
static void enableDataTransfer(Backends::GenericBackend_ptr backend,bool& backendError,Management::DataReceiver_ptr writer,bool& streamConnected,bool& streamPrepared) throw (
		ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl);

/**
 * It allows to send a configuration procedure to a backend.
 * @param backend reference to the backend
 * @param backendError will be returned back true if an error occurred in the communication to backend component
 * @param name name of the procedure
 * @param procedure sequence of commands contained by the configuration procedure
 * @throw ManagementErrors::BackendProcedureErrorExImpl
 * @throw ComponentErrors::CORBAProblemExImpl
 * @throw ComponentErrors::UnexpectedExImpl
 */
static void configureBackend(Backends::GenericBackend_ptr backend,bool& backendError,const IRA::CString& name, const std::vector<IRA::CString>& procedure) throw (
		ManagementErrors::BackendProcedureErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl);

/**
 * This command set up the data receiver with some parameters.
 * @param scanStarted if false the method will setup the start of the scan (a set it to true), otherwise it is ignored
 * @param writer reference to the writer or data recorder or data dealer 
 * @param writerError will be returned back true if an error occurred in the communication to writer component
 * @param backend reference to the backend
 * @param backendError will be returned back true if an error occurred in the communication to backend component
 * @param obsName name of the current observer
 * @param prj name of the project currently running
 * @param baseName name of the file to be created
 * @param path path to the file that has to be created
 * @param extraPath indicates the extra folder that has to be created (from basePath) in order to contain the file
 * @param schedule name of the schedule file
 * @param targetID identifier of the target of the subscan
 * @param layoutName name of the layout configuration
 * @param layout layout of the scan
 * @param scanTag numerical label to be attached to the scan
 * @param device identifier of the device currently in use
 * @param axis indicates which axis is currently used by the telescope.
 */
static void setupDataTransfer(bool& scanStarted,/*bool& streamPrepared,*/Management::DataReceiver_ptr writer,bool& writerError,Backends::GenericBackend_ptr backend,bool& backendError,
		const IRA::CString& obsName,const IRA::CString& prj,const IRA::CString& baseName,const IRA::CString& path,const IRA::CString& extraPath,const IRA::CString& schedule,const IRA::CString& targetID,
		const IRA::CString& layoutName,const ACS::stringSeq& layout,const long& scanTag,const long& device,const DWORD& scanID,const ACS::Time& startTime,const  DWORD& subScanID,
		const Management::TScanAxis& axis) throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::UnexpectedExImpl);

/**
 * This static method starts the data transfer between the backend and the configured data recorder.
 * @param backend reference to the backend 
 * @param backendError will be returned back true if an error occurred in the communication to backend component
 * @param startTime this is the exact time the data transfer should start
 * @param streamStarted this argument indicates that the data transfer has been started, on exit it is false if the closeup is successful
 * @param streamPrepared this argument indicates that the data transfer has been setup, on exit it is false if the closeup is successful
 * @param streamConnected this argument indicates that connection between the sender and receiver component was established, on exit it is false if the closeup is successful
 */
static void startDataTansfer(Backends::GenericBackend_ptr backend,bool& backendError,const ACS::Time& startTime,bool& streamStarted,bool& streamPrepared,bool& streamConnected) throw (
		ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl);

/**
 * This static method stops the data transfer between the backend and the configured data recorder.
 * @param backend reference to the backend 
 * @param backendError will be returned back true if an error occurred in the communication to backend component
 * @param streamStarted this argument indicates that the data transfer has been started, on exit it is false if the closeup is successful
 * @param streamPrepared this argument indicates that the data transfer has been setup, on exit it is false if the closeup is succesful
 * @param streamConnected this argument indicates that connection between the sender and receiver component was established, on exit it is false if the closeup is successful
 */ 
 static void stopDataTransfer(Backends::GenericBackend_ptr backend,bool& backendError,bool& streamStarted,bool& streamPrepared,bool& streamConnected) throw (ComponentErrors::OperationErrorExImpl,
		ManagementErrors::BackendNotAvailableExImpl);

 /**
  * This static method will call the <i>stopScan()</i> of the DataReceiver interface in order to inform the current data receiver component that a scan has to be finalized.
  * @param writer reference to the writer or data recorder or data dealer
  * @param writerError will be returned back true if an error occurred in the communication to writer component
  * @param it allows to keep track if the corresponding <i>startScan</i> has been called before.
  */
 static void stopScan(Management::DataReceiver_ptr writer,bool& writerError,bool& scanStarted) throw (ComponentErrors::OperationErrorExImpl);

 /**
  * This static method inquiries the DataReceiver to check if the data transfer is still active or not.
  * @param writer reference to the writer or data recorder or data dealer
  * @param writerError will be returned back true if an error occurred in the communication to writer component
  * @return true if the recording is still going on
  */
 static bool checkRecording(Management::DataReceiver_ptr writer,bool& writerError) throw (ComponentErrors::OperationErrorExImpl);

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
 * @param lst local sidereal time
 * @param prj name of the project
 * @param suffix string that will be appended to the end of file name
 * @param extra return the extra path
 * @return the name of the file to be created
*/
static IRA::CString computeOutputFileName(const ACS::Time& ut,const ACS::TimeInterval& lst,const IRA::CString& prj,const IRA::CString& suffix,IRA::CString& extra);

/**
 * computes the scan axis starting form the scan parameters
 * @param type type of the scan
 * @param scanRec structure that contains all the scan parameters
 * @return the scanning axis according to the definition of the management interface
 */
/*static Management::TScanAxis computeScanAxis(const Management::TScanTypes& type,const Schedule::CScanList::TRecord& scanRec);*/

/**
 * Computes the scan axis starting from the configuration of involved sub system bosses.
 * @param antBoss reference to the antenna boss
 * @param antBossErr flags the status of the antenna boss reference
 * @throw ComponentErrors::ComponentNotActiveExImpl
 * @throw ComponentErrors::CORBAProblemExImpl
 * @throw ComponentErrors::UnexpectedExImpl
 */
static Management::TScanAxis computeScanAxis(Antenna::AntennaBoss_ptr antBoss,bool& antBossError) throw (ComponentErrors::ComponentNotActiveExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl);

/**
 * Computes the name of the output file, lst time is derived from provided ut time
 * @param ut universal time
 * @param site information in order to compute local sidereal time
 * @param dut1 difference UT-UT1 in order to compute local sidereal time
 * @param prj name of the project
 * @param suffix string that will be appended to the end of file name
 * @param extra return the extra path
 * @return the name of the file to be created
*/	
static IRA::CString computeOutputFileName(const ACS::Time& ut,const IRA::CSite& site,const double& dut1,const IRA::CString& prj,const IRA::CString& suffix,IRA::CString& extra);

/**
 * This is the handler of the events coming from the notification channel published by the Antenna subsystem
 */
static void antennaNCHandler(Antenna::AntennaDataBlock antennaData,void *handlerParam);

/**
 * used as wrapper to function that are in charge of forwarding commands to other packages  
*/
bool remoteCall(const IRA::CString& command,const IRA::CString& package,const long& par,IRA::CString& out) throw (ParserErrors::PackageErrorExImpl,ManagementErrors::UnsupportedOperationExImpl);



#endif /*CORE_COMMON_H_*/

