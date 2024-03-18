#include "SRTMinorServoBossCore.h"

using namespace MinorServo;

SRTMinorServoBossCore::SRTMinorServoBossCore(SRTMinorServoBossImpl& component) :
    m_component(component),
    m_status_thread(nullptr),
    m_setup_thread(nullptr),
    m_park_thread(nullptr),
    m_tracking_thread(nullptr),
    m_scan_thread(nullptr),
    m_antennaBoss("IDL:alma/Antenna/AntennaBoss:1.0", m_component.getContainerServices()),
    m_status(),
    m_motion_status(MOTION_STATUS_UNCONFIGURED),
    m_commanded_setup("Unknown"),
    m_commanded_configuration(CONFIGURATION_UNKNOWN),
    m_subsystem_status(Management::MNG_WARNING),
    m_actual_setup("Unknown"),
    m_ready(Management::MNG_FALSE),
    m_starting(Management::MNG_FALSE),
    m_as_configuration(getCDBConfiguration("active_surface_configuration")),
    m_elevation_tracking(Management::MNG_FALSE),
    m_elevation_tracking_enabled(getCDBConfiguration("elevation_tracking_enabled")),
    m_scan_active(Management::MNG_FALSE),
    m_scanning(Management::MNG_FALSE),
    m_tracking(Management::MNG_FALSE),
    m_reload_servo_offsets(true),
    m_socket_configuration(SRTMinorServoSocketConfiguration::getInstance(m_component.getContainerServices())),
    m_socket(SRTMinorServoSocket::getInstance(m_socket_configuration.m_ip_address, m_socket_configuration.m_port, m_socket_configuration.m_timeout)),
    m_socket_connected(m_socket.isConnected() ? Management::MNG_TRUE : Management::MNG_FALSE),
    m_servos{
        //{ "PFP", m_component.getContainerServices()->getComponent<SRTBaseMinorServo>("MINORSERVO/PFP") },
        { "SRP", m_component.getContainerServices()->getComponent<SRTBaseMinorServo>("MINORSERVO/SRP") },
        { "GFR", m_component.getContainerServices()->getComponent<SRTBaseMinorServo>("MINORSERVO/GFR") }
        //{ "M3R", m_component.getContainerServices()->getComponent<SRTBaseMinorServo>("MINORSERVO/M3R") }
    },
    m_tracking_servos{
        //{ "PFP", m_component.getContainerServices()->getComponent<SRTProgramTrackMinorServo>("MINORSERVO/PFP") },
        { "SRP", m_component.getContainerServices()->getComponent<SRTProgramTrackMinorServo>("MINORSERVO/SRP") }
    }
{
    AUTO_TRACE("SRTMinorServoBossCore::SRTMinorServoBossCore()");

    startThread(m_status_thread, (ACS::TimeInterval)(getCDBValue<double>(m_component.getContainerServices(), "status_thread_period") * 10000000));
}

SRTMinorServoBossCore::~SRTMinorServoBossCore()
{
    AUTO_TRACE("SRTMinorServoBossCore::~SRTMinorServoBossCore()");

    destroyThread(m_setup_thread);
    destroyThread(m_park_thread);
    destroyThread(m_tracking_thread);
    destroyThread(m_scan_thread);
    destroyThread(m_status_thread);
}

bool SRTMinorServoBossCore::status()
{
    AUTO_TRACE("SRTMinorServoBossCore::status()");

    try
    {
        // Attempt communication anyway
        m_socket.sendCommand(SRTMinorServoCommandLibrary::status(), m_status);

        if(m_socket_connected.load() == Management::MNG_FALSE)
        {
            ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossCore::status()", (LM_NOTICE, "Socket connected."));
            m_socket_connected.store(Management::MNG_TRUE);
            m_subsystem_status.store(Management::MNG_WARNING);
        }
    }
    catch(MinorServoErrors::MinorServoErrorsEx& ex)
    {
        if(m_socket_connected.load() == Management::MNG_TRUE)
        {
            ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossCore::status()", (LM_CRITICAL, (std::string("Socket disconnected: ") + getReasonFromEx(ex)).c_str()));
            m_socket_connected.store(Management::MNG_FALSE);

            stopThread(m_setup_thread);
            stopThread(m_park_thread);
            stopThread(m_tracking_thread);
            stopThread(m_scan_thread);
            setFailure();

            m_reload_servo_offsets = true;
        }

        return false;
    }

    try
    {
        checkLineStatus();
    }
    catch(MinorServoErrors::MinorServoErrorsEx& ex)
    {
        _IRA_LOGFILTER_LOG(LM_ERROR, "SRTMinorServoBossCore::status()", getReasonFromEx(ex));
        setFailure();
        return false;
    }

    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    /*if(motion_status == MOTION_STATUS_TRACKING || motion_status == MOTION_STATUS_CONFIGURED)
    {
        // We only get here if the system is configured, therefore we check the correct position of the gregorian cover
        SRTMinorServoGregorianCoverStatus commanded_gregorian_cover_position = m_status.getFocalConfiguration() == CONFIGURATION_PRIMARY ? COVER_STATUS_CLOSED : COVER_STATUS_OPEN;
        if(m_status.getGregorianCoverPosition() != commanded_gregorian_cover_position)
        {
            ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossCore::status()", (LM_CRITICAL, "Gregorian cover in wrong position."));
            setFailure();
            return false;
        }
    }*/

    for(const auto& [name, servo] : m_servos)
    {
        if(!servo->status())
        {
            ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossCore::status()", (LM_CRITICAL, ("Error checking " + name + " status.").c_str()));
            setFailure();
            return false;
        }

        if(m_reload_servo_offsets)
        {
            servo->reloadOffsets();
        }
    }

    m_reload_servo_offsets = false;

    if(motion_status == MOTION_STATUS_TRACKING)
    {
        if(std::all_of(m_current_tracking_servos.begin(), m_current_tracking_servos.end(), [](const std::pair<std::string, SRTProgramTrackMinorServo_ptr>& servo) -> bool
        {
            ACSErr::Completion_var comp;
            return servo.second->isTracking();
        }))
        {
            m_tracking.store(Management::MNG_TRUE);
        }
        else
        {
            m_tracking.store(Management::MNG_FALSE);
        }
    }
    else if(motion_status == MOTION_STATUS_CONFIGURED)
    {
        m_tracking.store(Management::MNG_TRUE);
    }
    else
    {
        m_tracking.store(Management::MNG_FALSE);
    }

    return true;
}

void SRTMinorServoBossCore::setup(std::string commanded_setup)
{
    AUTO_TRACE("SRTMinorServoBossCore::setup()");

    try
    {
        checkLineStatus();
    }
    catch(MinorServoErrors::MinorServoErrorsEx& mse)
    {
        _ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl, ex, mse, "SRTMinorServoBossCore::setup()");
        ex.setSubsystem("MinorServo");
        ex.setReason(getReasonFromEx(mse));
        ex.log(LM_DEBUG);
        throw ex.getConfigurationErrorEx();
    }

    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        ACS_LOG(LM_FULL_INFO, "servoSetup", (LM_NOTICE, "THE SYSTEM IS PERFORMING A SCAN, CANNOT SETUP NOW"));
        _EXCPT(ManagementErrors::ConfigurationErrorExImpl, ex, "SRTMinorServoBossCore::setup()");
        ex.setSubsystem("MinorServo");
        ex.setReason("The system is waiting for a scan to be completed.");
        ex.log(LM_DEBUG);
        throw ex.getConfigurationErrorEx();
    }

    std::transform(commanded_setup.begin(), commanded_setup.end(), commanded_setup.begin(), ::toupper);

    std::pair<SRTMinorServoFocalConfiguration, bool> cmd_configuration;
    SRTMinorServoFocalConfiguration commanded_configuration;

    try
    {
        cmd_configuration = DiscosConfigurationNameTable.at(commanded_setup);
        commanded_configuration = cmd_configuration.first;
        if(m_as_configuration.load() == Management::MNG_TRUE && cmd_configuration.second)
        {
            commanded_setup += "_ASACTIVE";
        }
    }
    catch(std::out_of_range& oor)
    {
        ACS_LOG(LM_FULL_INFO, "servoSetup", (LM_NOTICE, ("UNKNOWN CONFIGURATION '" + commanded_setup + "'").c_str()));
        _EXCPT(ManagementErrors::ConfigurationErrorExImpl, ex, "SRTMinorServoBossCore::setup()");
        ex.setSubsystem("MinorServo");
        ex.setReason(("Unknown configuration '" + commanded_setup + "'.").c_str());
        ex.log(LM_DEBUG);
        throw ex.getConfigurationErrorEx();
    }

    // Exit if commanded setup is already in place or is about to be
    if(commanded_configuration == m_commanded_configuration.load() && commanded_setup == m_commanded_setup)
    {
        SRTMinorServoMotionStatus motion_status = m_motion_status.load();
        if(motion_status == MOTION_STATUS_STARTING)
        {
            ACS_LOG(LM_FULL_INFO, "servoSetup", (LM_NOTICE, ("ALREADY SETTING UP '" + commanded_setup + "' CONFIGURATION").c_str()));
            return;
        }
        else if(motion_status == MOTION_STATUS_CONFIGURED || motion_status == MOTION_STATUS_TRACKING)
        {
            ACS_LOG(LM_FULL_INFO, "servoSetup", (LM_NOTICE, ("CONFIGURATION '" + commanded_setup + "' ALREADY IN PLACE").c_str()));
            return;
        }
    }

    ACS_LOG(LM_FULL_INFO, "servoSetup", (LM_NOTICE, ("SETTING UP '" + commanded_setup + "' CONFIGURATION").c_str()));

    m_commanded_configuration.store(commanded_configuration);
    m_commanded_setup = commanded_setup;

    // Stop the setup, park and tracking threads if running
    stopThread(m_setup_thread);
    stopThread(m_park_thread);
    stopThread(m_tracking_thread);

    m_subsystem_status.store(Management::MNG_WARNING);
    m_actual_setup = "";
    m_ready.store(Management::MNG_FALSE);
    m_starting.store(Management::MNG_TRUE);
    m_scan_active.store(Management::MNG_FALSE);
    m_scanning.store(Management::MNG_FALSE);
    m_tracking.store(Management::MNG_FALSE);
    m_motion_status.store(MOTION_STATUS_STARTING);
    m_current_servos.clear();
    m_current_tracking_servos.clear();

    // Send the STOP command to all the servos
    for(const auto& [servo_name, servo] : m_servos)
    {
        try
        {
            servo->stop();
        }
        catch(MinorServoErrors::MinorServoErrorsEx& mse)
        {
            _ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl, ex, mse, "SRTMinorServoBossCore::setup()");
            ex.setSubsystem("MinorServo");
            ex.setReason(("Error while sending the STOP command to " + servo_name + ".").c_str());
            ex.log(LM_DEBUG);
            throw ex.getConfigurationErrorEx();
        }
    }

    // Start the setup thread
    try
    {
        startThread(m_setup_thread);
    }
    catch(ComponentErrors::ComponentErrorsEx& ce)
    {
        _ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl, ex, ce, "SRTMinorServoBossCore::setup()");
        ex.setSubsystem("MinorServo");
        ex.setReason("Error while trying to start the setup_thread");
        ex.log(LM_DEBUG);
        throw ex.getConfigurationErrorEx();
    }
}

void SRTMinorServoBossCore::park()
{
    AUTO_TRACE("SRTMinorServoBossCore::park()");

    try
    {
        checkLineStatus();
    }
    catch(MinorServoErrors::MinorServoErrorsEx& mse)
    {
        _ADD_BACKTRACE(ManagementErrors::ParkingErrorExImpl, ex, mse, "SRTMinorServoBossCore::park()");
        ex.setSubsystem("MinorServo");
        ex.setReason(getReasonFromEx(mse));
        ex.log(LM_DEBUG);
        throw ex.getParkingErrorEx();
    }

    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        ACS_LOG(LM_FULL_INFO, "servoPark", (LM_NOTICE, "THE SYSTEM IS PERFORMING A SCAN, CANNOT PARK NOW"));
        _EXCPT(ManagementErrors::ParkingErrorExImpl, ex, "SRTMinorServoBossCore::park()");
        ex.setSubsystem("MinorServo");
        ex.setReason("The system is waiting for a scan to be completed.");
        ex.log(LM_DEBUG);
        throw ex.getParkingErrorEx();
    }

    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status == MOTION_STATUS_PARKING)
    {
        ACS_LOG(LM_FULL_INFO, "servoPark", (LM_NOTICE, "MINOR SERVOS ALREADY PARKING"));
        return;
    }
    else if (motion_status == MOTION_STATUS_PARKED)
    {
        ACS_LOG(LM_FULL_INFO, "servoPark", (LM_NOTICE, "MINOR SERVOS ALREADY PARKED"));
        return;
    }

    ACS_LOG(LM_FULL_INFO, "servoPark", (LM_NOTICE, "PARKING"));

    m_commanded_configuration.store(CONFIGURATION_PARK);
    m_commanded_setup = "Park";

    // Stop the setup, park and tracking threads if running
    stopThread(m_setup_thread);
    stopThread(m_park_thread);
    stopThread(m_tracking_thread);

    m_subsystem_status.store(Management::MNG_WARNING);
    m_actual_setup = "";
    m_ready.store(Management::MNG_FALSE);
    m_starting.store(Management::MNG_TRUE);
    m_scan_active.store(Management::MNG_FALSE);
    m_scanning.store(Management::MNG_FALSE);
    m_tracking.store(Management::MNG_FALSE);
    m_motion_status.store(MOTION_STATUS_PARKING);
    m_current_servos.clear();
    m_current_tracking_servos.clear();

    /*try
    {
        // Send the STOW command to close the gregorian cover
        if(!m_socket.sendCommand(SRTMinorServoCommandLibrary::stow("Gregoriano", COVER_STATUS_CLOSED)).checkOutput())
        {
            _EXCPT(ManagementErrors::ParkingErrorExImpl, ex, "SRTMinorServoBossCore::park()");
            ex.setSubsystem("MinorServo");
            ex.setReason("Error while sending a STOW command to the gregorian cover.");
            ex.log(LM_DEBUG);
            throw ex.getParkingErrorEx();
        }
    }
    catch(MinorServoErrors::MinorServoErrorsEx& mse)
    {
        _ADD_BACKTRACE(ManagementErrors::ParkingErrorExImpl, ex, mse, "SRTMinorServoBossCore::park()");
        ex.setSubsystem("MinorServo");
        ex.setReason("Error while sending the STOW command to the gregorian cover.");
        ex.log(LM_DEBUG);
        throw ex.getParkingErrorEx();
    }*/

    // Send the STOP command to all the servos
    for(const auto& [servo_name, servo] : m_servos)
    {
        try
        {
            servo->stop();
        }
        catch(MinorServoErrors::MinorServoErrorsEx& mse)
        {
            _ADD_BACKTRACE(ManagementErrors::ParkingErrorExImpl, ex, mse, "SRTMinorServoBossCore::park()");
            ex.setSubsystem("MinorServo");
            ex.setReason(("Error while sending the STOP command to " + servo_name + ".").c_str());
            ex.log(LM_DEBUG);
            throw ex.getParkingErrorEx();
        }
    }

    // Start the park thread
    try
    {
        startThread(m_park_thread);
    }
    catch(ComponentErrors::ComponentErrorsEx& ce)
    {
        _ADD_BACKTRACE(ManagementErrors::ParkingErrorExImpl, ex, ce, "SRTMinorServoBossCore::park()");
        ex.setSubsystem("MinorServo");
        ex.setReason("Error while trying to start the park_thread");
        ex.log(LM_DEBUG);
        throw ex.getParkingErrorEx();
    }
}

void SRTMinorServoBossCore::setElevationTracking(std::string configuration)
{
    AUTO_TRACE("SRTMinorServoBossCore::setElevationTracking()");

    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::setElevationTracking()");
        ex.setReason("The system is waiting for a scan to be completed.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    std::transform(configuration.begin(), configuration.end(), configuration.begin(), ::toupper);

    if(configuration != "ON" && configuration != "OFF")
    {
       _EXCPT(MinorServoErrors::ConfigurationErrorExImpl, ex, "SRTMinorServoBossCore::setElevationTracking()");
       ex.addData("Reason", ("Unknown setElevationTracking configuration '" + configuration + "'.").c_str());
        ex.log(LM_DEBUG);
       throw ex.getMinorServoErrorsEx();
    }

    ACS_LOG(LM_FULL_INFO, "setServoElevationTracking", (LM_NOTICE, ("SETTING ELEVATION TRACKING TO " + configuration).c_str()));

    if(configuration == "ON" && m_elevation_tracking_enabled.load() != Management::MNG_TRUE)
    {
        m_elevation_tracking_enabled.store(Management::MNG_TRUE);

        if(m_motion_status.load() == MOTION_STATUS_CONFIGURED)
        {
            m_motion_status.store(MOTION_STATUS_TRACKING);
            startThread(m_tracking_thread);
        }
    }
    else if(configuration == "OFF" && m_elevation_tracking_enabled.load() != Management::MNG_FALSE)
    {
        stopThread(m_tracking_thread);
        m_elevation_tracking_enabled.store(Management::MNG_FALSE);

        if(m_motion_status.load() == MOTION_STATUS_TRACKING)
        {
            m_motion_status.store(MOTION_STATUS_CONFIGURED);
        }
    }
}

void SRTMinorServoBossCore::setASConfiguration(std::string configuration)
{
    AUTO_TRACE("SRTMinorServoBossCore::setASConfiguration()");

    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::setASConfiguration()");
        ex.setReason("The system is waiting for a scan to be completed.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    std::transform(configuration.begin(), configuration.end(), configuration.begin(), ::toupper);

    if(configuration != "ON" && configuration != "OFF")
    {
       _EXCPT(MinorServoErrors::ConfigurationErrorExImpl, ex, "SRTMinorServoBossCore::setASConfiguration()");
       ex.addData("Reason", ("Unknown setASConfiguration configuration '" + configuration + "'.").c_str());
        ex.log(LM_DEBUG);
       throw ex.getMinorServoErrorsEx();
    }

    ACS_LOG(LM_FULL_INFO, "setServoASConfiguration", (LM_NOTICE, ("SETTING ACTIVE SURFACE CONFIGURATION TO " + configuration).c_str()));

    if(configuration == "ON" && m_as_configuration.load() != Management::MNG_TRUE)
    {
        m_as_configuration.store(Management::MNG_TRUE);
    }
    else if(configuration == "OFF" && m_as_configuration.load() != Management::MNG_FALSE)
    {
        m_as_configuration.store(Management::MNG_FALSE);
    }
    else
    {
        // We asked for the same configuration, nothing else to do
        return;
    }

    // Should reload the correct setup if the system was already configured or was about to be
    if(!m_commanded_setup.empty())
    {
        configuration = m_commanded_setup.substr(0, m_commanded_setup.find("_"));
        try
        {
            setup(configuration);
        }
        catch(ManagementErrors::ConfigurationErrorEx& ce)
        {
            _ADD_BACKTRACE(MinorServoErrors::ConfigurationErrorExImpl, ex, ce, "SRTMinorServoBossCore::setASConfiguration()");
            ex.log(LM_DEBUG);
            throw ex.getMinorServoErrorsEx();
        }
    }
}

void SRTMinorServoBossCore::setGregorianCoverPosition(std::string position)
{
    AUTO_TRACE("SRTMinorServoBossCore::setGregorianCoverPosition()");

    checkLineStatus();

    std::transform(position.begin(), position.end(), position.begin(), ::toupper);

    if(position != "OPEN" && position != "CLOSED")
    {
        _EXCPT(MinorServoErrors::StowErrorExImpl, ex, "SRTMinorServoBossCore::setGregorianCoverPosition()");
        ex.addData("Reason", ("Unknown position '" + position + "'.").c_str());
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status != MOTION_STATUS_PARKING && motion_status != MOTION_STATUS_PARKED)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::setGregorianCoverPosition()");
        ex.setReason("You can set the gregorian cover position only when the system is parked or is parking.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    ACS_LOG(LM_FULL_INFO, "setGregorianCoverPosition", (LM_NOTICE, ("SETTING GREGORIAN COVER POSITION TO " + position).c_str()));

    if(!m_socket.sendCommand(SRTMinorServoCommandLibrary::stow("Gregoriano", position == "OPEN" ? COVER_STATUS_OPEN : COVER_STATUS_CLOSED)).checkOutput())
    {
        _EXCPT(MinorServoErrors::StowErrorExImpl, ex, "SRTMinorServoBossCore::setGregorianCoverPosition()");
        ex.addData("Reason", "Error while sending a STOW command to the gregorian cover.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }
}

void SRTMinorServoBossCore::preset(double elevation)
{
    // Elevation is expressed in degrees
    AUTO_TRACE("SRTMinorServoBossCore::preset()");

    checkLineStatus();

    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::preset()");
        ex.setReason("The system is waiting for a scan to be completed.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    if(m_motion_status.load() != MOTION_STATUS_CONFIGURED)
    {
        _EXCPT(MinorServoErrors::PositioningErrorExImpl, ex, "SRTMinorServoBossCore::preset()");
        ex.addData("Reason", "Minor servos are tracking, cannot send a global preset command now.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    for(const auto& [servo_name, servo] : m_current_servos)
    {
        servo->preset(*servo->calcCoordinates(elevation));
    }
}

void SRTMinorServoBossCore::clearUserOffsets(std::string servo_name)
{
    AUTO_TRACE("SRTMinorServoBossCore::clearUserOffsets()");

    checkLineStatus();

    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::clearUserOffsets()");
        ex.setReason("The system is waiting for a scan to be completed.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status != MOTION_STATUS_CONFIGURED && motion_status != MOTION_STATUS_TRACKING)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::clearUserOffsets()");
        ex.setReason("The system is not configured yet.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    std::transform(servo_name.begin(), servo_name.end(), servo_name.begin(), ::toupper);

    if(servo_name == "ALL")
    {
        for(const auto& [servo_name, servo] : m_current_servos)
        {
                servo->clearUserOffsets();
        }
        return;
    }
    else if(!m_servos.count(servo_name))
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, ex, "SRTMinorServoBossCore::clearUserOffsets()");
        ex.addData("Reason", ("Unknown servo '" + servo_name + "'.").c_str());
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    try
    {
        auto servo = m_current_servos.at(servo_name);
        servo->clearUserOffsets();
    }
    catch(std::out_of_range& oor)
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, ex, "SRTMinorServoBossCore::clearUserOffsets()");
        ex.addData("Reason", ("Servo '" + servo_name + "' not in use with the current configuration.").c_str());
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }
}

void SRTMinorServoBossCore::setUserOffset(std::string servo_axis_name, double offset, bool log)
{
    AUTO_TRACE("SRTMinorServoBossCore::setUserOffset()");

    checkLineStatus();

    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::setUserOffset()");
        ex.setReason("The system is waiting for a scan to be completed.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status != MOTION_STATUS_CONFIGURED && motion_status != MOTION_STATUS_TRACKING)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::setUserOffset()");
        ex.setReason("The system is not configured yet.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    std::transform(servo_axis_name.begin(), servo_axis_name.end(), servo_axis_name.begin(), ::toupper);

    std::stringstream ss(servo_axis_name);
    std::string servo_name, axis_name;
    std::getline(ss, servo_name, '_');
    ss >> axis_name;

    if(!m_servos.count(servo_name))
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, ex, "SRTMinorServoBossCore::setUserOffsets()");
        ex.addData("Reason", ("Unknown servo '" + servo_name + "'.").c_str());
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    try
    {
        auto servo = m_current_servos.at(servo_name);
        if(log)
        {
            ACS_LOG(LM_FULL_INFO, "setServoOffset", (LM_NOTICE, ("SETTING '" + servo_name + "' '" + axis_name + "' OFFSET TO " + std::to_string(offset)).c_str()));
        }
        servo->setUserOffset(axis_name.c_str(), offset);
    }
    catch(std::out_of_range& oor)
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, ex, "SRTMinorServoBossCore::setUserOffset()");
        ex.addData("Reason", ("Servo '" + servo_name + "' not in use with the current configuration.").c_str());
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }
}

ACS::doubleSeq* SRTMinorServoBossCore::getUserOffsets()
{
    AUTO_TRACE("SRTMinorServoBossCore::getUserOffsets()");

    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status != MOTION_STATUS_CONFIGURED && motion_status != MOTION_STATUS_TRACKING)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::getUserOffsets()");
        ex.setReason("The system is not configured yet.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    ACS::doubleSeq_var offsets = new ACS::doubleSeq;

    for(const auto& [servo_name, servo] : m_current_servos)
    {
        ACS::doubleSeq servo_offsets = *servo->getUserOffsets();
        size_t start_index = offsets->length();
        offsets->length(start_index + servo_offsets.length());
        std::copy(servo_offsets.begin(), servo_offsets.end(), offsets->begin() + start_index);
    }

    return offsets._retn();
}

void SRTMinorServoBossCore::clearSystemOffsets(std::string servo_name)
{
    AUTO_TRACE("SRTMinorServoBossCore::clearSystemOffsets()");

    checkLineStatus();

    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::clearSystemOffsets()");
        ex.setReason("The system is waiting for a scan to be completed.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status != MOTION_STATUS_CONFIGURED && motion_status != MOTION_STATUS_TRACKING)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::clearSystemOffsets()");
        ex.setReason("The system is not configured yet.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    std::transform(servo_name.begin(), servo_name.end(), servo_name.begin(), ::toupper);

    if(servo_name == "ALL")
    {
        for(const auto& [servo_name, servo] : m_current_servos)
        {
            servo->clearSystemOffsets();
        }
        return;
    }
    else if(!m_servos.count(servo_name))
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, ex, "SRTMinorServoBossCore::clearSystemOffsets()");
        ex.addData("Reason", ("Unknown servo '" + servo_name + "'.").c_str());
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    try
    {
        auto servo = m_current_servos.at(servo_name);
        servo->clearSystemOffsets();
    }
    catch(std::out_of_range& oor)
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, ex, "SRTMinorServoBossCore::clearSystemOffsets()");
        ex.addData("Reason", ("Servo '" + servo_name + "' not in use with the current configuration.").c_str());
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }
}

void SRTMinorServoBossCore::setSystemOffset(std::string servo_axis_name, double offset)
{
    AUTO_TRACE("SRTMinorServoBossCore::setSystemOffset()");

    checkLineStatus();

    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::setSystemOffset()");
        ex.setReason("The system is waiting for a scan to be completed.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status != MOTION_STATUS_CONFIGURED && motion_status != MOTION_STATUS_TRACKING)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::setSystemOffset()");
        ex.setReason("The system is not configured yet.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    std::transform(servo_axis_name.begin(), servo_axis_name.end(), servo_axis_name.begin(), ::toupper);

    std::stringstream ss(servo_axis_name);
    std::string servo_name, axis_name;
    std::getline(ss, servo_name, '_');
    ss >> axis_name;

    if(!m_servos.count(servo_name))
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, ex, "SRTMinorServoBossCore::setSystemOffset()");
        ex.addData("Reason", ("Unknown servo '" + servo_name + "'.").c_str());
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    try
    {
        auto servo = m_current_servos.at(servo_name);
        servo->setSystemOffset(axis_name.c_str(), offset);
    }
    catch(std::out_of_range& oor)
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, ex, "SRTMinorServoBossCore::setSystemOffset()");
        ex.addData("Reason", ("Servo '" + servo_name + "' not in use with the current configuration.").c_str());
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }
}

ACS::doubleSeq* SRTMinorServoBossCore::getSystemOffsets()
{
    AUTO_TRACE("SRTMinorServoBossCore::getSystemOffsets()");

    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status != MOTION_STATUS_CONFIGURED && motion_status != MOTION_STATUS_TRACKING)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::getSystemOffsets()");
        ex.setReason("The system is not configured yet.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    ACS::doubleSeq_var offsets = new ACS::doubleSeq;

    for(const auto& [servo_name, servo] : m_current_servos)
    {
        ACS::doubleSeq servo_offsets = *servo->getSystemOffsets();
        size_t start_index = offsets->length();
        offsets->length(start_index + servo_offsets.length());
        std::copy(servo_offsets.begin(), servo_offsets.end(), offsets->begin() + start_index);
    }

    return offsets._retn();
}

void SRTMinorServoBossCore::getAxesInfo(ACS::stringSeq_out axes_names_out, ACS::stringSeq_out axes_units_out)
{
    AUTO_TRACE("SRTMinorServoBossCore::getAxesInfo()");

    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status != MOTION_STATUS_CONFIGURED && motion_status != MOTION_STATUS_TRACKING)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::getAxesInfo()");
        ex.setReason("The system is not configured yet.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    std::vector<std::string> axes_names_vector, axes_units_vector;

    ACS::stringSeq_var axes_names = new ACS::stringSeq;
    ACS::stringSeq_var axes_units = new ACS::stringSeq;

    for(const auto& [servo_name, servo] : m_current_servos)
    {
        ACS::stringSeq_var servo_axes_names;
        ACS::stringSeq_var servo_axes_units;
        servo->getAxesInfo(servo_axes_names, servo_axes_units);

        std::transform(servo_axes_names->begin(), servo_axes_names->end(), servo_axes_names->begin(), [servo_name](const char* axis_name)
        {
            return CORBA::string_dup((servo_name + "_" + axis_name).c_str());
        });

        size_t names_index = axes_names->length();
        size_t units_index = axes_units->length();
        axes_names->length(names_index + servo_axes_names->length());
        axes_units->length(units_index + servo_axes_units->length());
        std::copy(servo_axes_names->begin(), servo_axes_names->end(), axes_names->begin() + names_index);
        std::copy(servo_axes_units->begin(), servo_axes_units->end(), axes_units->begin() + units_index);
    }

    axes_names_out = axes_names._retn();
    axes_units_out = axes_units._retn();
}

ACS::doubleSeq* SRTMinorServoBossCore::getAxesPositions(ACS::Time acs_time)
{
    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status != MOTION_STATUS_CONFIGURED && motion_status != MOTION_STATUS_TRACKING)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::getAxesPositions()");
        ex.setReason("The system is not configured yet.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    ACS::doubleSeq_var positions = new ACS::doubleSeq;

    for(const auto& [servo_name, servo] : m_current_servos)
    {
        ACS::doubleSeq servo_positions = *servo->getAxesPositions(acs_time);
        size_t start_index = positions->length();
        positions->length(start_index + servo_positions.length());
        std::copy(servo_positions.begin(), servo_positions.end(), positions->begin() + start_index);
    }

    return positions._retn();
}

SRTMinorServoScan SRTMinorServoBossCore::checkScanFeasibility(const ACS::Time& start_time, const MinorServoScan& scan_info, const Antenna::TRunTimeParameters& antenna_info)
{
    // TODO: check whether to start from the left side or the right side, LOW priority
    AUTO_TRACE("SRTMinorServoBossCore::checkScanFeasibility()");
    _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::checkScanFeasibility()");

    SRTMinorServoScan new_scan;
    new_scan.scan_range = scan_info.range;
    new_scan.scan_duration = scan_info.total_time;
    new_scan.starting_elevation = antenna_info.elevation * DR2D;

    std::string axis_code(scan_info.axis_code);
    // Get the axis that will have to be moved during the scan
    std::transform(axis_code.begin(), axis_code.end(), axis_code.begin(), ::toupper);
    std::string servo_name = axis_code.substr(0, axis_code.find("_"));
    std::string axis_name = axis_code.substr(axis_code.find("_") + 1);

    new_scan.servo_name = servo_name;
    new_scan.axis_name = axis_name;

    ACS::stringSeq_var servo_axes_names, servo_axes_units;

    if(!m_servos.count(servo_name))
    {
        ex.setReason(("Unknown servo '" + servo_name + "'.").c_str());
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }
    else if(!m_tracking_servos.count(servo_name))
    {
        ex.setReason(("Servo '" + servo_name + "' is not a tracking servo.").c_str());
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }
    else if(!m_current_tracking_servos.count(servo_name))
    {
        ex.setReason(("Servo '" + servo_name + "' not in use with the current configuration.").c_str());
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    SRTProgramTrackMinorServo_ptr servo = m_current_tracking_servos.at(servo_name);
    servo->getAxesInfo(servo_axes_names, servo_axes_units);

    size_t axis_index;
    for(axis_index = 0; axis_index < servo_axes_names->length(); axis_index++)
    {
        if(std::string(servo_axes_names[axis_index]) == axis_name)
        {
            break;
        }
    }

    if(axis_index == servo_axes_names->length())
    {
        ex.setReason(("Axis '" + axis_code + "' not found.").c_str());
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    new_scan.axis_index = axis_index;

    ACS::doubleSeq starting_position = *servo->calcCoordinates(new_scan.starting_elevation);
    ACS::doubleSeq final_position = *servo->calcCoordinates(new_scan.starting_elevation);
    // Don't be fooled by the name, starting position now still holds the central scan position
    new_scan.central_position = starting_position[axis_index];

    starting_position[axis_index] -= scan_info.range / 2;
    final_position[axis_index] += scan_info.range / 2;

    /**
     * Note, we perform the calculations with some degree of approximation,
     * we know the expected elevation at the start of the scan,
     * but the true elevation value is going to be known only inside the startScan,
     * depending on how much time the minor servos take to get to position
     * TBH, I don't really know if this will really matter, since we will be starting
     * the movement of the SRP far from the peak of the gaussian.
     * We will check this behavior later.
     * In any case, the scan will simply add an offset to the elevation tracking expected position,
     * the offset value depends on the time elapsed since the start of the scan, which,
     * in the case of the focusScan, it's probably always decided by the minor servo system
     */
    ACS::doubleSeq_var min_ranges, max_ranges;
    servo->getAxesRanges(min_ranges, max_ranges);

    // Read the servo offsets
    ACS::doubleSeq user_offsets = *servo->getUserOffsets();
    ACS::doubleSeq system_offsets = *servo->getSystemOffsets();

    // Check if starting or final positions are outside the axes range (considering offsets)
    for(size_t i = 0; i < starting_position.length(); i++)
    {
        if(starting_position[i] + user_offsets[i] + system_offsets[i] < min_ranges[i] || starting_position[i] + user_offsets[i] + system_offsets[i] > max_ranges[i])
        {
            ex.setReason("Starting position out of range.");
            ex.log(LM_DEBUG);
            throw ex.getMinorServoErrorsEx();
        }
        else if(final_position[i] + user_offsets[i] + system_offsets[i] < min_ranges[i] || final_position[i] + user_offsets[i] + system_offsets[i] > max_ranges[i])
        {
            ex.setReason("Final position out of range.");
            ex.log(LM_DEBUG);
            throw ex.getMinorServoErrorsEx();
        }
    }

    // Time to reach the desired position if I were to start the movement now
    ACS::Time min_starting_time = getTimeStamp() + servo->getTravelTime(ACS::doubleSeq(), starting_position) + PROGRAM_TRACK_FUTURE_TIME;

    if(start_time != 0 && min_starting_time > start_time)
    {
        ex.setReason("Not enough time to start the scan.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }
    else if(servo->getTravelTime(starting_position, final_position) > scan_info.total_time)  // Check scan total time
    {
        ex.setReason("Not enough time to perform the scan.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    new_scan.start_time = min_starting_time;
    return new_scan;
}

bool SRTMinorServoBossCore::checkScan(const ACS::Time start_time, const MinorServoScan& scan_info, const Antenna::TRunTimeParameters& antenna_info, TRunTimeParameters_out ms_parameters)
{
    AUTO_TRACE("SRTMinorServoBossCore::checkScan()");

    TRunTimeParameters_var ms_param_var = new TRunTimeParameters;
    ms_param_var->onTheFly = !scan_info.is_empty_scan;
    ms_param_var->scanAxis = CORBA::string_dup(scan_info.axis_code);
    ms_param_var->startEpoch = 0;
    ms_param_var->centerScan = 0;
    ms_param_var->timeToStop = 0;

    try
    {
        checkLineStatus();
    }
    catch(MinorServoErrors::MinorServoErrorsEx& ex)
    {
        // Something is not working, return ASAP
        ACS_SHORT_LOG((LM_ERROR, ex.errorTrace.routine));
        ms_parameters = ms_param_var._retn();
        return false;
    }

    // Check if we are already performing another scan
    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossCore::checkScan()", (LM_CRITICAL, "The system is waiting for a scan to be completed."));
        ms_parameters = ms_param_var._retn();
        return false;
    }

    /**
     * Empty scan, the old implementation always returned true
     * Given the fact that we start the elevation tracking thread automatically
     * and considering we will not have to move any axis because of the empty scan
     * it is safe to return true in any situation.
     * If the system is not configured this will not stop the antenna from starting to track a source
     * If the system is about to be configured we will start tracking soon anyway
     * The tracking flag will be set to true only when we are inside the tracking delta
     * The change in status will be notified via the NotificationChannel
     */
    if(scan_info.is_empty_scan)
    {
        ms_parameters = ms_param_var._retn();
        return true;
    }

    // We need to be ready here, we cannot wait for a setup procedure to complete
    // Check if the system is now configured and/or tracking
    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status != MOTION_STATUS_TRACKING && motion_status != MOTION_STATUS_CONFIGURED)
    {
        ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossCore::checkScan()", (LM_CRITICAL, "The system is not ready yet."));
        ms_parameters = ms_param_var._retn();
        return false;
    }

    SRTMinorServoScan scan;
    try
    {
        scan = checkScanFeasibility(start_time, scan_info, antenna_info);
    }
    catch(MinorServoErrors::MinorServoErrorsEx& ex)
    {
        ACS_SHORT_LOG((LM_ERROR, ex.errorTrace.routine));
        ms_parameters = ms_param_var._retn();
        return false;
    }

    ms_param_var->startEpoch = scan.start_time;
    ms_param_var->centerScan = scan.central_position;
    ms_param_var->timeToStop = ms_param_var->startEpoch + scan_info.total_time;
    ms_parameters = ms_param_var._retn();
    return true;
}

void SRTMinorServoBossCore::startScan(ACS::Time& start_time, const MinorServoScan& scan_info, const Antenna::TRunTimeParameters& antenna_info)
{
    AUTO_TRACE("SRTMinorServoBossCore::startScan()");

    // This will throw if anything is not working, no need to catch it since we will throw it again anyway
    checkLineStatus();

    // Check if we are already performing another scan
    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::startScan()");
        ex.setReason("The system is waiting for a scan to be completed.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    // Empty scan, OK in any case
    if(scan_info.is_empty_scan)
    {
        start_time = getTimeStamp();
        return;
    }

    // Check again, this should never block, otherwise something weird is happening
    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status != MOTION_STATUS_TRACKING && motion_status != MOTION_STATUS_CONFIGURED)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::startScan()");
        ex.setReason("The system is not ready yet.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    // This will throw if anything is not working, no need to catch it since we will throw it again anyway
    SRTMinorServoScan scan = checkScanFeasibility(start_time, scan_info, antenna_info);

    stopThread(m_tracking_thread);

    // If we got here we are safe to start the scan
    m_current_scan = scan;
    start_time = scan.start_time;
    m_scan_active.store(Management::MNG_TRUE);
    startThread(m_scan_thread);
}

void SRTMinorServoBossCore::closeScan(ACS::Time& close_time)
{
    AUTO_TRACE("SRTMinorServoBossCore::closeScan()");

    checkLineStatus();

    if(m_scan_active.load() != Management::MNG_TRUE)
    {
        // Not scanning, or scan just stopped and the servo is still going back to the initial position
        close_time = std::max(getTimeStamp(), m_last_scan.close_time);
        return;
    }

    // If we got here the scan was not complete, we need to update some values
    // Update the scan_duration value. The scan thread will read the value and update the scan center point accordingly.
    m_current_scan.scan_duration = getTimeStamp() - m_current_scan.start_time;

    // The thread might have already stopped, but we don't care
    stopThread(m_scan_thread);

    // At this point the thread must have stopped, we waited at most 200ms for it to finish its execution, so we can safely assume it's done
    // The thread also have updated m_last_scan with the final values

    // Set the close_time
    close_time = std::max(getTimeStamp(), m_last_scan.close_time);
    ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossCore::closeScan()", (LM_NOTICE, "Scan closed."));
}

double SRTMinorServoBossCore::getElevation(const ACS::Time& acs_time)
{
    AUTO_TRACE("SRTMinorServoBossCore::getElevation()");

    double azimuth, elevation;

    try
    {
        m_antennaBoss->getRawCoordinates(acs_time, azimuth, elevation);
        elevation *= DR2D;
        return elevation == 0.0 ? 45.0 : elevation;
    }
    catch(ComponentErrors::CouldntGetComponentExImpl& ex)
    {
        ex.addData("Reason", "Cannot get the ANTENNA/Boss component");
        throw ex.getComponentErrorsEx();
    }
}

void SRTMinorServoBossCore::checkLineStatus()
{
    AUTO_TRACE("SRTMinorServoBossCore::checkLineStatus()");

    if(!m_socket.isConnected())
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::checkLineStatus()");
        ex.setReason("Socket not connected.");
        ex.log(LM_DEBUG);
        setFailure();
        throw ex.getMinorServoErrorsEx();
    }

    if(m_status.getControl() != CONTROL_DISCOS)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::checkLineStatus()");
        ex.setReason("MinorServo system is not controlled by DISCOS.");
        ex.log(LM_DEBUG);
        setFailure();
        throw ex.getMinorServoErrorsEx();
    }

    if(m_status.emergencyPressed() == Management::MNG_TRUE)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, "SRTMinorServoBossCore::checkLineStatus()");
        ex.setReason("MinorServo system in emergency status.");
        ex.log(LM_DEBUG);
        setFailure();
        throw ex.getMinorServoErrorsEx();
    }
}

template <typename T, typename = std::enable_if<is_any_v<T, SRTMinorServoStatusThread, SRTMinorServoSetupThread, SRTMinorServoParkThread, SRTMinorServoTrackingThread, SRTMinorServoScanThread>>>
void SRTMinorServoBossCore::startThread(T*& thread, const ACS::TimeInterval& sleep_time)
{
    AUTO_TRACE("SRTMinorServoBossCore::startThread()");

    try
    {
        if(thread != nullptr)
        {
            // We don't want to restart already active threads
            if(!thread->isAlive())
            {
                if(sleep_time != 0)
                {
                    thread->setSleepTime(sleep_time);
                }
                thread->restart();
            }
        }
        else
        {
            thread = m_component.getContainerServices()->getThreadManager()->create<T, SRTMinorServoBossCore&>(T::c_thread_name, *this);
            if(sleep_time != 0)
            {
                thread->setSleepTime(sleep_time);
            }
            thread->resume();
        }
    }
    catch(acsthreadErrType::CanNotSpawnThreadExImpl& impl)
    {
        // The thread failed to start for some reason
        _ADD_BACKTRACE(ComponentErrors::CanNotStartThreadExImpl, ex, impl, "SRTMinorServoBossCore::startThread()");
        ex.setThreadName(T::c_thread_name);
        ex.log(LM_DEBUG);
        setFailure();
        throw ex.getComponentErrorsEx();
    }
}

template <typename T, typename = std::enable_if<is_any_v<T, SRTMinorServoStatusThread, SRTMinorServoSetupThread, SRTMinorServoParkThread, SRTMinorServoTrackingThread, SRTMinorServoScanThread>>>
void SRTMinorServoBossCore::stopThread(T*& thread)
{
    AUTO_TRACE("SRTMinorServoBossCore::stopThread()");

    if(thread != nullptr && thread->isAlive())
    {
        thread->stop();
    }
}

template <typename T, typename = std::enable_if<is_any_v<T, SRTMinorServoStatusThread, SRTMinorServoSetupThread, SRTMinorServoParkThread, SRTMinorServoTrackingThread, SRTMinorServoScanThread>>>
void SRTMinorServoBossCore::destroyThread(T*& thread)
{
    AUTO_TRACE("SRTMinorServoBossCore::destroyThread()");

    if(thread != nullptr)
    {
        thread->terminate();
        m_component.getContainerServices()->getThreadManager()->destroy(thread);
    }

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossCore::destroyThread()", (LM_NOTICE, (std::string(T::c_thread_name) + " destroyed.").c_str()));
}

void SRTMinorServoBossCore::setFailure()
{
    AUTO_TRACE("SRTMinorServoBossCore::setFailure()");

    m_subsystem_status.store(Management::MNG_FAILURE);
    m_ready.store(Management::MNG_FALSE);
    m_elevation_tracking.store(Management::MNG_FALSE);
    m_starting.store(Management::MNG_FALSE);
    m_scan_active.store(Management::MNG_FALSE);
    m_scanning.store(Management::MNG_FALSE);
    m_tracking.store(Management::MNG_FALSE);
    m_motion_status.store(MOTION_STATUS_ERROR);
}

Management::TBoolean SRTMinorServoBossCore::getCDBConfiguration(std::string which_configuration)
{
    AUTO_TRACE("SRTMinorServoBossCore::getCDBConfiguration()");
    std::string configuration = getCDBValue<std::string>(m_component.getContainerServices(), which_configuration);
    std::transform(configuration.begin(), configuration.end(), configuration.begin(), ::toupper);

    if(configuration != "ON" && configuration != "OFF")
    {
        _EXCPT(ComponentErrors::CDBAccessExImpl, ex, "SRTMinorServoBossCore::getCDBConfiguration()");
        ex.setFieldName(which_configuration.c_str());
        ex.addData("Reason", "Value should be 'ON' or 'OFF'");
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }

    return configuration == "ON" ? Management::MNG_TRUE : Management::MNG_FALSE;
}
