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

void CCore::initRecording(const long& scanid) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,
		ComponentErrors::CORBAProblemExImpl)
{
	// now take the mutex
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
	//now load the Backend and the dataReceiver
	// we always load the default backend because this method could not be called when a schedule is running so, for sure, there will not be a schedule backend
	loadDefaultBackend(); //throw (ComponentErrors::CouldntGetComponentExImpl);
	loadDefaultDataReceiver(); //throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl)
	// ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl
	CCore::enableDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_defaultDataReceiver.in(),m_streamConnected,m_streamPrepared);
	m_scanID=scanid;
}

bool CCore::checkScan(ACS::Time& ut,const Antenna::TTrackingParameters *const prim,const Antenna::TTrackingParameters *const sec,
	const Receivers::TReceiversParameters*const recvPar,const double& minEl,const double& maxEl) throw (ComponentErrors::UnexpectedExImpl,
	ComponentErrors::OperationErrorExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,
	ComponentErrors::CouldntGetComponentExImpl)
{
	/**
	 * ********************************************************
	 * The same checks should involve also the minor servo subsystem
	 * ********************************************************
  */
	baci::ThreadSyncGuard guard(&m_mutex);
	//ACS::Time antennaUT,receiversUT;
	bool antennaAnswer,receiversAnswer;
	//antennaUT=receiversUT=ut;
	TIMEVALUE now;
	loadAntennaBoss(m_antennaBoss,m_antennaBossError); // throw ComponentErrors::CouldntGetComponentExImpl
	try {
		if (!CORBA::is_nil(m_antennaBoss)) {
			//antennaUT will stores the extimated start time from the antenna for all kind of subscans
			antennaAnswer=m_antennaBoss->checkScan(ut,*prim,*sec,minEl,maxEl,m_antennaRunTime);
			ACS_LOG(LM_FULL_INFO,"CCore::checkScan()",(LM_DEBUG,"SLEWING_TIME %lld :",m_antennaRunTime.slewingTime));
		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::checkScan()");
			throw impl;
		}
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::checkScan()");
		impl.setReason("Could not connect to the antenna boss");
		throw impl;
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::checkScan()");
		impl.setReason("Could not connect to the antenna boss");
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::checkScan()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_antennaBossError=true;
		throw impl;
	}
	catch (...) {
		m_antennaBossError=true;
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::checkScan()");
		throw impl;
	}
	if (!antennaAnswer) return antennaAnswer;
	loadReceiversBoss(m_receiversBoss,m_receiversBossError);
	try {
		if (!CORBA::is_nil(m_receiversBoss)) {
			receiversAnswer=m_receiversBoss->checkScan(ut,*recvPar,m_antennaRunTime,m_receiversRunTime);
		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::checkScan()");
			throw impl;
		}
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::checkScan()");
		impl.setReason("Could not connect to the receivers boss");
		throw impl;
	}
	catch (ReceiversErrors::ReceiversErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::checkScan()");
		impl.setReason("Could not connect to the receivers boss");
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::checkScan()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_receiversBossError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::checkScan()");
		m_receiversBossError=true;
		throw impl;
	}
	/*******************************************************/
	/* the receiversUT variable must be substituted by the minorservoUT variable. */
	/* The receivers are considered exactly as the active surface,  */
	/* ****************************************************************** */
	if (ut==0) {  //if the request was to start as soon as possible, we need to do some computation and change the effective start time
		if ((!m_antennaRunTime.onTheFly) && (!m_receiversRunTime.onTheFly)) { //simple beampark scan, for example
			// do nothing as ut should be zero as well, i.e the system must wait for the tracking flag to be true
		}
		else if ((!m_antennaRunTime.onTheFly) && (m_receiversRunTime.onTheFly)) { //typical case is focus scan...
			ut=MAX(m_antennaRunTime.startEpoch,m_receiversRunTime.startEpoch)+2000000; // add 0.2 sec. as overhead
		}
		else if ((m_antennaRunTime.onTheFly) && (!m_receiversRunTime.onTheFly)) { //typical case of OTF scan...the resulting time is the one comingo from the antenna
			ut=m_antennaRunTime.startEpoch;
		}
		else { // both are OTF...typically a scan along a subreflactor axis (Y for example)
			// the correct start time is the maximum of the two times
			ut=MAX(m_antennaRunTime.startEpoch,m_receiversRunTime.startEpoch)+2000000; // add 0.2 sec. as overhead
		}
	}  // otherwise keep the original starting time, which is the one requested by the user
	return (antennaAnswer && receiversAnswer);
}

void CCore::doScan(ACS::Time& ut,const Antenna::TTrackingParameters * const prim,const Antenna::TTrackingParameters *const sec,
		const Receivers::TReceiversParameters& recvPa) throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,
				ComponentErrors::UnexpectedExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	loadAntennaBoss(m_antennaBoss,m_antennaBossError); // throw ComponentErrors::CouldntGetComponentExImpl
	try {
		if (!CORBA::is_nil(m_antennaBoss)) {
			m_antennaBoss->startScan(ut,*prim,*sec); // the ut could be modified by the call
		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::doScan()");
			throw impl;
		}
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::doScan()");
		impl.setReason("Could not command new scan to the telescope");
		throw impl;
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::doScan()");
		impl.setReason("Could not command new scan to the telescope");
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::doScan()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_antennaBossError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::doScan()");
		m_antennaBossError=true;
		throw impl;
	}
	/* ************************************************************************************ */
	/* now need to call the scan to minorservo system */
	/***************************************************************************************/
	loadReceiversBoss(m_receiversBoss,m_receiversBossError);
	try {
		if (!CORBA::is_nil(m_receiversBoss)) {
			m_receiversBoss->startScan(ut,recvPa);
		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::doScan()");
			throw impl;
		}
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::doScan()");
		impl.setReason("Could not connect to the receivers boss");
		throw impl;
	}
	catch (ReceiversErrors::ReceiversErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::doScan()");
		impl.setReason("Could not connect to the receivers boss");
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::doScan()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_receiversBossError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::doScan()");
		m_receiversBossError=true;
		throw impl;
	}
}

Management::TScanAxis CCore::computeScanAxis() throw (ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	Management::TScanAxis scanAxis_ant,scanAxis_ms;
	CORBA::String_var servo;
	scanAxis_ant=scanAxis_ms=Management::MNG_NO_AXIS;
	baci::ThreadSyncGuard guard(&m_mutex);
	loadAntennaBoss(m_antennaBoss,m_antennaBossError); // throw ComponentErrors::CouldntGetComponentExImpl
	try {
		if (!CORBA::is_nil(m_antennaBoss)) {
			m_antennaBoss->getScanAxis(scanAxis_ant);
		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::computeScanAxis()");
			throw impl;
		}
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::computeScanAxis()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_antennaBossError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::computeScanAxis()");
		m_antennaBossError=true;
		throw impl;
	}
	loadMinorServoBoss(m_minorServoBoss,m_minorServoBossError); // throw ComponentErrors::CouldntGetComponentExImpl
	try {
		if (!CORBA::is_nil(m_minorServoBoss)) {
			servo=m_minorServoBoss->getScanAxis();
			scanAxis_ms=m_config->getAxisFromServoName(servo.in());
		}
	}
	catch (ManagementErrors::SubscanErrorEx& ex) {

	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::computeScanAxis()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_minorServoBossError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::computeScanAxis()");
		m_minorServoBossError=true;
		throw impl;
	}
	// this choice has to be reconsidered, does the minor servo movement always prevail?
	if (scanAxis_ms!=Management::MNG_NO_AXIS) { // if a minor servo has been selected, the resulting scan axis is the axis mapped to the servo
		return scanAxis_ms;
	}
	else {
		return scanAxis_ant;
	}
}

void CCore::disableDataTransfer() throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	// no need to check the backend and writer are active....when disabling the data transfer....
	//loadDefaultBackend();// throw (ComponentErrors::CouldntGetComponentExImpl);
	//loadDefaultDataReceiver();
	CCore::disableDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_defaultDataReceiver.in(),m_defaultDataReceiverError,m_streamStarted,m_streamPrepared,
			m_streamConnected,m_scanStarted);
}

void CCore::enableDataTransfer() throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::CouldntGetComponentExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	loadDefaultBackend();// throw (ComponentErrors::CouldntGetComponentExImpl);
	loadDefaultDataReceiver();
	CCore::enableDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_defaultDataReceiver.in(),m_streamConnected,m_streamPrepared);
}

void CCore::configureBackend(const IRA::CString& name,const std::vector<IRA::CString>& procedure) throw (ManagementErrors::BackendProcedureErrorExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	loadDefaultBackend();// throw (ComponentErrors::CouldntGetComponentExImpl);
	CCore::configureBackend(m_defaultBackend.in(),m_defaultBackendError,name,procedure);
}

void CCore::stopDataTransfer() throw (ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	loadDefaultBackend();// throw (ComponentErrors::CouldntGetComponentExImpl);
	CCore::stopDataTransfer(m_defaultBackend,m_defaultBackendError,m_streamStarted,m_streamPrepared,m_streamConnected);
}

bool CCore::checkRecording() throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	loadDefaultDataReceiver();
	return CCore::checkRecording(m_defaultDataReceiver.in(),m_defaultDataReceiverError);
}

ACS::Time CCore::startRecording(const ACS::Time& recUt,
																		  const long& scanId,
																		  const long& subScanId,
																		  const long& scanTag,
																		  const IRA::CString& basePath,
																		  const IRA::CString& suffix,
																		  const IRA::CString observerName,
																		  const IRA::CString& projectName,
																		  const IRA::CString& scheduleFileName,
																		  const IRA::CString& layout,
																		  const ACS::stringSeq& layoutProc
																		  )  throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,
																				  ComponentErrors::UnexpectedExImpl,ManagementErrors::BackendNotAvailableExImpl,
																				  ManagementErrors::DataTransferSetupErrorExImpl,ComponentErrors::ComponentNotActiveExImpl,
																				  ComponentErrors::CouldntGetComponentExImpl)
{
	IRA::CString outputFileName,newSuffix;
	/*ACS::TimeInterval startLst;*/
	ACS::Time startUTTime;
	Management::TScanAxis scanAxis;
	IRA::CString baseName,path,extraPath,targetID,projectCode;
	// now take the mutex
	baci::ThreadSyncGuard guard(&m_mutex);
	//************************************************************************************************************************
	//*********************CHECK if a schedule is running or if the  recording is not initialized (streamConnected and streamPrepared) in that case we must give up
	//************************************************************************************************************************
	if (recUt==0) {
		TIMEVALUE now;
		TIMEDIFFERENCE lst;
		IRA::CDateTime currentUT;
		IRA::CIRATools::getTime(now);
		startUTTime=now.value().value;
	}
	else {
		startUTTime=recUt;
		/*startLst=recLst;*/
	}
	IRA::CString out;
	IRA::CIRATools::timeToStr(startUTTime,out);
	ACS_LOG(LM_FULL_INFO,"CCore::startRecording()",(LM_DEBUG,"RECORDING_START_TIME: %s",(const char *)out));
	getProjectCode(projectCode);
	path=/*m_config->getDataDirectory()*/basePath+projectCode+"/";

	loadAntennaBoss(m_antennaBoss,m_antennaBossError); // throw ComponentErrors::CouldntGetComponentExImpl
	try {
		ACSErr::Completion_var comp;
		ACS::ROstring_var targetRef=m_antennaBoss->target();
		if (!CORBA::is_nil(targetRef)) {
			CORBA::String_var  tmp;
			tmp=targetRef->get_sync(comp.out());
			ACSErr::CompletionImpl compImpl(comp);
			if (compImpl.isErrorFree()) {
				targetID=(const char *)tmp;
			}
		}
	}
	catch (...) {
		targetID="";
		//in this case we do not want to to do nothing.....it is an error but we can survive
	}
	if (suffix=="") {
		newSuffix=targetID;
	}
	else {
		newSuffix=suffix;
	}
	baseName=CCore::computeOutputFileName(startUTTime,m_site,m_dut1,projectCode,suffix,extraPath);
	// compute the axis or direction along which the scan is performed
	scanAxis=computeScanAxis(); //  throw (ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
	loadDefaultBackend();// throw (ComponentErrors::CouldntGetComponentExImpl);
	loadDefaultDataReceiver();
	CCore::setupDataTransfer(m_scanStarted,m_defaultDataReceiver.in(),m_defaultDataReceiverError,m_defaultBackend.in(),m_defaultBackendError,observerName,
			projectName,baseName,path,extraPath,scheduleFileName,targetID,layout,layoutProc,scanTag,getCurrentDevice(),scanId,startUTTime ,subScanId,scanAxis,
			m_config); // throws  ComponentErrors::OperationErrorExImpl,ComponentErrors::UnexpectedExImpl,ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl
	CCore::startDataTansfer(m_defaultBackend.in(),m_defaultBackendError,recUt,m_streamStarted,m_streamPrepared,m_streamConnected);
	return startUTTime;
}

void CCore::startRecording(const long& subScanId) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::ComponentNotActiveExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,
		ManagementErrors::DataTransferSetupErrorExImpl)
{
	startRecording(0,subScanId);
}

void CCore::startRecording(const ACS::Time& startTime,const long& subScanId) throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		  ComponentErrors::UnexpectedExImpl,ManagementErrors::BackendNotAvailableExImpl, ManagementErrors::DataTransferSetupErrorExImpl,
		  ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
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
}
