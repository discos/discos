#include <baciDB.h>
#include <maciContainerImpl.h>
#include "ActiveSurfaceLANImpl.h"


using namespace ActiveSurface::CommandLibrary;

// ===========================================================================
// Constructor / destructor
// ===========================================================================

LANImpl::LANImpl(const ACE_CString& name, maci::ContainerServices* cs) :
    CharacteristicComponentImpl(name, cs),
    m_connected_sp(this),
    m_lanIndex(0),
    m_socket(nullptr)
{
    ACS_TRACE("LANImpl::LANImpl()");
}

LANImpl::~LANImpl()
{
    ACS_TRACE("LANImpl::~LANImpl()");
}

// ===========================================================================
// ACS lifecycle
// ===========================================================================

void LANImpl::initialize()
{
    ACS_TRACE("LANImpl::initialize()");

    // Extract local LAN index from component name (e.g. "AS/SECTOR01/LAN03" -> 3)
    const std::string compName(this->name());
    const auto pos = compName.rfind("LAN");
    m_lanIndex = static_cast<unsigned int>(std::atoi(compName.c_str() + pos + 3));

    // Shared socket — creates and connects on first call, retrieved on subsequent calls
    try
    {
        m_socket = &ActiveSurface::Socket::getInstance(compName, getContainerServices());
    }
    catch (ASErrors::ASErrorsExImpl& ex)
    {
        ACS_LOG(LM_SOURCE_INFO, "LANImpl::initialize()", (LM_ERROR, "Socket connection failed, starting disconnected."));
    }

    // Property
    const ACE_CString cn(this->name());
    m_connected_sp = new ROboolean(cn + ":connected", getComponent());

    // Read broadcast configuration parameters from LAN CDB node
    if(!IRA::CIRATools::getDBValue(getContainerServices(), "Fmin", m_fmin))
    {
        ASErrors::CDBAccessErrorExImpl ex(__FILE__, __LINE__, "LANImpl::initialize()");
        ex.setFieldName("Fmin");
        throw acsErrTypeLifeCycle::LifeCycleExImpl(ex, __FILE__, __LINE__, "LANImpl::initialize()");
    }
    if(!IRA::CIRATools::getDBValue(getContainerServices(), "Fmax", m_fmax))
    {
        ASErrors::CDBAccessErrorExImpl ex(__FILE__, __LINE__, "LANImpl::initialize()");
        ex.setFieldName("Fmax");
        throw acsErrTypeLifeCycle::LifeCycleExImpl(ex, __FILE__, __LINE__, "LANImpl::initialize()");
    }
    if(!IRA::CIRATools::getDBValue(getContainerServices(), "acc", m_acc))
    {
        ASErrors::CDBAccessErrorExImpl ex(__FILE__, __LINE__, "LANImpl::initialize()");
        ex.setFieldName("acc");
        throw acsErrTypeLifeCycle::LifeCycleExImpl(ex, __FILE__, __LINE__, "LANImpl::initialize()");
    }
    if(!IRA::CIRATools::getDBValue(getContainerServices(), "delay", m_delay))
    {
        ASErrors::CDBAccessErrorExImpl ex(__FILE__, __LINE__, "LANImpl::initialize()");
        ex.setFieldName("delay");
        throw acsErrTypeLifeCycle::LifeCycleExImpl(ex, __FILE__, __LINE__, "LANImpl::initialize()");
    }
    if(!IRA::CIRATools::getDBValue(getContainerServices(), "uBits", m_uBits))
    {
        ASErrors::CDBAccessErrorExImpl ex(__FILE__, __LINE__, "LANImpl::initialize()");
        ex.setFieldName("uBits");
        throw acsErrTypeLifeCycle::LifeCycleExImpl(ex, __FILE__, __LINE__, "LANImpl::initialize()");
    }
    if(!IRA::CIRATools::getDBValue(getContainerServices(), "step_res", m_rs))
    {
        ASErrors::CDBAccessErrorExImpl ex(__FILE__, __LINE__, "LANImpl::initialize()");
        ex.setFieldName("step_res");
        throw acsErrTypeLifeCycle::LifeCycleExImpl(ex, __FILE__, __LINE__, "LANImpl::initialize()");
    }

    // Build sorted list of USD component names on this LAN
    std::string search = std::string(this->name()) + "/USD*";
    ACE_CString_Vector allNames = getContainerServices()->findComponents(search.c_str(), "*");

    std::vector<std::string> usdNames;
    usdNames.reserve(allNames.size());
    std::transform(allNames.begin(), allNames.end(), std::back_inserter(usdNames), [](const ACE_CString& s){ return s.c_str(); });
    std::sort(usdNames.begin(), usdNames.end());

    m_usds.reserve(usdNames.size());
    // Instantiate all USDCore objects — reads CDB, acquires socket, no hardware yet
    for(const auto& usdName : usdNames)
    {
        try
        {
            ActiveSurface::USDCore* usd = ActiveSurface::USDCore::getInstance(usdName, getContainerServices());
            usd->initialize();
            m_usds.push_back(usd);
        }
        catch(acsErrTypeLifeCycle::LifeCycleExImpl& ex)
        {
            ex.log(LM_ERROR);
            ACS_LOG(LM_SOURCE_INFO, "LANImpl::initialize()", (LM_ERROR, "Failed to instantiate USDCore for %s, skipping.", usdName.c_str()));
        }
    }
}

void LANImpl::execute()
{
    ACS_TRACE("LANImpl::execute()");

    // First of all we stop all movement
    stop();
    IRA::CIRATools::Wait(0, 5000);

    // Read status of each USD and mark as initialized
    for(const auto& usd : m_usds)
    {
        try
        {
            if(usd->needsReset())
            {
                usd->hardwareReset();
            }
        }
        catch(...)
        {
            //ACS_LOG(LM_SOURCE_INFO, "LANImpl::execute()", (LM_ERROR, "Failed to initialize USD %s.", usd->getName().c_str()));
        }
    }

    // Broadcast configuration to all USDs on the line
    setResolution(m_rs);
    setMinFrequency(m_fmin);
    setMaxFrequency(m_fmax);
    setSlopeMultiplier(m_acc);
    setResponseDelay(m_delay);
    setIoPins(m_uBits);

    IRA::CIRATools::Wait(0, 5000);

    for(const auto& usd : m_usds)
    {
        try
        {
            usd->readHardwareDetails();
        }
        catch(...)
        {
            //ACS_LOG(LM_SOURCE_INFO, "LANImpl::execute()", (LM_ERROR, "Failed to finalize hardware details for USD %s", usd->getName().c_str()));
        }
    }

    ACS_LOG(LM_SOURCE_INFO, "LANImpl::execute()", (LM_NOTICE, "Ready."));
}

void LANImpl::cleanUp()
{
    // Socket is owned by the multiton — do not delete
}

void LANImpl::aboutToAbort()
{
    // Socket is owned by the multiton — do not delete
}

// ===========================================================================
// Helpers
// ===========================================================================

void LANImpl::broadcast(const std::vector<uint8_t>& frame, const char* routine)
{
    if (!m_socket)
    {
        ACS_SHORT_LOG((LM_WARNING, "%s: socket not available.", routine));
        return;
    }
    try
    {
        m_socket->send(frame);
    }
    catch (ASErrors::ASErrorsExImpl ex)
    {
        ex.log(LM_WARNING);
    }
}

std::vector<double> LANImpl::parseCoeffs(const std::string s)
{
    std::vector<double> coeffs;
    std::stringstream ss(s);
    std::string token;
    while(std::getline(ss, token, ','))
    {
        coeffs.push_back(std::stod(token));
    }
    return coeffs;
}

void LANImpl::loadCorrectionTable(const std::string& compName, const std::string& lutName, maci::ContainerServices* cs)
{
    std::string cdbPath = "DataBlock/ActiveSurface/" + lutName + "/" + compName.substr(3);

    IRA::CDBTable table(cs, "usd", cdbPath.c_str());
    IRA::CError error;
    error.Reset();

    if(!table.addField(error, "id", IRA::CDataField::STRING))
    {
        error.setExtra("Error adding field id", 0);
    }
    if(!table.addField(error, "el15", IRA::CDataField::STRING))
    {
        error.setExtra("Error adding field el15", 0);
    }
    if(!table.addField(error, "el30", IRA::CDataField::STRING))
    {
        error.setExtra("Error adding field el30", 0);
    }
    if(!table.addField(error, "el45", IRA::CDataField::STRING))
    {
        error.setExtra("Error adding field el45", 0);
    }
    if(!table.addField(error, "el60", IRA::CDataField::STRING))
    {
        error.setExtra("Error adding field el60", 0);
    }
    if(!table.addField(error, "el75", IRA::CDataField::STRING))
    {
        error.setExtra("Error adding field el75", 0);
    }
    if(!table.addField(error, "p", IRA::CDataField::DOUBLE))
    {
        error.setExtra("Error adding field p", 0);
    }
    if(!error.isNoError())
    {
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl, ex, error);
        ex.setCode(error.getErrorCode());
        ex.setDescription((const char *)error.getDescription());
        ex.log(LM_DEBUG);
        throw ex;
    }
    if(!table.openTable(error))
    {
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, ex, error);
        ex.setFieldName(cdbPath.c_str());
        ex.log(LM_DEBUG);
        throw ex;
    }

    table.First();
    for(unsigned int i = 0; i < table.recordCount(); i++, table.Next())
    {
        std::string usdName = std::string(table["id"]->asString());
        std::vector<double> el15 = parseCoeffs(std::string(table["el15"]->asString()));
        std::vector<double> el30 = parseCoeffs(std::string(table["el30"]->asString()));
        std::vector<double> el45 = parseCoeffs(std::string(table["el45"]->asString()));
        std::vector<double> el60 = parseCoeffs(std::string(table["el60"]->asString()));
        std::vector<double> el75 = parseCoeffs(std::string(table["el75"]->asString()));
        double parabolicOffset   = table["p"]->asDouble();

        ActiveSurface::USDCore* usd = ActiveSurface::USDCore::getInstance(compName + "/" + usdName);
        usd->setCoefficients({ el15, el30, el45, el60, el75 }, parabolicOffset);

        ACS_STATIC_SHORT_LOG((LM_INFO, "LANImpl %s: loadCorrectionTable(),  row for %s", compName.c_str(), usdName.c_str()));
    }
    table.closeTable();

    ACS_STATIC_SHORT_LOG((LM_NOTICE, "LANImpl %s: loaded LUT %s.", compName.c_str(), lutName.c_str()));
}

template<typename T, typename U>
void LANImpl::updateAllUSDCache(T ActiveSurface::USDCore::* member, T value, U ActiveSurface::USDStatus::* statusMember, U statusValue)
{
    for(auto* usd : m_usds)
    {
        std::lock_guard<std::mutex> lock(usd->m_mutex);
        usd->*member = value;
        if(statusMember != nullptr)
        {
            usd->m_usdStatus.*statusMember = statusValue;
        }
    }
}

// ===========================================================================
// Tracking commands
// ===========================================================================

void LANImpl::update(CORBA::Double elevation, CORBA::Long tickIndex, CORBA::Boolean force)
{
    if(m_usds.empty())
    {
        // Should never occur
        return;
    }

    size_t n = m_usds.size();

    bool allUSDs = (tickIndex == -1);
    size_t startOffset = allUSDs ? 0 : (static_cast<size_t>(tickIndex) * TICK_DIVIDER) % n;
    size_t maxToProcess = allUSDs ? n : TICK_DIVIDER;

    std::vector<std::pair<ActiveSurface::USDCore*, long>> pending;

    for(size_t offset = 0; offset < n && pending.size() < maxToProcess; offset++)
    {
        size_t idx = (startOffset + offset) % n;
        ActiveSurface::USDCore* usd = m_usds[idx];

        try
        {
            if(!usd->isAvailable())
            {
                continue;
            }

            long targetPos = usd->computePosition(elevation, m_profile);

            if(!force && targetPos == usd->getCmdPos())
            {
                continue;
            }

            pending.emplace_back(usd, targetPos);
        }
        catch(ASErrors::ASErrorsExImpl& ex)
        {
            ex.log(LM_WARNING);
        }
    }

    for(const auto& [usd, targetPos] : pending)
    {
        try
        {
            usd->setCmdPos(targetPos);
        }
        catch(ASErrors::ASErrorsExImpl& ex)
        {
            ex.log(LM_WARNING);
        }
    }
}

// ===========================================================================
// Broadcast commands — control (rev. 1.2)
// ===========================================================================

void LANImpl::reset()
{
    broadcast(soft_reset(-1), "LANImpl::reset()");
}

void LANImpl::trigger()
{
    broadcast(soft_trigger(-1), "LANImpl::trigger()");
}

void LANImpl::stop()
{
    broadcast(soft_stop(-1), "LANImpl::stop()");
}

void LANImpl::setAbsolutePosition(CORBA::Long position)
{
    broadcast(set_absolute_position_steps(position, -1), "LANImpl::setAbsolutePosition()");
    updateAllUSDCache(&ActiveSurface::USDCore::m_cmdPos, static_cast<long>(position), &ActiveSurface::USDStatus::commandedPosition, position);
}

void LANImpl::setRelativePosition(CORBA::Long offset)
{
    // Better to not implement this now
    //broadcast(set_relative_position_steps(offset, -1), "LANImpl::setRelativePosition()");
}

void LANImpl::rotate(CORBA::Long direction)
{
    broadcast(ActiveSurface::CommandLibrary::rotate(direction, -1), "LANImpl::rotate()");
}

void LANImpl::setVelocity(CORBA::Long velocity)
{
    broadcast(set_velocity(velocity, -1), "LANImpl::setVelocity()");
}

// ===========================================================================
// Broadcast commands — configuration (rev. 1.2)
// ===========================================================================

void LANImpl::setMinFrequency(CORBA::Long frequency)
{
    broadcast(set_min_frequency(static_cast<uint16_t>(frequency), -1), "LANImpl::setMinFrequency()");
    updateAllUSDCache(&ActiveSurface::USDCore::m_fmin, static_cast<long>(frequency), &ActiveSurface::USDStatus::minimumFrequency, frequency);
}

void LANImpl::setMaxFrequency(CORBA::Long frequency)
{
    broadcast(set_max_frequency(static_cast<uint16_t>(frequency), -1), "LANImpl::setMaxFrequency()");
    updateAllUSDCache(&ActiveSurface::USDCore::m_fmax, static_cast<long>(frequency), &ActiveSurface::USDStatus::maximumFrequency, frequency);
}

void LANImpl::setSlopeMultiplier(CORBA::Long multiplier)
{
    broadcast(set_slope_multiplier(static_cast<uint8_t>(multiplier), -1), "LANImpl::setSlopeMultiplier()");
    updateAllUSDCache(&ActiveSurface::USDCore::m_acc, static_cast<long>(multiplier), &ActiveSurface::USDStatus::accelerationFactor, multiplier);
}

void LANImpl::setReferencePosition(CORBA::Long position)
{
    broadcast(set_reference_position_steps(position, -1), "LANImpl::setReferencePosition()");
}

void LANImpl::setIoPins(CORBA::Long io_byte)
{
    broadcast(set_io_pins(static_cast<uint8_t>(io_byte), -1), "LANImpl::setIoPins()");
    updateAllUSDCache(&ActiveSurface::USDCore::m_uBits, static_cast<long>(io_byte));
}

void LANImpl::setResolution(CORBA::Long res_byte)
{
    broadcast(set_resolution(static_cast<uint8_t>(res_byte), -1), "LANImpl::setResolution()");
    updateAllUSDCache(&ActiveSurface::USDCore::m_rs, static_cast<uint8_t>(res_byte));
}

void LANImpl::reduceCurrent(CORBA::Long standby_byte)
{
    broadcast(reduce_current(static_cast<uint8_t>(standby_byte), -1), "LANImpl::reduceCurrent()");
}

void LANImpl::setResponseDelay(CORBA::Long delay)
{
    broadcast(set_response_delay(static_cast<uint8_t>(delay), -1), "LANImpl::setResponseDelay()");
    updateAllUSDCache(&ActiveSurface::USDCore::m_delay, static_cast<long>(delay), &ActiveSurface::USDStatus::delay, delay);
}

void LANImpl::setDelayedExecution(CORBA::Boolean enable)
{
    broadcast(toggle_delayed_execution(enable, -1), "LANImpl::setDelayedExecution()");
}

// ===========================================================================
// Broadcast commands — rev. 1.3
// ===========================================================================

void LANImpl::setStopIo(CORBA::Long config_byte)
{
    broadcast(set_stop_io(static_cast<uint8_t>(config_byte), -1), "LANImpl::setStopIo()");
}

void LANImpl::setPositioningIo(CORBA::Long config_byte)
{
    broadcast(set_positioning_io(static_cast<uint8_t>(config_byte), -1), "LANImpl::setPositioningIo()");
}

void LANImpl::setHomeIo(CORBA::Long config_byte)
{
    broadcast(set_home_io(static_cast<uint8_t>(config_byte), -1), "LANImpl::setHomeIo()");
}

void LANImpl::setWorkingMode(CORBA::Long mode_byte)
{
    broadcast(set_working_mode(static_cast<uint8_t>(mode_byte), -1), "LANImpl::setWorkingMode()");
}

// ===========================================================================
// Status
// ===========================================================================

ACS::ROboolean_ptr LANImpl::connected()
{
    if (!m_connected_sp)
    {
        return ACS::ROboolean::_nil();
    }

    ACS::Time timestamp;
    CORBA::Boolean conn = m_socket != nullptr && m_socket->isConnected();
    m_connected_sp->getDevIO()->write(conn, timestamp);
    ACS::ROboolean_var prop = ACS::ROboolean::_narrow(m_connected_sp->getCORBAReference());
    return prop._retn();
}

CORBA::Boolean LANImpl::getSlotStatus(CORBA::Long slot, ActiveSurface::USDStatus_out usdStatus)
{
    if(static_cast<size_t>(slot) >= m_usds.size())
    {
        return false;
    }

    try
    {
        ActiveSurface::USDCore* usd = m_usds[slot];
        usd->readHWStatus();
        usdStatus = usd->getUSDStatus();
        return true;
    }
    catch(ASErrors::ASErrorsExImpl& ex)
    {
        ex.log(LM_WARNING);
        return false;
    }
}

// ===========================================================================
// LUT and profile management
// ===========================================================================

void LANImpl::setLUT(const char* LUTName)
{
    try
    {
        loadCorrectionTable(std::string(this->name()), std::string(LUTName), getContainerServices());
    }
    catch(ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_WARNING);
        ACS_SHORT_LOG((LM_WARNING, "LANImpl %s: failed to load LUT %s.", name(), LUTName));
        throw ex;
    }
}

void LANImpl::setProfile(ActiveSurface::TASProfile profile)
{
    m_profile = profile;
}

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(LANImpl)
