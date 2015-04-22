
bool CCore::isStreamStarted() const
{
	return m_streamStarted;
}

void CCore::clearTracking()
{
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	m_clearTrackingTime=now.value().value;
}

void CCore::changeSchedulerStatus(const Management::TSystemStatus& status)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (status>=m_schedulerStatus) m_schedulerStatus=status;
}

bool CCore::checkScan(ACS::Time& ut,const Antenna::TTrackingParameters *const prim,const Antenna::TTrackingParameters *const sec,
	const MinorServo::MinorServoScan*const servoPar,const Receivers::TReceiversParameters*const recvPar,const double& minEl,
	const double& maxEl) throw (ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,
	ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntGetComponentExImpl,
	ManagementErrors::UnsupportedOperationExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	//ACS::Time antennaUT,receiversUT;
	bool antennaAnswer,receiversAnswer,minorServoAnswer;
	//antennaUT=receiversUT=ut;
	TIMEVALUE now;
	loadAntennaBoss(m_antennaBoss,m_antennaBossError); // throw ComponentErrors::CouldntGetComponentExImpl
	if (CORBA::is_nil(m_antennaBoss)) {
		_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::checkScan()");
		throw impl;
	}
	try {
		//antennaUT will stores the estimated start time from the antenna for all kind of subscans
		antennaAnswer=m_antennaBoss->checkScan(ut,*prim,*sec,minEl,maxEl,m_antennaRTime.out());
		ACS_LOG(LM_FULL_INFO,"CCore::checkScan()",(LM_DEBUG,"SLEWING_TIME %lld :",m_antennaRTime->slewingTime));
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::checkScan()");
		impl.setReason("Could not check scan with antenna boss");
		throw impl;
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::checkScan()");
		impl.setReason("Could not check scan with antenna boss");
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
	if ((MINOR_SERVO_AVAILABLE)) { // check if the minor servo system is enabled
		loadMinorServoBoss(m_minorServoBoss,m_minorServoBossError); // (ComponentErrors::CouldntGetComponentExImpl)
		if (CORBA::is_nil(m_minorServoBoss)) {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::checkScan()");
			throw impl;
		}
		try {
			minorServoAnswer=m_minorServoBoss->checkScan(ut,*servoPar,m_antennaRTime.in(),m_servoRunTime.out());
		}
		catch (MinorServoErrors::MinorServoErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::checkScan()");
			impl.setReason("Could not check the minor servo scan");
			throw impl;
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::checkScan()");
			impl.setReason("Could not check the minor servo scan");
			throw impl;
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::checkScan()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			m_minorServoBossError=true;
			throw impl;
		}
		if (!minorServoAnswer) return minorServoAnswer;
	}
	else { // otherwise the scan is always considered "feasible"
		minorServoAnswer=true;
		m_servoRunTime->startEpoch=0;
		m_servoRunTime->onTheFly=false;
		if (!servoPar->is_empty_scan) { // if the minor servos are not supported and an effective scan is commanded.
			_EXCPT(ManagementErrors::UnsupportedOperationExImpl,impl,"CCore::checkScan()");
		}
	}
	loadReceiversBoss(m_receiversBoss,m_receiversBossError);
	if (CORBA::is_nil(m_receiversBoss)) {
		_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::checkScan()");
		throw impl;
	}
	try {
		receiversAnswer=m_receiversBoss->checkScan(ut,*recvPar,m_antennaRTime.in(),m_receiversRunTime);
		if (!receiversAnswer) return receiversAnswer;
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
	if (ut==0) {  //if the request was to start as soon as possible, we need to do some computation and change the effective start time
		/*******************************************************/
		/* At the moment I do not consider the case m_receiversRunTime.onTheFly==true to be a realistic case, so it is not
		 * considered in the "if then else" sequence below. it the things change I have to deal with them according the following table.
		 * When scans of secondary subsystem are only involved (i.e. focus scan) the transfer time of the antenna have to be taken into
		 * consideration as well.
		 * Ant.otf	servo.otf	recv.otf	real?	=>	UT		 			  (comment)
		 *    N			N			N		 y			0    	 			  (beam park, wait tracking flag...)
		 *    N			N			Y		 n          max(recv.ut,ant.ut)
		 *    N			Y			N		 y		    max(servo.ut,ant.ut)  (focus scan)
		 *    N			Y			Y        n          max(recv.ut,servo.ut)
		 *    Y			N			N		 y			ant.ut				  (OnTheFly scan)
		 *    Y			N			Y		 n			max(ant.ut,recv,ut)
		 *    Y			Y			N		 y			max(ant.ut,servo.ut)  (scan along subr.y)
		 *    Y			Y			Y        n			max(all)*/
		/* ****************************************************************** */
		if ((!m_antennaRTime->onTheFly) && (!m_servoRunTime->onTheFly)) { //simple beampark scan, for example
			// do nothing as ut should be zero as well, i.e the system must wait for the tracking flag to be true
		}
		else if ((!m_antennaRTime->onTheFly) && (m_servoRunTime->onTheFly)) { //typical case is focus scan...
			ut=MAX(m_antennaRTime->startEpoch,m_servoRunTime->startEpoch)+2000000; // add 0.2 sec. as overhead
		}
		else if ((m_antennaRTime->onTheFly) && (!m_servoRunTime->onTheFly)) { //typical case of OTF scan...the resulting time is the one comingo from the antenna
			ut=m_antennaRTime->startEpoch;
		}
		else { // both are OTF...typically a scan along a subreflector axis (Y for example)
			// the correct start time is the maximum of the two times
			ut=MAX(m_antennaRTime->startEpoch,m_servoRunTime->startEpoch)+2000000; // add 0.2 sec. as overhead
		}
	}  // otherwise keep the original starting time, which is the one requested by the user
	return true;
}

void CCore::doScan(ACS::Time& ut,const Antenna::TTrackingParameters * const prim,const Antenna::TTrackingParameters *const sec,
		const MinorServo::MinorServoScan*const servoPar,const Receivers::TReceiversParameters*const recvPa) throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,
				ComponentErrors::UnexpectedExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	ACS::Time antennaUT,servoUT,receiversUT,newUT;
	m_subScanStarted=true;
	loadAntennaBoss(m_antennaBoss,m_antennaBossError); // throw ComponentErrors::CouldntGetComponentExImpl
	if (CORBA::is_nil(m_antennaBoss)) {
		_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::doScan()");
		throw impl;
	}
	try {
		antennaUT=ut;
		m_antennaBoss->startScan(antennaUT,*prim,*sec); // the ut could be modified by the call
		ACS_LOG(LM_FULL_INFO,"CCore::doScan()",(LM_DEBUG,"ANTENNA_SCAN_EPOCH %lld",antennaUT));
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
	if ((MINOR_SERVO_AVAILABLE)) { // check if the minor servo system is enabled
		loadMinorServoBoss(m_minorServoBoss,m_minorServoBossError); // (ComponentErrors::CouldntGetComponentExImpl)
		if (CORBA::is_nil(m_minorServoBoss)) {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::doScan()");
			throw impl;
		}
		try {
			servoUT=ut;
			m_minorServoBoss->startScan(servoUT,*servoPar,m_antennaRTime.in());
			ACS_LOG(LM_FULL_INFO,"CCore::doScan()",(LM_DEBUG,"MINOR_SERVO_SCAN_EPOCH %lld",servoUT));
		}
		catch (MinorServoErrors::MinorServoErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::doScan()");
			impl.setReason("Could not start the minor servo scan");
			throw impl;
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::doScan()");
			impl.setReason("Could not start the minor servo scan");
			throw impl;
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::doScan()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			m_minorServoBossError=true;
			throw impl;
		}
	}
	else {
		servoUT=0;
	}
	loadReceiversBoss(m_receiversBoss,m_receiversBossError);
	if (CORBA::is_nil(m_receiversBoss)) {
		_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::doScan()");
		throw impl;
	}
	try {
		receiversUT=ut;
		m_receiversBoss->startScan(receiversUT,*recvPa,m_antennaRTime.in());
		ACS_LOG(LM_FULL_INFO,"CCore::doScan()",(LM_DEBUG,"RECEIEVERS_SCAN_EPOCH %lld",receiversUT));
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
	newUT=GETMAX3(receiversUT,servoUT,antennaUT);
	if (ut!=newUT) {
		ACS_LOG(LM_FULL_INFO,"CCore::doScan()",(LM_WARNING,"FAIL_TO_HANDSHAKE_SCAN_START_EPOCH"));
		ut=newUT;
		IRA::CString timeTag;
		IRA::CIRATools::timeToStr(ut,timeTag);
		ACS_LOG(LM_FULL_INFO,"CCore::doScan()",(LM_NOTICE,"NEW_NEGOTIATED_EPOCH %s",(const char *)timeTag));
	}
}

void CCore::startScan(ACS::Time& time,const Antenna::TTrackingParameters *const prim,const Antenna::TTrackingParameters *const sec,
  const MinorServo::MinorServoScan*const servoPar,const Receivers::TReceiversParameters*const recvPar) throw (
  ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl,ManagementErrors::CloseTelescopeScanErrorExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	try {
		closeScan(true);
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		_ADD_BACKTRACE(ManagementErrors::CloseTelescopeScanErrorExImpl,impl,ex,"CCore::startScan()");
		throw impl;
	}
	try {
		if (!checkScan(time,prim,sec,servoPar,recvPar)) {
			_EXCPT(ManagementErrors::TargetOrSubscanNotFeasibleExImpl,impl,"CCore::startScan()");
			throw impl;
		}
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		_ADD_BACKTRACE(ManagementErrors::TelescopeSubScanErrorExImpl,impl,ex,"CCore::startScan()");
		impl.setReason("cannot check sub scan correctness");
		throw impl;
	}
	try {
		doScan(time,prim,sec,servoPar,recvPar);
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		_ADD_BACKTRACE(ManagementErrors::TelescopeSubScanErrorExImpl,impl,ex,"CCore::startScan()");
		impl.setReason("cannot start sub scan");
		throw impl;
	}
	clearTracking();
}

void CCore::goOff(const Antenna::TCoordinateFrame& frame,const double& beams) throw (ComponentErrors::CouldntGetComponentExImpl,
		ComponentErrors::ComponentNotActiveExImpl,ManagementErrors::AntennaScanErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	loadAntennaBoss(m_antennaBoss,m_antennaBossError); // throw ComponentErrors::CouldntGetComponentExImpl
	try {
		if (!CORBA::is_nil(m_antennaBoss)) {
			m_antennaBoss->goOff(frame,beams);
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
}

ACS::Time CCore::closeScan(bool wait) throw (ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::OperationErrorExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::TimerErrorExImpl,ManagementErrors::AbortedByUserExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	ACS::Time antennaUT,servoUT,receiversUT,newUT;
	if (!m_subScanStarted) {
		return 0;
	}
	ACS_LOG(LM_FULL_INFO,"CCore::closeScan()",(LM_DEBUG,"CLOSING_CURRENT_TELESCOPE_SCAN"));
	loadAntennaBoss(m_antennaBoss,m_antennaBossError); // throw ComponentErrors::CouldntGetComponentExImpl
	try {
		if (!CORBA::is_nil(m_antennaBoss)) {
			m_antennaBoss->closeScan(antennaUT); // the ut could be modified by the call
			ACS_LOG(LM_FULL_INFO,"CCore::closeScan()",(LM_DEBUG,"ANTENNA_CLOSE_SCAN_EPOCH %lld",antennaUT));
			IRA::CString outstr;
			IRA::CIRATools::timeToStr(antennaUT,outstr);
			printf("tempo di chiusura (Antenna): %s\n",(const char*)outstr);
		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::closeScan()");
			throw impl;
		}
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::closeScan()");
		impl.setReason("Could not close the antenna scan");
		throw impl;
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::closeScan()");
		impl.setReason("Could not close the antenna scan");
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::closeScan()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_antennaBossError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::closeScan()");
		m_antennaBossError=true;
		throw impl;
	}
	if ((MINOR_SERVO_AVAILABLE)) { // check if the minor servo system is enabled
		loadMinorServoBoss(m_minorServoBoss,m_minorServoBossError); // (ComponentErrors::CouldntGetComponentExImpl)
		try {
			if (!CORBA::is_nil(m_minorServoBoss)) {
				m_minorServoBoss->closeScan(servoUT);
				ACS_LOG(LM_FULL_INFO,"CCore::closeScan()",(LM_DEBUG,"MINOR_SERVO_CLOSE_SCAN_EPOCH %lld",servoUT));
				IRA::CString outstr;
				IRA::CIRATools::timeToStr(servoUT,outstr);
				printf("tempo di chiusura (Servo): %s\n",(const char*)outstr);
			}
			else {
				_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::closeScan()");
				throw impl;
			}
		}
		catch (MinorServoErrors::MinorServoErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::closeScan()");
			impl.setReason("Could not close the minor servo scan");
			throw impl;
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::closeScan()");
			impl.setReason("Could not close the minor servo scan");
			throw impl;
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::closeScan()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			m_minorServoBossError=true;
			throw impl;
		}
	}
	else {
		servoUT=0;
	}
	loadReceiversBoss(m_receiversBoss,m_receiversBossError);
	try {
		if (!CORBA::is_nil(m_receiversBoss)) {
			m_receiversBoss->closeScan(receiversUT);
			ACS_LOG(LM_FULL_INFO,"CCore::closeScan()",(LM_DEBUG,"RECEIEVERS_STOP_SCAN_EPOCH %lld",receiversUT));
			IRA::CString outstr;
			IRA::CIRATools::timeToStr(receiversUT,outstr);
			printf("tempo di chiusura (receievers): %s\n",(const char*)outstr);
		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::closeScan()");
			throw impl;
		}
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::closeScan()");
		impl.setReason("Could not close to the receivers scan");
		throw impl;
	}
	catch (ReceiversErrors::ReceiversErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::closeScan()");
		impl.setReason("Could not close to the receivers scan");
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::closeScan()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_receiversBossError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::closeScan()");
		m_receiversBossError=true;
		throw impl;
	}
	m_subScanStarted=false;
	newUT=GETMAX3(receiversUT,servoUT,antennaUT);
	if (wait) {
		TIMEVALUE now;
		IRA::CIRATools::getTime(now);
		if ((newUT!=0) && (newUT>now.value().value)) {
			guard.release();
			_waitUntil(newUT); // throw (ComponentErrors::TimerErrorExImpl,ManagementErrors::AbortedByUserExImpl)
		}
	}
	return newUT;
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
	targetID=m_antennaRTime->targetName;
	if (suffix=="") {
		if (targetID=="") {
			newSuffix="none";
		}
		else {
			newSuffix=targetID;
		}
	}
	else {
		newSuffix=suffix;
	}
	baseName=CCore::computeOutputFileName(startUTTime,m_site,m_dut1,projectCode,newSuffix,extraPath);
	// compute the axis or direction along which the scan is performed
	scanAxis=computeScanAxis(); //  throw (ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
	loadDefaultBackend();// throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::OperationErrorExImpl);
	loadDefaultDataReceiver();
	CCore::setupDataTransfer(m_dataTransferInitialized,m_defaultDataReceiver.in(),m_defaultDataReceiverError,m_defaultBackend.in(),m_defaultBackendError,
			m_streamPrepared,observerName,projectName,baseName,path,extraPath,scheduleFileName,targetID,layout,layoutProc,scanTag,getCurrentDevice(),scanId,startUTTime ,subScanId,scanAxis,
			m_config); // throws  ComponentErrors::OperationErrorExImpl,ComponentErrors::UnexpectedExImpl,ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl
	CCore::startDataTansfer(m_defaultBackend.in(),m_defaultBackendError,recUt,m_streamStarted,m_streamPrepared,m_streamConnected);
	return startUTTime;
}

void CCore::configureBackend(const IRA::CString& name,const std::vector<IRA::CString>& procedure) throw (ManagementErrors::BackendProcedureErrorExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	loadDefaultBackend();// throw (ComponentErrors::CouldntGetComponentExImpl);
	CCore::configureBackend(m_defaultBackend.in(),m_defaultBackendError,name,procedure);
}

void CCore::disableDataTransfer() throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	// no need to check the backend and writer are active....when disabling the data transfer....
	//loadDefaultBackend();// throw (ComponentErrors::CouldntGetComponentExImpl);
	//loadDefaultDataReceiver();
	CCore::disableDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_defaultDataReceiver.in(),m_defaultDataReceiverError,m_streamStarted,m_streamPrepared,
			m_streamConnected,m_dataTransferInitialized);
}

void CCore::enableDataTransfer() throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::CouldntGetComponentExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	loadDefaultBackend();// throw (ComponentErrors::CouldntGetComponentExImpl);
	loadDefaultDataReceiver();
	CCore::enableDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_defaultDataReceiver.in(),m_streamConnected,m_streamPrepared);
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

Management::TScanAxis CCore::computeScanAxis() throw (ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::OperationErrorExImpl)
{
	Management::TScanAxis scanAxis_ant,scanAxis_ms;
	CORBA::String_var servo;
	scanAxis_ant=scanAxis_ms=Management::MNG_NO_AXIS;
	baci::ThreadSyncGuard guard(&m_mutex);
	/*loadAntennaBoss(m_antennaBoss,m_antennaBossError); // throw ComponentErrors::CouldntGetComponentExImpl
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
	}*/
	scanAxis_ant=m_antennaRTime->axis;
	servo=m_servoRunTime->scanAxis;
	scanAxis_ms=m_config->getAxisFromServoName(servo.in());
	/*loadMinorServoBoss(m_minorServoBoss,m_minorServoBossError); // throw ComponentErrors::CouldntGetComponentExImpl
	try {
		if (!CORBA::is_nil(m_minorServoBoss)) {
			servo=m_minorServoBoss->getScanAxis();
			scanAxis_ms=m_config->getAxisFromServoName(servo.in());
		}
	}
	catch (MinorServoErrors::MinorServoErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::computeScanAxis()");
		impl.setReason("Could not start the minor servo scan");
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::computeScanAxis()");
		impl.setReason("Could not start the minor servo scan");
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::computeScanAxis()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_minorServoBossError=true;
		throw impl;
	}*/
	// this choice has to be reconsidered, does the minor servo movement always prevail?
	if (scanAxis_ms!=Management::MNG_NO_AXIS) { // if a minor servo has been selected, the resulting scan axis is the axis mapped to the servo
		return scanAxis_ms;
	}
	else {
		return scanAxis_ant;
	}
}

