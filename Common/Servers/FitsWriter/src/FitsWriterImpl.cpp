
#include "FitsWriterImpl.h"
#include <Definitions.h>
#include <IRATools.h>
#include <maciContainerServices.h>
#include <LogFilter.h>
#include <DBTable.h>
#include <ObservatoryC.h>
#include "DevIOStatus.h"
#include "DevIOFileName.h"
#include "DevIOProjectName.h"
#include "DevIOObserver.h"
#include "DevIODeviceID.h"
#include "DevIOScanAxis.h"

//static char *rcsId="@(#) $Id: FitsWriterImpl.cpp,v 1.12 2011-06-21 16:38:41 a.orlati Exp $";
//static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

#define WORKTHREADNAME "FITSENGINE"
#define COLLECTORTHREADNAME "COLLECTORTHREAD"

_IRA_LOGFILTER_DECLARE;

using namespace baci;


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


FitsWriterImpl::FitsWriterImpl(const ACE_CString& name, maci::ContainerServices* containerServices): 
	BulkDataReceiverImpl<FitsWriter_private::ReceiverCallback>(name,containerServices),
	m_pfileName(this),
	m_pstatus(this),
	m_pprojectName(this),
	m_pobserver(this),
	/*m_pscanIdentifier(this),*/
	m_pdeviceID(this),
	m_pscanAxis(this),
	m_pdataX(this),
	m_pdataY(this),
	m_parrayDataX(this),
	m_parrayDataY(this)
{ 
}

FitsWriterImpl::~FitsWriterImpl()
{
}

void FitsWriterImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("FitsWriterImpl::initialize()");
	//FitsWriter_private::CDataCollection *m_data;
	ACS_LOG(LM_FULL_INFO,"FitsWriterImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZING"));
	try {
		m_config.init(getContainerServices());    //thorw CDBAcessExImpl;
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,_dummy,E,"FitsWriterImpl::initialize()");
		throw _dummy;
	}
	try {
		m_data=new FitsWriter_private::CDataCollection();
		//m_dataWrapper=new CSecureArea<FitsWriter_private::CDataCollection>(data);
		m_pfileName=new ROstring(getContainerServices()->getName()+":fileName",getComponent(),new FitsWriter_private::DevIOFileName(m_data),true);
		m_pstatus=new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus>
		  (getContainerServices()->getName()+":status",getComponent(),new FitsWriter_private::DevIOStatus(m_data),true);
		m_pprojectName=new ROstring(getContainerServices()->getName()+":projectName",getComponent(),new FitsWriter_private::DevIOProjectName(m_data),true);
		m_pobserver=new ROstring(getContainerServices()->getName()+":observer",getComponent(),new FitsWriter_private::DevIOObserver(m_data),true);
		m_pdeviceID=new ROlong(getContainerServices()->getName()+":deviceID",getComponent(),new FitsWriter_private::DevIODeviceID(m_data),true);
		m_pscanAxis=new ROEnumImpl<ACS_ENUM_T(Management::TScanAxis),POA_Management::ROTScanAxis>(getContainerServices()->getName()+":scanAxis",getComponent(),
				new FitsWriter_private::DevIOScanAxis(m_data),true);
		m_pdataX=new ROdouble(getContainerServices()->getName()+":dataX",getComponent());
		m_pdataY=new ROdouble(getContainerServices()->getName()+":dataY",getComponent());
		m_parrayDataX=new ROdoubleSeq(getContainerServices()->getName()+":arrayDataX",getComponent());
		m_parrayDataY=new ROdoubleSeq(getContainerServices()->getName()+":arrayDataY",getComponent());
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"FitsWriterImpl::initialize()");
		throw dummy;
	}
	FitsWriter_private::ReceiverCallback::m_dataCollection=m_data;	
	try {
		m_workThread=(FitsWriter_private::CEngineThread *)getContainerServices()->getThreadManager()->create
		<FitsWriter_private::CEngineThread,FitsWriter_private::CDataCollection *>(WORKTHREADNAME,m_data);
		m_collectThread=(FitsWriter_private::CCollectorThread *)getContainerServices()->getThreadManager()->create
		<FitsWriter_private::CCollectorThread,FitsWriter_private::CDataCollection *>(COLLECTORTHREADNAME,m_data);
	}
	catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"FitsWriterImpl::initialize()");
		throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"FitsWriterImpl::initialize()");
	}
	ACS_LOG(LM_FULL_INFO, "FitsWriterImpl::initialize()", (LM_INFO,"THREAD_CREATED"));
	ACS_LOG(LM_FULL_INFO,"FitsWriterImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZED"));
}

void FitsWriterImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	Antenna::TSiteInformation_var site;
	Antenna::Observatory_var observatory=Antenna::Observatory::_nil();
	ACS::ROstring_var obsName;
	CORBA::String_var siteName;
	_IRA_LOGFILTER_ACTIVATE(m_config.getRepetitionCacheTime(),m_config.getRepetitionExpireTime());
	try {
		observatory=getContainerServices()->getDefaultComponent<Antenna::Observatory>((const char*)m_config.getObservatoryComponent());
	}
	catch (maciErrType::CannotGetComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"FitsWriterImpl::execute()");
		Impl.setComponentName((const char*)m_config.getObservatoryComponent());
		throw Impl;
	}
	catch (maciErrType::NoPermissionExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"FitsWriterImpl::execute()");
		Impl.setComponentName((const char*)m_config.getObservatoryComponent());
		throw Impl;		
	}
	catch (maciErrType::NoDefaultComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"FitsWriterImpl::execute()");
		Impl.setComponentName((const char*)m_config.getObservatoryComponent());
		throw Impl;				
	}		
	ACS_LOG(LM_FULL_INFO,"FitsWriterImpl::execute()",(LM_INFO,"OBSERVATORY_LOCATED"));
	try	{	
		site=observatory->getSiteSummary();  //throw CORBA::SYSTEMEXCEPTION
		obsName=observatory->observatoryName();
	}
	catch (CORBA::SystemException& ex)	{
		_EXCPT(ComponentErrors::CORBAProblemExImpl,__dummy,"FitsWriterImpl::execute()");
		__dummy.setName(ex._name());
		__dummy.setMinor(ex.minor());
		throw __dummy;
	}
	//this macro could throw ACSBase exception....
	_GET_PROPERTY_VALUE_ONCE(siteName,obsName,"observatoryName","FitsWriterImpl::execute()");
	//CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data=m_dataWrapper->Get();
	IRA::CSite siteInfo=CSite(site.out());
	double dut1=site->DUT1;
	m_data->setSite(siteInfo,dut1,(const char *)siteName);
	ACS_LOG(LM_FULL_INFO,"FitsWriterImpl::execute()",(LM_INFO,"SITE_INITIALIZED"));
	try {
		getContainerServices()->releaseComponent((const char *)observatory->name());
	}
	catch  (maciErrType::CannotReleaseComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"FitsWriterImpl::execute()");
		Impl.setComponentName((const char*)observatory->name());
		throw Impl;
	}
	ACS::TimeInterval interval;
	interval=(ACS::TimeInterval)m_config.getWorkingThreadTime()*10;
	m_workThread->setSleepTime(interval);
	m_workThread->setTimeSlice(m_config.getWorkingThreadTimeSlice());
	m_workThread->setConfiguration(&m_config);
	m_workThread->setServices(getContainerServices());
	m_workThread->resume();
	interval=(ACS::TimeInterval)m_config.getCollectorThreadTime()*10;
	m_collectThread->setSleepTime(interval);
	m_collectThread->setMeteoParamDuty(m_config.getMeteoParameterDutyCycle());
	m_collectThread->setTrackingFlagDuty(m_config.getTrackingFlagDutyCycle());
	m_collectThread->setConfiguration(&m_config);
	m_collectThread->setServices(getContainerServices());
	m_collectThread->resume();
	try {
		startPropertiesMonitoring();
	}
	catch (acsthreadErrType::CanNotStartThreadExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"FitsWriterImpl::execute()");
		throw __dummy;
	}
	catch (ACSErrTypeCommon::NullPointerExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"FitsWriterImpl::execute()");
		throw __dummy;		
	}	
	ACS_LOG(LM_FULL_INFO,"FitsWriterImpl::execute()",(LM_INFO,"COMPSTATE_OPERATIONAL"));
}

void FitsWriterImpl::cleanUp()
{
	stopPropertiesMonitoring();
	//CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data=m_dataWrapper->Get();
	if (m_workThread!=NULL) {
		m_workThread->suspend();
		getContainerServices()->getThreadManager()->destroy(m_workThread);
	}
	if (m_collectThread!=NULL) {
		m_collectThread->suspend();
		getContainerServices()->getThreadManager()->destroy(m_collectThread);
	}
	ACS_LOG(LM_FULL_INFO,"FitsWriterImpl::cleanUp()",(LM_INFO,"THREADS_DESTROYED"));
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
	ACS_LOG(LM_FULL_INFO,"FitsWriterImpl::cleanUp()",(LM_INFO,"LOG_FLUSHED"));
	//delete m_dataWrapper;
	delete m_data;
	CharacteristicComponentImpl::cleanUp();	
}

void FitsWriterImpl::aboutToAbort()
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

char *FitsWriterImpl::startScan(const Management::TScanSetup & prm) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	//CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data=m_dataWrapper->Get();
	bool rec,inc;
	if (!m_data->setScanSetup(prm,rec,inc)) {
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"FitsWriterImpl::startScan");
		if (rec) {
			impl.setReason("Could not start a new scan while recording");
		}
		else if (inc) {
			impl.setReason("Could not start a new scan right now");
		}
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
	ACS_LOG(LM_FULL_INFO,"FitsWriterImpl::startScan()",(LM_DEBUG,"START_SCAN_ISSUED"));
	IRA::CString file;
	IRA::CString path;
	m_data->getFileName(file,path);
	return CORBA::string_dup((const char *)path);
}

void FitsWriterImpl::setScanLayout (const ACS::stringSeq & layout) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	ACS_LOG(LM_FULL_INFO,"FitsWriterImpl::setScanLayout()",(LM_INFO,"LAYOUT_IGNORED"));
}

void FitsWriterImpl::stopScan() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	//CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data=m_dataWrapper->Get();
	m_data->stopScan();
	ACS_LOG(LM_FULL_INFO,"FitsWriterImpl::stopScan()",(LM_DEBUG,"STOP_SCAN_ISSUED"));
}

char *FitsWriterImpl::startSubScan(const ::Management::TSubScanSetup & prm) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	//CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data=m_dataWrapper->Get();
	bool rec,inc;
	if (!m_data->setSubScanSetup(prm,rec,inc)) {
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"FitsWriterImpl::startSubScan");
		if (rec) {
			impl.setReason("Could not start a new subscan while recording");
		}
		else if (inc) {
			impl.setReason("Could not start a new subscan right now");
		}
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
	ACS_LOG(LM_FULL_INFO,"FitsWriterImpl::startSubScan()",(LM_DEBUG,"START_SUBSCAN_ISSUED"));
	IRA::CString file=m_data->getFileName();
	return CORBA::string_dup((const char *)file);
}

CORBA::Boolean FitsWriterImpl::isRecording() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	//CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data=m_dataWrapper->Get();
	return (CORBA::Boolean)m_data->isRunning();
}


void FitsWriterImpl::reset() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
{
	//CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data=m_dataWrapper->Get();
	m_data->forceReset();
}


_PROPERTY_REFERENCE_CPP(FitsWriterImpl,Management::ROTSystemStatus,m_pstatus,status);
_PROPERTY_REFERENCE_CPP(FitsWriterImpl,ACS::ROstring,m_pfileName,fileName);
_PROPERTY_REFERENCE_CPP(FitsWriterImpl,ACS::ROstring,m_pprojectName,projectName);
_PROPERTY_REFERENCE_CPP(FitsWriterImpl,ACS::ROstring,m_pobserver,observer);
/*_PROPERTY_REFERENCE_CPP(FitsWriterImpl,ACS::ROlong,m_pscanIdentifier,scanIdentifier);*/
_PROPERTY_REFERENCE_CPP(FitsWriterImpl,ACS::ROlong,m_pdeviceID,deviceID);
_PROPERTY_REFERENCE_CPP(FitsWriterImpl,Management::ROTScanAxis,m_pscanAxis,scanAxis);
_PROPERTY_REFERENCE_CPP(FitsWriterImpl,ACS::ROdouble,m_pdataX,dataX);
_PROPERTY_REFERENCE_CPP(FitsWriterImpl,ACS::ROdouble,m_pdataY,dataY);
_PROPERTY_REFERENCE_CPP(FitsWriterImpl,ACS::ROdoubleSeq,m_parrayDataX,arrayDataX);
_PROPERTY_REFERENCE_CPP(FitsWriterImpl,ACS::ROdoubleSeq,m_parrayDataY,arrayDataY);

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(FitsWriterImpl)

