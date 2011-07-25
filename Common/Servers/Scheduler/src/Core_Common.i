// $Id: Core_Common.i,v 1.4 2011-06-21 16:39:52 a.orlati Exp $

bool CCore::checkScan(const CSchedule::TScheduleMode& mode,const CSchedule::TRecord& scanInfo,const Schedule::CScanList::TRecord& scanData,Antenna::AntennaBoss_ptr antBoss,bool& antBossError) 
	throw (ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl)
{
	//No sync required, this is a private static method, called only by public method of this class and by the schedule executor. 
	ACS::TimeInterval slewTime;
	bool answer;
	//****************************************************************************************************************
	// this function should check if the telescope could perform the required motion according the schedule type and the scan type
	// at the moment the only motion involved is the antenna motion, in future it could be the subreflector or other subsystems.
	// the better way is to check the scanRec.type which should be changed from Antenna::TGeneratorType to something more generic
	// "scanData.type" which is Management::TScanTypes
	//*****************************************************************************************************************
	if (mode==CSchedule::LST) {    // all the motion should be checked against the ut time
		Antenna::TTrackingParameters *prim=static_cast<Antenna::TTrackingParameters *>(scanData.primaryParameters);
		Antenna::TTrackingParameters *sec=static_cast<Antenna::TTrackingParameters *>(scanData.secondaryParameters);
		//Antenna::TTrackingParameters * param=(Antenna::TTrackingParameters *)scanRec.scanPar;  //get the definitions of the 		
		try {
			if (!CORBA::is_nil(antBoss)) {
				answer=antBoss->checkScan(scanInfo.ut,*prim,*sec,slewTime);
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
			impl.setReason("Could not enquery the antenna boss");
			throw impl;
		}
		catch (AntennaErrors::AntennaErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::checkScan()");
			impl.setReason("Could not enquery the antenna boss");
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
				answer=antBoss->checkScan(timeTemp,*prim,*sec,slewTime);   // for SEQ schedule the position is not check against the time (ut=0)
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
			impl.setReason("Could not enquery the antenna boss");
			throw impl;
		}
		catch (AntennaErrors::AntennaErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::checkScan()");
			impl.setReason("Could not enquery the antenna boss");
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
}

void CCore::doScan(CSchedule::TRecord& scanInfo,const Schedule::CScanList::TRecord& scanData,Antenna::AntennaBoss_ptr antBoss,bool& antBossError) throw (ComponentErrors::OperationErrorExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::ComponentNotActiveExImpl)
{
	//No sync required, this is a private static method, called only by public method of this class and by the schedule executor.
	/********************************************/
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
}

IRA::CString CCore::computeOutputFileName(const ACS::Time& ut,const ACS::TimeInterval& lst,const IRA::CString& prj,const IRA::CString& suffix)
{
	IRA::CString out;
	TIMEVALUE UT(ut);
	TIMEDIFFERENCE LST(lst);
	out.Format("%04d%02d%02d-%02d%02d%02d-%02d%02d%02d-%s_%s.fits",
			UT.year(),UT.month(),UT.day(),UT.hour(),UT.minute(),UT.second(),
			LST.hour(),LST.minute(),LST.second(),(const char *)prj,(const char *)suffix);
	return out;
}

IRA::CString CCore::computeOutputFileName(const ACS::Time& ut,const IRA::CString& prj,const IRA::CString& suffix)
{
	IRA::CString out;
	TIMEVALUE UT(ut);
	out.Format("%04d%02d%02d-%02d%02d%02d-%s_%s.fits",
			UT.year(),UT.month(),UT.day(),UT.hour(),UT.minute(),UT.second(),(const char *)prj,(const char *)suffix);		
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

void CCore::enableDataTransfer(Backends::GenericBackend_ptr backend,bool& backendError,Management::DataReceiver_ptr writer,bool& writerError,bool& streamPrepared,bool& streamConnected) throw (
		ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
{
	if ((!CORBA::is_nil(backend))  &&  (!CORBA::is_nil(writer))) {
		try {
 			backend->connect(writer);
 			streamConnected=true;
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
 		try {
 			backend->sendHeader();
 			streamPrepared=true;
 		}
 		catch (BackendsErrors::BackendsErrorsEx& ex) {
 			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::enableDataTransfer()");
 			impl.setReason("backend failed to send header to writer");
 			throw impl;
 		}
 		catch (ComponentErrors::ComponentErrorsEx& ex) {
 			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::enableFileWriting()");
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
		bool& streamConnected) throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
{
	if (streamStarted) {
 		try {
 			if (!CORBA::is_nil(backend)) {
 				backend->sendStop();
 			}	
 			streamStarted=false;
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
 			if (!CORBA::is_nil(backend)) {
 				backend->terminate();
 			}	
 			streamPrepared=false;
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
 		streamConnected=false;	
 	}	
}

void CCore::setupDataTransfer(Management::DataReceiver_ptr writer,bool& writerError,const IRA::CString& obsName,const IRA::CString& prj,const long& scanId,const long& device,
		const Management::TScanAxis& axis,const IRA::CString& fileName) throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::ComponentNotActiveExImpl)
{
 	try {
 		if (!CORBA::is_nil(writer)) {
 			writer->setObserverName((const char *)obsName);
 			writer->setProjectName((const char *)prj);
 			writer->setScanIdentifier(scanId); //scan id is incremented at each step!!!!
 			writer->setDevice(device);
 			writer->setScanAxis(axis);
 			writer->setFileName((const char *)fileName);
 		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::setupDataTransfer()");
			throw impl;
		}
 	}
 	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCore::setupDataTransfer()");
		impl.setReason("cannot set auxiliary data on data recorder");
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
				ACS_STATIC_LOG(LM_FULL_INFO,"CCore::startDataTansfer()",(LM_NOTICE,"RECORDING_STARTED"));
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

void CCore::antennaNCHandler(Antenna::AntennaDataBlock antennaData,void *handlerParam)
{
	CCore *me=static_cast<CCore *>(handlerParam);
	if (antennaData.status!=Management::MNG_OK) {
		me->m_isTracking=false;
	}
	else {
		me->m_isTracking=antennaData.tracking;
	}
}

IRA::CString CCore::remoteCall(const IRA::CString& command,const IRA::CString& package,const long& par) throw (ParserErrors::ExecutionErrorExImpl,ParserErrors::PackageErrorExImpl)
{
	char * ret_val;
	IRA::CString out;
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
				ret_val=m_antennaBoss->command((const char *)command);
				out=IRA::CString(ret_val);
				CORBA::string_free(ret_val);
				return out;
			}
			catch (ManagementErrors::CommandLineErrorEx& err) {				
				_ADD_BACKTRACE(ParserErrors::ExecutionErrorExImpl,impl,err,"CCore::remoteCall()");
				impl.setCommand((const char *)command);
				throw impl;
			}
			catch (CORBA::SystemException& err) {
				_EXCPT(ParserErrors::ExecutionErrorExImpl,impl,"CCore::command()");
				impl.setCommand((const char *)command);
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
				ret_val=m_receiversBoss->command((const char *)command);
				out=IRA::CString(ret_val);
				CORBA::string_free(ret_val);
				return out;				
			}
			catch (ManagementErrors::CommandLineErrorEx& err) {				
				_ADD_BACKTRACE(ParserErrors::ExecutionErrorExImpl,impl,err,"CCore::remoteCall()");
				impl.setCommand((const char *)command);
				throw impl;
			}
			catch (CORBA::SystemException& err) {
				_EXCPT(ParserErrors::ExecutionErrorExImpl,impl,"CCore::command()");
				impl.setCommand((const char *)command);
				m_receiversBossError=true;
				throw impl;
			}
			break;	
		}
		case 3: { // default backend...bck wrapper 
			IRA::CString unwrappedCommand;
			int pos;
			pos=command.Find(m_parser->getCommandDelimiter());
			if ((pos<0) || (pos==command.GetLength()-1)) {
				_EXCPT(ParserErrors::ExecutionErrorExImpl,impl,"CCore::command()");
				impl.setCommand((const char *)command);				
				throw impl;
			}
			else {
				pos++;
				unwrappedCommand=command.Mid(pos,command.GetLength()-pos);
			}			
			try {
				baci::ThreadSyncGuard guard(&m_mutex);  //loadReceiversBoss works on class data so it is safe to sync
				loadDefaultBackend(); 				
			}
			catch (ComponentErrors::CouldntGetComponentExImpl& err) { //this exception can be thrown by the loadReceiversBoss()
				_ADD_BACKTRACE(ParserErrors::PackageErrorExImpl,impl,err,"CCore::remoteCall()");
				impl.setPackageName((const char *)package);
				throw impl;
			}
			try {
				ret_val=m_defaultBackend->command((const char *)unwrappedCommand);
				out=IRA::CString(ret_val);
				CORBA::string_free(ret_val);
				return out;				
			}
			catch (ManagementErrors::CommandLineErrorEx& err) {				
				_ADD_BACKTRACE(ParserErrors::ExecutionErrorExImpl,impl,err,"CCore::remoteCall()");
				impl.setCommand((const char *)command);
				throw impl;
			}
			catch (CORBA::SystemException& err) {
				_EXCPT(ParserErrors::ExecutionErrorExImpl,impl,"CCore::command()");
				impl.setCommand((const char *)command);	
				m_defaultBackendError=true;
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

