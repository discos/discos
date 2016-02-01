#ifndef CORE_COMMON_H_
#define CORE_COMMON_H_

#define COMMON_INIT

//some common, general use operations, the changes caused by these methods do not have influence on the status of the component
// and do not have direct interface to RAL.


/**
 *
 */

/**
 * Check the scan against the telescope whether it could be executed or not. The result could depend also on  scan type, instrument configuration and so on.
 * @param ut required start time for the scan, it could be zero meaning start as soon as possible. As output argument it returns the true start time
 * @param prim primary scan configuration for the subscan (antenna)
 * @param sec primary scan configuration for the subscan (antenna)
 * @param servoPar structure containing the scan configuration for the minor servo system
 * @param recvPar structure containing the scan configuration for the receivers subsystem.
 * @param minEl elevation lower limit
 * @param maxEl elevation upper limit
 * @throw ComponentErrors::UnexpectedExImpl
 * @throw ComponentErrors::OperationErrorExImpl
 * @throw ComponentErrors::ComponentNotActiveExImpl
 * @throw ComponentErrors::CORBAProblemExImpl
 * @throw ComponentErrors::CouldntGetComponentExImpl
 * @throw ManagementErrors::UnsupportedOperationExImpl
 * @return true if the scan has been checked successfully
 */
bool checkScan(ACS::Time& ut,const Antenna::TTrackingParameters *const prim,const Antenna::TTrackingParameters *const sec,
	const MinorServo::MinorServoScan*const servoPar, const Receivers::TReceiversParameters*const recvPar,const double& minEl=-1.0,
	const double& maxEl=-1.0) throw (ComponentErrors::UnexpectedExImpl,
	ComponentErrors::OperationErrorExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,
	ComponentErrors::CouldntGetComponentExImpl,ManagementErrors::UnsupportedOperationExImpl);

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
 * @param servoPar scan configuration of the servo subsystem.
 * @param recvPar structure containing the scan configuration for the receivers subsystem.
 * @param subScanConf structure containing the sub scan configuration
*/
void doScan(ACS::Time& ut,const Antenna::TTrackingParameters* const prim,const Antenna::TTrackingParameters* const sec,
	const MinorServo::MinorServoScan*const servoPar,const Receivers::TReceiversParameters*const recvPa,
	const Management::TSubScanConfiguration& subScanConf) throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,
	ComponentErrors::UnexpectedExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CouldntGetComponentExImpl);

/*
 * starts data recording at a given time, other arguments are required in order to prepare the acquisition
 * @param recUt start time in UT
 * @return the time tag at which the recording effectively took place.
 */
ACS::Time startRecording(const ACS::Time& recUt,const long& scanId, const long& subScanId, const long& scanTag,const IRA::CString& basePath,
		const IRA::CString& suffix,const IRA::CString observerName, const IRA::CString& projectName, const IRA::CString& scheduleFileName,
		const IRA::CString& layout, const ACS::stringSeq& layoutProc )  throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ManagementErrors::BackendNotAvailableExImpl, ManagementErrors::DataTransferSetupErrorExImpl,
		ComponentErrors::ComponentNotActiveExImpl, ComponentErrors::CouldntGetComponentExImpl);

/*
 * This method is is the wrap of both <i>checkScan()<i> and the <i>doScan()<i> methods, it takes all the actions to start a subscan.
 * @throw ManagementsErrors::TelescopeSubScanErrorExImpl
 * @throw ManagementsErrors::TargetOrSubscanNotFeasibleImpl
 * @thorw ManagementErrors::CloseTelescopeScanErrorExImpl
 * @param ut required start time for the scan, it could be zero meaning start as soon as possible. As output argument it returns the true start time
 * @param prim primary scan configuration for the subscan (antenna)
 * @param sec primary scan configuration for the subscan (antenna)
 * @param servoPar scan configuration of the servo subsystem.
 * @param recvPar structure containing the scan configuration for the receivers subsystem.
 * @param subScanConf structure containing the sub scan configuration
*/
void startScan(ACS::Time& time,const Antenna::TTrackingParameters *const prim,const Antenna::TTrackingParameters *const sec,
	const MinorServo::MinorServoScan*const servoPar, const Receivers::TReceiversParameters*const recvPar,
	const Management::TSubScanConfiguration& subScanConf) throw (
	ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl,
	ManagementErrors::CloseTelescopeScanErrorExImpl);

/*
 * Sends the Antenna off source for a given number of beams sizes. It calls the corresponding method of the antenna subsystem
 * @param frame reference frame
 * @param beams number of beams sizes to be applied as offset
 */
void goOff(const Antenna::TCoordinateFrame& frame,const double& beams) throw (ComponentErrors::CouldntGetComponentExImpl,
		ComponentErrors::ComponentNotActiveExImpl,ManagementErrors::AntennaScanErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl);

/**
 * This method issues a close to the current subscan and in case waits for this operation to complete
 * @param wait true if the methods has to wait for the operation to complete in all the subsystems
 * @return the time requested by the telescope to close the scan in all subsystems (not a duration but a precise time)
 */
ACS::Time closeScan(bool wait) throw (ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::OperationErrorExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::TimerErrorExImpl,ManagementErrors::AbortedByUserExImpl);

/**
 * Computes the scan axis starting from the current configuration of involved sub system bosses.
 * @throw ComponentErrors::ComponentNotActiveExImpl
 * @throw ComponentErrors::CORBAProblemExImpl
 * @throw ComponentErrors::UnexpectedExImpl
 * @throw ComponentErrors::CouldntGetComponentExImpl
 * @throw ComponentErrors::OperationErrorExImpl
 */
Management::TScanAxis computeScanAxis() throw (ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::OperationErrorExImpl);

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

/**
 * This is not thread safe but we can consider it almost atomic.
 * It clear the tracking flag, issued when a new scan is commanded in order to prevent the scheduler to consider the tracking when it is not the case
 */
inline void clearAntennaTracking() { m_isAntennaTracking=false; }

/**
 * This is not thread safe but we can consider it almost atomic.
 * It clear the tracking flag, issued when a new scan is commanded in order to prevent the scheduler to consider the tracking when it is not the case
 */
inline void clearReceiversTracking() { m_isReceiversTracking=false; }

/**
 * This is not thread safe but we can consider it almost atomic.
 * It clear the tracking flag from minor servo, issued when a new scan is commanded in order to prevent the scheduler to consider the tracking when it is not the case
 */
inline void clearMinorServoTracking() { if (MINOR_SERVO_AVAILABLE) m_isMinorServoTracking=false; }

/**
 * This is not thread safe but we can consider it almost atomic.
 * This method will cause the tracking flag to be false for a certain amount of time.
 */
void clearTracking();

/**
 * It allows to change the current status of the scheduler.
 * @param status new status of the scheduler
 */
 void changeSchedulerStatus(const Management::TSystemStatus& status);

/**
 * check is the stream or data recording has already started or not
 */
bool isStreamStarted() const;


#endif /*CORE_COMMON_H_*/

