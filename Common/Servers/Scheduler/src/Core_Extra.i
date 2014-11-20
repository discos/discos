void CCore::receiversNCHandler(Receivers::ReceiversDataBlock receiversData,void *handlerParam)
{
	CCore *me=static_cast<CCore *>(handlerParam);
	if (receiversData.status!=Management::MNG_OK) {
		me->m_isReceiversTracking=false;
	}
	else {
		me->m_isReceiversTracking=receiversData.tracking;
	}
}

void CCore::antennaNCHandler(Antenna::AntennaDataBlock antennaData,void *handlerParam)
{
	CCore *me=static_cast<CCore *>(handlerParam);
	if (antennaData.status!=Management::MNG_OK) {
		me->m_isAntennaTracking=false;
	}
	else {
		me->m_isAntennaTracking=antennaData.tracking;
	}
}

void CCore::minorServoNCHandler(MinorServo::MinorServoDataBlock servoData,void *handlerParam)
{
	CCore *me=static_cast<CCore *>(handlerParam);
	if (servoData.status!=Management::MNG_OK) {
		me->m_isMinorServoTracking=false;
	}
	else {
		me->m_isMinorServoTracking=servoData.tracking;
	}
}

bool CCore::remoteCall(const IRA::CString& command,const IRA::CString& package,const long& par,IRA::CString& out) throw (ParserErrors::PackageErrorExImpl,ManagementErrors::UnsupportedOperationExImpl)
{
	char * ret_val;
	CORBA::Boolean res;
	switch (par) {
		case 1: { //antenna package
			try {
				baci::ThreadSyncGuard guard(&m_mutex);  //loadAntennaBoss works on class data so it is safe to sync
				loadAntennaBoss(m_antennaBoss,m_antennaBossError);
			}
			catch (ComponentErrors::CouldntGetComponentExImpl& err) { //this exception can be thrown by the loadAntennaBoss()
				_ADD_BACKTRACE(ParserErrors::PackageErrorExImpl,impl,err,"CCore::remoteCall()");
				impl.setPackageName((const char *)package);
				throw impl;
			}
			try {
				res=m_antennaBoss->command((const char *)command,ret_val); // throw CORBA::SystemException
				out=IRA::CString(ret_val);
				CORBA::string_free(ret_val);
				return res;
			}
			catch (CORBA::SystemException& err) {
				_EXCPT(ParserErrors::PackageErrorExImpl,impl,"CCore::command()");
				impl.setPackageName((const char *)package);
				m_antennaBossError=true;
				throw impl;
			}
			break;
		}
		case 2: { //receivers package
			try {
				baci::ThreadSyncGuard guard(&m_mutex);  //loadReceiversBoss works on class data so it is safe to sync
				loadReceiversBoss(m_receiversBoss,m_receiversBossError);
			}
			catch (ComponentErrors::CouldntGetComponentExImpl& err) { //this exception can be thrown by the loadReceiversBoss()
				_ADD_BACKTRACE(ParserErrors::PackageErrorExImpl,impl,err,"CCore::remoteCall()");
				impl.setPackageName((const char *)package);
				throw impl;
			}
			try {
				res=m_receiversBoss->command((const char *)command,ret_val); // throw CORBA::SystemException
				out=IRA::CString(ret_val);
				CORBA::string_free(ret_val);
				return res;
			}
			catch (CORBA::SystemException& err) {
				_EXCPT(ParserErrors::PackageErrorExImpl,impl,"CCore::command()");
				impl.setPackageName((const char *)package);
				m_receiversBossError=true;
				throw impl;
			}
			break;
		}
		case 3: { // default backend..
			Backends::GenericBackend_var backend;
			//backend=m_schedExecuter->getBackendReference(); //get the reference to the currently used backend.
			try {
				/*if (CORBA::is_nil(backend)) {*/
				baci::ThreadSyncGuard guard(&m_mutex);
				loadDefaultBackend(); // throw ComponentErrors::CouldntGetComponentExImpl& err)
				backend=m_defaultBackend;
				/*}*/
			}
			catch (ComponentErrors::CouldntGetComponentExImpl& err) { //this exception can be thrown by the loadDefaultBackend()
				_ADD_BACKTRACE(ParserErrors::PackageErrorExImpl,impl,err,"CCore::remoteCall()");
				impl.setPackageName((const char *)package);
				throw impl;
			}
			try {
				res=backend->command((const char *)command,ret_val); // throw CORBA::SystemException
				out=IRA::CString(ret_val);
				CORBA::string_free(ret_val);
				return res;
			}
			catch (CORBA::SystemException& err) {
				_EXCPT(ParserErrors::PackageErrorExImpl,impl,"CCore::command()");
				impl.setPackageName((const char *)package);
				m_defaultBackendError=true;
				throw impl;
			}
			break;
		}
		case 4: { //minor servo package
			try {
				baci::ThreadSyncGuard guard(&m_mutex);
				loadMinorServoBoss(m_minorServoBoss,m_minorServoBossError);
				if (CORBA::is_nil(m_minorServoBoss)) {
					_EXCPT(ManagementErrors::UnsupportedOperationExImpl,impl,"CCore::remoteCall()");
					throw impl;
				}
			}
			catch (ComponentErrors::CouldntGetComponentExImpl& err) {
				_ADD_BACKTRACE(ParserErrors::PackageErrorExImpl,impl,err,"CCore::remoteCall()");
				impl.setPackageName((const char *)package);
				throw impl;
			}
			try {
				res=m_minorServoBoss->command((const char *)command,ret_val); // throw CORBA::SystemException
				out=IRA::CString(ret_val);
				CORBA::string_free(ret_val);
				return res;
			}
			catch (CORBA::SystemException& err) {
				_EXCPT(ParserErrors::PackageErrorExImpl,impl,"CCore::command()");
				impl.setPackageName((const char *)package);
				m_minorServoBossError=true;
				throw impl;
			}
			break;
		}
		case 5: { //active surface  package
				try {
					baci::ThreadSyncGuard guard(&m_mutex);
					loadActiveSurfaceBoss(m_activeSurfaceBoss,m_activeSurfaceBossError);
					if (CORBA::is_nil(m_activeSurfaceBoss)) {
						_EXCPT(ManagementErrors::UnsupportedOperationExImpl,impl,"CCore::remoteCall()");
						throw impl;
					}
				}
				catch (ComponentErrors::CouldntGetComponentExImpl& err) {
					_ADD_BACKTRACE(ParserErrors::PackageErrorExImpl,impl,err,"CCore::remoteCall()");
					impl.setPackageName((const char *)package);
					throw impl;
				}
				try {
					res=m_activeSurfaceBoss->command((const char *)command,ret_val); // throw CORBA::SystemException
					out=IRA::CString(ret_val);
					CORBA::string_free(ret_val);
					return res;
				}
				catch (CORBA::SystemException& err) {
					_EXCPT(ParserErrors::PackageErrorExImpl,impl,"CCore::command()");
					impl.setPackageName((const char *)package);
					m_activeSurfaceBossError=true;
					throw impl;
				}
				break;
			}
		default: {
			_EXCPT(ParserErrors::PackageErrorExImpl,impl,"CCore::remoteCall()");
			impl.setPackageName((const char *)package);
			throw impl;
		}
	}
}

void CCore::waitUntilHandler(const ACS::Time& time,const void *par)
{
	long *done;
	done=static_cast<long *>(const_cast<void *>(par));
	*done=1;
}



/*
 void CCore::crossScan(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration) throw (ManagementErrors::NotAllowedDuringScheduleExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,
		ManagementErrors::TargetIsNotVisibleExImpl,ManagementErrors::TsysErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl,
		ManagementErrors::AntennaScanErrorExImpl,ComponentErrors::TimerErrorExImpl)
{
	//no need to get the mutex, because it is already done inside the executor object
	if (m_schedExecuter) {
		if (m_schedExecuter->isScheduleActive()) {
			_THROW_EXCPT(ManagementErrors::NotAllowedDuringScheduleExImpl,"CCore::crossScan()");
		}
	}
	// let's create the scans.....filling up the required fields :-)
	CORBA::Double bwhm=0.017453; // one degree in radians
	CORBA::Double offset;
	//Management::TScanAxis scanAxis;
	IRA::CString obsName,prj,suffix,path,extraPath,baseName;
	IRA::CString layoutName,schedule;
	ACS::stringSeq layout;
	//long scanTag=0;
	//long scanID=1,subScanID;
	TIMEVALUE now;
	ACS::Time waitFor;
	//long long waitMicro;
	ACS::Time startTime;

	//obsName=IRA::CString("system");
	//prj=IRA::CString("pointingCrossScan");
	//getProjectCode(prj);
	//layout.length(0);
	//layoutName=_SCHED_NULLTARGET;
	//schedule="none";
	//targetID="crossScanTarget";

	// now take the mutex
	baci::ThreadSyncGuard guard(&m_mutex);
	//make sure the antenna is available.
	loadAntennaBoss(m_antennaBoss,m_antennaBossError); // throw ComponentErrors::CouldntGetComponentExImpl
	try {
		ACSErr::Completion_var comp;
		ACS::ROdouble_var ref=m_antennaBoss->FWHM();
		if (!CORBA::is_nil(ref)) {
			CORBA::Double tmp;
			tmp=ref->get_sync(comp.out());
			ACSErr::CompletionImpl compImpl(comp);
			if (compImpl.isErrorFree()) {
				bwhm=tmp;
			}
		}
		//ACS::ROstring_var targetRef=m_antennaBoss->target();
		//if (!CORBA::is_nil(targetRef)) {
		//	CORBA::String_var  tmp;
		//	tmp=targetRef->get_sync(comp.out());
		//	ACSErr::CompletionImpl compImpl(comp);
		//	if (compImpl.isErrorFree()) {
		//		targetID=(const char *)tmp;
		//	}
		//}
	}
	catch (...) {
		//in this case we do not want to to do nothing.....it is an error but we can survive
	}
	offset=bwhm*5; // the offset (sky) five times the antenna beam
	// TSYS SCAN
	ACS_LOG(LM_FULL_INFO,"CCore::crossScan()",(LM_NOTICE,"TSYS_COMPUTATION"));
	try {
		if (!CORBA::is_nil(m_antennaBoss)) {
			m_antennaBoss->goOff(scanFrame,offset);
		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::crossScan()");
			throw impl;
		}
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::AntennaScanErrorExImpl,impl,ex,"CCore::crossScan()");
		throw impl;
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::AntennaScanErrorExImpl,impl,ex,"CCore::crossScan()");
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::crossScan()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_antennaBossError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::crossScan()");
		m_antennaBossError=true;
		throw impl;
	}
	clearAntennaTracking();
	guard.release();
	waitOnSource();
	try {
		ACS::doubleSeq tsys;
		callTSys(tsys);
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		_ADD_BACKTRACE(ManagementErrors::TsysErrorExImpl,impl,ex,"CCore::crossScan()");
		throw impl;
	}
	IRA::CIRATools::Wait(2,0);
	guard.acquire();

	initRecording(1);

	ACS_LOG(LM_FULL_INFO,"CCore::crossScan()",(LM_NOTICE,"LATITUDE_SCAN"));
	try {
		if (!CORBA::is_nil(m_antennaBoss)) {
			startTime=m_antennaBoss->latOTFScan(scanFrame,span,duration);
		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::crossScan()");
			throw impl;
		}
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::AntennaScanErrorExImpl,impl,ex,"CCore::crossScan()");
		throw impl;
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::AntennaScanErrorExImpl,impl,ex,"CCore::crossScan()");
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::crossScan()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_antennaBossError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::crossScan()");
		m_antennaBossError=true;
		throw impl;
	}
	// throw  (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::ComponentNotActiveExImpl,
	//ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,
	//ManagementErrors::DataTransferSetupErrorExImpl)

	startRecording(startTime,1);

	//if (scanFrame==Antenna::ANT_HORIZONTAL) {
	//	scanAxis=Management::MNG_HOR_LAT;
	//}
	//else if (scanFrame==Antenna::ANT_EQUATORIAL) {
	//	scanAxis=Management::MNG_EQ_LAT;
	//}
	//else {
	//	scanAxis=Management::MNG_GAL_LAT;
	//}
	//path=m_config->getSystemDataDirectory()+prj+"/";
	//suffix=targetID;
	//baseName=CCore::computeOutputFileName(startTime,m_site,m_dut1,prj,suffix,extraPath);
	//subScanID=1;
	// throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl)
	//CCore::setupDataTransfer(m_scanStarted,m_defaultDataReceiver.in(),m_defaultDataReceiverError,m_defaultBackend.in(),m_defaultBackendError,
	//		obsName,prj,baseName,path,extraPath,schedule,targetID,layoutName,layout,scanTag,m_currentDevice,scanID,startTime,subScanID,scanAxis);
	// start the recording or data transfer
	// throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::UnexpectedExImpl,ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl)
	//CCore::startDataTansfer(m_defaultBackend.in(),m_defaultBackendError,startTime,m_streamStarted,m_streamPrepared,m_streamConnected);

	// now set the the data transfer stop
	waitFor=startTime+duration; // this is the time at which the stop should be issued
	//waitMicro=(duration/10)/10000; // one tens of the total duration
	//IRA::CIRATools::getTime(now);
	guard.release();
	waitUntil(waitFor); // throw ComponentErrors::TimerErrorExImpl
	//while (now.value().value<waitFor) {
	//	IRA::CIRATools::Wait(waitMicro);
	//	IRA::CIRATools::getTime(now);
	//}
	//guard.acquire();
	stopRecording();

	// throw (ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl)
	//CCore::stopDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_streamStarted,m_streamPrepared,m_streamConnected);
	// wait for a data transfer to complete before start with the latitude scan
	//guard.release();
	//while (checkRecording(m_defaultDataReceiver.in(),m_defaultDataReceiverError)) {
	//	IRA::CIRATools::Wait(0,250000); // 0.25 seconds
	//}

	guard.acquire();
	// LONGITUDE SCAN..............
	// now lets go and check the lon scan....typically if a source was commanded before....and it is above the horizon and the scan could be performed
	ACS_LOG(LM_FULL_INFO,"CCore::crossScan()",(LM_NOTICE,"LONGITUDE_SCAN"));
	try {
		if (!CORBA::is_nil(m_antennaBoss)) {
			startTime=m_antennaBoss->lonOTFScan(scanFrame,span,duration);
		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::crossScan()");
			throw impl;
		}
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::AntennaScanErrorExImpl,impl,ex,"CCore::crossScan()");
		throw impl;
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::AntennaScanErrorExImpl,impl,ex,"CCore::crossScan()");
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::crossScan()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_antennaBossError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::crossScan()");
		m_antennaBossError=true;
		throw impl;
	}
	startRecording(startTime,2);


	//if (scanFrame==Antenna::ANT_HORIZONTAL) {
	//	scanAxis=Management::MNG_HOR_LON;
	//}
	//else if (scanFrame==Antenna::ANT_EQUATORIAL) {
	//	scanAxis=Management::MNG_EQ_LON;
	//}
	//else {
	//	scanAxis=Management::MNG_GAL_LON;
	//}
	//suffix=targetID;
	//baseName=CCore::computeOutputFileName(startTime,m_site,m_dut1,prj,suffix,extraPath);
	//subScanID=2;
	// throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl)
	//CCore::setupDataTransfer(m_scanStarted,m_defaultDataReceiver.in(),m_defaultDataReceiverError,m_defaultBackend.in(),m_defaultBackendError,
	//		obsName,prj,baseName,path,extraPath,schedule,targetID,layoutName,layout,scanTag,m_currentDevice,scanID,startTime,subScanID,scanAxis);
	// start the recording or data transfer
	// throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::UnexpectedExImpl,ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl)
	//CCore::startDataTansfer(m_defaultBackend.in(),m_defaultBackendError,startTime,m_streamStarted,m_streamPrepared,m_streamConnected);


	// now set the the data transfer stop
	waitFor=startTime+duration; // this is the time at which the stop should be issued
	//waitMicro=(duration/10)/10000; // one tens of the total duration
	//IRA::CIRATools::getTime(now);
	guard.release();
	waitUntil(waitFor);
	//while (now.value().value<waitFor) {
	//	IRA::CIRATools::Wait(waitMicro);
	//	IRA::CIRATools::getTime(now);
	//}
	//guard.acquire();
	stopRecording();

	// throw (ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl)
	//CCore::stopDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_streamStarted,m_streamPrepared,m_streamConnected);
	// wait for a couple of seconds before start with the latitude scan
	//guard.release();
	//bool check=checkRecording(m_defaultDataReceiver.in(),m_defaultDataReceiverError);
	//while (checkRecording(m_defaultDataReceiver.in(),m_defaultDataReceiverError)) {
	//	IRA::CIRATools::Wait(0,250000); // 0.25 seconds
	//}

	guard.acquire();

	// throw (ComponentErrors::OperationErrorExImpl)
	//CCore::stopScan(m_defaultDataReceiver.in(), m_defaultDataReceiverError,m_scanStarted);
	//CCore::disableDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_defaultDataReceiver.in(),m_defaultDataReceiverError,m_streamStarted,m_streamPrepared,m_streamConnected,m_scanStarted);
	terminateScan();
	ACS_LOG(LM_FULL_INFO,"CCore::crossScan()",(LM_NOTICE,"CROSSSCAN_DONE"));
}
 */

