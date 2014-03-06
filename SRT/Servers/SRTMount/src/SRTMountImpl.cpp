#include <new>
#include <baciDB.h>
#include <ComponentErrors.h>
#include <ManagmentDefinitionsS.h>
#include "SRTMountImpl.h"
#include "DevIOs.h"

#include <math.h>

#define STOW_ACTION 0
#define UNSTOW_ACTION 1

#define GET_PROPERTY_REFERENCE(TYPE,PROPERTY,PROPERTYNAME) TYPE##_ptr SRTMountImpl::PROPERTYNAME() throw (CORBA::SystemException) \
{ \
	if (PROPERTY==0) return TYPE::_nil(); \
	TYPE##_var tmp=TYPE::_narrow(PROPERTY->getCORBAReference()); \
	return tmp._retn(); \
}
	
using namespace AntennaErrors;
using namespace ComponentErrors;
using namespace IRA;


/*static char *rcsId="@(#) $Id: SRTMountImpl.cpp,v 1.7 2011-06-03 18:02:49 a.orlati Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);*/

_IRA_LOGFILTER_DECLARE;

SRTMountImpl::SRTMountImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) : 
	CharacteristicComponentImpl(CompName,containerServices),
	m_commonData(true),
  	m_pelevation(this),
	m_pazimuth(this),
	m_pelevationError(this),
	m_pazimuthError(this),
	m_pazMode(this),
	m_pelMode(this),
	m_ptime(this),
	m_pcontrolLineStatus(this),
	m_pstatusLineStatus(this),
	m_pprogramTrackPositions(this),
	m_psection(this),
	m_pelevationRate(this),
	m_pazimuthRate(this),
	m_pazimuthOffset(this),
	m_pelevationOffset(this),
	m_pdeltaTime(this),
	m_pcommandedAzimuth(this),
	m_pcommandedElevation(this),
	m_pmountStatus(this),
	m_pcommandedAzimuthRate(this),
	m_pcommandedElevationRate(this),
	m_pcommandedAzimuthOffset(this),
	m_pcommandedElevationOffset(this),
	m_pcableWrapPosition(this),
	m_pcableWrapTrackingError(this),
	m_pcableWrapRate(this),
	m_pelevationTrackingError(this),
	m_pazimuthTrackingError(this),
	m_pgeneralStatus(this),
	m_pazimuthStatus(this),
	m_pelevationStatus(this),	
	m_pmotorsPosition(this),
	m_pmotorsSpeed(this),
	m_pmotorsTorque(this),
	m_pmotorsUtilization(this),
	m_pmotorsStatus(this)
{	
	AUTO_TRACE("SRTMountImpl::SRTMountImpl()");
}

SRTMountImpl::~SRTMountImpl()
{
	AUTO_TRACE("SRTMountImpl::~SRTMountImpl()");
}

void SRTMountImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("SRTMountImpl::initialize()");
	ACS_LOG(LM_FULL_INFO,"SRTMountImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZING"));
	ACS_LOG(LM_FULL_INFO,"SRTMountImpl::initialize()",(LM_INFO,"READING_CONFIGURATION"));	
	m_compConfiguration.init(getContainerServices()); // throw ComponentErrors::CDBAccessExImpl	
	ACS_LOG(LM_FULL_INFO,"SRTMountImpl::initialize()",(LM_INFO,"ACTIVATING_LOG_REPETITION_FILTER"));
	_IRA_LOGFILTER_ACTIVATE(m_compConfiguration.repetitionCacheTime(),m_compConfiguration.expireCacheTime());
	ACS_LOG(LM_FULL_INFO,"SRTMountImpl::initialize()",(LM_INFO,"THREAD_CREATION"));
	m_watchDogParam.commandSocket=&m_commandSocket;
	m_watchDogParam.statusSocket=&m_statusSocket;
	CWatchDog::TThreadParameter *tmpParam=&m_watchDogParam;
	try {
		m_watchDog=getContainerServices()->getThreadManager()->create<CWatchDog,CWatchDog::TThreadParameter*>("SRTMOUNTSUPERVISOR",tmpParam);
	}
	catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"SRTMountImpl::initialize()");
		throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"SRTMountImpl::initialize()");
	}
	ACS_LOG(LM_FULL_INFO,"SRTMountImpl::initialize()",(LM_INFO,"PROPERTY_CREATION"));
	try {		
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
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(MemoryAllocationExImpl,dummy,"SRTMountImpl::initialize()");
		throw dummy;
	}
	ACS_LOG(LM_FULL_INFO,"SRTMountImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZED"));
}

void SRTMountImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	ACS_LOG(LM_FULL_INFO,"SRTMountImpl::execute()",(LM_INFO,"CONNECTING_STATUS_SOCKET"));	
	m_statusSocket.init(&m_compConfiguration,&m_commonData); // this could throw an exception.....
	ACS_LOG(LM_FULL_INFO,"SRTMountImpl::execute()",(LM_INFO,"CONNECTING_CONTROL_SOCKET"));
	m_commandSocket.init(&m_compConfiguration,&m_commonData); // this could throw an exception.....
	
	//start the threads....
	m_watchDog->setSleepTime(m_compConfiguration.watchDogThreadPeriod()*10);
	m_watchDog->resume();
		
	ACS_LOG(LM_FULL_INFO,"SRTMountImpl::execute()",(LM_INFO,"CONTROL_THREAD_STARTED"));
	try {
		startPropertiesMonitoring();
	}
	catch (acsthreadErrType::CanNotStartThreadExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"SRTMountImpl::execute");
		throw __dummy;
	}
	catch (ACSErrTypeCommon::NullPointerExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"SRTMountImpl::execute");
		throw __dummy;		
	}
	ACS_LOG(LM_FULL_INFO,"SRTMountImpl::execute()",(LM_INFO,"COMPSTATE_OPERATIONAL"));
}

void SRTMountImpl::cleanUp()
{
	AUTO_TRACE("SRTMountImpl::cleanUp()");
	stopPropertiesMonitoring();	
	if (m_watchDog!=NULL) {
		m_watchDog->suspend();
	}
	m_commandSocket.cleanUp();
	ACS_LOG(LM_FULL_INFO,"SRTMountImpl::cleanUp()",(LM_INFO,"CONTROL_SOCKET_CLOSED"));
	m_statusSocket.cleanUp();
	ACS_LOG(LM_FULL_INFO,"SRTMountImpl::cleanUp()",(LM_INFO,"STATUS_SOCKET_CLOSED"));
	if (m_watchDog!=NULL) {
		getContainerServices()->getThreadManager()->destroy(m_watchDog);
	}
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
	CharacteristicComponentImpl::cleanUp();	
}

void SRTMountImpl::aboutToAbort()
{
	AUTO_TRACE("SRTMountImpl::aboutToAbort()");
	if (m_watchDog!=NULL) {
		m_watchDog->suspend();
	}
	m_commandSocket.cleanUp();
	m_statusSocket.cleanUp();
	if (m_watchDog!=NULL) {
		getContainerServices()->getThreadManager()->destroy(m_watchDog);
	}	
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
}

ActionRequest SRTMountImpl::invokeAction(int function,BACIComponent* cob,const int& callbackID,const CBDescIn& descIn,
  BACIValue *value,Completion& completion,CBDescOut& descOut)
{
	switch (function) {
    	case UNSTOW_ACTION : {
    		return unstowAction(cob,callbackID,descIn,value,completion,descOut);
    	}
		case STOW_ACTION : {
			return stowAction(cob,callbackID,descIn,value,completion,descOut);
		}
    	default : {
    		return reqDestroy;
    	}
	}
}

ActionRequest SRTMountImpl::unstowAction(BACIComponent* cob,const int& callbackID,const CBDescIn& descIn,BACIValue* value_p,
  Completion& completion,CBDescOut& descOut)
{
	bool timeout,stopped,unstowed;
	ACS::TimeInterval remainingTime;
	ACS::TimeInterval sleepTime;
	sleepTime=descIn.normal_timeout;
	if ((sleepTime>0) && (sleepTime<10000000)) {
		long micro=(sleepTime-1)/10;
		IRA::CIRATools::Wait(0,micro);
	}
	// copy of the TAG
	descOut.id_tag=descIn.id_tag;
	ACS::Time startTime=value_p->uLongLongValue();
	remainingTime=m_commandSocket.checkIsUnstowed(startTime,timeout,stopped,unstowed);
	if (unstowed) {
		completion=ComponentErrors::NoErrorCompletion();
		return reqInvokeDone;
	}
	else if (stopped) {
		_COMPL(AntennaErrors::StoppedByUserCompletion,dummy,"SRTMountImpl::unstowAction()");
		completion=dummy;
		return reqInvokeDone;
	}
	else if (timeout) {
		_COMPL(ComponentErrors::TimeoutCompletion,dummy,"SRTMountImpl::unstowAction()");
		completion=dummy;
		return reqInvokeDone;
	}
	else { // now events...continue with the procedure
		descOut.estimated_timeout=remainingTime;
		completion=ComponentErrors::NoErrorCompletion();
		return reqInvokeWorking;	
	}		
}

ActionRequest SRTMountImpl::stowAction(BACIComponent* cob,const int& callbackID,const CBDescIn& descIn,BACIValue* value_p,Completion& completion,CBDescOut& descOut)
{	        	
	bool timeout,stopped,stowed;
	ACS::TimeInterval remainingTime;
	ACS::TimeInterval sleepTime;
	sleepTime=descIn.normal_timeout;
	if ((sleepTime>0) && (sleepTime<10000000)) {
		long micro=(sleepTime-1)/10;
		IRA::CIRATools::Wait(0,micro);
	}
	// copy of the TAG
	descOut.id_tag=descIn.id_tag;
	ACS::Time startTime=value_p->uLongLongValue();
	remainingTime=m_commandSocket.checkIsStowed(startTime,timeout,stopped,stowed);
	if (stowed) {
		try {
			m_commandSocket.deactivate();
		}
		catch (...) {
		}
		completion=ComponentErrors::NoErrorCompletion();
		return reqInvokeDone;
	}
	else if (stopped) {
		_COMPL(AntennaErrors::StoppedByUserCompletion,dummy,"SRTMountImpl::stowAction()");
		completion=dummy;
		return reqInvokeDone;
	}
	else if (timeout) {
		_COMPL(ComponentErrors::TimeoutCompletion,dummy,"SRTMountImpl::stowAction()");
		completion=dummy;
		return reqInvokeDone;
	}
	else { // now events...continue with the procedure
		descOut.estimated_timeout=remainingTime;
		completion=ComponentErrors::NoErrorCompletion();
		return reqInvokeWorking;	
	}	
}

void SRTMountImpl::stop() throw (CORBA::SystemException,ComponentErrorsEx,AntennaErrorsEx)
{
	try {
		m_commandSocket.stop();
	}
	catch (ComponentErrors::ComponentErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();		
	}
	catch (AntennaErrors::AntennaErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getAntennaErrorsEx();		
	}
}

void SRTMountImpl::unstow(ACS::CBvoid_ptr cb,const ACS::CBDescIn &desc) throw (CORBA::SystemException)
{
	ACS::CBDescOut dout;
	TIMEVALUE now;
	if (m_commandSocket.checkIsUnstowed()) {
		Completion completion=ComponentErrors::NoErrorCompletion();
		dout.id_tag=desc.id_tag;
		dout.estimated_timeout=0;
		cb->done(completion,dout);
		return;		
	}
	try {
		m_commandSocket.unstow(); // send the stow command to the ACU
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		ex.log(LM_DEBUG);
		dout.id_tag=desc.id_tag;
		dout.estimated_timeout=0;
		_COMPL_FROM_EXCPT(OperationErrorCompletion,dummy,ex,"SRTMountImpl::unstow()"); // create the completion from the exception
		dummy.setReason("Unstow command could not be issued");
		try {
			// the done method required a Completion but the CompletionImpl class inherits from Completion so this call works perfectly 
			cb->done(dummy,dout); // immediately inform the caller that an error occured 
		}
		catch (...) {
		}
		return;
	}
	//register the action!
	IRA::CIRATools::getTime(now);
	getComponent()->registerAction(BACIValue::type_null,cb,desc,this,UNSTOW_ACTION,BACIValue(now.value().value));
}

void SRTMountImpl::stow(ACS::CBvoid_ptr cb,const ACS::CBDescIn &desc) throw (CORBA::SystemException)
{
	ACS::CBDescOut dout;
	TIMEVALUE now;
	// copy the tag
	try {
		m_commandSocket.stow(); // send the stow command to the ACU
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		ex.log(LM_DEBUG);
		dout.id_tag=desc.id_tag;
		dout.estimated_timeout=0;
		_COMPL_FROM_EXCPT(OperationErrorCompletion,dummy,ex,"SRTMountImpl::stow()"); // create the completion from the exception
		dummy.setReason("Stow command could not be issued");
		try {
			// the done method required a Completion but the CompletionImpl class inherits from Completion so this call warks perfectly 
			cb->done(dummy,dout); // immediately inform the caller that an error occured 
		}
		catch (...) {
		}
		return;
	}
	//register the action!
	IRA::CIRATools::getTime(now);
	getComponent()->registerAction(BACIValue::type_null,cb,desc,this,STOW_ACTION,BACIValue(now.value().value));
}

void SRTMountImpl::preset(CORBA::Double az,CORBA::Double el) throw (CORBA::SystemException, ComponentErrorsEx, AntennaErrorsEx)
{
	try {
		m_commandSocket.preset(az,el);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();		
	}
	catch (AntennaErrors::AntennaErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getAntennaErrorsEx();		
	}
}

void SRTMountImpl::programTrack(CORBA::Double az,CORBA::Double el,ACS::Time time,CORBA::Boolean restart) 
  throw (CORBA::SystemException, ComponentErrorsEx, AntennaErrorsEx)
{
	try {
		m_commandSocket.programTrack(az,el,time,restart);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();
	}
	catch (AntennaErrors::AntennaErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getAntennaErrorsEx();
	}
}

void SRTMountImpl::rates(CORBA::Double azRate,CORBA::Double elRate) throw (CORBA::SystemException,ComponentErrorsEx,AntennaErrorsEx)
{
	try {
		m_commandSocket.rates(azRate,elRate);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();		
	}
	catch (AntennaErrors::AntennaErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getAntennaErrorsEx();		
	}
}

void SRTMountImpl::reset() throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, AntennaErrors::AntennaErrorsEx)
{
	try {
		m_commandSocket.resetErrors();
	}
	catch (ComponentErrors::ComponentErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();		
	}
	catch (AntennaErrors::AntennaErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getAntennaErrorsEx();		
	}
}

void SRTMountImpl::activate() throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, AntennaErrors::AntennaErrorsEx)
{
	try {
		m_commandSocket.activate();
	}
	catch (ComponentErrors::ComponentErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();
	}
	catch (AntennaErrors::AntennaErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getAntennaErrorsEx();
	}
}

void SRTMountImpl::deactivate() throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, AntennaErrors::AntennaErrorsEx)
{
	try {
		m_commandSocket.deactivate();
	}
	catch (ComponentErrors::ComponentErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();
	}
	catch (AntennaErrors::AntennaErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getAntennaErrorsEx();
	}
}

void SRTMountImpl::changeMode(Antenna::TCommonModes azMode,Antenna::TCommonModes elMode) throw (CORBA::SystemException,ComponentErrorsEx,AntennaErrorsEx)
{
	try {
		m_commandSocket.changeMode(azMode,elMode);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();		
	}
	catch (AntennaErrors::AntennaErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getAntennaErrorsEx();		
	}
}

void SRTMountImpl::setTime(ACS::Time now) throw (CORBA::SystemException, ComponentErrorsEx, AntennaErrorsEx)
{
	try {
		m_commandSocket.setTime(now);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();		
	}
	catch (AntennaErrors::AntennaErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getAntennaErrorsEx();		
	}
}

void SRTMountImpl::getEncoderCoordinates(ACS::Time_out time,CORBA::Double_out azimuth,CORBA::Double_out elevation,CORBA::Double_out azOffset,CORBA::Double_out elOffset,
		Antenna::TSections_out section) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, AntennaErrorsEx)
{
	double azOff,elOff;
	double az,el;
	CSecAreaResourceWrapper<CCommonData> data=m_commonData.Get();
	data->getEncodersCoordinates(az,el,azOff,elOff,time,section);
	azimuth=(CORBA::Double)az;
	elevation=(CORBA::Double)el;
	azOffset=(CORBA::Double)azOff;
	elOffset=(CORBA::Double)elOff;
 }

void SRTMountImpl::getAntennaErrors(ACS::Time_out time,CORBA::Double_out azError,CORBA::Double_out elError) throw (
		CORBA::SystemException, ComponentErrors::ComponentErrorsEx,AntennaErrorsEx)
{
	double azErr,elErr;
	CSecAreaResourceWrapper<CCommonData> data=m_commonData.Get();
	data->getAntennaErrors(azErr,elErr,time);
	azError=(CORBA::Double)azErr;
	elError=(CORBA::Double)elErr;
}

void SRTMountImpl::forceSection(Antenna::TSections section) throw(CORBA::SystemException)
{
	m_commandSocket.forceSector(section);
}

CORBA::Double SRTMountImpl::getHWAzimuth(CORBA::Double destination,Antenna::TSections section) throw(CORBA::SystemException)
{
	CSecAreaResourceWrapper<CCommonData> data=m_commonData.Get();
	return data->getHWAzimuth(destination,section,m_compConfiguration.azimuthLowerLimit(),m_compConfiguration.azimuthUpperLimit(),m_compConfiguration.cwLimit());
}

void SRTMountImpl::setOffsets(CORBA::Double azOff,CORBA::Double elOff) throw (CORBA::SystemException, ComponentErrorsEx, AntennaErrorsEx)
{
	try {
		m_commandSocket.setOffsets(azOff,elOff);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();		
	}
	catch (AntennaErrors::AntennaErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getAntennaErrorsEx();		
	}
}
		
void SRTMountImpl::setDeltaTime(CORBA::Double delta) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, AntennaErrorsEx)
{
	try {
		m_commandSocket.setTimeOffset(delta);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();		
	}
	catch (AntennaErrors::AntennaErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getAntennaErrorsEx();		
	}	
}

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


/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SRTMountImpl)

/*___oOo___*/
