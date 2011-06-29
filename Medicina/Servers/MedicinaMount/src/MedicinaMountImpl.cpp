// $Id: MedicinaMountImpl.cpp,v 1.17 2011-04-22 17:15:07 a.orlati Exp $

#include <new>
#include <baciDB.h>
#include <ComponentErrors.h>
#include <ManagmentDefinitionsS.h>
#include "MedicinaMountImpl.h"
#include "MedicinaMountDevIOs.h"
#include <math.h>

#define GET_PROPERTY_REFERENCE(TYPE,PROPERTY,PROPERTYNAME) TYPE##_ptr MedicinaMountImpl::PROPERTYNAME() throw (CORBA::SystemException) \
{ \
	if (PROPERTY==0) return TYPE::_nil(); \
	TYPE##_var tmp=TYPE::_narrow(PROPERTY->getCORBAReference()); \
	return tmp._retn(); \
}
	
using namespace AntennaErrors;
using namespace ComponentErrors;

static char *rcsId="@(#) $Id: MedicinaMountImpl.cpp,v 1.17 2011-04-22 17:15:07 a.orlati Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

//_IRA_LOGFILTER_DECLARE;

MedicinaMountImpl::MedicinaMountImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) : 
	CharacteristicComponentImpl(CompName,containerServices),
  	m_pelevation(this),
	m_pazimuth(this),
	m_pelevationError(this),
	m_pazimuthError(this),
	m_pazMode(this),
	m_pelMode(this),
	m_ptime(this),
	m_pstatus(this),
	m_pazimuthServoStatus(this),
	m_pelevationServoStatus(this),
	m_pservoSystemStatus(this),
	m_pfreeProgramTrackPosition(this),
	m_psection(this),
	m_pelevationRate(this),
	m_pazimuthRate(this),
	m_pazimuthOffset(this),m_pelevationOffset(this),
	m_pdeltaTime(this),
	m_pcommandedAzimuth(this),
	m_pcommandedElevation(this),
	m_pmountStatus(this),
  	/*m_ACUDataSupplier(NULL),*/m_ACULink(NULL),m_pcontrolLoop(NULL)
{	
	AUTO_TRACE("MedicinaMountImpl::MedicinaMountImpl()");
}

MedicinaMountImpl::~MedicinaMountImpl()
{
	AUTO_TRACE("MedicinaMountImpl::~MedicinaMountImpl()");
}

void MedicinaMountImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("MedicinaMountImpl::initialize()");
	ACS_LOG(LM_FULL_INFO,"MedicinaMountImpl::initialize()",(LM_INFO,"MedicinaMount::COMPSTATE_INITIALIZING"));
	CMedicinaMountSocket *ACUSocket=NULL;
	ACS_LOG(LM_FULL_INFO,"MedicinaMountImpl::initialize()",(LM_INFO,"MedicinaMount::READING_CONFIGURATION"));
	// this could throw an exception.....	
	m_CompConfiguration.Init(getContainerServices());
	ACS_LOG(LM_FULL_INFO,"MedicinaMount::initialize()",(LM_INFO,"MedicinaMount::CONFIGURATION_DONE"));	
	DDWORD tm=m_CompConfiguration.repetitionCacheTime();
	try {
		ACUSocket=new CMedicinaMountSocket();
		m_ACULink=new CSecureArea<CMedicinaMountSocket>(ACUSocket);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(MemoryAllocationExImpl,dummy,"MedicinaMountImpl::initialize()");
		throw dummy;
	}
	try {		
		m_pazMode=new ROEnumImpl<ACS_ENUM_T(Antenna::TCommonModes),POA_Antenna::ROTCommonModes>(getContainerServices()->getName()+":azimuthMode",getComponent(),
		  new ACUDevIO<Antenna::TCommonModes>(m_ACULink,ACUDevIO<Antenna::TCommonModes>::AZIMUTH_MODE,tm),true);
		m_pelMode=new ROEnumImpl<ACS_ENUM_T(Antenna::TCommonModes),POA_Antenna::ROTCommonModes>(getContainerServices()->getName()+":elevationMode",getComponent(),
		  new ACUDevIO<Antenna::TCommonModes>(m_ACULink,ACUDevIO<Antenna::TCommonModes>::ELEVATION_MODE,tm),true);
		m_pelevation=new ROdouble(getContainerServices()->getName()+":elevation",getComponent(),
		  new ACUDevIO<CORBA::Double>(m_ACULink,ACUDevIO<CORBA::Double>::ELEVATION,tm),true);
		m_pazimuth=new ROdouble(getContainerServices()->getName()+":azimuth",getComponent(),
		  new ACUDevIO<CORBA::Double>(m_ACULink,ACUDevIO<CORBA::Double>::AZIMUTH,tm),true);		
		m_pelevationError=new ROdouble(getContainerServices()->getName()+":elevationError",getComponent(),
		  new ACUDevIO<CORBA::Double>(m_ACULink,ACUDevIO<CORBA::Double>::ELEVATION_ERROR,tm),true);
		m_pazimuthError=new ROdouble(getContainerServices()->getName()+":azimuthError",getComponent(),
		  new ACUDevIO<CORBA::Double>(m_ACULink,ACUDevIO<CORBA::Double>::AZIMUTH_ERROR,tm),true);		
		m_ptime=new ROuLongLong(getContainerServices()->getName()+":time",getComponent(),
		  new ACUDevIO<CORBA::ULongLong>(m_ACULink,ACUDevIO<CORBA::ULongLong>::TIME,tm),true);
		m_pstatus=new ROEnumImpl<ACS_ENUM_T(Antenna::TStatus),POA_Antenna::ROTStatus>(getContainerServices()->getName()+":status",getComponent(),
		  new ACUDevIO<Antenna::TStatus>(m_ACULink,ACUDevIO<Antenna::TStatus>::STATUS,tm),true);
		m_pazimuthServoStatus=new ROpattern(getContainerServices()->getName()+":azimuthServoStatus",getComponent(),
		  new ACUDevIO<ACS::pattern>(m_ACULink,ACUDevIO<ACS::pattern>::AZIMUTH_SERVO_STATUS,tm),true);
		m_pelevationServoStatus=new ROpattern(getContainerServices()->getName()+":elevationServoStatus",getComponent(),
		  new ACUDevIO<ACS::pattern>(m_ACULink,ACUDevIO<ACS::pattern>::ELEVATION_SERVO_STATUS,tm),true);
		m_pservoSystemStatus=new ROpattern(getContainerServices()->getName()+":servoSystemStatus",getComponent(),
		  new ACUDevIO<ACS::pattern>(m_ACULink,ACUDevIO<ACS::pattern>::SERVO_SYSTEM_STATUS,tm),true);	
		m_pfreeProgramTrackPosition=new ROlong(getContainerServices()->getName()+":freeProgramTrackPosition",getComponent(),
		  new ACUDevIO<CORBA::Long>(m_ACULink,ACUDevIO<CORBA::Long>::PROGRAM_TRACK_POS,tm),true);
		m_psection=new ROEnumImpl<ACS_ENUM_T(Antenna::TSections),POA_Antenna::ROTSections>(getContainerServices()->getName()+":section",getComponent(),
		  new ACUDevIO<Antenna::TSections>(m_ACULink,ACUDevIO<Antenna::TSections>::SECTION,tm),true);
		m_pelevationRate=new ROdouble(getContainerServices()->getName()+":elevationRate",getComponent(),
		  new ACUDevIO<CORBA::Double>(m_ACULink,ACUDevIO<CORBA::Double>::ELEVATION_RATE,tm),true);
		m_pazimuthRate=new ROdouble(getContainerServices()->getName()+":azimuthRate",getComponent(),
		  new ACUDevIO<CORBA::Double>(m_ACULink,ACUDevIO<CORBA::Double>::AZIMUTH_RATE,tm),true);
		m_pazimuthOffset=new ROdouble(getContainerServices()->getName()+":azimuthOffset",getComponent(),
		  new ACUDevIO<CORBA::Double>(m_ACULink,ACUDevIO<CORBA::Double>::AZIMUTH_OFF,tm),true);
		m_pelevationOffset=new ROdouble(getContainerServices()->getName()+":elevationOffset",getComponent(),
		  new ACUDevIO<CORBA::Double>(m_ACULink,ACUDevIO<CORBA::Double>::ELEVATION_OFF,tm),true);
		m_pdeltaTime=new ROdouble(getContainerServices()->getName()+":deltaTime",getComponent(),
		  new ACUDevIO<CORBA::Double>(m_ACULink,ACUDevIO<CORBA::Double>::DELTA_TIME,tm),true);	  
		m_pcommandedAzimuth=new ROdouble(getContainerServices()->getName()+":commandedAzimuth",getComponent(),
		  new ACUDevIO<CORBA::Double>(m_ACULink,ACUDevIO<CORBA::Double>::COMMANDED_AZ,tm),true);	  
		m_pcommandedElevation=new ROdouble(getContainerServices()->getName()+":commandedElevation",getComponent(),
		  new ACUDevIO<CORBA::Double>(m_ACULink,ACUDevIO<CORBA::Double>::COMMANDED_EL,tm),true);	 
		m_pmountStatus=new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus>(getContainerServices()->getName()+":mountStatus",getComponent(),
		  new ACUDevIO<Management::TSystemStatus>(m_ACULink,ACUDevIO<Management::TSystemStatus>::MOUNTSTATUS,tm),true);	  		  
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(MemoryAllocationExImpl,dummy,"MedicinaMountImpl::initialize()");
		throw dummy;
	}
	//ACS_LOG(LM_FULL_INFO,"MedicinaMountImpl::initialize()",(LM_INFO,"MedicinaMount::NOTIFICATION_CHANNEL_CONNECTING"));	
	//m_ACUDataSupplier=new nc::SimpleSupplier(Antenna::MOUNT_DATA_CHANNEL,this);
	//ACS_LOG(LM_FULL_INFO,"MedicinaMountImpl::initialize()",(LM_INFO,"MedicinaMount::NOTIFICATION_CHANNEL_CONNECTED"));		
	ACS_LOG(LM_FULL_INFO,"MedicinaMountImpl::initialize()",(LM_INFO,"MedicinaMount::COMPSTATE_INITIALIZED"));
}

void MedicinaMountImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	DWORD timeSlice;
	AUTO_TRACE("MedicinaMountImpl::execute()");
	//initialize the log filter
	//_IRA_LOGFILTER_ACTIVATE(m_CompConfiguration.repetitionCacheTime(),m_CompConfiguration.expireCacheTime());
	ACS_LOG(LM_FULL_INFO,"MedicinaMount::execute()",(LM_INFO,"MedicinaMount::LOGGING_REPETITION_FILTER_ACTIVATED"));		
	CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_ACULink->Get();
	ACS_LOG(LM_FULL_INFO,"MedicinaMountImpl::execute()",(LM_INFO,"MedicinaMount::ACU_SOCKET_CONNECTING"));
	// this could throw an exception.....
	Socket->Init(&m_CompConfiguration);
	ACS_LOG(LM_FULL_INFO,"MedicinaMountImpl::execute()",(LM_INFO,"MedicinaMount::ACU_SOCKET_CONNECTED"));
	timeSlice=m_CompConfiguration.controlThreadPeriod();
	ACS_LOG(LM_FULL_INFO,"MedicinaMountImpl::execute()",(LM_INFO,"MedicinaMount::CONTROL_THREAD_STARTING"));	
	try {
		if(m_pcontrolLoop==NULL) {
			m_pcontrolLoop=getContainerServices()->getThreadManager()->create<CMedicinaMountControlThread,CSecureArea<CMedicinaMountSocket> *>("MedicinaMountControl",m_ACULink);
			m_pcontrolLoop->setSleepTime(timeSlice);
			m_pcontrolLoop->resume();
		}
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"MedicinaMountImpl::execute()");
	}
	ACS_LOG(LM_FULL_INFO,"MedicinaMountImpl::execute()",(LM_INFO,"MedicinaMount::CONTROL_THREAD_STARTED"));	
	try {
		startPropertiesMonitoring();
	}
	catch (acsthreadErrType::CanNotStartThreadExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"MedicinaMountImpl::execute");
		throw __dummy;
	}
	catch (ACSErrTypeCommon::NullPointerExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"MedicinaMountImpl::execute");
		throw __dummy;		
	}
	ACS_LOG(LM_FULL_INFO,"MedicinaMountImpl::execute()",(LM_INFO,"MedicinaMount::COMPSTATE_OPERATIONAL"));
}

void MedicinaMountImpl::cleanUp()
{
	AUTO_TRACE("MedicinaMountImpl::cleanUp()");
	stopPropertiesMonitoring();
	if (m_pcontrolLoop!=NULL) {
		m_pcontrolLoop->suspend();
		getContainerServices()->getThreadManager()->destroy(m_pcontrolLoop);
	}
	ACS_LOG(LM_FULL_INFO,"MedicinaMountImpl::cleanUp()",(LM_INFO,"MedicinaMount::CONTROL_THREAD_TERMINATED"));	
	/*if (m_ACUDataSupplier!=0) {
		m_ACUDataSupplier->disconnect();
		m_ACUDataSupplier=0;
	}
	ACS_LOG(LM_FULL_INFO,"MedicinaMountImpl::cleanUp()",(LM_INFO,"MedicinaMount::NOTIFICATION_CHANNEL_DISCONNECTED"));*/	
	// the protected object is destoyed by the secure area destructor
	if (m_ACULink) {
		CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_ACULink->Get();
		Socket->cleanUp();
		Socket.Release();
		delete m_ACULink;
		m_ACULink=NULL;
	}
	ACS_LOG(LM_FULL_INFO,"MedicinaMountImpl::cleanUp()",(LM_INFO,"MedicinaMount::ACU_SOCKET_CLOSED"));
	CharacteristicComponentImpl::cleanUp();	
}

void MedicinaMountImpl::aboutToAbort()
{
	AUTO_TRACE("MedicinaMountImpl::aboutToAbort()");
	if (m_pcontrolLoop!=NULL) m_pcontrolLoop->suspend();
	getContainerServices()->getThreadManager()->destroy(m_pcontrolLoop);
	// the protected object is destoyed by the secure area destructor
	if (m_ACULink) {
		CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_ACULink->Get();
		Socket->cleanUp();
		Socket.Release();		
		delete m_ACULink;
		m_ACULink=NULL;
	}		
}

ActionRequest MedicinaMountImpl::invokeAction(int function,BACIComponent* cob,const int& callbackID,const CBDescIn& descIn,
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

ActionRequest MedicinaMountImpl::unstowAction(BACIComponent* cob,const int& callbackID,const CBDescIn& descIn,BACIValue* value_p,
  Completion& completion,CBDescOut& descOut)
{
	ACS::TimeInterval sleep100ns;
	AUTO_TRACE("MedicinaMountImpl::unstowAction()");        
	// the normal_timeout is interpreted as sleep time, if smaller than zero or greater than 1 sec is ignored.
	sleep100ns=descIn.normal_timeout;
	if (sleep100ns>0 && sleep100ns<10000000) {
		long micro=(sleep100ns-1)/10;
		CIRATools::Wait(0,micro);
	}
	// copy of the TAG
	descOut.id_tag=descIn.id_tag;
	CMedicinaMountControlThread::TLongJobResult *param=static_cast<CMedicinaMountControlThread::TLongJobResult*>
	  (const_cast<void *>(value_p->pointerValue()));
	if (param->done) {
		completion=param->comp;
		delete param;	
		return reqInvokeDone;
	}
	else {
		completion=ComponentErrors::NoErrorCompletion();
		return reqInvokeWorking;		
	}
}

ActionRequest MedicinaMountImpl::stowAction(BACIComponent* cob,const int& callbackID,const CBDescIn& descIn,BACIValue* value_p,
  Completion& completion,CBDescOut& descOut)
{
	ACS::TimeInterval sleep100ns;
	AUTO_TRACE("MedicinaMountImpl::stowAction()");        
	// the normal_timeout is interpreted as sleep time, if smaller than zero or greater than 1 sec is ignored.
	sleep100ns=descIn.normal_timeout;
	if (sleep100ns>0 && sleep100ns<10000000) {
		long micro=(sleep100ns-1)/10;
		CIRATools::Wait(0,micro);
	}
	// copy of the TAG
	descOut.id_tag=descIn.id_tag;
	CMedicinaMountControlThread::TLongJobResult *param=static_cast<CMedicinaMountControlThread::TLongJobResult*>
	  (const_cast<void *>(value_p->pointerValue()));
	if (param->done) {
		completion=param->comp;
		delete param;
		return reqInvokeDone;
	}
	else {
		completion=ComponentErrors::NoErrorCompletion();
		return reqInvokeWorking;		
	}
}

void MedicinaMountImpl::stop() throw (CORBA::SystemException,ComponentErrorsEx,AntennaErrorsEx)
{
	AUTO_TRACE("MedicinaMountImpl::stop()");
	if (m_ACULink) {
		CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_ACULink->Get();
		try {
			Socket->Stop();
			ACS_LOG(LM_FULL_INFO,"MedicinaMountImpl::stop()",(LM_NOTICE,"MedicinaMount::ANTENNA_STOPPED"));		
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
}

void MedicinaMountImpl::unstow(ACS::CBvoid_ptr cb,const ACS::CBDescIn &desc) throw (CORBA::SystemException)
{
	ACS::CBDescOut Dout;
	AUTO_TRACE("MedicinaMountImpl::unstow()");
	if (m_ACULink) {
		// copy the tag
		Dout.id_tag=desc.id_tag;
		// get the ACU communication link
		CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_ACULink->Get();
		try {
			Socket->Mode(CACUInterface::UNSTOW,CACUInterface::UNSTOW);
			ACS_LOG(LM_FULL_INFO,"MedicinaMountImpl::unstow()",(LM_NOTICE,"MedicinaMount::UNSTOW_COMMAND"));		
		}		
		catch (ACSErr::ACSbaseExImpl& E) {
			_COMPL_FROM_EXCPT(OperationErrorCompletion,dummy,E,"MedicinaMountImpl::unstow()");
			dummy.setReason("Mode could not be changed");
			//_IRA_LOGFILTER_LOG_COMPLETION(dummy,LM_DEBUG);
			dummy.log(LM_DEBUG);
			try {
				cb->done(dummy,Dout);
			}
			catch (...) {
			}
			return;
		}
		// component is now surely not busy and connected.....so i can safely register the new long Job.
		CMedicinaMountControlThread::TLongJobResult* tmp=m_pcontrolLoop->setLongJob(UNSTOW_ACTION);
		// sets the ACU busy	
		Socket->setStatus(Antenna::ACU_BSY);
		// register the action!
		getComponent()->registerAction(BACIValue::type_null,cb,desc,this,UNSTOW_ACTION,BACIValue(tmp));
	}
}

void MedicinaMountImpl::stow(ACS::CBvoid_ptr cb,const ACS::CBDescIn &desc) throw (CORBA::SystemException)
{
	ACS::CBDescOut Dout;
	AUTO_TRACE("MedicinaMountImpl::stow()");
	if (m_ACULink) {
		// copy the tag
		Dout.id_tag=desc.id_tag;
		// get the ACU communication link
		CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_ACULink->Get();
		try {
			Socket->Mode(CACUInterface::STOW,CACUInterface::STOW);
			ACS_LOG(LM_FULL_INFO,"MedicinaMountImpl::stow()",(LM_NOTICE,"MedicinaMount::STOW_COMMAND"));				
		}		
		catch (ACSErr::ACSbaseExImpl& E) {
			_COMPL_FROM_EXCPT(OperationErrorCompletion,dummy,E,"MedicinaMountImpl::stow()");
			dummy.setReason("Mode could not be changed");
			//_IRA_LOGFILTER_LOG_COMPLETION(dummy,LM_DEBUG);
			dummy.log(LM_DEBUG);
			try {
				cb->done(dummy,Dout);
			}
			catch (...) {
			}
			return;
		}
		// component is now surely not busy and connected.....so i can safely register the new long Job.
		CMedicinaMountControlThread::TLongJobResult* tmp=m_pcontrolLoop->setLongJob(STOW_ACTION);
		// sets the ACU busy	
		Socket->setStatus(Antenna::ACU_BSY);
		// register the action!
		getComponent()->registerAction(BACIValue::type_null,cb,desc,this,STOW_ACTION,BACIValue(tmp));
	}
}

void MedicinaMountImpl::preset(CORBA::Double az,CORBA::Double el) throw (CORBA::SystemException, ComponentErrorsEx, AntennaErrorsEx)
{
	AUTO_TRACE("MedicinaMountImpl::preset()");
	if (m_ACULink) {
		// get the ACU communication link
		CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_ACULink->Get();
		// set the commanded position	
		try {
			Socket->Preset(az,el);
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
}

void MedicinaMountImpl::programTrack(CORBA::Double az,CORBA::Double el,ACS::Time time,CORBA::Boolean restart) 
  throw (CORBA::SystemException, ComponentErrorsEx, AntennaErrorsEx)
{
 	AUTO_TRACE("MedicinaMountImpl::progamTrack()");
 	if (m_ACULink) {
 		// get the ACU communication link
 		CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_ACULink->Get();
 		try {
 			TIMEVALUE timeData(time);
 			Socket->programTrack(az,el,timeData,restart);
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
}

void MedicinaMountImpl::rates(CORBA::Double azRate,CORBA::Double elRate) throw (CORBA::SystemException,ComponentErrorsEx,AntennaErrorsEx)
{
	AUTO_TRACE("MedicinaMountImpl::rates()");
	if (m_ACULink) {
		// get the ACU communication link
		CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_ACULink->Get();
		try {
			Socket->Rate(azRate,elRate);
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
}

void MedicinaMountImpl::resetFailures() throw (CORBA::SystemException, ComponentErrorsEx)
{
	AUTO_TRACE("MedicinaMountImpl::failureReset()");
	if (m_ACULink) {
		CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_ACULink->Get();	
		try {
			Socket->failureReset();
			ACS_LOG(LM_FULL_INFO,"MedicinaMountImpl::resetFailures()",(LM_NOTICE,"MedicinaMount::FAILURES_CLEARED"));		
		}		
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(OperationErrorExImpl,dummy,E,"MedicinaMountImpl::failureReset()");
			dummy.setReason("servo failure could not be cleared");
			//_IRA_LOGFILTER_LOG_EXCEPTION(dummy,LM_DEBUG);
			dummy.log(LM_DEBUG);
			throw dummy.getComponentErrorsEx();		
		}
	}
}

void MedicinaMountImpl::changeMode(Antenna::TCommonModes azMode,Antenna::TCommonModes elMode) throw (CORBA::SystemException, ComponentErrorsEx,AntennaErrorsEx)
{
	AUTO_TRACE("MedicinaMountImpl::changeMode()");
	if (m_ACULink) {
		if ((azMode!=Antenna::ACU_PRESET) && (azMode!=Antenna::ACU_PROGRAMTRACK) && (azMode!=Antenna::ACU_RATE)) {
			CString msg;
			_EXCPT(OperationNotPermittedExImpl,dummy,"MedicinaMountImpl::changeMode()");
			msg="Mode "+CACUInterface::messageFromMode(CMedicinaMountSocket::idlModes2ACU(azMode))+" is not permitted";
			dummy.setReason((const char *)msg);
			_ADD_BACKTRACE(ValidationErrorExImpl,foo,dummy,"MedicinaMountImpl::changeMode()");
			foo.setReason("Azimuth mode is not allowed");
			//_IRA_LOGFILTER_LOG_EXCEPTION(foo,LM_DEBUG);
			foo.log(LM_DEBUG);
			throw foo.getComponentErrorsEx();		
		}
		if ((elMode!=Antenna::ACU_PRESET) && (elMode!=Antenna::ACU_PROGRAMTRACK) && (elMode!=Antenna::ACU_RATE)) {
			CString msg;
			_EXCPT(OperationNotPermittedExImpl,dummy,"MedicinaMountImpl::changeMode()");
			msg="Mode "+CACUInterface::messageFromMode(CMedicinaMountSocket::idlModes2ACU(elMode))+" is not permitted";
			dummy.setReason((const char *)msg);
			_ADD_BACKTRACE(ValidationErrorExImpl,foo,dummy,"MedicinaMountImpl::changeMode()");
			foo.setReason("Elevation mode is not allowed");
			//_IRA_LOGFILTER_LOG_EXCEPTION(dummy,LM_DEBUG);
			foo.log(LM_DEBUG);
			throw foo.getComponentErrorsEx();	  
		}
		CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_ACULink->Get();
		CACUInterface::TAxeModes az,el;
		az=CMedicinaMountSocket::idlModes2ACU(azMode);
		el=CMedicinaMountSocket::idlModes2ACU(elMode);
		try {
			Socket->Mode(az,el);
			IRA::CString azName,elName;
			azName=CACUInterface::messageFromMode(az);
			elName=CACUInterface::messageFromMode(el);
			ACS_LOG(LM_FULL_INFO,"MedicinaACU::changeMode()",(LM_NOTICE,"MedicinaACU::MODE_CHANGED az=%s el=%s",(const char*)azName,(const char *)elName));		
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
}

void MedicinaMountImpl::setTime(ACS::Time now) throw (CORBA::SystemException, ComponentErrorsEx, AntennaErrorsEx)
{
	AUTO_TRACE("MedicinaMountImpl::setTime()");
	if (m_ACULink) {	
		TIMEVALUE timeData(now);
		CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_ACULink->Get();	
		try {
			Socket->timeTransfer(timeData);
			ACS_LOG(LM_FULL_INFO,"MedicinaMountImpl::setTime()",(LM_NOTICE,"MedicinaMount:ACU_TIME_CHANGED"));
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
}

void MedicinaMountImpl::getEncoderCoordinates(ACS::Time_out time,CORBA::Double_out azimuth,CORBA::Double_out elevation,
	  CORBA::Double_out azOffset,CORBA::Double_out elOffset,Antenna::TSections_out section) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, AntennaErrorsEx)
{
	AUTO_TRACE("MedicinaMountImpl::getEncodersCoordinates()");
	if (m_ACULink) {	
		CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_ACULink->Get();
		try {
			CACUInterface::TAntennaSection tmp;
			Socket->getEncodersCoordinates(azimuth,elevation,azOffset,elOffset,time,tmp);
			section=CMedicinaMountSocket::acuSection2IDL(tmp);
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
 }

void MedicinaMountImpl::getAntennaErrors(ACS::Time_out time,CORBA::Double_out azError,CORBA::Double_out elError) throw (
		CORBA::SystemException, ComponentErrors::ComponentErrorsEx,AntennaErrorsEx)
{
	AUTO_TRACE("MedicinaMountImpl::getAntennaErrors()");
	if (m_ACULink) {	
		CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_ACULink->Get();
		try {
			Socket->getAntennaErrors(azError,elError,time);
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
}

void MedicinaMountImpl::forceSection(Antenna::TSections section) throw(CORBA::SystemException)
{
	AUTO_TRACE("MedicinaMountImpl::forceSection()");
	if (m_ACULink) {
		CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_ACULink->Get();
		Socket->setCommandSection(section);
	}
}

CORBA::Double MedicinaMountImpl::getHWAzimuth(CORBA::Double destination,Antenna::TSections section) throw(CORBA::SystemException)
{
	AUTO_TRACE("MedicinaMountImpl::getHWAzimuth()");
	if (m_ACULink) {
		CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_ACULink->Get();
		CACUInterface::TAntennaSection sect=CMedicinaMountSocket::IDLSection2Acu(section);
		return Socket->getHWAzimuth(destination,sect);
	}
	else return 0.0;
}

void MedicinaMountImpl::setOffsets(CORBA::Double azOff,CORBA::Double elOff) throw (CORBA::SystemException, ComponentErrorsEx, AntennaErrorsEx)
{
	double az,el;
	AUTO_TRACE("MedicinaMountImpl::setOffsets()");
	if (m_ACULink) {		
		CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_ACULink->Get();
		az=(double)azOff;
		el=(double)elOff;
		try {
			Socket->setPositionOffsets(az,el);
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
}
		
void MedicinaMountImpl::setDeltaTime(CORBA::Double delta) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, AntennaErrorsEx)
{
	double time;
	AUTO_TRACE("MedicinaMountImpl::setDeltaTime()");
	if (m_ACULink) {
		CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_ACULink->Get();
		time=(double)delta;
		Socket->setDeltaTime(time);
	}
}

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pelevation,elevation);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pazimuth,azimuth);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pazimuthError,azimuthError);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pelevationError,elevationError);

GET_PROPERTY_REFERENCE(Antenna::ROTCommonModes,m_pazMode,azimuthMode);

GET_PROPERTY_REFERENCE(Antenna::ROTCommonModes,m_pelMode,elevationMode);

GET_PROPERTY_REFERENCE(ACS::ROuLongLong,m_ptime,time);

GET_PROPERTY_REFERENCE(Antenna::ROTStatus,m_pstatus,status);

GET_PROPERTY_REFERENCE(ACS::ROpattern,m_pazimuthServoStatus,azimuthServoStatus);

GET_PROPERTY_REFERENCE(ACS::ROpattern,m_pelevationServoStatus,elevationServoStatus);

GET_PROPERTY_REFERENCE(ACS::ROpattern,m_pservoSystemStatus,servoSystemStatus);

GET_PROPERTY_REFERENCE(ACS::ROlong,m_pfreeProgramTrackPosition,freeProgramTrackPosition);

GET_PROPERTY_REFERENCE(Antenna::ROTSections,m_psection,section);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pelevationRate,elevationRate);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pazimuthRate,azimuthRate);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pazimuthOffset,azimuthOffset);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pelevationOffset,elevationOffset);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pdeltaTime,deltaTime);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pcommandedAzimuth,commandedAzimuth);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pcommandedElevation,commandedElevation);

GET_PROPERTY_REFERENCE(Management::ROTSystemStatus,m_pmountStatus,mountStatus);

void MedicinaMountImpl::changeAntennaStatus(Antenna::TStatus status)
{
	CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_ACULink->Get();	
	Socket->setStatus(status);
}

double MedicinaMountImpl::roundAngle(const double& ang)
{
	double tmp=fmod(ang,360.0);
	if (tmp<0.0) tmp+=360.0;
	return tmp;
}

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(MedicinaMountImpl)

/*___oOo___*/
