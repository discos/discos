#include "SRTMinorServoBossImpl.h"

using namespace maci;
using namespace SimpleParser;

_IRA_LOGFILTER_DECLARE;

SRTMinorServoBossImpl::SRTMinorServoBossImpl(const ACE_CString &componentName, maci::ContainerServices *containerServices) :
    CharacteristicComponentImpl(componentName, containerServices),
    m_core(NULL),
    m_parser(NULL),
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
    m_last_executed_command_ptr(this)
{
    AUTO_TRACE("SRTMinorServoBossImpl::SRTMinorServoBossImpl()");
}

SRTMinorServoBossImpl::~SRTMinorServoBossImpl()
{
    AUTO_TRACE("SRTMinorServoBossImpl::~SRTMinorServoBossImpl()");

    if(m_parser != NULL)
    {
        delete m_parser;
    }
    if(m_core != NULL)
    {
        delete m_core;
    }
}

void SRTMinorServoBossImpl::initialize()
{
    AUTO_TRACE("SRTMinorServoBossImpl::initialize()");
    _IRA_LOGFILTER_ACTIVATE(200000000, 500000000);

    m_core = new SRTMinorServoBossCore(this);

    IRA::CString string_buffer;
    if(!IRA::CIRATools::getDBValue(getContainerServices(), "active_surface_configuration", string_buffer))
    {
        _EXCPT(ComponentErrors::CDBAccessExImpl, impl, "SRTMinorServoBossImpl::initialize()");
        impl.setFieldName("active_surface_configuration");
        throw impl;
    }
    else
    {
        std::string active_surface_configuration(string_buffer);

        if(active_surface_configuration == "ON")
        {
            m_core->m_as_configuration = Management::MNG_TRUE;
        }
        else if(active_surface_configuration == "OFF")
        {
            m_core->m_as_configuration = Management::MNG_FALSE;
        }
        else
        {
            _EXCPT(ComponentErrors::CDBAccessExImpl, impl, "SRTMinorServoBossImpl::initialize()");
            impl.setFieldName("active_surface_configuration");
            impl.addData("Reason", "Value should be 'ON' or 'OFF'");
            throw impl;
        }
    }

    if(!IRA::CIRATools::getDBValue(getContainerServices(), "elevation_tracking_enabled", string_buffer))
    {
        _EXCPT(ComponentErrors::CDBAccessExImpl, impl, "SRTMinorServoBossImpl::initialize()");
        impl.setFieldName("elevation_tracking_enabled");
        throw impl;
    }
    else
    {
        std::string elevation_tracking_enabled(string_buffer);

        if(elevation_tracking_enabled == "ON")
        {
            m_core->m_elevation_tracking_enabled = Management::MNG_TRUE;
        }
        else if(elevation_tracking_enabled == "OFF")
        {
            m_core->m_elevation_tracking_enabled = Management::MNG_FALSE;
        }
        else
        {
            _EXCPT(ComponentErrors::CDBAccessExImpl, impl, "SRTMinorServoBossImpl::initialize()");
            impl.setFieldName("elevation_tracking_enabled");
            impl.addData("Reason", "Value should be 'ON' or 'OFF'");
            throw impl;
        }
    }

    try
    {
        std::string component_name = getContainerServices()->getName().c_str();

        m_status_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus), POA_Management::ROTSystemStatus>((component_name + ":status").c_str(), getComponent(), new MSGenericDevIO<Management::TSystemStatus, std::atomic<Management::TSystemStatus> >(&m_core->m_subsystem_status), true);
        m_ready_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((component_name + ":ready").c_str(), getComponent(), new MSGenericDevIO<Management::TBoolean, std::atomic<Management::TBoolean> >(&m_core->m_ready), true);
        m_actual_setup_ptr = new baci::ROstring((component_name + ":actualSetup").c_str(), getComponent(), new MSGenericDevIO<ACE_CString, std::string>(&m_core->m_actual_setup), true);
        m_motion_info_ptr = new baci::ROstring((component_name + ":motionInfo").c_str(), getComponent(), new MSGenericDevIO<ACE_CString, std::atomic<MinorServo::SRTMinorServoMotionStatus> >(&m_core->m_motion_status), true);
        m_starting_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((component_name + ":starting").c_str(), getComponent(), new MSGenericDevIO<Management::TBoolean, std::atomic<Management::TBoolean> >(&m_core->m_starting), true);
        m_as_configuration_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((component_name + ":asConfiguration").c_str(), getComponent(), new MSGenericDevIO<Management::TBoolean, std::atomic<Management::TBoolean> >(&m_core->m_as_configuration), true);
        m_elevation_tracking_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((component_name + ":elevationTrack").c_str(), getComponent(), new MSGenericDevIO<Management::TBoolean, std::atomic<Management::TBoolean> >(&m_core->m_elevation_tracking_enabled), true);
        m_scan_active_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((component_name + ":scanActive").c_str(), getComponent(), new MSGenericDevIO<Management::TBoolean, std::atomic<Management::TBoolean> >(&m_core->m_scan_active), true);
        m_scanning_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((component_name + ":scanning").c_str(), getComponent(), new MSGenericDevIO<Management::TBoolean, std::atomic<Management::TBoolean> >(&m_core->m_scanning), true);
        m_tracking_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((component_name + ":tracking").c_str(), getComponent(), new MSGenericDevIO<Management::TBoolean, std::atomic<Management::TBoolean> >(&m_core->m_tracking), true);

        SRTMinorServoDevIOInfo dev_io_info;
        dev_io_info.secure_area = m_core->m_status_secure_area;

        dev_io_info.property_name = "current_configuration";
        dev_io_info.property_fields = std::vector<std::string>{ "CURRENT_CONFIG" };
        m_current_configuration_ptr = new ROEnumImpl<ACS_ENUM_T(MinorServo::SRTMinorServoFocalConfiguration), POA_MinorServo::ROSRTMinorServoFocalConfiguration>((component_name + ":current_configuration").c_str(), getComponent(), new MSDevIO<MinorServo::SRTMinorServoFocalConfiguration>(dev_io_info), true);

        dev_io_info.property_name = "simulation_enabled";
        dev_io_info.property_fields = std::vector<std::string>{ "SIMULATION_ENABLED" };
        m_simulation_enabled_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((component_name + ":simulation_enabled").c_str(), getComponent(), new MSDevIO<Management::TBoolean>(dev_io_info), true);

        dev_io_info.property_name = "plc_time";
        dev_io_info.property_fields = std::vector<std::string>{ "PLC_TIME" };
        m_plc_time_ptr = new baci::ROdouble((component_name + ":plc_time").c_str(), getComponent(), new MSDevIO<CORBA::Double>(dev_io_info), true);

        dev_io_info.property_name = "plc_version";
        dev_io_info.property_fields = std::vector<std::string>{ "PLC_VERSION" };
        m_plc_version_ptr = new baci::ROstring((component_name + ":plc_version").c_str(), getComponent(), new MSDevIO<ACE_CString>(dev_io_info), true);

        dev_io_info.property_name = "control";
        dev_io_info.property_fields = std::vector<std::string>{ "CONTROL" };
        m_control_ptr = new ROEnumImpl<ACS_ENUM_T(MinorServo::SRTMinorServoControlStatus), POA_MinorServo::ROSRTMinorServoControlStatus>((component_name + ":control").c_str(), getComponent(), new MSDevIO<MinorServo::SRTMinorServoControlStatus>(dev_io_info), true);

        dev_io_info.property_name = "power";
        dev_io_info.property_fields = std::vector<std::string>{ "POWER" };
        m_power_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((component_name + ":power").c_str(), getComponent(), new MSDevIO<Management::TBoolean>(dev_io_info), true);

        dev_io_info.property_name = "emergency";
        dev_io_info.property_fields = std::vector<std::string>{ "EMERGENCY" };
        m_emergency_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((component_name + ":emergency").c_str(), getComponent(), new MSDevIO<Management::TBoolean>(dev_io_info), true);

        dev_io_info.property_name = "gregorian_cover";
        dev_io_info.property_fields = std::vector<std::string>{ "GREGORIAN_CAP" };
        m_gregorian_cover_ptr = new ROEnumImpl<ACS_ENUM_T(MinorServo::SRTMinorServoGregorianCoverStatus), POA_MinorServo::ROSRTMinorServoGregorianCoverStatus>((component_name + ":gregorian_cover").c_str(), getComponent(), new MSDevIO<MinorServo::SRTMinorServoGregorianCoverStatus>(dev_io_info), true);

        dev_io_info.property_name = "last_executed_command";
        dev_io_info.property_fields = std::vector<std::string>{ "LAST_EXECUTED_COMMAND" };
        m_last_executed_command_ptr = new baci::ROdouble((component_name + ":last_executed_command").c_str(), getComponent(), new MSDevIO<CORBA::Double>(dev_io_info), true);
    }
    catch(std::bad_alloc& ex)
    {
		_THROW_EXCPT(ComponentErrors::MemoryAllocationExImpl, "SRTMinorServoBossImpl::initialize()");
    }

    m_parser = new SimpleParser::CParser<SRTMinorServoBossImpl>(this, 10);

    m_parser->add("servoSetup", new function1<SRTMinorServoBossImpl, non_constant, void_type, I<string_type> >(this, &SRTMinorServoBossImpl::setup), 1);
    m_parser->add("servoPark", new function0<SRTMinorServoBossImpl, non_constant, void_type>(this, &SRTMinorServoBossImpl::park), 0);
    m_parser->add("setServoElevationTracking", new function1<SRTMinorServoBossImpl, non_constant, void_type, I<string_type> >(this, &SRTMinorServoBossImpl::setElevationTracking), 1);
    m_parser->add("setServoASConfiguration", new function1<SRTMinorServoBossImpl, non_constant, void_type, I<string_type> >(this, &SRTMinorServoBossImpl::setASConfiguration), 1);
    m_parser->add("setServoOffset", new function2<SRTMinorServoBossImpl, non_constant, void_type, I<string_type>, I<double_type> >(this, &SRTMinorServoBossImpl::setOffsets), 2);
    m_parser->add("clearServoOffsets", new function0<SRTMinorServoBossImpl, non_constant, void_type>(this, &SRTMinorServoBossImpl::clearOffsets), 0);

    try
    {
        double status_thread_period;
        if(!IRA::CIRATools::getDBValue(getContainerServices(), "status_thread_period", status_thread_period))
        {
            _EXCPT(ComponentErrors::CDBAccessExImpl, impl, "SRTMinorServoBossImpl::initialize()");
            impl.setFieldName("status_thread_period");
            throw impl;
        }
        m_status_thread = getContainerServices()->getThreadManager()->create<SRTMinorServoStatusThread, SRTMinorServoBossCore*>("SRTMinorServoStatusThread", m_core);
        m_status_thread->setSleepTime(status_thread_period * 10000000);
        m_status_thread->resume();
    }
    catch(acsthreadErrType::acsthreadErrTypeExImpl& ex)
    {
        _EXCPT(ComponentErrors::CanNotStartThreadExImpl, impl, "SRTMinorServoBossImpl::initialize()");
        impl.setThreadName("SRTMinorServoStatusThread");
        throw impl;
    }
    catch(...)
    {
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "SRTMinorServoBossImpl::initialize()");
        impl.addData("Reason", "Encountered an unexpected error when starting the SRTMinorServoStatusThread!");
        throw impl;
    }
}

void SRTMinorServoBossImpl::execute()
{
    AUTO_TRACE("SRTMinorServoBossImpl::execute()");
}

void SRTMinorServoBossImpl::cleanUp()
{
    AUTO_TRACE("SRTMinorServoBossImpl::cleanUp()");
    stopPropertiesMonitoring();

    if(m_status_thread != NULL)
    {
        m_status_thread->suspend();
        m_status_thread->terminate();
    }

	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
    CharacteristicComponentImpl::cleanUp();
}

void SRTMinorServoBossImpl::aboutToAbort()
{
    AUTO_TRACE("SRTMinorServoBossImpl::aboutToAbort()");
    stopPropertiesMonitoring();

    if(m_status_thread != NULL)
    {
        m_status_thread->suspend();
        m_status_thread->terminate();
    }

	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
    CharacteristicComponentImpl::aboutToAbort();
}

void SRTMinorServoBossImpl::setup(const char* configuration)
{
    AUTO_TRACE("SRTMinorServoBossImpl::setup()");
    m_core->setup(std::string(configuration));
}

void SRTMinorServoBossImpl::park()
{
    AUTO_TRACE("SRTMinorServoBossImpl::park()");
    m_core->park();
}

CORBA::Boolean SRTMinorServoBossImpl::isElevationTrackingEn()
{
    AUTO_TRACE("SRTMinorServoBossImpl::isElevationTrackingEn()");
    return m_core->m_elevation_tracking_enabled == Management::MNG_TRUE ? true : false;
}

CORBA::Boolean SRTMinorServoBossImpl::isElevationTracking()
{
    AUTO_TRACE("SRTMinorServoBossImpl::isElevationTracking()");
    return m_core->m_elevation_tracking == Management::MNG_TRUE ? true : false;
}

CORBA::Boolean SRTMinorServoBossImpl::isTracking()
{
    AUTO_TRACE("SRTMinorServoBossImpl::isTracking()");
    return m_core->m_tracking == Management::MNG_TRUE ? true : false;
}

CORBA::Boolean SRTMinorServoBossImpl::isStarting()
{
    AUTO_TRACE("SRTMinorServoBossImpl::isStarting()");
    return m_core->m_starting == Management::MNG_TRUE ? true : false;
}

CORBA::Boolean SRTMinorServoBossImpl::isASConfiguration()
{
    AUTO_TRACE("SRTMinorServoBossImpl::isASConfiguration()");
    return m_core->m_as_configuration == Management::MNG_TRUE ? true : false;
}

CORBA::Boolean SRTMinorServoBossImpl::isParking()
{
    AUTO_TRACE("SRTMinorServoBossImpl::isParking()");
    return (m_core->m_starting == Management::MNG_TRUE && m_core->m_commanded_configuration == MinorServo::CONFIGURATION_PARK);
}

CORBA::Boolean SRTMinorServoBossImpl::isReady()
{
    AUTO_TRACE("SRTMinorServoBossImpl::isReady()");
    return m_core->m_ready == Management::MNG_TRUE ? true : false;
}

CORBA::Boolean SRTMinorServoBossImpl::isScanning()
{
    AUTO_TRACE("SRTMinorServoBossImpl::isScanning()");
    return m_core->m_scanning == Management::MNG_TRUE ? true : false;
}

CORBA::Boolean SRTMinorServoBossImpl::isScanActive()
{
    AUTO_TRACE("SRTMinorServoBossImpl::isScanActive()");
    return m_core->m_scan_active == Management::MNG_TRUE ? true : false;
}

char* SRTMinorServoBossImpl::getActualSetup()
{
    AUTO_TRACE("SRTMinorServoBossImpl::getActualSetup()");
    return CORBA::string_dup(m_core->m_actual_setup.c_str());
}

char* SRTMinorServoBossImpl::getCommandedSetup()
{
    AUTO_TRACE("SRTMinorServoBossImpl::getCommandedSetup()");
    return CORBA::string_dup(m_core->m_commanded_setup.c_str());
}

CORBA::Double SRTMinorServoBossImpl::getCentralScanPosition()
{
    AUTO_TRACE("SRTMinorServoBossImpl::getCentralScanPosition()");
    return 0.0;
}

void SRTMinorServoBossImpl::clearUserOffset(const char* servo_name)
{
    AUTO_TRACE("SRTMinorServoBossImpl::clearUserOffset()");
    m_core->clearUserOffsets(std::string(servo_name));
}

void SRTMinorServoBossImpl::setUserOffset(const char* servo_axis_name, CORBA::Double offset)
{
    AUTO_TRACE("SRTMinorServoBossImpl::setUserOffset()");
    m_core->setUserOffset(std::string(servo_axis_name), (double)offset);
}

ACS::doubleSeq* SRTMinorServoBossImpl::getUserOffset()
{
    AUTO_TRACE("SRTMinorServoBossImpl::getUserOffset()");

    std::vector<double> userOffsets = m_core->getUserOffsets();
    ACS::doubleSeq_var offsets = new ACS::doubleSeq;
    offsets->length(userOffsets.size());
    for(size_t i = 0; i < userOffsets.size(); i++)
    {
        offsets[i] = userOffsets[i];
    }
    return offsets._retn();
}

void SRTMinorServoBossImpl::clearSystemOffset(const char* servo_name)
{
    AUTO_TRACE("SRTMinorServoBossImpl::clearSystemOffset()");
    m_core->clearSystemOffsets(std::string(servo_name));
}

void SRTMinorServoBossImpl::setSystemOffset(const char* servo_axis_name, CORBA::Double offset)
{
    AUTO_TRACE("SRTMinorServoBossImpl::setSystemOffset()");
    m_core->setSystemOffset(std::string(servo_axis_name), (double)offset);
}

ACS::doubleSeq* SRTMinorServoBossImpl::getSystemOffset()
{
    AUTO_TRACE("SRTMinorServoBossImpl::getSystemOffset()");

    std::vector<double> systemOffsets = m_core->getSystemOffsets();
    ACS::doubleSeq_var offsets = new ACS::doubleSeq;
    offsets->length(systemOffsets.size());
    for(size_t i = 0; i < systemOffsets.size(); i++)
    {
        offsets[i] = systemOffsets[i];
    }
    return offsets._retn();
}

void SRTMinorServoBossImpl::getAxesInfo(ACS::stringSeq_out axes_names, ACS::stringSeq_out axes_units)
{
    AUTO_TRACE("SRTMinorServoBossImpl::getAxesInfo()");
    m_core->getAxesInfo(axes_names, axes_units);
}

char* SRTMinorServoBossImpl::getScanAxis()
{
    AUTO_TRACE("SRTMinorServoBossImpl::getScanAxis()");

    if(isScanActive())
    {
        return CORBA::string_dup("");
    }
    else
    {
        return CORBA::string_dup("");
    }
}

ACS::doubleSeq* SRTMinorServoBossImpl::getAxesPosition(ACS::Time)
{
    AUTO_TRACE("SRTMinorServoBossImpl::getAxesPosition()");

    ACS::doubleSeq_var positions = new ACS::doubleSeq;
    positions->length(3);
    for(size_t i = 0; i < 3; i++)
    {
        positions[i] = 0;
    }
    return positions._retn();
}

void SRTMinorServoBossImpl::setElevationTracking(const char* configuration)
{
    AUTO_TRACE("SRTMinorServoBossImpl::setElevationTracking()");
    m_core->setElevationTracking(std::string(configuration));
}

void SRTMinorServoBossImpl::setASConfiguration(const char* configuration)
{
    AUTO_TRACE("SRTMinorServoBossImpl::setASConfiguration()");
    m_core->setASConfiguration(std::string(configuration));
}

void SRTMinorServoBossImpl::setOffsets(const char* axis_code, const double& offset)
{
    AUTO_TRACE("SRTMinorServoBossImpl::setOffset()");
}

void SRTMinorServoBossImpl::clearOffsets()
{
    AUTO_TRACE("SRTMinorServoBossImpl::clearOffsets()");

    for(const auto& [name, servo] : m_core->m_servos)
    {
        m_core->clearUserOffsets(name);
        // Not sure about the following
        //m_core->clearSystemOffsets(name);
    }
}

CORBA::Boolean SRTMinorServoBossImpl::checkScan(const ACS::Time start_time, const MinorServo::MinorServoScan& scan_info, const Antenna::TRunTimeParameters& antenna_info, MinorServo::TRunTimeParameters_out ms_parameters)
{
    AUTO_TRACE("SRTMinorServoBossImpl::checkScan()");
    return m_core->checkScan(start_time, scan_info, antenna_info, ms_parameters);
}

void SRTMinorServoBossImpl::startScan(ACS::Time& start_time, const MinorServo::MinorServoScan& scan_info, const Antenna::TRunTimeParameters& antenna_info)
{
    AUTO_TRACE("SRTMinorServoBossImpl::startScan()");
    m_core->startScan(start_time, scan_info, antenna_info);
}

void SRTMinorServoBossImpl::closeScan(ACS::Time& close_time)
{
    AUTO_TRACE("SRTMinorServoBossImpl::closeScan()");
    m_core->closeScan(close_time);
}

void SRTMinorServoBossImpl::preset(double elevation)
{
    AUTO_TRACE("SRTMinorServoBossImpl::preset()");
    m_core->preset(elevation);
}

CORBA::Boolean SRTMinorServoBossImpl::command(const char* cmd, CORBA::String_out answer)
{
    AUTO_TRACE("SRTMinorServoBossImpl::command()");

    IRA::CString out;
    bool res;
    try
    {
        m_parser->run(cmd, out);
        res = true;
    }
    catch(ParserErrors::ParserErrorsExImpl& ex)
    {
        res = false;
    }
    catch(ManagementErrors::ConfigurationErrorExImpl& ex)
    {
        ex.log(LM_ERROR); 
        res = false;
    }
    catch(ACSErr::ACSbaseExImpl& ex)
    {
        ex.log(LM_ERROR); 
        res = false;
    }
    catch(...)
    {
        ACS_SHORT_LOG((LM_WARNING, "SRTMinorServoBossImpl::command(): unknown exception."));
        res = false;
    }
    answer = CORBA::string_dup((const char *)out);
    return res;
}

GET_PROPERTY_REFERENCE(Management::ROTSystemStatus, m_status_ptr, status)
GET_PROPERTY_REFERENCE(Management::ROTBoolean, m_ready_ptr, ready)
GET_PROPERTY_REFERENCE(ACS::ROstring, m_actual_setup_ptr, actualSetup)
GET_PROPERTY_REFERENCE(ACS::ROstring, m_motion_info_ptr, motionInfo)
GET_PROPERTY_REFERENCE(Management::ROTBoolean, m_starting_ptr, starting)
GET_PROPERTY_REFERENCE(Management::ROTBoolean, m_as_configuration_ptr, asConfiguration)
GET_PROPERTY_REFERENCE(Management::ROTBoolean, m_elevation_tracking_ptr, elevationTrack)
GET_PROPERTY_REFERENCE(Management::ROTBoolean, m_scan_active_ptr, scanActive)
GET_PROPERTY_REFERENCE(Management::ROTBoolean, m_scanning_ptr, scanning)
GET_PROPERTY_REFERENCE(Management::ROTBoolean, m_tracking_ptr, tracking)

GET_PROPERTY_REFERENCE(MinorServo::ROSRTMinorServoFocalConfiguration, m_current_configuration_ptr, current_configuration);
GET_PROPERTY_REFERENCE(Management::ROTBoolean, m_simulation_enabled_ptr, simulation_enabled);
GET_PROPERTY_REFERENCE(ACS::ROdouble, m_plc_time_ptr, plc_time);
GET_PROPERTY_REFERENCE(ACS::ROstring, m_plc_version_ptr, plc_version);
GET_PROPERTY_REFERENCE(MinorServo::ROSRTMinorServoControlStatus, m_control_ptr, control);
GET_PROPERTY_REFERENCE(Management::ROTBoolean, m_power_ptr, power);
GET_PROPERTY_REFERENCE(Management::ROTBoolean, m_emergency_ptr, emergency);
GET_PROPERTY_REFERENCE(MinorServo::ROSRTMinorServoGregorianCoverStatus, m_gregorian_cover_ptr, gregorian_cover);
GET_PROPERTY_REFERENCE(ACS::ROdouble, m_last_executed_command_ptr, last_executed_command);

#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SRTMinorServoBossImpl)
