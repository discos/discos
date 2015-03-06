#ifndef CORE_OPERATIONS_H_
#define CORE_OPERATIONS_H_

//list of operations and commands that are directly exposed by the component interface, and are mapped to RAL commands

/**
 * Make the current thread wait for the given number of seconds.
 * @param seconds number of seconds to wait, expressed as integer number plus fraction of seconds.
*/
void _wait(const double& seconds) throw (ComponentErrors::TimerErrorExImpl,ManagementErrors::AbortedByUserExImpl);

/**
 * make the current thread wait until a given time
 * @param time time to wait for
 */
void _waitUntil(const ACS::Time& time) throw (ComponentErrors::TimerErrorExImpl,ManagementErrors::AbortedByUserExImpl);

/**
 * Make the current thread wait for the telescope to reach the target.
 */
void _waitOnSource() throw (ManagementErrors::AbortedByUserExImpl);

/**
 * Make the current thread wait for the tracking flag to be true.
 */
void _waitTracking() throw (ManagementErrors::AbortedByUserExImpl);

/**
 * forces the abort of current (in any) long running operations. If no operation is still ongoing it does nothing.
 */
void _abort() throw (ManagementErrors::NotAllowedDuringScheduleExImpl);

/**
 * The implementation of No Operation command. It does nothing
 */
void _nop() const;

/*
 * Starts a longitude OnTheFly scan
 * @param scanFrame select which frame is going to be scanned
 * @param span length of the scan
 * @param span duration of the scan
 */
void _lonOTF(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration) throw (
		ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl,
		ManagementErrors::CloseTelescopeScanErrorExImpl);

/*
 * Starts a latitude OnTheFly scan
 * @param scanFrame select which frame is going to be scanned
 * @param span length of the scan
 * @param span duration of the scan
 */
void _latOTF(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration) throw (
		ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl,
		ManagementErrors::CloseTelescopeScanErrorExImpl);

/**
 * Starts a peaker operation, which is indeed maximizing the focusing or in general the position of the subreflector.
 * @param axis name of the axis to be maximized
 * @param span range to be traveled by the axis
 * @param duration total allotted time
 */
void _peaker(const char *axis,const double& span,const ACS::TimeInterval& duration) throw (
		ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl,
		ManagementErrors::CloseTelescopeScanErrorExImpl,ComponentErrors::ValidationErrorExImpl);

/**
 * Starts a skydip OnTheFly scan
 * @param el1 first elevation border of the scan
 * @param el2 second elevation border of the scan
 * @param duration duration of the scan
 */
void _skydipOTF(const double& el1,const double& el2,const ACS::TimeInterval& duration) throw (
		ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl,
		ManagementErrors::CloseTelescopeScanErrorExImpl);

/**
 * start the tracking of a source form its catalog identifier
 * @param name of the target
 */
void _track(const char *targetName) throw (ManagementErrors::TelescopeSubScanErrorExImpl,
		ManagementErrors::TargetOrSubscanNotFeasibleExImpl,ManagementErrors::CloseTelescopeScanErrorExImpl);

/**
 * starts the tracking of the moon
 */
void _moon() throw (ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl,
		ManagementErrors::CloseTelescopeScanErrorExImpl);

/**
 * Starts a sidereal tracking of a source from equatorial coordinates
 * @param targetName name of the source
 * @param ra right ascension
 * @param dec declination
 * @param eq reference equinox of equatorial coordinate
 * @param section required section of the azimuth range
 */
void _sidereal(const char * targetName,const double& ra,const double& dec,const Antenna::TSystemEquinox& eq,const Antenna::TSections& section) throw (
	ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl,
	ManagementErrors::CloseTelescopeScanErrorExImpl);


/**
 * It sends the telescope to fixed position in the horizontal frame
 */
void _goTo(const double& azimuth,const double& elevation) throw (ManagementErrors::TelescopeSubScanErrorExImpl,
		ManagementErrors::TargetOrSubscanNotFeasibleExImpl,ManagementErrors::CloseTelescopeScanErrorExImpl);

/**
 * It computes the system temperature.
 * @param ComponentErrors::CouldntGetComponentExImpl
 * @param ComponentErrors::CORBAProblemExImpl
 * @param ComponentErrors::OperationErrorExImpl
 * @param ComponentErrors::CouldntReleaseComponentExImpl
 * @param  tsys list of the tsys given as output, one for each channel of the currently selected backend.
 * */
void _callTSys(ACS::doubleSeq& tsys) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::OperationErrorExImpl,
		ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl);

/**
 * It changes the current device, first it checks if the corresponding section is existent in the current backend (default backend if no schedule is running, the schedule backend if a schedule runs).
 * The the section configuration is read and eventually a new beamsize is computed also using the current configuration of the receiver.
 * @param deviceID identifier of the section (device) of the current backend. If negative, the current value is not changed.
 */
void _setDevice(const long& deviceID) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::ValidationErrorExImpl,
		ComponentErrors::OperationErrorExImpl,ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl);

/**
 * It allows to set a new project code. If requested by the component configuration (<i>CheckProjectCode</i>)  the project is checked to be registered in the system.
 * If not present an error is thrown. The check consist in verifying a folder named "code" exists in <i>SchedDir</i> of the configuration.
 * @param code new project code
 * @throw ManagementErrors::UnkownProjectCodeErrorExImpl
 */
void _setProjectCode(const char* code) throw (ManagementErrors::UnkownProjectCodeErrorExImpl);

/**
 * called to set proper values for the rest frequency
 * @param in new values
 */
void _setRestFrequency(const ACS::doubleSeq& in);

/**
 * This method stops the current schedule, if one is running. If no schedule is active at tehe moment it takes
 * no action.
 * This function is thread safe.
 */
void _stopSchedule();

/**
 * This method halts the current schedule, if one is runnig. If no schedule is active at the moment it takes
 * no action. The active schedule is halted after the currently running scan is completed.
 * This function is thread safe.
 */
void _haltSchedule();

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
void _startSchedule(const char* scheduleFile,const char * startSubScan) throw (ManagementErrors::ScheduleErrorExImpl,
		ManagementErrors::AlreadyRunningExImpl,ComponentErrors::MemoryAllocationExImpl,ManagementErrors::SubscanErrorExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ManagementErrors::LogFileErrorExImpl);

/**
 * It allows to change the backend elected as default backend, the default backend is the device used for all operation (for example tsys) when a schedule is not running.
 * @param bckInstance name of the instance of the backend that has to be placed as default backend
 */
void _chooseDefaultBackend(const char *bckInstance) throw (ComponentErrors::CouldntGetComponentExImpl);

/**
 * It allows to change the component elected as default data receiver.
 * @param rcvInstance name of the instance of the data receiver component
 */
void _chooseDefaultDataRecorder(const char *rcvInstance) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl);

/**
 * Allows to change the name of the current log file.
 * @param fileName new file name.
 * @param ComponentErrors::CouldntGetComponentExImpl
 * @param ComponentErrors::CORBAProblemExImpl
 * @param ManagementErrors::LogFileErrorExImpl
 */
void _changeLogFile(const char *fileName) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ManagementErrors::LogFileErrorExImpl);

/**
 * Allows to read out from weather station.
 * @throw ComponentErrors::CouldntGetComponentExImpl
 * @throw ManagementErrors::WeatherStationErrorExImpl
 * @throw ComponentErrors::CORBAProblemExImpl
 */
void _getWeatherStationParameters(double &temp,double& hum,double& pres, double& wind)  throw (ComponentErrors::CouldntGetComponentExImpl,ManagementErrors::WeatherStationErrorExImpl,ComponentErrors::CORBAProblemExImpl);

/**
 * initialize the writing of the data
 * @param scaid identifier of the scan
 */
void _initRecording(const long& scanid) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ManagementErrors::NotAllowedDuringScheduleExImpl);

/**
 * Start the recording at the given time
 * @param startTime epoch when the recording is started
 * @param subScanId identifier of the subscan
 */
/*void startRecording(const ACS::Time& startTime,const long& subScanId) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::ComponentNotActiveExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,
		ManagementErrors::DataTransferSetupErrorExImpl);*/

/**
 * Starts the recording immediately. It is used to be called by RAL
 * @param subScanId identifier of the subscan
 * @param duration recording time
 */
void _startRecording(const long& subScanId,const ACS::TimeInterval& duration) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::ComponentNotActiveExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,
		ManagementErrors::DataTransferSetupErrorExImpl,ComponentErrors::TimerErrorExImpl,ManagementErrors::AbortedByUserExImpl,
		ManagementErrors::NotAllowedDuringScheduleExImpl,ManagementErrors::RecordingAlreadyActiveExImpl);

/**
 * Immediately stops the data recording
 */
/*void _stopRecording() throw (ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,ComponentErrors::CouldntGetComponentExImpl,
		ComponentErrors::UnexpectedExImpl);*/

/**
 * Performs the required operation to close the current scan, including the data acquisition
 * @param ComponentErrors::OperationErrorExImpl
 * @param ComponentErrors::CORBAProblemExImpl
 * @param ComponentErrors::UnexpectedExImpl
 * @param ComponentErrors::CouldntGetComponentExImpl
 */
void _terminateScan() throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl);

#endif /* CORE_OPERATIONS_H_ */
