
#include "DataWriterImpl.h"
#include <Definitions.h>
#include <IRATools.h>
#include <maciContainerServices.h>
#include <LogFilter.h>
#include <DBTable.h>
/*#include <ObservatoryC.h>
#include "DevIOStatus.h"
#include "DevIOFileName.h"
#include "DevIOProjectName.h"
#include "DevIOObserver.h"
#include "DevIODeviceID.h"
#include "DevIOScanAxis.h"*/

// static char *rcsId="@(#) $Id: FitsWriterImpl.cpp,v 1.12 2011-06-21 16:38:41 a.orlati Exp $";
// static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

#define WORKTHREADNAME "FITSENGINE"
//#define COLLECTORTHREADNAME "COLLECTORTHREAD"

_IRA_LOGFILTER_DECLARE;

using namespace baci;

#define _GET_PROPERTY_VALUE_ONCE(OUTVAR, PROPERTYREF, PROPERTNAME, ROUTINE)                      \
	try                                                                                          \
	{                                                                                            \
		ACSErr::Completion_var cmpl;                                                             \
		OUTVAR = PROPERTYREF->get_sync(cmpl.out());                                              \
		CompletionImpl cmplImpl(cmpl.in());                                                      \
		if (!cmplImpl.isErrorFree())                                                             \
		{                                                                                        \
			_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl, impl, cmplImpl, ROUTINE); \
			impl.setAttributeName(PROPERTNAME);                                                  \
			throw impl;                                                                          \
		}                                                                                        \
	}                                                                                            \
	catch (...)                                                                                  \
	{                                                                                            \
		_EXCPT(ComponentErrors::UnexpectedExImpl, impl, ROUTINE);                                \
		throw impl;                                                                              \
	}

DataWriterImpl::DataWriterImpl(const ACE_CString &name, maci::ContainerServices *containerServices) : bulkdataZMQImpl::BulkDataZMQReceiverImpl<DataWriter_private::ReceiverCallback>(name, containerServices),
																									  m_pfileName(this),
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

DataWriterImpl::~DataWriterImpl()
{
}

// throw (ACSErr::ACSbaseExImpl)
void DataWriterImpl::initialize()
{
	AUTO_TRACE("DataWriterImpl::initialize()");
	ACS_LOG(LM_FULL_INFO, "DataWriterImpl::initialize()", (LM_INFO, "COMPSTATE_INITIALIZING"));
	try
	{
		m_compConfig.init(getContainerServices()); // thorw CDBAcessExImpl;
	}
	catch (ACSErr::ACSbaseExImpl &E)
	{
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl, _dummy, E, "DataWriterImpl::initialize()");
		throw _dummy;
	}
	try
	{
		m_data = new DataWriter_private::CDataCollection();
		m_pfileName = new ROstring(getContainerServices()->getName() + ":fileName", getComponent());
		m_pprojectName = new ROstring(getContainerServices()->getName() + ":projectName", getComponent());
		m_pobserver = new ROstring(getContainerServices()->getName() + ":observer", getComponent());
		m_pdeviceID = new ROlong(getContainerServices()->getName() + ":deviceID", getComponent());
		m_pscanAxis = new ROEnumImpl<ACS_ENUM_T(Management::TScanAxis), POA_Management::ROTScanAxis>(getContainerServices()->getName() + ":scanAxis", getComponent());
		m_pdataX = new ROdouble(getContainerServices()->getName() + ":dataX", getComponent());
		m_pdataY = new ROdouble(getContainerServices()->getName() + ":dataY", getComponent());
		m_parrayDataX = new ROdoubleSeq(getContainerServices()->getName() + ":arrayDataX", getComponent());
		m_parrayDataY = new ROdoubleSeq(getContainerServices()->getName() + ":arrayDataY", getComponent());
	}
	catch (std::bad_alloc &ex)
	{
		_EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "DataWriterImpl::initialize()");
		throw dummy;
	}
	DataWriter_private::ReceiverCallback::m_dataCollection = m_data;
	try
	{
		m_workThread=(DataWriter_private::CEngineThread *)getContainerServices()->getThreadManager()->create
		<DataWriter_private::CEngineThread,DataWriter_private::CDataCollection *>(WORKTHREADNAME,m_data);
		/*m_collectThread=(DataWriter_private::CCollectorThread *)getContainerServices()->getThreadManager()->create
		<DataWriter_private::CCollectorThread,DataWriter_private::CDataCollection *>(COLLECTORTHREADNAME,m_data);*/
	}
	catch (acsthreadErrType::acsthreadErrTypeExImpl &ex)
	{
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl, _dummy, ex, "DataWriterImpl::initialize()");
		throw _dummy;
	}
	catch (...)
	{
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl, "DataWriterImpl::initialize()");
	}
	ACS_LOG(LM_FULL_INFO, "DataWriterImpl::initialize()", (LM_INFO, "THREAD_CREATED"));
	ACS_LOG(LM_FULL_INFO, "DataWriterImpl::initialize()", (LM_INFO, "COMPSTATE_INITIALIZED"));
}

// throw (ACSErr::ACSbaseExImpl)
void DataWriterImpl::execute()
{
	_IRA_LOGFILTER_ACTIVATE(m_compConfig.getRepetitionCacheTime(), m_compConfig.getRepetitionExpireTime());

	ACS::TimeInterval interval;
	interval=(ACS::TimeInterval)m_compConfig.getWorkingThreadTime()*10;
	m_workThread->setSleepTime(interval);
	m_workThread->setTimeSlice(m_compConfig.getWorkingThreadTimeSlice());
	m_workThread->setConfiguration(&m_compConfig);
	m_workThread->setServices(getContainerServices());
	m_workThread->resume();
	/*interval=(ACS::TimeInterval)m_config.getCollectorThreadTime()*10;
	m_collectThread->setSleepTime(interval);
	m_collectThread->setMeteoParamDuty(m_config.getMeteoParameterDutyCycle());
	m_collectThread->setTrackingFlagDuty(m_config.getTrackingFlagDutyCycle());
	m_collectThread->setConfiguration(&m_config);
	m_collectThread->setServices(getContainerServices());
	m_collectThread->resume();*/
	try
	{
		startPropertiesMonitoring();
	}
	catch (acsthreadErrType::CanNotStartThreadExImpl &E)
	{
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl, __dummy, E, "DataWriterImpl::execute()");
		throw __dummy;
	}
	catch (ACSErrTypeCommon::NullPointerExImpl &E)
	{
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl, __dummy, E, "DataWriterImpl::execute()");
		throw __dummy;
	}
	ACS_LOG(LM_FULL_INFO, "DataWriterImpl::execute()", (LM_INFO, "COMPSTATE_OPERATIONAL"));
}

void DataWriterImpl::cleanUp()
{
	stopPropertiesMonitoring();

	if (m_workThread!=NULL) {
		m_workThread->suspend();
		getContainerServices()->getThreadManager()->destroy(m_workThread);
	}
	/*if (m_collectThread!=NULL) {
		m_collectThread->suspend();
		getContainerServices()->getThreadManager()->destroy(m_collectThread);
	}*/
	ACS_LOG(LM_FULL_INFO, "DataWriterImpl::cleanUp()", (LM_INFO, "THREADS_DESTROYED"));
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
	ACS_LOG(LM_FULL_INFO, "DataWriterImpl::cleanUp()", (LM_INFO, "LOG_FLUSHED"));
	// delete m_dataWrapper;
	delete m_data;
	CharacteristicComponentImpl::cleanUp();
}

void DataWriterImpl::aboutToAbort()
{
	if (m_workThread!=NULL) {
		m_workThread->suspend();
		getContainerServices()->getThreadManager()->destroy(m_workThread);
	}
	/*if (m_collectThread!=NULL) {
		m_collectThread->suspend();
		getContainerServices()->getThreadManager()->destroy(m_collectThread);
	}*/
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
	delete m_data;
}

// throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
char *DataWriterImpl::startScan(const Management::TScanSetup &prm)
{
	// CSecAreaResourceWrapper<DataWriter_private::CDataCollection> data=m_dataWrapper->Get();
	bool rec, inc;
	if (!m_data->setScanSetup(prm,rec,inc)) {
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"DataWriterImpl::startScan");
		if (rec) {
			impl.setReason("Could not start a new scan while recording");
		}
		else if (inc) {
			impl.setReason("Could not start a new scan right now");
		}
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
	ACS_LOG(LM_FULL_INFO, "DataWriterImpl::startScan()", (LM_DEBUG, "START_SCAN_ISSUED"));
	IRA::CString file;
	IRA::CString path;
	m_data->getFileName(file,path);
	return CORBA::string_dup((const char *)path);
}

// throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
void DataWriterImpl::setScanLayout(const ACS::stringSeq &layout)
{
	ACS_LOG(LM_FULL_INFO, "DataWriterImpl::setScanLayout()", (LM_INFO, "LAYOUT_IGNORED"));
}

// throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
void DataWriterImpl::stopScan()
{
	// CSecAreaResourceWrapper<DataWriter_private::CDataCollection> data=m_dataWrapper->Get();
	m_data->stopScan();
	ACS_LOG(LM_FULL_INFO, "DataWriterImpl::stopScan()", (LM_DEBUG, "STOP_SCAN_ISSUED"));
}

// throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
char *DataWriterImpl::startSubScan(const ::Management::TSubScanSetup &prm)
{
	// CSecAreaResourceWrapper<DataWriter_private::CDataCollection> data=m_dataWrapper->Get();
	bool rec, inc;
	if (!m_data->setSubScanSetup(prm,rec,inc)) {
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"DataWriterImpl::startSubScan");
		if (rec) {
			impl.setReason("Could not start a new subscan while recording");
		}
		else if (inc) {
			impl.setReason("Could not start a new subscan right now");
		}
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
	ACS_LOG(LM_FULL_INFO, "DataWriterImpl::startSubScan()", (LM_DEBUG, "START_SUBSCAN_ISSUED"));
	IRA::CString file=m_data->getFileName();
	return CORBA::string_dup((const char *)file);
}

// throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
CORBA::Boolean DataWriterImpl::isRecording()
{
	return (CORBA::Boolean)m_data->isRunning();
	
}

// throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx)
void DataWriterImpl::reset()
{
	m_data->forceReset();
}

_PROPERTY_REFERENCE_CPP(DataWriterImpl, ACS::ROstring, m_pfileName, fileName);
_PROPERTY_REFERENCE_CPP(DataWriterImpl, ACS::ROstring, m_pprojectName, projectName);
_PROPERTY_REFERENCE_CPP(DataWriterImpl, ACS::ROstring, m_pobserver, observer);

_PROPERTY_REFERENCE_CPP(DataWriterImpl, ACS::ROlong, m_pdeviceID, deviceID);
_PROPERTY_REFERENCE_CPP(DataWriterImpl, Management::ROTScanAxis, m_pscanAxis, scanAxis);
_PROPERTY_REFERENCE_CPP(DataWriterImpl, ACS::ROdouble, m_pdataX, dataX);
_PROPERTY_REFERENCE_CPP(DataWriterImpl, ACS::ROdouble, m_pdataY, dataY);
_PROPERTY_REFERENCE_CPP(DataWriterImpl, ACS::ROdoubleSeq, m_parrayDataX, arrayDataX);
_PROPERTY_REFERENCE_CPP(DataWriterImpl, ACS::ROdoubleSeq, m_parrayDataY, arrayDataY);

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(DataWriterImpl)
