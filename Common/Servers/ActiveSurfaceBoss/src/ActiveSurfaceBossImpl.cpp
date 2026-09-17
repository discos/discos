#include <future>
#include <cmath>
#include "ActiveSurfaceBossImpl.h"
#include "DevIOStatus.h"
#include "DevIOProfile.h"
#include "DevIOTracking.h"
#include "DevIOLUT.h"

namespace SP = SimpleParser;

ActiveSurfaceBossImpl::ActiveSurfaceBossImpl(const ACE_CString& CompName, maci::ContainerServices* containerServices) :
    CharacteristicComponentImpl(CompName, containerServices),
    m_containerServices(*containerServices),
    m_parser(this, 1),
    m_pstatus(this),
    m_pprofile(this),
    m_ptracking(this),
    m_pLUT(this),
    m_profile(ActiveSurface::AS_PARK),
    m_status(Management::MNG_WARNING),
    m_tracking(Management::MNG_FALSE),
    m_trackingEnabled(true),
    m_LUT("--"),
    m_maxUSDCount(0),
    m_zmqPublisher("active_surface"),
    m_antennaBoss("IDL:alma/Antenna/AntennaBoss:1.0", containerServices)
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

    long workingThreadTime;
    IRA::CString buff;
    if(!IRA::CIRATools::getDBValue(&m_containerServices, "WorkingThreadTime", (long&)workingThreadTime) ||
       !IRA::CIRATools::getDBValue(&m_containerServices, "acceptedProfiles", buff))
    {
        ACS_LOG(LM_SOURCE_INFO, "ActiveSurfaceBossImpl::initialize()", (LM_ERROR, "Error reading CDB!"));
        ASErrors::CDBAccessErrorExImpl exImpl(__FILE__, __LINE__, "ActiveSurfaceBossImpl::initialize() - Error reading CDB parameters");
        throw acsErrTypeLifeCycle::LifeCycleExImpl(exImpl, __FILE__, __LINE__, "ActiveSurfaceBossImpl::initialize()");
    }

    std::istringstream iss(std::string(buff).c_str());
    std::string token;

    while(std::getline(iss, token, ','))
    {
        try
        {
            m_acceptedProfiles.insert((ActiveSurface::TASProfile)std::stoi(token));
        }
        catch(...)
        {
            ACS_LOG(LM_SOURCE_INFO, "ActiveSurfaceBossImpl::initialize()", (LM_ERROR, "Error reading CDB!"));
            ASErrors::CDBAccessErrorExImpl exImpl(__FILE__, __LINE__, "ActiveSurfaceBossImpl::initialize() - Error reading CDB parameters");
            throw acsErrTypeLifeCycle::LifeCycleExImpl(exImpl, __FILE__, __LINE__, "ActiveSurfaceBossImpl::initialize()");
        }
    }

    if(m_acceptedProfiles.empty())
    {
        ACS_LOG(LM_SOURCE_INFO, "ActiveSurfaceBossImpl::initialize()", (LM_ERROR, "Error reading CDB!"));
        ASErrors::CDBAccessErrorExImpl exImpl(__FILE__, __LINE__, "ActiveSurfaceBossImpl::initialize() - Error reading CDB parameters");
        throw acsErrTypeLifeCycle::LifeCycleExImpl(exImpl, __FILE__, __LINE__, "ActiveSurfaceBossImpl::initialize()");
    }

    ACS_LOG(LM_FULL_INFO, "ActiveSurfaceBossImpl::initialize()", (LM_INFO, "COMPSTATE_INITIALIZING"));

    try
    {
        m_pstatus = new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus), POA_Management::ROTSystemStatus>
            (m_containerServices.getName() + ":status", getComponent(), new ActiveSurfaceBossImplDevIOStatus(*this), true);
        m_pprofile = new ROEnumImpl<ACS_ENUM_T(ActiveSurface::TASProfile), POA_ActiveSurface::ROTASProfile>
            (m_containerServices.getName() + ":profile", getComponent(), new ActiveSurfaceBossImplDevIOProfile(*this), true);
        m_ptracking = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>
            (m_containerServices.getName() + ":tracking", getComponent(), new ActiveSurfaceBossImplDevIOTracking(*this), true);
        m_pLUT = new ROstring(m_containerServices.getName() + ":LUT", getComponent(), new ActiveSurfaceBossImplDevIOLUT(*this), true);
    }
    catch(std::bad_alloc& ex)
    {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "ActiveSurfaceBossImpl::initialize()");
        throw dummy;
    }

    m_parser.add("asSetup", new SP::function1<ActiveSurfaceBossImpl, SP::non_constant, SP::void_type, SP::I<SP::string_type>>(this, &ActiveSurfaceBossImpl::_setup), 1);
    m_parser.add("asOn", new SP::function0<ActiveSurfaceBossImpl, SP::non_constant, SP::void_type>(this, &ActiveSurfaceBossImpl::asOn), 0);
    m_parser.add("asOff", new SP::function0<ActiveSurfaceBossImpl, SP::non_constant, SP::void_type>(this, &ActiveSurfaceBossImpl::asOff), 0);
    m_parser.add("asPark", new SP::function0<ActiveSurfaceBossImpl, SP::non_constant, SP::void_type>(this, &ActiveSurfaceBossImpl::_park), 0);
    m_parser.add("asSetLUT", new SP::function1<ActiveSurfaceBossImpl, SP::non_constant, SP::void_type, SP::I<SP::string_type>>(this, &ActiveSurfaceBossImpl::_setLUT), 1);

    // Find the minimum and maximum USD indexes for this station
    ACE_CString_Vector allUSDs = m_containerServices.findComponents("AS/SECTOR*/LAN*/USD*", "*");
    std::map<std::string, unsigned int> usdCountPerLan;

    for(const auto& n : allUSDs)
    {
        std::string name(n.c_str());
        std::size_t pos = name.rfind("/");

        if(pos == std::string::npos)
        {
            continue;
        }

        std::string lanKey = name.substr(0, pos);
        usdCountPerLan[lanKey]++;
    }

    for(const auto& [lan, usdCount] : usdCountPerLan)
    {
        m_maxUSDCount = std::max(m_maxUSDCount, usdCount);
    }

    m_maxTickIndex = static_cast<unsigned int>(std::ceil(static_cast<double>(m_maxUSDCount) / TICK_DIVIDER));

    // Start the working thread
    try
    {
        m_workingThread = m_containerServices.getThreadManager()->create<ActiveSurfaceBossWorkingThread, ActiveSurfaceBossImpl&>(
            "ActiveSurfaceBossWorkingThread", *this, ACS::ThreadBase::defaultResponseTime, ACS::TimeInterval(workingThreadTime * 10)
        );
    }
    catch(acsthreadErrType::CanNotSpawnThreadExImpl& _impl)
    {
        _ADD_BACKTRACE(ComponentErrors::CanNotStartThreadExImpl, impl, _impl, "ActiveSurfaceBossImpl::initialize()");
        impl.setThreadName("ActiveSurfaceBossWorkingThread");
        impl.log(LM_DEBUG);
        throw acsErrTypeLifeCycle::LifeCycleExImpl(impl, __FILE__, __LINE__, "ActiveSurfaceBossImpl::initialize()");
    }

    ACS_LOG(LM_FULL_INFO, "ActiveSurfaceBossImpl::initialize()", (LM_INFO, "COMPSTATE_INITIALIZED"));
}

void ActiveSurfaceBossImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::execute()");

    std::string search = "AS/SECTOR*";
    ACE_CString_Vector allNames = m_containerServices.findComponents(search.c_str(), "*");

    const std::string prefix = "AS/SECTOR";

    for(const auto& n : allNames)
    {
        std::string name(n.c_str());
        std::string relative = name.substr(3);
    
        if(relative.find('/') != std::string::npos)
        {
            continue;
        }

        unsigned int sectorIndex = static_cast<unsigned int>(std::atoi(name.c_str() + prefix.length()));
        ActiveSurface::Sector_proxy proxy;
        proxy.setContainerServices(&m_containerServices);
        proxy.setComponentName(name.c_str());
        m_sectors[sectorIndex] = proxy;

        std::ostringstream sectorKey;
        sectorKey << "SECTOR" << std::setw(2) << std::setfill('0') << sectorIndex;
        ZMQ::ZMQDictionary& sectorDictionary = m_zmqDictionary[sectorKey.str()];

        for(unsigned int lanIndex = 1; lanIndex <= LANS_PER_SECTOR; lanIndex++)
        {
            std::ostringstream lanKey;
            lanKey << "LAN" << std::setw(2) << std::setfill('0') << lanIndex;
            ZMQ::ZMQDictionary& lanDictionary = sectorDictionary[lanKey.str()];

            lanDictionary["connected"] = false;

            std::ostringstream lanSearch;
            lanSearch << name << "/" << lanKey.str() << "/USD*";
            ACE_CString_Vector allUSDs = m_containerServices.findComponents(lanSearch.str().c_str(), "*");

            for(const auto& usdName : allUSDs)
            {
                std::string usdFullName(usdName.c_str());
                std::size_t usdPos = usdFullName.rfind("USD");

                if(usdPos == std::string::npos)
                {
                    continue;
                }

                std::string usdKey = usdFullName.substr(usdPos);
                lanDictionary[usdKey]["available"] = false;
            }
        }
    }

    m_workingThread->resume();
}

void ActiveSurfaceBossImpl::cleanUp()
{
    AUTO_TRACE("ActiveSurfaceBossImpl::cleanUp()");
    CharacteristicComponentImpl::cleanUp();
}

void ActiveSurfaceBossImpl::aboutToAbort()
{
    AUTO_TRACE("ActiveSurfaceBossImpl::aboutToAbort()");
    CharacteristicComponentImpl::aboutToAbort();
}

void ActiveSurfaceBossImpl::setup(const char* config) throw (ManagementErrors::ConfigurationErrorEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::setup()");

    try
    {
        _setup(config);
    }
    catch(ManagementErrors::ConfigurationErrorExImpl& impl)
    {
        impl.log(LM_DEBUG);
        throw impl.getConfigurationErrorEx();
    }
}

void ActiveSurfaceBossImpl::_setup(const char* config) throw (ManagementErrors::ConfigurationErrorExImpl)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::_setup()");
    IRA::CString strVal(config);
    strVal.MakeUpper();
    try
    {
        if(strVal == "S")
        {
            _setProfile(ActiveSurface::AS_SHAPED);
        }
        else if(strVal == "SF")
        {
            _setProfile(ActiveSurface::AS_SHAPED_FIXED);
        }
        else if(strVal == "P")
        {
            _setProfile(ActiveSurface::AS_PARABOLIC);
        }
        else if(strVal == "PF")
        {
            _setProfile(ActiveSurface::AS_PARABOLIC_FIXED);
        }
        else
        {
            _THROW_EXCPT(ASErrors::UnknownProfileExImpl, "ActiveSurfaceBossImpl::_setup()");
        }
    }
    catch(ASErrors::UnknownProfileExImpl& ex)
    {
        _ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl, impl, ex, "ActiveSurfaceBossImpl::_setup()");
        impl.setSubsystem("ActiveSurfaceBoss");
        throw impl;
    }
}

void ActiveSurfaceBossImpl::park() throw (ManagementErrors::ParkingErrorEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::park()");

    try
    {
        _park();
    }
    catch(ManagementErrors::ParkingErrorExImpl& impl)
    {
        impl.log(LM_DEBUG);
        throw impl.getParkingErrorEx();
    }
}

void ActiveSurfaceBossImpl::_park() throw (ManagementErrors::ParkingErrorExImpl)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::_park()");

    try
    {
        _setProfile(ActiveSurface::AS_PARK);
    }
    catch(ASErrors::UnknownProfileExImpl& ex)
    {
        _ADD_BACKTRACE(ManagementErrors::ParkingErrorExImpl, impl, ex, "ActiveSurfaceBossImpl::park()");
        impl.setSubsystem("ActiveSurfaceBoss");
        throw impl;
    }

    m_profile = ActiveSurface::AS_PARK;
}

void ActiveSurfaceBossImpl::update(unsigned long tickIndex) throw (ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::update()");

    double azimuth, elevation;

    try
    {
        m_antennaBoss->getRawCoordinates(getTimeStamp(), azimuth, elevation);
    }
    catch(ComponentErrors::CouldntGetComponentExImpl& ex)
    {
        ex.log(LM_WARNING);
        m_status = Management::MNG_WARNING;
        return;
    }

    m_status = Management::MNG_OK;

    elevation = elevation * DR2D;

    std::vector<std::future<void>> futures;

    for(auto& [sectorIndex, sector] : m_sectors)
    {
        futures.push_back(std::async(std::launch::async, [sector, elevation, tickIndex]()
        {
            try
            {
                sector->update(elevation, tickIndex, false);
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

void ActiveSurfaceBossImpl::setProfile(ActiveSurface::TASProfile newProfile) throw (ComponentErrors::ComponentErrorsEx, ASErrors::ASErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::setProfile()");

    try
    {
        _setProfile(newProfile);
    }
    catch(ASErrors::UnknownProfileExImpl& impl)
    {
        throw impl.getASErrorsEx();
    }
}

void ActiveSurfaceBossImpl::_setProfile(ActiveSurface::TASProfile newProfile) throw (ComponentErrors::ComponentErrorsExImpl, ASErrors::ASErrorsExImpl)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::_setProfile()");

    if(!checkProfile(newProfile))
    {
        _THROW_EXCPT(ASErrors::UnknownProfileExImpl, "ActiveSurfaceBossImpl::_setProfile()");
    }

    for(const auto& [sectorIndex, sector] : m_sectors)
    {
        try
        {
            sector->setProfile(newProfile);
        }
        catch(ComponentErrors::ComponentErrorsEx& ex)
        {
            ComponentErrors::ComponentErrorsExImpl exImpl(ex);
            exImpl.log(LM_WARNING);
        }
    }

    if(!IRA::CIRATools::setDBValue(&m_containerServices, "profile", static_cast<const long&>(newProfile)))
    {
        ASErrors::CDBAccessErrorExImpl exImpl(__FILE__, __LINE__, "ActiveSurfaceBossImpl::_setProfile()");
        exImpl.setFieldName("profile");
        throw exImpl;
    }

    m_profile = newProfile;
}

void ActiveSurfaceBossImpl::setLUT(const char* LUTName) throw (ComponentErrors::ComponentErrorsEx, ASErrors::ASErrorsEx)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::setLUT()");

    try
    {
        _setLUT(LUTName);
    }
    catch(ASErrors::UnknownLUTExImpl& impl)
    {
        throw impl.getASErrorsEx();
    }
}

void ActiveSurfaceBossImpl::_setLUT(const char* LUTName) throw (ComponentErrors::ComponentErrorsExImpl, ASErrors::ASErrorsExImpl)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::_setLUT()");

    std::string LUT(LUTName);
    std::transform(LUT.begin(), LUT.end(), LUT.begin(), ::toupper);

    CDB::DAL_var dal = m_containerServices.getCDB();
    try
    {
        dal->get_DAO(("alma/DataBlock/ActiveSurface/" + LUT).c_str());
    }
    catch(cdbErrType::CDBRecordDoesNotExistEx& ex)
    {
        _ADD_BACKTRACE(ASErrors::UnknownLUTExImpl, impl, ex, "ActiveSurfaceBossImpl::setLUT()");
        impl.log(LM_DEBUG);
        throw impl;
    }

    for(const auto& [sectorIndex, sector] : m_sectors)
    {
        try
        {
            sector->setLUT(LUT.c_str());
        }
        catch(ComponentErrors::ComponentErrorsEx& ex)
        {
            ComponentErrors::ComponentErrorsExImpl exImpl(ex);
            exImpl.log(LM_WARNING);
        }
    }

    m_LUT = LUT;
}

void ActiveSurfaceBossImpl::asOn()
{
    AUTO_TRACE("ActiveSurfaceBossImpl::asOn()");
    m_trackingEnabled = true;
}

void ActiveSurfaceBossImpl::asOff()
{
    AUTO_TRACE("ActiveSurfaceBossImpl::asOff()");
    m_trackingEnabled = false;
}

void ActiveSurfaceBossImpl::pollSectorStatus(unsigned long tickIndex)
{
    AUTO_TRACE("ActiveSurfaceBossImpl::pollSectorStatus()");

    std::vector<std::future<void>> futures;

    for(auto& [sectorIndex, sector] : m_sectors)
    {
        futures.push_back(std::async(std::launch::async, [this, sectorIndex, sector, tickIndex]()
        {
            try
            {
                CORBA::BooleanSeq_var connectedSeq;
                ActiveSurface::USDStatusSeq_var USDSeq;
                sector->getTickStatus(tickIndex, connectedSeq, USDSeq);

                std::ostringstream sectorKey;
                sectorKey << "SECTOR" << std::setw(2) << std::setfill('0') << sectorIndex;

                for(size_t i = 0; i < LANS_PER_SECTOR; i++)
                {
                    unsigned int lanIndex = i + 1;
                    std::ostringstream lanKey;
                    lanKey << "LAN" << std::setw(2) << std::setfill('0') << lanIndex;

                    ZMQ::ZMQDictionary& lan = m_zmqDictionary[sectorKey.str()][lanKey.str()];
                    lan["connected"] = static_cast<bool>(connectedSeq[i]);

                    for(size_t j = 0; j < TICK_DIVIDER; j++)
                    {
                        CORBA::ULong usdIndex = j * LANS_PER_SECTOR + i;

                        if(USDSeq[usdIndex].id == -1)
                        {
                            continue;
                        }

                        std::ostringstream usdKey;
                        usdKey << "USD" << std::setw(2) << std::setfill('0') << USDSeq[usdIndex].id;

                        ZMQ::ZMQDictionary& usd = lan[usdKey.str()];

                        usd["available"] = USDSeq[usdIndex].available;
                        if(!USDSeq[usdIndex].available)
                        {
                            continue;
                        }
                        usd["accelerationFactor"] = USDSeq[usdIndex].accelerationFactor;
                        usd["commandedPosition"] = USDSeq[usdIndex].commandedPosition;
                        usd["currentPosition"] = USDSeq[usdIndex].currentPosition;
                        usd["delay"] = USDSeq[usdIndex].delay == 255 ? -1 : 256 * USDSeq[usdIndex].delay;
                        usd["maximumFrequency"] = USDSeq[usdIndex].maximumFrequency;
                        usd["minimumFrequency"] = USDSeq[usdIndex].minimumFrequency;
                        usd["softwareVersion"] = std::to_string((USDSeq[usdIndex].softwareVersion >> 4) & 0xF) + "." + std::to_string(USDSeq[usdIndex].softwareVersion & 0xF);
                        usd["USDType"] = USDSeq[usdIndex].type == 0x20 ? "USD50xxx" : "USD60xxx";
                        usd["calibrated"] = (USDSeq[usdIndex].status & CAL) != 0;
                        usd["enabled"] = (USDSeq[usdIndex].status & ENBL) != 0;
                        usd["running"] = (USDSeq[usdIndex].status & MRUN) != 0;
                    }
                }
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

void ActiveSurfaceBossImpl::publishZMQDictionary(ACS::Time now)
{
    m_zmqDictionary["timestamp"] = ZMQ::ZMQTimeStamp::fromACSTime(now);
    m_zmqDictionary["LUT"] = m_LUT;
    m_zmqDictionary["tracking"] = m_tracking == Management::MNG_TRUE;

    switch(m_status)
    {
        case Management::MNG_OK:
        {
            m_zmqDictionary["status"] = "OK";
            break;
        }
        case Management::MNG_WARNING:
        {
            m_zmqDictionary["status"] = "WARNING";
            break;
        }
        default: //Management::MNG_FAILURE
        {
            m_zmqDictionary["status"] = "FAILURE";
            break;
        }
    }

    switch(m_profile)
    {
        default: //ActiveSurface::AS_PARK
        {
            m_zmqDictionary["profile"] = "PARK";
            break;
        }
        case ActiveSurface::AS_SHAPED:
        {
            m_zmqDictionary["profile"] = "SHAPED";
            break;
        }
        case ActiveSurface::AS_SHAPED_FIXED:
        {
            m_zmqDictionary["profile"] = "SHAPED FIXED";
            break;
        }
        case ActiveSurface::AS_PARABOLIC:
        {
            m_zmqDictionary["profile"] = "PARABOLIC";
            break;
        }
        case ActiveSurface::AS_PARABOLIC_FIXED:
        {
            m_zmqDictionary["profile"] = "PARABOLIC FIXED";
            break;
        }
    }

    m_zmqPublisher.publish(m_zmqDictionary);
}

CORBA::Boolean ActiveSurfaceBossImpl::command(const char* cmd, CORBA::String_out answer)
{
    AUTO_TRACE("AntennaBossImpl::command()");
    IRA::CString out;
    bool res = false;

    try
    {
        m_parser.run(cmd, out);
        res = true;
    }
    catch(ParserErrors::ParserErrorsExImpl &impl)
    {
        // Parser errors are never logged
    }
    catch(ACSErr::ACSbaseExImpl& impl)
    {
        // The errors resulting from the execution are logged here as stated in the documentation of CommandInterpreter interface
        impl.log(LM_ERROR); 
    }

    answer = CORBA::string_dup((const char*)out);
    return res;
}

_PROPERTY_REFERENCE_CPP(ActiveSurfaceBossImpl, Management::ROTSystemStatus, m_pstatus, status);
_PROPERTY_REFERENCE_CPP(ActiveSurfaceBossImpl, ActiveSurface::ROTASProfile, m_pprofile, profile);
_PROPERTY_REFERENCE_CPP(ActiveSurfaceBossImpl, Management::ROTBoolean, m_ptracking, tracking);
_PROPERTY_REFERENCE_CPP(ActiveSurfaceBossImpl, ACS::ROstring, m_pLUT, LUT);
/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(ActiveSurfaceBossImpl)

/*___oOo___*/
