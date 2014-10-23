#ifndef CORE_OPERATIONS_H_
#define CORE_OPERATIONS_H_

/**
 * Make the current thread wait for the given number of seconds. It can be used stop execution.
 * @param seconds number of seconds to wait, expressed as integer number plus fraction of seconds.
*/
void wait(const double& seconds) const;

/**
 * make the current thread wait until a given time
 * @param time time to wait for
 */
void waitUntil(const ACS::Time& time) throw (ComponentErrors::TimerErrorExImpl);

/**
 * The implementation of No Operation command. It does nothing
 */
void nop() const;

/**
 * Make the current thread wait for the tracking flag to be true. It can be used to stop the schedule execution until the telescope is tracking
 */
void waitOnSource() const;

/**
 * initialize the writing of the data
 * @param scaid identifier of the scan
 */
void initRecording(const long& scanid) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl);

/*
 * starts data recording at a given time, other arguments are required in order to prepare the acquisition
 * @param recUt start time in UT
 */
ACS::Time startRecording(const ACS::Time& recUt,const long& scanId, const long& subScanId, const long& scanTag,const IRA::CString& basePath,
														 const IRA::CString& suffix,const IRA::CString observerName, const IRA::CString& projectName, const IRA::CString& scheduleFileName,
														 const IRA::CString& layout, const ACS::stringSeq& layoutProc )  throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,
															  ComponentErrors::UnexpectedExImpl,ManagementErrors::BackendNotAvailableExImpl, ManagementErrors::DataTransferSetupErrorExImpl,
															  ComponentErrors::ComponentNotActiveExImpl, ComponentErrors::CouldntGetComponentExImpl);

/**
 * Start the recording at the given time
 * @param startTime epoch when the recording is started
 * @param subScanId identifier of the subscan
 */
void startRecording(const ACS::Time& startTime,const long& subScanId) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::ComponentNotActiveExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,
		ManagementErrors::DataTransferSetupErrorExImpl);

/**
 * Starts the recording immediately. It is used to be called by RAL
 * @param subScanId identifier of the subscan
 */
void startRecording(const long& subScanId) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::ComponentNotActiveExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,
		ManagementErrors::DataTransferSetupErrorExImpl);

/**
 * Immediately stops the data recording
 */
void stopRecording() throw (ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,ComponentErrors::CouldntGetComponentExImpl,
		ComponentErrors::UnexpectedExImpl);

/**
 * Performs the required operation to close the current scan, including the data acquisition
 * @param ComponentErrors::OperationErrorExImpl
 * @param ComponentErrors::CORBAProblemExImpl
 * @param ComponentErrors::UnexpectedExImpl
 * @param ComponentErrors::CouldntGetComponentExImpl
 */
void terminateScan() throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl);

/**
 * Check the scan against the telescope whether it could be executed or not. The result could depend also on  scan type, instrument configuration and so on.
 * @param ut required start time for the scan, it could be zero meaning start as soon as possible. As output argument it returns the true start time
 * @param prim primary scan configuration for the subscan (antenna)
 * @param sec primary scan configuration for the subscan (antenna)
 * @param recvPar structure containing the scan configuration for the receivers subsystem.
 * @param minEl elevation lower limit
 * @param maxEl elevation upper limit
 * @throw ComponentErrors::UnexpectedExImpl
 * @throw ComponentErrors::OperationErrorExImpl
 * @throw ComponentErrors::ComponentNotActiveExImpl
 * @throw ComponentErrors::CORBAProblemExImpl
 * @thorw ComponentErrors::CouldntGetComponentExImpl
 * @return true if the scan has been checked successfully
 */
bool checkScan(ACS::Time& ut,const Antenna::TTrackingParameters *const prim,const Antenna::TTrackingParameters *const sec,
	const Receivers::TReceiversParameters*const recvPar,const double& minEl,const double& maxEl) throw (ComponentErrors::UnexpectedExImpl,
	ComponentErrors::OperationErrorExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,
	ComponentErrors::CouldntGetComponentExImpl);

/**
 * Send to the telescope the commands required to start scan.
 * @throw ComponentErrors::OperationErrorExImpl
 * @throw ComponentErrors::CORBAProblemExImpl
 * @throw ComponentErrors::UnexpectedExImpl
 * @throw ComponentErrors::ComponentNotActiveExImpl
 * @throw ComponentErrors::CouldntGetComponentExImpl
 * @param ut required start time for the scan, it could be zero meaning start as soon as possible. As output argument it returns the true start time
 * @param prim primary scan configuration for the subscan (antenna)
 * @param sec primary scan configuration for the subscan (antenna)
 * @param recvPar structure containing the scan configuration for the receivers subsystem. It may be modified with proper values by the routine.
*/
void doScan(ACS::Time& ut,const Antenna::TTrackingParameters * const prim,const Antenna::TTrackingParameters *const sec,const Receivers::TReceiversParameters& recvPa)
	throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::ComponentNotActiveExImpl,
	ComponentErrors::CouldntGetComponentExImpl);

/**
 * Computes the scan axis starting from the current configuration of involved sub system bosses.
 * @throw ComponentErrors::ComponentNotActiveExImpl
 * @throw ComponentErrors::CORBAProblemExImpl
 * @throw ComponentErrors::UnexpectedExImpl
 * @throw ComponentErrors::CouldntGetComponentExImpl
 */
Management::TScanAxis computeScanAxis() throw (ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::CouldntGetComponentExImpl);

/**
 * This method will close (if necessary) the established connection between a backend and a data receiver.It closes and frees the resources allocated to enable the trasmission. In practice it calls sendStop()
 * terminate(), disconnect() and closeReceiver()
 * @throw ComponentErrors::OperationErrorExImpl
 * @throw ComponentErrors::CORBAProblemExImpl
 * @throw ComponentErrors::UnexpectedExImpl
 * @throw ComponentErrors::CouldntGetComponentExImpl);
 */
void disableDataTransfer() throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::CouldntGetComponentExImpl);

/**
 * It allows to send a configuration procedure to a backend.
 * @param name name of the procedure
 * @param procedure sequence of commands contained by the configuration procedure
 * @throw ManagementErrors::BackendProcedureErrorExImpl
 * @throw ComponentErrors::CORBAProblemExImpl
 * @throw ComponentErrors::UnexpectedExImpl
 * @throw ComponentErrors::CouldntGetComponentExImpl
 */
void configureBackend(const IRA::CString& name,const std::vector<IRA::CString>& procedure) throw (ManagementErrors::BackendProcedureErrorExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl);

/**
 * Enable the transfer between the backend and the receiver. In practice it connects the backend with the receiver itself
 * @throwComponentErrors::OperationErrorExImpl
 * @throw ComponentErrors::CORBAProblemExImpl
 * @throw ComponentErrors::UnexpectedExImpl
 * @throw ComponentErrors::CouldntGetComponentExImpl
 */
void enableDataTransfer() throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::CouldntGetComponentExImpl);

/**
 * It stops the data transfer between the backend and the configured data recorder.
 * @throw ComponentErrors::OperationErrorExImpl
 * @throw ManagementErrors::BackendNotAvailableExImpl
 * @throw ComponentErrors::CouldntGetComponentExImpl
 */
void stopDataTransfer() throw (ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,ComponentErrors::CouldntGetComponentExImpl);

/**
 * This static method inquiries the DataReceiver to check if the data transfer is still active or not.
 * @throw ComponentErrors::OperationErrorExImpl
 * @throw ComponentErrors::UnexpectedExImpl
 * @throw ComponentErrors::CouldntGetComponentExImpl
 * @return true if the recording is still going on
 */
bool checkRecording() throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl);



#endif /* CORE_OPERATIONS_H_ */
