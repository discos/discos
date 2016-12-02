// $Id: NotoMinorServoBossImpl.cpp,v 1.2 2016-11-15 12:37:07 c.migoni Exp $

#include <new>
#include "NotoMinorServoBossImpl.h"
#include "DevIOStatus.h"
#include "DevIOEnable.h"
//#include "DevIOProfile.h"
#include "DevIOTracking.h"

static char *rcsId="@(#) $Id: NotoMinorServoBossImpl.cpp,v 1.2 2016-11-15 12:37:07 c.migoni Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

using namespace SimpleParser;

/*class NotoMinorServoProfile2String {
public:
	static char *valToStr(const MinorServo::TASProfile& val) {
		char *c=new char[16];
		if (val==MinorServo::AS_SHAPED) {
			strcpy(c,"S");
		}
		if (val==MinorServo::AS_SHAPED_FIXED) {
			strcpy(c,"SF");
		}
		if (val==MinorServo::AS_PARABOLIC) {
			strcpy(c,"P");
		}
		if (val==MinorServo::AS_PARABOLIC_FIXED) {
			strcpy(c,"PF");
		}
		return c;
	}
	static MinorServo::TASProfile strToVal(const char* str) throw (ParserErrors::BadTypeFormatExImpl) {
		IRA::CString strVal(str);
		strVal.MakeUpper();
		if (strVal=="S") {
			return MinorServo::AS_SHAPED;
		}
		else if (strVal=="SF") {
			return MinorServo::AS_SHAPED_FIXED;
		}
		else if (strVal=="P") {
			return MinorServo::AS_PARABOLIC;
		}
		else if (strVal=="PF") {
			return MinorServo::AS_PARABOLIC_FIXED;
		}
		else {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"NotoMinorServoProfile2String::strToVal()");
			throw ex;
		}
	}
};*/

NotoMinorServoBossImpl::NotoMinorServoBossImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices) : 
	CharacteristicComponentImpl(CompName,containerServices),
	m_pstatus(this),
	m_penabled(this),
//	m_pprofile(this),
	m_ptracking(this),
	m_core(NULL)
{	
	AUTO_TRACE("NotoMinorServoBossImpl::NotoMinorServoBossImpl()");
}

NotoMinorServoBossImpl::~NotoMinorServoBossImpl()
{
	AUTO_TRACE("NotoMinorServoBossImpl::~NotoMinorServoBossImpl()");
}

void NotoMinorServoBossImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("NotoMinorServoBossImpl::initialize()");
    	cs = getContainerServices();
    	ACS_LOG(LM_FULL_INFO,"NotoMinorServoBossImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZING"));
	try {
		boss=(CNotoMinorServoBossCore *)new CNotoMinorServoBossCore(getContainerServices());
		m_core=new IRA::CSecureArea<CNotoMinorServoBossCore>(boss);
        	m_pstatus=new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus>
		  (getContainerServices()->getName()+":status",getComponent(),new NotoMinorServoBossImplDevIOStatus(m_core),true);	
		m_penabled=new ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>
		  (getContainerServices()->getName()+":enabled",getComponent(),new NotoMinorServoBossImplDevIOEnable(m_core),true);
//		m_pprofile=new ROEnumImpl<ACS_ENUM_T(MinorServo::TASProfile),POA_MinorServo::ROTASProfile>
//		  (getContainerServices()->getName()+":pprofile",getComponent(),new NotoMinorServoBossImplDevIOProfile(m_core),true);	
		m_ptracking=new ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>
		  (getContainerServices()->getName()+":tracking",getComponent(),new NotoMinorServoBossImplDevIOTracking(m_core),true);

        	// create the parser for command line execution
		m_parser =  new SimpleParser::CParser<CNotoMinorServoBossCore>(boss,10);
    	}
    	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"NotoMinorServoBossImpl::initialize()");
		throw dummy;
	}
    	boss->initialize();
    	try {
		m_watchingThread=getContainerServices()->getThreadManager()->create<CNotoMinorServoBossWatchingThread,CSecureArea<CNotoMinorServoBossCore> *>
		  ("NotoMinorServoBOSSWATCHER",m_core);
	}
	catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"NotoMinorServoBossImpl::initialize()");
		throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"NotoMinorServoBossImpl::initialize()");
	}

	try {
		m_workingThread=getContainerServices()->getThreadManager()->create<CNotoMinorServoBossWorkingThread,CSecureArea<CNotoMinorServoBossCore> *>
		  ("NotoMinorServoBOSSWORKER",m_core);
	}
	catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"NotoMinorServoBossImpl::initialize()");
		throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"NotoMinorServoBossImpl::initialize()");
	}
/*
    	if (CIRATools::getDBValue(cs,"profile",(long&)m_profile))
	{
		ACS_SHORT_LOG((LM_INFO,"NotoMinorServoBoss: CDB %d profile parameter read", m_profile));
		boss->m_profile = m_profile;
	}
	else
	{
		ACS_LOG(LM_SOURCE_INFO,"NotoMinorServoBossImpl:initialize()",(LM_ERROR,"Error reading CDB!"));
		ASErrors::CDBAccessErrorExImpl exImpl(__FILE__,__LINE__,"USDImpl::initialize() - Error reading CDB parameters");
		throw acsErrTypeLifeCycle::LifeCycleExImpl(exImpl,__FILE__,__LINE__,"USDImpl::initialize()");
	}
*/
	m_configuration.init(cs);

    	// configure the parser.....
   m_parser->add("servoSetup",new function1<CNotoMinorServoBossCore,non_constant,void_type,I<string_type> >(boss,&CNotoMinorServoBossCore::msSetup),1 );
	m_parser->add("servoOn",new function0<CNotoMinorServoBossCore,non_constant,void_type >(boss,&CNotoMinorServoBossCore::msOn),0);
	m_parser->add("servoOff",new function0<CNotoMinorServoBossCore,non_constant,void_type >(boss,&CNotoMinorServoBossCore::msOff),0);
	m_parser->add("servoPark",new function0<CNotoMinorServoBossCore,non_constant,void_type >(boss,&CNotoMinorServoBossCore::msPark),0);

	ACS_LOG(LM_FULL_INFO, "NotoMinorServoBossImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZED"));
}

void NotoMinorServoBossImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("NotoMinorServoBossImpl::execute()");

    	try {
		boss->execute(&m_configuration);
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,_dummy,E,"NotoMinorServoBossImpl::execute()");
		throw _dummy;
	}
	//starts the loop status thread....
	//m_watchingThread->resume();
	//m_watchingThread->setSleepTime(LOOPSTATUSTIME);

	//starts the loop working thread....
	m_workingThread->resume();
	m_workingThread->setSleepTime(LOOPWORKINGTIME);

	ACS_LOG(LM_FULL_INFO,"NotoMinorServoBossImpl::execute()",(LM_INFO,"NotoMinorServoBossImpl::COMPSTATE_OPERATIONAL"));
}

void NotoMinorServoBossImpl::cleanUp()
{
		AUTO_TRACE("NotoMinorServoBossImpl::cleanUp()");
    	CSecAreaResourceWrapper<CNotoMinorServoBossCore>  core=m_core->Get();
   	if (m_workingThread!=NULL) m_workingThread->suspend();
   	if (m_watchingThread!=NULL) m_watchingThread->suspend();
    	getContainerServices()->getThreadManager()->destroy(m_workingThread);
    	getContainerServices()->getThreadManager()->destroy(m_watchingThread);
    	ACS_LOG(LM_FULL_INFO,"NotoMinorServoBossImpl::cleanUp()",(LM_INFO,"NotoMinorServoBossImpl::THREADS_TERMINATED"));
		core->cleanUp();
		ACS_LOG(LM_FULL_INFO,"NotoMinorServoBossImpl::cleanUp()",(LM_INFO,"NotoMinorServoBossImpl::BOSS_CORE_FREED"));
    	if (m_parser!=NULL) delete m_parser;
		ACS_LOG(LM_FULL_INFO,"NotoMinorServoBossImpl::cleanUp()",(LM_INFO,"NotoMinorServoBossImpl::PARSER_FREED"));
		CharacteristicComponentImpl::cleanUp();
}

void NotoMinorServoBossImpl::aboutToAbort()
{
	AUTO_TRACE("NotoMinorServoBossImpl::aboutToAbort()");
    	if (m_workingThread!=NULL) m_workingThread->suspend();
    	if (m_watchingThread!=NULL) m_watchingThread->suspend();
    	getContainerServices()->getThreadManager()->destroy(m_workingThread);
    	getContainerServices()->getThreadManager()->destroy(m_watchingThread);
	CSecAreaResourceWrapper<CNotoMinorServoBossCore>  core=m_core->Get(); 
	core->cleanUp();
    	if (m_parser!=NULL) delete m_parser;
}
/*
void NotoMinorServoBossImpl::stop (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoMinorServoBossImpl::stop()");

    CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
    try {
        //resource->onewayAction(MinorServo::AS_STOP, circle, actuator, radius, 0, 0, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}
*/
void NotoMinorServoBossImpl::setup (const char *config) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoMinorServoBossImpl::setup()");
	//IRA::CString strVal(config);
	//strVal.MakeUpper();

	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
   try {
   	resource->msSetup(config);
   }
   catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"NotoMinorServoBossImpl::setup()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
}

void NotoMinorServoBossImpl::park () throw (CORBA::SystemException, ManagementErrors::ParkingErrorEx)
{
	AUTO_TRACE("NotoMinorServoBossImpl::park()");
	msOff();
	//setProfile (MinorServo::AS_SHAPED_FIXED);
    	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
    	try {
        	//resource->onewayAction(MinorServo::AS_UPDATE, 0, 0, 0, 45.0, 0, 0, m_profile);
        	resource->msPark();
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		_EXCPT(ManagementErrors::ParkingErrorExImpl,ex,"NotoMinorServoBossImpl::park()");
		throw ex.getParkingErrorEx();
	}
	resource->m_tracking = false;
}
/*
void NotoMinorServoBossImpl::stow (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoMinorServoBossImpl::stow()");

    	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
    	try {
        	//resource->onewayAction(MinorServo::AS_STOW, circle, actuator, radius, 0, 0, 0, m_profile);
        	resource->asPark();
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoMinorServoBossImpl::refPos (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoMinorServoBossImpl::refPos()");

    	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
    	try {
        	//resource->onewayAction(MinorServo::AS_REFPOS, circle, actuator, radius, 0, 0, 0, m_profile);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoMinorServoBossImpl::update (CORBA::Double elevation) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoMinorServoBossImpl::update()");

    	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();

	try {
        	//resource->onewayAction(MinorServo::AS_UPDATE, 0, 0, 0, elevation, 0, 0, m_profile);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoMinorServoBossImpl::up (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoMinorServoBossImpl::up()");

    	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
    	try {
        	//resource->onewayAction(MinorServo::AS_UP, circle, actuator, radius, 0, 0, 0, m_profile);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoMinorServoBossImpl::down (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoMinorServoBossImpl::down()");

    	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
    	try {
        	//resource->onewayAction(MinorServo::AS_DOWN, circle, actuator, radius, 0, 0, 0, m_profile);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoMinorServoBossImpl::bottom (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoMinorServoBossImpl::bottom()");

    	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
    	try {
        	//resource->onewayAction(MinorServo::AS_BOTTOM, circle, actuator, radius, 0, 0, 0, m_profile);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoMinorServoBossImpl::top (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoMinorServoBossImpl::top()");

    	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
    	try {
        	//resource->onewayAction(MinorServo::AS_TOP, circle, actuator, radius, 0, 0, 0, m_profile);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoMinorServoBossImpl::move (CORBA::Long circle,  CORBA::Long actuator,  CORBA::Long radius, CORBA::Long incr) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoMinorServoBossImpl::move()");

    	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
    	try {
        	//resource->onewayAction(MinorServo::AS_MOVE, circle, actuator, radius, 0, 0, incr, m_profile);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoMinorServoBossImpl::correction (CORBA::Long circle,  CORBA::Long actuator,  CORBA::Long radius, CORBA::Double correction) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoMinorServoBossImpl::correction()");

	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
	try {
		//resource->onewayAction(MinorServo::AS_CORRECTION, circle, actuator, radius, 0, correction, 0, m_profile);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoMinorServoBossImpl::reset (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoMinorServoBossImpl::reset()");

    	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
    	try {
        	//resource->reset(circle, actuator, radius);
   	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoMinorServoBossImpl::setProfile (MinorServo::TASProfile newProfile) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoMinorServoBossImpl::setProfile()");

	m_profile = newProfile;

	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
	try {
		//resource->setProfile(m_profile);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoMinorServoBossImpl::usdStatus4GUIClient (CORBA::Long circle, CORBA::Long actuator, CORBA::Long_out status) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoMinorServoBossImpl::usdStatus4GUIClient()");

    	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
    	try {
        	//resource->usdStatus4GUIClient(circle, actuator, status);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        	ex.log(LM_DEBUG);
        	throw ex.getComponentErrorsEx();
    	}
}

void NotoMinorServoBossImpl::setActuator (CORBA::Long circle, CORBA::Long actuator, CORBA::Long_out actPos, CORBA::Long_out cmdPos, CORBA::Long_out Fmin, CORBA::Long_out Fmax, CORBA::Long_out acc, CORBA::Long_out delay) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoMinorServoBossImpl::setActuator");

    	//long int act, cmd, fmin, fmax, ac, del;

    	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
    	try {
        	//resource->setActuator(circle, actuator, act, cmd, fmin, fmax, ac, del);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
    	actPos = (CORBA::Long)act;
    	cmdPos = (CORBA::Long)cmd;
    	Fmin = (CORBA::Long)fmin;
    	Fmax = (CORBA::Long)fmax;
    	acc = (CORBA::Long)ac;
    	delay = (CORBA::Long)del;
}


void NotoMinorServoBossImpl::calibrate (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoMinorServoBossImpl::calibrate()");

    	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
    	try {
        	//resource->calibrate(circle, actuator, radius);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoMinorServoBossImpl::calVer (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoMinorServoBossImpl::calibration verification()");

    	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
    	try {
        	//resource->calVer(circle, actuator, radius);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}


void NotoMinorServoBossImpl::recoverUSD (CORBA::Long circle, CORBA::Long actuator) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoMinorServoBossImpl::recoverUSD()");

    	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
    	try {
        	//resource->recoverUSD(circle, actuator); TBC!!
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}
*/
void NotoMinorServoBossImpl::msOn() throw (CORBA::SystemException)
{
	AUTO_TRACE("NotoMinorServoBossImpl::asOn()");
	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
	try {
		resource->msOn();
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoMinorServoBossImpl::msOff() throw (CORBA::SystemException)
{
	AUTO_TRACE("NotoMinorServoBossImpl::asOff()");
	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
	try {
		resource->msOff();
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

CORBA::Boolean NotoMinorServoBossImpl::command(const char *cmd,CORBA::String_out answer) throw (CORBA::SystemException)
{
	AUTO_TRACE("AntennaBossImpl::command()");
	IRA::CString out;
	bool res;

	CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();
	try {
		m_parser->run(cmd,out);
		res = true;
	}
	catch (ParserErrors::ParserErrorsExImpl &ex) {
		res = false;
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		ex.log(LM_ERROR); // the errors resulting from the execution are logged here as stated
				  // in the documentation of CommandInterpreter interface,
				  // while the parser errors are never logged.
		res=false;
	}
	answer=CORBA::string_dup((const char *)out);
	return res;
}

_PROPERTY_REFERENCE_CPP(NotoMinorServoBossImpl,Management::ROTSystemStatus,m_pstatus,status);
_PROPERTY_REFERENCE_CPP(NotoMinorServoBossImpl,Management::ROTBoolean,m_penabled,enabled);
//_PROPERTY_REFERENCE_CPP(NotoMinorServoBossImpl,MinorServo::ROTASProfile,m_pprofile,pprofile);
_PROPERTY_REFERENCE_CPP(NotoMinorServoBossImpl,Management::ROTBoolean,m_ptracking,tracking);

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(NotoMinorServoBossImpl)

/*___oOo___*/
