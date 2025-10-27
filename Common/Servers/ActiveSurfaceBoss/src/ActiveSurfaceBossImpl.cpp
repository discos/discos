// $Id: ActiveSurfaceBossImpl.cpp,v 1.2 2010-07-26 12:37:07 c.migoni Exp $

#include <new>
#include "ActiveSurfaceBossImpl.h"
#include "DevIOStatus.h"
#include "DevIOEnable.h"
#include "DevIOProfile.h"
#include "DevIOTracking.h"
#include "DevIOLUT.h"

_IRA_LOGFILTER_DECLARE;

static char const *rcsId="@(#) $Id: ActiveSurfaceBossImpl.cpp,v 1.2 2010-07-26 12:37:07 c.migoni Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

using namespace SimpleParser;

ActiveSurfaceBossImpl::ActiveSurfaceBossImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices) :
    CharacteristicComponentImpl(CompName,containerServices),
    m_pstatus(this),
    m_penabled(this),
    m_pprofile(this),
    m_ptracking(this),
    m_pLUT_filename(this),
    m_core(NULL)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::ActiveSurfaceBossImpl()");
}

ActiveSurfaceBossImpl::~ActiveSurfaceBossImpl()
{
    AUTO_TRACE("ActiveSurfaceBossImpl::~ActiveSurfaceBossImpl()");
}

void ActiveSurfaceBossImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::initialize()");

    _IRA_LOGFILTER_ACTIVATE(10000000, 20000000);

    cs = getContainerServices();
    ACS_LOG(LM_FULL_INFO,"ActiveSurfaceBossImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZING"));

    try
    {
        boss=(CActiveSurfaceBossCore *)new CActiveSurfaceBossCore(getContainerServices(),this);
        m_core=new IRA::CSecureArea<CActiveSurfaceBossCore>(boss);
        m_pstatus=new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus>
            (getContainerServices()->getName()+":status",getComponent(),new ActiveSurfaceBossImplDevIOStatus(m_core),true);
        m_penabled=new ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>
            (getContainerServices()->getName()+":enabled",getComponent(),new ActiveSurfaceBossImplDevIOEnable(m_core),true);
        m_pprofile=new ROEnumImpl<ACS_ENUM_T(ActiveSurface::TASProfile),POA_ActiveSurface::ROTASProfile>
            (getContainerServices()->getName()+":pprofile",getComponent(),new ActiveSurfaceBossImplDevIOProfile(m_core),true);
        m_ptracking=new ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>
            (getContainerServices()->getName()+":tracking",getComponent(),new ActiveSurfaceBossImplDevIOTracking(m_core),true);
        m_pLUT_filename=new ROstring(getContainerServices()->getName()+":LUT_filename",getComponent(),new ActiveSurfaceBossImplDevIOLUT(m_core),true);

        // create the parser for command line execution
        m_parser = new SimpleParser::CParser<CActiveSurfaceBossCore>(boss,10);
    }
    catch (std::bad_alloc& ex)
    {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"ActiveSurfaceBossImpl::initialize()");
        throw dummy;
    }
    boss->initialize();

    long workingThreadTime;
    if(!CIRATools::getDBValue(cs, "profile", (long&)m_profile) ||
       !CIRATools::getDBValue(cs, "WorkingThreadTime", (long&)workingThreadTime))
    {
        ACS_LOG(LM_SOURCE_INFO,"ActiveSurfaceBossImpl:initialize()",(LM_ERROR,"Error reading CDB!"));
        ASErrors::CDBAccessErrorExImpl exImpl(__FILE__,__LINE__,"USDImpl::initialize() - Error reading CDB parameters");
        throw acsErrTypeLifeCycle::LifeCycleExImpl(exImpl,__FILE__,__LINE__,"USDImpl::initialize()");
    }

    if(boss->validProfile(m_profile))
    {
        ACS_SHORT_LOG((LM_INFO,"ActiveSurfaceBoss: CDB %d profile parameter read", m_profile));
        boss->m_profile = m_profile;
    }
    else
    {
        ACS_LOG(LM_SOURCE_INFO,"ActiveSurfaceBossImpl:initialize()",(LM_ERROR,"Profile unknown or not accepted"));
        ASErrors::UnknownProfileExImpl exImpl(__FILE__,__LINE__,"ActiveSurfaceBossImpl::initialize() - Profile unknown or not accepted");
        throw acsErrTypeLifeCycle::LifeCycleExImpl(exImpl,__FILE__,__LINE__,"ActiveSurfaceBossImpl::initialize()");
    }

    try
    {
        m_workingThread=getContainerServices()->getThreadManager()->create<CActiveSurfaceBossWorkingThread,CSecureArea<CActiveSurfaceBossCore> *>("ACTIVESURFACEBOSSWORKER",m_core);
        m_workingThread->setSleepTime(workingThreadTime*10);
    }
    catch (acsthreadErrType::acsthreadErrTypeExImpl& ex)
    {
        _ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"ActiveSurfaceBossImpl::initialize()");
        throw _dummy;
    }
    catch (...)
    {
        _THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"ActiveSurfaceBossImpl::initialize()");
    }

    try
    {
        m_initializationThread=getContainerServices()->getThreadManager()->create<CActiveSurfaceBossInitializationThread,CActiveSurfaceBossCore *>("ACTIVESURFACEBOSSINITTHREAD",boss);
    }
    catch (acsthreadErrType::acsthreadErrTypeExImpl& ex)
    {
        _ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"ActiveSurfaceBossImpl::initialize()");
        throw _dummy;
    }
    catch (...)
    {
        _THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"ActiveSurfaceBossImpl::initialize()");
    }

    // configure the parser.....
    m_parser->add("asSetup",new function1<CActiveSurfaceBossCore,non_constant,void_type,I<enum_type<ActiveSurfaceProfile2String,ActiveSurface::TASProfile> > >(boss,&CActiveSurfaceBossCore::setProfile),1);
    m_parser->add("asOn",new function0<CActiveSurfaceBossCore,non_constant,void_type >(boss,&CActiveSurfaceBossCore::asOn),0);
    m_parser->add("asOff",new function0<CActiveSurfaceBossCore,non_constant,void_type >(boss,&CActiveSurfaceBossCore::asOff),0);
    m_parser->add("asPark",new function0<CActiveSurfaceBossCore,non_constant,void_type >(boss,&CActiveSurfaceBossCore::asPark),0);
    m_parser->add("asSetLUT",new function1<CActiveSurfaceBossCore,non_constant,void_type,I<string_type> >(boss,&CActiveSurfaceBossCore::asSetLUT),1 );

    ACS_LOG(LM_FULL_INFO, "ActiveSurfaceBossImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZED"));
}

void ActiveSurfaceBossImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::execute()");

    try
    {
        boss->execute();
    }
    catch (ACSErr::ACSbaseExImpl& E)
    {
        _ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,_dummy,E,"ActiveSurfaceBossImpl::execute()");
        throw _dummy;
    }
    m_workingThread->resume();
    m_initializationThread->resume();

    ACS_LOG(LM_FULL_INFO,"ActiveSurfaceBossImpl::execute()",(LM_INFO,"ActiveSurfaceBossImpl::COMPSTATE_OPERATIONAL"));
}

void ActiveSurfaceBossImpl::cleanUp()
{
    AUTO_TRACE("ActiveSurfaceBossImpl::cleanUp()");
    if (m_workingThread!=NULL)
    {
        m_workingThread->suspend();
        getContainerServices()->getThreadManager()->destroy(m_workingThread);
    }
    if (m_initializationThread!=NULL)
    {
        m_initializationThread->suspend();
        getContainerServices()->getThreadManager()->destroy(m_initializationThread);
    }
    ACS_LOG(LM_FULL_INFO,"ActiveSurfaceBossImpl::cleanUp()",(LM_INFO,"ActiveSurfaceBossImpl::THREADS_TERMINATED"));
    if (m_parser!=NULL) delete m_parser;
    ACS_LOG(LM_FULL_INFO,"ActiveSurfaceBossImpl::cleanUp()",(LM_INFO,"ActiveSurfaceBossImpl::PARSER_FREED"));
    CSecAreaResourceWrapper<CActiveSurfaceBossCore> core = m_core->Get();
    core->cleanUp();
    ACS_LOG(LM_FULL_INFO,"ActiveSurfaceBossImpl::cleanUp()",(LM_INFO,"ActiveSurfaceBossImpl::BOSS_CORE_FREED"));
    _IRA_LOGFILTER_FLUSH;
    _IRA_LOGFILTER_DESTROY;
    CharacteristicComponentImpl::cleanUp();
}

void ActiveSurfaceBossImpl::aboutToAbort()
{
    AUTO_TRACE("ActiveSurfaceBossImpl::aboutToAbort()");
    if (m_workingThread!=NULL)
    {
        m_workingThread->suspend();
        getContainerServices()->getThreadManager()->destroy(m_workingThread);
    }
    CSecAreaResourceWrapper<CActiveSurfaceBossCore>  core=m_core->Get();
    core->cleanUp();
    if (m_parser!=NULL) delete m_parser;
}

void ActiveSurfaceBossImpl::stop (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::stop()");

    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->onewayAction(ActiveSurface::AS_STOP, circle, actuator, radius, 0, 0, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void ActiveSurfaceBossImpl::setup (const char *config) throw (CORBA::SystemException, ManagementErrors::ConfigurationErrorEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::setup()");
    IRA::CString strVal(config);
    strVal.MakeUpper();
    try {
        if (strVal=="S") {
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
            _EXCPT(ManagementErrors::ConfigurationErrorExImpl, ex, "ActiveSurfaceBossImpl::setup()");
            throw ex.getConfigurationErrorEx();
        }
    }
    catch (ASErrors::UnknownProfileEx& ex) {
        _ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl, impl, ex, "ActiveSurfaceBossImpl::setup()");
        impl.setSubsystem("ActiveSurfaceBoss");
        throw impl.getConfigurationErrorEx();
    }
}

void ActiveSurfaceBossImpl::park () throw (CORBA::SystemException, ManagementErrors::ParkingErrorEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::park()");
    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->asPark();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        _EXCPT(ManagementErrors::ParkingErrorExImpl,ex,"ActiveSurfaceBossImpl::park()");
        throw ex.getParkingErrorEx();
    }
    resource->m_tracking = false;
}

void ActiveSurfaceBossImpl::stow (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::stow()");

    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->onewayAction(ActiveSurface::AS_STOW, circle, actuator, radius, 0, 0, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void ActiveSurfaceBossImpl::refPos (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::refPos()");

    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->onewayAction(ActiveSurface::AS_REFPOS, circle, actuator, radius, 0, 0, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void ActiveSurfaceBossImpl::update (CORBA::Double elevation) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::update()");

    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    if(resource->m_profileSetted)
    {
        try
        {
            resource->onewayAction(ActiveSurface::AS_UPDATE, 0, 0, 0, elevation, 0, 0, m_profile);
        }
        catch (ComponentErrors::ComponentErrorsExImpl& ex)
        {
            ex.log(LM_DEBUG);
            throw ex.getComponentErrorsEx();
        }
    }
}

void ActiveSurfaceBossImpl::up (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::up()");

    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->onewayAction(ActiveSurface::AS_UP, circle, actuator, radius, 0, 0, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void ActiveSurfaceBossImpl::down (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::down()");

    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->onewayAction(ActiveSurface::AS_DOWN, circle, actuator, radius, 0, 0, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void ActiveSurfaceBossImpl::bottom (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::bottom()");

    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->onewayAction(ActiveSurface::AS_BOTTOM, circle, actuator, radius, 0, 0, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void ActiveSurfaceBossImpl::top (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::top()");

    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->onewayAction(ActiveSurface::AS_TOP, circle, actuator, radius, 0, 0, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void ActiveSurfaceBossImpl::move (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius, CORBA::Long incr) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::move()");

    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->onewayAction(ActiveSurface::AS_MOVE, circle, actuator, radius, 0, 0, incr, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void ActiveSurfaceBossImpl::correction (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius, CORBA::Double correction) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::correction()");

    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->onewayAction(ActiveSurface::AS_CORRECTION, circle, actuator, radius, 0, correction, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void ActiveSurfaceBossImpl::reset (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::reset()");

    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->onewayAction(ActiveSurface::AS_RESET, circle, actuator, radius, 0, 0, 0, m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void ActiveSurfaceBossImpl::setProfile (ActiveSurface::TASProfile newProfile) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, ASErrors::UnknownProfileEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::setProfile()");

    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->setProfile(newProfile);
        m_profile = newProfile;
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
    catch (ASErrors::UnknownProfileExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getUnknownProfileEx();
    }
}

void ActiveSurfaceBossImpl::usdStatus4GUIClient (CORBA::Long circle, CORBA::Long actuator, CORBA::Long_out status) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::usdStatus4GUIClient()");

    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->usdStatus4GUIClient(circle, actuator, status);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void ActiveSurfaceBossImpl::asStatus4GUIClient (ACS::longSeq_out status) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::asStatus4GUIClient()");

    try
    {
        status = new ACS::longSeq;
        status->length(boss->lastUSD);
        unsigned int i = 0;

        CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource = m_core->Get();
        auto& usdStatusMap = resource->usdStatusMap;
        resource.Release();

        for (int circle = 1; circle <= boss->CIRCLES; circle++)
        {
            for (int actuator = 1; actuator <= boss->actuatorsInCircle[circle]; actuator++)
            {
                // Initialize the status word as component unavailable. If the component is available it will be overwritten
                int usdStatus = UNAV;

                if(!CORBA::is_nil(boss->usd[circle][actuator]))
                {
                    usdStatus = usdStatusMap[boss->usd[circle][actuator]->name()].status;
                }

                status[i++] = usdStatus;
            }
        }
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void ActiveSurfaceBossImpl::setActuator (CORBA::Long circle, CORBA::Long actuator, CORBA::Long_out actPos, CORBA::Long_out cmdPos, CORBA::Long_out Fmin, CORBA::Long_out Fmax, CORBA::Long_out acc, CORBA::Long_out delay) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::setActuator");

    long int act, cmd, fmin, fmax, ac, del;

    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->setActuator(circle, actuator, act, cmd, fmin, fmax, ac, del);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
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


void ActiveSurfaceBossImpl::calibrate (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::calibrate()");

    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->calibrate(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void ActiveSurfaceBossImpl::calVer (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::calibration verification()");

    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->calVer(circle, actuator, radius);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}


void ActiveSurfaceBossImpl::recoverUSD (CORBA::Long circle, CORBA::Long actuator) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::recoverUSD()");

    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        //resource->recoverUSD(circle, actuator); TBC!!
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void ActiveSurfaceBossImpl::asOn() throw (CORBA::SystemException)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::asOn()");
    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->asOn();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void ActiveSurfaceBossImpl::asOff() throw (CORBA::SystemException)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::asOff()");
    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->asOff();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

CORBA::Boolean ActiveSurfaceBossImpl::command(const char *cmd,CORBA::String_out answer) throw (CORBA::SystemException)
{
    AUTO_TRACE("AntennaBossImpl::command()");
    IRA::CString out;
    bool res;
    CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        m_parser->run(cmd,out);
        res = true;
    }
    catch (ParserErrors::ParserErrorsExImpl &ex)
    {
        res = false;
    }
    catch (ACSErr::ACSbaseExImpl& ex)
    {
        ex.log(LM_ERROR); // the errors resulting from the execution are logged here as stated in the documentation of CommandInterpreter interface, while the parser errors are never logged.
        res=false;
    }
    answer=CORBA::string_dup((const char *)out);
    return res;
}

_PROPERTY_REFERENCE_CPP(ActiveSurfaceBossImpl,Management::ROTSystemStatus,m_pstatus,status);
_PROPERTY_REFERENCE_CPP(ActiveSurfaceBossImpl,Management::ROTBoolean,m_penabled,enabled);
_PROPERTY_REFERENCE_CPP(ActiveSurfaceBossImpl,ActiveSurface::ROTASProfile,m_pprofile,pprofile);
_PROPERTY_REFERENCE_CPP(ActiveSurfaceBossImpl,Management::ROTBoolean,m_ptracking,tracking);
_PROPERTY_REFERENCE_CPP(ActiveSurfaceBossImpl,ACS::ROstring,m_pLUT_filename,LUT_filename);
/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(ActiveSurfaceBossImpl)

/*___oOo___*/
