#include "SRTMinorServoBossImpl.h"

using namespace MinorServo;
namespace SP = SimpleParser;

_IRA_LOGFILTER_DECLARE;

SRTMinorServoBossImpl::SRTMinorServoBossImpl(const ACE_CString& component_name, maci::ContainerServices* container_services) :
    CharacteristicComponentImpl(component_name, container_services),
    m_component_name(std::string(component_name.c_str())),
    m_core_ptr(std::make_shared<SRTMinorServoBossCore>(*this)),
    m_core(*m_core_ptr),
    m_parser(SP::CParser<SRTMinorServoBossImpl>(this, 2)),
    m_connected_ptr(this),
    m_status_ptr(this),
    m_ready_ptr(this),
    m_actual_setup_ptr(this),
    m_motion_info_ptr(this),
    m_starting_ptr(this),
    m_as_configuration_ptr(this),
    m_elevation_tracking_ptr(this),
    m_scan_active_ptr(this),
    m_scanning_ptr(this),
    m_tracking_ptr(this),
    m_current_configuration_ptr(this),
    m_simulation_enabled_ptr(this),
    m_plc_time_ptr(this),
    m_plc_version_ptr(this),
    m_control_ptr(this),
    m_power_ptr(this),
    m_emergency_ptr(this),
    m_gregorian_cover_ptr(this),
    m_air_blade_ptr(this),
    m_last_executed_command_ptr(this)
{
    AUTO_TRACE("SRTMinorServoBossImpl::SRTMinorServoBossImpl()");

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossImpl::SRTMinorServoBossImpl()", (LM_NOTICE, "COMPONENT CREATED"));
}

SRTMinorServoBossImpl::~SRTMinorServoBossImpl()
{
    AUTO_TRACE("SRTMinorServoBossImpl::~SRTMinorServoBossImpl()");

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossImpl::SRTMinorServoBossImpl()", (LM_NOTICE, "COMPONENT DESTROYED"));
}

void SRTMinorServoBossImpl::initialize()
{
    AUTO_TRACE("SRTMinorServoBossImpl::initialize()");
    _IRA_LOGFILTER_ACTIVATE(10000000, 20000000);

    try
    {
        m_connected_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((m_component_name + ":connected").c_str(), getComponent(),
                new MSGenericDevIO<Management::TBoolean, std::atomic<Management::TBoolean>>(m_core.m_socket_connected), true);
        m_status_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus), POA_Management::ROTSystemStatus>((m_component_name + ":status").c_str(), getComponent(),
                new MSGenericDevIO<Management::TSystemStatus, std::atomic<Management::TSystemStatus>>(m_core.m_subsystem_status), true);
        m_ready_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((m_component_name + ":ready").c_str(), getComponent(),
                new MSGenericDevIO<Management::TBoolean, std::atomic<Management::TBoolean>>(m_core.m_ready), true);
        m_actual_setup_ptr = new baci::ROstring((m_component_name + ":actualSetup").c_str(), getComponent(),
                new MSGenericDevIO<ACE_CString, std::string>(m_core.m_actual_setup), true);
        m_motion_info_ptr = new baci::ROstring((m_component_name + ":motionInfo").c_str(), getComponent(),
                new MSMotionInfoDevIO(m_core.m_motion_status, m_core.m_status, m_core.m_scanning, m_core.m_current_scan), true);
        m_starting_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((m_component_name + ":starting").c_str(), getComponent(),
                new MSGenericDevIO<Management::TBoolean, std::atomic<Management::TBoolean>>(m_core.m_starting), true);
        m_as_configuration_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((m_component_name + ":asConfiguration").c_str(), getComponent(),
                new MSGenericDevIO<Management::TBoolean, std::atomic<Management::TBoolean>>(m_core.m_as_configuration), true);
        m_elevation_tracking_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((m_component_name + ":elevationTrack").c_str(), getComponent(),
                new MSGenericDevIO<Management::TBoolean, std::atomic<Management::TBoolean>>(m_core.m_elevation_tracking_enabled), true);
        m_scan_active_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((m_component_name + ":scanActive").c_str(), getComponent(),
                new MSGenericDevIO<Management::TBoolean, std::atomic<Management::TBoolean>>(m_core.m_scan_active), true);
        m_scanning_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((m_component_name + ":scanning").c_str(), getComponent(),
                new MSGenericDevIO<Management::TBoolean, std::atomic<Management::TBoolean>>(m_core.m_scanning), true);
        m_tracking_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((m_component_name + ":tracking").c_str(), getComponent(),
                new MSGenericDevIO<Management::TBoolean, std::atomic<Management::TBoolean>>(m_core.m_tracking), true);
        m_current_configuration_ptr = new ROEnumImpl<ACS_ENUM_T(SRTMinorServoFocalConfiguration), POA_MinorServo::ROSRTMinorServoFocalConfiguration>((m_component_name + ":current_configuration").c_str(), getComponent(), new MSAnswerMapDevIO<SRTMinorServoFocalConfiguration, SRTMinorServoGeneralStatus>("current_configuration", m_core.m_status, &SRTMinorServoGeneralStatus::getFocalConfiguration), true);
        m_simulation_enabled_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((m_component_name + ":simulation_enabled").c_str(), getComponent(),
                new MSAnswerMapDevIO<Management::TBoolean, SRTMinorServoGeneralStatus>("simulation_enabled", m_core.m_status, &SRTMinorServoGeneralStatus::isSimulationEnabled), true);
        m_plc_time_ptr = new baci::ROdouble((m_component_name + ":plc_time").c_str(), getComponent(),
                new MSAnswerMapDevIO<CORBA::Double, SRTMinorServoGeneralStatus>("plc_time", m_core.m_status, &SRTMinorServoGeneralStatus::getPLCTime), true);
        m_plc_version_ptr = new baci::ROstring((m_component_name + ":plc_version").c_str(), getComponent(),
                new MSAnswerMapDevIO<ACE_CString, SRTMinorServoGeneralStatus>("plc_version", m_core.m_status, &SRTMinorServoGeneralStatus::getPLCVersion), true);
        m_control_ptr = new ROEnumImpl<ACS_ENUM_T(SRTMinorServoControlStatus), POA_MinorServo::ROSRTMinorServoControlStatus>((m_component_name + ":control").c_str(), getComponent(),
                new MSAnswerMapDevIO<SRTMinorServoControlStatus, SRTMinorServoGeneralStatus>("control", m_core.m_status, &SRTMinorServoGeneralStatus::getControl), true);
        m_power_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((m_component_name + ":power").c_str(), getComponent(),
                new MSAnswerMapDevIO<Management::TBoolean, SRTMinorServoGeneralStatus>("power", m_core.m_status, &SRTMinorServoGeneralStatus::hasPower), true);
        m_emergency_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((m_component_name + ":emergency").c_str(), getComponent(),
                new MSAnswerMapDevIO<Management::TBoolean, SRTMinorServoGeneralStatus>("emergency", m_core.m_status, &SRTMinorServoGeneralStatus::emergencyPressed), true);
        m_gregorian_cover_ptr = new ROEnumImpl<ACS_ENUM_T(SRTMinorServoGregorianCoverStatus), POA_MinorServo::ROSRTMinorServoGregorianCoverStatus>((m_component_name + ":gregorian_cover").c_str(), getComponent(), new MSAnswerMapDevIO<SRTMinorServoGregorianCoverStatus, SRTMinorServoGeneralStatus>("gregorian_cover", m_core.m_status, &SRTMinorServoGeneralStatus::getGregorianCoverPosition), true);
        m_air_blade_ptr = new ROEnumImpl<ACS_ENUM_T(SRTMinorServoGregorianAirBladeStatus), POA_MinorServo::ROSRTMinorServoGregorianAirBladeStatus>((m_component_name + ":air_blade").c_str(), getComponent(), new MSAnswerMapDevIO<SRTMinorServoGregorianAirBladeStatus, SRTMinorServoGeneralStatus>("air_blade", m_core.m_status, &SRTMinorServoGeneralStatus::getGregorianAirBladeStatus), true);
        m_last_executed_command_ptr = new baci::ROdouble((m_component_name + ":last_executed_command").c_str(), getComponent(),
                new MSAnswerMapDevIO<CORBA::Double, SRTMinorServoGeneralStatus>("last_executed_command", m_core.m_status, &SRTMinorServoGeneralStatus::getLastExecutedCommand), true);
    }
    catch(std::bad_alloc& ba)
    {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, ex, "SRTMinorServoBossImpl::initialize()");
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossImpl::initialize()", (LM_NOTICE, "PROPERTIES INITIALIZED"));

    m_parser.add("servoSetup", new SP::function1<SRTMinorServoBossImpl, SP::non_constant, SP::void_type, SP::I<SP::string_type>>(this, &SRTMinorServoBossImpl::setup), 1);
    m_parser.add("servoPark", new SP::function0<SRTMinorServoBossImpl, SP::non_constant, SP::void_type>(this, &SRTMinorServoBossImpl::park), 0);
    m_parser.add("setServoElevationTracking", new SP::function1<SRTMinorServoBossImpl, SP::non_constant, SP::void_type, SP::I<SP::string_type>>(this, &SRTMinorServoBossImpl::setElevationTracking), 1);
    m_parser.add("setServoASConfiguration", new SP::function1<SRTMinorServoBossImpl, SP::non_constant, SP::void_type, SP::I<SP::string_type>>(this, &SRTMinorServoBossImpl::setASConfiguration), 1);
    m_parser.add("setServoOffset", new SP::function2<SRTMinorServoBossImpl, SP::non_constant, SP::void_type, SP::I<SP::string_type>, SP::I<SP::double_type>>(this, &SRTMinorServoBossImpl::setUserOffset), 2);
    m_parser.add("clearServoOffsets", new SP::function0<SRTMinorServoBossImpl, SP::non_constant, SP::void_type>(this, &SRTMinorServoBossImpl::clearOffsets), 0);
    m_parser.add("setGregorianCoverPosition", new SP::function1<SRTMinorServoBossImpl, SP::non_constant, SP::void_type, SP::I<SP::string_type>>(this, &SRTMinorServoBossImpl::setGregorianCoverPosition), 1);
    m_parser.add("setGregorianAirBladeStatus", new SP::function1<SRTMinorServoBossImpl, SP::non_constant, SP::void_type, SP::I<SP::string_type>>(this, &SRTMinorServoBossImpl::setGregorianAirBladeStatus), 1);

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossImpl::initialize()", (LM_NOTICE, "PARSER INITIALIZED"));

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossImpl::initialize()", (LM_NOTICE, "COMPONENT INITIALIZED"));
}

void SRTMinorServoBossImpl::execute()
{
    AUTO_TRACE("SRTMinorServoBossImpl::execute()");

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossImpl::execute()", (LM_NOTICE, "STARTING TO ACCEPT FUNCTIONAL CALLS"));
}

void SRTMinorServoBossImpl::cleanUp()
{
    AUTO_TRACE("SRTMinorServoBossImpl::cleanUp()");
    ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossImpl::cleanUp()", (LM_NOTICE, "CLEANING UP RESOURCES"));
    stopPropertiesMonitoring();
    _IRA_LOGFILTER_FLUSH;
    _IRA_LOGFILTER_DESTROY;
    CharacteristicComponentImpl::cleanUp();
}

void SRTMinorServoBossImpl::aboutToAbort()
{
    AUTO_TRACE("SRTMinorServoBossImpl::aboutToAbort()");
    ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossImpl::aboutToAbort()", (LM_NOTICE, "ATTEMPTING TO CLEAN UP RESOURCES"));
    stopPropertiesMonitoring();
    _IRA_LOGFILTER_FLUSH;
    _IRA_LOGFILTER_DESTROY;
    CharacteristicComponentImpl::aboutToAbort();
}

void SRTMinorServoBossImpl::setup(const char* configuration)
{
    AUTO_TRACE("SRTMinorServoBossImpl::setup()");
    m_core.setup(std::string(configuration));
}

void SRTMinorServoBossImpl::park()
{
    AUTO_TRACE("SRTMinorServoBossImpl::park()");
    m_core.park();
}

CORBA::Boolean SRTMinorServoBossImpl::isElevationTrackingEn()
{
    AUTO_TRACE("SRTMinorServoBossImpl::isElevationTrackingEn()");
    return m_core.m_elevation_tracking_enabled.load() == Management::MNG_TRUE ? true : false;
}

CORBA::Boolean SRTMinorServoBossImpl::isElevationTracking()
{
    AUTO_TRACE("SRTMinorServoBossImpl::isElevationTracking()");
    return m_core.m_elevation_tracking.load() == Management::MNG_TRUE ? true : false;
}

CORBA::Boolean SRTMinorServoBossImpl::isTracking()
{
    AUTO_TRACE("SRTMinorServoBossImpl::isTracking()");
    return m_core.m_tracking.load() == Management::MNG_TRUE ? true : false;
}

CORBA::Boolean SRTMinorServoBossImpl::isStarting()
{
    AUTO_TRACE("SRTMinorServoBossImpl::isStarting()");
    return m_core.m_starting.load() == Management::MNG_TRUE ? true : false;
}

CORBA::Boolean SRTMinorServoBossImpl::isASConfiguration()
{
    AUTO_TRACE("SRTMinorServoBossImpl::isASConfiguration()");
    return m_core.m_as_configuration.load() == Management::MNG_TRUE ? true : false;
}

CORBA::Boolean SRTMinorServoBossImpl::isParking()
{
    AUTO_TRACE("SRTMinorServoBossImpl::isParking()");
    return (m_core.m_starting.load() == Management::MNG_TRUE && m_core.m_commanded_configuration.load() == CONFIGURATION_PARK);
}

CORBA::Boolean SRTMinorServoBossImpl::isReady()
{
    AUTO_TRACE("SRTMinorServoBossImpl::isReady()");
    return m_core.m_ready.load() == Management::MNG_TRUE ? true : false;
}

CORBA::Boolean SRTMinorServoBossImpl::isScanning()
{
    AUTO_TRACE("SRTMinorServoBossImpl::isScanning()");
    return m_core.m_scanning.load() == Management::MNG_TRUE ? true : false;
}

CORBA::Boolean SRTMinorServoBossImpl::isScanActive()
{
    AUTO_TRACE("SRTMinorServoBossImpl::isScanActive()");
    return m_core.m_scan_active.load() == Management::MNG_TRUE ? true : false;
}

char* SRTMinorServoBossImpl::getActualSetup()
{
    AUTO_TRACE("SRTMinorServoBossImpl::getActualSetup()");
    return CORBA::string_dup(m_core.m_actual_setup.c_str());
}

char* SRTMinorServoBossImpl::getCommandedSetup()
{
    AUTO_TRACE("SRTMinorServoBossImpl::getCommandedSetup()");
    return CORBA::string_dup(m_core.m_commanded_setup.c_str());
}

CORBA::Double SRTMinorServoBossImpl::getCentralScanPosition()
{
    AUTO_TRACE("SRTMinorServoBossImpl::getCentralScanPosition()");

    if(m_core.m_scan_active.load() == Management::MNG_TRUE)
    {
        return CORBA::Double(m_core.m_current_scan.central_position);
    }
    else if(m_core.m_last_scan.servo_name != "")  // We are not scanning now, but we performed a scan previously
    {
        return CORBA::Double(m_core.m_last_scan.central_position);
    }
    else
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossImpl::getCentralPosition()");
        ex.setReason("No scan has been performed yet.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }
}

void SRTMinorServoBossImpl::clearOffsets()
{
    AUTO_TRACE("SRTMinorServoBossImpl::clearOffsets()");
    ACS_LOG(LM_FULL_INFO, "clearServoOffsets", (LM_NOTICE, "CLEARING ALL USER OFFSETS"));
    m_core.clearUserOffsets("ALL");
}

void SRTMinorServoBossImpl::clearUserOffset(const char* servo_name)
{
    AUTO_TRACE("SRTMinorServoBossImpl::clearUserOffset()");
    m_core.clearUserOffsets(std::string(servo_name));
}

void SRTMinorServoBossImpl::setUserOffset(const char* servo_axis_name, CORBA::Double offset)
{
    AUTO_TRACE("SRTMinorServoBossImpl::setUserOffset()");
    m_core.setUserOffset(std::string(servo_axis_name), (double)offset);
}

void SRTMinorServoBossImpl::setUserOffset(const char* servo_axis_name, const double& offset)
{
    AUTO_TRACE("SRTMinorServoBossImpl::setUserOffset()");
    m_core.setUserOffset(std::string(servo_axis_name), (double)offset, true);
}

ACS::doubleSeq* SRTMinorServoBossImpl::getUserOffset()
{
    AUTO_TRACE("SRTMinorServoBossImpl::getUserOffset()");
    return m_core.getUserOffsets();
}

void SRTMinorServoBossImpl::clearSystemOffset(const char* servo_name)
{
    AUTO_TRACE("SRTMinorServoBossImpl::clearSystemOffset()");
    m_core.clearSystemOffsets(std::string(servo_name));
}

void SRTMinorServoBossImpl::setSystemOffset(const char* servo_axis_name, CORBA::Double offset)
{
    AUTO_TRACE("SRTMinorServoBossImpl::setSystemOffset()");
    m_core.setSystemOffset(std::string(servo_axis_name), (double)offset);
}

ACS::doubleSeq* SRTMinorServoBossImpl::getSystemOffset()
{
    AUTO_TRACE("SRTMinorServoBossImpl::getSystemOffset()");
    return m_core.getSystemOffsets();
}

void SRTMinorServoBossImpl::getAxesInfo(ACS::stringSeq_out axes_names, ACS::stringSeq_out axes_units)
{
    AUTO_TRACE("SRTMinorServoBossImpl::getAxesInfo()");
    m_core.getAxesInfo(axes_names, axes_units);
}

char* SRTMinorServoBossImpl::getScanAxis()
{
    AUTO_TRACE("SRTMinorServoBossImpl::getScanAxis()");

    if(m_core.m_scan_active.load() == Management::MNG_TRUE)
    {
        return CORBA::string_dup((m_core.m_current_scan.servo_name + "_" + m_core.m_current_scan.axis_name).c_str());
    }
    else if(!m_core.m_last_scan.servo_name.empty())
    {
        return CORBA::string_dup((m_core.m_last_scan.servo_name + "_" + m_core.m_last_scan.axis_name).c_str());
    }
    else
    {
        return CORBA::string_dup("");
    }
}

ACS::doubleSeq* SRTMinorServoBossImpl::getAxesPosition(ACS::Time acs_time)
{
    AUTO_TRACE("SRTMinorServoBossImpl::getAxesPositions()");
    return m_core.getAxesPositions(acs_time == 0 ? getTimeStamp() : acs_time);
}

void SRTMinorServoBossImpl::setElevationTracking(const char* configuration)
{
    AUTO_TRACE("SRTMinorServoBossImpl::setElevationTracking()");
    m_core.setElevationTracking(std::string(configuration));
}

void SRTMinorServoBossImpl::setASConfiguration(const char* configuration)
{
    AUTO_TRACE("SRTMinorServoBossImpl::setASConfiguration()");
    m_core.setASConfiguration(std::string(configuration));
}

void SRTMinorServoBossImpl::setGregorianCoverPosition(const char* position)
{
    AUTO_TRACE("SRTMinorServoBossImpl::setGregorianCoverPosition()");
    m_core.setGregorianCoverPosition(std::string(position));
}

void SRTMinorServoBossImpl::setGregorianAirBladeStatus(const char* status)
{
    AUTO_TRACE("SRTMinorServoBossImpl::setGregorianAirBladeStatus()");
    m_core.setGregorianAirBladeStatus(std::string(status));
}

CORBA::Boolean SRTMinorServoBossImpl::checkScan(const ACS::Time start_time, const MinorServoScan& scan_info, const Antenna::TRunTimeParameters& antenna_info, TRunTimeParameters_out ms_parameters)
{
    AUTO_TRACE("SRTMinorServoBossImpl::checkScan()");
    return m_core.checkScan(start_time, scan_info, antenna_info, ms_parameters);
}

void SRTMinorServoBossImpl::startScan(ACS::Time& start_time, const MinorServoScan& scan_info, const Antenna::TRunTimeParameters& antenna_info)
{
    AUTO_TRACE("SRTMinorServoBossImpl::startScan()");
    m_core.startScan(start_time, scan_info, antenna_info);
}

void SRTMinorServoBossImpl::closeScan(ACS::Time& close_time)
{
    AUTO_TRACE("SRTMinorServoBossImpl::closeScan()");
    m_core.closeScan(close_time);
}

void SRTMinorServoBossImpl::preset(double elevation)
{
    AUTO_TRACE("SRTMinorServoBossImpl::preset()");
    m_core.preset(elevation);
}

CORBA::Boolean SRTMinorServoBossImpl::command(const char* cmd, CORBA::String_out answer)
{
    AUTO_TRACE("SRTMinorServoBossImpl::command()");

    IRA::CString out;
    bool res = false;

    try
    {
        m_parser.run(cmd, out);
        res = true;
    }
    catch(MinorServoErrors::MinorServoErrorsEx& ex)
    {
        LOG_EX(MinorServoErrors::MinorServoErrorsEx);
    }
    catch(ComponentErrors::ComponentErrorsEx& ex)
    {
        LOG_EX(ComponentErrors::ComponentErrorsEx);
    }
    catch(ManagementErrors::ConfigurationErrorEx& ex)
    {
        LOG_EX(ManagementErrors::ConfigurationErrorEx);
    }
    catch(ManagementErrors::ParkingErrorEx& ex)
    {
        LOG_EX(ManagementErrors::ParkingErrorEx);
    }
    catch(...) // Unknown exception. If the above catch blocks are written correctly we should never get here.
    {
        ACS_SHORT_LOG((LM_ERROR, "SRTMinorServoBossImpl::command()"));
    }

    answer = CORBA::string_dup((const char *)out);
    return res;
}


GET_PROPERTY_REFERENCE(Management::ROTSystemStatus, SRTMinorServoBossImpl, m_status_ptr, status)
GET_PROPERTY_REFERENCE(Management::ROTBoolean, SRTMinorServoBossImpl, m_ready_ptr, ready)
GET_PROPERTY_REFERENCE(ACS::ROstring, SRTMinorServoBossImpl, m_actual_setup_ptr, actualSetup)
GET_PROPERTY_REFERENCE(ACS::ROstring, SRTMinorServoBossImpl, m_motion_info_ptr, motionInfo)
GET_PROPERTY_REFERENCE(Management::ROTBoolean, SRTMinorServoBossImpl, m_starting_ptr, starting)
GET_PROPERTY_REFERENCE(Management::ROTBoolean, SRTMinorServoBossImpl, m_as_configuration_ptr, asConfiguration)
GET_PROPERTY_REFERENCE(Management::ROTBoolean, SRTMinorServoBossImpl, m_elevation_tracking_ptr, elevationTrack)
GET_PROPERTY_REFERENCE(Management::ROTBoolean, SRTMinorServoBossImpl, m_scan_active_ptr, scanActive)
GET_PROPERTY_REFERENCE(Management::ROTBoolean, SRTMinorServoBossImpl, m_scanning_ptr, scanning)
GET_PROPERTY_REFERENCE(Management::ROTBoolean, SRTMinorServoBossImpl, m_tracking_ptr, tracking)
GET_PROPERTY_REFERENCE(Management::ROTBoolean, SRTMinorServoBossImpl, m_connected_ptr, connected)
GET_PROPERTY_REFERENCE(ROSRTMinorServoFocalConfiguration, SRTMinorServoBossImpl, m_current_configuration_ptr, current_configuration);
GET_PROPERTY_REFERENCE(Management::ROTBoolean, SRTMinorServoBossImpl, m_simulation_enabled_ptr, simulation_enabled);
GET_PROPERTY_REFERENCE(ACS::ROdouble, SRTMinorServoBossImpl, m_plc_time_ptr, plc_time);
GET_PROPERTY_REFERENCE(ACS::ROstring, SRTMinorServoBossImpl, m_plc_version_ptr, plc_version);
GET_PROPERTY_REFERENCE(ROSRTMinorServoControlStatus, SRTMinorServoBossImpl, m_control_ptr, control);
GET_PROPERTY_REFERENCE(Management::ROTBoolean, SRTMinorServoBossImpl, m_power_ptr, power);
GET_PROPERTY_REFERENCE(Management::ROTBoolean, SRTMinorServoBossImpl, m_emergency_ptr, emergency);
GET_PROPERTY_REFERENCE(ROSRTMinorServoGregorianCoverStatus, SRTMinorServoBossImpl, m_gregorian_cover_ptr, gregorian_cover);
GET_PROPERTY_REFERENCE(ROSRTMinorServoGregorianAirBladeStatus, SRTMinorServoBossImpl, m_air_blade_ptr, air_blade);
GET_PROPERTY_REFERENCE(ACS::ROdouble, SRTMinorServoBossImpl, m_last_executed_command_ptr, last_executed_command);

#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SRTMinorServoBossImpl)
