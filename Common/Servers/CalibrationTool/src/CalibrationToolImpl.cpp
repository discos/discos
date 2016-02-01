
#include "CalibrationToolImpl.h"
#include <Definitions.h>
#include <IRATools.h>
#include <maciContainerServices.h>
#include <LogFilter.h>
#include "DevIOStatus.h"
#include "DevIOFileName.h"
#include "DevIOProjectName.h"
#include "DevIOObserver.h"
#include "DevIODeviceID.h"
#include "DevIOScanAxis.h"
#include "DevIODataY.h"
#include "DevIODataX.h"
#include "DevIOArrayDataY.h"
#include "DevIOArrayDataX.h"
#include "DevIOHPBW.h"
#include "DevIOAmplitude.h"
#include "DevIOPeakOffset.h"
#include "DevIOSlope.h"
#include "DevIOOffset.h"
#include "DevIOSourceFlux.h"


static char *rcsId="@(#) $Id: CalibrationToolImpl.cpp,v 1.5 2011-03-11 12:27:21 c.migoni Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

#define WORKTHREADNAME "CALIBRATIONTOOLENGINE"
#define COLLECTORTHREADNAME "COLLECTORTHREAD"

_IRA_LOGFILTER_DECLARE;

#define _GET_PROPERTY_VALUE_ONCE(OUTVAR,PROPERTYREF,PROPERTNAME,ROUTINE) \
try { \
	ACSErr::Completion_var cmpl; \
	OUTVAR=PROPERTYREF->get_sync(cmpl.out()); \
	CompletionImpl cmplImpl(cmpl.in()); \
	if (!cmplImpl.isErrorFree()) { \
		_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,cmplImpl,ROUTINE); \
		impl.setAttributeName(PROPERTNAME); \
		throw impl; \
	} \
} \
catch (...) { \
	_EXCPT(ComponentErrors::UnexpectedExImpl,impl,ROUTINE); \
	throw impl; \
} \


CalibrationToolImpl::CalibrationToolImpl(const ACE_CString& name, maci::ContainerServices* containerServices): 
	BulkDataReceiverImpl<CalibrationTool_private::ReceiverCallback>(name,containerServices),
	m_pstatus(this),
	m_pfileName(this),
    m_pprojectName(this),
    m_pobserver(this),
    m_pdeviceID(this),
    m_pscanAxis(this),
    m_pdataY(this),
    m_pdataX(this),
    m_parrayDataY(this),
    m_parrayDataX(this),
    m_phpbw(this),
    m_pamplitude(this),
    m_ppeakOffset(this),
    m_pslope(this),
    m_poffset(this),
    m_psourceFlux(this)
{ 
}

CalibrationToolImpl::~CalibrationToolImpl()
{
}

void CalibrationToolImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("CalibrationToolImpl::initialize()");
	//CalibrationTool_private::CDataCollection *data;
	ACS_LOG(LM_FULL_INFO,"CalibrationToolImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZING"));
	try {
		m_config.init(getContainerServices());    //thorw CDBAcessExImpl;
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,_dummy,E,"CalibrationToolImpl::initialize()");
		throw _dummy;
	}
	try {
		m_data=new CalibrationTool_private::CDataCollection();
		//m_dataWrapper=new CSecureArea<CalibrationTool_private::CDataCollection>(data);
		m_pstatus=new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus>
		  (getContainerServices()->getName()+":status",getComponent(),new CalibrationTool_private::DevIOStatus(m_data),true);
		m_pfileName=new ROstring(getContainerServices()->getName()+":fileName",getComponent(),
				new CalibrationTool_private::DevIOFileName(m_data),true);
		m_pprojectName=new ROstring(getContainerServices()->getName()+":projectName",getComponent(),
				new CalibrationTool_private::DevIOProjectName(m_data),true);
		m_pobserver=new ROstring(getContainerServices()->getName()+":observer",getComponent(),
				new CalibrationTool_private::DevIOObserver(m_data),true);
		m_pdeviceID=new ROlong(getContainerServices()->getName()+":deviceID",getComponent(),
				new CalibrationTool_private::DevIODeviceID(m_data),true);
		m_pscanAxis=new ROEnumImpl<ACS_ENUM_T(Management::TScanAxis),POA_Management::ROTScanAxis>
		  (getContainerServices()->getName()+":scanAxis",getComponent(),new CalibrationTool_private::DevIOScanAxis(m_data),true);
		m_pdataY=new ROdouble(getContainerServices()->getName()+":dataY",getComponent(),
				new CalibrationTool_private::DevIODataY(m_data),true);
		m_pdataX=new ROdouble(getContainerServices()->getName()+":dataX",getComponent(),
				new CalibrationTool_private::DevIODataX(m_data),true);
		m_parrayDataY=new ROdoubleSeq(getContainerServices()->getName()+":arrayDataY",getComponent(),
				new CalibrationTool_private::DevIOArrayDataY(m_data),true);
		m_parrayDataX=new ROdoubleSeq(getContainerServices()->getName()+":arrayDataX",getComponent(),
				new CalibrationTool_private::DevIOArrayDataX(m_data),true);
		m_phpbw=new ROdouble(getContainerServices()->getName()+":hpbw",getComponent(),
				new CalibrationTool_private::DevIOHPBW(m_data),true);
		m_pamplitude=new ROdouble(getContainerServices()->getName()+":amplitude",getComponent(),
				new CalibrationTool_private::DevIOAmplitude(m_data),true);
		m_ppeakOffset=new ROdouble(getContainerServices()->getName()+":peakOffset",getComponent(),
				new CalibrationTool_private::DevIOPeakOffset(m_data),true);
		m_pslope=new ROdouble(getContainerServices()->getName()+":slope",getComponent(),
				new CalibrationTool_private::DevIOSlope(m_data),true);
		m_poffset=new ROdouble(getContainerServices()->getName()+":offset",getComponent(),
				new CalibrationTool_private::DevIOOffset(m_data),true);
		m_psourceFlux=new ROdouble(getContainerServices()->getName()+":sourceFlux",getComponent(),
				new CalibrationTool_private::DevIOSourceFlux(m_data),true);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CalibrationToolImpl::initialize()");
		throw dummy;
	}
	CalibrationTool_private::ReceiverCallback::m_dataCollection=m_data;	
	try {
		m_workThread=(CalibrationTool_private::CEngineThread *)getContainerServices()->getThreadManager()->create
		<CalibrationTool_private::CEngineThread,CalibrationTool_private::CDataCollection *>(WORKTHREADNAME,m_data);
		m_collectThread=(CalibrationTool_private::CCollectorThread *)getContainerServices()->getThreadManager()->create
		<CalibrationTool_private::CCollectorThread,CalibrationTool_private::CDataCollection *>(COLLECTORTHREADNAME,m_data);
	}
	catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"CalibrationToolImpl::initialize()");
		throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CalibrationToolImpl::initialize()");
	}
	ACS_LOG(LM_FULL_INFO, "CalibrationToolImpl::initialize()", (LM_INFO,"THREAD_CREATED"));
	ACS_LOG(LM_FULL_INFO,"CalibrationToolImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZED"));
}

void CalibrationToolImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	ACS::TimeInterval interval;
	interval=(ACS::TimeInterval)m_config.getWorkingThreadTime()*10;
	m_workThread->setSleepTime(interval);
	m_workThread->setTimeSlice(m_config.getWorkingThreadTimeSlice());
	m_workThread->setConfiguration(&m_config);
	m_workThread->setServices(getContainerServices());
	m_workThread->initialize(); //  throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl)
	m_workThread->resume();
	interval=(ACS::TimeInterval)m_config.getCollectorThreadTime()*10;
	m_collectThread->setSleepTime(interval);
	m_collectThread->setConfiguration(&m_config);
	m_collectThread->setServices(getContainerServices());
	/*******************************************************/
	/* THREAD IS NOT RESUMED as it is not needed at the moment */
	/*******************************************************/
	//m_collectThread->resume();
	try {
		startPropertiesMonitoring();
	}
	catch (acsthreadErrType::CanNotStartThreadExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"CalibrationToolImpl::execute()");
		throw __dummy;
	}
	catch (ACSErrTypeCommon::NullPointerExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"CalibrationToolImpl::execute()");
		throw __dummy;		
	}	
	ACS_LOG(LM_FULL_INFO,"CalibrationToolImpl::execute()",(LM_INFO,"COMPSTATE_OPERATIONAL"));
}

void CalibrationToolImpl::cleanUp()
{
	stopPropertiesMonitoring();
	if (m_workThread!=NULL) {
		m_workThread->suspend();
		getContainerServices()->getThreadManager()->destroy(m_workThread);
	}
	if (m_collectThread!=NULL) {
		m_collectThread->suspend();
		getContainerServices()->getThreadManager()->destroy(m_collectThread);
	}
	ACS_LOG(LM_FULL_INFO,"CalibrationToolImpl::cleanUp()",(LM_INFO,"THREADS_DESTROYED"));
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
	ACS_LOG(LM_FULL_INFO,"CalibrationToolImpl::cleanUp()",(LM_INFO,"LOG_FLUSHED"));
	delete m_data;
	CharacteristicComponentImpl::cleanUp();	
}

void CalibrationToolImpl::aboutToAbort()
{
	if (m_workThread!=NULL) {
		m_workThread->suspend();
		getContainerServices()->getThreadManager()->destroy(m_workThread);
	}
	if (m_collectThread!=NULL) {
		m_collectThread->suspend();
		getContainerServices()->getThreadManager()->destroy(m_collectThread);
	}
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;	
	delete m_data;
}

void CalibrationToolImpl::startScan(const Management::TScanSetup & prm) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	//CSecAreaResourceWrapper<CalibrationTool_private::CDataCollection> data=m_dataWrapper->Get();
	bool rec,inc;
	if (!m_data->setScanSetup(prm,rec,inc)) {
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"CalibrationToolImpl::startScan");
		if (rec) {
			impl.setReason("Could not start a new scan while recording");
		}
		else if (inc) {
			impl.setReason("Could not start a new scan right now");
		}
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
	ACS_LOG(LM_FULL_INFO,"CalibrationToolImpl::startScan()",(LM_DEBUG,"START_SCAN_ISSUED"));
}

char * CalibrationToolImpl::startSubScan(const ::Management::TSubScanSetup & prm) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	//CSecAreaResourceWrapper<CalibrationTool_private::CDataCollection> data=m_dataWrapper->Get();
	bool rec,inc,noScan,warn;
	if (!m_data->setSubScanSetup(prm,rec,inc,noScan,warn)) {
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"CalibrationTool::startSubScan");
		if (rec) {
			impl.setReason("Could not start a new subscan while recording");
		}
		else if (inc) {
			impl.setReason("Could not start a new subscan right now");
		}
		else if (noScan) {
			impl.setReason("Could not start a subscan because the axis is not pertinent");
		}
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
	if (warn) {
		ACS_LOG(LM_FULL_INFO,"CalibrationToolImpl::startSubScan()",(LM_WARNING,"UNEXPECTED_SUBSCAN_SEQUENCE"));
	}
	ACS_LOG(LM_FULL_INFO,"CalibrationToolImpl::startSubScan()",(LM_DEBUG,"START_SUBSCAN_ISSUED"));
	IRA::CString file=m_data->getFileName();
	return CORBA::string_dup((const char *)file);
}

void CalibrationToolImpl::stopScan() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	//CSecAreaResourceWrapper<CalibrationTool_private::CDataCollection> data=m_dataWrapper->Get();
	m_data->stopScan();
	ACS_LOG(LM_FULL_INFO,"CalibrationToolImpl::stopScan()",(LM_DEBUG,"STOP_SCAN_ISSUED"));
}

CORBA::Boolean CalibrationToolImpl::isRecording() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	//CSecAreaResourceWrapper<CalibrationTool_private::CDataCollection> data=m_dataWrapper->Get();
	return (CORBA::Boolean)m_data->isRunning();
}


void CalibrationToolImpl::reset() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	//CSecAreaResourceWrapper<CalibrationTool_private::CDataCollection> data=m_dataWrapper->Get();
	m_data->forceReset();
}

void CalibrationToolImpl::setScanLayout (const ACS::stringSeq & layout) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
    ACS_LOG(LM_FULL_INFO,"CalibrationToolImpl::setScanLayout()",(LM_DEBUG,"SET_SCAN_LAYOUT"));
}

_PROPERTY_REFERENCE_CPP(CalibrationToolImpl,Management::ROTSystemStatus,m_pstatus,status);
_PROPERTY_REFERENCE_CPP(CalibrationToolImpl,ACS::ROstring,m_pfileName,fileName);
_PROPERTY_REFERENCE_CPP(CalibrationToolImpl,ACS::ROstring,m_pprojectName,projectName);
_PROPERTY_REFERENCE_CPP(CalibrationToolImpl,ACS::ROstring,m_pobserver,observer);
_PROPERTY_REFERENCE_CPP(CalibrationToolImpl,ACS::ROlong,m_pdeviceID,deviceID);
_PROPERTY_REFERENCE_CPP(CalibrationToolImpl,Management::ROTScanAxis,m_pscanAxis,scanAxis);
_PROPERTY_REFERENCE_CPP(CalibrationToolImpl,ACS::ROdouble,m_pdataY,dataY);
_PROPERTY_REFERENCE_CPP(CalibrationToolImpl,ACS::ROdouble,m_pdataX,dataX);
_PROPERTY_REFERENCE_CPP(CalibrationToolImpl,ACS::ROdoubleSeq,m_parrayDataY,arrayDataY);
_PROPERTY_REFERENCE_CPP(CalibrationToolImpl,ACS::ROdoubleSeq,m_parrayDataX,arrayDataX);
_PROPERTY_REFERENCE_CPP(CalibrationToolImpl,ACS::ROdouble,m_phpbw,hpbw);
_PROPERTY_REFERENCE_CPP(CalibrationToolImpl,ACS::ROdouble,m_pamplitude,amplitude);
_PROPERTY_REFERENCE_CPP(CalibrationToolImpl,ACS::ROdouble,m_ppeakOffset,peakOffset);
_PROPERTY_REFERENCE_CPP(CalibrationToolImpl,ACS::ROdouble,m_pslope,slope);
_PROPERTY_REFERENCE_CPP(CalibrationToolImpl,ACS::ROdouble,m_poffset,offset);
_PROPERTY_REFERENCE_CPP(CalibrationToolImpl,ACS::ROdouble,m_psourceFlux,sourceFlux);

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(CalibrationToolImpl)

