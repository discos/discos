#include <ObservatoryS.h>
#include <slamac.h>
#include "Core.h"
#include "ScheduleExecutor.h"
#include <LogFilter.h>
#include <ACSBulkDataError.h>

using namespace SimpleParser;
using namespace maci;

_IRA_LOGFILTER_IMPORT;

#define SCHED_EXECUTOR_NAME "SCHED_THREAD"

CCore::CCore(ContainerServices *service,CConfiguration *conf) : 
	m_config(conf), m_services(service)
{
}

CCore::~CCore()
{
}

void CCore::initialize()
{
	RESOURCE_INIT;
	resetSchedulerStatus();
	m_currentProceduresFile="";
}

void CCore::execute() throw (ComponentErrors::TimerErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::MemoryAllocationExImpl,ManagementErrors::ProcedureFileLoadingErrorExImpl)
{
	Antenna::TSiteInformation_var site;
	Antenna::Observatory_var observatory=Antenna::Observatory::_nil();
	try {
		observatory=m_services->getDefaultComponent<Antenna::Observatory>((const char*)m_config->getObservatoryComponent());
	}
	catch (maciErrType::CannotGetComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::execute()");
		Impl.setComponentName((const char*)m_config->getObservatoryComponent());
		throw Impl;
	}
	catch (maciErrType::NoPermissionExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::execute()");
		Impl.setComponentName((const char*)m_config->getObservatoryComponent());
		throw Impl;		
	}
	catch (maciErrType::NoDefaultComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::execute()");
		Impl.setComponentName((const char*)m_config->getObservatoryComponent());
		throw Impl;				
	}
	ACS_LOG(LM_FULL_INFO,"CCore::execute()",(LM_INFO,"OBSERVATORY_LOCATED"));	
	try	{	
		site=observatory->getSiteSummary();  //throw CORBA::SYSTEMEXCEPTION
	}
	catch (CORBA::SystemException& ex)	{
		_EXCPT(ComponentErrors::CORBAProblemExImpl,__dummy,"CCore::updateSite()");
		__dummy.setName(ex._name());
		__dummy.setMinor(ex.minor());
		throw __dummy;
	}
	m_site=IRA::CSite(site.out());
	m_dut1=site->DUT1;
	ACS_LOG(LM_FULL_INFO,"CCore::execute()",(LM_INFO,"SITE_INITIALIZED"));
	try {
		m_services->releaseComponent((const char*)observatory->name());
	}
	catch  (maciErrType::CannotReleaseComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CCore::execute()");
		Impl.setComponentName((const char*)observatory->name());
		throw Impl;
	}
	RESOURCE_EXEC;
	// spawn schedule executor thread........
	try {
		CCore *tmp=this;
		m_schedExecuter=m_services->getThreadManager()->create<CScheduleExecutor,CCore *>(SCHED_EXECUTOR_NAME,tmp);
	}
	catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"CCore::execute()");
		throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CCore::execute()");
	}
	m_schedExecuter->initialize(m_services,m_dut1,m_site); // throw (ComponentErrors::TimerErrorExImpl)
	ACS::TimeInterval sleepTime=m_config->getScheduleExecutorSleepTime()*10;
	m_schedExecuter->setSleepTime(sleepTime);

	//add local commands
	m_parser->add<0>("tsys",new function1<CCore,non_constant,void_type,O<doubleSeq_type> >(this,&CCore::callTSys));
	m_parser->add<1>("wait",new function1<CCore,constant,void_type,I<double_type> >(this,&CCore::wait));
	m_parser->add<0>("nop",new function0<CCore,constant,void_type >(this,&CCore::nop));
	m_parser->add<0>("waitOnSource",new function0<CCore,constant,void_type >(this,&CCore::waitOnSource));
	m_parser->add<0>("status",new function0<CCore,constant,int_type >(this,&CCore::status));
	m_parser->add<0>("haltSchedule",new function0<CCore,non_constant,void_type >(this,&CCore::haltSchedule));
	m_parser->add<0>("stopSchedule",new function0<CCore,non_constant,void_type >(this,&CCore::stopSchedule));
	m_parser->add<2>("startSchedule",new function2<CCore,non_constant,void_type,I<string_type>,I<string_type> >(this,&CCore::startSchedule));
	m_parser->add<1>("device",new function1<CCore,non_constant,void_type,I<long_type> >(this,&CCore::setDevice));
	m_parser->add<1>("chooseBackend",new function1<CCore,non_constant,void_type,I<string_type> >(this,&CCore::chooseDefaultBackend));
	m_parser->add<1>("chooseRecorder",new function1<CCore,non_constant,void_type,I<string_type> >(this,&CCore::chooseDefaultDataRecorder));
	m_parser->add<3>("crossScan",new function3<CCore,non_constant,void_type,I<enum_type<AntennaFrame2String,Antenna::TCoordinateFrame > >,I<angleOffset_type<rad> >,
			I<interval_type> >(this,&CCore::crossScan));
	
	//add remote commands ************  should be loaded from a CDB table............................**********/
	// antenna subsystem
	m_parser->add("antennaDisable","antenna",1,&CCore::remoteCall);
	m_parser->add("antennaEnable","antenna",1,&CCore::remoteCall);
	m_parser->add("antennaPark","antenna",1,&CCore::remoteCall);
	m_parser->add("antennaAzEl","antenna",1,&CCore::remoteCall);
	m_parser->add("antennaTrack","antenna",1,&CCore::remoteCall);
	m_parser->add("antennaUnstow","antenna",1,&CCore::remoteCall);
	m_parser->add("antennaStop","antenna",1,&CCore::remoteCall);
	m_parser->add("antennaSetup","antenna",1,&CCore::remoteCall);	
	m_parser->add("preset","antenna",1,&CCore::remoteCall);
	m_parser->add("moon","antenna",1,&CCore::remoteCall);
	m_parser->add("lonOTF","antenna",1,&CCore::remoteCall);
	m_parser->add("latOTF","antenna",1,&CCore::remoteCall);
	m_parser->add("goOff","antenna",1,&CCore::remoteCall);
	m_parser->add("sidereal","antenna",1,&CCore::remoteCall);
	m_parser->add("vlsr","antenna",1,&CCore::remoteCall);
	m_parser->add("bwhm","antenna",1,&CCore::remoteCall);
	m_parser->add("track","antenna",1,&CCore::remoteCall);
	m_parser->add("azelOffsets","antenna",1,&CCore::remoteCall);
	m_parser->add("radecOffsets","antenna",1,&CCore::remoteCall);
	m_parser->add("lonlatOffsets","antenna",1,&CCore::remoteCall);	
	// receivers subsystem
	m_parser->add("receiversPark","receivers",2,&CCore::remoteCall);
	m_parser->add("receiversSetup","receivers",2,&CCore::remoteCall);
	m_parser->add("receiversMode","receivers",2,&CCore::remoteCall);
	m_parser->add("calOn","receivers",2,&CCore::remoteCall);
	m_parser->add("calOff","receivers",2,&CCore::remoteCall);
	m_parser->add("setLO","receivers",2,&CCore::remoteCall);
	// backend wrapper function
	m_parser->add("bck","backends",3,&CCore::remoteCall);
	loadProcedures(m_config->getDefaultProceduresFile()); // throws ManagementErrors::ProcedureFileLoadingErrorExImpl
}

void CCore::cleanUp()
{
	/******************************************************************/
	/** shouldn't be necessary to take the mutex ? */
	/******************************************************************/
	RESOURCE_CLEANUP;
	unloadAntennaBoss(m_antennaBoss);
	unloadReceiversBoss(m_receiversBoss);
	unloadDefaultBackend();
	unloadDefaultDataReceiver();
	if (m_schedExecuter!=NULL) m_schedExecuter->suspend();
	m_services->getThreadManager()->destroy(m_schedExecuter);
	ACS_LOG(LM_FULL_INFO,"CCore::cleanUp()",(LM_INFO,"THREAD_DESTROYED"));
}

void CCore::loadProcedures(const IRA::CString& proceduresFile) throw (ManagementErrors::ProcedureFileLoadingErrorExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	ACS::stringSeq names;
	ACS::stringSeq *commands=NULL;
	try {
		m_config->readProcedures(m_services,proceduresFile,names,commands);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		_ADD_BACKTRACE(ManagementErrors::ProcedureFileLoadingErrorExImpl,impl,ex,"CCore::loadProcedures()");
		impl.setFileName((const char *)proceduresFile);
		throw impl;
	}
	for (unsigned i=0;i<names.length();i++) {
		IRA::CString name(names[i]);
		m_parser->add(name,proceduresFile,commands[i]);
	}
	if (commands) delete[] commands;
	m_currentProceduresFile=proceduresFile;
	ACS_LOG(LM_FULL_INFO,"CCore::loadProcedures()",(LM_NOTICE,"PROCEDURES_FILE_LOADED: %s",(const char *)proceduresFile));
}

void CCore::chooseDefaultBackend(const char *bckInstance)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	IRA::CString instance(bckInstance);
	if (m_defaultBackendInstance!=instance) {
		m_defaultBackendInstance=instance;
		m_defaultBackendError=true;  // this is tricky...in order to force to unload the preset backend and then reload the new one the next time the default backend is required 
	}
}

void CCore::chooseDefaultDataRecorder(const char *rcvInstance)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	IRA::CString instance(rcvInstance);
	if (m_defaultDataReceiverInstance!=instance) {
		m_defaultDataReceiverInstance=instance;
		m_defaultDataReceiverError=true;  // this is tricky...in order to force to unload the preset data recorder and then reload the new one the next time the default data recroder is required 
	}
}

void CCore::callTSys(ACS::doubleSeq& tsys) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::OperationErrorExImpl,
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
	long inputs;
	IRA::CString outLog;
	IRA::CString backendName;
	
	backend=m_schedExecuter->getBackendReference(); //get the reference to the currently used backend.
	baci::ThreadSyncGuard guard(&m_mutex);
	if (CORBA::is_nil(backend)) {
		loadDefaultBackend(); // throw ComponentErrors::CouldntGetComponentExImpl& err)
		backend=m_defaultBackend;
	}	
	else {
		/*try {  	
			backend=m_services->getComponent<Backends::GenericBackend>((const char *)backendName);
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::callTSys()");
			Impl.setComponentName((const char*)backendName);
			throw Impl;		
		}*/
	}
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
		mark=m_receiversBoss->getCalibrationMark(freq,bandWidth,feed,IFs,skyFreq.out(),skyBw.out());
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
	/*guard.release();
	IRA::CIRATools::Wait(m_config->getTsysGapTime());
	guard.acquire();*/
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
	//release the backend...if necessary
	/*if (backendName!="") {
		try {
			m_services->releaseComponent((const char*)backendName);
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CCore::callTSys()");
			Impl.setComponentName((const char *)backendName);
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::callTSys())");
		}
	}*/
	//Now let's compose a message to log
	for (int i=0;i<inputs;i++) {
		outLog.Format("DEVICE/%d Feed: %d, IF: %d, Freq: %lf, Bw: %lf/",i,feed[i],IFs[i],freq[i],bandWidth[i]);
		ACS_LOG(LM_FULL_INFO,"CCore::callTSys()",(LM_NOTICE,(const char *)outLog));
		outLog.Format("CALTEMP/%d %lf",i,mark[i]);
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

void CCore::crossScan(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration) throw (ManagementErrors::NotAllowedDuringScheduleExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,
		ManagementErrors::TargetIsNotVisibleExImpl,ManagementErrors::TsysErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl,
		ManagementErrors::AntennaScanErrorExImpl)
{
	//no need to get the mutex, because it is already done inside the executor object 
	if (m_schedExecuter) {
		if (m_schedExecuter->isScheduleActive()) {
			_THROW_EXCPT(ManagementErrors::NotAllowedDuringScheduleExImpl,"CCore::crossScan()");
		}
	}
	// let's create the scans.....filling up the required fields :-)
	CORBA::Double bwhm=0.017453; // one degree in rad
	CORBA::Double offset;
	Management::TScanAxis scanAxis;
	IRA::CString obsName,prj,suffix,path,extraPath,baseName;
	IRA::CString layoutName,schedule;
	ACS::stringSeq layout;
	long scanTag=0;
	long scanID=1,subScanID;
	TIMEVALUE now;
	ACS::Time waitFor;
	long long waitMicro;
	ACS::Time startTime;
		
	obsName=IRA::CString("system");
	prj=obsName;
	layout.length(0);
	layoutName=_SCHED_NULLTARGET;
	schedule="crossScan";
	
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
	clearTracking();
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
		
	//make sure the communication was not already there
	try {
		CCore::disableDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_defaultDataReceiver.in(),m_defaultDataReceiverError,m_streamStarted,m_streamPrepared,m_streamConnected,m_scanStarted);
	}
	catch (...) {
		m_streamStarted=m_streamPrepared=m_streamConnected=m_scanStarted=false;
		// keep it up, no need to give up now......
	}
	//now load the Backend and the dataReceiver	
	loadDefaultBackend(); //throw (ComponentErrors::CouldntGetComponentExImpl);
	loadDefaultDataReceiver(); //throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl)
	// ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl
	CCore::enableDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_defaultDataReceiver.in(),m_streamConnected);
	//LATITUDE SCAN.........................
	// now lets go and check the lon scan....typically if a source was commanded before....and it is above the horizon and the scan could be performed
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
	if (scanFrame==Antenna::ANT_HORIZONTAL) {
		scanAxis=Management::MNG_HOR_LAT;
	}
	else if (scanFrame==Antenna::ANT_EQUATORIAL) {
		scanAxis=Management::MNG_EQ_LAT;
	}
	else {
		scanAxis=Management::MNG_GAL_LAT;
	}
	path=m_config->getSystemDataDirectory();
	suffix=IRA::CString("latitude");
	baseName=CCore::computeOutputFileName(startTime,prj,suffix,extraPath);
	subScanID=1;
	// throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl)
	CCore::setupDataTransfer(m_scanStarted,m_streamPrepared,m_defaultDataReceiver.in(),m_defaultDataReceiverError,m_defaultBackend.in(),m_defaultBackendError,
			obsName,prj,baseName,path,extraPath,schedule,layoutName,layout,scanTag,m_currentDevice,scanID,startTime,subScanID,scanAxis);
	// start the recording or data transfer
	// throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::UnexpectedExImpl,ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl)
	CCore::startDataTansfer(m_defaultBackend.in(),m_defaultBackendError,startTime,m_streamStarted,m_streamPrepared,m_streamConnected);
	// now set the the data transfer stop
	waitFor=startTime+duration; // this is the time at which the stop should be issued
	waitMicro=(duration/10)/10000; // one tens of the total duration 
	IRA::CIRATools::getTime(now);
	guard.release();	
	while (now.value().value<waitFor) {
		IRA::CIRATools::Wait(waitMicro);
		IRA::CIRATools::getTime(now);
	}
	guard.acquire();
	// throw (ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl)
	CCore::stopDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_streamStarted,m_streamPrepared,m_streamConnected);
	// wait for a data transfer to complete before start with the latitude scan
	guard.release();
	while (checkRecording(m_defaultDataReceiver.in(),m_defaultDataReceiverError)) {
		IRA::CIRATools::Wait(0,250000); // 0.25 seconds
	}
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
	if (scanFrame==Antenna::ANT_HORIZONTAL) {
		scanAxis=Management::MNG_HOR_LON;
	}
	else if (scanFrame==Antenna::ANT_EQUATORIAL) {
		scanAxis=Management::MNG_EQ_LON;
	}
	else {
		scanAxis=Management::MNG_GAL_LON;
	}
	suffix=IRA::CString("longitude");
	baseName=CCore::computeOutputFileName(startTime,prj,suffix,extraPath);
	subScanID=2;
	// throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl)
	CCore::setupDataTransfer(m_scanStarted,m_streamPrepared,m_defaultDataReceiver.in(),m_defaultDataReceiverError,m_defaultBackend.in(),m_defaultBackendError,
			obsName,prj,baseName,path,extraPath,schedule,layoutName,layout,scanTag,m_currentDevice,scanID,startTime,subScanID,scanAxis);
	// start the recording or data transfer
	// throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::UnexpectedExImpl,ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl)
	CCore::startDataTansfer(m_defaultBackend.in(),m_defaultBackendError,startTime,m_streamStarted,m_streamPrepared,m_streamConnected);
	// now set the the data transfer stop
	waitFor=startTime+duration; // this is the time at which the stop should be issued
	waitMicro=(duration/10)/10000; // one tens of the total duration 
	IRA::CIRATools::getTime(now);
	guard.release();	
	while (now.value().value<waitFor) {
		IRA::CIRATools::Wait(waitMicro);
		IRA::CIRATools::getTime(now);
	}
	guard.acquire();
	// throw (ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl)
	CCore::stopDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_streamStarted,m_streamPrepared,m_streamConnected);
	// throw (ComponentErrors::OperationErrorExImpl)
	CCore::stopScan(m_defaultDataReceiver.in(), m_defaultDataReceiverError,m_scanStarted);
	// wait for a couple of seconds before start with the latitude scan
	guard.release();
	while (checkRecording(m_defaultDataReceiver.in(),m_defaultDataReceiverError)) {
		IRA::CIRATools::Wait(0,250000); // 0.25 seconds
	}
	guard.acquire();
	CCore::disableDataTransfer(m_defaultBackend.in(),m_defaultBackendError,m_defaultDataReceiver.in(),m_defaultDataReceiverError,m_streamStarted,m_streamPrepared,m_streamConnected,m_scanStarted);
	ACS_LOG(LM_FULL_INFO,"CCore::crossScan()",(LM_NOTICE,"CROSSSCAN_DONE"));
}

void CCore::resetSchedulerStatus()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	m_schedulerStatus=Management::MNG_OK;
	ACS_LOG(LM_FULL_INFO,"CCore::resetSchedulerStatus()",(LM_NOTICE,"COMPONENT_STATUS_RESET"));
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
	backend=m_schedExecuter->getBackendReference(); //get the reference to the currently used backend.
	guard.acquire();
	if (CORBA::is_nil(backend)) {
		loadDefaultBackend(); // throw ComponentErrors::CouldntGetComponentExImpl& err)
		backend=m_defaultBackend;
	}
	else {
		/*try {  	
			backend=m_services->getComponent<Backends::GenericBackend>((const char *)backendName);
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::setDevice()");
			Impl.setComponentName((const char*)backendName);
			throw Impl;		
		}*/
	}	
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
	/*if (backendName!="") {
		try {
			m_services->releaseComponent((const char*)backendName);
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CCore::setDevice()");
			Impl.setComponentName((const char *)backendName);
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::callTSys())");
		}
	}*/
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

void CCore::startSchedule(const char* scheduleFile,const char * startSubScan) throw (
		ManagementErrors::ScheduleErrorExImpl, ManagementErrors::AlreadyRunningExImpl,
		ComponentErrors::MemoryAllocationExImpl,ManagementErrors::SubscanErrorExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	//no need to get the mutex, because it is already done inside the Schedule Executor thread
	if (m_schedExecuter) {
		//throw ManagementErrors::ScheduleErrorExImpl, ManagementErrors::AlreadyRunningExImpl,ComponentErrors::MemoryAllocationExImpl,ComponentErrors::CouldntGetComponentExImpl)
		m_schedExecuter->startSchedule(scheduleFile,startSubScan);
	}
}

IRA::CString CCore::command(const IRA::CString& line) throw (ManagementErrors::CommandLineErrorExImpl)
{
	IRA::CString out;
	try {
		m_parser->run(line,out); //parser is already thread safe.....
		return out;
	}
	catch (ParserErrors::ParserErrorsExImpl &ex) {
		_ADD_BACKTRACE(ManagementErrors::CommandLineErrorExImpl,impl,ex,"CCore::command()");
		impl.setCommand((const char *)line);
		impl.setErrorMessage((const char *)out);
		throw impl;
	}	
}

const DWORD& CCore::getScanCounter()
{
	return m_schedExecuter->getCurrentScheduleCounter();
}

void CCore::getCurrentIdentifiers(DWORD& scanID,DWORD& subScanID)
{
	m_schedExecuter->getCurrentScanIdentifers(scanID,subScanID);
}
	
const IRA::CString& CCore::getScheduleName()
{
	return m_schedExecuter->getScheduleName();
}

/////// PRIVATES

void CCore::changeSchedulerStatus(const Management::TSystemStatus& status)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (status>=m_schedulerStatus) m_schedulerStatus=status; 
}

#include "Core_Common.i"

#include "Core_Resource.i"

#include "Core_Extra.i"
