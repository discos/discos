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
	clearStatus();
	m_currentProceduresFile="";
	m_lastWeatherTime=0;
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
	m_schedExecuter->initialize(m_services,m_dut1,m_site,m_config); // throw (ComponentErrors::TimerErrorExImpl)
	ACS::TimeInterval sleepTime=m_config->getScheduleExecutorSleepTime()*10;
	m_schedExecuter->setSleepTime(sleepTime);

	//add local commands
	m_parser->add("tsys",new function1<CCore,non_constant,void_type,O<doubleSeq_type> >(this,&CCore::callTSys),0);
	m_parser->add("wait",new function1<CCore,constant,void_type,I<double_type> >(this,&CCore::wait),1);
	m_parser->add("nop",new function0<CCore,constant,void_type >(this,&CCore::nop),0);
	m_parser->add("waitOnSource",new function0<CCore,constant,void_type >(this,&CCore::waitOnSource),0);
	m_parser->add("haltSchedule",new function0<CCore,non_constant,void_type >(this,&CCore::haltSchedule),0);
	m_parser->add("stopSchedule",new function0<CCore,non_constant,void_type >(this,&CCore::stopSchedule),0);
	m_parser->add("startSchedule",new function2<CCore,non_constant,void_type,I<string_type>,I<string_type> >(this,&CCore::startSchedule),2);
	m_parser->add("device",new function1<CCore,non_constant,void_type,I<long_type> >(this,&CCore::setDevice),1);
	m_parser->add("chooseBackend",new function1<CCore,non_constant,void_type,I<string_type> >(this,&CCore::chooseDefaultBackend),1);
	m_parser->add("chooseRecorder",new function1<CCore,non_constant,void_type,I<string_type> >(this,&CCore::chooseDefaultDataRecorder),1);
	m_parser->add("crossScan",new function3<CCore,non_constant,void_type,I<enum_type<AntennaFrame2String,Antenna::TCoordinateFrame > >,I<angleOffset_type<rad> >,
			I<interval_type> >(this,&CCore::crossScan),3);
	m_parser->add("focusScan",new function2<CCore,non_constant,void_type,I<double_type >,I<interval_type> >(this,&CCore::focusScan),2);
	m_parser->add("log",new function1<CCore,non_constant,void_type,I<string_type> >(this,&CCore::changeLogFile),1);
	m_parser->add("wx",new function4<CCore,non_constant,void_type,O<double_type>,O<double_type>,O<double_type>,O<double_type> >(this,&CCore::getWeatherStationParameters),0);
	m_parser->add("project",new function1<CCore,non_constant,void_type,I<string_type> >(this,&CCore::setProjectCode),1);
	// no range checks because * is allowed
	m_parser->add("skydip",new function3<CCore,non_constant,void_type,I<elevation_type<rad,false> >,I<elevation_type<rad,false> >,I<interval_type> >(this,&CCore::skydip),3);
	m_parser->add("agc","_tp_agc",2);
	m_parser->add("restFrequency",new function1<CCore,non_constant,void_type,I<doubleSeq_type> >(this,&CCore::setRestFrequency),1);


	//add remote commands ************  should be loaded from a CDB table............................**********/
	// antenna subsystem
	m_parser->add("antennaDisable","antenna",1,&CCore::remoteCall);
	m_parser->add("antennaEnable","antenna",1,&CCore::remoteCall);
	m_parser->add("antennaCorrectionOn","antenna",1,&CCore::remoteCall);
	m_parser->add("antennaCorrectionOff","antenna",1,&CCore::remoteCall);
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
	m_parser->add("goTo","antenna",1,&CCore::remoteCall);
	m_parser->add("skydipOTF","antenna",1,&CCore::remoteCall);
	m_parser->add("sidereal","antenna",1,&CCore::remoteCall);
	m_parser->add("bwhm","antenna",1,&CCore::remoteCall);
	m_parser->add("track","antenna",1,&CCore::remoteCall);
	m_parser->add("azelOffsets","antenna",1,&CCore::remoteCall);
	m_parser->add("radecOffsets","antenna",1,&CCore::remoteCall);
	m_parser->add("lonlatOffsets","antenna",1,&CCore::remoteCall);
	m_parser->add("antennaReset","antenna",1,&CCore::remoteCall);
	m_parser->add("radialVelocity","antenna",1,&CCore::remoteCall);

	// receivers subsystem
	m_parser->add("receiversPark","receivers",2,&CCore::remoteCall);
	m_parser->add("receiversSetup","receivers",2,&CCore::remoteCall);
	m_parser->add("receiversMode","receivers",2,&CCore::remoteCall);
	m_parser->add("calOn","receivers",2,&CCore::remoteCall);
	m_parser->add("calOff","receivers",2,&CCore::remoteCall);
	m_parser->add("setLO","receivers",2,&CCore::remoteCall);
	m_parser->add("antennaUnitOn","receivers",2,&CCore::remoteCall);
	m_parser->add("antennaUnitOff","receivers",2,&CCore::remoteCall);
	m_parser->add("derotatorPark","receivers",2,&CCore::remoteCall);
	m_parser->add("derotatorMode","receivers",2,&CCore::remoteCall);
	m_parser->add("derotatorPosition","receivers",2,&CCore::remoteCall);

	// backend
	//m_parser->add("bck","backends",3,&CCore::remoteCall);
	m_parser->add("integration","backend",3,&CCore::remoteCall);
	m_parser->add("setSection","backend",3,&CCore::remoteCall);
	m_parser->add("setAttenuation","backend",3,&CCore::remoteCall);
	m_parser->add("getTpi","backend",3,&CCore::remoteCall);
	m_parser->add("getZero","backend",3,&CCore::remoteCall);
	m_parser->add("initialize","backend",3,&CCore::remoteCall);

	// minor servo
	m_parser->add("servoSetup","minorservo",4,&CCore::remoteCall);
	m_parser->add("servoPark","minorservo",4,&CCore::remoteCall);
	m_parser->add("setServoElevationTracking","minorservo",4,&CCore::remoteCall);
	m_parser->add("setServoASConfiguration","minorservo",4,&CCore::remoteCall);
	m_parser->add("clearServoOffsets","minorservo",4,&CCore::remoteCall);
	m_parser->add("setServoOffset","minorservo",4,&CCore::remoteCall);

	// active surface
	m_parser->add("asSetup","activesurface",5,&CCore::remoteCall);
	m_parser->add("asPark","activesurface",5,&CCore::remoteCall);
	m_parser->add("asOn","activesurface",5,&CCore::remoteCall);
	m_parser->add("asOff","activesurface",5,&CCore::remoteCall);

	// procedures
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
	unloadMinorServoBoss(m_minorServoBoss);
	unloadCustomLogger(m_customLogger);
	unloadWeatherStation(m_weatherStation);
	unloadDefaultBackend();
	unloadDefaultDataReceiver();
	if (m_schedExecuter!=NULL) m_schedExecuter->suspend();
	m_services->getThreadManager()->destroy(m_schedExecuter);
	ACS_LOG(LM_FULL_INFO,"CCore::cleanUp()",(LM_INFO,"THREAD_DESTROYED"));
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
	double scaleFactor;
	long inputs;
	IRA::CString outLog;
	IRA::CString backendName;
	
	//backend=m_schedExecuter->getBackendReference(); //get the reference to the currently used backend.
	baci::ThreadSyncGuard guard(&m_mutex);
	//if (CORBA::is_nil(backend)) {
	loadDefaultBackend(); // throw ComponentErrors::CouldntGetComponentExImpl& err)
	backend=m_defaultBackend;
	/*}
	else {
	}*/
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

void CCore::skydip(const double& el1,const double& el2,const ACS::TimeInterval& duration) throw (ManagementErrors::NotAllowedDuringScheduleExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ManagementErrors::TsysErrorExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl, ComponentErrors::CORBAProblemExImpl,
		ManagementErrors::AntennaScanErrorExImpl,ComponentErrors::ComponentNotActiveExImpl,ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl,
		ComponentErrors::TimerErrorExImpl)
{
	//no need to get the mutex, because it is already done inside the executor object
	if (m_schedExecuter) {
		if (m_schedExecuter->isScheduleActive()) {
			_THROW_EXCPT(ManagementErrors::NotAllowedDuringScheduleExImpl,"CCore::skydip()");
		}
	}
	IRA::CString obsName,prj,suffix,path,extraPath,baseName;
	IRA::CString layoutName,schedule/*,targetID*/;
	ACS::stringSeq layout;
	TIMEVALUE now;
	ACS::Time waitFor;
	ACS::Time startTime;
	// now take the mutex
	baci::ThreadSyncGuard guard(&m_mutex);
	//make sure the antenna is available.
	loadAntennaBoss(m_antennaBoss,m_antennaBossError); // throw ComponentErrors::CouldntGetComponentExImpl

	// TSYS SCAN
	ACS_LOG(LM_FULL_INFO,"CCore::skydip()",(LM_NOTICE,"TSYS_COMPUTATION"));
	try {
		if (!CORBA::is_nil(m_antennaBoss)) {
			m_antennaBoss->goOff(Antenna::ANT_HORIZONTAL,-1.0*DD2R); // go off 1 degree....
		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::skydip()");
			throw impl;
		}
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::AntennaScanErrorExImpl,impl,ex,"CCore::skydip()");
		throw impl;
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::AntennaScanErrorExImpl,impl,ex,"CCore::skydip()");
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::skydip()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_antennaBossError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::skydip()");
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
		_ADD_BACKTRACE(ManagementErrors::TsysErrorExImpl,impl,ex,"CCore::skydip()");
		throw impl;
	}
	IRA::CIRATools::Wait(2,0);
	guard.acquire();

	// throw  (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,
	// ComponentErrors::CORBAProblemExImpl)
	initRecording(1);

	ACS_LOG(LM_FULL_INFO,"CCore::skydip()",(LM_NOTICE,"SKYDIP_SCAN"));
	try {
		if (!CORBA::is_nil(m_antennaBoss)) {
			startTime=m_antennaBoss->skydipScan(el1,el2,duration);
		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::skydip()");
			throw impl;
		}
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::AntennaScanErrorExImpl,impl,ex,"CCore::skydip()");
		throw impl;
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::AntennaScanErrorExImpl,impl,ex,"CCore::skydip()");
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::skydip()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_antennaBossError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::skydip()");
		m_antennaBossError=true;
		throw impl;
	}
	// throw  (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::ComponentNotActiveExImpl,
	//ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,
	//ManagementErrors::DataTransferSetupErrorExImpl)
	startRecording(startTime,1);
	// now set the the data transfer stop
	waitFor=startTime+duration; // this is the time at which the stop should be issued
	guard.release();
	waitUntil(waitFor); // throw ComponentErrors::TimerErrorExImpl
	//throw (ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl, ComponentErrors::OperationErrorExImpl)
	stopRecording();
	guard.acquire();
	//throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
	terminateScan();
	ACS_LOG(LM_FULL_INFO,"CCore::skydip()",(LM_NOTICE,"SKYDIP_DONE"));
}

void CCore::focusScan(const double& span,const ACS::TimeInterval& duration) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::ComponentNotActiveExImpl,
		ManagementErrors::AntennaScanErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ManagementErrors::TsysErrorExImpl,ComponentErrors::OperationErrorExImpl,
		ManagementErrors::UnsupportedOperationExImpl,ManagementErrors::MinorServoScanErrorExImpl,ComponentErrors::TimerErrorExImpl)
{
	//no need to get the mutex, because it is already done inside the executor object
	if (m_schedExecuter) {
		if (m_schedExecuter->isScheduleActive()) {
			_THROW_EXCPT(ManagementErrors::NotAllowedDuringScheduleExImpl,"CCore::focusScan()");
		}
	}
	// let's create the scans.....filling up the required fields :-)
	CORBA::Double bwhm=0.017453; // one degree in radians
	CORBA::Double offset;
	//Management::TScanAxis scanAxis;
	IRA::CString obsName,prj,suffix,path,extraPath,baseName;
	IRA::CString layoutName,schedule/*,targetID*/;
	ACS::stringSeq layout;
	//long scanTag=0;
	//long scanID=1,subScanID;
	TIMEVALUE now;
	ACS::Time waitFor;
	//long long waitMicro;
	ACS::Time startTime;

	// check if the operation is available
	if (!(MINOR_SERVO_AVAILABLE)) {
		_EXCPT(ManagementErrors::UnsupportedOperationExImpl,impl,"CCore::focusScan()");
		throw impl;
	}
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
	ACS_LOG(LM_FULL_INFO,"CCore::focusScan()",(LM_NOTICE,"TSYS_COMPUTATION"));
	try {
		if (!CORBA::is_nil(m_antennaBoss)) {
			m_antennaBoss->goOff(Antenna::ANT_HORIZONTAL,offset);
		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::focusScan()");
			throw impl;
		}
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::AntennaScanErrorExImpl,impl,ex,"CCore::focusScan()");
		throw impl;
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::AntennaScanErrorExImpl,impl,ex,"CCore::focusScan()");
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::focusScan()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_antennaBossError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::focusScan()");
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
		_ADD_BACKTRACE(ManagementErrors::TsysErrorExImpl,impl,ex,"CCore::focusScan()");
		throw impl;
	}
	IRA::CIRATools::Wait(2,0);
	guard.acquire();
	try {
		if (!CORBA::is_nil(m_antennaBoss)) {
			m_antennaBoss->goOff(Antenna::ANT_HORIZONTAL,0.0);
		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::focusScan()");
			throw impl;
		}
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::AntennaScanErrorExImpl,impl,ex,"CCore::focusScan()");
		throw impl;
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::AntennaScanErrorExImpl,impl,ex,"CCore::focusScan()");
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::focusScan()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_antennaBossError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::focusScan()");
		m_antennaBossError=true;
		throw impl;
	}
	clearAntennaTracking();
	guard.release();
	waitOnSource();
	guard.acquire();

	initRecording(1); //  throw ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl
	// we already check the minor servo boss is available
	loadMinorServoBoss(m_minorServoBoss,m_minorServoBossError); // (ComponentErrors::CouldntGetComponentExImpl)

	try {
		if (!CORBA::is_nil(m_minorServoBoss)) {
			startTime=0;
			m_minorServoBoss->startFocusScan(startTime,span,duration);
			//startTime=m_antennaBoss->latOTFScan(scanFrame,span,duration);
		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::focusScan()");
			throw impl;
		}
	}
	catch (ManagementErrors::ConfigurationErrorEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::MinorServoScanErrorExImpl,impl,ex,"CCore::focusScan()");
		throw impl;
	}
	catch (ManagementErrors::SubscanErrorEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::MinorServoScanErrorExImpl,impl,ex,"CCore::focusScan()");
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCore::focusScan()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		m_antennaBossError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::focusScan()");
		m_antennaBossError=true;
		throw impl;
	}
	startRecording(startTime,1);
	// now set the the data transfer stop
	waitFor=startTime+duration; // this is the time at which the stop should be issued
	guard.release();
	waitUntil(waitFor); // throw ComponentErrors::TimerErrorExImpl
	stopRecording();
	guard.acquire();
	terminateScan();
	try {
		if (!CORBA::is_nil(m_minorServoBoss)) {
			m_minorServoBoss->stopScan();
		}
		else {
			_EXCPT(ComponentErrors::ComponentNotActiveExImpl,impl,"CCore::focusScan()");
			throw impl;
		}
	}
	catch (ManagementErrors::SubscanErrorEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::MinorServoScanErrorExImpl,impl,ex,"CCore::focusScan()");
		throw impl;
	}
	ACS_LOG(LM_FULL_INFO,"CCore::focusScan()",(LM_NOTICE,"FOCUS_SCAN_DONE"));
}

/*void CCore::crossScan(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration) throw (ManagementErrors::NotAllowedDuringScheduleExImpl,
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
	TIMEVALUE now;
	ACS::Time waitFor;
	ACS::Time startTime;
		
	
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

	// now set the the data transfer stop
	waitFor=startTime+duration; // this is the time at which the stop should be issued
	//waitMicro=(duration/10)/10000; // one tens of the total duration
	//IRA::CIRATools::getTime(now);
	guard.release();
	waitUntil(waitFor); // throw ComponentErrors::TimerErrorExImpl
	//guard.acquire();
	stopRecording();

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
	// now set the the data transfer stop
	waitFor=startTime+duration; // this is the time at which the stop should be issued
	//waitMicro=(duration/10)/10000; // one tens of the total duration
	//IRA::CIRATools::getTime(now);
	guard.release();	
	waitUntil(waitFor);
	//guard.acquire();
	stopRecording();
	guard.acquire();
	terminateScan();
	ACS_LOG(LM_FULL_INFO,"CCore::crossScan()",(LM_NOTICE,"CROSSSCAN_DONE"));
}*/

void CCore::crossScan(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration) throw (ManagementErrors::NotAllowedDuringScheduleExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,
		ManagementErrors::TsysErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl,
		ManagementErrors::AntennaScanErrorExImpl,ComponentErrors::TimerErrorExImpl,ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl)
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
	TIMEVALUE now;

	/*******************************************************/
	/* THIS MACRO METHOD SHOULD NOT TAKE THE MUTEX; RELYING ON THE MUTEX OF THE CALLED METHODS */
	/* IF I CAN GET RID OF THE GOOFF CALL TO THE ANTENNA THIS SHOULD BE ACCOMPLISHED VERY EASILY */

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
	clearAntennaTracking();
	guard.release(); // release the mutex...from now I rely on mutex of called methods
	waitOnSource();
	/***********************************************/
	/* I have to think again on I handle the call in case of error */
	/* isn't the callTsys an operation as all the others? */
	/*************************************************/
	try {
		ACS::doubleSeq tsys;
		callTSys(tsys);
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		_ADD_BACKTRACE(ManagementErrors::TsysErrorExImpl,impl,ex,"CCore::crossScan()");
		throw impl;
	}
	IRA::CIRATools::Wait(2,0);
	initRecording(1);
	ACS_LOG(LM_FULL_INFO,"CCore::crossScan()",(LM_NOTICE,"LATITUDE_SCAN"));
	// start the scan
	latOTF(scanFrame,span,duration); //ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl
	startRecording(1,duration); // start recording
	stopRecording();
	// LONGITUDE SCAN..............
	ACS_LOG(LM_FULL_INFO,"CCore::crossScan()",(LM_NOTICE,"LONGITUDE_SCAN"));
	// start the scan
	lonOTF(scanFrame,span,duration); //ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl
	startRecording(1,duration); // start recording the first subscan
	stopRecording();
	startRecording(2,duration); // start recording the second subscan
	stopRecording();
	terminateScan();
	ACS_LOG(LM_FULL_INFO,"CCore::crossScan()",(LM_NOTICE,"CROSSSCAN_DONE"));
}

void CCore::clearStatus()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	m_schedulerStatus=Management::MNG_OK;
	ACS_LOG(LM_FULL_INFO,"CCore::clearStatus()",(LM_NOTICE,"COMPONENT_STATUS_RESET"));
}

bool CCore::command(const IRA::CString& cmd,IRA::CString& answer)
{
	try {
		m_parser->run(cmd,answer); //parser is already thread safe.....
		return true;
	}
	catch (ParserErrors::ParserErrorsExImpl &ex) {
		return false;
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		ex.log(LM_ERROR); // the errors resulting from the execution are logged here as stated in the documentation of CommandInterpreter interface, while the parser errors are never logged.
		return false;
	}
}

#include "Core_Getter.i"

#include "Core_Operations.i"

/////// PRIVATES


#include "Core_Common.i"

#include "Core_Resource.i"

#include "Core_Extra.i"

#include "Core_Basic.i"

