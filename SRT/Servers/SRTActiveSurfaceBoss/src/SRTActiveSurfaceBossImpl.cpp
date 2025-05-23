// $Id: SRTActiveSurfaceBossImpl.cpp,v 1.2 2010-07-26 12:37:07 c.migoni Exp $

#include <new>
#include "SRTActiveSurfaceBossImpl.h"
#include "DevIOStatus.h"
#include "DevIOEnable.h"
#include "DevIOProfile.h"
#include "DevIOTracking.h"
#include "DevIOLUT.h"

static char const *rcsId="@(#) $Id: SRTActiveSurfaceBossImpl.cpp,v 1.2 2010-07-26 12:37:07 c.migoni Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

using namespace SimpleParser;

class SRTActiveSurfaceProfile2String {
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
            _EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"SRTActiveSurfaceProfile2String::strToVal()");
            throw ex;
        }
    }
};

SRTActiveSurfaceBossImpl::SRTActiveSurfaceBossImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices) :
    CharacteristicComponentImpl(CompName,containerServices),
    m_pstatus(this),
    m_penabled(this),
    m_pprofile(this),
    m_ptracking(this),
    m_pLUT_filename(this),
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
    ACS_LOG(LM_FULL_INFO,"SRTActiveSurfaceBossImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZING"));

    try
    {
        boss=(CSRTActiveSurfaceBossCore *)new CSRTActiveSurfaceBossCore(getContainerServices(),this);
        m_core=new IRA::CSecureArea<CSRTActiveSurfaceBossCore>(boss);
        m_pstatus=new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus>
            (getContainerServices()->getName()+":status",getComponent(),new SRTActiveSurfaceBossImplDevIOStatus(m_core),true);
        m_penabled=new ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>
            (getContainerServices()->getName()+":enabled",getComponent(),new SRTActiveSurfaceBossImplDevIOEnable(m_core),true);
        m_pprofile=new ROEnumImpl<ACS_ENUM_T(ActiveSurface::TASProfile),POA_ActiveSurface::ROTASProfile>
            (getContainerServices()->getName()+":pprofile",getComponent(),new SRTActiveSurfaceBossImplDevIOProfile(m_core),true);
        m_ptracking=new ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>
            (getContainerServices()->getName()+":tracking",getComponent(),new SRTActiveSurfaceBossImplDevIOTracking(m_core),true);
        m_pLUT_filename=new ROstring(getContainerServices()->getName()+":LUT_filename",getComponent(),new SRTActiveSurfaceBossImplDevIOLUT(m_core),true);

        // create the parser for command line execution
        m_parser = new SimpleParser::CParser<CSRTActiveSurfaceBossCore>(boss,10);
    }
    catch (std::bad_alloc& ex)
    {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"SRTActiveSurfaceBossImpl::initialize()");
        throw dummy;
    }
    boss->initialize();

    long workingThreadTime;
    if (CIRATools::getDBValue(cs,"profile",(long&)m_profile) && CIRATools::getDBValue(cs,"WorkingThreadTime",(long&)workingThreadTime))
    {
        ACS_SHORT_LOG((LM_INFO,"SRTActiveSurfaceBoss: CDB %d profile parameter read", m_profile));
        boss->m_profile = m_profile;
    }
    else
    {
        ACS_LOG(LM_SOURCE_INFO,"SRTActiveSurfaceBossImpl:initialize()",(LM_ERROR,"Error reading CDB!"));
        ASErrors::CDBAccessErrorExImpl exImpl(__FILE__,__LINE__,"USDImpl::initialize() - Error reading CDB parameters");
        throw acsErrTypeLifeCycle::LifeCycleExImpl(exImpl,__FILE__,__LINE__,"USDImpl::initialize()");
    }

    try
    {
        m_workingThread=getContainerServices()->getThreadManager()->create<CSRTActiveSurfaceBossWorkingThread,CSecureArea<CSRTActiveSurfaceBossCore> *>("SRTACTIVESURFACEBOSSWORKER",m_core);
        m_workingThread->setSleepTime(workingThreadTime*10);
    }
    catch (acsthreadErrType::acsthreadErrTypeExImpl& ex)
    {
        _ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"SRTActiveSurfaceBossImpl::initialize()");
        throw _dummy;
    }
    catch (...)
    {
        _THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"SRTActiveSurfaceBossImpl::initialize()");
    }

    try
    {
        m_initializationThread=getContainerServices()->getThreadManager()->create<CSRTActiveSurfaceBossInitializationThread,CSRTActiveSurfaceBossCore *>("SRTACTIVESURFACEBOSSINITTHREAD",boss);
    }
    catch (acsthreadErrType::acsthreadErrTypeExImpl& ex)
    {
        _ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"SRTActiveSurfaceBossImpl::initialize()");
        throw _dummy;
    }
    catch (...)
    {
        _THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"SRTActiveSurfaceBossImpl::initialize()");
    }

    // configure the parser.....
    m_parser->add("asSetup",new function1<CSRTActiveSurfaceBossCore,non_constant,void_type,I<enum_type<SRTActiveSurfaceProfile2String,ActiveSurface::TASProfile> > >(boss,&CSRTActiveSurfaceBossCore::setProfile),1);
    m_parser->add("asOn",new function0<CSRTActiveSurfaceBossCore,non_constant,void_type >(boss,&CSRTActiveSurfaceBossCore::asOn),0);
    m_parser->add("asOff",new function0<CSRTActiveSurfaceBossCore,non_constant,void_type >(boss,&CSRTActiveSurfaceBossCore::asOff),0);
    m_parser->add("asPark",new function0<CSRTActiveSurfaceBossCore,non_constant,void_type >(boss,&CSRTActiveSurfaceBossCore::asPark),0);
    m_parser->add("asSetLUT",new function1<CSRTActiveSurfaceBossCore,non_constant,void_type,I<string_type> >(boss,&CSRTActiveSurfaceBossCore::asSetLUT),1 );

    ACS_LOG(LM_FULL_INFO, "SRTActiveSurfaceBossImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZED"));
}

void SRTActiveSurfaceBossImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::execute()");

    try
    {
        boss->execute();
    }
    catch (ACSErr::ACSbaseExImpl& E)
    {
        _ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,_dummy,E,"SRTActiveSurfaceBossImpl::execute()");
        throw _dummy;
    }
    m_workingThread->resume();
    m_initializationThread->resume();

    ACS_LOG(LM_FULL_INFO,"SRTActiveSurfaceBossImpl::execute()",(LM_INFO,"SRTActiveSurfaceBossImpl::COMPSTATE_OPERATIONAL"));
}

void SRTActiveSurfaceBossImpl::cleanUp()
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::cleanUp()");
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
    ACS_LOG(LM_FULL_INFO,"SRTActiveSurfaceBossImpl::cleanUp()",(LM_INFO,"SRTActiveSurfaceBossImpl::THREADS_TERMINATED"));
    if (m_parser!=NULL) delete m_parser;
    ACS_LOG(LM_FULL_INFO,"SRTActiveSurfaceBossImpl::cleanUp()",(LM_INFO,"SRTActiveSurfaceBossImpl::PARSER_FREED"));
    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> core = m_core->Get();
    core->cleanUp();
    ACS_LOG(LM_FULL_INFO,"SRTActiveSurfaceBossImpl::cleanUp()",(LM_INFO,"SRTActiveSurfaceBossImpl::BOSS_CORE_FREED"));
    CharacteristicComponentImpl::cleanUp();
}

void SRTActiveSurfaceBossImpl::aboutToAbort()
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::aboutToAbort()");
    if (m_workingThread!=NULL)
    {
        m_workingThread->suspend();
        getContainerServices()->getThreadManager()->destroy(m_workingThread);
    }
    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore>  core=m_core->Get();
    core->cleanUp();
    if (m_parser!=NULL) delete m_parser;
}

void SRTActiveSurfaceBossImpl::stop (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::stop()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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

void SRTActiveSurfaceBossImpl::setup (const char *config) throw (CORBA::SystemException, ManagementErrors::ConfigurationErrorEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::setup()");
    IRA::CString strVal(config);
    strVal.MakeUpper();
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
        _EXCPT(ManagementErrors::ConfigurationErrorExImpl,ex,"SRTActiveSurfaceBossImpl::setup()");
        throw ex.getConfigurationErrorEx();
    }
}

void SRTActiveSurfaceBossImpl::park () throw (CORBA::SystemException, ManagementErrors::ParkingErrorEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::park()");
    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->asPark();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        _EXCPT(ManagementErrors::ParkingErrorExImpl,ex,"SRTActiveSurfaceBossImpl::park()");
        throw ex.getParkingErrorEx();
    }
    resource->m_tracking = false;
}

void SRTActiveSurfaceBossImpl::stow (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::stow()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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

void SRTActiveSurfaceBossImpl::refPos (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::refPos()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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

void SRTActiveSurfaceBossImpl::update (CORBA::Double elevation) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::update()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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

void SRTActiveSurfaceBossImpl::up (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::up()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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

void SRTActiveSurfaceBossImpl::down (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::down()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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

void SRTActiveSurfaceBossImpl::bottom (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::bottom()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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

void SRTActiveSurfaceBossImpl::top (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::top()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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

void SRTActiveSurfaceBossImpl::move (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius, CORBA::Long incr) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::move()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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

void SRTActiveSurfaceBossImpl::correction (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius, CORBA::Double correction) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::correction()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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

void SRTActiveSurfaceBossImpl::reset (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::reset()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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

void SRTActiveSurfaceBossImpl::setProfile (ActiveSurface::TASProfile newProfile) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::setProfile()");

    m_profile = newProfile;

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->setProfile(m_profile);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void SRTActiveSurfaceBossImpl::usdStatus4GUIClient (CORBA::Long circle, CORBA::Long actuator, CORBA::Long_out status) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::usdStatus4GUIClient()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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

void SRTActiveSurfaceBossImpl::asStatus4GUIClient (ACS::longSeq_out status) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::asStatus4GUIClient()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        status = new ACS::longSeq;
        resource->asStatus4GUIClient(*status);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void SRTActiveSurfaceBossImpl::setActuator (CORBA::Long circle, CORBA::Long actuator, CORBA::Long_out actPos, CORBA::Long_out cmdPos, CORBA::Long_out Fmin, CORBA::Long_out Fmax, CORBA::Long_out acc, CORBA::Long_out delay) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::setActuator");

    long int act, cmd, fmin, fmax, ac, del;

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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


void SRTActiveSurfaceBossImpl::calibrate (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::calibrate()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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

void SRTActiveSurfaceBossImpl::calVer (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::calibration verification()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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


void SRTActiveSurfaceBossImpl::recoverUSD (CORBA::Long circle, CORBA::Long actuator) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::recoverUSD()");

    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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

void SRTActiveSurfaceBossImpl::asOn() throw (CORBA::SystemException)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::asOn()");
    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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

void SRTActiveSurfaceBossImpl::asOff() throw (CORBA::SystemException)
{
    AUTO_TRACE("SRTActiveSurfaceBossImpl::asOff()");
    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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

CORBA::Boolean SRTActiveSurfaceBossImpl::command(const char *cmd,CORBA::String_out answer) throw (CORBA::SystemException)
{
    AUTO_TRACE("AntennaBossImpl::command()");
    IRA::CString out;
    bool res;
    CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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

_PROPERTY_REFERENCE_CPP(SRTActiveSurfaceBossImpl,Management::ROTSystemStatus,m_pstatus,status);
_PROPERTY_REFERENCE_CPP(SRTActiveSurfaceBossImpl,Management::ROTBoolean,m_penabled,enabled);
_PROPERTY_REFERENCE_CPP(SRTActiveSurfaceBossImpl,ActiveSurface::ROTASProfile,m_pprofile,pprofile);
_PROPERTY_REFERENCE_CPP(SRTActiveSurfaceBossImpl,Management::ROTBoolean,m_ptracking,tracking);
_PROPERTY_REFERENCE_CPP(SRTActiveSurfaceBossImpl,ACS::ROstring,m_pLUT_filename,LUT_filename);
/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SRTActiveSurfaceBossImpl)

/*___oOo___*/
