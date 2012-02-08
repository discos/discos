/* $Id: CalibrationToolImpl.cpp,v 1.5 2011-03-11 12:27:21 c.migoni Exp $ */

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
	CalibrationTool_private::CDataCollection *data;
	ACS_LOG(LM_FULL_INFO,"CalibrationToolImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZING"));
	try {
		m_config.init(getContainerServices());    //thorw CDBAcessExImpl;
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,_dummy,E,"CalibrationToolImpl::initialize()");
		throw _dummy;
	}
	try {
		data=new CalibrationTool_private::CDataCollection();
		m_dataWrapper=new CSecureArea<CalibrationTool_private::CDataCollection>(data);
		m_pstatus=new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus>
		  (getContainerServices()->getName()+":status",getComponent(),new CalibrationTool_private::DevIOStatus(m_dataWrapper),true);
		m_pfileName=new ROstring(getContainerServices()->getName()+":fileName",getComponent(),
				new CalibrationTool_private::DevIOFileName(m_dataWrapper),true);
		m_pprojectName=new ROstring(getContainerServices()->getName()+":projectName",getComponent(),
				new CalibrationTool_private::DevIOProjectName(m_dataWrapper),true);
		m_pobserver=new ROstring(getContainerServices()->getName()+":observer",getComponent(),
				new CalibrationTool_private::DevIOObserver(m_dataWrapper),true);
		m_pdeviceID=new ROlong(getContainerServices()->getName()+":deviceID",getComponent(),
				new CalibrationTool_private::DevIODeviceID(m_dataWrapper),true);
		m_pscanAxis=new ROEnumImpl<ACS_ENUM_T(Management::TScanAxis),POA_Management::ROTScanAxis>
		  (getContainerServices()->getName()+":scanAxis",getComponent(),new CalibrationTool_private::DevIOScanAxis(m_dataWrapper),true);
		m_pdataY=new ROdouble(getContainerServices()->getName()+":dataY",getComponent(),
				new CalibrationTool_private::DevIODataY(m_dataWrapper),true);
		m_pdataX=new ROdouble(getContainerServices()->getName()+":dataX",getComponent(),
				new CalibrationTool_private::DevIODataX(m_dataWrapper),true);
		m_parrayDataY=new ROdoubleSeq(getContainerServices()->getName()+":arrayDataY",getComponent(),
				new CalibrationTool_private::DevIOArrayDataY(m_dataWrapper),true);
		m_parrayDataX=new ROdoubleSeq(getContainerServices()->getName()+":arrayDataX",getComponent(),
				new CalibrationTool_private::DevIOArrayDataX(m_dataWrapper),true);
		m_phpbw=new ROdouble(getContainerServices()->getName()+":hpbw",getComponent(),
				new CalibrationTool_private::DevIOHPBW(m_dataWrapper),true);
		m_pamplitude=new ROdouble(getContainerServices()->getName()+":amplitude",getComponent(),
				new CalibrationTool_private::DevIOAmplitude(m_dataWrapper),true);
		m_ppeakOffset=new ROdouble(getContainerServices()->getName()+":peakOffset",getComponent(),
				new CalibrationTool_private::DevIOPeakOffset(m_dataWrapper),true);
		m_pslope=new ROdouble(getContainerServices()->getName()+":slope",getComponent(),
				new CalibrationTool_private::DevIOSlope(m_dataWrapper),true);
		m_poffset=new ROdouble(getContainerServices()->getName()+":offset",getComponent(),
				new CalibrationTool_private::DevIOOffset(m_dataWrapper),true);
		m_psourceFlux=new ROdouble(getContainerServices()->getName()+":sourceFlux",getComponent(),
				new CalibrationTool_private::DevIOSourceFlux(m_dataWrapper),true);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CalibrationToolImpl::initialize()");
		throw dummy;
	}
	CalibrationTool_private::ReceiverCallback::m_dataCollection=m_dataWrapper;	
	try {
		m_workThread=(CalibrationTool_private::CEngineThread *)getContainerServices()->getThreadManager()->create
		<CalibrationTool_private::CEngineThread,CSecureArea<CalibrationTool_private::CDataCollection> *>(WORKTHREADNAME,m_dataWrapper);
		m_collectThread=(CalibrationTool_private::CCollectorThread *)getContainerServices()->getThreadManager()->create
		<CalibrationTool_private::CCollectorThread,CSecureArea<CalibrationTool_private::CDataCollection> *>(COLLECTORTHREADNAME,m_dataWrapper);
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
	m_workThread->setSleepTime(m_config.getWorkingThreadTime());
	m_workThread->setTimeSlice(m_config.getWorkingThreadTimeSlice());
	m_workThread->setConfiguration(&m_config);
	m_workThread->setServices(getContainerServices());
	m_workThread->initialize(); //  throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl)
	m_workThread->resume();
	m_collectThread->setSleepTime(m_config.getCollectorThreadTime());
	m_collectThread->setConfiguration(&m_config);
	m_collectThread->setServices(getContainerServices());
	m_collectThread->resume();
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
	delete m_dataWrapper;
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
	delete m_dataWrapper;
}

void CalibrationToolImpl::startScan(const Management::TScanSetup & prm) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	CSecAreaResourceWrapper<CalibrationTool_private::CDataCollection> data=m_dataWrapper->Get();
	bool rec,inc;
	if (!data->setScanSetup(prm,rec,inc)) {
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

void CalibrationToolImpl::startSubScan(const ::Management::TSubScanSetup & prm) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	CSecAreaResourceWrapper<CalibrationTool_private::CDataCollection> data=m_dataWrapper->Get();
	bool rec,inc;
	if (!data->setSubScanSetup(prm,rec,inc)) {
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"CalibrationTool::startSubScan");
		if (rec) {
			impl.setReason("Could not start a new subscan while recording");
		}
		else if (inc) {
			impl.setReason("Could not start a new subscan right now");
		}
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
	ACS_LOG(LM_FULL_INFO,"CalibrationToolImpl::startSubScan()",(LM_DEBUG,"START_SUBSCAN_ISSUED"));
}

void CalibrationToolImpl::stopScan() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	CSecAreaResourceWrapper<CalibrationTool_private::CDataCollection> data=m_dataWrapper->Get();
	data->scanStop();
	ACS_LOG(LM_FULL_INFO,"CalibrationToolImpl::stopScan()",(LM_DEBUG,"STOP_SCAN_ISSUED"));
}

CORBA::Boolean CalibrationToolImpl::isRecording() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	CSecAreaResourceWrapper<CalibrationTool_private::CDataCollection> data=m_dataWrapper->Get();
	return (CORBA::Boolean)data->isRunning();
}


void CalibrationToolImpl::reset() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	CSecAreaResourceWrapper<CalibrationTool_private::CDataCollection> data=m_dataWrapper->Get();
	data->forceReset();
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

