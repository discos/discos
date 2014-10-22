
/*bool CCore::checkScan(const CSchedule::TScheduleMode& mode,const CSchedule::TRecord& scanInfo,const Schedule::CScanList::TRecord& scanData,const double& minEl,const double& maxEl,Antenna::AntennaBoss_ptr antBoss,bool& antBossError)
	throw (ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl)
{
	//No sync required, this is a private static method, called only by public method of this class and by the schedule executor. 
	ACS::TimeInterval slewTime;
	Antenna::TAzimuthSection azSection;
	bool answer;
	// this function should check if the telescope could perform the required motion according the schedule type and the scan type
	// at the moment the only motion involved is the antenna motion, in future it could be the subreflector or other subsystems.
	// the better way is to check the scanRec.type which should be changed from Antenna::TGeneratorType to something more generic
	// "scanData.type" which is Management::TScanTypes
	if (mode==CSchedule::LST) {    // all the motion should be checked against the ut time
		Antenna::TTrackingParameters *prim=static_cast<Antenna::TTrackingParameters *>(scanData.primaryParameters);
		Antenna::TTrackingParameters *sec=static_cast<Antenna::TTrackingParameters *>(scanData.secondaryParameters);
		//Antenna::TTrackingParameters * param=(Antenna::TTrackingParameters *)scanRec.scanPar;  //get the definitions of the 		
		try {
			if (!CORBA::is_nil(antBoss)) {
				answer=antBoss->checkScan(scanInfo.ut,*prim,*sec,minEl,maxEl,slewTime,azSection);
				ACS_STATIC_LOG(LM_FULL_INFO,"CCore::checkScan()",(LM_DEBUG,"SLEWING_TIME %lld :",slewTime));
				return answer;
			}
			else {
				_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::checkScan()");
				throw impl;
			}
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::checkScan()");
			impl.setReason("Could not inquiry the antenna boss");
			throw impl;
		}
		catch (AntennaErrors::AntennaErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::checkScan()");
			impl.setReason("Could not inquiry the antenna boss");
			throw impl;
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::checkScan()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			antBossError=true;
			throw impl;
		}
		catch (...) {
			antBossError=true;
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::checkScan()");
			throw impl;
		}
	}
	else if ((mode==CSchedule::SEQ) || (mode==CSchedule::TIMETAGGED)) {    // all the motion should be checked in general, if the source if above a given elevation....		
		Antenna::TTrackingParameters *prim=static_cast<Antenna::TTrackingParameters *>(scanData.primaryParameters);
		Antenna::TTrackingParameters *sec=static_cast<Antenna::TTrackingParameters *>(scanData.secondaryParameters);
		//Antenna::TTrackingParameters * param=(Antenna::TTrackingParameters *)scanRec.scanPar;  //get the definitions of the 		
		try {
			if (!CORBA::is_nil(antBoss)) {
				ACS::Time timeTemp=0;
				answer=antBoss->checkScan(timeTemp,*prim,*sec,minEl,maxEl,slewTime,azSection);   // for SEQ schedule the position is not check against the time (ut=0)
				ACS_STATIC_LOG(LM_FULL_INFO,"CCore::checkScan()",(LM_DEBUG,"SLEWING_TIME %lld :",slewTime));
				return answer;
			}
			else {
				_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::checkScan()");
				throw impl;
			}			
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::checkScan()");
			impl.setReason("Could not inquiry the antenna boss");
			throw impl;
		}
		catch (AntennaErrors::AntennaErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::checkScan()");
			impl.setReason("Could not inquiry the antenna boss");
			throw impl;
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::checkScan()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			antBossError=true;
			throw impl;
		}
		catch (...) {
			antBossError=true;
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::checkScan()");
			throw impl;
		}
	}
	else {
		// ******************* UT, we'll see if it makes sense
		return true;
	}	
}*/

/*void CCore::doScan(CSchedule::TRecord& scanInfo,const Schedule::CScanList::TRecord& scanData,Antenna::AntennaBoss_ptr antBoss,bool& antBossError) throw (ComponentErrors::OperationErrorExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::ComponentNotActiveExImpl)
{
	//No sync required, this is a private static method, called only by public method of this class and by the schedule executor.
	//This performs the scan from the Antenna point of view....when more apparatus will be involved we should think about it
	// here we'll have to put the code to command a different scans according the involved subsystem, for example subreflector instead of the antenna.
	// the better way is to check the m_currentScanRec.type which should be changed from Antenna::TGeneratorType to something more generic
	try {
		if (!CORBA::is_nil(antBoss)) {
			Antenna::TTrackingParameters *prim=static_cast<Antenna::TTrackingParameters *>(scanData.primaryParameters);
			Antenna::TTrackingParameters *sec=static_cast<Antenna::TTrackingParameters *>(scanData.secondaryParameters);
			antBoss->startScan(scanInfo.ut,*prim,*sec);
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
		antBossError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::doScan()");
		antBossError=true;
		throw impl;
	}
}*/

/*Management::TScanAxis CCore::computeScanAxis(const Management::TScanTypes& type,const Schedule::CScanList::TRecord& scanRec)
{
	Management::TScanAxis scanAxis;
	// compute the axis or direction along which the scan is performed
	scanAxis=Management::MNG_NO_AXIS;
	if ((type==Management::MNG_OTF) || (type==Management::MNG_OTFC)) {
		Antenna::TTrackingParameters *pars=(Antenna::TTrackingParameters *)scanRec.primaryParameters;
		if (pars->otf.geometry==Antenna::SUBSCAN_CONSTLON) {
			if (pars->otf.subScanFrame==Antenna::ANT_EQUATORIAL) {
				scanAxis=Management::MNG_EQ_LAT;
			}
			else if (pars->otf.subScanFrame==Antenna::ANT_HORIZONTAL) {
				scanAxis=Management::MNG_HOR_LAT;
			}
			else if (pars->otf.subScanFrame==Antenna::ANT_GALACTIC) {
				scanAxis=Management::MNG_GAL_LAT;
			}
		}
		else if (pars->otf.geometry==Antenna::SUBSCAN_CONSTLAT) {
			if (pars->otf.subScanFrame==Antenna::ANT_EQUATORIAL) {
				scanAxis=Management::MNG_EQ_LON;
			}
			else if (pars->otf.subScanFrame==Antenna::ANT_HORIZONTAL) {
				scanAxis=Management::MNG_HOR_LON;
			}
			else if (pars->otf.subScanFrame==Antenna::ANT_GALACTIC) {
				scanAxis=Management::MNG_GAL_LON;
			}
		}
	}
	return scanAxis;
	// *******************************
	// I should consider all remaining scan types, not all scan types have a well defined scan axis....simple sidereal tracking for example!
	// *******************************
}*/

/*Management::TScanAxis CCore::computeScanAxis(Antenna::AntennaBoss_ptr antBoss,bool& antBossError,MinorServo::MinorServoBoss_ptr minorServoBoss,bool& minorServoError,
		const CConfiguration& config) throw (
		ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
{
	Management::TScanAxis scanAxis_ant,scanAxis_ms;
	CORBA::String_var servo;
	scanAxis_ant=scanAxis_ms=Management::MNG_NO_AXIS;
	try {
		if (!CORBA::is_nil(antBoss)) {
			antBoss->getScanAxis(scanAxis_ant);
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
		antBossError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::computeScanAxis()");
		antBossError=true;
		throw impl;
	}
	try {
		if (!CORBA::is_nil(minorServoBoss)) {
			servo=minorServoBoss->getScanAxis();
			scanAxis_ms=config.getAxisFromServoName(servo.in());
		}
	}
	catch (ManagementErrors::SubscanErrorEx& ex) {

	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::computeScanAxis()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		minorServoError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::computeScanAxis()");
		minorServoError=true;
		throw impl;
	}
	// this choice has to be reconsidered, does the minor servo movement always prevail?
	if (scanAxis_ms!=Management::MNG_NO_AXIS) { // if a minor servo has been selected, the resulting scan axis is the axis mapped to the servo
		return scanAxis_ms;
	}
	else {
		return scanAxis_ant;
	}
}*/

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
	if (streamStarted) {
 		try {
 			streamStarted=false;
 			if (!CORBA::is_nil(backend)) {
 				backend->sendStop();
 			}
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
																		 /*bool& streamPrepared,*/
																		Management::DataReceiver_ptr writer,bool& writerError,
																		Backends::GenericBackend_ptr backend,bool& backendError,
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
																		const  DWORD& subScanID,
																		const Management::TScanAxis& axis,
																		const CConfiguration* config
	) throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::UnexpectedExImpl)
{
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
 				writer->startScan(setup);
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
	/*if (!CORBA::is_nil(backend)) {
		try {
			if (!streamPrepared) {
				backend->sendHeader();
				streamPrepared=true;
			}
		}
		catch (BackendsErrors::BackendsErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::setupDataTransfer()");
			impl.setReason("backend failed to send header to writer");
			throw impl;
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::setupDataTransfer()");
			impl.setReason("backend failed to send header to writer");
			throw impl;
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::setupDataTransfer()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			backendError=true;
			throw impl;
		}
		catch (...) {
			backendError=true;
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::setupDataTransfer()");
			throw impl;
		}
	}*/
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
 			writer->startSubScan(subSetup);
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

