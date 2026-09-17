#include "ActiveSurfaceUSDCore.hpp"

static constexpr size_t FRAME_OVERHEAD  = 4;    // ACK + start + nbyte_addr + checksum

namespace ActiveSurface {

// ---------------------------------------------------------------------------
// Static member definitions
// ---------------------------------------------------------------------------

std::mutex USDCore::s_map_mutex;
std::map<std::string, std::unique_ptr<USDCore>> USDCore::s_instances;

// ---------------------------------------------------------------------------
// Multiton access
// ---------------------------------------------------------------------------

USDCore* USDCore::getInstance(const std::string& usdName)
{
    std::lock_guard<std::mutex> guard(s_map_mutex);

    auto it = s_instances.find(usdName);
    if(it != s_instances.end())
    {
        return it->second.get();
    }

    throw std::runtime_error("USDCore instance not found for: " + usdName);
}

USDCore* USDCore::getInstance(const std::string& usdName, maci::ContainerServices* containerServices)
{
    std::lock_guard<std::mutex> guard(s_map_mutex);

    auto it = s_instances.find(usdName);
    if(it != s_instances.end())
    {
        return it->second.get();
    }

    s_instances.emplace(usdName, std::unique_ptr<USDCore>(new USDCore(usdName, containerServices)));
    return s_instances.at(usdName).get();
}

// ---------------------------------------------------------------------------
// Constructor — CDB only, no hardware
// ---------------------------------------------------------------------------

USDCore::USDCore(const std::string& usdName, maci::ContainerServices* containerServices) :
    m_name(usdName),
    m_containerServices(containerServices),
    m_sector(0),
    m_lanNum(0),
    m_addr(0),
    m_fullRange(0),
    m_zeroRef(0),
    m_step_giro(0),
    m_rs(0),
    m_step_res(1.0),
    m_step2deg(0.0),
    m_top(0),
    m_bottom(0),
    m_cammaLen(0),
    m_cammaPos(0),
    m_cammaLenD(0.0),
    m_cammaPosD(0.0),
    m_calibrate(false),
    m_cmdPos(0),
    m_actPos(0),
    m_softVer(0),
    m_type(0),
    m_hwStatus(UNAV),
    m_lastCmdStep(0),
    m_fmin(0),
    m_fmax(0),
    m_acc(0),
    m_delay(0),
    m_uBits(0),
    m_available(false),
    m_softwareInitialized(false),
    m_initialized(false),
    m_socket(nullptr)
{
    if(!parseName(usdName, m_sector, m_lanNum, m_addr))
    {
        ACS_SHORT_LOG((LM_ERROR, "USDCore: failed to parse sector/lan/addr from name: %s", usdName.c_str()));
        return;
    }

    m_usdStatus.id                  = static_cast<CORBA::Long>(m_addr);
    m_usdStatus.available           = false;
    m_usdStatus.status              = UNAV;
    m_usdStatus.currentPosition     = 0;
    m_usdStatus.commandedPosition   = 0;
    m_usdStatus.softwareVersion     = 0;
    m_usdStatus.type                = 0;
    m_usdStatus.accelerationFactor  = 0;
    m_usdStatus.delay               = 0;
    m_usdStatus.minimumFrequency    = 0;
    m_usdStatus.maximumFrequency    = 0;
}

// ---------------------------------------------------------------------------
// Initialization
// ---------------------------------------------------------------------------

void USDCore::initialize()
{
    ACS_TRACE("USDCore::initialize()");

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(m_softwareInitialized)
        {
            return;
        }
    }

    // Read CDB — addressed by m_name, not by the caller's component name
    m_fullRange = readCDB<long>("fullRange");
    m_zeroRef   = readCDB<long>("zeroRef");
    m_cammaLen  = static_cast<int>(readCDB<double>("cammaLen"));
    m_cammaPos  = static_cast<int>(readCDB<double>("cammaPos"));
    m_step_giro = readCDB<long>("step_giro");
    m_rs        = static_cast<uint8_t>(readCDB<long>("step_res"));
    m_calibrate = readCDB<long>("calibrate") != 0;

    m_fmin  = readCDB<long>("Fmin/default_value");
    m_fmax  = readCDB<long>("Fmax/default_value");
    m_acc   = readCDB<long>("acc/default_value");
    m_delay = readCDB<long>("delay/default_value");
    m_uBits = readCDB<long>("uBits/default_value");

    m_step2deg = 360.0 / m_step_giro;
    m_step_res = 1.0 / std::pow(2.0, static_cast<double>(m_rs));
    m_top      = -m_zeroRef;
    m_bottom   = m_fullRange - m_zeroRef;
    
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_usdStatus.accelerationFactor  = m_acc;
        m_usdStatus.delay               = m_delay;
        m_usdStatus.minimumFrequency    = m_fmin;
        m_usdStatus.maximumFrequency    = m_fmax;
    }

    // Acquire shared socket — may not be connected yet, that is fine
    const std::string lanName = m_name.substr(0, m_name.rfind('/'));
    try
    {
        m_socket = &ActiveSurface::Socket::getInstance(lanName, m_containerServices);
    }
    catch(...)
    {
    }
}

bool USDCore::isInitialized() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_initialized;
}

bool USDCore::needsReset()
{
    if(!m_socket || !m_socket->isConnected())
    {
        return false;
    }

    auto verRaw = sendReceive(ActiveSurface::CommandLibrary::get_version(m_addr), FRAME_OVERHEAD + 1);
    m_softVer = static_cast<long>(verRaw[3]);
    m_usdStatus.softwareVersion = m_softVer;

    auto stInit = sendReceive(ActiveSurface::CommandLibrary::get_status(m_addr), FRAME_OVERHEAD + 3);
    m_hwStatus = static_cast<int>(ActiveSurface::CommandLibrary::params_to_uint(std::vector<uint8_t>(stInit.begin() + 3, stInit.end() - 1), 0, 3));

    return !(m_calibrate && (m_hwStatus & ENBL));
}

void USDCore::sendUnicastConfig()
{
    if(!m_socket || !m_socket->isConnected())
    {
        return;
    }

    sendCommand(ActiveSurface::CommandLibrary::set_resolution(m_rs, m_addr));
    setFmin(m_fmin);
    setFmax(m_fmax);
    setAcc(m_acc);
    setDelay(m_delay);
    setUBits(m_uBits);
}

void USDCore::readHardwareDetails()
{
    if(!m_socket || !m_socket->isConnected())
    {
        return;
    }

    auto typeRaw = sendReceive(ActiveSurface::CommandLibrary::get_driver_type(m_addr), FRAME_OVERHEAD + 1);
    m_type = static_cast<long>(typeRaw[3]);
    m_usdStatus.type = m_type;

    readHWStatus(); 

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_usdStatus.available = true;
        m_available   = true;
        m_initialized = true;
    }
}

// ---------------------------------------------------------------------------
// Tracking commands
// ---------------------------------------------------------------------------

void USDCore::setCoefficients(const std::array<std::vector<double>, 5>& coefficients, const double& parabolicOffset)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_coefficients = coefficients;
    m_parabolicOffset = parabolicOffset;
}

long USDCore::computePosition(double elevation, ActiveSurface::TASProfile profile) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if(profile == ActiveSurface::AS_PARK)
    {
        return 0;
    }

    if(m_coefficients[0].empty())
    {
        // Coefficients not yet loaded
        throw ASErrors::CoefficientsNotLoadedExImpl(__FILE__, __LINE__, "USDCore::computePosition()");
    }

    static const double sectionBounds[6] = { 15.0, 30.0, 45.0, 60.0, 75.0, 90.0 };

    double evalEl = elevation;
    if(profile == ActiveSurface::AS_SHAPED_FIXED || profile == ActiveSurface::AS_PARABOLIC_FIXED)
    {
        evalEl = 45.0;
    }
    else
    {
        if(evalEl < sectionBounds[0])
        {
            evalEl = sectionBounds[0];
        }
        if(evalEl > sectionBounds[5])
        {
            evalEl = sectionBounds[5];
        }
    }

    size_t sectionIdx = 0;
    for(size_t i = 0; i < 5; i++)
    {
        if(evalEl >= sectionBounds[i] && evalEl < sectionBounds[i + 1])
        {
            sectionIdx = i;
            break;
        }
    }

    const std::vector<double>& coeffs = m_coefficients[sectionIdx];
    double posMM = 0.0;
    for(size_t i = 0; i < coeffs.size(); i++)
    {
        posMM += coeffs[i] * std::pow(evalEl, static_cast<double>(i));
    }

    if(profile == ActiveSurface::AS_PARABOLIC || profile == ActiveSurface::AS_PARABOLIC_FIXED)
    {
        posMM += m_parabolicOffset;
    }

    return std::clamp(static_cast<long>(posMM * MM2STEP), m_bottom, m_top);
}

// ---------------------------------------------------------------------------
// Motion commands
// ---------------------------------------------------------------------------

void USDCore::stop()
{
    sendCommand(ActiveSurface::CommandLibrary::soft_stop(m_addr));
}

void USDCore::up()
{
    sendCommand(ActiveSurface::CommandLibrary::rotate(1, m_addr));
}

void USDCore::down()
{
    sendCommand(ActiveSurface::CommandLibrary::rotate(-1, m_addr));
}

void USDCore::move(int32_t incr)
{
    sendCommand(ActiveSurface::CommandLibrary::set_relative_position_steps(incr, m_addr));
}

void USDCore::refPos()
{
    sendCommand(ActiveSurface::CommandLibrary::set_absolute_position_steps(0, m_addr));
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cmdPos = 0;
    m_lastCmdStep = 0;
    m_usdStatus.commandedPosition = 0;
}

void USDCore::stow()
{
    stop();
    sendCommand(ActiveSurface::CommandLibrary::set_absolute_position_steps(static_cast<int32_t>(m_bottom), m_addr));
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cmdPos = m_bottom;
    m_lastCmdStep = static_cast<int>(m_bottom);
    m_usdStatus.commandedPosition = static_cast<int>(m_bottom);
}

void USDCore::setup()
{
    stop();
    refPos();
}

void USDCore::top()
{
    sendCommand(ActiveSurface::CommandLibrary::set_absolute_position_steps(static_cast<int32_t>(m_top), m_addr));
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cmdPos = m_top;
    m_lastCmdStep = static_cast<int>(m_top);
    m_usdStatus.commandedPosition = static_cast<int>(m_top);
}

void USDCore::bottom()
{
    sendCommand(ActiveSurface::CommandLibrary::set_absolute_position_steps(static_cast<int32_t>(m_bottom), m_addr));
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cmdPos = m_bottom;
    m_lastCmdStep = static_cast<int>(m_bottom);
    m_usdStatus.commandedPosition = static_cast<int>(m_bottom);
}

// ---------------------------------------------------------------------------
// Hardware management
// ---------------------------------------------------------------------------

void USDCore::hardwareReset()
{
    ACS_TRACE("USDCore::hardwareReset()");

    sendCommand(ActiveSurface::CommandLibrary::soft_reset(m_addr));
    IRA::CIRATools::Wait(0, 200000); 

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_calibrate = false;
    }

    if(m_containerServices != nullptr)
    {
        if(!IRA::CIRATools::setDBValue(m_containerServices, "calibrate", static_cast<long>(0), "alma/", m_name.c_str()))
        {
            ASErrors::CDBAccessErrorExImpl ex(__FILE__, __LINE__, "USDCore::hardwareReset()");
            ex.setFieldName("calibrate");
            ex.log(LM_WARNING); 
        }
    }
}

void USDCore::reset()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_initialized = false;
        m_available   = false;
    }

    hardwareReset();

    sendCommand(ActiveSurface::CommandLibrary::set_resolution(m_rs, m_addr));
    setFmin(m_fmin);
    setFmax(m_fmax);
    setAcc(m_acc);
    setDelay(m_delay);
    setUBits(m_uBits);

    readHWStatus();

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_initialized = true;
    }

    ACS_SHORT_LOG((LM_INFO, "USDCore %s: reset complete.", m_name.c_str()));
}

void USDCore::calibrate()
{
    long cammaBegin = 0, cammaEnd = 0;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_calibrate = false;
        m_cammaLen  = m_cammaPos = -1;
    }

    sendCommand(ActiveSurface::CommandLibrary::soft_stop(m_addr));
    IRA::CIRATools::Wait(1, 0);

    sendCommand(ActiveSurface::CommandLibrary::set_reference_position_steps(m_step_giro, m_addr));

    sendCommand(ActiveSurface::CommandLibrary::set_max_frequency(100, m_addr));
    sendCommand(ActiveSurface::CommandLibrary::set_stop_io(0x09, m_addr));
    sendCommand(ActiveSurface::CommandLibrary::rotate(-1, m_addr));
    IRA::CIRATools::Wait(3, 0);

    {
        auto stRaw = sendReceive(ActiveSurface::CommandLibrary::get_status(m_addr), FRAME_OVERHEAD + 3);
        int hw = static_cast<int>(ActiveSurface::CommandLibrary::params_to_uint(std::vector<uint8_t>(stRaw.begin() + 3, stRaw.end() - 1), 0, 3));
        if(hw & MRUN)
        {
            sendCommand(ActiveSurface::CommandLibrary::soft_stop(m_addr));
            throw ASErrors::USDStillRunningExImpl(__FILE__, __LINE__, "USDCore::calibrate()");
        }
        auto posRaw = sendReceive(ActiveSurface::CommandLibrary::get_position(m_addr), FRAME_OVERHEAD + 4);
        cammaBegin = ActiveSurface::CommandLibrary::microsteps_to_steps(
            static_cast<long>(ActiveSurface::CommandLibrary::params_to_int(std::vector<uint8_t>(posRaw.begin() + 3, posRaw.end() - 1), 0, 4))
        );
    }

    sendCommand(ActiveSurface::CommandLibrary::set_stop_io(0x00, m_addr));
    move(-10);
    IRA::CIRATools::Wait(1, 0);

    sendCommand(ActiveSurface::CommandLibrary::set_stop_io(0x01, m_addr));
    sendCommand(ActiveSurface::CommandLibrary::rotate(-1, m_addr));
    IRA::CIRATools::Wait(3, 0);

    {
        auto stRaw = sendReceive(ActiveSurface::CommandLibrary::get_status(m_addr), FRAME_OVERHEAD + 3);
        int hw = static_cast<int>(ActiveSurface::CommandLibrary::params_to_uint(std::vector<uint8_t>(stRaw.begin() + 3, stRaw.end() - 1), 0, 3));
        if(hw & MRUN)
        {
            sendCommand(ActiveSurface::CommandLibrary::soft_stop(m_addr));
            throw ASErrors::USDStillRunningExImpl(__FILE__, __LINE__, "USDCore::calibrate()");
        }
        auto posRaw = sendReceive(ActiveSurface::CommandLibrary::get_position(m_addr), FRAME_OVERHEAD + 4);
        cammaEnd = ActiveSurface::CommandLibrary::microsteps_to_steps(
            static_cast<long>(ActiveSurface::CommandLibrary::params_to_int(std::vector<uint8_t>(posRaw.begin() + 3, posRaw.end() - 1), 0, 4))
        );
    }

    sendCommand(ActiveSurface::CommandLibrary::set_stop_io(0x00, m_addr));
    sendCommand(ActiveSurface::CommandLibrary::set_max_frequency(500, m_addr));

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_cammaLen  = static_cast<int>(cammaBegin - cammaEnd);
        m_cammaPos  = static_cast<int>(cammaEnd + m_cammaLen / 2);
        m_cammaLenD = m_cammaLen  * m_step2deg;
        m_cammaPosD = (m_step_giro - cammaEnd - m_cammaLen / 2) * m_step2deg;
    }

    setCmdPos(m_cammaPos);
    IRA::CIRATools::Wait(1, 0);

    sendCommand(ActiveSurface::CommandLibrary::set_reference_position_steps(m_top, m_addr));

    m_calibrate = true;

    ACS_SHORT_LOG((LM_INFO, "USDCore %s: calibration complete. cammaLen=%d cammaPos=%d", m_name.c_str(), m_cammaLen, m_cammaPos));
}

void USDCore::calVer()
{
    if(!m_calibrate)
    {
        throw ASErrors::USDunCalibratedExImpl(__FILE__, __LINE__, "USDCore::calVer()");
    }

    setCmdPos(m_top);
    if(stillRunning(m_top))
    {
        sendCommand(ActiveSurface::CommandLibrary::soft_stop(m_addr));
        throw ASErrors::USDStillRunningExImpl(__FILE__, __LINE__, "USDCore::calVer()");
    }
    if(!chkCal())
    {
        std::lock_guard<std::mutex> l(m_mutex);
        m_calibrate = false;
        return;
    }

    move(-(m_cammaLen / 2 + 5));
    IRA::CIRATools::Wait(0, 500000);
    if(!chkCal())
    {
        std::lock_guard<std::mutex> l(m_mutex);
        m_calibrate = false;
        return;
    }

    setCmdPos(m_bottom);
    if(stillRunning(m_bottom))
    {
        sendCommand(ActiveSurface::CommandLibrary::soft_stop(m_addr));
        throw ASErrors::USDStillRunningExImpl(__FILE__, __LINE__, "USDCore::calVer()");
    }
    if(!chkCal())
    {
        std::lock_guard<std::mutex> l(m_mutex);
        m_calibrate = false;
        return;
    }

    setCmdPos(0);
    if(stillRunning(0))
    {
        sendCommand(ActiveSurface::CommandLibrary::soft_stop(m_addr));
        throw ASErrors::USDStillRunningExImpl(__FILE__, __LINE__, "USDCore::calVer()");
    }
    if(!chkCal())
    {
        std::lock_guard<std::mutex> l(m_mutex);
        m_calibrate = false;
        return;
    }

    ACS_SHORT_LOG((LM_INFO, "USDCore %s: calibration verified.", m_name.c_str()));
}

void USDCore::writeCalibration(double& cammaLenD, double& cammaPosD, bool& calibrated)
{
    if(!IRA::CIRATools::setDBValue(m_containerServices, "cammaLen", m_cammaLenD, "alma/", m_name.c_str()) ||
        !IRA::CIRATools::setDBValue(m_containerServices, "cammaPos", m_cammaPosD, "alma/", m_name.c_str()) ||
        !IRA::CIRATools::setDBValue(m_containerServices, "calibrate", static_cast<long>(m_calibrate), "alma/", m_name.c_str()))
    {
        ASErrors::CDBAccessErrorExImpl ex(__FILE__, __LINE__, "USDCore::writeCalibration()");
        ex.setFieldName("cammaLen/cammaPos/calibrate");
        throw ex;
    }
    cammaLenD  = m_cammaLenD;
    cammaPosD  = m_cammaPosD;
    calibrated = m_calibrate;
}

bool USDCore::isReady() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return (m_hwStatus & READY) != 0;
}

void USDCore::setDelayedExecutionMode(bool enable)
{
    uint8_t configByte = enable ? 0x80 : 0x00;
    sendCommand(ActiveSurface::CommandLibrary::toggle_delayed_execution(configByte, m_addr));
}

// ---------------------------------------------------------------------------
// Status
// ---------------------------------------------------------------------------

void USDCore::readHWStatus()
{
    auto stRaw = sendReceive(ActiveSurface::CommandLibrary::get_status(m_addr), FRAME_OVERHEAD + 3);
    auto posRaw = sendReceive(ActiveSurface::CommandLibrary::get_position(m_addr), FRAME_OVERHEAD + 4);

    std::lock_guard<std::mutex> lock(m_mutex);

    int rawStatus = static_cast<int>(ActiveSurface::CommandLibrary::params_to_uint(std::vector<uint8_t>(stRaw.begin() + 3, stRaw.end() - 1), 0, 3));
    if(m_calibrate)
    {
        rawStatus |= CAL;
    }
    else
    {
        rawStatus &= ~CAL;
    }

    m_hwStatus = rawStatus;
    m_usdStatus.status = m_hwStatus;

    m_actPos = ActiveSurface::CommandLibrary::microsteps_to_steps(
        static_cast<long>(ActiveSurface::CommandLibrary::params_to_int(std::vector<uint8_t>(posRaw.begin() + 3, posRaw.end() - 1), 0, 4))
    );
    m_usdStatus.currentPosition = m_actPos;
}

void USDCore::readActPos()
{
    auto posRaw = sendReceive(ActiveSurface::CommandLibrary::get_position(m_addr), FRAME_OVERHEAD + 4);

    std::lock_guard<std::mutex> lock(m_mutex);

    m_actPos = ActiveSurface::CommandLibrary::microsteps_to_steps(
        static_cast<long>(ActiveSurface::CommandLibrary::params_to_int(std::vector<uint8_t>(posRaw.begin() + 3, posRaw.end() - 1), 0, 4))
    );
    m_usdStatus.currentPosition = m_actPos;
}

// ---------------------------------------------------------------------------
// State accessors
// ---------------------------------------------------------------------------

long USDCore::getCmdPos()  const { std::lock_guard<std::mutex> l(m_mutex); return m_cmdPos;  }
long USDCore::getActPos()  const { std::lock_guard<std::mutex> l(m_mutex); return m_actPos;  }
int  USDCore::getHwStatus()const { std::lock_guard<std::mutex> l(m_mutex); return m_hwStatus;}
long USDCore::getSoftVer() const { std::lock_guard<std::mutex> l(m_mutex); return m_softVer; }
long USDCore::getType()    const { std::lock_guard<std::mutex> l(m_mutex); return m_type;    }
long USDCore::getFmin()    const { std::lock_guard<std::mutex> l(m_mutex); return m_fmin;    }
long USDCore::getFmax()    const { std::lock_guard<std::mutex> l(m_mutex); return m_fmax;    }
long USDCore::getAcc()     const { std::lock_guard<std::mutex> l(m_mutex); return m_acc;     }
long USDCore::getDelay()   const { std::lock_guard<std::mutex> l(m_mutex); return m_delay;   }
long USDCore::getUBits()   const { std::lock_guard<std::mutex> l(m_mutex); return m_uBits;   }
bool USDCore::isAvailable()const { std::lock_guard<std::mutex> l(m_mutex); return m_available;}

ActiveSurface::USDStatus USDCore::getUSDStatus() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_usdStatus;
}

void USDCore::setCmdPos(long pos)
{
    sendCommand(ActiveSurface::CommandLibrary::set_absolute_position_steps(static_cast<int32_t>(pos), m_addr));
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cmdPos = pos;
    m_usdStatus.commandedPosition = static_cast<int>(pos);
}

void USDCore::setFmin(long v)
{
    sendCommand(ActiveSurface::CommandLibrary::set_min_frequency(static_cast<uint16_t>(v), m_addr));
    std::lock_guard<std::mutex> l(m_mutex);
    m_fmin = v;
    m_usdStatus.minimumFrequency = static_cast<int>(v);
}

void USDCore::setFmax(long v)
{
    sendCommand(ActiveSurface::CommandLibrary::set_max_frequency(static_cast<uint16_t>(v), m_addr));
    std::lock_guard<std::mutex> l(m_mutex);
    m_fmax = v;
    m_usdStatus.maximumFrequency = static_cast<int>(v);
}

void USDCore::setAcc(long v)
{
    sendCommand(ActiveSurface::CommandLibrary::set_slope_multiplier(static_cast<uint8_t>(v), m_addr));
    std::lock_guard<std::mutex> l(m_mutex);
    m_acc = v;
    m_usdStatus.accelerationFactor = static_cast<int>(v);
}

void USDCore::setDelay(long v)
{
    sendCommand(ActiveSurface::CommandLibrary::set_response_delay(static_cast<uint8_t>(v), m_addr));
    std::lock_guard<std::mutex> l(m_mutex);
    m_delay = v;
    m_usdStatus.delay = static_cast<int>(v);
}

void USDCore::setUBits(long v)
{
    sendCommand(ActiveSurface::CommandLibrary::set_io_pins(static_cast<uint8_t>(v), m_addr));
    std::lock_guard<std::mutex> l(m_mutex);
    m_uBits = v;
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

bool USDCore::parseName(const std::string& name, uint8_t& sector, uint8_t& lan, uint8_t& addr)
{
    const auto posSector = name.find("SECTOR");
    const auto posLan    = name.find("LAN");
    const auto posUsd    = name.find("USD");

    if(posSector == std::string::npos ||
       posLan    == std::string::npos ||
       posUsd    == std::string::npos)
    {
        return false;
    }

    sector = static_cast<uint8_t>(std::atoi(name.c_str() + posSector + 6));
    lan    = static_cast<uint8_t>(std::atoi(name.c_str() + posLan    + 3));
    addr   = static_cast<uint8_t>(std::atoi(name.c_str() + posUsd    + 3));

    return sector > 0 && lan > 0 && addr > 0;
}

template <typename T>
T USDCore::readCDB(const char* field)
{
    T value{};
    if(!IRA::CIRATools::getDBValue(m_containerServices, field, value, "alma/", m_name.c_str()))
    {
        ASErrors::CDBAccessErrorExImpl ex(__FILE__, __LINE__, "USDCore::readCDB()");
        ex.setFieldName(field);
        throw acsErrTypeLifeCycle::LifeCycleExImpl(ex, __FILE__, __LINE__, "USDCore::readCDB()");
    }
    return value;
}

void USDCore::sendCommand(const std::vector<uint8_t>& frame)
{
    if(!m_socket)
    {
        throw ASErrors::LANConnectionErrorExImpl(__FILE__, __LINE__, "USDCore::sendCommand()");
    }

    long delay;
    {
        std::lock_guard<std::mutex> l(m_mutex);
        delay = m_delay;
    }

    try
    {
        if(delay != 255)
        {
            m_socket->sendReceive(frame, 1, delay);
            std::lock_guard<std::mutex> l(m_mutex);
            m_available = true;
            m_usdStatus.available = true;
            m_hwStatus &= ~UNAV;
            m_usdStatus.status &= ~UNAV;
        }
        else
        {
            m_socket->send(frame);
        }
    }
    catch(ASErrors::ASErrorsExImpl& impl)
    {
        impl.log(LM_WARNING);
        std::lock_guard<std::mutex> l(m_mutex);
        m_available = false;
        m_usdStatus.available = false;
        m_hwStatus |= UNAV;
        m_usdStatus.status |= UNAV;
        throw;
    }
}

std::vector<uint8_t> USDCore::sendReceive(const std::vector<uint8_t>& frame, size_t responseBytes)
{
    if(!m_socket)
    {
        throw ASErrors::LANConnectionErrorExImpl(__FILE__, __LINE__, "USDCore::sendReceive()");
    }

    long delay;
    {
        std::lock_guard<std::mutex> l(m_mutex);
        delay = m_delay;
    }

    try
    {
        std::vector<uint8_t> response = m_socket->sendReceive(frame, responseBytes, delay);
        std::lock_guard<std::mutex> l(m_mutex);
        m_available = true;
        m_usdStatus.available = true;
        m_hwStatus &= ~UNAV;
        m_usdStatus.status &= ~UNAV;
        return response;
    }
    catch(ASErrors::ASErrorsExImpl& impl)
    {
        impl.log(LM_WARNING);
        std::lock_guard<std::mutex> l(m_mutex);
        m_available = false;
        m_usdStatus.available = false;
        m_hwStatus |= UNAV;
        m_usdStatus.status |= UNAV;
        throw;
    }
}

bool USDCore::stillRunning(long targetPos)
{
    long distSteps = std::abs(targetPos - m_actPos);
    long fmax;
    {
        std::lock_guard<std::mutex> l(m_mutex);
        fmax = m_fmax;
    }
    int timeout = distSteps / std::max(1L, fmax) + 2;

    for(time_t end = std::time(nullptr) + timeout; std::time(nullptr) < end; IRA::CIRATools::Wait(0, 500000))
    {
        try
        {
            auto stRaw = sendReceive(ActiveSurface::CommandLibrary::get_status(m_addr), FRAME_OVERHEAD + 3);
            int hw = static_cast<int>(ActiveSurface::CommandLibrary::params_to_uint(std::vector<uint8_t>(stRaw.begin() + 3, stRaw.end() - 1), 0, 3));
            if(!(hw & MRUN))
            {
                return false;
            }
        }
        catch(...)
        {
            break;
        }
    }

    std::lock_guard<std::mutex> l(m_mutex);
    return (m_hwStatus & MRUN) != 0;
}

bool USDCore::chkCal()
{
    try
    {
        auto posRaw = sendReceive(ActiveSurface::CommandLibrary::get_position(m_addr), FRAME_OVERHEAD + 4);
        long pos = static_cast<long>(ActiveSurface::CommandLibrary::params_to_int(std::vector<uint8_t>(posRaw.begin() + 3, posRaw.end() - 1), 0, 4));

        auto stRaw = sendReceive(ActiveSurface::CommandLibrary::get_status(m_addr), FRAME_OVERHEAD + 3);
        int hw = static_cast<int>(ActiveSurface::CommandLibrary::params_to_uint(std::vector<uint8_t>(stRaw.begin() + 3, stRaw.end() - 1), 0, 3));

        int fgiro = static_cast<int>(ActiveSurface::CommandLibrary::microsteps_to_steps(pos)) % m_step_giro;
        if(fgiro > m_step_giro / 2)
        {
            fgiro = m_step_giro - fgiro;
        }

        if(fgiro < m_cammaLen / 2 && (hw & CAMM))
        {
            return true;
        }
        if(fgiro > m_cammaLen / 2 && !(hw & CAMM))
        {
            return true;
        }
        return false;
    }
    catch(...)
    {
        return false;
    }
}

} // namespace ActiveSurface
