#ifndef CORE_BASIC_H_
#define CORE_BASIC_H_

//Some basic operations, they are called by other methods as APIs.

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
 * @param config pointer to the configuration object
 */
static void setupDataTransfer(bool& scanStarted,/*bool& streamPrepared,*/Management::DataReceiver_ptr writer,bool& writerError,Backends::GenericBackend_ptr backend,bool& backendError,
		const IRA::CString& obsName,const IRA::CString& prj,const IRA::CString& baseName,const IRA::CString& path,const IRA::CString& extraPath,const IRA::CString& schedule,const IRA::CString& targetID,
		const IRA::CString& layoutName,const ACS::stringSeq& layout,const long& scanTag,const long& device,const DWORD& scanID,const ACS::Time& startTime,const  DWORD& subScanID,
		const Management::TScanAxis& axis,const CConfiguration* config) throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::UnexpectedExImpl);

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



#endif /* CORE_BASIC_H_ */
