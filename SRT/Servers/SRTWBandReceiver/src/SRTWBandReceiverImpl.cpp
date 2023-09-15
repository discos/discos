#include <new>
#include <baciDB.h>
#include <ComponentErrors.h>
#include <ManagmentDefinitionsS.h>
#include "SRTWBandReceiverImpl.h"
#include <LogFilter.h>

#include <math.h>

#define STOW_ACTION 0
#define UNSTOW_ACTION 1

#define GET_PROPERTY_REFERENCE(TYPE,PROPERTY,PROPERTYNAME) TYPE##_ptr SRTWBandReceiverImpl::PROPERTYNAME() throw (CORBA::SystemException) \
{ \
	if (PROPERTY==0) return TYPE::_nil(); \
	TYPE##_var tmp=TYPE::_narrow(PROPERTY->getCORBAReference()); \
	return tmp._retn(); \
}

using namespace ComponentErrors;
using namespace IRA;

_IRA_LOGFILTER_IMPORT;

_IRA_LOGFILTER_DECLARE;

SRTWBandReceiverImpl::SRTWBandReceiverImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) : 
	CharacteristicComponentImpl(CompName,containerServices)
{	
	AUTO_TRACE("SRTWBandReceiverImpl::SRTWBandReceiverImpl()");
}

SRTWBandReceiverImpl::~SRTWBandReceiverImpl()
{
	AUTO_TRACE("SRTWBandReceiverImpl::~SRTWBandReceiverImpl()");
}

void SRTWBandReceiverImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("SRTWBandReceiverImpl::initialize()");
	ACS_LOG(LM_FULL_INFO,"SRTWBandReceiverImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZING"));
	ACS_LOG(LM_FULL_INFO,"SRTWBandReceiverImpl::initialize()",(LM_INFO,"READING_CONFIGURATION"));	
	cout << "errore" << endl;
	m_compConfiguration.init(getContainerServices()); // throw ComponentErrors::CDBAccessExImpl	
	ACS_LOG(LM_FULL_INFO,"SRTWBandReceiverImpl::initialize()",(LM_INFO,"ACTIVATING_LOG_REPETITION_FILTER"));
	_IRA_LOGFILTER_ACTIVATE(m_compConfiguration.repetitionCacheTime(),m_compConfiguration.expireCacheTime());
	ACS_LOG(LM_FULL_INFO,"SRTWBandReceiverImpl::initialize()",(LM_INFO,"THREAD_CREATION"));
	m_watchDogParam.calSocket=&m_calSocket;
	m_watchDogParam.loSocket=&m_loSocket;
    m_watchDogParam.swMatrixSocket=&m_swMatrixSocket;
	CWatchDog::TThreadParameter *tmpParam=&m_watchDogParam;
	try {
		m_watchDog=getContainerServices()->getThreadManager()->create<CWatchDog,CWatchDog::TThreadParameter*>("SRTWBandReceiverSUPERVISOR",tmpParam);
	}
	catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"SRTWBandReceiverImpl::initialize()");
		throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"SRTWBandReceiverImpl::initialize()");
	}
	ACS_LOG(LM_FULL_INFO,"SRTWBandReceiverImpl::initialize()",(LM_INFO,"PROPERTY_CREATION"));
	try {		
		/*
		m_pazMode=new ROEnumImpl<ACS_ENUM_T(Antenna::TCommonModes),POA_Antenna::ROTCommonModes>(getContainerServices()->getName()+":azimuthMode",getComponent(),
				new ACUDevIO<Antenna::TCommonModes>(&m_commonData,ACUDevIO<Antenna::TCommonModes>::AZIMUTH_MODE),true);
		m_pelMode=new ROEnumImpl<ACS_ENUM_T(Antenna::TCommonModes),POA_Antenna::ROTCommonModes>(getContainerServices()->getName()+":elevationMode",getComponent(),
				new ACUDevIO<Antenna::TCommonModes>(&m_commonData,ACUDevIO<Antenna::TCommonModes>::ELEVATION_MODE),true);
		m_pelevation=new ROdouble(getContainerServices()->getName()+":elevation",getComponent(),
				new ACUDevIO<CORBA::Double>(&m_commonData,ACUDevIO<CORBA::Double>::ELEVATION),true);
		m_pazimuth=new ROdouble(getContainerServices()->getName()+":azimuth",getComponent(),
				new ACUDevIO<CORBA::Double>(&m_commonData,ACUDevIO<CORBA::Double>::AZIMUTH),true);
		m_pelevationError=new ROdouble(getContainerServices()->getName()+":elevationError",getComponent(),
				new ACUDevIO<CORBA::Double>(&m_commonData,ACUDevIO<CORBA::Double>::ELEVATION_ERROR),true);
		m_pazimuthError=new ROdouble(getContainerServices()->getName()+":azimuthError",getComponent(),
				new ACUDevIO<CORBA::Double>(&m_commonData,ACUDevIO<CORBA::Double>::AZIMUTH_ERROR),true);		
		m_ptime=new ROuLongLong(getContainerServices()->getName()+":time",getComponent(),
				new ACUDevIO<CORBA::ULongLong>(&m_commonData,ACUDevIO<CORBA::ULongLong>::TIME),true);
		m_pcontrolLineStatus=new ROEnumImpl<ACS_ENUM_T(Antenna::TStatus),POA_Antenna::ROTStatus>(getContainerServices()->getName()+":controlLineStatus",getComponent(),
				new ACUDevIO<Antenna::TStatus>(&m_commonData,ACUDevIO<Antenna::TStatus>::CONTROLLINESTATUS),true);
		m_pstatusLineStatus=new ROEnumImpl<ACS_ENUM_T(Antenna::TStatus),POA_Antenna::ROTStatus>(getContainerServices()->getName()+":statusLineStatus",getComponent(),
				new ACUDevIO<Antenna::TStatus>(&m_commonData,ACUDevIO<Antenna::TStatus>::STATUSLINESTATUS),true);
		m_pprogramTrackPositions=new ROlong(getContainerServices()->getName()+":programTrackPositions",getComponent(),
				new ACUDevIO<CORBA::Long>(&m_commonData,ACUDevIO<CORBA::Long>::PROGRAM_TRACK_POS),true);
		m_psection=new ROEnumImpl<ACS_ENUM_T(Antenna::TSections),POA_Antenna::ROTSections>(getContainerServices()->getName()+":section",getComponent(),
				new ACUDevIO<Antenna::TSections>(&m_commonData,ACUDevIO<Antenna::TSections>::SECTION),true);
		m_pelevationRate=new ROdouble(getContainerServices()->getName()+":elevationRate",getComponent(),
				new ACUDevIO<CORBA::Double>(&m_commonData,ACUDevIO<CORBA::Double>::ELEVATION_RATE),true);
		m_pazimuthRate=new ROdouble(getContainerServices()->getName()+":azimuthRate",getComponent(),
				new ACUDevIO<CORBA::Double>(&m_commonData,ACUDevIO<CORBA::Double>::AZIMUTH_RATE),true);
		m_pazimuthOffset=new ROdouble(getContainerServices()->getName()+":azimuthOffset",getComponent(),
				new ACUDevIO<CORBA::Double>(&m_commonData,ACUDevIO<CORBA::Double>::AZIMUTH_OFF),true);
		m_pelevationOffset=new ROdouble(getContainerServices()->getName()+":elevationOffset",getComponent(),
				new ACUDevIO<CORBA::Double>(&m_commonData,ACUDevIO<CORBA::Double>::ELEVATION_OFF),true);
		m_pdeltaTime=new ROdouble(getContainerServices()->getName()+":deltaTime",getComponent(),
				new ACUDevIO<CORBA::Double>(&m_commonData,ACUDevIO<CORBA::Double>::DELTA_TIME),true);	  
		m_pcommandedAzimuth=new ROdouble(getContainerServices()->getName()+":commandedAzimuth",getComponent(),
				new ACUDevIO<CORBA::Double>(&m_commonData,ACUDevIO<CORBA::Double>::COMMANDED_AZ),true);	  
		m_pcommandedElevation=new ROdouble(getContainerServices()->getName()+":commandedElevation",getComponent(),
				new ACUDevIO<CORBA::Double>(&m_commonData,ACUDevIO<CORBA::Double>::COMMANDED_EL),true);	 
		m_pmountStatus=new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus>(getContainerServices()->getName()+":mountStatus",getComponent(),
				new ACUDevIO<Management::TSystemStatus>(&m_commonData,ACUDevIO<Management::TSystemStatus>::MOUNT_STATUS),true);
		m_pcommandedAzimuthRate=new ROdouble(getContainerServices()->getName()+":commandedAzimuthRate",getComponent(),
				new ACUDevIO<CORBA::Double>(&m_commonData,ACUDevIO<CORBA::Double>::COMMANDED_AZ_RATE),true);
		m_pcommandedElevationRate=new ROdouble(getContainerServices()->getName()+":commandedElevationRate",getComponent(),
				new ACUDevIO<CORBA::Double>(&m_commonData,ACUDevIO<CORBA::Double>::COMMANDED_EL_RATE),true);
		m_pcommandedAzimuthOffset=new ROdouble(getContainerServices()->getName()+":commandedAzimuthOffset",getComponent(),
				new ACUDevIO<CORBA::Double>(&m_commonData,ACUDevIO<CORBA::Double>::COMMANDED_AZ_OFFSET),true);
		m_pcommandedElevationOffset=new ROdouble(getContainerServices()->getName()+":commandedElevationOffset",getComponent(),
				new ACUDevIO<CORBA::Double>(&m_commonData,ACUDevIO<CORBA::Double>::COMMANDED_EL_OFFSET),true);
		m_pcableWrapPosition=new ROdouble(getContainerServices()->getName()+":cableWrapPosition",getComponent(),
				new ACUDevIO<double>(&m_commonData,ACUDevIO<CORBA::Double>::CABLEWRAP_POSITION),true);
		m_pcableWrapTrackingError=new ROdouble(getContainerServices()->getName()+":cableWrapTrackingError",getComponent(),
				new ACUDevIO<CORBA::Double>(&m_commonData,ACUDevIO<CORBA::Double>::CABLEWRAP_TRACKERROR),true);
		m_pcableWrapRate=new ROdouble(getContainerServices()->getName()+":cableWrapRate",getComponent(),
				new ACUDevIO<CORBA::Double>(&m_commonData,ACUDevIO<CORBA::Double>::CABLEWRAP_RATE),true);
		m_pelevationTrackingError=new ROdouble(getContainerServices()->getName()+":elevationTrackingError",getComponent(),
				new ACUDevIO<CORBA::Double>(&m_commonData,ACUDevIO<CORBA::Double>::ELEVATION_TRACKERROR),true);
		m_pazimuthTrackingError=new ROdouble(getContainerServices()->getName()+":azimuthTrackingError",getComponent(),
				new ACUDevIO<CORBA::Double>(&m_commonData,ACUDevIO<CORBA::Double>::AZIMUTH_TRACKERROR),true);
		m_pgeneralStatus=new ROpattern(getContainerServices()->getName()+":generalStatus",getComponent(),
				new ACUDevIO<ACS::pattern>(&m_commonData,ACUDevIO<ACS::pattern>::GENERAL_STATUS),true);
		m_pazimuthStatus=new ROpattern(getContainerServices()->getName()+":azimuthStatus",getComponent(),
				new ACUDevIO<ACS::pattern>(&m_commonData,ACUDevIO<ACS::pattern>::AZIMUTH_AXIS_STATUS),true);
		m_pelevationStatus=new ROpattern(getContainerServices()->getName()+":elevationStatus",getComponent(),
				new ACUDevIO<ACS::pattern>(&m_commonData,ACUDevIO<ACS::pattern>::ELEVATION_AXIS_STATUS),true);
		m_pmotorsPosition=new ROdoubleSeq(getContainerServices()->getName()+":motorsPosition",getComponent(),
				new ACUDevIO<ACS::doubleSeq>(&m_commonData,ACUDevIO<ACS::doubleSeq>::MOTORS_POSITION),true);
		m_pmotorsSpeed=new ROdoubleSeq(getContainerServices()->getName()+":motorsSpeed",getComponent(),
				new ACUDevIO<ACS::doubleSeq>(&m_commonData,ACUDevIO<ACS::doubleSeq>::MOTORS_SPEED),true);
		m_pmotorsTorque=new ROdoubleSeq(getContainerServices()->getName()+":motorsTorque",getComponent(),
				new ACUDevIO<ACS::doubleSeq>(&m_commonData,ACUDevIO<ACS::doubleSeq>::MOTORS_TORQUE),true);
		m_pmotorsUtilization=new ROdoubleSeq(getContainerServices()->getName()+":motorsUtilization",getComponent(),
				new ACUDevIO<ACS::doubleSeq>(&m_commonData,ACUDevIO<ACS::doubleSeq>::MOTORS_UTILIZATION),true);
		m_pmotorsStatus=new ROlongSeq(getContainerServices()->getName()+":motorsStatus",getComponent(),
				new ACUDevIO<ACS::longSeq>(&m_commonData,ACUDevIO<ACS::longSeq>::MOTORS_STATUS),true);
		*/
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(MemoryAllocationExImpl,dummy,"SRTWBandReceiverImpl::initialize()");
		throw dummy;
	}
	ACS_LOG(LM_FULL_INFO,"SRTWBandReceiverImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZED"));
}

void SRTWBandReceiverImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	ACS_LOG(LM_FULL_INFO,"SRTWBandReceiverImpl::execute()",(LM_INFO,"CONNECTING_CAL_SOCKET"));	
	m_calSocket.init(&m_compConfiguration); // this could throw an exception.....
	ACS_LOG(LM_FULL_INFO,"SRTWBandReceiverImpl::execute()",(LM_INFO,"CONNECTING_LO_SOCKET"));
	m_loSocket.init(&m_compConfiguration); // this could throw an exception.....
	ACS_LOG(LM_FULL_INFO,"SRTWBandReceiverImpl::execute()",(LM_INFO,"CONNECTING_LO_SOCKET"));
	m_swMatrixSocket.init(&m_compConfiguration); // this could throw an exception.....
	
	//start the threads....
	m_watchDog->setSleepTime(m_compConfiguration.watchDogThreadPeriod()*10);
	m_watchDog->resume();
		
	ACS_LOG(LM_FULL_INFO,"SRTWBandReceiverImpl::execute()",(LM_INFO,"CONTROL_THREAD_STARTED"));
	try {
		startPropertiesMonitoring();
	}
	catch (acsthreadErrType::CanNotStartThreadExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"SRTWBandReceiverImpl::execute");
		throw __dummy;
	}
	catch (ACSErrTypeCommon::NullPointerExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"SRTWBandReceiverImpl::execute");
		throw __dummy;		
	}
	ACS_LOG(LM_FULL_INFO,"SRTWBandReceiverImpl::execute()",(LM_INFO,"COMPSTATE_OPERATIONAL"));
}

void SRTWBandReceiverImpl::cleanUp()
{
	AUTO_TRACE("SRTWBandReceiverImpl::cleanUp()");
	stopPropertiesMonitoring();	
	if (m_watchDog!=NULL) {
		m_watchDog->suspend();
	}
	m_calSocket.cleanUp();
	ACS_LOG(LM_FULL_INFO,"SRTWBandReceiverImpl::cleanUp()",(LM_INFO,"CAL_SOCKET_CLOSED"));
	m_loSocket.cleanUp();
	ACS_LOG(LM_FULL_INFO,"SRTWBandReceiverImpl::cleanUp()",(LM_INFO,"LO_SOCKET_CLOSED"));
	m_swMatrixSocket.cleanUp();
	ACS_LOG(LM_FULL_INFO,"SRTWBandReceiverImpl::cleanUp()",(LM_INFO,"CAL_SOCKET_CLOSED"));
	if (m_watchDog!=NULL) {
		getContainerServices()->getThreadManager()->destroy(m_watchDog);
	}
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
	CharacteristicComponentImpl::cleanUp();	
}

void SRTWBandReceiverImpl::aboutToAbort()
{
	AUTO_TRACE("SRTWBandReceiverImpl::aboutToAbort()");
	if (m_watchDog!=NULL) {
		m_watchDog->suspend();
	}
	m_calSocket.cleanUp();
	m_loSocket.cleanUp();
	m_swMatrixSocket.cleanUp();
	if (m_watchDog!=NULL) {
		getContainerServices()->getThreadManager()->destroy(m_watchDog);
	}	
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
}

/*
GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pelevation,elevation);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pazimuth,azimuth);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pazimuthError,azimuthError);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pelevationError,elevationError);

GET_PROPERTY_REFERENCE(Antenna::ROTCommonModes,m_pazMode,azimuthMode);

GET_PROPERTY_REFERENCE(Antenna::ROTCommonModes,m_pelMode,elevationMode);

GET_PROPERTY_REFERENCE(ACS::ROuLongLong,m_ptime,time);

GET_PROPERTY_REFERENCE(Antenna::ROTStatus,m_pcontrolLineStatus,controlLineStatus);

GET_PROPERTY_REFERENCE(Antenna::ROTStatus,m_pstatusLineStatus,statusLineStatus);

GET_PROPERTY_REFERENCE(ACS::ROlong,m_pprogramTrackPositions,programTrackPositions);

GET_PROPERTY_REFERENCE(Antenna::ROTSections,m_psection,section);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pelevationRate,elevationRate);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pazimuthRate,azimuthRate);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pazimuthOffset,azimuthOffset);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pelevationOffset,elevationOffset);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pdeltaTime,deltaTime);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pcommandedAzimuth,commandedAzimuth);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pcommandedElevation,commandedElevation);

GET_PROPERTY_REFERENCE(Management::ROTSystemStatus,m_pmountStatus,mountStatus);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pcommandedAzimuthRate,commandedAzimuthRate);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pcommandedElevationRate,commandedElevationRate);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pcommandedAzimuthOffset,commandedAzimuthOffset);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pcommandedElevationOffset,commandedElevationOffset);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pcableWrapPosition,cableWrapPosition);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pcableWrapTrackingError,cableWrapTrackingError);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pcableWrapRate,cableWrapRate);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pelevationTrackingError,elevationTrackingError);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pazimuthTrackingError,azimuthTrackingError);

GET_PROPERTY_REFERENCE(ACS::ROpattern,m_pgeneralStatus,generalStatus);

GET_PROPERTY_REFERENCE(ACS::ROpattern,m_pazimuthStatus,azimuthStatus);

GET_PROPERTY_REFERENCE(ACS::ROpattern,m_pelevationStatus,elevationStatus);

GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq,m_pmotorsPosition,motorsPosition);

GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq,m_pmotorsSpeed,motorsSpeed);

GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq,m_pmotorsTorque,motorsTorque);

GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq,m_pmotorsUtilization,motorsUtilization);

GET_PROPERTY_REFERENCE(ACS::ROlongSeq,m_pmotorsStatus,motorsStatus);
*/


/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SRTWBandReceiverImpl)

/*___oOo___*/
