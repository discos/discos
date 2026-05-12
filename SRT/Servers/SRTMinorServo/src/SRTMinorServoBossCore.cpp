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
    m_elevation_tracking(Management::MNG_FALSE),
    m_scan_active(Management::MNG_FALSE),
    m_scanning(Management::MNG_FALSE),
    m_tracking(Management::MNG_FALSE),
    m_error_code(ERROR_NO_ERROR),
    m_reload_servo_offsets(true),
    m_notification_channel(nullptr),
    m_zmqPublisher("minor_servo")
{
    AUTO_TRACE("SRTMinorServoBossCore::SRTMinorServoBossCore()");
}

SRTMinorServoBossCore::~SRTMinorServoBossCore()
{
    AUTO_TRACE("SRTMinorServoBossCore::~SRTMinorServoBossCore()");
}

void SRTMinorServoBossCore::initialize()
{
    AUTO_TRACE("SRTMinorServoBossCore::initialize()");

    // We let the BossImpl catch anything that can be thrown from here

    m_as_configuration = getCDBConfiguration("active_surface_configuration");
    m_elevation_tracking_enabled = getCDBConfiguration("elevation_tracking_enabled");
    m_socket_configuration = &SRTMinorServoSocketConfiguration::getInstance(m_component.getContainerServices());
    m_socket = &SRTMinorServoSocket::getInstance(m_socket_configuration->m_ip_address, m_socket_configuration->m_port, m_socket_configuration->m_timeout);
    m_socket_connected.store(m_socket->isConnected() ? Management::MNG_TRUE : Management::MNG_FALSE);
    //m_servos["PFP"] = m_component.getContainerServices()->getComponent<SRTBaseMinorServo>("MINORSERVO/PFP");
    m_servos["SRP"] = m_component.getContainerServices()->getComponent<SRTBaseMinorServo>("MINORSERVO/SRP");
    m_servos["GFR"] = m_component.getContainerServices()->getComponent<SRTBaseMinorServo>("MINORSERVO/GFR");
    m_servos["M3R"] = m_component.getContainerServices()->getComponent<SRTBaseMinorServo>("MINORSERVO/M3R");
    //m_tracking_servos["PFP"] = m_component.getContainerServices()->getComponent<SRTProgramTrackMinorServo>("MINORSERVO/PFP");
    m_tracking_servos["SRP"] = m_component.getContainerServices()->getComponent<SRTProgramTrackMinorServo>("MINORSERVO/SRP");

    m_DISCOS_2_LDO_configurations = loadConfigurations();
}

void SRTMinorServoBossCore::execute()
{
    AUTO_TRACE("SRTMinorServoBossCore::execute()");
    if(m_socket_connected.load() == Management::MNG_FALSE)
    {
        ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossCore::execute()", (LM_CRITICAL, "Starting with socket disconnected."));
        setError(ERROR_NOT_CONNECTED);
    }

    startThread(m_status_thread, (ACS::TimeInterval)(getCDBValue<double>(m_component.getContainerServices(), "status_thread_period") * 10000000));
}

void SRTMinorServoBossCore::cleanUp()
{
    AUTO_TRACE("SRTMinorServoBossCore::cleanUp()");

    destroyThread(m_setup_thread);
    destroyThread(m_park_thread);
    destroyThread(m_tracking_thread);
    destroyThread(m_scan_thread);
    destroyThread(m_status_thread);

    if(m_notification_channel != nullptr)
    {
        m_notification_channel->disconnect();
        m_notification_channel = nullptr;
    }
}

bool SRTMinorServoBossCore::status()
{
    AUTO_TRACE("SRTMinorServoBossCore::status()");

    try
    {
        // Attempt communication anyway
        if(!m_socket->sendCommand(SRTMinorServoCommandLibrary::status(), m_status).checkOutput())
        {
            setError(ERROR_COMMAND_ERROR);
            return false;
        }

        if(m_socket_connected.load() == Management::MNG_FALSE)
        {
            ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossCore::status()", (LM_NOTICE, "Socket connected."));
            // We just reconnected, we can try to reset the error automatically, if there is another error in the following code the reset will simply be overridden
            m_socket_connected.store(Management::MNG_TRUE);
            reset();
        }
    }
    catch(MinorServoErrors::CommunicationErrorExImpl& impl)
    {
        if(m_socket_connected.load() == Management::MNG_TRUE)
        {
            ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossCore::status()", (LM_CRITICAL, "Socket disconnected: %s", impl.getReason().c_str()));
            m_socket_connected.store(Management::MNG_FALSE);

            stopThread(m_setup_thread);
            stopThread(m_park_thread);
            stopThread(m_tracking_thread);
            stopThread(m_scan_thread);
            setError(ERROR_NOT_CONNECTED);

            m_reload_servo_offsets = true;
        }

        return false;
    }

    try
    {
        checkLineStatus();
    }
    catch(MinorServoErrors::StatusErrorExImpl& impl)
    {
        _IRA_LOGFILTER_LOG(LM_ERROR, "SRTMinorServoBossCore::status()", impl.getReason());
        return false;
    }

    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status == MOTION_STATUS_TRACKING || motion_status == MOTION_STATUS_CONFIGURED)
    {
        // We only get here if the system is configured, therefore we check the correct position of the gregorian cover
        SRTMinorServoGregorianCoverStatus commanded_gregorian_cover_position = m_status.getFocalConfiguration() == CONFIGURATION_PRIMARY ? COVER_STATUS_CLOSED : COVER_STATUS_OPEN;
        if(m_status.getGregorianCoverPosition() != commanded_gregorian_cover_position)
        {
            ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossCore::status()", (LM_CRITICAL, "Gregorian cover in wrong position."));
            setError(ERROR_COVER_WRONG_POSITION);
            return false;
        }
    }

    for(const auto& [name, servo] : m_servos)
    {
        try
        {
            servo->status();
        }
        catch(MinorServoErrors::MinorServoErrorsEx& ex)
        {
            _ADD_BACKTRACE(MinorServoErrors::CommunicationErrorExImpl, impl, ex, "SRTMinorServoBossCore::status()");
            impl.setReason(("Error checking " + name + " status.").c_str());
            impl.log(LM_CRITICAL);
            setError(servo->getErrorCode());
            return false;
        }

        if(m_reload_servo_offsets)
        {
            servo->reloadOffsets();
        }
    }

    m_reload_servo_offsets = false;

    if(motion_status == MOTION_STATUS_CONFIGURED || (motion_status == MOTION_STATUS_TRACKING && m_elevation_tracking.load() == Management::MNG_TRUE))
    {
        if(std::all_of(m_current_tracking_servos.begin(), m_current_tracking_servos.end(), [](const std::pair<std::string, SRTProgramTrackMinorServo_ptr>& servo) -> bool
        {
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
    else
    {
        m_tracking.store(Management::MNG_FALSE);
    }

    return true;
}

void SRTMinorServoBossCore::publishData()
{
    m_zmqDictionary["socketConnected"] = m_socket_connected.load() == Management::MNG_TRUE;

    if(!m_zmqDictionary["socketConnected"])
    {
        m_zmqDictionary["timestamp"] = ZMQ::ZMQTimeStamp::now();
    }
    else
    {
        m_zmqDictionary["timestamp"] = ZMQ::ZMQTimeStamp::fromUNIXTime(m_status.getPLCTime());
    }

    m_zmqDictionary["tracking"] = m_tracking.load() == Management::MNG_TRUE;
    m_zmqDictionary["scanning"] = m_scanning.load() == Management::MNG_TRUE;
    m_zmqDictionary["currentSetup"] = m_actual_setup;
    m_zmqDictionary["trackingEnabled"] = m_elevation_tracking_enabled.load() == Management::MNG_TRUE;

    try
    {
        m_zmqDictionary["emergency"] = m_status.emergencyPressed() == Management::MNG_TRUE;
    }
    catch(std::out_of_range&)
    {
        m_zmqDictionary["emergency"] = false;
    }
    try
    {
        m_zmqDictionary["maintenanceMode"] = m_status.getControl() == CONTROL_VBRAIN;
    }
    catch(std::out_of_range&)
    {
        m_zmqDictionary["maintenanceMode"] = false;
    }
    try
    {
        m_zmqDictionary["PLCFirmwareVersion"] = m_status.getPLCVersion().c_str();
    }
    catch(std::out_of_range&)
    {
        m_zmqDictionary["PLCFirmwareVersion"] = "";;
    }

    // error_code enum
    switch(m_error_code.load())
    {
        case ERROR_NO_ERROR:
        {
            m_zmqDictionary["errorCode"] = "NO ERROR";
            break;
        }
        case ERROR_NOT_CONNECTED:
        {
            m_zmqDictionary["errorCode"] = "SOCKET NOT CONNECTED";
            break;
        }
        case ERROR_MAINTENANCE:
        {
            m_zmqDictionary["errorCode"] = "SYSTEM IN MAINTENANCE MODE";
            break;
        }
        case ERROR_EMERGENCY_STOP:
        {
            m_zmqDictionary["errorCode"] = "EMERGENCY STOP";
            break;
        }
        case ERROR_COVER_WRONG_POSITION:
        {
            m_zmqDictionary["errorCode"] = "GREGORIAN COVER IN WRONG POSITION";
            break;
        }
        case ERROR_CONFIG_ERROR:
        {
            m_zmqDictionary["errorCode"] = "CONFIGURATION ERROR";
            break;
        }
        case ERROR_COMMAND_ERROR:
        {
            m_zmqDictionary["errorCode"] = "REMOTE COMMAND ERROR";
            break;
        }
        case ERROR_SERVO_BLOCKED:
        {
            m_zmqDictionary["errorCode"] = "MINOR SERVO IS BLOCKED";
            break;
        }
        case ERROR_DRIVE_CABINET:
        {
            m_zmqDictionary["errorCode"] = "DRIVE CABINET ERROR";
            break;
        }
    }

    try
    {
        // gregorian cover enum
        switch(m_status.getGregorianCoverPosition())
        {
            case COVER_STATUS_UNKNOWN :
            {
                m_zmqDictionary["gregorianCoverPosition"] = "UNKNOWN";
                break;
            }
            case COVER_STATUS_CLOSED :
            {
                m_zmqDictionary["gregorianCoverPosition"] = "CLOSED";
                break;
            }
            case COVER_STATUS_OPEN :
            {
                m_zmqDictionary["gregorianCoverPosition"] = "OPEN";
                break;
            }
        }
    }
    catch(std::out_of_range&)
    {
        m_zmqDictionary["gregorianCoverPosition"] = "UNKNOWN";
    }

    // motion info enum
    switch(m_motion_status.load())
    {
        case MOTION_STATUS_UNCONFIGURED :
        {
            m_zmqDictionary["motionInfo"] = "NOT CONFIGURED";
            break;
        }
        case MOTION_STATUS_STARTING :
        {
            m_zmqDictionary["motionInfo"] = "STARTING";
            break;
        }
        case MOTION_STATUS_CONFIGURED :
        {
            m_zmqDictionary["motionInfo"] = "CONFIGURED";
            break;
        }
        case MOTION_STATUS_TRACKING :
        {
            m_zmqDictionary["motionInfo"] = "TRACKING";
            break;
        }
        case MOTION_STATUS_PARKING :
        {
            m_zmqDictionary["motionInfo"] = "PARKING";
            break;
        }
        case MOTION_STATUS_PARKED :
        {
            m_zmqDictionary["motionInfo"] = "PARKED";
            break;
        }
        case MOTION_STATUS_ERROR :
        {
            m_zmqDictionary["motionInfo"] = "ERROR";
            break;
        }
    }

    // status enum
    switch (m_subsystem_status.load())
    {
        case Management::MNG_OK :
        {
            m_zmqDictionary["status"] = "OK";
            break;
        }
        case Management::MNG_WARNING :
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

    for(const auto& [name, servo] : m_servos)
    {
        SRTMinorServoZMQStatus status = *servo->getSRTMinorServoZMQStatus(m_zmqDictionary["timestamp"]["unix_time"]);

        ZMQ::ZMQDictionary servo_status;
        servo_status["blocked"] = status.blocked;
        servo_status["currentSetup"] = status.currentSetup;

        switch(status.driveCabinetStatus)
        {
            case DRIVE_CABINET_OK:
            {
                servo_status["driveCabinetStatus"] = "OK";
                break;
            }
            case DRIVE_CABINET_WARNING:
            {
                servo_status["driveCabinetStatus"] = "WARNING";
                break;
            }
            case DRIVE_CABINET_ERROR:
            {
                servo_status["driveCabinetStatus"] = "ERROR";
                break;
            }
        }

        servo_status["enabled"] = status.enabled;

        switch(status.errorCode)
        {
            case ERROR_NO_ERROR:
            {
                servo_status["errorCode"] = "NO ERROR";
                break;
            }
            case ERROR_NOT_CONNECTED:
            {
                servo_status["errorCode"] = "SOCKET NOT CONNECTED";
                break;
            }
            case ERROR_MAINTENANCE:
            {
                servo_status["errorCode"] = "SYSTEM IN MAINTENANCE MODE";
                break;
            }
            case ERROR_EMERGENCY_STOP:
            {
                servo_status["errorCode"] = "EMERGENCY STOP";
                break;
            }
            case ERROR_COVER_WRONG_POSITION:
            {
                servo_status["errorCode"] = "GREGORIAN COVER IN WRONG POSITION";
                break;
            }
            case ERROR_CONFIG_ERROR:
            {
                servo_status["errorCode"] = "CONFIGURATION ERROR";
                break;
            }
            case ERROR_COMMAND_ERROR:
            {
                servo_status["errorCode"] = "REMOTE COMMAND ERROR";
                break;
            }
            case ERROR_SERVO_BLOCKED:
            {
                servo_status["errorCode"] = "MINOR SERVO IS BLOCKED";
                break;
            }
            default: //case ERROR_DRIVE_CABINET:
            {
                servo_status["errorCode"] = "DRIVE CABINET ERROR";
                break;
            }
        }

        servo_status["inUse"] = status.inUse;

        switch(status.operativeMode)
        {
            case OPERATIVE_MODE_UNKNOWN:
            {
                servo_status["operativeMode"] = "UNKNOWN";
                break;
            }
            case OPERATIVE_MODE_SETUP:
            {
                servo_status["operativeMode"] = "SETUP";
                break;
            }
            case OPERATIVE_MODE_STOW:
            {
                servo_status["operativeMode"] = "STOW";
                break;
            }
            case OPERATIVE_MODE_STOP:
            {
                servo_status["operativeMode"] = "STOP";
                break;
            }
            case OPERATIVE_MODE_PRESET:
            {
                servo_status["operativeMode"] = "PRESET";
                break;
            }
            default: //case OPERATIVE_MODE_PROGRAMTRACK:
            {
                servo_status["operativeMode"] = "PROGRAM TRACK";
                break;
            }
        }

        servo_status["axes"] = ZMQ::ZMQDictionary();

        for(size_t i = 0; i < status.axesNames.length(); i++)
        {
            ZMQ::ZMQDictionary axis;
            axis["currentPosition"] = status.currentPositions[i];
            axis["commandedPosition"] = status.commandedPositions[i];
            axis["userOffset"] = status.userOffsets[i];
            axis["systemOffset"] = status.systemOffsets[i];

            if(status.trackingErrors.length() == status.axesNames.length())
            {
                axis["trackingError"] = status.trackingErrors[i];
            }

            servo_status["axes"][(const char*)status.axesNames[i]] = axis;
        }

        if(status.trackingErrors.length() > 0)
        {
            servo_status["remainingTrajectoryPoints"] = status.remainingTrajectoryPoints;
            servo_status["totalTrajectoryPoints"] = status.totalTrajectoryPoints;
            servo_status["tracking"] = status.tracking;
            servo_status["trajectoryID"] = status.trajectoryID;
        }

        m_zmqDictionary[name] = servo_status;
    }

    m_zmqPublisher.publish(m_zmqDictionary);

    static TIMEVALUE lastEvent(0UL);
    static MinorServoDataBlock prvData = {0UL, false, false, false, false, Management::MNG_WARNING};

    TIMEVALUE now(ACS::Time(IRA::CIRATools::UNIXTime2ACSTime(m_zmqDictionary["timestamp"]["unix_time"])));

    if(IRA::CIRATools::timeDifference(now, lastEvent) < 1000000 && prvData.tracking == (m_tracking.load() == Management::MNG_TRUE) && prvData.status == m_subsystem_status.load())
    {
        return;
    }

    prvData.timeMark = now.value().value;
    prvData.tracking = m_tracking.load() == Management::MNG_TRUE;
    prvData.starting = m_starting.load() == Management::MNG_TRUE;
    prvData.parking = m_motion_status.load() == MOTION_STATUS_PARKING;
    prvData.parked = m_motion_status.load() == MOTION_STATUS_PARKED;
    prvData.status = m_subsystem_status.load();

    if(m_notification_channel == nullptr)
    {
        try
        {
            m_notification_channel = new nc::SimpleSupplier(MINORSERVO_DATA_CHANNEL, &m_component);
        }
        catch(...)
        {
            _IRA_LOGFILTER_LOG(LM_WARNING, "SRTMinorServoBossCore::publish()", "cannot access the MinorServoData notification channel!");
            return;
        }
    }

    try
    {
        m_notification_channel->publishData<MinorServoDataBlock>(prvData);
    }
    catch(ComponentErrors::CORBAProblemEx& ex)
    {
        _IRA_LOGFILTER_LOG(LM_WARNING, "SRTMinorServoBossCore::publish()", "cannot send MinorServoData over the notification channel!");
    }

    IRA::CIRATools::timeCopy(lastEvent, now);
}

void SRTMinorServoBossCore::setup(const char* commanded_setup_c)
{
    AUTO_TRACE("SRTMinorServoBossCore::setup()");

    if(m_error_code.load() != ERROR_NO_ERROR)
    {
        _EXCPT(ManagementErrors::ConfigurationErrorExImpl, impl, "SRTMinorServoBossCore::setup()");
        impl.setSubsystem("MinorServo");
        impl.setReason("The system is in error state. Reset the errors first with the 'servoReset' command.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    try
    {
        checkLineStatus();
    }
    catch(MinorServoErrors::StatusErrorExImpl& se)
    {
        _ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl, impl, se, "SRTMinorServoBossCore::setup()");
        impl.setSubsystem("MinorServo");
        impl.log(LM_DEBUG);
        throw impl;
    }

    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        ACS_LOG(LM_FULL_INFO, "servoSetup", (LM_NOTICE, "THE SYSTEM IS PERFORMING A SCAN, CANNOT SETUP NOW"));
        _EXCPT(ManagementErrors::ConfigurationErrorExImpl, impl, "SRTMinorServoBossCore::setup()");
        impl.setSubsystem("MinorServo");
        impl.setReason("The system is waiting for a scan to be completed.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    std::string commanded_setup = commanded_setup_c;
    std::transform(commanded_setup.begin(), commanded_setup.end(), commanded_setup.begin(), ::toupper);

    SRTMinorServoFocalConfiguration commanded_configuration;

    try
    {
        std::string LDO_configuration = m_DISCOS_2_LDO_configurations.at(commanded_setup);
        commanded_configuration = LDOConfigurationNameTable.right.at(LDO_configuration);

        if(m_as_configuration.load() == Management::MNG_FALSE)
        {
            commanded_setup += "_AS_OFF";
        }
    }
    catch(std::out_of_range& oor)
    {
        ACS_LOG(LM_FULL_INFO, "servoSetup", (LM_NOTICE, ("UNKNOWN CONFIGURATION '" + commanded_setup + "'").c_str()));
        _EXCPT(ManagementErrors::ConfigurationErrorExImpl, impl, "SRTMinorServoBossCore::setup()");
        impl.setSubsystem("MinorServo");
        impl.setReason(("Unknown configuration '" + commanded_setup + "'.").c_str());
        impl.log(LM_DEBUG);
        throw impl;
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
            _ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl, impl, mse, "SRTMinorServoBossCore::setup()");
            impl.setSubsystem("MinorServo");
            impl.setReason(("Error while sending the STOP command to " + servo_name + ".").c_str());
            impl.log(LM_DEBUG);
            throw impl;
        }
    }

    // Start the setup thread
    try
    {
        startThread(m_setup_thread);
    }
    catch(ComponentErrors::CanNotStartThreadExImpl& ce)
    {
        _ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl, impl, ce, "SRTMinorServoBossCore::setup()");
        impl.setSubsystem("MinorServo");
        impl.setReason("Error while trying to start the setup_thread");
        impl.log(LM_DEBUG);
        throw impl;
    }
}

void SRTMinorServoBossCore::park()
{
    AUTO_TRACE("SRTMinorServoBossCore::park()");

    if(m_error_code.load() != ERROR_NO_ERROR)
    {
        _EXCPT(ManagementErrors::ParkingErrorExImpl, impl, "SRTMinorServoBossCore::park()");
        impl.setSubsystem("MinorServo");
        impl.setReason("The system is in error state. Reset the errors first with the 'servoReset' command.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    try
    {
        checkLineStatus();
    }
    catch(MinorServoErrors::StatusErrorExImpl& se)
    {
        _ADD_BACKTRACE(ManagementErrors::ParkingErrorExImpl, impl, se, "SRTMinorServoBossCore::park()");
        impl.setSubsystem("MinorServo");
        impl.log(LM_DEBUG);
        throw impl;
    }

    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        ACS_LOG(LM_FULL_INFO, "servoPark", (LM_NOTICE, "THE SYSTEM IS PERFORMING A SCAN, CANNOT PARK NOW"));
        _EXCPT(ManagementErrors::ParkingErrorExImpl, impl, "SRTMinorServoBossCore::park()");
        impl.setSubsystem("MinorServo");
        impl.setReason("The system is waiting for a scan to be completed.");
        impl.log(LM_DEBUG);
        throw impl;
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

    // Skipping this because of the cover not always engaging the limit switch
    /*try
    {
        // Send the STOW command to close the gregorian cover
        if(!m_socket->sendCommand(SRTMinorServoCommandLibrary::stow("GREGORIAN_CAP", COVER_STATUS_CLOSED)).checkOutput())
        {
            _EXCPT(ManagementErrors::ParkingErrorExImpl, impl, "SRTMinorServoBossCore::park()");
            impl.setSubsystem("MinorServo");
            impl.setReason("Sending STOW to the gregorian cover received NAK!");
            impl.log(LM_DEBUG);
            throw impl;
        }
    }
    catch(MinorServoErrors::MinorServoErrorsExImpl& mse)
    {
        _ADD_BACKTRACE(ManagementErrors::ParkingErrorExImpl, impl, mse, "SRTMinorServoBossCore::park()");
        impl.setSubsystem("MinorServo");
        impl.setReason("Error while sending the STOW command to the gregorian cover.");
        impl.log(LM_DEBUG);
        throw impl;
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
            _ADD_BACKTRACE(ManagementErrors::ParkingErrorExImpl, impl, mse, "SRTMinorServoBossCore::park()");
            impl.setSubsystem("MinorServo");
            impl.setReason(("Error while sending the STOP command to " + servo_name + ".").c_str());
            impl.log(LM_DEBUG);
            throw impl;
        }
    }

    // Start the park thread
    try
    {
        startThread(m_park_thread);
    }
    catch(ComponentErrors::CanNotStartThreadExImpl& ce)
    {
        _ADD_BACKTRACE(ManagementErrors::ParkingErrorExImpl, impl, ce, "SRTMinorServoBossCore::park()");
        impl.setSubsystem("MinorServo");
        impl.setReason("Error while trying to start the park_thread");
        impl.log(LM_DEBUG);
        throw impl;
    }
}

void SRTMinorServoBossCore::setElevationTracking(const char* configuration_c)
{
    AUTO_TRACE("SRTMinorServoBossCore::setElevationTracking()");

    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::setElevationTracking()");
        impl.setReason("The system is waiting for a scan to be completed.");
        impl.log(LM_DEBUG);
        throw impl;
    }
    else if(m_error_code.load() != ERROR_NO_ERROR)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::setElevationTracking()");
        impl.setReason("The system is in error state. Reset the errors first with the 'servoReset' command.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    std::string configuration = configuration_c;
    std::transform(configuration.begin(), configuration.end(), configuration.begin(), ::toupper);

    if(configuration != "ON" && configuration != "OFF")
    {
        _EXCPT(MinorServoErrors::ConfigurationErrorExImpl, impl, "SRTMinorServoBossCore::setElevationTracking()");
        impl.addData("Reason", ("Unknown setElevationTracking configuration '" + configuration + "'.").c_str());
        impl.log(LM_DEBUG);
        throw impl;
    }

    ACS_LOG(LM_FULL_INFO, "setServoElevationTracking", (LM_NOTICE, ("SETTING ELEVATION TRACKING TO " + configuration).c_str()));

    if(configuration == "ON" && m_elevation_tracking_enabled.load() != Management::MNG_TRUE)
    {
        m_elevation_tracking_enabled.store(Management::MNG_TRUE);

        if(m_motion_status.load() == MOTION_STATUS_CONFIGURED)
        {
            m_motion_status.store(MOTION_STATUS_TRACKING);

            try
            {
                startThread(m_tracking_thread);
            }
            catch(ComponentErrors::CanNotStartThreadExImpl& ce)
            {
                _ADD_BACKTRACE(MinorServoErrors::StatusErrorExImpl, impl, ce, "SRTMinorServoBossCore::setElevationTracking()");
                impl.setReason("Error while trying to start the tracking thread!");
                impl.log(LM_DEBUG);
                throw impl;
            }
        }
    }
    else if(configuration == "OFF" && m_elevation_tracking_enabled.load() != Management::MNG_FALSE)
    {
        stopThread(m_tracking_thread);
        m_elevation_tracking_enabled.store(Management::MNG_FALSE);

        if(m_motion_status.load() == MOTION_STATUS_TRACKING)
        {
            m_motion_status.store(MOTION_STATUS_CONFIGURED);

            try
            {
                preset(getElevation(getTimeStamp()));
            }
            catch(...)
            {
            }
        }
    }
}

void SRTMinorServoBossCore::setASConfiguration(const char* configuration_c)
{
    AUTO_TRACE("SRTMinorServoBossCore::setASConfiguration()");

    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::setASConfiguration()");
        impl.setReason("The system is waiting for a scan to be completed.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    std::string configuration = configuration_c;
    std::transform(configuration.begin(), configuration.end(), configuration.begin(), ::toupper);

    if(configuration != "ON" && configuration != "OFF")
    {
        _EXCPT(MinorServoErrors::ConfigurationErrorExImpl, impl, "SRTMinorServoBossCore::setASConfiguration()");
        impl.addData("Reason", ("Unknown setASConfiguration configuration '" + configuration + "'.").c_str());
        impl.log(LM_DEBUG);
        throw impl;
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
    if(!m_commanded_setup.empty() && m_motion_status.load() != MOTION_STATUS_PARKED)
    {
        configuration = m_commanded_setup.substr(0, m_commanded_setup.find("_"));

        try
        {
            setup(configuration.c_str());
        }
        catch(ManagementErrors::ConfigurationErrorExImpl& ce)
        {
            _ADD_BACKTRACE(MinorServoErrors::ConfigurationErrorExImpl, impl, ce, "SRTMinorServoBossCore::setASConfiguration()");
            impl.log(LM_DEBUG);
            throw impl;
        }
    }
}

void SRTMinorServoBossCore::setGregorianCoverPosition(const char* position_c)
{
    AUTO_TRACE("SRTMinorServoBossCore::setGregorianCoverPosition()");

    checkLineStatus();

    std::string position = position_c;
    std::transform(position.begin(), position.end(), position.begin(), ::toupper);

    if(position != "OPEN" && position != "CLOSED")
    {
        _EXCPT(MinorServoErrors::StowErrorExImpl, impl, "SRTMinorServoBossCore::setGregorianCoverPosition()");
        impl.addData("Reason", ("Unknown position '" + position + "'.").c_str());
        impl.log(LM_DEBUG);
        throw impl;
    }

    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status != MOTION_STATUS_PARKING && motion_status != MOTION_STATUS_PARKED)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::setGregorianCoverPosition()");
        impl.setReason("You can set the gregorian cover position only when the system is parked or is parking.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    SRTMinorServoGregorianCoverStatus desired_position;
    if(position == "OPEN")
    {
        desired_position = COVER_STATUS_OPEN;
    }
    else
    {
        desired_position = COVER_STATUS_CLOSED;
    }

    if(desired_position != m_status.getGregorianCoverPosition())
    {
        ACS_LOG(LM_FULL_INFO, "setGregorianCoverPosition", (LM_NOTICE, ("SETTING GREGORIAN COVER POSITION TO " + position).c_str()));

        try
        {
            if(!m_socket->sendCommand(SRTMinorServoCommandLibrary::stow("GREGORIAN_CAP", desired_position)).checkOutput())
            {
                _EXCPT(MinorServoErrors::StowErrorExImpl, impl, "SRTMinorServoBossCore::setGregorianCoverPosition()");
                impl.addData("Reason", "Sending STOW to the gregorian cover received NAK!");
                impl.log(LM_DEBUG);
                throw impl;
            }
        }
        catch(MinorServoErrors::MinorServoErrorsEx& ex)
        {
            _ADD_BACKTRACE(MinorServoErrors::StatusErrorExImpl, impl, ex, "SRTMinorServoBossCore::setGregorianCoverPosition()");
            impl.setReason("Communication error while sending a STOW command to the gregorian cover.");
            impl.log(LM_DEBUG);
            throw impl;
        }
    }
}

void SRTMinorServoBossCore::setGregorianAirBladeStatus(const char* status_c)
{
    AUTO_TRACE("SRTMinorServoBossCore::setGregorianAirBladeStatus()");

    checkLineStatus();

    std::string status = status_c;
    std::transform(status.begin(), status.end(), status.begin(), ::toupper);

    if(status != "ON" && status != "OFF" && status != "AUTO")
    {
        _EXCPT(MinorServoErrors::StowErrorExImpl, impl, "SRTMinorServoBossCore::setGregorianAirBladeStatus()");
        impl.addData("Reason", ("Unknown status '" + status + "'.").c_str());
        impl.log(LM_DEBUG);
        throw impl;
    }

    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    SRTMinorServoGregorianCoverStatus cover_status = m_status.getGregorianCoverPosition();
    if((motion_status != MOTION_STATUS_CONFIGURED && motion_status != MOTION_STATUS_TRACKING) || cover_status == COVER_STATUS_CLOSED)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::setGregorianAirBladeStatus()");
        impl.setReason("You can set the gregorian cover air blade status only when the system is configured or tracking and the gregorian cover is open.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    SRTMinorServoGregorianAirBladeStatus desired_status;
    if(status == "ON")
    {
        desired_status = AIR_BLADE_STATUS_ON;
    }
    else if(status == "OFF")
    {
        desired_status = AIR_BLADE_STATUS_OFF;
    }
    else
    {
        desired_status = AIR_BLADE_STATUS_AUTO;
    }

    if(desired_status != m_status.getGregorianAirBladeStatus())
    {
        ACS_LOG(LM_FULL_INFO, "setGregorianAirBladeStatus", (LM_NOTICE, ("SETTING AIR BLADE STATUS TO " + status).c_str()));

        try
        {
            if(!m_socket->sendCommand(SRTMinorServoCommandLibrary::stow("GREGORIAN_CAP", 2 + (unsigned int)desired_status)).checkOutput())
            {
                _EXCPT(MinorServoErrors::StowErrorExImpl, impl, "SRTMinorServoBossCore::setGregorianAirBladeStatus()");
                impl.addData("Reason", "Sending STOW to the gregorian cover received NAK!");
                impl.log(LM_DEBUG);
                throw impl;
            }
        }
        catch(MinorServoErrors::MinorServoErrorsEx& ex)
        {
            _ADD_BACKTRACE(MinorServoErrors::StatusErrorExImpl, impl, ex, "SRTMinorServoBossCore::setGregorianAirBladeStatus()");
            impl.setReason("Communication error while sending a STOW command to the gregorian cover.");
            impl.log(LM_DEBUG);
            throw impl;
        }
    }
}

void SRTMinorServoBossCore::preset(double elevation)
{
    // Elevation is expressed in degrees
    AUTO_TRACE("SRTMinorServoBossCore::preset()");

    checkLineStatus();

    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::preset()");
        impl.setReason("The system is waiting for a scan to be completed.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    if(m_motion_status.load() != MOTION_STATUS_CONFIGURED)
    {
        _EXCPT(MinorServoErrors::PositioningErrorExImpl, impl, "SRTMinorServoBossCore::preset()");
        impl.addData("Reason", "Minor servos are tracking, cannot send a global preset command now.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    for(const auto& [servo_name, servo] : m_current_servos)
    {
        try
        {
            servo->preset(*servo->calcCoordinates(elevation));
        }
        catch(MinorServoErrors::MinorServoErrorsEx& ex)
        {
            _ADD_BACKTRACE(MinorServoErrors::PositioningErrorExImpl, impl, ex, "SRTMinorServoBossCore::preset()");
            impl.addData("Reason", ("Raised error while commanding PRESET to " + servo_name + ".").c_str());
            impl.log(LM_DEBUG);
            throw impl;
        }
    }
}

void SRTMinorServoBossCore::clearUserOffsets(const char* servo_name_c)
{
    AUTO_TRACE("SRTMinorServoBossCore::clearUserOffsets()");

    checkLineStatus();

    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::clearUserOffsets()");
        impl.setReason("The system is waiting for a scan to be completed.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status != MOTION_STATUS_CONFIGURED && motion_status != MOTION_STATUS_TRACKING)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::clearUserOffsets()");
        impl.setReason("The system is not configured yet.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    std::string servo_name = servo_name_c;
    std::transform(servo_name.begin(), servo_name.end(), servo_name.begin(), ::toupper);

    std::map<std::string, SRTBaseMinorServo_ptr> clear_these_offsets;

    if(servo_name == "ALL")
    {
        clear_these_offsets = m_current_servos;
    }
    else if(!m_servos.count(servo_name))
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, impl, "SRTMinorServoBossCore::clearUserOffsets()");
        impl.addData("Reason", ("Unknown servo '" + servo_name + "'.").c_str());
        impl.log(LM_DEBUG);
        throw impl;
    }
    else if(!m_current_servos.count(servo_name))
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, impl, "SRTMinorServoBossCore::clearUserOffsets()");
        impl.addData("Reason", ("Servo '" + servo_name + "' not in use with the current configuration.").c_str());
        impl.log(LM_DEBUG);
        throw impl;
    }
    else
    {
        clear_these_offsets.insert({servo_name, m_current_servos.at(servo_name)});
    }

    for(const auto& [servo_name, servo] : clear_these_offsets)
    {
        try
        {
            servo->clearUserOffsets();
        }
        catch(MinorServoErrors::MinorServoErrorsEx& ex)
        {
            _ADD_BACKTRACE(MinorServoErrors::OffsetErrorExImpl, impl, ex, "SRTMinorServoBossCore::clearUserOffsets()");
            impl.addData("Reason", ("Failed to clear user offsets for servo " + servo_name + ".").c_str());
            impl.log(LM_DEBUG);
            throw impl;
        }

        if(motion_status == MOTION_STATUS_CONFIGURED || m_tracking_servos.find(servo_name) == m_tracking_servos.end())
        {
            // We get here in 2 different scenarios
            // 1) The system is configured and is not tracking the elevation, therefore we ALWAYS need to update the position of the servo we just set the offset
            // 2) The servo we set the offset is not a program track servo, therefore we need to update its position manually
            try
            {
                servo->preset(ACS::doubleSeq());
            }
            catch(MinorServoErrors::MinorServoErrorsEx& ex)
            {
                _ADD_BACKTRACE(MinorServoErrors::OffsetErrorExImpl, impl, ex, "SRTMinorServoBossCore::clearUserOffsets()");
                impl.addData("Reason", ("Failed to reposition servo " + servo_name + " after clearing user offsets.").c_str());
                impl.log(LM_DEBUG);
                throw impl;
            }
        }
    }
}

void SRTMinorServoBossCore::setUserOffset(const char* servo_axis_name_c, const double& offset)
{
    AUTO_TRACE("SRTMinorServoBossCore::setUserOffset()");

    checkLineStatus();

    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::setUserOffset()");
        impl.setReason("The system is waiting for a scan to be completed.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status != MOTION_STATUS_CONFIGURED && motion_status != MOTION_STATUS_TRACKING)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::setUserOffset()");
        impl.setReason("The system is not configured yet.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    std::string servo_axis_name = servo_axis_name_c;
    std::transform(servo_axis_name.begin(), servo_axis_name.end(), servo_axis_name.begin(), ::toupper);

    std::stringstream ss(servo_axis_name);
    std::string servo_name, axis_name;
    std::getline(ss, servo_name, '_');
    ss >> axis_name;

    if(!m_servos.count(servo_name))
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, impl, "SRTMinorServoBossCore::setUserOffsets()");
        impl.addData("Reason", ("Unknown servo '" + servo_name + "'.").c_str());
        impl.log(LM_DEBUG);
        throw impl;
    }

    try
    {
        auto servo = m_current_servos.at(servo_name);

        try
        {
            servo->setUserOffset(axis_name.c_str(), offset);
        }
        catch(MinorServoErrors::MinorServoErrorsEx& ex)
        {
            _ADD_BACKTRACE(MinorServoErrors::OffsetErrorExImpl, impl, ex, "SRTMinorServoBossCore::setUserOffsets()");
            impl.addData("Reason", ("Failed to set user offset for servo " + servo_name + ".").c_str());
            impl.log(LM_DEBUG);
            throw impl;
        }

        if(motion_status == MOTION_STATUS_CONFIGURED || m_tracking_servos.find(servo_name) == m_tracking_servos.end())
        {
            // We get here in 2 different scenarios
            // 1) The system is configured and is not tracking the elevation, therefore we ALWAYS need to update the position of the servo we just set the offset
            // 2) The servo we set the offset is not a program track servo, therefore we need to update its position manually
            try
            {
                servo->preset(ACS::doubleSeq());
            }
            catch(MinorServoErrors::MinorServoErrorsEx& ex)
            {
                _ADD_BACKTRACE(MinorServoErrors::OffsetErrorExImpl, impl, ex, "SRTMinorServoBossCore::setUserOffset()");
                impl.addData("Reason", ("Failed to reposition servo " + servo_name + " after setting user offset.").c_str());
                impl.log(LM_DEBUG);
                throw impl;
            }
        }
    }
    catch(std::out_of_range& oor)
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, impl, "SRTMinorServoBossCore::setUserOffset()");
        impl.addData("Reason", ("Servo '" + servo_name + "' not in use with the current configuration.").c_str());
        impl.log(LM_DEBUG);
        throw impl;
    }
}

ACS::doubleSeq* SRTMinorServoBossCore::getUserOffsets(ACS::Time acs_time)
{
    AUTO_TRACE("SRTMinorServoBossCore::getUserOffsets()");

    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status != MOTION_STATUS_CONFIGURED && motion_status != MOTION_STATUS_TRACKING)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::getUserOffsets()");
        impl.setReason("The system is not configured yet.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    ACS::doubleSeq_var offsets = new ACS::doubleSeq;

    for(const auto& [servo_name, servo] : m_current_servos)
    {
        ACS::doubleSeq servo_offsets = *servo->getUserOffsets(acs_time);
        size_t start_index = offsets->length();
        offsets->length(start_index + servo_offsets.length());
        std::copy(servo_offsets.begin(), servo_offsets.end(), offsets->begin() + start_index);
    }

    return offsets._retn();
}

void SRTMinorServoBossCore::clearSystemOffsets(const char* servo_name_c)
{
    AUTO_TRACE("SRTMinorServoBossCore::clearSystemOffsets()");

    checkLineStatus();

    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::clearSystemOffsets()");
        impl.setReason("The system is waiting for a scan to be completed.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status != MOTION_STATUS_CONFIGURED && motion_status != MOTION_STATUS_TRACKING)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::clearSystemOffsets()");
        impl.setReason("The system is not configured yet.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    std::string servo_name = servo_name_c;
    std::transform(servo_name.begin(), servo_name.end(), servo_name.begin(), ::toupper);

    std::map<std::string, SRTBaseMinorServo_ptr> clear_these_offsets;

    if(servo_name == "ALL")
    {
        clear_these_offsets = m_current_servos;
    }
    else if(!m_servos.count(servo_name))
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, impl, "SRTMinorServoBossCore::clearSystemOffsets()");
        impl.addData("Reason", ("Unknown servo '" + servo_name + "'.").c_str());
        impl.log(LM_DEBUG);
        throw impl;
    }
    else if(!m_current_servos.count(servo_name))
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, impl, "SRTMinorServoBossCore::clearSystemOffsets()");
        impl.addData("Reason", ("Servo '" + servo_name + "' not in use with the current configuration.").c_str());
        impl.log(LM_DEBUG);
        throw impl;
    }
    else
    {
        clear_these_offsets.insert({servo_name, m_current_servos.at(servo_name)});
    }

    for(const auto& [servo_name, servo] : clear_these_offsets)
    {
        try
        {
            servo->clearSystemOffsets();
        }
        catch(MinorServoErrors::MinorServoErrorsEx& ex)
        {
            _ADD_BACKTRACE(MinorServoErrors::OffsetErrorExImpl, impl, ex, "SRTMinorServoBossCore::clearSystemOffsets()");
            impl.addData("Reason", ("Failed to clear system offsets for servo " + servo_name + ".").c_str());
            impl.log(LM_DEBUG);
            throw impl;
        }

        if(motion_status == MOTION_STATUS_CONFIGURED || m_tracking_servos.find(servo_name) == m_tracking_servos.end())
        {
            // We get here in 2 different scenarios
            // 1) The system is configured and is not tracking the elevation, therefore we ALWAYS need to update the position of the servo we just set the offset
            // 2) The servo we set the offset is not a program track servo, therefore we need to update its position manually
            try
            {
                servo->preset(ACS::doubleSeq());
            }
            catch(MinorServoErrors::MinorServoErrorsEx& ex)
            {
                _ADD_BACKTRACE(MinorServoErrors::OffsetErrorExImpl, impl, ex, "SRTMinorServoBossCore::clearSystemOffsets()");
                impl.addData("Reason", ("Failed to reposition servo " + servo_name + " after clearing system offsets.").c_str());
                impl.log(LM_DEBUG);
                throw impl;
            }
        }
    }
}

void SRTMinorServoBossCore::setSystemOffset(const char* servo_axis_name_c, double offset)
{
    AUTO_TRACE("SRTMinorServoBossCore::setSystemOffset()");

    checkLineStatus();

    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::setSystemOffset()");
        impl.setReason("The system is waiting for a scan to be completed.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status != MOTION_STATUS_CONFIGURED && motion_status != MOTION_STATUS_TRACKING)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::setSystemOffset()");
        impl.setReason("The system is not configured yet.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    std::string servo_axis_name = servo_axis_name_c;
    std::transform(servo_axis_name.begin(), servo_axis_name.end(), servo_axis_name.begin(), ::toupper);

    std::stringstream ss(servo_axis_name);
    std::string servo_name, axis_name;
    std::getline(ss, servo_name, '_');
    ss >> axis_name;

    if(!m_servos.count(servo_name))
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, impl, "SRTMinorServoBossCore::setSystemOffsets()");
        impl.addData("Reason", ("Unknown servo '" + servo_name + "'.").c_str());
        impl.log(LM_DEBUG);
        throw impl;
    }

    try
    {
        auto servo = m_current_servos.at(servo_name);

        try
        {
            servo->setSystemOffset(axis_name.c_str(), offset);
        }
        catch(MinorServoErrors::MinorServoErrorsEx& ex)
        {
            _ADD_BACKTRACE(MinorServoErrors::OffsetErrorExImpl, impl, ex, "SRTMinorServoBossCore::setSystemOffsets()");
            impl.addData("Reason", ("Failed to set system offset for servo " + servo_name + ".").c_str());
            impl.log(LM_DEBUG);
            throw impl;
        }

        if(motion_status == MOTION_STATUS_CONFIGURED || m_tracking_servos.find(servo_name) == m_tracking_servos.end())
        {
            // We get here in 2 different scenarios
            // 1) The system is configured and is not tracking the elevation, therefore we ALWAYS need to update the position of the servo we just set the offset
            // 2) The servo we set the offset is not a program track servo, therefore we need to update its position manually
            try
            {
                servo->preset(ACS::doubleSeq());
            }
            catch(MinorServoErrors::MinorServoErrorsEx& ex)
            {
                _ADD_BACKTRACE(MinorServoErrors::OffsetErrorExImpl, impl, ex, "SRTMinorServoBossCore::setSystemOffset()");
                impl.addData("Reason", ("Failed to reposition servo " + servo_name + " after setting system offset.").c_str());
                impl.log(LM_DEBUG);
                throw impl;
            }
        }
    }
    catch(std::out_of_range& oor)
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, impl, "SRTMinorServoBossCore::setSystemOffset()");
        impl.addData("Reason", ("Servo '" + servo_name + "' not in use with the current configuration.").c_str());
        impl.log(LM_DEBUG);
        throw impl;
    }
}

ACS::doubleSeq* SRTMinorServoBossCore::getSystemOffsets(ACS::Time acs_time)
{
    AUTO_TRACE("SRTMinorServoBossCore::getSystemOffsets()");

    SRTMinorServoMotionStatus motion_status = m_motion_status.load();
    if(motion_status != MOTION_STATUS_CONFIGURED && motion_status != MOTION_STATUS_TRACKING)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::getSystemOffsets()");
        impl.setReason("The system is not configured yet.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    ACS::doubleSeq_var offsets = new ACS::doubleSeq;

    for(const auto& [servo_name, servo] : m_current_servos)
    {
        ACS::doubleSeq servo_offsets = *servo->getSystemOffsets(acs_time);
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
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::getAxesInfo()");
        impl.setReason("The system is not configured yet.");
        impl.log(LM_DEBUG);
        throw impl;
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
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::getAxesPositions()");
        impl.setReason("The system is not configured yet.");
        impl.log(LM_DEBUG);
        throw impl;
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
    _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::checkScanFeasibility()");

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
        impl.setReason(("Unknown servo '" + servo_name + "'.").c_str());
        impl.log(LM_DEBUG);
        throw impl;
    }
    else if(!m_tracking_servos.count(servo_name))
    {
        impl.setReason(("Servo '" + servo_name + "' is not a tracking servo.").c_str());
        impl.log(LM_DEBUG);
        throw impl;
    }
    else if(!m_current_tracking_servos.count(servo_name))
    {
        impl.setReason(("Servo '" + servo_name + "' not in use with the current configuration.").c_str());
        impl.log(LM_DEBUG);
        throw impl;
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
        impl.setReason(("Axis '" + axis_code + "' not found.").c_str());
        impl.log(LM_DEBUG);
        throw impl;
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

    // Read the current servo offsets
    ACS::doubleSeq user_offsets = *servo->getUserOffsets(0);
    ACS::doubleSeq system_offsets = *servo->getSystemOffsets(0);

    // Check if starting or final positions are outside the axes range (considering offsets)
    for(size_t i = 0; i < starting_position.length(); i++)
    {
        if(starting_position[i] + user_offsets[i] + system_offsets[i] < min_ranges[i] || starting_position[i] + user_offsets[i] + system_offsets[i] > max_ranges[i])
        {
            impl.setReason("Starting position out of range.");
            impl.log(LM_DEBUG);
            throw impl;
        }
        else if(final_position[i] + user_offsets[i] + system_offsets[i] < min_ranges[i] || final_position[i] + user_offsets[i] + system_offsets[i] > max_ranges[i])
        {
            impl.setReason("Final position out of range.");
            impl.log(LM_DEBUG);
            throw impl;
        }
    }

    // Time to reach the desired position if I were to start the movement now
    ACS::Time min_starting_time = getTimeStamp() + servo->getTravelTime(ACS::doubleSeq(), starting_position) + PROGRAM_TRACK_FUTURE_TIME;

    if(start_time != 0 && min_starting_time > start_time)
    {
        impl.setReason("Not enough time to start the scan.");
        impl.log(LM_DEBUG);
        throw impl;
    }
    else if(servo->getTravelTime(starting_position, final_position) > scan_info.total_time)  // Check scan total time
    {
        impl.setReason("Not enough time to perform the scan.");
        impl.log(LM_DEBUG);
        throw impl;
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
    catch(MinorServoErrors::StatusErrorExImpl& impl)
    {
        // Something is not working, return ASAP
        impl.log(LM_CRITICAL);
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
    catch(MinorServoErrors::StatusErrorExImpl& impl)
    {
        impl.log(LM_CRITICAL);
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
    //TODO: restart from here
    AUTO_TRACE("SRTMinorServoBossCore::startScan()");

    // This will throw if anything is not working, no need to catch it since we will throw it again anyway
    checkLineStatus();

    // Check if we are already performing another scan
    if(m_scan_active.load() == Management::MNG_TRUE)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::startScan()");
        impl.setReason("The system is waiting for a scan to be completed.");
        impl.log(LM_DEBUG);
        throw impl;
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
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::startScan()");
        impl.setReason("The system is not ready yet.");
        impl.log(LM_DEBUG);
        throw impl;
    }

    // This will throw if anything is not working, no need to catch it since we will throw it again anyway
    SRTMinorServoScan scan = checkScanFeasibility(start_time, scan_info, antenna_info);

    stopThread(m_tracking_thread);

    // If we got here we are safe to start the scan
    m_current_scan = scan;
    start_time = scan.start_time;
    m_scan_active.store(Management::MNG_TRUE);

    try
    {
        startThread(m_scan_thread);
    }
    catch(ComponentErrors::CanNotStartThreadExImpl& ce)
    {
        _ADD_BACKTRACE(MinorServoErrors::StatusErrorExImpl, impl, ce, "SRTMinorServoBossCore::startScan()");
        impl.setReason("Error while trying to start the scan thread!");
        impl.log(LM_DEBUG);
        throw impl;
    }
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
    catch(ComponentErrors::CouldntGetComponentExImpl& impl)
    {
        impl.addData("Reason", "Cannot get the ANTENNA/Boss component");
        throw impl;
    }
}

void SRTMinorServoBossCore::checkLineStatus()
{
    AUTO_TRACE("SRTMinorServoBossCore::checkLineStatus()");

    if(!m_socket->isConnected())
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::checkLineStatus()");
        impl.setReason("Socket not connected.");
        impl.log(LM_DEBUG);
        setError(ERROR_NOT_CONNECTED);
        throw impl;
    }

    if(m_status.getControl() != CONTROL_DISCOS)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::checkLineStatus()");
        impl.setReason("MinorServo system is not controlled by DISCOS.");
        impl.log(LM_DEBUG);
        setError(ERROR_MAINTENANCE);
        throw impl;
    }

    if(m_status.emergencyPressed() == Management::MNG_TRUE)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, "SRTMinorServoBossCore::checkLineStatus()");
        impl.setReason("MinorServo system in emergency status.");
        impl.log(LM_DEBUG);
        setError(ERROR_EMERGENCY_STOP);
        throw impl;
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
    catch(acsthreadErrType::CanNotSpawnThreadExImpl& _impl)
    {
        // The thread failed to start for some reason
        _ADD_BACKTRACE(ComponentErrors::CanNotStartThreadExImpl, impl, _impl, "SRTMinorServoBossCore::startThread()");
        impl.setThreadName(T::c_thread_name);
        impl.log(LM_DEBUG);
        setError(ERROR_CONFIG_ERROR);
        throw impl;
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

void SRTMinorServoBossCore::setError(SRTMinorServoError error)
{
    AUTO_TRACE("SRTMinorServoBossCore::setError()");

    m_commanded_setup = "";
    m_actual_setup = "Error";
    m_subsystem_status.store(Management::MNG_FAILURE);
    m_ready.store(Management::MNG_FALSE);
    m_elevation_tracking.store(Management::MNG_FALSE);
    m_starting.store(Management::MNG_FALSE);
    m_scan_active.store(Management::MNG_FALSE);
    m_scanning.store(Management::MNG_FALSE);
    m_tracking.store(Management::MNG_FALSE);
    m_motion_status.store(MOTION_STATUS_ERROR);
    m_error_code.store(error);
}

void SRTMinorServoBossCore::reset(bool vbrain_reset)
{
    AUTO_TRACE("SRTMinorServoBossCore::reset()");

    if(m_error_code.load() == ERROR_NO_ERROR)
    {
        return;
    }

    if(vbrain_reset)
    {
        try
        {
            std::string protocol = getCDBValue<std::string>(m_component.getContainerServices(), "Protocol", "MINORSERVO/VBrain");
            std::string address = getCDBValue<std::string>(m_component.getContainerServices(), "IPAddress", "MINORSERVO/VBrain");
            std::string port = getCDBValue<std::string>(m_component.getContainerServices(), "Port", "MINORSERVO/VBrain");

            std::string baseUrl = protocol + "://" + address + ":" + port + "/Exporting/json/ExecuteCommand?name";
            std::string emergencyUrl = baseUrl + "=INAF_SRT_OR7_EMG_RESET_CMD";
            std::string alarmsUrl = baseUrl + "=INAF_SRT_OR7_RESET_CMD";

            CURL* curl = curl_easy_init();
            if(curl)
            {
                curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                    +[](void* contents, size_t size, size_t nmemb, void* userp) -> size_t
                    {
                        return size * nmemb;
                    }
                );

                ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossCore::reset()", (LM_DEBUG, "Sending VBrain emergency reset"));
                curl_easy_setopt(curl, CURLOPT_URL, emergencyUrl.c_str());

                CURLcode res = curl_easy_perform(curl);
                if(res != CURLE_OK)
                {
                    std::string err = "VBrain emergency reset failed: " + std::string(curl_easy_strerror(res));
                    ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossCore::reset()", (LM_WARNING, err.c_str()));
                }

                IRA::CIRATools::Wait(3, 0);

                ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossCore::reset()", (LM_DEBUG, "Sending VBrain alarms reset"));
                curl_easy_setopt(curl, CURLOPT_URL, alarmsUrl.c_str());

                res = curl_easy_perform(curl);
                if(res != CURLE_OK)
                {
                    std::string err = "VBrain alarms reset failed: " + std::string(curl_easy_strerror(res));
                    ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossCore::reset()", (LM_WARNING, err.c_str()));
                }

                curl_easy_cleanup(curl);
            }
            else
            {
                ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossCore::reset()", (LM_WARNING, "Failed to initialize libcurl for VBrain reset"));
            }
        }
        catch (...)
        {
            ACS_LOG(LM_FULL_INFO, "SRTMinorServoBossCore::reset()", (LM_WARNING, "Exception ignored during VBrain HTTP reset logic."));
        }
    }

    if(m_socket_connected.load() == Management::MNG_FALSE)
    {
        // If we are still not connected we should not proceed with the reset
        return;
    }

    for(const auto& [servo_name, servo] : m_servos)
    {
        servo->reset();
    }

    m_actual_setup = "Unknown";
    m_subsystem_status.store(Management::MNG_WARNING);
    m_motion_status.store(MOTION_STATUS_UNCONFIGURED);
    m_error_code.store(ERROR_NO_ERROR);
}

std::map<std::string, std::string> SRTMinorServoBossCore::loadConfigurations()
{
    AUTO_TRACE("SRTMinorServoBossCore::loadConfigurations()");

    IRA::CDBTable table(m_component.getContainerServices(), "configuration", std::string("DataBlock/MinorServo/Boss").c_str());
    IRA::CError error;
    error.Reset();

    if(!table.addField(error, "DISCOS", IRA::CDataField::STRING))
    {
        error.setExtra("Error adding field DISCOS", 0);
    }
    if(!table.addField(error, "LDO", IRA::CDataField::STRING))
    {
        error.setExtra("Error adding field LDO", 0);
    }
    if(!error.isNoError())
    {
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl, impl, error);
        impl.setCode(error.getErrorCode());
        impl.setDescription((const char *)error.getDescription());
        impl.log(LM_DEBUG);
        throw impl;
    }
    if(!table.openTable(error))
    {
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, impl, error);
        impl.log(LM_DEBUG);
        throw impl;
    }

    std::map<std::string, std::string> map;

    table.First();
    for(unsigned int i = 0; i < table.recordCount(); i++, table.Next())
    {
        map[std::string(table["DISCOS"]->asString())] = std::string(table["LDO"]->asString());
    }
    table.closeTable();

    return map;
}

Management::TBoolean SRTMinorServoBossCore::getCDBConfiguration(std::string which_configuration)
{
    AUTO_TRACE("SRTMinorServoBossCore::getCDBConfiguration()");
    std::string configuration = getCDBValue<std::string>(m_component.getContainerServices(), which_configuration);
    std::transform(configuration.begin(), configuration.end(), configuration.begin(), ::toupper);

    if(configuration != "ON" && configuration != "OFF")
    {
        _EXCPT(ComponentErrors::CDBAccessExImpl, impl, "SRTMinorServoBossCore::getCDBConfiguration()");
        impl.setFieldName(which_configuration.c_str());
        impl.addData("Reason", "Value should be 'ON' or 'OFF'");
        impl.log(LM_DEBUG);
        throw impl;
    }

    return configuration == "ON" ? Management::MNG_TRUE : Management::MNG_FALSE;
}
