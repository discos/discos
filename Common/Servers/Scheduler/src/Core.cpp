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
	loadCustomLogger(m_customLogger,m_customLoggerError); // throw ComponentErrors::CouldntGetComponentExImpl

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
	m_parser->add("tsys",new function1<CCore,non_constant,void_type,O<doubleSeq_type> >(this,&CCore::_callTSys),0);
	m_parser->add("wait",new function1<CCore,non_constant,void_type,I<double_type> >(this,&CCore::_wait),1);
	m_parser->add("nop",new function0<CCore,constant,void_type >(this,&CCore::_nop),0);
	m_parser->add("waitOnSource",new function0<CCore,non_constant,void_type >(this,&CCore::_waitOnSource),0);
	m_parser->add("waitTracking",new function0<CCore,non_constant,void_type >(this,&CCore::_waitTracking),0);
	m_parser->add("waitUntil",new function1<CCore,non_constant,void_type,I<time_type> >(this,&CCore::_waitUntil),1);
	m_parser->add("haltSchedule",new function0<CCore,non_constant,void_type >(this,&CCore::_haltSchedule),0);
	m_parser->add("stopSchedule",new function0<CCore,non_constant,void_type >(this,&CCore::_stopSchedule),0);
	m_parser->add("startSchedule",new function2<CCore,non_constant,void_type,I<string_type>,I<string_type> >(this,&CCore::_startSchedule),2);
	m_parser->add("device",new function1<CCore,non_constant,void_type,I<long_type> >(this,&CCore::_setDevice),1);
	m_parser->add("chooseBackend",new function1<CCore,non_constant,void_type,I<string_type> >(this,&CCore::_chooseDefaultBackend),1);
	m_parser->add("chooseRecorder",new function1<CCore,non_constant,void_type,I<string_type> >(this,&CCore::_chooseDefaultDataRecorder),1);
	m_parser->add("crossScan",new function3<CCore,non_constant,void_type,I<enum_type<AntennaFrame2String,Antenna::TCoordinateFrame > >,I<angleOffset_type<rad> >,
			I<interval_type> >(this,&CCore::crossScan),3);
	m_parser->add("peakerScan",new function3<CCore,non_constant,void_type,I<string_type>,I<double_type >,I<interval_type> >(this,&CCore::peakerScan),3);
	m_parser->add("log",new function1<CCore,non_constant,void_type,I<string_type> >(this,&CCore::_changeLogFile),1);
	m_parser->add("logMessage",new function1<CCore,non_constant,void_type,I<string_type> >(this,&CCore::_logMessage),1);
	m_parser->add("wx",new function4<CCore,non_constant,void_type,O<double_type>,O<double_type>,O<double_type>,O<double_type> >(this,&CCore::_getWeatherStationParameters),0);
	m_parser->add("project",new function1<CCore,non_constant,void_type,I<string_type> >(this,&CCore::_setProjectCode),1);
	// no range checks because * is allowed
	m_parser->add("skydip",new function3<CCore,non_constant,void_type,I<elevation_type<rad,false> >,I<elevation_type<rad,false> >,I<interval_type> >(this,&CCore::skydip),3);
	m_parser->add("agc","_tp_agc",2,"NONE");
	// m_parser->add("ifd","_ifd",1,"SRT");
	m_parser->add("calmux","_calmux",1);
	m_parser->add("dmed","_dmed",1,"Medicina");
	m_parser->add("restFrequency",new function1<CCore,non_constant,void_type,I<doubleSeq_type> >(this,&CCore::_setRestFrequency),1);
	m_parser->add("fTrack",new function1<CCore,non_constant,void_type,I<string_type> >(this,&CCore::_fTrack),1);
	//m_parser->add("peaker",new function3<CCore,non_constant,void_type,I<string_type>,I<double_type>,I<interval_type> >(this,&CCore::_peaker),3);
	//m_parser->add("lonOTF",new function3<CCore,non_constant,void_type,I<enum_type<AntennaFrame2String,Antenna::TCoordinateFrame > >,I<angleOffset_type<rad> >, I<interval_type> >(this,&CCore::_lonOTF),3);
	//m_parser->add("latOTF",new function3<CCore,non_constant,void_type,I<enum_type<AntennaFrame2String,Antenna::TCoordinateFrame > >,I<angleOffset_type<rad> >, I<interval_type> >(this,&CCore::_latOTF),3);
	//m_parser->add("skydipOTF",new function3<CCore,non_constant,void_type,I<elevation_type<rad,false> >,I<elevation_type<rad,false> >,I<interval_type> >(this,&CCore::_skydipOTF),3);
	m_parser->add("moon",new function0<CCore,non_constant,void_type >(this,&CCore::_moon),0);
	m_parser->add("sidereal",new function5<CCore,non_constant,void_type,I<string_type>,I<rightAscension_type<rad,true> >,
			I<declination_type<rad,true> >,I<enum_type<AntennaEquinox2String,Antenna::TSystemEquinox > >,
			I<enum_type<AntennaSection2String,Antenna::TSections> > >(this,&CCore::_sidereal),5);
	m_parser->add("track",new function1<CCore,non_constant,void_type,I<string_type> >(this,&CCore::_track),1);
	m_parser->add("goTo",new function2<CCore,non_constant,void_type,I<azimuth_type<rad,false> >,I<elevation_type<rad,false> > >(this,&CCore::_goTo),2);
	m_parser->add("abort",new function0<CCore,non_constant,void_type >(this,&CCore::_abort),0);
	m_parser->add("initRecording",new function1<CCore,non_constant,void_type,I<long_type> >(this,&CCore::_initRecording),1);
	m_parser->add("startRecording",new function2<CCore,non_constant,void_type,I<long_type>,I<interval_type> >(this,&CCore::_startRecording),2);
	m_parser->add("terminateScan",new function0<CCore,non_constant,void_type >(this,&CCore::_terminateScan),0);
	m_parser->add("goOff",new function2<CCore,non_constant,void_type,I<enum_type<AntennaFrame2String,Antenna::TCoordinateFrame > >,I<double_type > >(this,&CCore::goOff),2);

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
	m_parser->add("bwhm","antenna",1,&CCore::remoteCall);
	m_parser->add("azelOffsets","antenna",1,&CCore::remoteCall);
	m_parser->add("radecOffsets","antenna",1,&CCore::remoteCall);
	m_parser->add("lonlatOffsets","antenna",1,&CCore::remoteCall);
	m_parser->add("antennaReset","antenna",1,&CCore::remoteCall);
	m_parser->add("radialVelocity","antenna",1,&CCore::remoteCall);
	//m_parser->add("goOff","antenna",1,&CCore::remoteCall);

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
	//m_parser->add("derotatorMode","receivers",2,&CCore::remoteCall);
	m_parser->add("derotatorSetPosition","receivers",2,&CCore::remoteCall);
	m_parser->add("derotatorRewind","receivers",2,&CCore::remoteCall);
	m_parser->add("derotatorSetConfiguration","receivers",2,&CCore::remoteCall);
	m_parser->add("derotatorSetRewindingMode","receivers",2,&CCore::remoteCall);
	m_parser->add("derotatorSetAutoRewindingSteps","receivers",2,&CCore::remoteCall);
	m_parser->add("externalCalOn","receivers",2,&CCore::remoteCall);
	m_parser->add("externalCalOff","receivers",2,&CCore::remoteCall);
	// backend
	//m_parser->add("bck","backends",3,&CCore::remoteCall);
	m_parser->add("integration","backend",3,&CCore::remoteCall);
	m_parser->add("setSection","backend",3,&CCore::remoteCall);
	m_parser->add("setAttenuation","backend",3,&CCore::remoteCall);
	m_parser->add("getTpi","backend",3,&CCore::remoteCall);
	m_parser->add("getZero","backend",3,&CCore::remoteCall);
	m_parser->add("initialize","backend",3,&CCore::remoteCall);
	m_parser->add("calSwitch","backend",3,&CCore::remoteCall);
	m_parser->add("getRms","backend",3,&CCore::remoteCall);

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

void CCore::skydip(const double& el1,const double& el2,const ACS::TimeInterval& duration) throw (
		ManagementErrors::NotAllowedDuringScheduleExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::OperationErrorExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,
		ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl,
		ManagementErrors::AntennaScanErrorExImpl,ComponentErrors::TimerErrorExImpl,ManagementErrors::TelescopeSubScanErrorExImpl,
		ManagementErrors::TargetOrSubscanNotFeasibleExImpl,ManagementErrors::AbortedByUserExImpl,
		ManagementErrors::RecordingAlreadyActiveExImpl,ManagementErrors::CloseTelescopeScanErrorExImpl)
{
	//no need to get the mutex, because it is already done inside the executor object
	if (m_schedExecuter) {
		if (m_schedExecuter->isScheduleActive()) {
			_THROW_EXCPT(ManagementErrors::NotAllowedDuringScheduleExImpl,"CCore::skydip()");
		}
	}
	ACS::doubleSeq tsys;
	//TIMEVALUE now;
	ACS::Time startTime=0; // start asap
	Antenna::TTrackingParameters primary,secondary;
	Management::TSubScanConfiguration subConf;
	MinorServo::MinorServoScan servo;
	Receivers::TReceiversParameters receievers;	
	
	// the mutex is not necessary as it's taken internally by all the called procedures, generally this macro calls should not required the mutex
	//baci::ThreadSyncGuard guard(&m_mutex);
	// TSYS SCAN
	ACS_LOG(LM_FULL_INFO,"CCore::skydip()",(LM_NOTICE,"TSYS_COMPUTATION"));
	// it calls directly the antennaBoss method
	goOff(Antenna::ANT_HORIZONTAL,3.0); // go off 3 beams sizes
	_waitOnSource();
	_callTSys(tsys);
	_wait(1.5);
	
	// prepare the subscans	
	Schedule::CSubScanBinder binder(&primary,&secondary,&servo,&receievers,&subConf);
	binder.skydip(el1,el2,duration,NULL);	
	m_scanBinder.nextScan();
	m_scanBinder.addSubScan(&primary,&secondary,&servo,&receievers);
	
	_initRecording(1);
	ACS_LOG(LM_FULL_INFO,"CCore::skydip()",(LM_NOTICE,"START_SKYDIP"));
	// start the scan	
	startTime=0; // it means start as soon as possible
	startScan(startTime,&primary,&secondary,&servo,&receievers,subConf,m_scanConf); //ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl
	m_subScanEpoch=startTime;
	
	_startRecording(1,duration); // start recording
	_terminateScan();
	ACS_LOG(LM_FULL_INFO,"CCore::skydip()",(LM_NOTICE,"SKYDIP_DONE"));
}

void CCore::peakerScan(const char *axis,const double& span,const ACS::TimeInterval& duration) throw (ComponentErrors::CouldntGetComponentExImpl,
		ComponentErrors::ComponentNotActiveExImpl,ManagementErrors::AntennaScanErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ManagementErrors::TsysErrorExImpl,ComponentErrors::OperationErrorExImpl,
		ManagementErrors::UnsupportedOperationExImpl,ManagementErrors::MinorServoScanErrorExImpl,ComponentErrors::TimerErrorExImpl,
		ManagementErrors::AbortedByUserExImpl,ManagementErrors::NotAllowedDuringScheduleExImpl,ManagementErrors::RecordingAlreadyActiveExImpl,
		ComponentErrors::ValidationErrorExImpl,ManagementErrors::TelescopeSubScanErrorExImpl)
{
	//no need to get the mutex, because it is already done inside the executor object
	if (m_schedExecuter) {
		if (m_schedExecuter->isScheduleActive()) {
			_THROW_EXCPT(ManagementErrors::NotAllowedDuringScheduleExImpl,"CCore::peakerScan()");
		}
	}
	ACS::doubleSeq tsys;
	//TIMEVALUE now;
	ACS::Time startTime=0; // start asap
	Antenna::TTrackingParameters primary,secondary;
	Management::TSubScanConfiguration subConf;
	MinorServo::MinorServoScan servo;
	Receivers::TReceiversParameters receievers;
	
	ACS_LOG(LM_FULL_INFO,"CCore::skydip()",(LM_NOTICE,"TSYS_COMPUTATION"));
	// it calls directly the antennaBoss method
	goOff(Antenna::ANT_HORIZONTAL,3.0); // go off 3 beams sizes
	_waitOnSource();
	_callTSys(tsys);
	_wait(1.5);

	// prepare the subscans	
	Schedule::CSubScanBinder binder(&primary,&secondary,&servo,&receievers,&subConf);
	binder.peaker(IRA::CString(axis),span,duration,NULL);	
	m_scanBinder.nextScan();
	m_scanBinder.addSubScan(&primary,&secondary,&servo,&receievers);
	
	_initRecording(1);
	ACS_LOG(LM_FULL_INFO,"CCore::skydip()",(LM_NOTICE,"START_PEAKERSCAN"));
	// start the scan
	startTime=0; // it means start as soon as possible
	startScan(startTime,&primary,&secondary,&servo,&receievers,subConf,m_scanConf); //ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl
	m_subScanEpoch=startTime;

	_startRecording(1,duration); // start recording
	_terminateScan();
}

void CCore::crossScan(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration) throw (
		ManagementErrors::NotAllowedDuringScheduleExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::OperationErrorExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,
		ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl,
		ManagementErrors::AntennaScanErrorExImpl,ComponentErrors::TimerErrorExImpl,ManagementErrors::TelescopeSubScanErrorExImpl,
		ManagementErrors::TargetOrSubscanNotFeasibleExImpl,ManagementErrors::AbortedByUserExImpl,
		ManagementErrors::RecordingAlreadyActiveExImpl,ManagementErrors::CloseTelescopeScanErrorExImpl)
{
	//no need to get the mutex, because it is already done inside the executor object
	if (m_schedExecuter) {
		if (m_schedExecuter->isScheduleActive()) {
			_THROW_EXCPT(ManagementErrors::NotAllowedDuringScheduleExImpl,"CCore::crossScan()");
		}
	}
	IRA::CString obsName,prj,suffix,path,extraPath,baseName;
	IRA::CString layoutName,schedule;
	ACS::stringSeq layout;
	ACS::doubleSeq tsys;
	//TIMEVALUE now;
	ACS::Time startTime=0; // start asap
	Antenna::TTrackingParameters primaryLon,secondaryLon,primaryLat,secondaryLat;
	Management::TSubScanConfiguration subConfLon,subConfLat;
	MinorServo::MinorServoScan servoLon,servoLat;
	Receivers::TReceiversParameters receieversLon,receieversLat;
	// the mutex is not necessary as it's taken internally by all the called procedures, generally this macro calls should not required the mutex
	//baci::ThreadSyncGuard guard(&m_mutex);
	// TSYS SCAN
	ACS_LOG(LM_FULL_INFO,"CCore::crossScan()",(LM_NOTICE,"TSYS_COMPUTATION"));
	// it calls directly the antennaBoss method
	goOff(scanFrame,3.0); // go off 3 beams sizes
	_waitOnSource();
	_callTSys(tsys);
	_wait(1.5);
	
	// prepare the subscans	
	Schedule::CSubScanBinder binderLat(&primaryLat,&secondaryLat,&servoLat,&receieversLat,&subConfLat);
	binderLat.latOTF(scanFrame,span,duration);
	Schedule::CSubScanBinder binderLon(&primaryLon,&secondaryLon,&servoLon,&receieversLon,&subConfLon);
	binderLon.lonOTF(scanFrame,span,duration);	
	m_scanBinder.nextScan();
	m_scanBinder.addSubScan(&primaryLat,&secondaryLat,&servoLat,&receieversLat);
	m_scanBinder.addSubScan(&primaryLat,&secondaryLat,&servoLon,&receieversLon);
	
	_initRecording(1);
	ACS_LOG(LM_FULL_INFO,"CCore::crossScan()",(LM_NOTICE,"LATITUDE_SCAN"));

	// start the LAT scan
	startTime=0; // it means start as soon as possible
	startScan(startTime,&primaryLat,&secondaryLat,&servoLat,&receieversLat,subConfLat,m_scanConf); //ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl
	m_subScanEpoch=startTime;

	_startRecording(1,duration); // start recording
	// LONGITUDE SCAN..............
	ACS_LOG(LM_FULL_INFO,"CCore::crossScan()",(LM_NOTICE,"LONGITUDE_SCAN"));
	// start the LON scan
	startTime=0; // it means start as soon as possible
	startScan(startTime,&primaryLon,&secondaryLon,&servoLon,&receieversLon,subConfLon,m_scanConf); //ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl
	m_subScanEpoch=startTime;
	_startRecording(2,duration); // start recording the second subscan
	
	_terminateScan();
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

