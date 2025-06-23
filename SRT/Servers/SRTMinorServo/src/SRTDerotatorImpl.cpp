#include "SRTDerotatorImpl.h"

using namespace MinorServo;

SRTDerotatorImpl::SRTDerotatorImpl(const ACE_CString& component_name, maci::ContainerServices* container_services):
    CharacteristicComponentImpl(component_name, container_services),
    m_component_name(std::string(component_name.c_str())),
    m_servo_name(std::string(strchr(component_name.c_str(), '/') + 1)),
    m_virtual_axes(getCDBValue<size_t>(container_services, "virtual_axes")),
    m_physical_axes(getCDBValue<size_t>(container_services, "physical_axes")),
    m_virtual_axes_names(SRTDerotatorImpl::getPropertiesTable(*this, "virtual_positions")),
    m_virtual_axes_units(SRTDerotatorImpl::getPropertiesTable(*this, "virtual_axes_units")),
    m_status(
        m_servo_name,
        SRTDerotatorImpl::getPropertiesTable(*this, "physical_axes_enabled"),
        SRTDerotatorImpl::getPropertiesTable(*this, "physical_positions"),
        m_virtual_axes_names,
        SRTDerotatorImpl::getPropertiesTable(*this, "virtual_offsets")
    ),
    m_positions_queue(5 * 60 * int(1 / getCDBValue<double>(container_services, "status_thread_period", "/MINORSERVO/Boss")), m_virtual_axes),
    m_zero_offset(getCDBValue<double>(container_services, "zero_offset")),
    m_min(-(getCDBValue<double>(container_services, "max_range") - m_zero_offset)),
    m_max(-(getCDBValue<double>(container_services, "min_range") - m_zero_offset)),
    m_tracking_queue(1500, m_virtual_axes),
    m_step(getCDBValue<double>(container_services, "step")),
    m_tracking_delta(getCDBValue<double>(container_services, "tracking_delta")),
    m_tracking_error(m_virtual_axes, 0.0),
    m_tracking(Management::MNG_FALSE),
    m_trajectory_id(0),
    m_total_trajectory_points(0),
    m_remaining_trajectory_points(0),
    m_commanded_position(0),
    m_position_difference(0),
    m_status_pattern(0),
    m_c_s(0),
    m_enabled_ptr(this),
    m_drive_cabinet_status_ptr(this),
    m_block_ptr(this),
    m_operative_mode_ptr(this),
    m_physical_axes_enabled_ptr(this),
    m_physical_positions_ptr(this),
    m_virtual_axes_ptr(this),
    m_plain_virtual_positions_ptr(this),
    m_virtual_positions_ptr(this),
    m_virtual_offsets_ptr(this),
    m_tracking_ptr(this),
    m_trajectory_id_ptr(this),
    m_total_trajectory_points_ptr(this),
    m_remaining_trajectory_points_ptr(this),
    m_actual_position_ptr(this),
    m_commanded_position_ptr(this),
    m_position_difference_ptr(this),
    m_status_ptr(this),
    m_socket_configuration(SRTMinorServoSocketConfiguration::getInstance(container_services)),
    m_socket(SRTMinorServoSocket::getInstance(m_socket_configuration.m_ip_address, m_socket_configuration.m_port, m_socket_configuration.m_timeout))
{
    AUTO_TRACE(m_servo_name + "::SRTDerotatorImpl()");
}

SRTDerotatorImpl::~SRTDerotatorImpl()
{
    AUTO_TRACE(m_servo_name + "::~SRTDerotatorImpl()");

    if(m_status_thread != nullptr)
    {
        m_status_thread->terminate();
        getContainerServices()->getThreadManager()->destroy(m_status_thread);
    }
}

void SRTDerotatorImpl::initialize()
{
    AUTO_TRACE(m_servo_name + "::initialize()");

    try
    {
        m_enabled_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((m_component_name + ":enabled").c_str(), getComponent(),
                new MSAnswerMapDevIO<Management::TBoolean, SRTMinorServoStatus>("enabled", m_status, &SRTMinorServoStatus::isEnabled), true);
        m_drive_cabinet_status_ptr = new ROEnumImpl<ACS_ENUM_T(SRTMinorServoCabinetStatus), POA_MinorServo::ROSRTMinorServoCabinetStatus>((m_component_name + ":drive_cabinet_status").c_str(), getComponent(),
                new MSAnswerMapDevIO<SRTMinorServoCabinetStatus, SRTMinorServoStatus>("drive_cabinet_status", m_status, &SRTMinorServoStatus::getDriveCabinetStatus), true);
        m_block_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((m_component_name + ":block").c_str(), getComponent(),
                new MSAnswerMapDevIO<Management::TBoolean, SRTMinorServoStatus>("block", m_status, &SRTMinorServoStatus::isBlocked), true);
        m_operative_mode_ptr = new ROEnumImpl<ACS_ENUM_T(SRTMinorServoOperativeMode), POA_MinorServo::ROSRTMinorServoOperativeMode>((m_component_name + ":operative_mode").c_str(), getComponent(),
                new MSAnswerMapDevIO<SRTMinorServoOperativeMode, SRTMinorServoStatus>("operative_mode", m_status, &SRTMinorServoStatus::getOperativeMode), true);
        m_physical_axes_enabled_ptr = new baci::RObooleanSeq((m_component_name + ":physical_axes_enabled").c_str(), getComponent(),
                new MSAnswerMapDevIO<ACS::booleanSeq, SRTMinorServoStatus>("physical_axes_enabled", m_status, &SRTMinorServoStatus::getPhysicalAxesEnabled), true);
        m_physical_positions_ptr = new baci::ROdoubleSeq((m_component_name + ":physical_positions").c_str(), getComponent(),
                new MSAnswerMapDevIO<ACS::doubleSeq, SRTMinorServoStatus>("physical_positions", m_status, &SRTMinorServoStatus::getPhysicalPositions), true);
        m_virtual_axes_ptr = new baci::ROlong((m_component_name + ":virtual_axes").c_str(), getComponent(),
                new MSGenericDevIO<CORBA::Long, const size_t>(m_virtual_axes), true);
        m_plain_virtual_positions_ptr = new baci::ROdoubleSeq((m_component_name + ":plain_virtual_positions").c_str(), getComponent(),
                new MSAnswerMapDevIO<ACS::doubleSeq, SRTMinorServoStatus>("plain_virtual_positions", m_status, &SRTMinorServoStatus::getPlainVirtualPositions), true);
        m_virtual_positions_ptr = new baci::ROdoubleSeq((m_component_name + ":virtual_positions").c_str(), getComponent(),
                new MSAnswerMapDevIO<ACS::doubleSeq, SRTMinorServoStatus>("virtual_positions", m_status, &SRTMinorServoStatus::getVirtualPositions), true);
        m_virtual_offsets_ptr = new baci::ROdoubleSeq((m_component_name + ":virtual_offsets").c_str(), getComponent(),
                new MSAnswerMapDevIO<ACS::doubleSeq, SRTMinorServoStatus>("virtual_offsets", m_status, &SRTMinorServoStatus::getVirtualOffsets), true);
        m_tracking_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((m_component_name + ":tracking").c_str(), getComponent(),
                new MSGenericDevIO<Management::TBoolean, std::atomic<Management::TBoolean>>(m_tracking), true);
        m_trajectory_id_ptr = new baci::ROlong((m_component_name + ":trajectory_id").c_str(), getComponent(),
                new MSGenericDevIO<CORBA::Long, std::atomic<unsigned int>>(m_trajectory_id), true);
        m_total_trajectory_points_ptr = new baci::ROlong((m_component_name + ":total_trajectory_points").c_str(), getComponent(),
                new MSGenericDevIO<CORBA::Long, std::atomic<unsigned int>>(m_total_trajectory_points), true);
        m_remaining_trajectory_points_ptr = new baci::ROlong((m_component_name + ":remaining_trajectory_points").c_str(), getComponent(),
                new MSGenericDevIO<CORBA::Long, std::atomic<unsigned int>>(m_remaining_trajectory_points), true);
        m_actual_position_ptr = new baci::ROdouble((m_component_name + ":actPosition").c_str(), getComponent(),
                new MSAnswerMapDevIO<CORBA::Double, SRTDerotatorStatus>("actPosition", m_status, &SRTDerotatorStatus::getActualPosition), true);
        m_commanded_position_ptr = new baci::RWdouble((m_component_name + ":cmdPosition").c_str(), getComponent(),
                new MSGenericDevIO<CORBA::Double, std::atomic<double>>(m_commanded_position), true);
        m_position_difference_ptr = new baci::ROdouble((m_component_name + ":positionDiff").c_str(), getComponent(),
                new MSGenericDevIO<CORBA::Double, std::atomic<double>>(m_position_difference), true);
        m_status_ptr = new baci::ROpattern((m_component_name + ":status").c_str(), getComponent(),
                new MSGenericDevIO<ACS::pattern, std::atomic<long>>(m_status_pattern), true);
    }
    catch(std::bad_alloc& ba)
    {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, ex, (m_servo_name + "::initialize()").c_str());
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }

    // Try to read the current status of the derotator
    try
    {
        updateStatus();
    }
    catch(...)
    {
        // This block is necessary since the socket might not be connected yet. If the Leonardo system is not reachable the status(); call will fail, but we want to instantiate the component anyway
        // A non connected socket will try to connect every time a new command is sent, therefore the status thread will establish the connection as soon as possible.
    }
}

void SRTDerotatorImpl::execute()
{
    AUTO_TRACE(m_servo_name + "::execute()");

    try
    {
        m_status_thread = getContainerServices()->getThreadManager()->create<SRTDerotatorStatusThread, SRTDerotatorImpl&>((m_component_name + "StatusThread").c_str(), *this);
        m_status_thread->setSleepTime(getCDBValue<double>(getContainerServices(), "status_thread_period") * 10000000);
        m_status_thread->resume();
    }
    catch(acsthreadErrType::CanNotSpawnThreadExImpl& impl)
    {
        // The thread failed to start for some reason
        _ADD_BACKTRACE(ComponentErrors::CanNotStartThreadExImpl, ex, impl, (m_component_name + "::startThread()").c_str());
        ex.setThreadName((m_component_name + "StatusThread").c_str());
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

void SRTDerotatorImpl::cleanUp()
{
    AUTO_TRACE(m_servo_name + "::cleanUp()");
}

void SRTDerotatorImpl::aboutToAbort()
{
    AUTO_TRACE(m_servo_name + "::aboutToAbort()");
}

/////////////////// PUBLIC methods
bool SRTDerotatorImpl::updateStatus()
{
    AUTO_TRACE(m_servo_name + "::status()");

    // Initialize the status variable
    long status = 0;

    // We don't check if the socket is connected here since a status command will try to reconnect it automatically
    try
    {
        m_socket.sendCommand(SRTMinorServoCommandLibrary::status(m_servo_name), m_status);

        ACS::Time last_timestamp = m_status.getTimestamp();

        // Send the zero offset if not correct
        if(m_status.getVirtualOffsets()[0] != m_zero_offset)
        {
            m_socket.sendCommand(SRTMinorServoCommandLibrary::offset(m_servo_name, { m_zero_offset }));
        }

        if(m_status.isBlocked() == Management::MNG_TRUE || m_status.getDriveCabinetStatus() == DRIVE_CABINET_ERROR)
        {
            // Set to failure
            status |= (1L << 1);
            // Not ready as well
            status |= (1L << 3);
        }

        if(!isReady())
        {
            // No failures but not ready
            status |= (1L << 3);
        }

        if(isSlewing())
        {
            status |= (1L << 4);
        }

        double current_point = m_status.getActualPosition();

        // Calculate the current speed of the axes, °/s
        try
        {
            std::pair<ACS::Time, const std::vector<double>> previous_point = m_positions_queue.get(last_timestamp);
            m_c_s = (current_point - previous_point.second[0]) * ((double(last_timestamp - previous_point.first)) / 10000000);
        }
        catch(...)
        {
            // Empty queue, first reading, skip the speed calculation
        }

        if(m_status.getOperativeMode() == OPERATIVE_MODE_PROGRAMTRACK)
        {
            try
            {
                m_commanded_position = m_tracking_queue.get(last_timestamp).second[0];
                m_remaining_trajectory_points.store(m_tracking_queue.getRemainingPoints(last_timestamp));
            }
            catch(...)
            {
                // Weird, we should have at least one tracking point by now, just skip
            }
        }

        m_positions_queue.put(last_timestamp, { current_point });
        m_position_difference.store(m_commanded_position - current_point);
        if(std::fabs(m_position_difference.load()) <= m_tracking_delta)
        {
            m_tracking.store(Management::MNG_TRUE);
        }
        else
        {
            m_tracking.store(Management::MNG_FALSE);
        }
    }
    catch(...)
    {
        // Communication error, sets failure, communication error and not ready bits
        status |= (1L << 1);
        status |= (1L << 2);
        status |= (1L << 3);

        m_status_pattern.store(status);
        return false;
    }

    m_status_pattern.store(status);
    return true;
}

void SRTDerotatorImpl::setup()
{
    AUTO_TRACE(m_servo_name + "::setup()");
}

void SRTDerotatorImpl::setPosition(CORBA::Double position)
{
    AUTO_TRACE(m_servo_name + "::setPosition()");

    checkLineStatus();

    if(position < m_min || position > m_max)
    {
        _EXCPT(DerotatorErrors::OutOfRangeErrorExImpl, ex, (m_servo_name + "::setPosition()").c_str());
        ex.addData("Reason", std::string("Resulting position " + std::to_string(position) + " out of range.").c_str());
        ex.log(LM_DEBUG);
        throw ex.getDerotatorErrorsEx();
    }

    // Sign inversion required
    double pos = position * -1;

    if(!m_socket.sendCommand(SRTMinorServoCommandLibrary::preset(m_servo_name, std::vector<double>{ pos })).checkOutput())
    {
        _EXCPT(DerotatorErrors::CommunicationErrorExImpl, ex, (m_servo_name + "::setPosition()").c_str());
        ex.addData("Reason", "Received NAK in response to a PRESET command.");
        ex.log(LM_DEBUG);
        throw ex.getDerotatorErrorsEx();
    }

    m_commanded_position.store(position);
}

double SRTDerotatorImpl::getPositionFromHistory(ACS::Time acs_time)
{
    AUTO_TRACE(m_servo_name + "::getPositionFromHistory()");

    // Get the latest position
    if(acs_time == 0)
    {
        acs_time = getTimeStamp();
    }

    try
    {
        return m_positions_queue.get(acs_time).second[0];
    }
    catch(std::logic_error& le)
    {
        // TODO: change this to ComponentErrors
        _EXCPT(ComponentErrors::OperationErrorExImpl, ex, (m_servo_name + "::getPositionFromHistory()").c_str());
        ex.setReason("Positions history is empty!");
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

bool SRTDerotatorImpl::isReady()
{
    // Always ready unless it has errors
    return (m_status.isBlocked() == Management::MNG_TRUE || m_status.getDriveCabinetStatus() == DRIVE_CABINET_ERROR) ? false : true;
}

bool SRTDerotatorImpl::isSlewing()
{
    SRTMinorServoOperativeMode operative_mode = m_status.getOperativeMode();
    if(operative_mode == OPERATIVE_MODE_PROGRAMTRACK)
    {
        if(m_remaining_trajectory_points.load() > 0)
        {
            // We are still tracking a trajectory, therefore we are slewing
            return true;
        }
        else
        {
            // Trajectory is finished, the derotator might still be slowing down to 0°/s but for simplicity we say it's not slewing anymore
            return false;
        }
    }
    else if(operative_mode == OPERATIVE_MODE_UNKNOWN)
    {
        // We trust the protocol, this means the derotator is moving due to a preset command
        return true;
    }

    return false;
}

void SRTDerotatorImpl::loadTrackingPoint(ACS::Time point_time, CORBA::Double position, CORBA::Boolean restart)
{
    AUTO_TRACE(m_servo_name + "::loadTrackingPoint()");

    checkLineStatus();

    if(position < m_min || position > m_max)
    {
        _EXCPT(DerotatorErrors::OutOfRangeErrorExImpl, ex, (m_servo_name + "::loadTrackingPoint()").c_str());
        ex.addData("Reason", std::string("Resulting position " + std::to_string(position) + " out of range.").c_str());
        ex.log(LM_DEBUG);
        throw ex.getDerotatorErrorsEx();
    }

    unsigned int trajectory_id, point_id;

    if(restart)
    {
        trajectory_id = (unsigned int)(IRA::CIRATools::ACSTime2UNIXTime(point_time));
        point_id = 0;
    }
    else
    {
        trajectory_id = m_trajectory_id.load();
        point_id = m_total_trajectory_points.load();
    }

    if(!m_socket.sendCommand(SRTMinorServoCommandLibrary::programTrack(m_servo_name, trajectory_id, point_id, std::vector<double>{ -position }, restart ? IRA::CIRATools::ACSTime2UNIXTime(point_time) : 0)).checkOutput())
    {
        _EXCPT(DerotatorErrors::CommunicationErrorExImpl, ex, (m_servo_name + "::loadTrackingPoint()").c_str());
        ex.addData("Reason", "Received NAK in response to a PROGRAMTRACK command.");
        ex.log(LM_DEBUG);
        throw ex.getDerotatorErrorsEx();
    }

    if(restart)
    {
        // Clear the tracking queue to avoid interpolation between 2 different trajectories
        m_tracking_queue.clear();
    }

    m_trajectory_id.store(trajectory_id);
    m_tracking_queue.put(point_time, { position });
    m_total_trajectory_points.store(point_id + 1);
}


/////////////////// PROTECTED methods
void SRTDerotatorImpl::checkLineStatus()
{
    if(!m_socket.isConnected())
    {
        _EXCPT(DerotatorErrors::CommunicationErrorExImpl, ex, (m_servo_name + "checkLineStatus()").c_str());
        ex.addData("Reason", "Socket not connected.");
        ex.log(LM_DEBUG);
        throw ex.getDerotatorErrorsEx();
    }

    if(m_status.isBlocked() == Management::MNG_TRUE || m_status.getDriveCabinetStatus() == DRIVE_CABINET_ERROR)
    {
        _EXCPT(DerotatorErrors::UnexpectedErrorExImpl, ex, (m_servo_name + "::checkLineStatus()").c_str());
        ex.addData("Reason", "Servo system blocked or drive cabinet error.");
        ex.log(LM_DEBUG);
        throw ex.getDerotatorErrorsEx();
    }
}

/////////////////// PRIVATE methods
std::vector<std::string> SRTDerotatorImpl::getPropertiesTable(SRTDerotatorImpl& object, const std::string& properties_name)
{
    AUTO_STATIC_TRACE(object.m_servo_name + "::getPropertiesTable()");

    std::vector<std::string> properties;
    
    IRA::CDBTable table(object.getContainerServices(), properties_name.c_str(), std::string("DataBlock/MinorServo/" + object.m_servo_name).c_str());
    IRA::CError error;
    error.Reset();

    if(!table.addField(error, "property_name", IRA::CDataField::STRING))
    {
        error.setExtra("Error adding field property_name", 0);
    }
    if(!error.isNoError())
    {
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl, ex, error);
        ex.setCode(error.getErrorCode());
        ex.setDescription((const char *)error.getDescription());
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
    if(!table.openTable(error))
    {
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, ex, error);
        ex.setFieldName(properties_name.c_str());
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }

    table.First();
    for(unsigned int i = 0; i < table.recordCount(); i++, table.Next())
    {
        properties.push_back(std::string(table["property_name"]->asString()));
    }
    table.closeTable();

    size_t expected_size = 0;

    if(properties_name.find("virtual_") == 0)
    {
        expected_size = object.m_virtual_axes;
    }
    else if(properties_name.find("physical_") == 0)
    {
        expected_size = object.m_physical_axes;
    }

    if(expected_size == 0 || properties.size() != expected_size)
    {
        _EXCPT(ComponentErrors::CDBAccessExImpl, ex, (object.m_servo_name + "::initialize()").c_str());
        ex.setFieldName(properties_name.c_str());
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }

    return properties;
}


GET_PROPERTY_REFERENCE(Management::ROTBoolean, SRTDerotatorImpl, m_enabled_ptr, enabled);
GET_PROPERTY_REFERENCE(ROSRTMinorServoCabinetStatus, SRTDerotatorImpl, m_drive_cabinet_status_ptr, drive_cabinet_status);
GET_PROPERTY_REFERENCE(Management::ROTBoolean, SRTDerotatorImpl, m_block_ptr, block);
GET_PROPERTY_REFERENCE(ROSRTMinorServoOperativeMode, SRTDerotatorImpl, m_operative_mode_ptr, operative_mode);
GET_PROPERTY_REFERENCE(ACS::RObooleanSeq, SRTDerotatorImpl, m_physical_axes_enabled_ptr, physical_axes_enabled);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq, SRTDerotatorImpl, m_physical_positions_ptr, physical_positions);
GET_PROPERTY_REFERENCE(ACS::ROlong, SRTDerotatorImpl, m_virtual_axes_ptr, virtual_axes);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq, SRTDerotatorImpl, m_plain_virtual_positions_ptr, plain_virtual_positions);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq, SRTDerotatorImpl, m_virtual_positions_ptr, virtual_positions);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq, SRTDerotatorImpl, m_virtual_offsets_ptr, virtual_offsets);
GET_PROPERTY_REFERENCE(Management::ROTBoolean, SRTDerotatorImpl, m_tracking_ptr, tracking);
GET_PROPERTY_REFERENCE(ACS::ROlong, SRTDerotatorImpl, m_trajectory_id_ptr, trajectory_id);
GET_PROPERTY_REFERENCE(ACS::ROlong, SRTDerotatorImpl, m_total_trajectory_points_ptr, total_trajectory_points);
GET_PROPERTY_REFERENCE(ACS::ROlong, SRTDerotatorImpl, m_remaining_trajectory_points_ptr, remaining_trajectory_points);
GET_PROPERTY_REFERENCE(ACS::ROdouble, SRTDerotatorImpl, m_actual_position_ptr, actPosition);
GET_PROPERTY_REFERENCE(ACS::RWdouble, SRTDerotatorImpl, m_commanded_position_ptr, cmdPosition);
GET_PROPERTY_REFERENCE(ACS::ROdouble, SRTDerotatorImpl, m_position_difference_ptr, positionDiff);
GET_PROPERTY_REFERENCE(ACS::ROpattern, SRTDerotatorImpl, m_status_ptr, status);

MACI_DLL_SUPPORT_FUNCTIONS(SRTDerotatorImpl)
