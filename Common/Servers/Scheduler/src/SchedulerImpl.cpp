
#include "SchedulerImpl.h"
#include "Core.h"
#include "DevIOScanNumber.h"
#include "DevIOScheduleName.h"
#include "DevIOStatus.h"
#include "DevIOTracking.h"
#include "DevIOCurrentDevice.h"
#include <LogFilter.h>

static char *rcsId="@(#) schedulerImpl";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

_IRA_LOGFILTER_DECLARE;

SchedulerImpl::SchedulerImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) : 
	CharacteristicComponentImpl(CompName,containerServices),
	m_pscheduleName(this),
	m_pstatus(this),
	m_pscanID(this),
	m_psubScanID(this),
	m_ptracking(this),
	m_pcurrentDevice(this)
{	
	AUTO_TRACE("SchedulerImpl::SchedulerImpl()");
}

SchedulerImpl::~SchedulerImpl()
{
	AUTO_TRACE("SchedulerImpl::~SchedulerImpl()");
}

void SchedulerImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("SchedulerImpl::initialize()");
	ACS_LOG(LM_FULL_INFO,"SchedulerImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZING"));
	try {
		m_config.init(getContainerServices());    //thorw CDBAcessExImpl;
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,_dummy,E,"SchedulerImpl::initialize()");
		throw _dummy;
	}
	try {
		m_core=new CCore(getContainerServices(),&m_config);
		m_pscheduleName=new ROstring(getContainerServices()->getName()+":scheduleName",getComponent(),
				new DevIOScheduleName(m_core),true);
		m_pstatus=new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus>(getContainerServices()->getName()+":status",getComponent(),
				new DevIOStatus(m_core),true);
		m_pscanID=new ROlong(getContainerServices()->getName()+":scanID",getComponent(),new DevIOScanNumber(m_core,DevIOScanNumber::SCANID),true);
		m_psubScanID=new ROlong(getContainerServices()->getName()+":subScanID",getComponent(),new DevIOScanNumber(m_core,DevIOScanNumber::SUBSCANID),true);
		m_ptracking=new ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>(getContainerServices()->getName()+":tracking",getComponent(),
				new DevIOTracking(m_core),true);
		m_pcurrentDevice=new ROlong(getContainerServices()->getName()+":currentDevice",getComponent(),new DevIOCurrentDevice(m_core),true);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"SchedulerImpl::initialize()");
		throw dummy;
	}
	m_core->initialize();
	ACS_LOG(LM_FULL_INFO,"SchedulerImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZED"));
}

void SchedulerImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("SchedulerImpl::execute()");
	_IRA_LOGFILTER_ACTIVATE(m_config.getRepetitionCacheTime(),m_config.getRepetitionExpireTime());
	try {
		m_core->execute();
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,_dummy,E,"SchedulerImpl::execute()");
		throw _dummy;
	}
	try {
		startPropertiesMonitoring();
	}
	catch (acsthreadErrType::CanNotStartThreadExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"SchedulerImpl::execute()");
		throw __dummy;
	}
	catch (ACSErrTypeCommon::NullPointerExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"SchedulerImpl::execute()");
		throw __dummy;		
	}
	ACS_LOG(LM_FULL_INFO,"SchedulerImpl::execute()",(LM_INFO,"COMPSTATE_OPERATIONAL"));
}

void SchedulerImpl::cleanUp()
{
	AUTO_TRACE("SchedulerImpl::cleanUp()");
	stopPropertiesMonitoring();
	if (m_core) {
		m_core->cleanUp();
		delete m_core;
	}
	ACS_LOG(LM_FULL_INFO,"SchedulerImpl::cleanUp()",(LM_INFO,"SCHEDULER_CORE_FREED"));
	_IRA_LOGFILTER_FLUSH;
	ACS_LOG(LM_FULL_INFO,"SchedulerImpl::cleanUp()",(LM_INFO,"LOG_FLUSHED"));
	CharacteristicComponentImpl::cleanUp();
}

void SchedulerImpl::aboutToAbort()
{
	AUTO_TRACE("SchedulerImpl::aboutToAbort()"); 
	m_core->cleanUp();
	_IRA_LOGFILTER_FLUSH;
}

void SchedulerImpl::changeLogFile(const char *fileName) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	try {
		m_core->changeLogFile(fileName);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
	catch (ManagementErrors::ManagementErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getManagementErrorsEx();
	}
}

ACS::doubleSeq *SchedulerImpl::systemTemperature() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	AUTO_TRACE("SchedulerImpl::systemTemperature()");
	ACS::doubleSeq *out=NULL;
	out=new ACS::doubleSeq;
	try {
		m_core->callTSys(*out);
	}
	catch (ManagementErrors::ManagementErrorsExImpl& ex) {
		if (out!=NULL) delete out;
		out=NULL;
		ex.log(LM_DEBUG);
		throw ex.getManagementErrorsEx();
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		if (out!=NULL) delete out;
		out=NULL;		
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();		
	}
	catch (...) {
		if (out!=NULL) delete out;
		out=NULL;
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SchedulerImpl::systemTemperature()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	} 
	return out;
 }
 
char * SchedulerImpl::command(const char *cmd) throw (CORBA::SystemException,ManagementErrors::CommandLineErrorEx)
{
	try {
		return CORBA::string_dup((const char *)m_core->command(cmd)); 
	}
	catch (ManagementErrors::CommandLineErrorExImpl& ex) {
		throw ex.getCommandLineErrorEx();
	}
}

void SchedulerImpl::stopSchedule() throw (CORBA::SystemException)
{
	AUTO_TRACE("SchedulerImpl::stopSchedule()");
	m_core->stopSchedule();
}

void SchedulerImpl::haltSchedule() throw (CORBA::SystemException)
{
	AUTO_TRACE("SchedulerImpl::haltSchedule()");
	m_core->haltSchedule();
}

void SchedulerImpl::clearStatus() throw (CORBA::SystemException)
{
	AUTO_TRACE("SchedulerImpl::clearStatus()");
	m_core->resetSchedulerStatus();
}

void SchedulerImpl::chooseDefaultBackend(const char *bckInstance) throw (CORBA::SystemException)
{
	AUTO_TRACE("SchedulerImpl::chooseDefaultBackend()");
	m_core->chooseDefaultBackend(bckInstance);	
}

void SchedulerImpl::chooseDefaultDataRecorder(const char *rcvInstance) throw (CORBA::SystemException)
{
	AUTO_TRACE("SchedulerImpl::chooseDefaultDataRecorder()");
	m_core->chooseDefaultDataRecorder(rcvInstance);	
}

void SchedulerImpl::startSchedule(const char * fileName,const char *startSubScan) throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	AUTO_TRACE("SchedulerImpl::startSchedule()");
	try {
		m_core->startSchedule(fileName,startSubScan);
	}
	catch (ManagementErrors::ManagementErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getManagementErrorsEx();
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();		
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SchedulerImpl::startSchedule()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

void SchedulerImpl::crossScan(Management::TCoordinateFrame coordFrame,CORBA::Double span,ACS::TimeInterval duration) throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	Antenna::TCoordinateFrame frame;
	if (coordFrame==Management::MNG_EQUATORIAL) {
		frame=Antenna::ANT_EQUATORIAL;
	} else if (coordFrame==Management::MNG_GALACTIC) {
		frame=Antenna::ANT_GALACTIC;
	}
	else {
		frame=Antenna::ANT_HORIZONTAL;
	}
	try {
		m_core->crossScan(frame,span,duration);
	}
	catch (ManagementErrors::ManagementErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getManagementErrorsEx();
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();		
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SchedulerImpl::crossScan()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}	
}

void SchedulerImpl::setDevice(CORBA::Long deviceID) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	AUTO_TRACE("SchedulerImpl::setDevice()");
	try {
		m_core->setDevice(deviceID);
	}
	catch (ManagementErrors::ManagementErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getManagementErrorsEx();
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();		
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SchedulerImpl::setDevice()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}	
}

_PROPERTY_REFERENCE_CPP(SchedulerImpl,Management::ROTSystemStatus,m_pstatus,status);
_PROPERTY_REFERENCE_CPP(SchedulerImpl,ACS::ROstring,m_pscheduleName,scheduleName);
_PROPERTY_REFERENCE_CPP(SchedulerImpl,ACS::ROlong,m_pscanID,scanID);
_PROPERTY_REFERENCE_CPP(SchedulerImpl,ACS::ROlong,m_psubScanID,subScanID);
_PROPERTY_REFERENCE_CPP(SchedulerImpl,Management::ROTBoolean,m_ptracking,tracking);
_PROPERTY_REFERENCE_CPP(SchedulerImpl,ACS::ROlong,m_pcurrentDevice,currentDevice);


/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SchedulerImpl)

