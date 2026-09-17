#include "ActiveSurfaceSectorImpl.h"
#include <future>

ActiveSurfaceSectorImpl::ActiveSurfaceSectorImpl(const ACE_CString& CompName, maci::ContainerServices* containerServices) :
    baci::CharacteristicComponentImpl(CompName, containerServices),
    m_containerServices(containerServices),
    m_index_sp(this),
    m_ready_sp(this)
{
    AUTO_TRACE("ActiveSurfaceSectorImpl::ActiveSurfaceSectorImpl()");
}

ActiveSurfaceSectorImpl::~ActiveSurfaceSectorImpl()
{
    AUTO_TRACE("ActiveSurfaceSectorImpl::~ActiveSurfaceSectorImpl()");
}

void ActiveSurfaceSectorImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
    AUTO_TRACE("ActiveSurfaceSectorImpl::initialize()");

    const ACE_CString cn(name());
    m_index_sp = new baci::ROlong(cn + ":index", getComponent());
    m_ready_sp = new baci::ROboolean(cn + ":ready", getComponent());

    const std::string compName(name());
    const auto pos = compName.rfind("SECTOR");

    ACS::Time timestamp;
    m_index_sp->getDevIO()->write(static_cast<CORBA::Long>(std::atoi(compName.c_str() + pos + 6)), timestamp);
    m_ready_sp->getDevIO()->write(false, timestamp);
}

void ActiveSurfaceSectorImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
    AUTO_TRACE("ActiveSurfaceSectorImpl::execute()");

    const std::string prefix = std::string(name()) + "/LAN";

    for(unsigned int i = 1; i <= LANS_PER_SECTOR; i++)
    {
        std::ostringstream oss;
        oss << prefix << std::setw(2) << std::setfill('0') << i;
        std::string lanName = oss.str();

        m_lans[i] = m_containerServices->getComponent<ActiveSurface::LAN>(lanName.c_str());
    }

    ACS::Time timestamp;
    m_ready_sp->getDevIO()->write(true, timestamp);
}

void ActiveSurfaceSectorImpl::cleanUp()
{
    AUTO_TRACE("ActiveSurfaceSectorImpl::cleanUp()");
    baci::CharacteristicComponentImpl::cleanUp();
}

void ActiveSurfaceSectorImpl::aboutToAbort()
{
    AUTO_TRACE("ActiveSurfaceSectorImpl::aboutToAbort()");
    baci::CharacteristicComponentImpl::aboutToAbort();
}

void ActiveSurfaceSectorImpl::update(CORBA::Double elevation, CORBA::Long tickIndex, CORBA::Boolean force) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceSectorImpl::update()");

    std::vector<std::future<void>> futures;

    for(auto& [lanIndex, lan] : m_lans)
    {
        futures.push_back(std::async(std::launch::async, [lan, elevation, tickIndex, force]()
        {
            try
            {
                lan->update(elevation, tickIndex, force);
            }
            catch(ComponentErrors::ComponentErrorsEx& ex)
            {
                ComponentErrors::ComponentErrorsExImpl exImpl(ex);
                exImpl.log(LM_WARNING);
            }
        }));
    }

    for(auto& f : futures)
    {
        f.wait();
    }
}

void ActiveSurfaceSectorImpl::setProfile(ActiveSurface::TASProfile newProfile) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, ASErrors::UnknownProfileEx)
{
    AUTO_TRACE("ActiveSurfaceSectorImpl::setProfile()");

    for(auto& [lanIndex, lan] : m_lans)
    {
        try
        {
            lan->setProfile(newProfile);
        }
        catch(ComponentErrors::ComponentErrorsEx& ex)
        {
            ComponentErrors::ComponentErrorsExImpl exImpl(ex);
            exImpl.log(LM_WARNING);
        }
    }

    m_profile = newProfile;
}

void ActiveSurfaceSectorImpl::setLUT(const char* LUTName) throw (ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceSectorImpl::loadLut()");

    for(auto& [lanIndex, lan] : m_lans)
    {
        try
        {
            lan->setLUT(LUTName);
        }
        catch(ComponentErrors::ComponentErrorsEx& ex)
        {
            ComponentErrors::ComponentErrorsExImpl exImpl(ex);
            exImpl.log(LM_WARNING);
        }
    }
}

void ActiveSurfaceSectorImpl::getTickStatus(CORBA::Long tickIndex, CORBA::BooleanSeq_out connected, ActiveSurface::USDStatusSeq_out USDSeq) throw (ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceSectorImpl::getTickStatus()");

    USDSeq = new ActiveSurface::USDStatusSeq;
    USDSeq->length(TICK_DIVIDER * LANS_PER_SECTOR);

    connected = new CORBA::BooleanSeq;
    connected->length(LANS_PER_SECTOR);

    std::vector<std::future<void>> futures;

    for(auto& [lanIndex, lan] : m_lans)
    {
        futures.push_back(std::async(std::launch::async, [lanIndex, lan, tickIndex, &USDSeq, &connected]()
        {
            unsigned int index = lanIndex - 1;

            try
            {
                ACSErr::Completion_var completion;
                connected[index] = lan->connected()->get_sync(completion.out());
            }
            catch(...)
            {
                connected[index] = false;
            }

            for(size_t i = 0; i < TICK_DIVIDER; i++)
            {
                unsigned long slot = tickIndex * TICK_DIVIDER + i;

                ActiveSurface::USDStatus status;
                if(lan->getSlotStatus(slot, status))
                {
                    USDSeq[index + i * LANS_PER_SECTOR] = status;
                }
                else
                {
                    USDSeq[index + i * LANS_PER_SECTOR].id = -1;
                }
            }
        }));
    }

    for(auto& f : futures)
    {
        f.wait();
    }
}

void ActiveSurfaceSectorImpl::park() throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, ASErrors::ASErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceSectorImpl::loadLut()");

    std::vector<std::future<void>> futures;

    for(auto& [lanIndex, lan] : m_lans)
    {
        futures.push_back(std::async(std::launch::async, [lan]()
        {
            try
            {
                lan->setAbsolutePosition(0);
            }
            catch(ComponentErrors::ComponentErrorsEx& ex)
            {
                ComponentErrors::ComponentErrorsExImpl exImpl(ex);
                exImpl.log(LM_WARNING);
            }
        }));
    }

    for(auto& f : futures)
    {
        f.wait();
    }

    futures.clear();

    for(auto& [lanIndex, lan] : m_lans)
    {
        futures.push_back(std::async(std::launch::async, [lan]()
        {
            try
            {
                lan->trigger();
            }
            catch(ComponentErrors::ComponentErrorsEx& ex)
            {
                ComponentErrors::ComponentErrorsExImpl exImpl(ex);
                exImpl.log(LM_WARNING);
            }
        }));
    }

    for(auto& f : futures)
    {
        f.wait();
    }

    setProfile(ActiveSurface::AS_PARK);
}

/*ACS::ROlong_ptr ActiveSurfaceSectorImpl::index()
{
    if(!m_index_sp)
    {
        return ACS::ROlong::_nil();
    }
    ACS::ROlong_var prop = ACS::ROlong::_narrow(m_index_sp->getCORBAReference());
    return prop._retn();
}

ACS::ROboolean_ptr ActiveSurfaceSectorImpl::ready()
{
    if(!m_ready_sp)
    {
        return ACS::ROboolean::_nil();
    }
    ACS::ROboolean_var prop = ACS::ROboolean::_narrow(m_ready_sp->getCORBAReference());
    return prop._retn();
}*/

_PROPERTY_REFERENCE_CPP(ActiveSurfaceSectorImpl, ACS::ROlong, m_index_sp, index);
_PROPERTY_REFERENCE_CPP(ActiveSurfaceSectorImpl, ACS::ROboolean, m_ready_sp, ready);

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(ActiveSurfaceSectorImpl)

/*___oOo___*/
