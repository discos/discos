void CCore::wait(const double& seconds) const
{
	int sec;
	long micro;
	sec=(int)seconds;
	micro=(long)((seconds-(double)sec)*1000000.0);
	IRA::CIRATools::Wait(sec,micro);
}

void CCore::waitUntil(const ACS::Time& time) throw (ComponentErrors::TimerErrorExImpl)
{
	long done=0;
	if (!addTimerEvent(time,waitUntilHandler,static_cast<void *>(&done))) {
		//errore
		_EXCPT(ComponentErrors::TimerErrorExImpl,dummy,"CCore::waitUntil()");
		dummy.setReason("timer event could not be scheduled");
		throw dummy;
	}
	while (!done) {
		IRA::CIRATools::Wait(25000); // 25 milliseconds
	}
}

void CCore::nop() const
{
	return;
}

void CCore::waitOnSource() const
{
	while (!isTracking()) {
		wait(0.1);
	}
}

void CCore::lonOTF(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration) throw (
		ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	ACS::Time startTime=0; // start asap
	Antenna::TTrackingParameters primary,secondary;
	MinorServo::MinorServoScan servo;
	Receivers::TReceiversParameters receievers;
	Schedule::CSubScanBinder binder(&primary,&secondary,&servo,&receievers);
	binder.lonOTF(scanFrame,span,duration);
	startTime=0; // it means start as soon as possible
	startScan(startTime,&primary,&secondary,&servo,&receievers); //ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl
	m_subScanEpoch=startTime;
}

void CCore::latOTF(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration) throw (
		ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	ACS::Time startTime=0; // start asap
	Antenna::TTrackingParameters primary,secondary;
	MinorServo::MinorServoScan servo;
	Receivers::TReceiversParameters receievers;
	Schedule::CSubScanBinder binder(&primary,&secondary,&servo,&receievers);
	binder.latOTF(scanFrame,span,duration);
	startTime=0; // it means start as soon as possible
	startScan(startTime,&primary,&secondary,&servo,&receievers); //ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl
	m_subScanEpoch=startTime;
}

void CCore::chooseDefaultBackend(const char *bckInstance) throw (ComponentErrors::CouldntGetComponentExImpl)
{
	//************************************************************** /
	/* It should be forbidden is a schedule is running or recording is active */
	/* Also the check that the backend is available must be done */
	/* *****************************************************************/
	baci::ThreadSyncGuard guard(&m_mutex);
	IRA::CString instance(bckInstance);
	if (m_defaultBackendInstance!=instance) {
		m_defaultBackendInstance=instance;
		m_defaultBackendError=true;  // this is tricky...in order to force to unload the preset backend and then reload the new one the next time the default backend is required
		loadDefaultBackend();  //throw (ComponentErrors::CouldntGetComponentExImpl)
	}
}

void CCore::chooseDefaultDataRecorder(const char *rcvInstance) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl)
{
	//************************************************************** /
	/* It should be forbidden is a schedule is running or recording is active */
	/* Also the check that the backend is available must be done */
	/* *****************************************************************/
	baci::ThreadSyncGuard guard(&m_mutex);
	IRA::CString instance(rcvInstance);
	if (m_defaultDataReceiverInstance!=instance) {
		m_defaultDataReceiverInstance=instance;
		m_defaultDataReceiverError=true;  // this is tricky...in order to force to unload the preset data recorder and then reload the new one the next time the default data recorder is required
		loadDefaultDataReceiver(); // throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl)
	}
}

void CCore::changeLogFile(const char *fileName) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ManagementErrors::LogFileErrorExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	loadCustomLogger(m_customLogger,m_customLoggerError); // throw ComponentErrors::CouldntGetComponentExImpl
	IRA::CString fullName,fullSysName;
	IRA::CString logName(fileName);
	fullName=logName+".log";
	fullSysName=logName+".xml";
	ACS_LOG(LM_FULL_INFO,"CCore::changeLogFile()",(LM_NOTICE,"NEW_LOG_FILE: %s",(const char *)fullName));
	try {
		m_customLogger->flush();
		m_customLogger->setLogfile((const char *)m_config->getLogDirectory(),
																	 (const char *)m_config->getSystemLogDirectory(),
																	 (const char *)fullName,
																	 (const char *)fullSysName);
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::changeLogFile()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_customLoggerError=true;
		throw impl;
	}
	catch (ManagementErrors::ManagementErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::LogFileErrorExImpl,impl,ex,"CCore::changeLogFile()");
		impl.setFileName((const char *)fullName);
		throw impl;
	}
}

void CCore::getWeatherStationParameters(double &temp,double& hum,double& pres, double& wind) throw (ComponentErrors::CouldntGetComponentExImpl,
		ManagementErrors::WeatherStationErrorExImpl,ComponentErrors::CORBAProblemExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	loadWeatherStation(m_weatherStation,m_weatherStationError); // throw ComponentErrors::CouldntGetComponentExImpl
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	try {
		if (now.value().value>m_lastWeatherTime+100000000) {  // hard coded for now...ten seconds
			m_weatherPar=m_weatherStation->getData();
		}
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::getWeatherStationParameters()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_weatherStationError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ManagementErrors::WeatherStationErrorExImpl,impl,"CCore::getWeatherStationParameters()");
		throw impl;
	}
	temp=m_weatherPar.temperature;
	hum=m_weatherPar.humidity;
	pres=m_weatherPar.pressure;
	wind=m_weatherPar.wind;
}

void CCore::initRecording(const long& scanid) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,
		ComponentErrors::CORBAProblemExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	//************************************************************************************************************************
	//*********************CHECK if a schedule is running or if already recording in that case we must give up
	//************************************************************************************************************************
	try {
		CCore::disableDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_defaultDataReceiver.in(),m_defaultDataReceiverError,m_streamStarted,m_streamPrepared,m_streamConnected,m_scanStarted);
	}
	catch (...) {
		m_streamStarted=m_streamPrepared=m_streamConnected=m_scanStarted=false;
		// keep it up, no need to give up now......
	}
	m_scanID=0; m_subScanID=0;
	m_subScanEpoch=0;
	//now load the Backend and the dataReceiver
	// we always load the default backend because this method could not be called when a schedule is running so, for sure, there will not be a schedule backend
	loadDefaultBackend(); //throw (ComponentErrors::CouldntGetComponentExImpl);
	loadDefaultDataReceiver(); //throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl)
	// ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl
	CCore::enableDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_defaultDataReceiver.in(),m_streamConnected,m_streamPrepared);
	m_scanID=scanid;
}

void CCore::startRecording(const long& subScanId,const ACS::TimeInterval& duration) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::ComponentNotActiveExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,
		ManagementErrors::DataTransferSetupErrorExImpl,ComponentErrors::TimerErrorExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	//************************************************************************************************************************
	//*********************CHECK if a schedule is running or if the  recording is not initialized (streamConnected and streamPrepared) in that case we must give up
	//************************************************************************************************************************
	ACS::Time realStart,waitFor;
	IRA::CString prj,path,suffix,obsName,schedule,layoutName;
	long scanTag;
	ACS::stringSeq layout;
	//no mutex below.....
	guard.release();
	if (m_subScanEpoch==0) {
		waitOnSource();
	}
	scanTag=0; suffix=""; obsName=IRA::CString("system"); getProjectCode(prj); schedule="none";
	layout.length(0); layoutName=_SCHED_NULLTARGET;
	guard.acquire();
	m_subScanID=subScanId;
	realStart=startRecording(m_subScanEpoch,m_scanID,m_subScanID,scanTag,m_config->getSystemDataDirectory(),suffix,obsName,prj,schedule,layoutName,layout);
	waitFor=realStart+duration; // this is the time at which the stop should be issued
	guard.release();
	waitUntil(waitFor); // throw ComponentErrors::TimerErrorExImpl
}

void CCore::startRecording(const ACS::Time& startTime,const long& subScanId) throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		  ComponentErrors::UnexpectedExImpl,ManagementErrors::BackendNotAvailableExImpl, ManagementErrors::DataTransferSetupErrorExImpl,
		  ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	//************************************************************************************************************************
	//*********************CHECK if a schedule is running or if the  recording is not initialized (streamConnected and streamPrepared) in that case we must give up
	//************************************************************************************************************************
	IRA::CString prj,path,suffix,obsName,schedule,layoutName;
	long scanTag;
	ACS::stringSeq layout;
	ACS::Time ut;
	scanTag=0;
	suffix="";
	obsName=IRA::CString("system");
	getProjectCode(prj);
	schedule="none";
	layout.length(0);
	layoutName=_SCHED_NULLTARGET;
	m_subScanID=subScanId;
	ut=startRecording(startTime,m_scanID,subScanId,scanTag,m_config->getSystemDataDirectory(),suffix,obsName,prj,schedule,layoutName,layout);
}

void CCore::stopRecording() throw (ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,ComponentErrors::CouldntGetComponentExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	// now take the mutex
	baci::ThreadSyncGuard guard(&m_mutex);
	// throw (ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl)
	loadDefaultBackend();// throw (ComponentErrors::CouldntGetComponentExImpl);
	loadDefaultDataReceiver();
	CCore::stopDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_streamStarted,m_streamPrepared,m_streamConnected);
	// wait for a data transfer to complete before start with the latitude scan
	while (checkRecording(m_defaultDataReceiver.in(),m_defaultDataReceiverError)) {  // throw (ComponentErrors::OperationErrorExImpl)
		guard.release();
		IRA::CIRATools::Wait(0,250000); // 0.25 seconds
		guard.acquire();
	}
	m_subScanEpoch=0;
}

void CCore::terminateScan() throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	// throw (ComponentErrors::OperationErrorExImpl)
	loadDefaultBackend();// throw (ComponentErrors::CouldntGetComponentExImpl);
	loadDefaultDataReceiver();
	CCore::stopScan(m_defaultDataReceiver.in(), m_defaultDataReceiverError,m_scanStarted);
	//throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
	CCore::disableDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_defaultDataReceiver.in(),m_defaultDataReceiverError,m_streamStarted,m_streamPrepared,m_streamConnected,m_scanStarted);
	m_scanID=0; m_subScanID=0;
	m_subScanEpoch=0;
}

void CCore::setRestFrequency(const ACS::doubleSeq& in)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	m_restFrequency=in;
}

void CCore::setProjectCode(const char* code) throw (ManagementErrors::UnkownProjectCodeErrorExImpl)
{
	IRA::CString newCode(code);
	if (newCode=="''") { // if '' given...maps to default user
		newCode=m_config->getDefaultProjectCode();
	}
	if (m_config->getCheckProjectCode()) {
		IRA::CString path=m_config->getScheduleDirectory()+newCode;
			if (!IRA::CIRATools::directoryExists(path)) {
				_EXCPT(ManagementErrors::UnkownProjectCodeErrorExImpl,impl,"CCore::setProjectCode()");
				throw impl;
			}
	}
	//no need to get the mutex, because it is already done inside the Schedule Executor thread
	if (m_schedExecuter) {
		m_schedExecuter->setProjectCode(newCode);
	}
}

void CCore::setDevice(const long& deviceID) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::ValidationErrorExImpl,
		ComponentErrors::OperationErrorExImpl,ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	//ACS::Time timestamp;
	Backends::GenericBackend_var backend;
	ACS::ROlong_var sectionsNumber;
	ACS::ROlongSeq_var inputSection;
	ACSErr::Completion_var comp;
	long sections=-1;
	long input=-1;
	ACS::longSeq_var iS;
	ACS::longSeq_var IFs,feeds;
	ACS::doubleSeq_var bws,freqs;
	double freq,bw;
	long IF,feed,inputs;
	double taper,waveLen;
	long device;
	//IRA::CString backendName;
	if (deviceID<0) {
		device=m_currentDevice;
	}
	else {
		device=deviceID;
	}
	guard.release();
	//backend=m_schedExecuter->getBackendReference(); //get the reference to the currently used backend.
	guard.acquire();
	//if (CORBA::is_nil(backend)) {
	loadDefaultBackend(); // throw ComponentErrors::CouldntGetComponentExImpl& err)
	backend=m_defaultBackend;
	/*}
	else {

	}	*/
	// get the receiver boss.....
	loadReceiversBoss(m_receiversBoss,m_receiversBossError); // throw ComponentErrors::CouldntGetComponentExImpl)
	try {
		sectionsNumber=backend->sectionsNumber();
		inputSection=backend->inputSection();
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::setDevice()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		throw impl;
	}
	sections=sectionsNumber->get_sync(comp.out());
	iS=inputSection->get_sync(comp.out());
	if (device>=sections) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCore::setDevice()");
		impl.setReason("device identifier does not exist in the current backend");
		throw impl;
	}
	for (unsigned ii=0;ii<iS->length();ii++) {
		if (iS[ii]==device) {
			input=ii;
			break;
		}
	}
	//********************************
	// in the case no iS[ii] corresponds to the device/section an error should be risen
	//********************************
	try {
		inputs=backend->getInputs(freqs,bws,feeds,IFs);
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::setDevice()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::setDevice()");
		impl.setReason("Unable to get inputs information from backend");
		throw impl;
	}
	catch (BackendsErrors::BackendsErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::setDevice()");
		impl.setReason("Unable to get inputs information from backend");
		throw impl;
	}
	if ((input<0) || (input>=inputs)) { //no input for the given section
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCore::setDevice()");
		impl.setReason("device does not look to have an input");
		throw impl;
	}
	bw=bws[input];
	freq=freqs[input];
	feed=feeds[input];
	IF=IFs[input];
	try {
		taper=m_receiversBoss->getTaper(freq,bw,feed,IF,waveLen);
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::setDevice()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_receiversBossError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::setDevice()");
		impl.setReason("Unable to get taper information from receivers boss");
		throw impl;
	}
	catch (ReceiversErrors::ReceiversErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::setDevice()");
		impl.setReason("Unable to get taper information from receivers boss");
		throw impl;
	}
	loadAntennaBoss(m_antennaBoss,m_antennaBossError);
	try {
		m_antennaBoss->computeFWHM(taper,waveLen);
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::setDevice()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_antennaBossError=true;
		throw impl;
	}
	m_currentDevice=device;
	ACS_LOG(LM_FULL_INFO,"CCore::setDevice()",(LM_NOTICE,"DEFAULT_DEVICE: %ld",m_currentDevice));
}
void CCore::stopSchedule()
{
	//no need to get the mutex, because it is already done inside the Schedule Executor thread
	if (m_schedExecuter) {
		m_schedExecuter->stopSchedule(true);
	}
}

void CCore::haltSchedule()
{
	//no need to get the mutex, because it is already done inside the Schedule Executor thread
	if (m_schedExecuter) {
		m_schedExecuter->stopSchedule(false);
	}
}

void CCore::startSchedule(const char* scheduleFile,const char * startSubScan) throw (
		ManagementErrors::ScheduleErrorExImpl, ManagementErrors::AlreadyRunningExImpl,
		ComponentErrors::MemoryAllocationExImpl,ManagementErrors::SubscanErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,ManagementErrors::LogFileErrorExImpl)
{
	//no need to get the mutex, because it is already done inside the Schedule Executor thread
	if (m_schedExecuter) {
 		//ManagementErrors::ScheduleErrorExImpl, ManagementErrors::AlreadyRunningExImpl,ComponentErrors::MemoryAllocationExImpl,ComponentErrors::CouldntGetComponentExImpl,
 		//ComponentErrors::CORBAProblemExImpl,ManagementErrors::LogFileErrorExImpl
		m_schedExecuter->startSchedule(scheduleFile,startSubScan);
	}
}
