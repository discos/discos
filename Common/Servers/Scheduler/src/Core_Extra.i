
void CCore::wait(const double& seconds) const
{
	int sec;
	long micro;
	sec=(int)seconds;
	micro=(long)((seconds-(double)sec)*1000000.0);
	IRA::CIRATools::Wait(sec,micro);
}

void CCore::waitUntilHandler(const ACS::Time& time,const void *par)
{
	long *done;
	done=static_cast<long *>(const_cast<void *>(par));
	*done=1;
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

int CCore::status() const
{
	if (isTracking()) {
		return 0;
	}
	else {
		return 1;
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

void CCore::startRecording(const long& subScanId) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::ComponentNotActiveExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,
		ManagementErrors::DataTransferSetupErrorExImpl)
{
	startRecording(0,subScanId);
}

void CCore::startRecording(const ACS::Time& startTime,const long& subScanId) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::ComponentNotActiveExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,
		ManagementErrors::DataTransferSetupErrorExImpl)
{
	IRA::CString extraPath,prj,path,suffix,obsName,schedule,targetID,layoutName,baseName;
	ACS::stringSeq layout;
	long scanTag;
	Management::TScanAxis scanAxis;
	ACS::Time ut;
	// now take the mutex
	baci::ThreadSyncGuard guard(&m_mutex);
	//************************************************************************************************************************
	//*********************CHECK if a schedule is running or if the  recording is not initialized (streamConnected and streamPrepared) in that case we must give up
	//************************************************************************************************************************
	getProjectCode(prj);
	path=m_config->getSystemDataDirectory()+prj+"/";
	obsName=IRA::CString("system");
	suffix="default";
	schedule="none";
	layout.length(0);
	layoutName=_SCHED_NULLTARGET;
	scanTag=0;
	// get file name suffix!
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
		//in this case we do not want to to do nothing.....it is an error but we can survive
	}
	suffix=targetID;
	// computes the name of the file..............................
	if (startTime==0) {
		TIMEVALUE now;
		ut=now.value().value;
	}
	else {
		ut=startTime;
	}
	//throw (ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
	baseName=CCore::computeOutputFileName(ut,m_site,m_dut1,prj,suffix,extraPath);
	//get the scan axis
	scanAxis=CCore::computeScanAxis(m_antennaBoss,m_antennaBossError,m_minorServoBoss,m_receiversBossError,*m_config); // throw (ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
	// throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl)
	CCore::setupDataTransfer(m_scanStarted/*,m_streamPrepared*/,m_defaultDataReceiver.in(),m_defaultDataReceiverError,m_defaultBackend.in(),m_defaultBackendError,
			obsName,prj,baseName,path,extraPath,schedule,targetID,layoutName,layout,scanTag,m_currentDevice,m_scanID,startTime,subScanId,scanAxis,m_config);
	// start the recording or data transfer
	// throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::UnexpectedExImpl,ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl)
	CCore::startDataTansfer(m_defaultBackend.in(),m_defaultBackendError,startTime,m_streamStarted,m_streamPrepared,m_streamConnected);
}

void CCore::stopRecording() throw (ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl)
{
	// now take the mutex
	baci::ThreadSyncGuard guard(&m_mutex);
	// throw (ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl)
	CCore::stopDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_streamStarted,m_streamPrepared,m_streamConnected);
	// wait for a data transfer to complete before start with the latitude scan
	while (checkRecording(m_defaultDataReceiver.in(),m_defaultDataReceiverError)) {  // throw (ComponentErrors::OperationErrorExImpl)
		guard.release();
		IRA::CIRATools::Wait(0,250000); // 0.25 seconds
		guard.acquire();
	}
}

void CCore::terminateRecording() throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	// throw (ComponentErrors::OperationErrorExImpl)
	CCore::stopScan(m_defaultDataReceiver.in(), m_defaultDataReceiverError,m_scanStarted);
	//throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
	CCore::disableDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_defaultDataReceiver.in(),m_defaultDataReceiverError,m_streamStarted,m_streamPrepared,m_streamConnected,m_scanStarted);
}
