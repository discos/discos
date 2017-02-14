
IRA::CString CCore::computeOutputFileName(const ACS::Time& ut,const ACS::TimeInterval& lst,const IRA::CString& prj,const IRA::CString& suffix,IRA::CString& extra)
{
	IRA::CString out;
	TIMEVALUE UT(ut);
	TIMEDIFFERENCE LST(lst);
	/*out.Format("%04d%02d%02d-%02d%02d%02d-%02d%02d%02d-%s_%s",
			UT.year(),UT.month(),UT.day(),UT.hour(),UT.minute(),UT.second(),
			LST.hour(),LST.minute(),LST.second(),(const char *)prj,(const char *)suffix);*/
	/* The LST removed from the name of the output file */
	out.Format("%04d%02d%02d-%02d%02d%02d-%s-%s",
			UT.year(),UT.month(),UT.day(),UT.hour(),UT.minute(),UT.second(),(const char *)prj,(const char *)suffix);
	extra.Format("%04d%02d%02d/",UT.year(),UT.month(),UT.day());
	return out;
}

IRA::CString CCore::computeOutputFileName(const ACS::Time& ut,const IRA::CSite& site,const double& dut1,const IRA::CString& prj,const IRA::CString& suffix,IRA::CString& extra)
{
	IRA::CString out;
	TIMEVALUE UT(ut);
	TIMEDIFFERENCE LST;
	/* The LST removed from the name of the output file */
	/*IRA::CDateTime::UT2Sidereal(LST,UT,site,dut1);*/
	out.Format("%04d%02d%02d-%02d%02d%02d-%s-%s",
			UT.year(),UT.month(),UT.day(),UT.hour(),UT.minute(),UT.second(),(const char *)prj,(const char *)suffix);
	extra.Format("%04d%02d%02d/",UT.year(),UT.month(),UT.day());
	return out;
}

ACS::Time CCore::getUTFromLST(const IRA::CDateTime& currentUT,const IRA::CDateTime& checkUT,const ACS::TimeInterval& lst,const IRA::CSite& site,const double& dut1)
{
	TIMEVALUE time;
	CDateTime ut1,ut2;
	TIMEDIFFERENCE myLst;
	double lstRad;
	myLst.value(lst);
	lstRad=(myLst.hour()*3600.0+myLst.minute()*60.0+myLst.second()+myLst.microSecond()/1000000.0)*DS2R;
	if (IRA::CDateTime::sidereal2UT(ut1,ut2,lstRad,site,currentUT.getYear(),currentUT.getMonth(),currentUT.getDay(),dut1)) {
		// one corresponding ut
		if (ut1.getJD()<checkUT.getJD()) { //if the ut has already elapsed
			// add one day
			IRA::CDateTime dayMore(currentUT.getJD()+1.0,dut1);
			return getUTFromLST(dayMore,checkUT,lst,site,dut1);
		}
		else {
			ut1.getDateTime(time);
			return time.value().value;
		}
	}
	else {
		// two corresponding ut;
		double jd1,jd2,jdNow;

		jd1=ut1.getJD(); jd2=ut2.getJD(); jdNow=checkUT.getJD();
		if ((jdNow>jd1) && (jdNow>jd2)) {
			IRA::CDateTime dayMore(currentUT.getJD()+1.0,dut1);
			return getUTFromLST(dayMore,checkUT,lst,site,dut1);
		}
		else if ((jdNow<jd1) && (jdNow<jd2)) {
			if (jd1==GETMIN(jd1,jd2)) {
				ut1.getDateTime(time);
				return time.value().value;
			}
			else {
				ut2.getDateTime(time);
				return time.value().value;
			}
		}
		else if  (jd2>jd1) {
			ut2.getDateTime(time);
			return time.value().value;
		}
		else {
			ut1.getDateTime(time);
			return time.value().value;
		}
	}
}

void CCore::configureBackend(Backends::GenericBackend_ptr backend,bool& backendError,const IRA::CString& name, const std::vector<IRA::CString>& procedure) throw (
		ManagementErrors::BackendProcedureErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
{
	char *answer;
	unsigned i;
	CORBA::Boolean res;
	IRA::CString message;
	if (!CORBA::is_nil(backend)) {
		for (i=0;i<procedure.size();i++) {
			try {
				res=backend->command((const char *)procedure[i],answer);
				message=answer;
				CORBA::string_free(answer);
				if (!res) {
					_EXCPT(ManagementErrors::BackendProcedureErrorExImpl,impl,"CCore::configureBackend()");
					impl.setMessage((const char*)message);
					impl.setProcedure((const char *)name);
					throw impl;
				}
			}
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::configureBackend()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				backendError=true;
				throw impl;
			}
			catch (...) {
				backendError=true;
				_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::configureBackend()");
				throw impl;
			}
		}
		ACS_STATIC_LOG(LM_FULL_INFO,"CCore::configureBackend()",(LM_NOTICE,"BACKEND_CONFIGURED"));
	}
}

void CCore::enableDataTransfer(Backends::GenericBackend_ptr backend,bool& backendError,Management::DataReceiver_ptr writer,bool& streamConnected,bool& streamPrepared) throw (
		ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
{
	if ((!CORBA::is_nil(backend))  &&  (!CORBA::is_nil(writer))) {
		try {
			if (!streamConnected) {
				backend->connect(writer);
				streamConnected=true;
			}
		}
		catch (ACSBulkDataError::AVConnectErrorEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::enableDataTransfer()");
			impl.setReason("backend could not be connected to writer");
			throw impl;
	 	}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::enableDataTransfer()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			backendError=true;
			throw impl;
		}
	 	catch (...) {
	 		backendError=true;
	 		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::enableDataTransfer()");
	 		throw impl;
	 	}
	}
	if (!CORBA::is_nil(backend)) {
		try {
			if (!streamPrepared) {
				backend->sendHeader();
				streamPrepared=true;
			}
		}
		catch (BackendsErrors::BackendsErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::enableDataTransfer()");
			impl.setReason("backend failed to send header to writer");
			throw impl;
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::enableDataTransfer()");
			impl.setReason("backend failed to send header to writer");
			throw impl;
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::enableDataTransfer()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			backendError=true;
			throw impl;
		}
		catch (...) {
			backendError=true;
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::enableDataTransfer()");
			throw impl;
		}
	}
}

void CCore::disableDataTransfer(Backends::GenericBackend_ptr backend,bool& backendError,Management::DataReceiver_ptr writer,bool& writerError,bool& streamStarted,bool& streamPrepared,
		bool& streamConnected,bool& scanStarted) throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
{
	bool forceReset=false;
	if (streamStarted) {
 		try {
 			streamStarted=false;
 			if (!CORBA::is_nil(backend)) {
 				backend->sendStop();
 			}
 			forceReset=true;
 		}
 		catch (BackendsErrors::BackendsErrorsEx& ex) {
 			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::disableDataTransfer()");
 			impl.setReason("backend failed to stop data transfer");
 			throw impl;
 		}
 		catch (ComponentErrors::ComponentErrorsEx& ex) {
 			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::disableDataTransfer()");
 			impl.setReason("backend failed to stop data transfer");
 			throw impl;
 		}
 		catch (CORBA::SystemException& ex) {
 			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::disableDataTransfer()");
 			impl.setName(ex._name());
 			impl.setMinor(ex.minor());
 			backendError=true;
 			throw impl;
 		}
 		catch (...) {
 			backendError=true;
 			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::disableDataTransfer())");
 			throw impl;
 		}
 	}
	if (scanStarted) {
		try {
			scanStarted=false;
			if (!CORBA::is_nil(writer)) {
				writer->stopScan();
			}
		}
 		catch (ComponentErrors::ComponentErrorsEx& ex) {
 			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::disableDataTransfer()");
 			impl.setReason("could not stop current scan");
 			throw impl;
 		}
 		catch (ManagementErrors::ManagementErrorsEx& ex) {
 			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::disableDataTransfer()");
 			impl.setReason("could not stop current scan");
 			throw impl;
 		}
 		catch (CORBA::SystemException& ex) {
 			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::disableDataTransfer()");
 			impl.setName(ex._name());
 			impl.setMinor(ex.minor());
 			writerError=true;
 			throw impl;
 		}
 		catch (...) {
 			writerError=true;
 			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::disableDataTransfer())");
 			throw impl;
 		}
	}
	if (forceReset) {
 		// the writer will reset,
		try {
 			if (!CORBA::is_nil(writer)) {
 				writer->reset();
 			}
 		}
 		catch (...) {
 			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::disableDataTransfer())");
 			writerError=true;
 			throw impl;
 		}
	}
 	if (streamPrepared) {
 		try {
 			streamPrepared=false;
 			if (!CORBA::is_nil(backend)) {
 				backend->terminate();
 			}
 		}
 		catch (BackendsErrors::BackendsErrorsEx& ex) {
 			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::disableDataTransfer()");
 			impl.setReason("backend failed to terminate data transfer");
 			throw impl;
 		}
 		catch (ComponentErrors::ComponentErrorsEx& ex) {
 			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::disableDataTransfer()");
 			impl.setReason("backend failed to terminate data transfer");
 			throw impl;
 		}
 		catch (CORBA::SystemException& ex) {
 			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::disableDataTransfer()");
 			impl.setName(ex._name());
 			impl.setMinor(ex.minor());
 			backendError=true;
 			throw impl;
 		}
 		catch (...) {
 			backendError=true;
 			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::disableDataTransfer()");
 			throw impl;
 		}
 	}
 	if (streamConnected) {
 		try {
 			streamConnected=false;
 			if (!CORBA::is_nil(backend)) {
 				backend->disconnect();
 			}
 		}
 		catch (ACSBulkDataError::AVDisconnectErrorEx& ex) {
 			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::disableDataTransfer()");
 			impl.setReason("backend failed to disconnect");
 			throw impl;
 		}
 		catch (CORBA::SystemException& ex) {
 			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::disableDataTransfer()");
 			impl.setName(ex._name());
 			impl.setMinor(ex.minor());
 			backendError=true;
 			throw impl;
 		}
 		catch (...) {
 			backendError=true;
 			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::disableDataTransfer()");
 			throw impl;
 		}
 		try {
 			if (!CORBA::is_nil(writer)) {
 				writer->closeReceiver();
 			}
 		}
 		catch (ACSBulkDataError::AVCloseReceiverErrorEx& ex) {
 			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::disableDataTransfer()");
 			impl.setReason("receiver failed to close");
 			throw impl;
 		}
 		catch (...) {
 			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::disableDataTransfer())");
 			writerError=true;
 			throw impl;
 		}
 	}
}

void CCore::setupDataTransfer(bool& scanStarted,
							  Management::DataReceiver_ptr writer,bool& writerError,
							  Backends::GenericBackend_ptr backend,bool& backendError,
							  const bool& streamPrepared,
							  const IRA::CString& obsName,
							  const IRA::CString& prj,
							  const IRA::CString& baseName,
							  const IRA::CString& path,
							  const IRA::CString& extraPath,
							  const IRA::CString& schedule,
							  const IRA::CString& targetID,
							  const IRA::CString& layoutName,
							  const ACS::stringSeq& layout,
							  const long& scanTag,
							  const long& device,
							  const DWORD& scanID,
							  const ACS::Time& startTime,
							  const DWORD& subScanID,
							  const Management::TScanAxis& axis,
							  const CConfiguration* config,
							  IRA::CString &fullSubscanFileName,
							  IRA::CString &fullScanFolder
	) throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::ComponentNotActiveExImpl,
			ComponentErrors::UnexpectedExImpl,ManagementErrors::DataTransferSetupErrorExImpl,ManagementErrors::BackendNotAvailableExImpl)
{
	fullScanFolder=fullSubscanFileName="";
	if (!streamPrepared) {
		_EXCPT(ManagementErrors::DataTransferSetupErrorExImpl,impl,"CCore::setupDataTansfer()");
		throw impl;
	}
	CORBA::String_var fullFileName;
	CORBA::String_var fullPath;
 	try {
 		if (!CORBA::is_nil(writer)) {
 			Management::TScanSetup setup;
 			if (!scanStarted) {
 				setup.scanTag=scanTag;
 				setup.scanId=scanID;
 				setup.projectName=CORBA::string_dup((const char *)prj);
 				setup.observerName=CORBA::string_dup((const char *)obsName);
 				setup.baseName=CORBA::string_dup((const char *)baseName);
 				setup.path=CORBA::string_dup((const char *)path);
 				setup.extraPath=CORBA::string_dup((const char *)extraPath);
 				setup.schedule=CORBA::string_dup((const char *)schedule);
 				if (layoutName!=_SCHED_NULLTARGET) {
 					setup.scanLayout=CORBA::string_dup((const char *)layoutName);
 				}
 				else {
 					setup.scanLayout=CORBA::string_dup((const char *)"");
 				}
 				setup.device=device;
 				fullPath=writer->startScan(setup);
 				fullScanFolder=(const char *)fullPath;
 				scanStarted=true;
 				if (layout.length()>0) {
 					writer->setScanLayout(layout);
 					ACS_STATIC_LOG(LM_FULL_INFO,"CCore::setupDataTransfer()",(LM_DEBUG,"SCAN_LAYOUT_DEFINITION_DONE"));
 				}
 				else {
 					ACS_STATIC_LOG(LM_FULL_INFO,"CCore::setupDataTransfer()",(LM_DEBUG,"SCAN_LAYOUT_DEFINITION_EMPTY"));
 				}
 			}
 		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::setupDataTransfer()");
			throw impl;
		}
 	}
 	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::setupDataTransfer()");
		impl.setReason("not able to pass extra scan information to data recorder");
		throw impl;
 	}
 	catch (ManagementErrors::ManagementErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::setupDataTransfer()");
		impl.setReason("not able to pass extra scan information to data recorder");
		throw impl;
 	}
 	catch (CORBA::SystemException& ex) {
 		writerError=true;
 		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::setupDataTransfer()");
 		impl.setName(ex._name());
 		impl.setMinor(ex.minor());
 		throw impl;
 	}
 	try {
 		if (!CORBA::is_nil(writer)) {
 			Management::TSubScanSetup subSetup;
 			subSetup.startUt=startTime;
 			subSetup.subScanId=subScanID;
 			subSetup.axis=axis;
 			subSetup.minorServoNameForAxis=CORBA::string_dup((const char *)config->getServoNameFromAxis(axis));
 			subSetup.extraPath=CORBA::string_dup((const char *)extraPath);
 			subSetup.baseName=CORBA::string_dup((const char *)baseName);
 			subSetup.targetID=CORBA::string_dup((const char *)targetID);
 			fullFileName=writer->startSubScan(subSetup);
 			fullSubscanFileName=(const char *)fullFileName;
 		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::setupDataTransfer()");
			throw impl;
		}
 	}
 	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::setupDataTransfer()");
		impl.setReason("not able to pass extra subscan information to data recorder");
		throw impl;
 	}
 	catch (ManagementErrors::ManagementErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::setupDataTransfer()");
		impl.setReason("not able to pass extra subscan information to data recorder");
		throw impl;
 	}
 	catch (CORBA::SystemException& ex) {
 		writerError=true;
 		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::setupDataTransfer()");
 		impl.setName(ex._name());
 		impl.setMinor(ex.minor());
 		throw impl;
 	}
 	// also inform the backend on the currently processed file
	if (!CORBA::is_nil(backend)) {
		try {
			backend->setTargetFileName(fullFileName.in());
		}
		catch (...) {
			_EXCPT(ComponentErrors::OperationErrorExImpl,impl,"CCore::setupDataTransfer()");
			impl.setReason("could not pass the file name to the backend");
			backendError=true;
			throw impl;
		}
	}
	else {
		_EXCPT(ManagementErrors::BackendNotAvailableExImpl,impl,"CCore::setupDataTransfer()");
		throw impl;
	}
}

void CCore::stopScan(Management::DataReceiver_ptr writer,bool& writerError,bool& scanStarted) throw (ComponentErrors::OperationErrorExImpl)
{
	if (!CORBA::is_nil(writer)) {
		try {
			if (scanStarted) {
				writer->stopScan();
			}
			scanStarted=false;
			ACS_STATIC_LOG(LM_FULL_INFO,"CCore::stopScan()",(LM_DEBUG,"SCAN_FINALIZED"));
		}
 		catch (...) {
 			_EXCPT(ComponentErrors::OperationErrorExImpl,impl,"CCore::stopScan()");
 			impl.setReason("could not stop current scan");
 			writerError=true;
 			throw impl;
 		}
	}
}

bool CCore::checkRecording(Management::DataReceiver_ptr writer,bool& writerError) throw (ComponentErrors::OperationErrorExImpl)
{
	if (!CORBA::is_nil(writer)) {
		try {
				return (bool)writer->isRecording();
		}
		catch (...) {
			_EXCPT(ComponentErrors::OperationErrorExImpl,impl,"CCore::checkRecording()");
			impl.setReason("could not check if recording is completed");
			writerError=true;
			throw impl;
		}
	}
	return true;
}

void CCore::stopDataTransfer(Backends::GenericBackend_ptr backend,bool& backendError,bool& streamStarted,bool& streamPrepared,bool& streamConnected) throw (ComponentErrors::OperationErrorExImpl,
		ManagementErrors::BackendNotAvailableExImpl)
{
	if (!CORBA::is_nil(backend)) {
		try {
			if (streamStarted) {
				backend->sendStop();
				streamStarted=false;
				ACS_STATIC_LOG(LM_FULL_INFO,"CCore::stopDataTransfer()",(LM_NOTICE,"RECORDING_STOPPED"));
			}
		}
		catch (...) {
			_EXCPT(ComponentErrors::OperationErrorExImpl,impl,"CCore::stopDataTransfer()");
			impl.setReason("backend could not stop data transfer");
			backendError=true;
			throw impl;
		}
	}
	else {
		_EXCPT(ManagementErrors::BackendNotAvailableExImpl,impl,"CCore::stopDataTransfer()");
		throw impl;
	}
}

void CCore::startDataTansfer(Backends::GenericBackend_ptr backend,bool& backendError,const ACS::Time& startTime,bool& streamStarted,bool& streamPrepared,bool& streamConnected) throw (
		ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl)
{
	if (!CORBA::is_nil(backend)) {
		try {
			if ((streamPrepared) && (!streamStarted)) {
				backend->sendData(startTime);
				streamStarted=true;
				if (startTime==0) {
					ACS_STATIC_LOG(LM_FULL_INFO,"CCore::startDataTansfer()",(LM_NOTICE,"RECORDING_STARTED"));
				}
				else {
					IRA::CString out;
					IRA::CIRATools::timeToStr(startTime,out);
					ACS_STATIC_LOG(LM_FULL_INFO,"CCore::startDataTansfer()",(LM_NOTICE,"RECORDING_STARTS_AT: %s",(const char *)out));
				}
			}
			else {
				_EXCPT(ManagementErrors::DataTransferSetupErrorExImpl,impl,"CCore::startDataTansfer()");
				throw impl;
			}
		}
		catch (BackendsErrors::BackendsErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::startDataTansfer()");
			impl.setReason("backend could not send data to writer");
			throw impl;
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::startDataTansfer()");
			impl.setReason("backend could not send data to writer");
			throw impl;
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::startDataTansfer()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			backendError=true;
			throw impl;
		}
		catch (...) {
			backendError=true;
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::startDataTansfer()");
			throw impl;
		}
	}
	else {
		_EXCPT(ManagementErrors::BackendNotAvailableExImpl,impl,"CCore::startDataTansfer()");
		throw impl;
	}
}
