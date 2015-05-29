void CCore::_wait(const double& seconds) throw (ComponentErrors::TimerErrorExImpl,ManagementErrors::AbortedByUserExImpl)
{
	long *done;
	done=new long;
	*done=0;
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	ACS::Time time;
	time=now.value().value+(ACS::Time)(seconds*10000000);
	m_abortCurrentOperation=false;
	if (!addTimerEvent(time,waitUntilHandler,static_cast<void *>(done),waitUntilHandlerCleanup)) {
		//errore
		_EXCPT(ComponentErrors::TimerErrorExImpl,dummy,"CCore::wait()");
		dummy.setReason("timer event could not be scheduled");
		delete done;
		throw dummy;
	}
	while ((*done)==0) {
		if (m_abortCurrentOperation) {
			m_abortCurrentOperation=false;
			_EXCPT(ManagementErrors::AbortedByUserExImpl,dummy,"CCore::wait()");
			dummy.setOperation("wait for an amount of time");
			throw dummy;
		}
		IRA::CIRATools::Wait(25000); // 25 milliseconds
	}
}

void CCore::_waitUntil(const ACS::Time& time) throw (ComponentErrors::TimerErrorExImpl,ManagementErrors::AbortedByUserExImpl)
{
	long *done;
	done=new long;
	*done=0;
	m_abortCurrentOperation=false;
	if (!addTimerEvent(time,waitUntilHandler,static_cast<void *>(done),waitUntilHandlerCleanup)) {
		//errore
		_EXCPT(ComponentErrors::TimerErrorExImpl,dummy,"CCore::waitUntil()");
		dummy.setReason("timer event could not be scheduled");
		delete done;
		throw dummy;
	}
	while ((*done)==0) {
		if (m_abortCurrentOperation) {
			m_abortCurrentOperation=false;
			_EXCPT(ManagementErrors::AbortedByUserExImpl,dummy,"CCore::waitUntil()");
			dummy.setOperation("wait until an epoch");
			throw dummy;
		}
		IRA::CIRATools::Wait(25000); // 25 milliseconds
	}
}

void CCore::_abort() throw (ManagementErrors::NotAllowedDuringScheduleExImpl)
{
	//no need to get the mutex, because it is already done inside the Schedule Executor thread
	if (m_schedExecuter) {
		if (m_schedExecuter->isScheduleActive()) {
			_EXCPT(ManagementErrors::NotAllowedDuringScheduleExImpl,dummy,"CCore::abort()");
			throw dummy;
		}
	}
	m_abortCurrentOperation=true;
}

void CCore::_nop() const
{
	return;
}

void CCore::_waitOnSource() throw (ManagementErrors::AbortedByUserExImpl)
{
	m_abortCurrentOperation=false;
	while (!m_isAntennaTracking) {
		if (m_abortCurrentOperation) {
			m_abortCurrentOperation=false;
			_EXCPT(ManagementErrors::AbortedByUserExImpl,dummy,"CCore::_waitOnSource()");
			dummy.setOperation("wait for source tracking");
			throw dummy;
		}
		IRA::CIRATools::Wait(25000);
	}
}

void CCore::_waitTracking() throw (ManagementErrors::AbortedByUserExImpl)
{
	m_abortCurrentOperation=false;
	while (!isTracking()) {
		if (m_abortCurrentOperation) {
			m_abortCurrentOperation=false;
			_EXCPT(ManagementErrors::AbortedByUserExImpl,dummy,"CCore::_waitTracking()");
			dummy.setOperation("wait for telescope tracking");
			throw dummy;
		}
		IRA::CIRATools::Wait(25000);
	}
}

void CCore::_lonOTF(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration) throw (
		ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl,
		ManagementErrors::CloseTelescopeScanErrorExImpl)
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

void CCore::_latOTF(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration) throw (
		ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl,ManagementErrors::CloseTelescopeScanErrorExImpl)
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

void CCore::_skydipOTF(const double& el1,const double& el2,const ACS::TimeInterval& duration) throw (
		ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl,
		ManagementErrors::CloseTelescopeScanErrorExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	ACS::Time startTime=0; // start asap
	Antenna::TTrackingParameters primary,secondary;
	MinorServo::MinorServoScan servo;
	Receivers::TReceiversParameters receievers;
	Schedule::CSubScanBinder binder(&primary,&secondary,&servo,&receievers);
	binder.skydip(el1,el2,duration,NULL);
	startTime=0; // it means start as soon as possible
	startScan(startTime,&primary,&secondary,&servo,&receievers); //ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl
	m_subScanEpoch=startTime;
}

void CCore::_track(const char *targetName) throw (ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl,
		ManagementErrors::CloseTelescopeScanErrorExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	ACS::Time startTime=0; // start asap
	Antenna::TTrackingParameters primary,secondary;
	MinorServo::MinorServoScan servo;
	Receivers::TReceiversParameters receievers;
	Schedule::CSubScanBinder binder(&primary,&secondary,&servo,&receievers);
	binder.track(targetName);
	startTime=0; // it means start as soon as possible
	startScan(startTime,&primary,&secondary,&servo,&receievers); //ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl
	m_subScanEpoch=startTime;
}

void CCore::_moon() throw (ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl,
		ManagementErrors::CloseTelescopeScanErrorExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	ACS::Time startTime=0; // start asap
	Antenna::TTrackingParameters primary,secondary;
	MinorServo::MinorServoScan servo;
	Receivers::TReceiversParameters receievers;
	Schedule::CSubScanBinder binder(&primary,&secondary,&servo,&receievers);
	binder.moon();
	startTime=0; // it means start as soon as possible
	startScan(startTime,&primary,&secondary,&servo,&receievers); //ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl
	m_subScanEpoch=startTime;
}

void CCore::_sidereal(const char * targetName,const double& ra,const double& dec,const Antenna::TSystemEquinox& eq,const Antenna::TSections& section) throw (
	ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl,ManagementErrors::CloseTelescopeScanErrorExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	ACS::Time startTime=0; // start asap
	Antenna::TTrackingParameters primary,secondary;
	MinorServo::MinorServoScan servo;
	Receivers::TReceiversParameters receievers;
	Schedule::CSubScanBinder binder(&primary,&secondary,&servo,&receievers);
	binder.sidereal(targetName,ra,dec,eq,section);
	startTime=0; // it means start as soon as possible
	startScan(startTime,&primary,&secondary,&servo,&receievers); //ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl
	m_subScanEpoch=startTime;
}

void CCore::_goTo(const double& azimuth,const double& elevation) throw (ManagementErrors::TelescopeSubScanErrorExImpl,
		ManagementErrors::TargetOrSubscanNotFeasibleExImpl,ManagementErrors::CloseTelescopeScanErrorExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	ACS::Time startTime=0; // start asap
	Antenna::TTrackingParameters primary,secondary;
	MinorServo::MinorServoScan servo;
	Receivers::TReceiversParameters receievers;
	Schedule::CSubScanBinder binder(&primary,&secondary,&servo,&receievers);
	//printf("Parametri del goTo %lf, %lf\n",azimuth,elevation);
	binder.goTo(azimuth,elevation);
	startTime=0; // it means start as soon as possible
	startScan(startTime,&primary,&secondary,&servo,&receievers); //ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl
	m_subScanEpoch=startTime;
}

void CCore::_peaker(const char* axis,const double& span,const ACS::TimeInterval& duration) throw (
		ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl,
		ManagementErrors::CloseTelescopeScanErrorExImpl,ComponentErrors::ValidationErrorExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	ACS::Time startTime=0; // start asap

	Antenna::TTrackingParameters primary,secondary;
	MinorServo::MinorServoScan servo;
	Receivers::TReceiversParameters receievers;
	if (m_config->getAxisFromServoName(IRA::CString(axis))==Management::MNG_NO_AXIS) {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCore::_peaker()");
        impl.setReason("Invalid axis");
        throw impl;
	}
	Schedule::CSubScanBinder binder(&primary,&secondary,&servo,&receievers);
	binder.peaker(IRA::CString(axis),span,duration,NULL);
	startTime=0; // it means start as soon as possible
	startScan(startTime,&primary,&secondary,&servo,&receievers); //ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl
	m_subScanEpoch=startTime;
}

/*void CCore::_goOff(const Antenna::TCoordinateFrame& frame,const double& beams) throw (ComponentErrors::CouldntGetComponentExImpl,
		ComponentErrors::ComponentNotActiveExImpl,ManagementErrors::AntennaScanErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	loadAntennaBoss(m_antennaBoss,m_antennaBossError); // throw ComponentErrors::CouldntGetComponentExImpl
	try {
		if (!CORBA::is_nil(m_antennaBoss)) {
			m_subScanEpoch=0;
			m_antennaBoss->goOff(frame,beams);
			clearAntennaTracking();
		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::goOff()");
			throw impl;
		}
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::AntennaScanErrorExImpl,impl,ex,"CCore::goOff()");
		throw impl;
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::AntennaScanErrorExImpl,impl,ex,"CCore::goOff()");
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::goOff()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_antennaBossError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::goOff()");
		m_antennaBossError=true;
		throw impl;
	}
}*/

void CCore::_chooseDefaultBackend(const char *bckInstance) throw (ComponentErrors::CouldntGetComponentExImpl)
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

void CCore::_chooseDefaultDataRecorder(const char *rcvInstance) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl)
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

void CCore::_changeLogFile(const char *fileName) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ManagementErrors::LogFileErrorExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	loadCustomLogger(m_customLogger,m_customLoggerError); // throw ComponentErrors::CouldntGetComponentExImpl
	IRA::CString fullName;
	IRA::CString logName(fileName);
	fullName=logName+".log";
	ACS_LOG(LM_FULL_INFO,"CCore::changeLogFile()",(LM_NOTICE,"NEW_LOG_FILE: %s",(const char *)fullName));
	try {
		m_customLogger->flush();
		m_customLogger->setLogfile((const char *)m_config->getLogDirectory(),(const char *)fullName);
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

void CCore::_getWeatherStationParameters(double &temp,double& hum,double& pres, double& wind) throw (ComponentErrors::CouldntGetComponentExImpl,
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

void CCore::_initRecording(const long& scanid) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,
		ComponentErrors::CORBAProblemExImpl,ManagementErrors::NotAllowedDuringScheduleExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_schedExecuter) {
		if (m_schedExecuter->isScheduleActive()) {
			_EXCPT(ManagementErrors::NotAllowedDuringScheduleExImpl,dummy,"CCore::_initRecording()");
			throw dummy;
		}
	}
	try {
		CCore::disableDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_defaultDataReceiver.in(),m_defaultDataReceiverError,m_streamStarted,m_streamPrepared,
				m_streamConnected,m_dataTransferInitialized);
	}
	catch (...) {
		m_streamStarted=m_streamPrepared=m_streamConnected=m_dataTransferInitialized=false;
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

void CCore::_startRecording(const long& subScanId,const ACS::TimeInterval& duration) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::ComponentNotActiveExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,
		ManagementErrors::DataTransferSetupErrorExImpl,ComponentErrors::TimerErrorExImpl,ManagementErrors::AbortedByUserExImpl,
		ManagementErrors::NotAllowedDuringScheduleExImpl,ManagementErrors::RecordingAlreadyActiveExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_schedExecuter) {
		if (m_schedExecuter->isScheduleActive()) {
			_EXCPT(ManagementErrors::NotAllowedDuringScheduleExImpl,dummy,"CCore::_initRecording()");
			throw dummy;
		}
	}
	if (m_streamStarted) {
		_EXCPT(ManagementErrors::RecordingAlreadyActiveExImpl,dummy,"CCore::_initRecording()");
		throw dummy;
	}
	if ((!m_streamConnected) && (!m_streamPrepared)) {
		_EXCPT(ManagementErrors::DataTransferSetupErrorExImpl,dummy,"CCore::_initRecording()");
		throw dummy;
	}
	ACS::Time realStart,waitFor;
	IRA::CString prj,path,suffix,obsName,schedule,layoutName;
	long scanTag;
	ACS::stringSeq layout;
	// throw (ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl)
	loadDefaultBackend();// throw (ComponentErrors::CouldntGetComponentExImpl);
	loadDefaultDataReceiver();

	//no mutex below.....
	guard.release();
	/*if (m_subScanEpoch==0) {
		ACS_LOG(LM_FULL_INFO,"CCore::_startRecording()",(LM_NOTICE,"WAITING_ON_SOURCE_FLAG"));
		_waitOnSource();
	}*/
	scanTag=0; suffix=""; obsName=IRA::CString("system"); getProjectCode(prj); schedule="none";
	layout.length(0); layoutName=_SCHED_NULLTARGET;
	guard.acquire();
	m_subScanID=subScanId;
	realStart=startRecording(m_subScanEpoch,m_scanID,m_subScanID,scanTag,m_config->getSystemDataDirectory(),suffix,obsName,prj,schedule,layoutName,layout);
	waitFor=realStart+duration; // this is the time at which the stop should be issued
	guard.release();
	//This is to implement a finally clause, if the user abort when the recording has already started I'd like, at least to stop it
	try {
		_waitUntil(waitFor); // throw ComponentErrors::TimerErrorExImpl,ManagementErrors::AbortedByUserExImpl
	}
	catch (ManagementErrors::AbortedByUserExImpl& ex){
		guard.acquire();
		m_subScanEpoch=0;
		CCore::stopDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_streamStarted,m_streamPrepared,m_streamConnected);
		guard.release();
		throw ex;
	}
	guard.acquire();
	m_subScanEpoch=0;
	CCore::stopDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_streamStarted,m_streamPrepared,m_streamConnected);
	guard.release();
	// wait for a data transfer to complete before start with the latitude scan
	while (checkRecording(m_defaultDataReceiver.in(),m_defaultDataReceiverError)) {  // throw (ComponentErrors::OperationErrorExImpl)
		_wait(0.20); // throw ComponentErrors::TimerErrorExImpl,ManagementErrors::AbortedByUserExImpl
		//IRA::CIRATools::Wait(0,250000); // 0.25 seconds
	}
}

void CCore::_terminateScan() throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	m_scanID=0; m_subScanID=0;
	m_subScanEpoch=0;
	// throw (ComponentErrors::OperationErrorExImpl)
	loadDefaultBackend();// throw (ComponentErrors::CouldntGetComponentExImpl);
	loadDefaultDataReceiver();
	CCore::stopScan(m_defaultDataReceiver.in(),m_defaultDataReceiverError,m_dataTransferInitialized);
	//throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
	CCore::disableDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_defaultDataReceiver.in(),m_defaultDataReceiverError,m_streamStarted,m_streamPrepared,
			m_streamConnected,m_dataTransferInitialized);
}

void CCore::_setRestFrequency(const ACS::doubleSeq& in)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	m_restFrequency=in;
	IRA::CString out;
	IRA::CIRATools::doubleSeqToStr(in,out);
	ACS_LOG(LM_FULL_INFO,"CCore::_setRestFrequency()",(LM_NOTICE,"Rest frequency : %s",(const char *)out));
}

void CCore::_fTrack(const char *dev) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ManagementErrors::InvalidRestFrequencyExImpl,ComponentErrors::OperationErrorExImpl,ComponentErrors::ValidationErrorExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntCallOperationExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	Backends::GenericBackend_var backend;
	ACS::ROlong_var sectionsNumberRO;
	ACS::ROlongSeq_var inputSectionRO;
	ACS::ROlong_var IFNumberRO;
	ACS::longSeq_var inputSection;
	ACSErr::Completion_var comp;
	long sections;
	long inputs;
	long IFNumber;
	long digits;
	ACS::longSeq_var bckinputIF,bckinputFeed,fndoutputPol;
	ACS::doubleSeq_var bckinputBW,bckinputFreq,fndoutputLO,fndoutputBw,fndoutputFreq,topocentricFreq;
	ACS::doubleSeq sectionFreq,inputLO,lo;
	IRA::CString device(dev);

	//1) preliminary checks----------------------------------------------------------------------------------
	if (m_restFrequency.length()==0) {
		_EXCPT(ManagementErrors::InvalidRestFrequencyExImpl,impl,"CCore::_fTrack()");
		throw impl;
	}
	device.MakeUpper();
	if ((device!="ALL") && (device!="LO") & (device!="BCK")) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCore::_fTrack()");
		impl.setReason("parameter differs from ALL,LO,BCK");
		throw impl;
	}
	//---------------------------------------------------------------------------------------------------
	//2) initializations----------------------------------------------------------------------------------
	loadDefaultBackend();// throw (ComponentErrors::CouldntGetComponentExImpl);
	backend=m_defaultBackend;
	loadReceiversBoss(m_receiversBoss,m_receiversBossError); // throw ComponentErrors::CouldntGetComponentExImpl)
	loadAntennaBoss(m_antennaBoss,m_antennaBossError);
	// now let's collect all the information required by computation
	digits=m_config->getFTrackDigits();
	//printf("digits :%ld\n",digits);
	//---------------------------------------------------------------------------------------------------
	//3) info from backend--------------------------------------------------------------------------------
	try {
		sectionsNumberRO=backend->sectionsNumber();
		inputSectionRO=backend->inputSection();
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::_fTrack()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		throw impl;
	}
	sections=sectionsNumberRO->get_sync(comp.out()); // number of backend sections
	inputSection=inputSectionRO->get_sync(comp.out());
	//printf("sections :%ld\n",sections);
	if ((m_restFrequency.length()>1) && (m_restFrequency.length()!=(unsigned)sections)) {   // check one rest frequency or as many as section are given
		_EXCPT(ManagementErrors::InvalidRestFrequencyExImpl,impl,"CCore::_fTrack()");
		throw impl;
	}
	try {
		inputs=backend->getInputs(bckinputFreq,bckinputBW,bckinputFeed,bckinputIF);
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::_fTrack()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_defaultBackendError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CCore::_fTrack()");
		impl.setOperationName("getInputs()");
		impl.setComponentName(backend->name());
		throw impl;
	}
	catch (BackendsErrors::BackendsErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CCore::_fTrack()");
		impl.setOperationName("getInputs()");
		impl.setComponentName(backend->name());
		throw impl;
	}
	if (inputSection->length()!=(unsigned)inputs) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCore::_fTrack()");
		impl.setReason("inconsistent data from the backend inputs number");
		throw impl;
	}
	//---------------------------------------------------------------------------------------------------
	//4) info from antenna--------------------------------------------------------------------------------
	try {
		m_antennaBoss->getTopocentricFrequency(m_restFrequency,topocentricFreq.out());
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CCore::_fTrack()");
		impl.setOperationName("getTopocentricFrequency()");
		impl.setComponentName(m_antennaBoss->name());
		throw impl;
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CCore::_fTrack()");
		impl.setOperationName("getTopocentricFrequency()");
		impl.setComponentName(m_antennaBoss->name());
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::_fTrack()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_antennaBossError=true;
		throw impl;
	}
	catch (...) {
		m_antennaBossError=true;
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::_fTrack()");
		throw impl;
	}
	//for (unsigned t=0;t<topocentricFreq->length();t++) //printf("topocentric Freq :%lf\n",topocentricFreq[t]);
	// just to make sure the topocentric sequence has the right dimension!
	if (topocentricFreq->length()!=m_restFrequency.length()) {
		topocentricFreq->length(m_restFrequency.length());
	}
	//---------------------------------------------------------------------------------------------------
	//4) info from receivers-------------------------------------------------------------------------------
	try {
		IFNumberRO=m_receiversBoss->IFs();
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::_fTrack()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		throw impl;
	}
	IFNumber=IFNumberRO->get_sync(comp.out()); // number of output IFs of the receeever
	//printf("if number :%ld\n",IFNumber);
	try {
		m_receiversBoss->getIFOutput(bckinputFeed,bckinputIF,fndoutputFreq.out(),fndoutputBw.out(),fndoutputPol.out(),fndoutputLO.out());
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::_fTrack()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_receiversBossError=true;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CCore::_fTrack()");
		impl.setOperationName("getIFOutput");
		impl.setComponentName(m_receiversBoss->name());
		throw impl;
	}
	catch (ReceiversErrors::ReceiversErrorsEx & ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CCore::_fTrack()");
		impl.setOperationName("getIFOutput");
		impl.setComponentName(m_receiversBoss->name());
		throw impl;
	}
	if (fndoutputFreq->length()!=(unsigned)inputs) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCore::_fTrack()");
		impl.setReason("inconsistent data from the receivers if outputs");
		throw impl;
	}
	//---------------------------------------------------------------------------------------------------
	//5) let's start with some computations  -----------------------------------------------------------------------------
	sectionFreq.length(sections);
	inputLO.length(inputs);
	lo.length(IFNumber);
	long currentSection;
	for (long j=0;j<inputs;j++) {
		if ((device=="ALL") || (device=="LO")) {
			currentSection=inputSection[j];
			//printf("currentSection :%ld\n",currentSection);
			if (topocentricFreq->length()==1) {
				inputLO[j]=IRA::CIRATools::roundNearest(topocentricFreq[0]-bckinputFreq[currentSection]-
						(bckinputBW[currentSection]/2.0),digits);
				//printf("inputLO[j] :%lf\n",inputLO[j]);
			}
			else {
				inputLO[j]=IRA::CIRATools::roundNearest(topocentricFreq[currentSection]-bckinputFreq[currentSection]-
						(bckinputBW[currentSection]/2.0),digits);
				//printf("inputLO[j] :%lf\n",inputLO[j]);
			}
			lo[bckinputIF[j]]=inputLO[j]; // local oscillator per IFs
		}
		else {
			inputLO[j]=fndoutputLO[j];
		}
	}
	for (long j=0;j<inputs;j++) {
		if ((device=="ALL") || (device=="BCK")) {
			currentSection=inputSection[j];
			if (topocentricFreq->length()==1) {
				sectionFreq[currentSection]=topocentricFreq[0]-inputLO[j]-(bckinputBW[currentSection]/2.0);
			}
			else {
				sectionFreq[currentSection]=topocentricFreq[currentSection]-inputLO[j]-(bckinputBW[currentSection]/2.0);
			}
		}
	}
	//---------------------------------------------------------------------------------------------------
	//5) now lets command the new values  -----------------------------------------------------------------------------
	if ((device=="ALL") || (device=="LO")) {
		try {
			m_receiversBoss->setLO(lo);
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::_fTrack()");
			impl.setReason("Could not set local oscillator");
			throw impl;
		}
		catch (ReceiversErrors::ReceiversErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::_fTrack()");
			impl.setReason("Could not set local oscillator");
			throw impl;
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::_fTrack()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			m_receiversBossError=true;
			throw impl;
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::_fTrack()");
			m_receiversBossError=true;
			throw impl;
		}
	}
	if ((device=="ALL") || (device=="BCK")) {
		for (long j=0;j<sections;j++) {
			try {
				backend->setSection(j,sectionFreq[j],-1.0,-1,-1,-1.0,-1);
			}
			catch (ComponentErrors::ComponentErrorsEx& ex) {
				_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::_fTrack()");
				impl.setReason("Could not set the backend sections");
				throw impl;
			}
			catch (BackendsErrors::BackendsErrorsEx& ex) {
				_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::_fTrack()");
				impl.setReason("Could not set backend sections");
				throw impl;
			}
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::_fTrack()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				m_defaultBackendError=true;
				throw impl;
			}
			catch (...) {
				_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::_fTrack()");
				m_defaultBackendError=true;
				throw impl;
			}
		}
	}
}

void CCore::_setProjectCode(const char* code) throw (ManagementErrors::UnkownProjectCodeErrorExImpl)
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

void CCore::_setDevice(const long& deviceID) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::ValidationErrorExImpl,
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
	//guard.release();
	//backend=m_schedExecuter->getBackendReference(); //get the reference to the currently used backend.
	//guard.acquire();
	//if (CORBA::is_nil(backend)) {
	loadDefaultBackend(); // throw ComponentErrors::CouldntGetComponentExImpl& err)
	backend=m_defaultBackend;
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

void CCore::_callTSys(ACS::doubleSeq& tsys) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::OperationErrorExImpl,
		ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl)
{
	Backends::GenericBackend_var backend;
	ACS::doubleSeq_var freq;
	ACS::doubleSeq_var bandWidth;
	ACS::longSeq_var feed;
	ACS::doubleSeq_var skyFreq;
	ACS::doubleSeq_var skyBw;
	ACS::doubleSeq ratio;
	ACS::doubleSeq_var mark,tpi,zero,tpiCal;
	ACS::longSeq_var IFs;
	double scaleFactor;
	long inputs;
	IRA::CString outLog;
	IRA::CString backendName;

	//backend=m_schedExecuter->getBackendReference(); //get the reference to the currently used backend.
	baci::ThreadSyncGuard guard(&m_mutex);
	//if (CORBA::is_nil(backend)) {
	loadDefaultBackend(); // throw ComponentErrors::CouldntGetComponentExImpl& err)
	backend=m_defaultBackend;
	loadReceiversBoss(m_receiversBoss,m_receiversBossError); // throw ComponentErrors::CouldntGetComponentExImpl& err)
	//Now get information from the backend about all the involved inputs.....
	try {
		inputs=backend->getInputs(freq,bandWidth,feed,IFs);
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::callTSys()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::callTSys()");
		impl.setReason("Unable to get inputs information from backend");
		throw impl;
	}
	catch (BackendsErrors::BackendsErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::callTSys()");
		impl.setReason("Unable to get inputs information from backend");
		throw impl;
	}
	// call the receivers boss in order to get the calibration diode values......
	try {
		mark=m_receiversBoss->getCalibrationMark(freq,bandWidth,feed,IFs,skyFreq.out(),skyBw.out(),scaleFactor);
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::callTSys()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_receiversBossError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::callTSys()");
		impl.setReason("Unable to get calibration diode values");
		throw impl;
	}
	catch (ReceiversErrors::ReceiversErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::callTSys()");
		impl.setReason("Unable to get calibration diode values");
		throw impl;
	}
	//wait for the calibration diode to settle......
	//Now contact the backend to get the TotalPower when the calibration diode is switched off
	try {
		tpi=backend->getTpi();
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::callTSys()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::callTSys()");
		impl.setReason("Unable to get TPI measurements");
		throw impl;
	}
	catch (BackendsErrors::BackendsErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::callTSys()");
		impl.setReason("Unable to get TPI measurements");
		throw impl;
	}
	try {
		zero=backend->getZero();
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::callTSys()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::callTSys()");
		impl.setReason("Unable to get TPI zero measurements");
		throw impl;
	}
	catch (BackendsErrors::BackendsErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::callTSys()");
		impl.setReason("Unable to get TPI zero measurements");
		throw impl;
	}
	// now turn again the calibration diode on
	try {
		m_receiversBoss->calOn();
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::callTSys()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_receiversBossError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::callTSys()");
		impl.setReason("Could not turn the calibration mark on");
		throw impl;
	}
	catch (ReceiversErrors::ReceiversErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::callTSys()");
		impl.setReason("Could not turn the calibration mark on");
		throw impl;
	}
	//wait for the calibration diode to switch on completely
	guard.release();
	IRA::CIRATools::Wait(m_config->getTsysGapTime());
	guard.acquire();
	//Now contact the backend to get the TotalPower and readings.
	try {
		tpiCal=backend->getTpi();
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::callTSys()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::callTSys()");
		impl.setReason("Unable to get TPI cal measurements");
		throw impl;
	}
	catch (BackendsErrors::BackendsErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::callTSys()");
		impl.setReason("Unable to get TPI cal measurements");
		throw impl;
	}
	//eventually let's make sure the calibration diode is off again
	try {
		m_receiversBoss->calOff();
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::callTSys()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_receiversBossError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::callTSys()");
		impl.setReason("Could not turn the calibration mark off");
		throw impl;
	}
	catch (ReceiversErrors::ReceiversErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::callTSys()");
		impl.setReason("Could not turn the calibration mark off");
		throw impl;
	}
	// computes the tsys and the Kelvin over counts ratio.......
	ratio.length(inputs);
	tsys.length(inputs);
	for (int i=0;i<inputs;i++) {
		if ((mark[i]>0.0) && (tpiCal[i]>tpi[i])) {
			//ratio[i]=(tpiCal[i]-tpi[i])/mark[i];
			ratio[i]=mark[i]/(tpiCal[i]-tpi[i]);
			ratio[i]*=scaleFactor;
			tsys[i]=(tpi[i]-zero[i])*ratio[i];
		}
		else {
			ratio[i]=1.0;
			tsys[i]=-1.0;
		}
	}
	//Now contact the backend to give back some results.....
	try {
		backend->setKelvinCountsRatio(ratio,tsys);
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::callTSys()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		throw impl;
	}
	//Now let's compose a message to log
	for (int i=0;i<inputs;i++) {
		outLog.Format("DEVICE/%d Feed: %d, IF: %d, Freq: %lf, Bw: %lf/",i,feed[i],IFs[i],freq[i],bandWidth[i]);
		ACS_LOG(LM_FULL_INFO,"CCore::callTSys()",(LM_NOTICE,(const char *)outLog));
		outLog.Format("CALTEMP/%d %lf(%lf)",i,mark[i],scaleFactor);
		ACS_LOG(LM_FULL_INFO,"CCore::callTSys()",(LM_NOTICE,(const char *)outLog));
		outLog.Format("TPICAL/%d %lf",i,tpiCal[i]);
		ACS_LOG(LM_FULL_INFO,"CCore::callTSys()",(LM_NOTICE,(const char *)outLog));
		outLog.Format("TPIZERO/%d %lf",i,zero[i]);
		ACS_LOG(LM_FULL_INFO,"CCore::callTSys()",(LM_NOTICE,(const char *)outLog));
		outLog.Format("TPI/%d %lf",i,tpi[i]);
		ACS_LOG(LM_FULL_INFO,"CCore::callTSys()",(LM_NOTICE,(const char *)outLog));
		outLog.Format("TSYS/%d %lf",i,tsys[i]);
		ACS_LOG(LM_FULL_INFO,"CCore::callTSys()",(LM_NOTICE,(const char *)outLog));
	}
}

void CCore::_stopSchedule()
{
	//no need to get the mutex, because it is already done inside the Schedule Executor thread
	if (m_schedExecuter) {
		m_schedExecuter->stopSchedule(true);
	}
}

void CCore::_haltSchedule()
{
	//no need to get the mutex, because it is already done inside the Schedule Executor thread
	if (m_schedExecuter) {
		m_schedExecuter->stopSchedule(false);
	}
}

void CCore::_startSchedule(const char* scheduleFile,const char * startSubScan) throw (ManagementErrors::ScheduleErrorExImpl,ManagementErrors::AlreadyRunningExImpl,
		ComponentErrors::MemoryAllocationExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ManagementErrors::LogFileErrorExImpl)
{
	//no need to get the mutex, because it is already done inside the Schedule Executor thread
	if (m_schedExecuter) {
 		//ManagementErrors::ScheduleErrorExImpl, ManagementErrors::AlreadyRunningExImpl,ComponentErrors::MemoryAllocationExImpl,ComponentErrors::CouldntGetComponentExImpl,
 		//ComponentErrors::CORBAProblemExImpl,ManagementErrors::LogFileErrorExImpl
		m_schedExecuter->startSchedule(scheduleFile,startSubScan);
	}
}
