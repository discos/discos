#include "SRTMinorServoBossCore.h"

using namespace maci;

SRTMinorServoBossCore::SRTMinorServoBossCore(SRTMinorServoBossImpl* component)
{
    AUTO_TRACE("SRTMinorServoBossCore::SRTMinorServoBossCore()");

    m_component = component;

    m_motion_status = MinorServo::MOTION_STATUS_UNCONFIGURED;
    m_actual_setup = "Unknown";
    m_commanded_setup = "Unknown";
    m_commanded_configuration = MinorServo::CONFIGURATION_UNKNOWN;

    m_subsystem_status = Management::MNG_WARNING;
    m_ready = Management::MNG_FALSE;
    m_starting = Management::MNG_FALSE;
    m_as_configuration = Management::MNG_FALSE;
    m_elevation_tracking = Management::MNG_FALSE;
    m_elevation_tracking_enabled = Management::MNG_FALSE;
    m_scan_active = Management::MNG_FALSE;
    m_scanning = Management::MNG_FALSE;
    m_tracking = Management::MNG_FALSE;

    m_socket_configuration = &SRTMinorServoSocketConfiguration::getInstance(m_component->getContainerServices());
    m_socket = &SRTMinorServoSocket::getInstance(m_socket_configuration->m_ip_address, m_socket_configuration->m_port, m_socket_configuration->m_timeout);

    m_status = SRTMinorServoAnswerMap();
    m_socket->sendCommand(SRTMinorServoCommandLibrary::status(), m_status);
    m_status_secure_area = new IRA::CSecureArea<SRTMinorServoAnswerMap>(&m_status);

    // Retrieve the minor servos components
    m_tracking_servos["PFP"] = m_component->getContainerServices()->getComponent<MinorServo::SRTProgramTrackMinorServo>("MINORSERVO/PFP");
    m_tracking_servos["SRP"] = m_component->getContainerServices()->getComponent<MinorServo::SRTProgramTrackMinorServo>("MINORSERVO/SRP");
    m_servos["PFP"] = (MinorServo::SRTBaseMinorServo_ptr)m_tracking_servos["PFP"];
    m_servos["SRP"] = (MinorServo::SRTBaseMinorServo_ptr)m_tracking_servos["SRP"];
    m_servos["GFR"] = (MinorServo::SRTBaseMinorServo_ptr)m_component->getContainerServices()->getComponent<MinorServo::SRTGenericMinorServo>("MINORSERVO/GFR");
    m_servos["M3R"] = (MinorServo::SRTBaseMinorServo_ptr)m_component->getContainerServices()->getComponent<MinorServo::SRTGenericMinorServo>("MINORSERVO/M3R");

    m_setup_thread = NULL;
    m_park_thread = NULL;
    m_tracking_thread = NULL;
}

SRTMinorServoBossCore::~SRTMinorServoBossCore()
{
    AUTO_TRACE("SRTMinorServoBossCore::~SRTMinorServoBossCore()");

    if(m_setup_thread != NULL)
    {
        m_setup_thread->suspend();
        m_setup_thread->terminate();
        m_component->getContainerServices()->getThreadManager()->destroy(m_setup_thread);
    }
    if(m_park_thread != NULL)
    {
        m_park_thread->suspend();
        m_park_thread->terminate();
        m_component->getContainerServices()->getThreadManager()->destroy(m_park_thread);
    }
    if(m_tracking_thread != NULL)
    {
        m_tracking_thread->suspend();
        m_tracking_thread->terminate();
        m_component->getContainerServices()->getThreadManager()->destroy(m_tracking_thread);
    }
}

void SRTMinorServoBossCore::setup(std::string commanded_setup)
{
    AUTO_TRACE("SRTMinorServoBossCore::setup()");

    checkControl();
    checkEmergency();

    std::transform(commanded_setup.begin(), commanded_setup.end(), commanded_setup.begin(), ::toupper);

    std::pair<MinorServo::SRTMinorServoFocalConfiguration, bool> cmd_configuration;
    MinorServo::SRTMinorServoFocalConfiguration commanded_configuration;

    try
    {
        cmd_configuration = MinorServo::DiscosConfigurationNameTable.at(commanded_setup);
        commanded_configuration = cmd_configuration.first;
        if(m_as_configuration == Management::MNG_TRUE && cmd_configuration.second)
        {
            commanded_setup += "_ASACTIVE";
        }
    }
    catch(std::out_of_range& ex)  // Unknown configuration
    {
        _EXCPT(MinorServoErrors::ConfigurationErrorExImpl, impl, "SRTMinorServoBossCore::setup()");
        throw impl;
    }

    // Exit if commanded setup is already in place or is about to be
    if(commanded_configuration == m_commanded_configuration && commanded_setup == m_commanded_setup)
    {
        if(m_motion_status == MinorServo::MOTION_STATUS_CONFIGURING || m_motion_status == MinorServo::MOTION_STATUS_CONFIGURED || m_motion_status == MinorServo::MOTION_STATUS_TRACKING)
        {
            return;
        }
    }

    m_commanded_configuration = commanded_configuration;
    m_commanded_setup = commanded_setup;

    // Stop the setup and park threads if running
    if(m_setup_thread != NULL && m_setup_thread->isAlive())
    {
        m_setup_thread->setStopped();
    }
    if(m_park_thread != NULL && m_park_thread->isAlive())
    {
        m_park_thread->setStopped();
    }
    if(m_tracking_thread != NULL && m_tracking_thread->isAlive())
    {
        m_tracking_thread->setStopped();
    }

    m_actual_setup = "";
    m_ready = Management::MNG_FALSE;
    m_starting = Management::MNG_TRUE;
    m_tracking = Management::MNG_FALSE;
    m_motion_status = MinorServo::MOTION_STATUS_CONFIGURING;
    m_subsystem_status = Management::MNG_WARNING;

    // Send the STOP command to each servo
    for(const auto& [name, servo] : m_servos)
    {
        servo->stop();
    }

    // Start the setup thread
    if(m_setup_thread != NULL)
    {
        m_setup_thread->restart();
    }
    else
    {
        try
        {
            m_setup_thread = m_component->getContainerServices()->getThreadManager()->create<SRTMinorServoSetupThread, SRTMinorServoBossCore*>("SRTMinorServoBossCoreSetupThread", m_component->m_core);
        }
        catch(acsthreadErrType::acsthreadErrTypeExImpl& ex)
        {
            m_starting = Management::MNG_FALSE;
            m_motion_status = MinorServo::MOTION_STATUS_ERROR;
            m_subsystem_status = Management::MNG_FAILURE;
            _EXCPT(ComponentErrors::CanNotStartThreadExImpl, impl, "SRTMinorServoBossCore::setup()");
            impl.setThreadName("SRTMinorServoSetupThread");
            throw impl;
        }

        m_setup_thread->resume();
    }
}

void SRTMinorServoBossCore::park()
{
    AUTO_TRACE("SRTMinorServoBossCore::park()");

    checkControl();
    checkEmergency();

    // Exit if the system is already parked or is about to be
    if(m_commanded_configuration == MinorServo::CONFIGURATION_PARK)
    {
        if(m_motion_status == MinorServo::MOTION_STATUS_CONFIGURING || m_motion_status == MinorServo::MOTION_STATUS_PARK)
        {
            return;
        }
    }

    m_commanded_configuration = MinorServo::CONFIGURATION_PARK;
    m_commanded_setup = "Park";

    // Stop the setup and park threads if running
    if(m_setup_thread != NULL && m_setup_thread->isAlive())
    {
        m_setup_thread->setStopped();
    }
    if(m_park_thread != NULL && m_park_thread->isAlive())
    {
        m_park_thread->setStopped();
    }
    if(m_tracking_thread != NULL && m_tracking_thread->isAlive())
    {
        m_tracking_thread->setStopped();
    }

    m_actual_setup = "";
    m_ready = Management::MNG_FALSE;
    m_starting = Management::MNG_TRUE;
    m_tracking = Management::MNG_FALSE;
    m_motion_status = MinorServo::MOTION_STATUS_CONFIGURING;
    m_subsystem_status = Management::MNG_WARNING;

    // Send the STOW command to the gregorian cover
    if(std::get<std::string>(m_socket->sendCommand(SRTMinorServoCommandLibrary::stow("GREGORIAN_CAP", 2))["OUTPUT"]) != "GOOD")
    {
        m_starting = Management::MNG_FALSE;
        m_motion_status = MinorServo::MOTION_STATUS_ERROR;
        m_subsystem_status = Management::MNG_FAILURE;
        _EXCPT(MinorServoErrors::SetupErrorExImpl, impl, "SRTMinorServoBossCore::park()");
        throw impl;
    }

    // Send the STOP command to each servo
    for(const auto& [name, servo] : m_servos)
    {
        servo->stop();
    }

    // Start the park thread
    if(m_park_thread != NULL)
    {
        m_park_thread->restart();
    }
    else
    {
        try
        {
            m_park_thread = m_component->getContainerServices()->getThreadManager()->create<SRTMinorServoParkThread, SRTMinorServoBossCore*>("SRTMinorServoBossCoreParkThread", m_component->m_core);
        }
        catch(acsthreadErrType::acsthreadErrTypeExImpl& ex)
        {
            m_starting = Management::MNG_FALSE;
            m_motion_status = MinorServo::MOTION_STATUS_ERROR;
            m_subsystem_status = Management::MNG_FAILURE;
            _EXCPT(ComponentErrors::CanNotStartThreadExImpl, impl, "SRTMinorServoBossCore::park()");
            impl.setThreadName("SRTMinorServoParkThread");
            throw impl;
        }

        m_park_thread->resume();
    }
}

void SRTMinorServoBossCore::setElevationTracking(std::string configuration)
{
    AUTO_TRACE("SRTMinorServoBossCore::setElevationTracking()");
    std::transform(configuration.begin(), configuration.end(), configuration.begin(), ::toupper);

    if(configuration == "ON")
    {
        m_elevation_tracking_enabled = Management::MNG_TRUE;
    }
    else if(configuration == "OFF")
    {
        m_elevation_tracking_enabled = Management::MNG_FALSE;
    }
    else
    {
        _EXCPT(MinorServoErrors::ConfigurationErrorExImpl, impl, "SRTMinorServoBossCore::setElevationTracking()");
        throw impl;
    }
}

void SRTMinorServoBossCore::setASConfiguration(std::string configuration)
{
    AUTO_TRACE("SRTMinorServoBossCore::setASConfiguration()");
    std::transform(configuration.begin(), configuration.end(), configuration.begin(), ::toupper);

    if(configuration == "ON")
    {
        m_as_configuration = Management::MNG_TRUE;
    }
    else if(configuration == "OFF")
    {
        m_as_configuration = Management::MNG_FALSE;
    }
    else
    {
        _EXCPT(MinorServoErrors::ConfigurationErrorExImpl, impl, "SRTMinorServoBossCore::setASConfiguration()");
        throw impl;
    }

    // Should reload the correct setup if the system was already configured
    if(!m_actual_setup.empty())
    {
        configuration = m_actual_setup.substr(0, m_actual_setup.find("_"));
        setup(configuration);
    }
}

void SRTMinorServoBossCore::status()
{
    AUTO_TRACE("SRTMinorServoBossCore::status()");

    IRA::CSecAreaResourceWrapper<SRTMinorServoAnswerMap> status = m_status_secure_area->Get();
    m_socket->sendCommand(SRTMinorServoCommandLibrary::status(), m_status);
}

void SRTMinorServoBossCore::preset(double elevation)  // Elevation is expressed in degrees
{
    AUTO_TRACE("SRTMinorServoBossCore::preset()");

    checkControl();
    checkEmergency();

    if(m_ready == Management::MNG_FALSE)
    {
        _EXCPT(MinorServoErrors::PositioningErrorExImpl, impl, "SRTMinorServoBossCore::preset()");
        throw impl;
    }

    ACSErr::Completion_var comp;

    for(const auto& [name, servo] : m_servos)
    {
        std::vector<double> coordinates;

        if(servo->in_use()->get_sync(comp.out()) == Management::MNG_TRUE)
        {
            servo->preset(*servo->calcCoordinates(elevation));
        }
    }
}

void SRTMinorServoBossCore::clearUserOffsets(std::string servo_name)
{
    AUTO_TRACE("SRTMinorServoBossCore::clearUserOffsets()");

    checkControl();
    checkEmergency();

    std::transform(servo_name.begin(), servo_name.end(), servo_name.begin(), ::toupper);

    try
    {
        m_servos.at(servo_name)->clearUserOffsets();
    }
    catch(std::out_of_range& ex)
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, impl, "SRTMinorServoBossCore::clearUserOffsets()");
        throw impl;
    }
}

void SRTMinorServoBossCore::setUserOffset(std::string servo_axis_name, double offset)
{
    AUTO_TRACE("SRTMinorServoBossCore::setUserOffset()");

    checkControl();
    checkEmergency();

    std::transform(servo_axis_name.begin(), servo_axis_name.end(), servo_axis_name.begin(), ::toupper);

    std::stringstream ss(servo_axis_name);
    std::string servo_name, axis_name;
    std::getline(ss, servo_name, '_');
    ss >> axis_name;

    try
    {
        m_servos.at(servo_name)->setUserOffset(axis_name.c_str(), offset);
    }
    catch(std::out_of_range& ex)
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, impl, "SRTMinorServoBossCore::setUserOffsets()");
        throw impl;
    }
}

std::vector<double> SRTMinorServoBossCore::getUserOffsets()
{
    AUTO_TRACE("SRTMinorServoBossCore::getUserOffsets()");

    std::vector<double> user_offsets;

    ACSErr::Completion_var comp;
    for(const auto& servo_name : MinorServo::ServoOrder)
    {
        auto servo = m_servos.at(servo_name);
        if(servo->in_use()->get_sync(comp.out()) == Management::MNG_TRUE)
        {
            ACS::doubleSeq* servo_offsets = servo->getUserOffsets();
            std::vector<double> offsets(servo_offsets->get_buffer(), servo_offsets->get_buffer() + servo_offsets->length());
            user_offsets.insert(user_offsets.end(), offsets.begin(), offsets.end());
        }
    }

    return user_offsets;
}

void SRTMinorServoBossCore::clearSystemOffsets(std::string servo_name)
{
    AUTO_TRACE("SRTMinorServoBossCore::clearSystemOffsets()");

    checkControl();
    checkEmergency();

    std::transform(servo_name.begin(), servo_name.end(), servo_name.begin(), ::toupper);

    try
    {
        m_servos.at(servo_name)->clearSystemOffsets();
    }
    catch(std::out_of_range& ex)
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, impl, "SRTMinorServoBossCore::clearSystemOffsets()");
        throw impl;
    }
}

void SRTMinorServoBossCore::setSystemOffset(std::string servo_axis_name, double offset)
{
    AUTO_TRACE("SRTMinorServoBossCore::setSystemOffset()");

    checkControl();
    checkEmergency();

    std::transform(servo_axis_name.begin(), servo_axis_name.end(), servo_axis_name.begin(), ::toupper);

    std::stringstream ss(servo_axis_name);
    std::string servo_name, axis_name;
    std::getline(ss, servo_name, '_');
    ss >> axis_name;

    try
    {
        m_servos.at(servo_name)->setSystemOffset(axis_name.c_str(), offset);
    }
    catch(std::out_of_range& ex)
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, impl, "SRTMinorServoBossCore::setSystemOffsets()");
        throw impl;
    }
}

std::vector<double> SRTMinorServoBossCore::getSystemOffsets()
{
    AUTO_TRACE("SRTMinorServoBossCore::getSystemOffsets()");

    std::vector<double> system_offsets;

    ACSErr::Completion_var comp;
    for(const auto& servo_name : MinorServo::ServoOrder)
    {
        auto servo = m_servos.at(servo_name);
        if(servo->in_use()->get_sync(comp.out()) == Management::MNG_TRUE)
        {
            ACS::doubleSeq* servo_offsets = servo->getSystemOffsets();
            std::vector<double> offsets(servo_offsets->get_buffer(), servo_offsets->get_buffer() + servo_offsets->length());
            system_offsets.insert(system_offsets.end(), offsets.begin(), offsets.end());
        }
    }

    return system_offsets;
}

void SRTMinorServoBossCore::getAxesInfo(ACS::stringSeq_out axes_names, ACS::stringSeq_out axes_units)
{
    AUTO_TRACE("SRTMinorServoBossCore::getAxesInfo()");

    std::vector<std::string> axes_names_vector, axes_units_vector;

    unsigned int length = 0;
    ACS::stringSeq_var axes_names_var = new ACS::stringSeq;
    ACS::stringSeq_var axes_units_var = new ACS::stringSeq;

    ACSErr::Completion_var comp;
    for(const auto& servo_name : MinorServo::ServoOrder)
    {
        auto servo = m_servos.at(servo_name);
        if(servo->in_use()->get_sync(comp.out()) == Management::MNG_TRUE)
        {
            ACS::stringSeq_var servo_axes_names;
            ACS::stringSeq_var servo_axes_units;
            servo->getAxesInfo(servo_axes_names, servo_axes_units);

            unsigned int index = length;
            unsigned int servo_length = (unsigned int)servo->virtual_axes()->get_sync(comp.out());
            length += servo_length;
            axes_names_var->length(length);
            axes_units_var->length(length);

            for(size_t i = 0; i < servo_length; i++, index++)
            {
                axes_names_var[index] = std::string(servo_name + "_" + (const char*)servo_axes_names[i]).c_str();
                axes_units_var[index] = servo_axes_units[i];
            }
        }
    }

    axes_names = axes_names_var._retn();
    axes_units = axes_units_var._retn();
}

bool SRTMinorServoBossCore::checkScan(const ACS::Time start_time, const MinorServo::MinorServoScan& scan_info, const Antenna::TRunTimeParameters& antenna_info, MinorServo::TRunTimeParameters_out ms_parameters)
{
    AUTO_TRACE("SRTMinorServoBossCore::checkScan()");

    checkControl();
    checkEmergency();

    std::cout << "checkScan" << std::endl << std::endl;
    std::cout << "now: " << CIRATools::getUNIXEpoch() << std::endl;

    std::cout << "Scan info:" << std::endl;
    std::cout << "start time: " << CIRATools::ACSTime2UNIXEpoch(start_time) << std::endl;
    std::cout << "empty: " << std::boolalpha << scan_info.is_empty_scan << std::endl;
    std::cout << "span: " << scan_info.range << std::endl;
    std::cout << "total time: " << (int)scan_info.total_time << std::endl;
    std::cout << "axis code: " << std::string(scan_info.axis_code) << std::endl << std::endl;

    std::cout << "Antenna info:" << std::endl;
    std::cout << "target name: " << std::string(antenna_info.targetName) << std::endl;
    std::cout << "azimuth: " << antenna_info.azimuth << std::endl;
    std::cout << "elevation: " << antenna_info.elevation << std::endl;
    std::cout << "right ascension: " << antenna_info.rightAscension << std::endl;
    std::cout << "declination: " << antenna_info.declination << std::endl;
    std::cout << "start time: " << CIRATools::ACSTime2UNIXEpoch(antenna_info.startEpoch) << std::endl;
    std::cout << "on the fly: " << std::boolalpha << antenna_info.onTheFly << std::endl;
    std::cout << "slewing time: " << antenna_info.slewingTime << std::endl;
    std::cout << "azimuth section: " << antenna_info.section << std::endl;
    std::cout << "scan axis: " << antenna_info.axis << std::endl;
    std::cout << "stop time: " << CIRATools::ACSTime2UNIXEpoch(antenna_info.timeToStop) << std::endl;

    MinorServo::TRunTimeParameters_var ms_param_var = new MinorServo::TRunTimeParameters;
    ms_param_var->onTheFly = false;
    ms_param_var->startEpoch = 0;
    ms_param_var->centerScan = 0;
    ms_param_var->scanAxis = CORBA::string_dup("");
    ms_param_var->timeToStop = 0;
    ms_parameters = ms_param_var._retn();

    if(scan_info.is_empty_scan && m_ready == Management::MNG_FALSE)
    {
        return true;
    }

    return true;
}

void SRTMinorServoBossCore::startScan(ACS::Time& start_time, const MinorServo::MinorServoScan& scan_info, const Antenna::TRunTimeParameters& antenna_info)
{
    AUTO_TRACE("SRTMinorServoBossCore::startScan()");

    checkControl();
    checkEmergency();

    std::cout << "startScan" << std::endl;

    // Start the tracking thread
    if(m_tracking_thread != NULL)
    {
        m_tracking_thread->restart();
    }
    else
    {
        try
        {
            m_tracking_thread = m_component->getContainerServices()->getThreadManager()->create<SRTMinorServoTrackingThread, SRTMinorServoBossCore*>("SRTMinorServoTrackingThread", m_component->m_core);
        }
        catch(acsthreadErrType::acsthreadErrTypeExImpl& ex)
        {
            m_subsystem_status = Management::MNG_FAILURE;
            _EXCPT(ComponentErrors::CanNotStartThreadExImpl, impl, "SRTMinorServoBossCore::startScan()");
            impl.setThreadName("SRTMinorServoTrackingThread");
            throw impl;
        }

        m_tracking_thread->resume();
    }
}

void SRTMinorServoBossCore::closeScan(ACS::Time& close_time)
{
    AUTO_TRACE("SRTMinorServoBossCore::closeScan()");

    checkControl();
    checkEmergency();

    std::cout << "closeScan" << std::endl;
}

void SRTMinorServoBossCore::checkControl()
{
    ACSErr::Completion_var comp;
    if(m_component->control()->get_sync(comp.out()) != MinorServo::CONTROL_DISCOS)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::checkControl()");
        impl.setReason("MinorServo system is not controlled by DISCOS!");
        throw impl;
    }
}

void SRTMinorServoBossCore::checkEmergency()
{
    ACSErr::Completion_var comp;
    if(m_component->emergency()->get_sync(comp.out()) == Management::MNG_TRUE)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::checkEmergency()");
        impl.setReason("MinorServo system in emergency status!");
        throw impl;
    }
}
