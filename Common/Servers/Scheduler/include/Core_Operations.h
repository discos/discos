#ifndef CORE_OPERATIONS_H_
#define CORE_OPERATIONS_H_

//list of operations and commands that are directly exposed by the component interface, and are mapped to RAL commands

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

/*
 * Starts a longitude OnTheFly scan
 * @param scanFrame select which frame is going to be scanned
 * @param span length of the scan
 * @param span duration of the scan
 */
void lonOTF(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration) throw (
		ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl);

/*
 * Starts a latitude OnTheFly scan
 * @param scanFrame select which frame is going to be scanned
 * @param span length of the scan
 * @param span duration of the scan
 */
void latOTF(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration) throw (
		ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl);


/**
 * It changes the current device, first it checks if the corresponding section is existent in the current backend (default backend if no schedule is running, the schedule backend if a schedule runs).
 * The the section configuration is read and eventually a new beamsize is computed also using the current configuration of the receiver.
 * @param deviceID identifier of the section (device) of the current backend. If negative, the current value is not changed.
 */
void setDevice(const long& deviceID) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::ValidationErrorExImpl,
		ComponentErrors::OperationErrorExImpl,ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl);

/**
 * It allows to set a new project code. If requested by the component configuration (<i>CheckProjectCode</i>)  the project is checked to be registered in the system.
 * If not present an error is thrown. The check consist in verifying a folder named "code" exists in <i>SchedDir</i> of the configuration.
 * @param code new project code
 * @throw ManagementErrors::UnkownProjectCodeErrorExImpl
 */
void setProjectCode(const char* code) throw (ManagementErrors::UnkownProjectCodeErrorExImpl);

/**
 * called to set proper values for the rest frequency
 * @param in new values
 */
void setRestFrequency(const ACS::doubleSeq& in);

/**
 * This method stops the current schedule, if one is running. If no schedule is active at tehe moment it takes
 * no action.
 * This function is thread safe.
 */
void stopSchedule();

/**
 * This method halts the current schedule, if one is runnig. If no schedule is active at the moment it takes
 * no action. The active schedule is halted after the currently running scan is completed.
 * This function is thread safe.
 */
void haltSchedule();

/**
 * This method loads a schedule file and starts the execution of the schedule form the given line.
 * This function is thread safe.
 * @param scheduleFile string that reports the file name of the schedule to load
 * @param startSubScan subscan of the schedule to start from
 * @throw ComponentErrors::MemoryAllocationExImpl
 * @throw ManagementErrors::ScheduleErrorExImpl
 * @throw ComponentErrors::AlreadyRunningExImpl
 * @throw ManagementErrors::SubscanErrorExImpl
 * @throw ComponentErrors::CouldntGetComponentExImpl
 * @throw ManagementErrors::LogFileErrorExImpl
*/
void startSchedule(const char* scheduleFile,const char * startSubScan) throw (ManagementErrors::ScheduleErrorExImpl,
		ManagementErrors::AlreadyRunningExImpl,ComponentErrors::MemoryAllocationExImpl,ManagementErrors::SubscanErrorExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ManagementErrors::LogFileErrorExImpl);

/**
 * It allows to change the backend elected as default backend, the default backend is the device used for all operation (for example tsys) when a schedule is not running.
 * @param bckInstance name of the instance of the backend that has to be placed as default backend
 */
void chooseDefaultBackend(const char *bckInstance) throw (ComponentErrors::CouldntGetComponentExImpl);

/**
 * It allows to change the component elected as default data receiver.
 * @param rcvInstance name of the instance of the data receiver component
 */
void chooseDefaultDataRecorder(const char *rcvInstance) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl);

/**
 * Allows to change the name of the current log file.
 * @param fileName new file name.
 * @param ComponentErrors::CouldntGetComponentExImpl
 * @param ComponentErrors::CORBAProblemExImpl
 * @param ManagementErrors::LogFileErrorExImpl
 */
void changeLogFile(const char *fileName) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ManagementErrors::LogFileErrorExImpl);

/**
 * Allows to read out from weather station.
 * @throw ComponentErrors::CouldntGetComponentExImpl
 * @throw ManagementErrors::WeatherStationErrorExImpl
 * @throw ComponentErrors::CORBAProblemExImpl
 */
void getWeatherStationParameters(double &temp,double& hum,double& pres, double& wind)  throw (ComponentErrors::CouldntGetComponentExImpl,ManagementErrors::WeatherStationErrorExImpl,ComponentErrors::CORBAProblemExImpl);

/**
 * initialize the writing of the data
 * @param scaid identifier of the scan
 */
void initRecording(const long& scanid) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl);

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
 * @param duration recording time
 */
void startRecording(const long& subScanId,const ACS::TimeInterval& duration) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::ComponentNotActiveExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,
		ManagementErrors::DataTransferSetupErrorExImpl,ComponentErrors::TimerErrorExImpl);

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

#endif /* CORE_OPERATIONS_H_ */
