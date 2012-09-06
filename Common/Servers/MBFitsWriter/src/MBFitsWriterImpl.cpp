#include "MBFitsWriterImpl.h"

#include "DevIOFileName.h"
#include "DevIOStatus.h"

#include <LogFilter.h>
#include <ObservatoryC.h>

#include <baciBACIComponent.h>

using baci::BACIComponent;

#define _GET_PROPERTY_VALUE_ONCE(outVar_, propertyReference_, propertyName_, routine_) \
try { \
	ACSErr::Completion_var cmpl; \
	outVar_ = propertyReference_->get_sync(cmpl.out()); \
	CompletionImpl cmplImpl(cmpl.in()); \
	if ( !cmplImpl.isErrorFree() ) { \
		_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl, impl, cmplImpl, routine_); \
		impl.setAttributeName(propertyName_); \
		throw impl; \
	} \
} catch( ... ) { \
	_EXCPT(ComponentErrors::UnexpectedExImpl, impl, routine_); \
	throw impl; \
} \

/*	// Not implemented
MBFitsWriterImpl::MBFitsWriterImpl() : BulkDataReceiverImpl<FitsWriter_private::ReceiverCallback>(),
																			 m_fileName_p(this),
																			 m_projectName_p(this),
																			 m_observer_p(this),
																			 m_scanID_p(this),
																			 m_deviceID_p(this),
																			 m_scanAxis_p(this),
																			 m_dataX_p(this),
																			 m_dataY_p(this),
																			 m_arrayDataX_p(this),
																			 m_arrayDataY_p(this),
																			 m_status_p(this),
																			 m_configuration_p(NULL),
																			 m_dataWrapper_p(NULL),
																			 m_workThread_p(NULL),
																			 m_collectThread_p(NULL) {
}
*/

/*	// Not implemented
MBFitsWriterImpl::MBFitsWriterImpl( const MBFitsWriterImpl& mbFitsWriter_ ) : BulkDataReceiverImpl<FitsWriter_private::ReceiverCallback>(mbFitsWriter_),
																																							m_fileName_p(mbFitsWriter_.m_fileName_p),
																																							m_projectName_p(mbFitsWriter_.m_projectName_p),
																																							m_observer_p(mbFitsWriter_.m_observer_p),
																																							m_scanID_p(mbFitsWriter_.m_scanID_p),
																																							m_deviceID_p(mbFitsWriter_.m_deviceID_p),
																																							m_scanAxis_p(mbFitsWriter_.m_scanAxis_p),
																																							m_dataX_p(mbFitsWriter_.m_dataX_p),
																																							m_dataY_p(mbFitsWriter_.m_dataY_p),
																																							m_arrayDataX_p(mbFitsWriter_.m_arrayDataX_p),
																																							m_arrayDataY_p(mbFitsWriter_.m_arrayDataY_p),
																																							m_status_p(mbFitsWriter_.m_status_p),
																																							m_configuration_p(NULL),
																																							m_dataWrapper_p(NULL),
																																							m_workThread_p(NULL),
																																							m_collectThread_p(NULL) {
}
*/

MBFitsWriterImpl::MBFitsWriterImpl( const ACE_CString& name_,
																		ContainerServices* containerServices_p_ ) : BulkDataReceiverImpl<FitsWriter_private::ReceiverCallback>(name_, containerServices_p_),
																																								m_fileName_p(this),
																																								m_projectName_p(this),
																																								m_observer_p(this),
																																								m_scanID_p(this),
																																								m_deviceID_p(this),
																																								m_scanAxis_p(this),
																																								m_dataX_p(this),
																																								m_dataY_p(this),
																																								m_arrayDataX_p(this),
																																								m_arrayDataY_p(this),
																																								m_status_p(this),
																																								m_configuration_p(NULL),
																																								m_dataWrapper_p(NULL),
																																								m_workThread_p(NULL),
																																								m_collectThread_p(NULL) {

}

MBFitsWriterImpl::~MBFitsWriterImpl() {
}

/*	// Not implemented
bool MBFitsWriterImpl::operator==( const MBFitsWriterImpl& mbFitsWriter_ ) const {
	return ( BulkDataReceiverImpl<FitsWriter_private::ReceiverCallback>::operator==(mbFitsWriter_) &&
					 ((m_fileName_p				== mbFitsWriter_.m_fileName_p				) &&
						(m_projectName_p		== mbFitsWriter_.m_projectName_p		) &&
						(m_observer_p				== mbFitsWriter_.m_observer_p				) &&
						(m_scanID_p					== mbFitsWriter_.m_scanID_p					) &&
						(m_deviceID_p				== mbFitsWriter_.m_deviceID_p				) &&
						(m_scanAxis_p				== mbFitsWriter_.m_scanAxis_p				) &&

						(m_dataX_p					== mbFitsWriter_.m_dataX_p					) &&
						(m_dataY_p					== mbFitsWriter_.m_dataY_p					) &&
						(m_arrayDataX_p			== mbFitsWriter_.m_arrayDataX_p			) &&
						(m_arrayDataY_p			== mbFitsWriter_.m_arrayDataY_p			) &&

						(m_status_p					== mbFitsWriter_.m_status_p					)	&&

						((!m_configuration_p	&& !mbFitsWriter_.m_configuration_p) ||
						 ( m_configuration_p	&&  mbFitsWriter_.m_configuration_p && (*m_configuration_p	== *mbFitsWriter_.m_configuration_p))) &&

						((!m_dataWrapper_p		&& !mbFitsWriter_.m_dataWrapper_p	 ) ||
						 ( m_dataWrapper_p		&&  mbFitsWriter_.m_dataWrapper_p		&& (*m_dataWrapper_p		== *mbFitsWriter_.m_dataWrapper_p	 ))) &&

						((!m_workThread_p			&& !mbFitsWriter_.m_workThread_p	 ) ||
						 ( m_workThread_p			&&  mbFitsWriter_.m_workThread_p		&& (*m_workThread_p			== *mbFitsWriter_.m_workThread_p	 ))) &&

						((!m_collectThread_p	&& !mbFitsWriter_.m_collectThread_p) ||
						 ( m_collectThread_p	&&  mbFitsWriter_.m_collectThread_p && (*m_collectThread_p	== *mbFitsWriter_.m_collectThread_p))) ) );
}
*/

/*	// Not implemented
bool MBFitsWriterImpl::operator!=( const MBFitsWriterImpl& mbFitsWriter_ ) const {
	return !(*this == mbFitsWriter_);
}
*/

/*	// Not implemented
MBFitsWriterImpl& MBFitsWriterImpl::operator=( const MBFitsWriterImpl& mbFitsWriter_ ) {
	if ( *this != mbFitsWriter ) {	// handle self assignment
		BulkDataReceiverImpl<FitsWriter_private::ReceiverCallback>::operator=(mbFitsWriter_);

		m_fileName_p				= mbFitsWriter_.m_fileName_p;
		m_projectName_p			= mbFitsWriter_.m_projectName_p;
		m_observer_p				= mbFitsWriter_.m_observer_p;
		m_scanID_p					= mbFitsWriter_.m_scanID_p;
		m_deviceID_p				= mbFitsWriter_.m_deviceID_p;
		m_scanAxis_p				= mbFitsWriter_.m_scanAxis_p;

		m_dataX_p						= mbFitsWriter_.m_dataX_p;
		m_dataY_p						= mbFitsWriter_.m_dataY_p;
		m_arrayDataX_p			= mbFitsWriter_.m_arrayDataX_p;
		m_arrayDataY_p			= mbFitsWriter_.m_arrayDataY_p;

		m_status_p					= mbFitsWriter_.m_status_p;
	}

	return *this;
}
*/

void MBFitsWriterImpl::initialize() throw (ACSErr::ACSbaseExImpl) {
	static const string workThreadName("MBFitsEngine");
	static const string collectorThreadName("CollectorThread");

	AUTO_TRACE("MBFitsWriterImpl::initialize()");

	ACS_LOG(LM_FULL_INFO, "MBFitsWriterImpl::initialize()", (LM_INFO, "COMPSTATE_INITIALIZING"));

	CMBFitsWriter::initialize();
	Scan::initialize();

	try {
		m_configuration_p = new FitsWriter_private::CConfiguration();
		m_configuration_p->init(getContainerServices());    // throw CDBAcessExImpl;
	} catch( ACSErr::ACSbaseExImpl& exception_ ) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl, _dummy, exception_, "MBFitsWriterImpl::initialize()");
		throw _dummy;
	}

	FitsWriter_private::CDataCollection* data_p = NULL;

	try {
		data_p					= new FitsWriter_private::CDataCollection();
		m_dataWrapper_p	= new CSecureArea<FitsWriter_private::CDataCollection>(data_p);

		const ACE_CString	containerServicesName(getContainerServices()->getName());
		BACIComponent*		component_p = getComponent();

		m_fileName_p		= new ROstring(containerServicesName		+ ":fileName",				component_p, new FitsWriter_private::DevIOFileName(m_dataWrapper_p), true);
		m_projectName_p	= new ROstring(containerServicesName		+ ":projectName",			component_p);
		m_observer_p		= new ROstring(containerServicesName		+ ":observer",				component_p);
		m_scanID_p			= new ROlong(containerServicesName			+ ":scanIdentifier",	component_p);
		m_deviceID_p		= new ROlong(containerServicesName			+ ":deviceID",				component_p);
		m_scanAxis_p		= new ROEnumImpl<ACS_ENUM_T(Management::TScanAxis), POA_Management::ROTScanAxis>(containerServicesName					+ ":scanAxis",	component_p);

		m_dataX_p				= new ROdouble(containerServicesName		+ ":dataX",						component_p);
		m_dataY_p				= new ROdouble(containerServicesName		+ ":dataY",						component_p);
		m_arrayDataX_p	= new ROdoubleSeq(containerServicesName	+ ":arrayDataX",			component_p);
		m_arrayDataY_p	= new ROdoubleSeq(containerServicesName	+ ":arrayDataY",			component_p);

		m_status_p			= new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus), POA_Management::ROTSystemStatus>(containerServicesName	+ ":status",		component_p,
																																																						 new FitsWriter_private::DevIOStatus(m_dataWrapper_p), true);
	} catch( std::bad_alloc& exception_ ) {
		if ( data_p ) { delete data_p; data_p = NULL; }

		_EXCPT(ComponentErrors::MemoryAllocationExImpl, _dummy, "MBFitsWriterImpl::initialize()");
		throw _dummy;
	}

	FitsWriter_private::ReceiverCallback::m_dataCollection = m_dataWrapper_p;

	try {
		ACS::ThreadManager* const threadManager_p = getContainerServices()->getThreadManager();

		m_workThread_p		= dynamic_cast<MBFitsWriter_private::EngineThread *>(threadManager_p->create<MBFitsWriter_private::EngineThread, CSecureArea<FitsWriter_private::CDataCollection> *>(workThreadName.c_str(), m_dataWrapper_p));
		m_collectThread_p	= dynamic_cast<MBFitsWriter_private::CCollectorThread *>(threadManager_p->create<MBFitsWriter_private::CCollectorThread, CSecureArea<FitsWriter_private::CDataCollection> *>(collectorThreadName.c_str(), m_dataWrapper_p));
		m_workThread_p->setCollectorThread(m_collectThread_p);
	} catch( acsthreadErrType::acsthreadErrTypeExImpl& exception_ ) {
		if ( data_p ) { delete data_p; data_p = NULL; }

		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl, __dummy, exception_, "MBFitsWriterImpl::initialize()");
		throw __dummy;
	} catch( ... ) {
		if ( data_p ) { delete data_p; data_p = NULL; }

		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl, "MBFitsWriterImpl::initialize()");
	}

	ACS_LOG(LM_FULL_INFO, "MBFitsWriterImpl::initialize()", (LM_INFO, "THREAD_CREATED"));

	ACS_LOG(LM_FULL_INFO, "MBFitsWriterImpl::initialize()", (LM_INFO, "COMPSTATE_INITIALIZED"));
}

void MBFitsWriterImpl::execute() throw (ACSErr::ACSbaseExImpl) {
//	_IRA_LOGFILTER_ACTIVATE(m_configuration_p->getRepetitionCacheTime(), m_configuration_p->getRepetitionExpireTime());

	Antenna::Observatory_var observatory = Antenna::Observatory::_nil();

	try {
		observatory = getContainerServices()->getDefaultComponent<Antenna::Observatory>(static_cast<const char*>(m_configuration_p->getObservatoryComponent()));
	} catch( maciErrType::CannotGetComponentExImpl& exception_ ) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl, Impl, exception_, "MBFitsWriterImpl::execute()");
		Impl.setComponentName(static_cast<const char*>(m_configuration_p->getObservatoryComponent()));
		throw Impl;
	} catch( maciErrType::NoPermissionExImpl& exception_ ) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl, Impl, exception_, "MBFitsWriterImpl::execute()");
		Impl.setComponentName(static_cast<const char*>(m_configuration_p->getObservatoryComponent()));
		throw Impl;
	} catch( maciErrType::NoDefaultComponentExImpl& exception_ ) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl, Impl, exception_, "MBFitsWriterImpl::execute()");
		Impl.setComponentName(static_cast<const char*>(m_configuration_p->getObservatoryComponent()));
		throw Impl;
	}

	ACS_LOG(LM_FULL_INFO, "MBFitsWriterImpl::execute()", (LM_INFO, "OBSERVATORY_LOCATED"));

	try	{
		Antenna::TSiteInformation_var site;
		ACS::ROstring_var						 observatoryName;
		CORBA::String_var						 siteName;

		site						= observatory->getSiteSummary();		// throw CORBA::SYSTEMEXCEPTION
		observatoryName	= observatory->observatoryName();

		// this macro could throw ACSBase exception....
		_GET_PROPERTY_VALUE_ONCE(siteName, observatoryName, "observatoryName", "MBFitsWriterImpl::execute()");

		const IRA::CSite siteInfo	= CSite(site.out());
		const double		 dut1			= site->DUT1;

		CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data_p = m_dataWrapper_p->Get();
		data_p->setSite(siteInfo, dut1, static_cast<const char *>(siteName));
	} catch( CORBA::SystemException& exception_ )	{
		_EXCPT(ComponentErrors::CORBAProblemExImpl, __dummy, "MBFitsWriterImpl::execute()");
		__dummy.setName(exception_._name());
		__dummy.setMinor(exception_.minor());
		throw __dummy;
	} catch( ... ) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl, "MBFitsWriterImpl::initialize()");
	}

	ACS_LOG(LM_FULL_INFO,"MBFitsWriterImpl::execute()",(LM_INFO,"SITE_INITIALIZED"));

	try {
		getContainerServices()->releaseComponent(static_cast<const char*>(observatory->name()));
	} catch( maciErrType::CannotReleaseComponentExImpl& exception_ ) {
		_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl, Impl, exception_, "MBFitsWriterImpl::execute()");
		Impl.setComponentName(static_cast<const char*>(observatory->name()));
		throw Impl;
	}

	m_workThread_p->setSleepTime(m_configuration_p->getWorkingThreadTime());
	m_workThread_p->setTimeSlice(m_configuration_p->getWorkingThreadTimeSlice());
	m_workThread_p->setConfiguration(m_configuration_p);
	m_workThread_p->setServices(getContainerServices());
	m_workThread_p->resume();

	m_collectThread_p->setSleepTime(m_configuration_p->getCollectorThreadTime());
	m_collectThread_p->setMeteoParamDuty(m_configuration_p->getMeteoParameterDutyCycle());
	m_collectThread_p->setTrackingFlagDuty(m_configuration_p->getTrackingFlagDutyCycle());
	m_collectThread_p->setConfiguration(m_configuration_p);
	m_collectThread_p->setServices(getContainerServices());
	m_collectThread_p->resume();
}

void MBFitsWriterImpl::cleanUp() {
	stopPropertiesMonitoring();

	ACS::ThreadManager* const threadManager_p = getContainerServices()->getThreadManager();

	if ( m_workThread_p ) {
		m_workThread_p->suspend();
		threadManager_p->destroy(m_workThread_p);
		m_workThread_p = NULL;
	}

	if ( m_collectThread_p ) {
		m_collectThread_p->suspend();
		threadManager_p->destroy(m_collectThread_p);
		m_collectThread_p = NULL;
	}

	ACS_LOG(LM_FULL_INFO, "MBFitsWriterImpl::cleanUp()", (LM_INFO, "THREADS_DESTROYED"));

	Scan::terminate();
	CMBFitsWriter::terminate();

//	_IRA_LOGFILTER_FLUSH;
//	_IRA_LOGFILTER_DESTROY;

	ACS_LOG(LM_FULL_INFO, "MBFitsWriterImpl::cleanUp()", (LM_INFO, "LOG_FLUSHED"));

	if ( m_dataWrapper_p ) { delete m_dataWrapper_p; m_dataWrapper_p = NULL; }

	CharacteristicComponentImpl::cleanUp();
}

void MBFitsWriterImpl::aboutToAbort() {
	stopPropertiesMonitoring();

	ACS::ThreadManager* const threadManager_p = getContainerServices()->getThreadManager();

	if ( m_workThread_p ) {
		m_workThread_p->suspend();
		threadManager_p->destroy(m_workThread_p);
		m_workThread_p = NULL;
	}

	if ( m_collectThread_p ) {
		m_collectThread_p->suspend();
		threadManager_p->destroy(m_collectThread_p);
		m_collectThread_p = NULL;
	}

	ACS_LOG(LM_FULL_INFO, "MBFitsWriterImpl::aboutToAbort()", (LM_INFO, "THREADS_DESTROYED"));

	CMBFitsWriter::terminate();

//	_IRA_LOGFILTER_FLUSH;
//	_IRA_LOGFILTER_DESTROY;

	ACS_LOG(LM_FULL_INFO, "MBFitsWriterImpl::aboutToAbort()", (LM_INFO, "LOG_FLUSHED"));

	if ( m_dataWrapper_p ) { delete m_dataWrapper_p; m_dataWrapper_p = NULL; }

	CharacteristicComponentImpl::aboutToAbort();
}

void MBFitsWriterImpl::startScan( const Management::TScanSetup& scanSetup_ ) {
  CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();

  bool recording = false;
  bool inconsistent = false;

	if ( !data_p->setScanSetup(scanSetup_, recording, inconsistent) ) {
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"MBFitsWriterImpl::startScan");

		if ( recording ) {
			impl.setReason("Could not start a new scan while recording");
		} else if ( inconsistent ) {
			impl.setReason("Could not start a new scan right now");
		}
		impl.log(LM_DEBUG);

		throw impl.getComponentErrorsEx();
	}

ACS_LOG(LM_FULL_INFO,"MBFitsWriterImpl::startScan()",(LM_DEBUG,"START_SCAN_ISSUED"));
}

void MBFitsWriterImpl::setScanLayout( const ACS::stringSeq& scanLayout_ ) {
	ACS_LOG(LM_FULL_INFO,"MBFitsWriterImpl::setScanLayout()",(LM_DEBUG,"SET_SCAN_LAYOUT_ISSUED"));

	LayoutCollection layoutCollection;
	layoutCollection.parse(scanLayout_);
	m_workThread_p->setLayoutCollection(layoutCollection);
}

void MBFitsWriterImpl::stopScan() {
ACS_LOG(LM_FULL_INFO,"MBFitsWriterImpl::stopScan()",(LM_DEBUG,"STOP_SCAN_ISSUED"));
  CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();
  data_p->scanStop();
}
void MBFitsWriterImpl::startSubScan( const Management::TSubScanSetup& subscanSetup_ ) {
  CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();

  bool recording = false;
  bool inconsistent = false;

ACS_LOG(LM_FULL_INFO, "MBFitsWriterImpl::startSubScan()", (LM_DEBUG, "MBFitsWriterImpl::startSubScan"));
	if ( !data_p->setSubScanSetup(subscanSetup_, recording, inconsistent) ) {
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"MBFitsWriterImpl::startSubScan");

		if ( recording ) {
			impl.setReason("Could not start a new sub scan while recording");
		} else if ( inconsistent ) {
			impl.setReason("Could not start a new sub scan right now");
		}
		impl.log(LM_DEBUG);

		throw impl.getComponentErrorsEx();
	}

ACS_LOG(LM_FULL_INFO,"MBFitsWriterImpl::startSubScan()",(LM_DEBUG,"START_SUBSCAN_ISSUED"));
}
CORBA::Boolean MBFitsWriterImpl::isRecording() {
//ACS_LOG(LM_FULL_INFO,"MBFitsWriterImpl::isRecording()",(LM_DEBUG,"IS_RECORDING_ISSUED"));
  CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();
	return (CORBA::Boolean)data_p->isRunning();
}
void MBFitsWriterImpl::reset() {
	CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();
	data_p->forceReset();
ACS_LOG(LM_FULL_INFO,"MBFitsWriterImpl::reset()",(LM_DEBUG,"RESET_ISSUED"));
}

_PROPERTY_REFERENCE_CPP(MBFitsWriterImpl, ACS::ROstring,								m_fileName_p,			fileName			);
_PROPERTY_REFERENCE_CPP(MBFitsWriterImpl, ACS::ROstring,								m_projectName_p,	projectName		);
_PROPERTY_REFERENCE_CPP(MBFitsWriterImpl, ACS::ROstring,								m_observer_p,			observer			);
_PROPERTY_REFERENCE_CPP(MBFitsWriterImpl, ACS::ROlong,									m_scanID_p,				scanIdentifier);
_PROPERTY_REFERENCE_CPP(MBFitsWriterImpl, ACS::ROlong,									m_deviceID_p,			deviceID			);
_PROPERTY_REFERENCE_CPP(MBFitsWriterImpl, Management::ROTScanAxis,			m_scanAxis_p,			scanAxis			);

_PROPERTY_REFERENCE_CPP(MBFitsWriterImpl, ACS::ROdouble,								m_dataX_p,				dataX					);
_PROPERTY_REFERENCE_CPP(MBFitsWriterImpl, ACS::ROdouble,								m_dataY_p,				dataY					);
_PROPERTY_REFERENCE_CPP(MBFitsWriterImpl, ACS::ROdoubleSeq,							m_arrayDataX_p,		arrayDataX		);
_PROPERTY_REFERENCE_CPP(MBFitsWriterImpl, ACS::ROdoubleSeq,							m_arrayDataY_p,		arrayDataY		);

_PROPERTY_REFERENCE_CPP(MBFitsWriterImpl, Management::ROTSystemStatus,	m_status_p,				status				);

/*
void MBFitsWriterImpl::setFileName( const char* fileName_ ) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx) {
	CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data_p = m_dataWrapper_p->Get();

	if ( !data_p->setFileName(IRA::CString(fileName_)) ) {
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl, "MBFitsWriterImpl::setFileName");
		impl.setReason("Could not change file name while recording");
		impl.log(LM_DEBUG);

		throw impl.getComponentErrorsEx();
	}
}

void MBFitsWriterImpl::setProjectName( const char *projectName_ ) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx) {
	CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data_p = m_dataWrapper_p->Get();
	data_p->setProjectName(projectName_);
}

void MBFitsWriterImpl::setObserverName( const char *observerName_ ) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx) {
	CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data_p = m_dataWrapper_p->Get();
	data_p->setObserverName(observerName_);
}

void MBFitsWriterImpl::setScanIdentifier( CORBA::Long scanID_ ) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx) {
	CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data_p = m_dataWrapper_p->Get();
	data_p->setScanId(scanID_);
}

void MBFitsWriterImpl::setDevice( CORBA::Long deviceID_ ) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx) {
}

void MBFitsWriterImpl::setScanAxis( Management::TScanAxis scanAxis_ ) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx) {
}
*/

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>

MACI_DLL_SUPPORT_FUNCTIONS(MBFitsWriterImpl)
