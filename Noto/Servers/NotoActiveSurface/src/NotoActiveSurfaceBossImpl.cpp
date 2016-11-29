// $Id: NotoActiveSurfaceBossImpl.cpp,v 1.2 2016-11-15 12:37:07 c.migoni Exp $

#include <new>
#include "NotoActiveSurfaceBossImpl.h"
#include "DevIOStatus.h"
#include "DevIOEnable.h"
#include "DevIOProfile.h"
#include "DevIOTracking.h"

static char *rcsId="@(#) $Id: NotoActiveSurfaceBossImpl.cpp,v 1.2 2016-11-15 12:37:07 c.migoni Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

using namespace SimpleParser;

class NotoActiveSurfaceProfile2String {
public:
	static char *valToStr(const ActiveSurface::TASProfile& val) {
		char *c=new char[16];
		if (val==ActiveSurface::AS_SHAPED) {
			strcpy(c,"S");
		}
		if (val==ActiveSurface::AS_SHAPED_FIXED) {
			strcpy(c,"SF");
		}
		if (val==ActiveSurface::AS_PARABOLIC) {
			strcpy(c,"P");
		}
		if (val==ActiveSurface::AS_PARABOLIC_FIXED) {
			strcpy(c,"PF");
		}
		return c;
	}
	static ActiveSurface::TASProfile strToVal(const char* str) throw (ParserErrors::BadTypeFormatExImpl) {
		IRA::CString strVal(str);
		strVal.MakeUpper();
		if (strVal=="S") {
			return ActiveSurface::AS_SHAPED;
		}
		else if (strVal=="SF") {
			return ActiveSurface::AS_SHAPED_FIXED;
		}
		else if (strVal=="P") {
			return ActiveSurface::AS_PARABOLIC;
		}
		else if (strVal=="PF") {
			return ActiveSurface::AS_PARABOLIC_FIXED;
		}
		else {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"NotoActiveSurfaceProfile2String::strToVal()");
			throw ex;
		}
	}
};

NotoActiveSurfaceBossImpl::NotoActiveSurfaceBossImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices) : 
	CharacteristicComponentImpl(CompName,containerServices),
	m_pstatus(this),
	m_penabled(this),
	m_pprofile(this),
	m_ptracking(this),
	m_core(NULL)
{	
	AUTO_TRACE("NotoActiveSurfaceBossImpl::NotoActiveSurfaceBossImpl()");
}

NotoActiveSurfaceBossImpl::~NotoActiveSurfaceBossImpl()
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::~NotoActiveSurfaceBossImpl()");
}

void NotoActiveSurfaceBossImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::initialize()");
    	cs = getContainerServices();
    	ACS_LOG(LM_FULL_INFO,"NotoActiveSurfaceBossImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZING"));
	try {
		boss=(CNotoActiveSurfaceBossCore *)new CNotoActiveSurfaceBossCore(getContainerServices());
		m_core=new IRA::CSecureArea<CNotoActiveSurfaceBossCore>(boss);
        	m_pstatus=new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus>
		  (getContainerServices()->getName()+":status",getComponent(),new NotoActiveSurfaceBossImplDevIOStatus(m_core),true);	
		m_penabled=new ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>
		  (getContainerServices()->getName()+":enabled",getComponent(),new NotoActiveSurfaceBossImplDevIOEnable(m_core),true);
		m_pprofile=new ROEnumImpl<ACS_ENUM_T(ActiveSurface::TASProfile),POA_ActiveSurface::ROTASProfile>
		  (getContainerServices()->getName()+":pprofile",getComponent(),new NotoActiveSurfaceBossImplDevIOProfile(m_core),true);	
		m_ptracking=new ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>
		  (getContainerServices()->getName()+":tracking",getComponent(),new NotoActiveSurfaceBossImplDevIOTracking(m_core),true);

        	// create the parser for command line execution
		m_parser =  new SimpleParser::CParser<CNotoActiveSurfaceBossCore>(boss,10);
    	}
    	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"NotoActiveSurfaceBossImpl::initialize()");
		throw dummy;
	}
    	boss->initialize();
    	try {
		m_watchingThread=getContainerServices()->getThreadManager()->create<CNotoActiveSurfaceBossWatchingThread,CSecureArea<CNotoActiveSurfaceBossCore> *>
		  ("NotoACTIVESURFACEBOSSWATCHER",m_core);
	}
	catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"NotoActiveSurfaceBossImpl::initialize()");
		throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"NotoActiveSurfaceBossImpl::initialize()");
	}

	try {
		m_workingThread=getContainerServices()->getThreadManager()->create<CNotoActiveSurfaceBossWorkingThread,CSecureArea<CNotoActiveSurfaceBossCore> *>
		  ("NotoACTIVESURFACEBOSSWORKER",m_core);
	}
	catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"NotoActiveSurfaceBossImpl::initialize()");
		throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"NotoActiveSurfaceBossImpl::initialize()");
	}
/*
    	if (CIRATools::getDBValue(cs,"profile",(long&)m_profile))
	{
		ACS_SHORT_LOG((LM_INFO,"NotoActiveSurfaceBoss: CDB %d profile parameter read", m_profile));
		boss->m_profile = m_profile;
	}
	else
	{
		ACS_LOG(LM_SOURCE_INFO,"NotoActiveSurfaceBossImpl:initialize()",(LM_ERROR,"Error reading CDB!"));
		ASErrors::CDBAccessErrorExImpl exImpl(__FILE__,__LINE__,"USDImpl::initialize() - Error reading CDB parameters");
		throw acsErrTypeLifeCycle::LifeCycleExImpl(exImpl,__FILE__,__LINE__,"USDImpl::initialize()");
	}
*/
	m_configuration.init(cs);

    	// configure the parser.....
    	m_parser->add("asSetup",new function1<CNotoActiveSurfaceBossCore,non_constant,void_type,I<enum_type<NotoActiveSurfaceProfile2String,ActiveSurface::TASProfile> > >(boss,&CNotoActiveSurfaceBossCore::setProfile),1);
	m_parser->add("asOn",new function0<CNotoActiveSurfaceBossCore,non_constant,void_type >(boss,&CNotoActiveSurfaceBossCore::asOn),0);
	m_parser->add("asOff",new function0<CNotoActiveSurfaceBossCore,non_constant,void_type >(boss,&CNotoActiveSurfaceBossCore::asOff),0);
	m_parser->add("asPark",new function0<CNotoActiveSurfaceBossCore,non_constant,void_type >(boss,&CNotoActiveSurfaceBossCore::asPark),0);

	ACS_LOG(LM_FULL_INFO, "NotoActiveSurfaceBossImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZED"));
}

void NotoActiveSurfaceBossImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::execute()");

    	try {
		boss->execute(&m_configuration);
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,_dummy,E,"NotoActiveSurfaceBossImpl::execute()");
		throw _dummy;
	}
	//starts the loop status thread....
	//m_watchingThread->resume();
	//m_watchingThread->setSleepTime(LOOPSTATUSTIME);

	//starts the loop working thread....
	m_workingThread->resume();
	m_workingThread->setSleepTime(LOOPWORKINGTIME);

	ACS_LOG(LM_FULL_INFO,"NotoActiveSurfaceBossImpl::execute()",(LM_INFO,"NotoActiveSurfaceBossImpl::COMPSTATE_OPERATIONAL"));
}

void NotoActiveSurfaceBossImpl::cleanUp()
{
		AUTO_TRACE("NotoActiveSurfaceBossImpl::cleanUp()");
    	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore>  core=m_core->Get();
   	if (m_workingThread!=NULL) m_workingThread->suspend();
   	if (m_watchingThread!=NULL) m_watchingThread->suspend();
    	getContainerServices()->getThreadManager()->destroy(m_workingThread);
    	getContainerServices()->getThreadManager()->destroy(m_watchingThread);
    	ACS_LOG(LM_FULL_INFO,"NotoActiveSurfaceBossImpl::cleanUp()",(LM_INFO,"NotoActiveSurfaceBossImpl::THREADS_TERMINATED"));
		core->cleanUp();
		ACS_LOG(LM_FULL_INFO,"NotoActiveSurfaceBossImpl::cleanUp()",(LM_INFO,"NotoActiveSurfaceBossImpl::BOSS_CORE_FREED"));
    	if (m_parser!=NULL) delete m_parser;
		ACS_LOG(LM_FULL_INFO,"NotoActiveSurfaceBossImpl::cleanUp()",(LM_INFO,"NotoActiveSurfaceBossImpl::PARSER_FREED"));
		CharacteristicComponentImpl::cleanUp();
}

void NotoActiveSurfaceBossImpl::aboutToAbort()
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::aboutToAbort()");
    	if (m_workingThread!=NULL) m_workingThread->suspend();
    	if (m_watchingThread!=NULL) m_watchingThread->suspend();
    	getContainerServices()->getThreadManager()->destroy(m_workingThread);
    	getContainerServices()->getThreadManager()->destroy(m_watchingThread);
	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore>  core=m_core->Get(); 
	core->cleanUp();
    	if (m_parser!=NULL) delete m_parser;
}

void NotoActiveSurfaceBossImpl::stop (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::stop()");

    CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
    try {
        //resource->onewayAction(ActiveSurface::AS_STOP, circle, actuator, radius, 0, 0, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoActiveSurfaceBossImpl::setup (const char *config) throw (CORBA::SystemException, ManagementErrors::ConfigurationErrorEx)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::setup()");
	IRA::CString strVal(config);
	strVal.MakeUpper();
/*	if (strVal=="S") {
		setProfile(ActiveSurface::AS_SHAPED);
	}
	else if (strVal=="SF") {
		setProfile(ActiveSurface::AS_SHAPED_FIXED);
	}
	else if (strVal=="P") {
		setProfile(ActiveSurface::AS_PARABOLIC);
	}
	else if (strVal=="PF") {
		setProfile(ActiveSurface::AS_PARABOLIC_FIXED);
	}
	else {
		_EXCPT(ManagementErrors::ConfigurationErrorExImpl,ex,"NotoActiveSurfaceBossImpl::setup()");
		throw ex.getConfigurationErrorEx();
	} */
}

void NotoActiveSurfaceBossImpl::park () throw (CORBA::SystemException, ManagementErrors::ParkingErrorEx)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::park()");
	asOff();
	setProfile (ActiveSurface::AS_SHAPED_FIXED);
    	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
    	try {
        	//resource->onewayAction(ActiveSurface::AS_UPDATE, 0, 0, 0, 45.0, 0, 0, m_profile);
        	resource->asPark();
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		_EXCPT(ManagementErrors::ParkingErrorExImpl,ex,"NotoActiveSurfaceBossImpl::park()");
		throw ex.getParkingErrorEx();
	}
	resource->m_tracking = false;
}

void NotoActiveSurfaceBossImpl::stow (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::stow()");

    	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
    	try {
        	//resource->onewayAction(ActiveSurface::AS_STOW, circle, actuator, radius, 0, 0, 0, m_profile);
        	resource->asPark();
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoActiveSurfaceBossImpl::refPos (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::refPos()");

    	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
    	try {
        	//resource->onewayAction(ActiveSurface::AS_REFPOS, circle, actuator, radius, 0, 0, 0, m_profile);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoActiveSurfaceBossImpl::update (CORBA::Double elevation) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::update()");

    	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();

	try {
        	//resource->onewayAction(ActiveSurface::AS_UPDATE, 0, 0, 0, elevation, 0, 0, m_profile);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoActiveSurfaceBossImpl::up (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::up()");

    	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
    	try {
        	//resource->onewayAction(ActiveSurface::AS_UP, circle, actuator, radius, 0, 0, 0, m_profile);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoActiveSurfaceBossImpl::down (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::down()");

    	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
    	try {
        	//resource->onewayAction(ActiveSurface::AS_DOWN, circle, actuator, radius, 0, 0, 0, m_profile);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoActiveSurfaceBossImpl::bottom (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::bottom()");

    	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
    	try {
        	//resource->onewayAction(ActiveSurface::AS_BOTTOM, circle, actuator, radius, 0, 0, 0, m_profile);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoActiveSurfaceBossImpl::top (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::top()");

    	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
    	try {
        	//resource->onewayAction(ActiveSurface::AS_TOP, circle, actuator, radius, 0, 0, 0, m_profile);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoActiveSurfaceBossImpl::move (CORBA::Long circle,  CORBA::Long actuator,  CORBA::Long radius, CORBA::Long incr) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::move()");

    	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
    	try {
        	//resource->onewayAction(ActiveSurface::AS_MOVE, circle, actuator, radius, 0, 0, incr, m_profile);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoActiveSurfaceBossImpl::correction (CORBA::Long circle,  CORBA::Long actuator,  CORBA::Long radius, CORBA::Double correction) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::correction()");

	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
	try {
		//resource->onewayAction(ActiveSurface::AS_CORRECTION, circle, actuator, radius, 0, correction, 0, m_profile);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoActiveSurfaceBossImpl::reset (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::reset()");

    	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
    	try {
        	//resource->reset(circle, actuator, radius);
   	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoActiveSurfaceBossImpl::setProfile (ActiveSurface::TASProfile newProfile) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::setProfile()");

	m_profile = newProfile;

	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
	try {
		//resource->setProfile(m_profile);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoActiveSurfaceBossImpl::usdStatus4GUIClient (CORBA::Long circle, CORBA::Long actuator, CORBA::Long_out status) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::usdStatus4GUIClient()");

    	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
    	try {
        	//resource->usdStatus4GUIClient(circle, actuator, status);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        	ex.log(LM_DEBUG);
        	throw ex.getComponentErrorsEx();
    	}
}

void NotoActiveSurfaceBossImpl::setActuator (CORBA::Long circle, CORBA::Long actuator, CORBA::Long_out actPos, CORBA::Long_out cmdPos, CORBA::Long_out Fmin, CORBA::Long_out Fmax, CORBA::Long_out acc, CORBA::Long_out delay) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::setActuator");

    	//long int act, cmd, fmin, fmax, ac, del;

    	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
    	try {
        	//resource->setActuator(circle, actuator, act, cmd, fmin, fmax, ac, del);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
/*    	actPos = (CORBA::Long)act;
    	cmdPos = (CORBA::Long)cmd;
    	Fmin = (CORBA::Long)fmin;
    	Fmax = (CORBA::Long)fmax;
    	acc = (CORBA::Long)ac;
    	delay = (CORBA::Long)del;*/
}


void NotoActiveSurfaceBossImpl::calibrate (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::calibrate()");

    	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
    	try {
        	//resource->calibrate(circle, actuator, radius);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoActiveSurfaceBossImpl::calVer (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::calibration verification()");

    	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
    	try {
        	//resource->calVer(circle, actuator, radius);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}


void NotoActiveSurfaceBossImpl::recoverUSD (CORBA::Long circle, CORBA::Long actuator) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::recoverUSD()");

    	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
    	try {
        	//resource->recoverUSD(circle, actuator); TBC!!
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoActiveSurfaceBossImpl::asOn() throw (CORBA::SystemException)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::asOn()");
	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
	try {
		resource->asOn();
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NotoActiveSurfaceBossImpl::asOff() throw (CORBA::SystemException)
{
	AUTO_TRACE("NotoActiveSurfaceBossImpl::asOff()");
	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
	try {
		resource->asOff();
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

CORBA::Boolean NotoActiveSurfaceBossImpl::command(const char *cmd,CORBA::String_out answer) throw (CORBA::SystemException)
{
	AUTO_TRACE("AntennaBossImpl::command()");
	IRA::CString out;
	bool res;

	CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
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

_PROPERTY_REFERENCE_CPP(NotoActiveSurfaceBossImpl,Management::ROTSystemStatus,m_pstatus,status);
_PROPERTY_REFERENCE_CPP(NotoActiveSurfaceBossImpl,Management::ROTBoolean,m_penabled,enabled);
_PROPERTY_REFERENCE_CPP(NotoActiveSurfaceBossImpl,ActiveSurface::ROTASProfile,m_pprofile,pprofile);
_PROPERTY_REFERENCE_CPP(NotoActiveSurfaceBossImpl,Management::ROTBoolean,m_ptracking,tracking);

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(NotoActiveSurfaceBossImpl)

/*___oOo___*/
