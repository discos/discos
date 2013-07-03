// $Id: SRTActiveSurfaceBossImpl.cpp,v 1.2 2010-07-26 12:37:07 c.migoni Exp $

#include <new>
#include "SRTActiveSurfaceBossImpl.h"
#include "DevIOStatus.h"
#include "DevIOEnable.h"

static char *rcsId="@(#) $Id: SRTActiveSurfaceBossImpl.cpp,v 1.2 2010-07-26 12:37:07 c.migoni Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

using namespace SimpleParser;

SRTActiveSurfaceBossImpl::SRTActiveSurfaceBossImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices) : 
	CharacteristicComponentImpl(CompName,containerServices),
    m_pstatus(this),
	m_penabled(this),
    m_core(NULL)
{	
	AUTO_TRACE("SRTActiveSurfaceBossImpl::SRTActiveSurfaceBossImpl()");
}

SRTActiveSurfaceBossImpl::~SRTActiveSurfaceBossImpl()
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::~SRTActiveSurfaceBossImpl()");
}

void SRTActiveSurfaceBossImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::initialize()");
    	cs = getContainerServices();
    	CSRTActiveSurfaceBossCore *boss;
    	ACS_LOG(LM_FULL_INFO,"SRTActiveSurfaceBossImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZING"));
	try {
		boss=(CSRTActiveSurfaceBossCore *)new CSRTActiveSurfaceBossCore(getContainerServices(),this);
		m_core=new IRA::CSecureArea<CSRTActiveSurfaceBossCore>(boss);
        	m_pstatus=new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus>
		  (getContainerServices()->getName()+":status",getComponent(),new SRTActiveSurfaceBossImplDevIOStatus(m_core),true);	
		m_penabled=new ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>
		  (getContainerServices()->getName()+":enabled",getComponent(),new SRTActiveSurfaceBossImplDevIOEnable(m_core),true);

        	// create the parser for command line execution
		m_parser =  new SimpleParser::CParser<CSRTActiveSurfaceBossCore>(boss,10);
    	}
    	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"SRTActiveSurfaceBossImpl::initialize()");
		throw dummy;
	}
    	boss->initialize();
    	try {
		m_watchingThread=getContainerServices()->getThreadManager()->create<CSRTActiveSurfaceBossWatchingThread,CSecureArea<CSRTActiveSurfaceBossCore> *>
		  ("SRTACTIVESURFACEBOSSWATCHER",m_core);
	}
	catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"SRTActiveSurfaceBossImpl::initialize()");
		throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"SRTActiveSurfaceBossImpl::initialize()");
	}

	try {
		m_workingThread=getContainerServices()->getThreadManager()->create<CSRTActiveSurfaceBossWorkingThread,CSecureArea<CSRTActiveSurfaceBossCore> *>
		  ("SRTACTIVESURFACEBOSSWORKER",m_core);
	}
	catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"SRTActiveSurfaceBossImpl::initialize()");
		throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"SRTActiveSurfaceBossImpl::initialize()");
	}

    	if (CIRATools::getDBValue(cs,"profile",(long&)m_profile))
	{
		ACS_SHORT_LOG((LM_INFO,"SRTActiveSurfaceBoss: CDB %d profile parameter read", m_profile));
	}
	else
	{
		ACS_LOG(LM_SOURCE_INFO,"SRTActiveSurfaceBossImpl:initialize()",(LM_ERROR,"Error reading CDB!"));
		ASErrors::CDBAccessErrorExImpl exImpl(__FILE__,__LINE__,"USDImpl::initialize() - Error reading CDB parameters");
		throw acsErrTypeLifeCycle::LifeCycleExImpl(exImpl,__FILE__,__LINE__,"USDImpl::initialize()");
	}

    	// configure the parser.....
	//m_parser->add("asSetup",new function1<CSRTActiveSurfaceBossCore,non_constant,void_type,I<enum_type<SRTActiveSurfaceProfile2String,SRTActiveSurface::TASProfile> > >(boss,&CSRTActiveSurfaceBossCore::setProfile),1);
	m_parser->add("asOn",new function0<CSRTActiveSurfaceBossCore,non_constant,void_type >(boss,&CSRTActiveSurfaceBossCore::asOn),0);
	m_parser->add("asOff",new function0<CSRTActiveSurfaceBossCore,non_constant,void_type >(boss,&CSRTActiveSurfaceBossCore::asOff),0);
	m_parser->add("asPark",new function0<CSRTActiveSurfaceBossCore,non_constant,void_type >(boss,&CSRTActiveSurfaceBossCore::asPark),0);

    ACS_LOG(LM_FULL_INFO, "SRTActiveSurfaceBossImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZED"));
}

void SRTActiveSurfaceBossImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::execute()");

	CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore>  core=m_core->Get();
    	core->m_profile = m_profile;

    	try {
		core->execute();
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,_dummy,E,"SRTActiveSurfaceBossImpl::execute()");
		throw _dummy;
	}
	//starts the loop status thread....
	m_watchingThread->resume();
	m_watchingThread->setSleepTime(LOOPSTATUSTIME);

	//starts the loop working thread....
	m_workingThread->resume();
	m_workingThread->setSleepTime(LOOPWORKINGTIME);

	ACS_LOG(LM_FULL_INFO,"SRTActiveSurfaceBossImpl::execute()",(LM_INFO,"SRTActiveSurfaceBossImpl::COMPSTATE_OPERATIONAL"));
}

void SRTActiveSurfaceBossImpl::cleanUp()
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::cleanUp()");
    	CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore>  core=m_core->Get();
   	if (m_workingThread!=NULL) m_workingThread->suspend();
   	if (m_watchingThread!=NULL) m_watchingThread->suspend();
    	getContainerServices()->getThreadManager()->destroy(m_workingThread);
    	getContainerServices()->getThreadManager()->destroy(m_watchingThread);
    	ACS_LOG(LM_FULL_INFO,"SRTActiveSurfaceBossImpl::cleanUp()",(LM_INFO,"SRTActiveSurfaceBossImpl::THREADS_TERMINATED"));
	core->cleanUp();
	ACS_LOG(LM_FULL_INFO,"SRTActiveSurfaceBossImpl::cleanUp()",(LM_INFO,"SRTActiveSurfaceBossImpl::BOSS_CORE_FREED"));
    	if (m_parser!=NULL) delete m_parser;
	ACS_LOG(LM_FULL_INFO,"SRTActiveSurfaceBossImpl::cleanUp()",(LM_INFO,"SRTActiveSurfaceBossImpl::PARSER_FREED"));
	CharacteristicComponentImpl::cleanUp();
}

void SRTActiveSurfaceBossImpl::aboutToAbort()
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::aboutToAbort()");
    	if (m_workingThread!=NULL) m_workingThread->suspend();
    	if (m_watchingThread!=NULL) m_watchingThread->suspend();
    	getContainerServices()->getThreadManager()->destroy(m_workingThread);
    	getContainerServices()->getThreadManager()->destroy(m_watchingThread);
	CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore>  core=m_core->Get(); 
	core->cleanUp();
    	if (m_parser!=NULL) delete m_parser;
}

void SRTActiveSurfaceBossImpl::stop (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::stop()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
    try {
        resource->onewayAction(SRTActiveSurface::AS_STOP, circle, actuator, radius, 0, 0, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void SRTActiveSurfaceBossImpl::setup (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::setup()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
    try {
        resource->onewayAction(SRTActiveSurface::AS_SETUP, circle, actuator, radius, 0, 0, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void SRTActiveSurfaceBossImpl::stow (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::stow()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
    try {
        resource->onewayAction(SRTActiveSurface::AS_STOW, circle, actuator, radius, 0, 0, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void SRTActiveSurfaceBossImpl::refPos (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::refPos()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
    try {
        resource->onewayAction(SRTActiveSurface::AS_REFPOS, circle, actuator, radius, 0, 0, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void SRTActiveSurfaceBossImpl::update (CORBA::Double elevation) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::update()");

    	CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();

	try {
        	resource->onewayAction(SRTActiveSurface::AS_UPDATE, 0, 0, 0, elevation, 0, 0, m_profile);
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void SRTActiveSurfaceBossImpl::up (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::up()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
    try {
        resource->onewayAction(SRTActiveSurface::AS_UP, circle, actuator, radius, 0, 0, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void SRTActiveSurfaceBossImpl::down (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::down()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
    try {
        resource->onewayAction(SRTActiveSurface::AS_DOWN, circle, actuator, radius, 0, 0, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void SRTActiveSurfaceBossImpl::bottom (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::bottom()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
    try {
        resource->onewayAction(SRTActiveSurface::AS_BOTTOM, circle, actuator, radius, 0, 0, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void SRTActiveSurfaceBossImpl::top (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::top()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
    try {
        resource->onewayAction(SRTActiveSurface::AS_TOP, circle, actuator, radius, 0, 0, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void SRTActiveSurfaceBossImpl::move (CORBA::Long circle,  CORBA::Long actuator,  CORBA::Long radius, CORBA::Long incr) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::move()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
    try {
        resource->onewayAction(SRTActiveSurface::AS_MOVE, circle, actuator, radius, 0, 0, incr, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void SRTActiveSurfaceBossImpl::correction (CORBA::Long circle,  CORBA::Long actuator,  CORBA::Long radius, CORBA::Double correction) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::correction()");

	CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
	try {
		resource->onewayAction(SRTActiveSurface::AS_CORRECTION, circle, actuator, radius, 0, correction, 0, m_profile);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void SRTActiveSurfaceBossImpl::reset (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::reset()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
    try {
        resource->reset(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void SRTActiveSurfaceBossImpl::setProfile (SRTActiveSurface::TASProfile newProfile) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::setProfile()");

	m_profile = newProfile;

	//_SET_CDB(profile, m_profile,"SRTActiveSurfaceBossImpl::setProfile")

	CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
	try {
		//resource->onewayAction(SRTActiveSurface::AS_PROFILE, 0, 0, 0, 0, 0, 0, m_profile);
		resource->setProfile(m_profile);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
	
}

void SRTActiveSurfaceBossImpl::usdStatus4GUIClient (CORBA::Long circle, CORBA::Long actuator, CORBA::Long_out status) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::usdStatus4GUIClient()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
    try {
        resource->usdStatus4GUIClient(circle, actuator, status);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void SRTActiveSurfaceBossImpl::setActuator (CORBA::Long circle, CORBA::Long actuator, CORBA::Long_out actPos, CORBA::Long_out cmdPos, CORBA::Long_out Fmin, CORBA::Long_out Fmax, CORBA::Long_out acc, CORBA::Long_out delay) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::setActuator");

    long int act, cmd, fmin, fmax, ac, del;

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
    try {
        resource->setActuator(circle, actuator, act, cmd, fmin, fmax, ac, del);
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


void SRTActiveSurfaceBossImpl::calibrate (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::calibrate()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
    try {
        resource->calibrate(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void SRTActiveSurfaceBossImpl::calVer (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::calibration verification()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
    try {
        resource->calVer(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}


void SRTActiveSurfaceBossImpl::recoverUSD (CORBA::Long circle, CORBA::Long actuator) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SRTActiveSurfaceBossImpl::recoverUSD()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
    try {
        //resource->recoverUSD(circle, actuator); TBC!!
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void SRTActiveSurfaceBossImpl::enableAutoUpdate() throw (CORBA::SystemException)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::enableAutoUpdate()");
	CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
	resource->enableAutoUpdate();
}

void SRTActiveSurfaceBossImpl::disableAutoUpdate() throw (CORBA::SystemException)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::disable()");
	CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
	resource->disableAutoUpdate();
}

CORBA::Boolean SRTActiveSurfaceBossImpl::command(const char *cmd,CORBA::String_out answer) throw (CORBA::SystemException)
//char *SRTActiveSurfaceBossImpl::command(const char *cmd) throw (CORBA::SystemException, ManagementErrors::CommandLineErrorEx)
{
	AUTO_TRACE("AntennaBossImpl::command()");
	IRA::CString out;
	bool res;
	//IRA::CString in;
	CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
	//in=IRA::CString(cmd);
	try {
		m_parser->run(cmd,out);
		res = true;
	}
	catch (ParserErrors::ParserErrorsExImpl &ex) {
		/*_ADD_BACKTRACE(ManagementErrors::CommandLineErrorExImpl,impl,ex,"SRTActiveSurfaceBossImpl::command()");
		impl.setCommand(cmd);
		impl.setErrorMessage((const char *)out);
		impl.log(LM_DEBUG);
		throw impl.getCommandLineErrorEx();*/
		res = false;
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		ex.log(LM_ERROR); // the errors resulting from the execution are logged here as stated in the documentation of CommandInterpreter interface, while the parser errors are never logged.
		res=false;
	}
	answer=CORBA::string_dup((const char *)out);
	return res;
	//return CORBA::string_dup((const char *)out);
}

_PROPERTY_REFERENCE_CPP(SRTActiveSurfaceBossImpl,Management::ROTSystemStatus,m_pstatus,status);
_PROPERTY_REFERENCE_CPP(SRTActiveSurfaceBossImpl,Management::ROTBoolean,m_penabled,enabled);
/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SRTActiveSurfaceBossImpl)

/*___oOo___*/
