// $Id: MedicinaActiveSurfaceBossImpl.cpp,v 1.2 2010-07-26 12:37:07 c.migoni Exp $

#include <new>
#include "MedicinaActiveSurfaceBossImpl.h"
#include "DevIOStatus.h"
#include "DevIOEnable.h"
#include "DevIOProfile.h"
#include "DevIOTracking.h"

static char const *rcsId="@(#) $Id: MedicinaActiveSurfaceBossImpl.cpp,v 1.2 2010-07-26 12:37:07 c.migoni Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

using namespace SimpleParser;

class MedicinaActiveSurfaceProfile2String {
public:
    static char *valToStr(const ActiveSurface::TASProfile& val) {
        char *c=new char[16];
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
        if (strVal=="P") {
            return ActiveSurface::AS_PARABOLIC;
        }
        else if (strVal=="PF") {
            return ActiveSurface::AS_PARABOLIC_FIXED;
        }
        else {
            _EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"MedicinaActiveSurfaceProfile2String::strToVal()");
            throw ex;
        }
    }
};

MedicinaActiveSurfaceBossImpl::MedicinaActiveSurfaceBossImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices) :
    CharacteristicComponentImpl(CompName,containerServices),
    m_pstatus(this),
    m_penabled(this),
    m_pprofile(this),
    m_ptracking(this),
    m_core(NULL)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::MedicinaActiveSurfaceBossImpl()");
}

MedicinaActiveSurfaceBossImpl::~MedicinaActiveSurfaceBossImpl()
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::~MedicinaActiveSurfaceBossImpl()");
}

void MedicinaActiveSurfaceBossImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::initialize()");
    cs = getContainerServices();
    ACS_LOG(LM_FULL_INFO,"MedicinaActiveSurfaceBossImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZING"));

    try
    {
        boss=(CMedicinaActiveSurfaceBossCore *)new CMedicinaActiveSurfaceBossCore(getContainerServices(),this);
        m_core=new IRA::CSecureArea<CMedicinaActiveSurfaceBossCore>(boss);
        m_pstatus=new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus>
            (getContainerServices()->getName()+":status",getComponent(),new MedicinaActiveSurfaceBossImplDevIOStatus(m_core),true);
        m_penabled=new ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>
            (getContainerServices()->getName()+":enabled",getComponent(),new MedicinaActiveSurfaceBossImplDevIOEnable(m_core),true);
        m_pprofile=new ROEnumImpl<ACS_ENUM_T(ActiveSurface::TASProfile),POA_ActiveSurface::ROTASProfile>
            (getContainerServices()->getName()+":pprofile",getComponent(),new MedicinaActiveSurfaceBossImplDevIOProfile(m_core),true);
        m_ptracking=new ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>
            (getContainerServices()->getName()+":tracking",getComponent(),new MedicinaActiveSurfaceBossImplDevIOTracking(m_core),true);

        // create the parser for command line execution
        m_parser = new SimpleParser::CParser<CMedicinaActiveSurfaceBossCore>(boss,10);
    }
    catch (std::bad_alloc& ex)
    {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"MedicinaActiveSurfaceBossImpl::initialize()");
        throw dummy;
    }
    boss->initialize();

    long workingThreadTime;
    if (CIRATools::getDBValue(cs,"profile",(long&)m_profile) && CIRATools::getDBValue(cs,"WorkingThreadTime",(long&)workingThreadTime))
    {
        ACS_SHORT_LOG((LM_INFO,"MedicinaActiveSurfaceBoss: CDB %d profile parameter read", m_profile));
        boss->m_profile = m_profile;
    }
    else
    {
        ACS_LOG(LM_SOURCE_INFO,"MedicinaActiveSurfaceBossImpl:initialize()",(LM_ERROR,"Error reading CDB!"));
        ASErrors::CDBAccessErrorExImpl exImpl(__FILE__,__LINE__,"USDImpl::initialize() - Error reading CDB parameters");
        throw acsErrTypeLifeCycle::LifeCycleExImpl(exImpl,__FILE__,__LINE__,"USDImpl::initialize()");
    }

    try
    {
        m_workingThread=getContainerServices()->getThreadManager()->create<CMedicinaActiveSurfaceBossWorkingThread,CSecureArea<CMedicinaActiveSurfaceBossCore> *>("MEDICINAACTIVESURFACEBOSSWORKER",m_core);
        m_workingThread->setSleepTime(workingThreadTime*10);
    }
    catch (acsthreadErrType::acsthreadErrTypeExImpl& ex)
    {
        _ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"MedicinaActiveSurfaceBossImpl::initialize()");
        throw _dummy;
    }
    catch (...)
    {
        _THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"MedicinaActiveSurfaceBossImpl::initialize()");
    }

    for(int sector = 0; sector < SECTORS; sector++)
    {
        std::stringstream threadName;
        threadName << "MEDICINAACTIVESURFACEBOSSSECTOR";
        threadName << sector+1;
        try
        {
            CMedicinaActiveSurfaceBossSectorThread* sectorThread = getContainerServices()->getThreadManager()->create<CMedicinaActiveSurfaceBossSectorThread,CMedicinaActiveSurfaceBossCore *> (threadName.str().c_str(), boss);
            m_sectorThread.push_back(sectorThread);
        }
        catch (acsthreadErrType::acsthreadErrTypeExImpl& ex)
        {
            _ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"MedicinaActiveSurfaceBossImpl::initialize()");
            throw _dummy;
        }
        catch (...)
        {
            _THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"MedicinaActiveSurfaceBossImpl::initialize()");
        }
    }

    // configure the parser.....
    m_parser->add("asSetup",new function1<CMedicinaActiveSurfaceBossCore,non_constant,void_type,I<enum_type<MedicinaActiveSurfaceProfile2String,ActiveSurface::TASProfile> > >(boss,&CMedicinaActiveSurfaceBossCore::setProfile),1);
    m_parser->add("asOn",new function0<CMedicinaActiveSurfaceBossCore,non_constant,void_type >(boss,&CMedicinaActiveSurfaceBossCore::asOn),0);
    m_parser->add("asOff",new function0<CMedicinaActiveSurfaceBossCore,non_constant,void_type >(boss,&CMedicinaActiveSurfaceBossCore::asOff),0);
    m_parser->add("asPark",new function0<CMedicinaActiveSurfaceBossCore,non_constant,void_type >(boss,&CMedicinaActiveSurfaceBossCore::asPark),0);

    ACS_LOG(LM_FULL_INFO, "MedicinaActiveSurfaceBossImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZED"));
}

void MedicinaActiveSurfaceBossImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::execute()");

    try
    {
        boss->execute();
    }
    catch (ACSErr::ACSbaseExImpl& E)
    {
        _ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,_dummy,E,"MedicinaActiveSurfaceBossImpl::execute()");
        throw _dummy;
    }
    m_workingThread->resume();

    for(unsigned int i = 0; i < m_sectorThread.size(); i++)
    {
        m_sectorThread[i]->setSleepTime(SECTORTIME);
        m_sectorThread[i]->resume();
    }
    ACS_LOG(LM_FULL_INFO,"MedicinaActiveSurfaceBossImpl::execute()",(LM_INFO,"MedicinaActiveSurfaceBossImpl::COMPSTATE_OPERATIONAL"));
}

void MedicinaActiveSurfaceBossImpl::cleanUp()
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::cleanUp()");
    if (m_workingThread!=NULL)
    {
        m_workingThread->suspend();
        getContainerServices()->getThreadManager()->destroy(m_workingThread);
    }
    for(unsigned int i = 0; i < m_sectorThread.size(); i++)
    {
        if(m_sectorThread[i] != NULL)
        {
            m_sectorThread[i]->suspend();
            getContainerServices()->getThreadManager()->destroy(m_sectorThread[i]);
        }
    }
    ACS_LOG(LM_FULL_INFO,"MedicinaActiveSurfaceBossImpl::cleanUp()",(LM_INFO,"MedicinaActiveSurfaceBossImpl::THREADS_TERMINATED"));
    if (m_parser!=NULL) delete m_parser;
    ACS_LOG(LM_FULL_INFO,"MedicinaActiveSurfaceBossImpl::cleanUp()",(LM_INFO,"MedicinaActiveSurfaceBossImpl::PARSER_FREED"));
    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> core = m_core->Get();
    core->cleanUp();
    ACS_LOG(LM_FULL_INFO,"MedicinaActiveSurfaceBossImpl::cleanUp()",(LM_INFO,"MedicinaActiveSurfaceBossImpl::BOSS_CORE_FREED"));
    CharacteristicComponentImpl::cleanUp();
}

void MedicinaActiveSurfaceBossImpl::aboutToAbort()
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::aboutToAbort()");
    if (m_workingThread!=NULL)
    {
        m_workingThread->suspend();
        getContainerServices()->getThreadManager()->destroy(m_workingThread);
    }
    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore>  core=m_core->Get();
    core->cleanUp();
    if (m_parser!=NULL) delete m_parser;
}

void MedicinaActiveSurfaceBossImpl::stop (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::stop()");

    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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

void MedicinaActiveSurfaceBossImpl::setup (const char *config) throw (CORBA::SystemException, ManagementErrors::ConfigurationErrorEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::setup()");
    IRA::CString strVal(config);
    strVal.MakeUpper();
    if (strVal=="P") {
        setProfile(ActiveSurface::AS_PARABOLIC);
    }
    else if (strVal=="PF") {
        setProfile(ActiveSurface::AS_PARABOLIC_FIXED);
    }
    else {
        _EXCPT(ManagementErrors::ConfigurationErrorExImpl,ex,"MedicinaActiveSurfaceBossImpl::setup()");
        throw ex.getConfigurationErrorEx();
    }
}

void MedicinaActiveSurfaceBossImpl::park () throw (CORBA::SystemException, ManagementErrors::ParkingErrorEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::park()");
    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
    try
    {
        resource->asPark();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        _EXCPT(ManagementErrors::ParkingErrorExImpl,ex,"MedicinaActiveSurfaceBossImpl::park()");
        throw ex.getParkingErrorEx();
    }
    resource->m_tracking = false;
}

void MedicinaActiveSurfaceBossImpl::stow (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::stow()");

    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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

void MedicinaActiveSurfaceBossImpl::refPos (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::refPos()");

    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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

void MedicinaActiveSurfaceBossImpl::update (CORBA::Double elevation) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::update()");

    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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

void MedicinaActiveSurfaceBossImpl::up (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::up()");

    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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

void MedicinaActiveSurfaceBossImpl::down (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::down()");

    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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

void MedicinaActiveSurfaceBossImpl::bottom (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::bottom()");

    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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

void MedicinaActiveSurfaceBossImpl::top (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::top()");

    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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

void MedicinaActiveSurfaceBossImpl::move (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius, CORBA::Long incr) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::move()");

    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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

void MedicinaActiveSurfaceBossImpl::correction (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius, CORBA::Double correction) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::correction()");

    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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

void MedicinaActiveSurfaceBossImpl::reset (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::reset()");

    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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

void MedicinaActiveSurfaceBossImpl::setProfile (ActiveSurface::TASProfile newProfile) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::setProfile()");

    m_profile = newProfile;

    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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

void MedicinaActiveSurfaceBossImpl::usdStatus4GUIClient (CORBA::Long circle, CORBA::Long actuator, CORBA::Long_out status) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::usdStatus4GUIClient()");

    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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

void MedicinaActiveSurfaceBossImpl::asStatus4GUIClient (ACS::longSeq_out status) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::asStatus4GUIClient()");

    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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

void MedicinaActiveSurfaceBossImpl::setActuator (CORBA::Long circle, CORBA::Long actuator, CORBA::Long_out actPos, CORBA::Long_out cmdPos, CORBA::Long_out Fmin, CORBA::Long_out Fmax, CORBA::Long_out acc, CORBA::Long_out delay) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::setActuator");

    long int act, cmd, fmin, fmax, ac, del;

    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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


void MedicinaActiveSurfaceBossImpl::calibrate (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::calibrate()");

    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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

void MedicinaActiveSurfaceBossImpl::calVer (CORBA::Long circle, CORBA::Long actuator, CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::calibration verification()");

    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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


void MedicinaActiveSurfaceBossImpl::recoverUSD (CORBA::Long circle, CORBA::Long actuator) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::recoverUSD()");

    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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

void MedicinaActiveSurfaceBossImpl::asOn() throw (CORBA::SystemException)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::asOn()");
    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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

void MedicinaActiveSurfaceBossImpl::asOff() throw (CORBA::SystemException)
{
    AUTO_TRACE("MedicinaActiveSurfaceBossImpl::asOff()");
    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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

CORBA::Boolean MedicinaActiveSurfaceBossImpl::command(const char *cmd,CORBA::String_out answer) throw (CORBA::SystemException)
{
    AUTO_TRACE("AntennaBossImpl::command()");
    IRA::CString out;
    bool res;
    CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();
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

_PROPERTY_REFERENCE_CPP(MedicinaActiveSurfaceBossImpl,Management::ROTSystemStatus,m_pstatus,status);
_PROPERTY_REFERENCE_CPP(MedicinaActiveSurfaceBossImpl,Management::ROTBoolean,m_penabled,enabled);
_PROPERTY_REFERENCE_CPP(MedicinaActiveSurfaceBossImpl,ActiveSurface::ROTASProfile,m_pprofile,pprofile);
_PROPERTY_REFERENCE_CPP(MedicinaActiveSurfaceBossImpl,Management::ROTBoolean,m_ptracking,tracking);
/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(MedicinaActiveSurfaceBossImpl)

/*___oOo___*/
