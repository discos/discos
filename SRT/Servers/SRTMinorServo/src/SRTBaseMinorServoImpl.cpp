#include "SRTMinorServoImpl.h"

using namespace MinorServo;

SRTBaseMinorServoImpl::SRTBaseMinorServoImpl(const ACE_CString& component_name, maci::ContainerServices* container_services):
    CharacteristicComponentImpl(component_name, container_services),
    m_component_name(std::string(component_name.c_str())),
    m_servo_name(std::string(strchr(component_name.c_str(), '/') + 1)),
    m_status(),
    m_virtual_axes(getCDBValue<size_t>(container_services, "virtual_axes")),
    m_physical_axes(getCDBValue<size_t>(container_services, "physical_axes")),
    m_virtual_axes_names(SRTBaseMinorServoImpl::getPropertiesTable(*this, "virtual_positions")),
    m_virtual_axes_units(SRTBaseMinorServoImpl::getPropertiesTable(*this, "virtual_axes_units")),
    m_user_offsets(m_virtual_axes, 0.0),
    m_system_offsets(m_virtual_axes, 0.0),
    m_positions_queue(5 * 60 * int(1 / getCDBValue<double>(container_services, "status_thread_period", "/MINORSERVO/Boss")), m_virtual_axes),
    m_min(SRTBaseMinorServoImpl::getMotionConstant(*this, "min_range")),
    m_max(SRTBaseMinorServoImpl::getMotionConstant(*this, "max_range")),
    m_m_s(SRTBaseMinorServoImpl::getMotionConstant(*this, "max_speed")),
    m_a(SRTBaseMinorServoImpl::getMotionConstant(*this, "acceleration")),
    m_r_t(SRTBaseMinorServoImpl::getMotionConstant(*this, "ramp_times")),
    m_r_d(SRTBaseMinorServoImpl::getMotionConstant(*this, "ramp_distances")),
    m_c_s(m_virtual_axes, 0.0),
    m_in_use(Management::MNG_FALSE),
    m_current_setup(""),
    m_enabled_ptr(this),
    m_drive_cabinet_status_devio(nullptr),
    m_drive_cabinet_status_ptr(this),
    m_block_devio(nullptr),
    m_block_ptr(this),
    m_operative_mode_ptr(this),
    m_physical_axes_enabled_ptr(this),
    m_physical_positions_ptr(this),
    m_virtual_axes_ptr(this),
    m_plain_virtual_positions_ptr(this),
    m_virtual_positions_devio(nullptr),
    m_virtual_positions_ptr(this),
    m_virtual_offsets_devio(nullptr),
    m_virtual_offsets_ptr(this),
    m_virtual_user_offsets_ptr(this),
    m_virtual_system_offsets_ptr(this),
    m_in_use_ptr(this),
    m_current_setup_ptr(this),
    m_current_lookup_table(),
    m_socket_configuration(SRTMinorServoSocketConfiguration::getInstance(container_services)),
    m_socket(SRTMinorServoSocket::getInstance(m_socket_configuration.m_ip_address, m_socket_configuration.m_port, m_socket_configuration.m_timeout))
{
    AUTO_TRACE(m_servo_name + "::SRTBaseMinorServoImpl()");
}

SRTBaseMinorServoImpl::~SRTBaseMinorServoImpl()
{
    AUTO_TRACE(m_servo_name + "::~SRTBaseMinorServoImpl()");
}

void SRTBaseMinorServoImpl::initialize()
{
    AUTO_TRACE(m_servo_name + "::initialize()");

    try
    {
        m_enabled_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((m_component_name + ":enabled").c_str(), getComponent(),
                new MSAnswerMapDevIO<Management::TBoolean>(m_servo_name, "enabled", "ENABLED", m_status), true);
        m_drive_cabinet_status_devio = new MSAnswerMapDevIO<SRTMinorServoCabinetStatus>(m_servo_name, "drive_cabinet_status", "STATUS", m_status);
        m_drive_cabinet_status_ptr = new ROEnumImpl<ACS_ENUM_T(SRTMinorServoCabinetStatus), POA_MinorServo::ROSRTMinorServoCabinetStatus>((m_component_name + ":drive_cabinet_status").c_str(), getComponent(), m_drive_cabinet_status_devio, true);
        m_block_devio = new MSAnswerMapDevIO<Management::TBoolean>(m_servo_name, "block", "BLOCK", m_status);
        m_block_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((m_component_name + ":block").c_str(), getComponent(), m_block_devio, true);
        m_operative_mode_ptr = new ROEnumImpl<ACS_ENUM_T(SRTMinorServoOperativeMode), POA_MinorServo::ROSRTMinorServoOperativeMode>((m_component_name + ":operative_mode").c_str(), getComponent(),
                new MSAnswerMapDevIO<SRTMinorServoOperativeMode>(m_servo_name, "operative_mode", "OPERATIVE_MODE", m_status), true);
        m_physical_axes_enabled_ptr = new baci::RObooleanSeq((m_component_name + ":physical_axes_enabled").c_str(), getComponent(),
                new MSAnswerMapDevIO<ACS::booleanSeq>(m_servo_name, "physical_axes_enabled", SRTBaseMinorServoImpl::getPropertiesTable(*this, "physical_axes_enabled"), m_status), true);
        m_physical_positions_ptr = new baci::ROdoubleSeq((m_component_name + ":physical_positions").c_str(), getComponent(),
                new MSAnswerMapDevIO<ACS::doubleSeq>(m_servo_name, "physical_positions", SRTBaseMinorServoImpl::getPropertiesTable(*this, "physical_positions"), m_status), true);
        m_virtual_axes_ptr = new baci::ROlong((m_component_name + ":virtual_axes").c_str(), getComponent(),
                new MSGenericDevIO<CORBA::Long, const size_t>(m_virtual_axes), true);
        m_plain_virtual_positions_ptr = new baci::ROdoubleSeq((m_component_name + ":plain_virtual_positions").c_str(), getComponent(),
                new MSAnswerMapDevIO<ACS::doubleSeq>(m_servo_name, "virtual_positions", m_virtual_axes_names, m_status), true);
        std::vector<std::string> virtual_offsets = SRTBaseMinorServoImpl::getPropertiesTable(*this, "virtual_offsets");
        m_virtual_positions_devio = new MSVirtualPositionsDevIO(m_servo_name, m_virtual_axes_names, virtual_offsets, m_status);
        m_virtual_positions_ptr = new baci::ROdoubleSeq((m_component_name + ":virtual_positions").c_str(), getComponent(), m_virtual_positions_devio, true);
        m_virtual_offsets_devio = new MSAnswerMapDevIO<ACS::doubleSeq>(m_servo_name, "virtual_offsets", virtual_offsets, m_status);
        m_virtual_offsets_ptr = new baci::ROdoubleSeq((m_component_name + ":virtual_offsets").c_str(), getComponent(), m_virtual_offsets_devio, true);
        m_virtual_user_offsets_ptr = new baci::ROdoubleSeq((m_component_name + ":virtual_user_offsets").c_str(), getComponent(),
                new MSGenericDevIO<ACS::doubleSeq, std::vector<double>>(m_user_offsets), true);
        m_virtual_system_offsets_ptr = new baci::ROdoubleSeq((m_component_name + ":virtual_system_offsets").c_str(), getComponent(),
                new MSGenericDevIO<ACS::doubleSeq, std::vector<double>>(m_system_offsets), true);
        m_in_use_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((m_component_name + ":in_use").c_str(), getComponent(),
                new MSGenericDevIO<Management::TBoolean, std::atomic<Management::TBoolean>>(m_in_use), true);
        m_current_setup_ptr = new baci::ROstring((m_component_name + ":current_setup").c_str(), getComponent(),
                new MSGenericDevIO<ACE_CString, std::string>(m_current_setup), true);
    }
    catch(std::bad_alloc& ba)
    {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, ex, (m_servo_name + "::initialize()").c_str());
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }

    // Try to read the current status of the servo
    try
    {
        status();
    }
    catch(...)
    {
        // This block is necessary since the socket might not be connected yet. If the Leonardo system is not reachable the status(); call will fail, but we want to instantiate the component anyway
        // A non connected socket will try to connect every time a new command is sent, therefore the status thread will establish the connection as soon as possible.
    }
}

void SRTBaseMinorServoImpl::execute()
{
    AUTO_TRACE(m_servo_name + "::execute()");
}

void SRTBaseMinorServoImpl::cleanUp()
{
    AUTO_TRACE(m_servo_name + "::cleanUp()");
}

void SRTBaseMinorServoImpl::aboutToAbort()
{
    AUTO_TRACE(m_servo_name + "::aboutToAbort()");
}

/////////////////// PUBLIC methods
bool SRTBaseMinorServoImpl::status()
{
    AUTO_TRACE(m_servo_name + "::status()");

    // We don't check if the socket is connected here since a status command will try to reconnect it automatically
    try
    {
        m_socket.sendCommand(SRTMinorServoCommandLibrary::status(m_servo_name), m_status);

        ACS::Time comp;
        ACS::doubleSeq current_point = m_virtual_positions_devio->read(comp);

        // Calculate the current speed of the axes
        try
        {
            std::pair<ACS::Time, const std::vector<double>> previous_point = m_positions_queue.get(m_status.getTimestamp());
            for(size_t i = 0; i < m_virtual_axes; i++)
            {
                m_c_s[i] = (current_point[i] - previous_point.second[i]) * ((double(m_status.getTimestamp() - previous_point.first)) / 10000000);
            }
        }
        catch(...)
        {
            // Empty queue, first reading, skip the speed calculation
        }

        m_positions_queue.put(m_status.getTimestamp(), current_point);
    }
    catch(...)
    {
        return false;
    }

    return true;
}

void SRTBaseMinorServoImpl::stow(CORBA::Long stow_position)
{
    AUTO_TRACE(m_servo_name + "::stow()");

    checkLineStatus();

    if(!m_socket.sendCommand(SRTMinorServoCommandLibrary::stow(m_servo_name, (unsigned int)stow_position)).checkOutput())
    {
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, ex, (m_servo_name + "::stow()").c_str());
        ex.setReason("Received NAK in response to a STOW command.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }
}

void SRTBaseMinorServoImpl::stop()
{
    AUTO_TRACE(m_servo_name + "::stop()");

    checkLineStatus();

    if(!m_socket.sendCommand(SRTMinorServoCommandLibrary::stop(m_servo_name)).checkOutput())
    {
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, ex, (m_servo_name + "::stop()").c_str());
        ex.setReason("Received NAK in response to a STOP command.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }
}

void SRTBaseMinorServoImpl::preset(const ACS::doubleSeq& virtual_coordinates)
{
    AUTO_TRACE(m_servo_name + "::preset()");

    checkLineStatus();

    if(virtual_coordinates.length() != m_virtual_axes)
    {
        _EXCPT(MinorServoErrors::PositioningErrorExImpl, ex, (m_servo_name + "::preset()").c_str());
        ex.addData("Reason", "Wrong number of values for this servo system.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    std::vector<double> coordinates(virtual_coordinates.get_buffer(), virtual_coordinates.get_buffer() + virtual_coordinates.length());
    ACS::Time comp;
    ACS::doubleSeq offsets = m_virtual_offsets_devio->read(comp);

    for(size_t i = 0; i < m_virtual_axes; i++)
    {
        double coordinate = coordinates[i] + offsets[i];
        if(coordinate < m_min[i] || coordinate > m_max[i])
        {
            _EXCPT(MinorServoErrors::PositioningErrorExImpl, ex, (m_servo_name + "::preset()").c_str());
            ex.addData("Reason", "Resulting position out of range, check the offsets.");
            ex.log(LM_DEBUG);
            throw ex.getMinorServoErrorsEx();
        }
    }

    if(!m_socket.sendCommand(SRTMinorServoCommandLibrary::preset(m_servo_name, coordinates)).checkOutput())
    {
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, ex, (m_servo_name + "::preset()").c_str());
        ex.setReason("Received NAK in response to a PRESET command.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }
}

bool SRTBaseMinorServoImpl::setup(const char* configuration_name)
{
    AUTO_TRACE(m_servo_name + "::setup()");
    m_in_use.store(Management::MNG_FALSE);
    m_current_lookup_table.clear();

    m_current_setup = "";
    std::string setup_name(configuration_name);
    std::transform(setup_name.begin(), setup_name.end(), setup_name.begin(), ::toupper);

    if(setup_name.empty())
    {
        return false;
    }

    IRA::CDBTable table(getContainerServices(), setup_name.c_str(), std::string("DataBlock/MinorServo/" + m_servo_name).c_str());
    IRA::CError error;
    error.Reset();

    if(!table.addField(error, "axis", IRA::CDataField::STRING))
    {
        error.setExtra("Error adding field axis", 0);
    }
    if(!table.addField(error, "coefficients", IRA::CDataField::STRING))
    {
        error.setExtra("Error adding field coefficients", 0);
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
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }

    table.First();
    for(unsigned int i = 0; i < table.recordCount(); i++, table.Next())
    {
        std::string axis = std::string(table["axis"]->asString());

        std::vector<double> coefficients;

        std::string coefficient_str;
        std::stringstream stream(std::string(table["coefficients"]->asString()));

        while(std::getline(stream, coefficient_str, ','))
        {
            coefficients.push_back(std::stod(std::regex_replace(coefficient_str, std::regex("\\s+"), "")));
        }

        m_current_lookup_table[axis] = coefficients;
    }
    table.closeTable();

    if(m_current_lookup_table.size() > 0)
    {
        m_current_setup = setup_name;
        clearUserOffsets();
        clearSystemOffsets();
        m_in_use.store(Management::MNG_TRUE);
    }

    return true;
}

ACS::doubleSeq* SRTBaseMinorServoImpl::calcCoordinates(double elevation)
{
    AUTO_TRACE(m_servo_name + "::calcCoordinates()");

    if(m_in_use.load() == Management::MNG_TRUE)
    {
        ACS::doubleSeq_var coordinates = new ACS::doubleSeq;
        coordinates->length(m_virtual_axes);

        for(size_t axis = 0; axis < m_virtual_axes; axis++)
        {
            std::vector<double> coefficients = m_current_lookup_table.at(m_virtual_axes_names[axis]);

            double coordinate = 0;

            for(size_t index = 0; index < coefficients.size(); index++)
            {
                coordinate += coefficients[index] * pow(elevation, index);
            }

            coordinates[axis] = coordinate;
        }

        return coordinates._retn();
    }
    else
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, (m_servo_name + "::calcCoordinates()").c_str());
        ex.setReason("Unable to calculate the coordinates since the servo system has not been configured yet.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }
}

ACS::doubleSeq* SRTBaseMinorServoImpl::getUserOffsets()
{
    AUTO_TRACE(m_servo_name + "::getUserOffsets()");

    ACS::doubleSeq_var offsets = new ACS::doubleSeq;
    offsets->length(m_user_offsets.size());
    std::copy(m_user_offsets.begin(), m_user_offsets.end(), offsets->begin());
    return offsets._retn();
}

void SRTBaseMinorServoImpl::setUserOffset(const char* axis_name, CORBA::Double offset)
{
    AUTO_TRACE(m_servo_name + "::setUserOffset()");

    checkLineStatus();

    std::string axis(axis_name);
    std::transform(axis.begin(), axis.end(), axis.begin(), ::toupper);

    if(axis == "RZ" && m_virtual_axes == 1)
    {
        axis = "ROTATION";
    }

    unsigned int index = std::distance(m_virtual_axes_names.begin(), std::find(m_virtual_axes_names.begin(), m_virtual_axes_names.end(), axis));

    if(index == m_virtual_axes)
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, ex, (m_servo_name + "::setUserOffset()").c_str());
        ex.addData("Reason", ("Unknown axis '" + axis + "'.").c_str());
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    // Calculate the new offsets
    std::vector<double> offsets(m_virtual_axes, 0);
    std::transform(m_user_offsets.begin(), m_user_offsets.end(), m_system_offsets.begin(), offsets.begin(), std::plus<double>());
    offsets[index] = m_system_offsets[index] + offset;

    if(offsets[index] < m_min[index] || offsets[index] > m_max[index])
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, ex, (m_servo_name + "::setUserOffset()").c_str());
        ex.addData("Reason", "Sum of user and system offsets out of range.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    if(!m_socket.sendCommand(SRTMinorServoCommandLibrary::offset(m_servo_name, offsets)).checkOutput())
    {
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, ex, (m_servo_name + "::setUserOffset()").c_str());
        ex.setReason("Received NAK in response to an OFFSET command.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    m_user_offsets[index] = offset;
}

void SRTBaseMinorServoImpl::clearUserOffsets()
{
    AUTO_TRACE(m_servo_name + "::clearUserOffsets()");

    checkLineStatus();

    m_user_offsets = std::vector<double>(m_virtual_axes, 0.0);

    std::vector<double> offsets(m_virtual_axes, 0);
    std::transform(m_user_offsets.begin(), m_user_offsets.end(), m_system_offsets.begin(), offsets.begin(), std::plus<double>());

    if(!m_socket.sendCommand(SRTMinorServoCommandLibrary::offset(m_servo_name, offsets)).checkOutput())
    {
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, ex, (m_servo_name + "::clearUserOffset()").c_str());
        ex.setReason("Received NAK in response to an OFFSET command.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }
}

ACS::doubleSeq* SRTBaseMinorServoImpl::getSystemOffsets()
{
    AUTO_TRACE(m_servo_name + "::getSystemOffsets()");

    ACS::doubleSeq_var offsets = new ACS::doubleSeq;
    offsets->length(m_system_offsets.size());
    std::copy(m_system_offsets.begin(), m_system_offsets.end(), offsets->begin());
    return offsets._retn();
}

void SRTBaseMinorServoImpl::setSystemOffset(const char* axis_name, CORBA::Double offset)
{
    AUTO_TRACE(m_servo_name + "::setSystemOffset()");

    checkLineStatus();

    std::string axis(axis_name);
    std::transform(axis.begin(), axis.end(), axis.begin(), ::toupper);

    if(axis == "RZ" && m_virtual_axes == 1)
    {
        axis = "ROTATION";
    }

    unsigned int index = std::distance(m_virtual_axes_names.begin(), std::find(m_virtual_axes_names.begin(), m_virtual_axes_names.end(), axis));

    if(index == m_virtual_axes)
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, ex, (m_servo_name + "::setSystemOffset()").c_str());
        ex.addData("Reason", ("Unknown axis '" + axis + "'.").c_str());
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    // Calculate the new offsets
    std::vector<double> offsets(m_virtual_axes, 0);
    std::transform(m_user_offsets.begin(), m_user_offsets.end(), m_system_offsets.begin(), offsets.begin(), std::plus<double>());
    offsets[index] = m_user_offsets[index] + offset;

    if(offsets[index] < m_min[index] || offsets[index] > m_max[index])
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, ex, (m_servo_name + "::setUserOffset()").c_str());
        ex.addData("Reason", "Sum of user and system offsets out of range.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    if(!m_socket.sendCommand(SRTMinorServoCommandLibrary::offset(m_servo_name, offsets)).checkOutput())
    {
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, ex, (m_servo_name + "::setSystemOffset()").c_str());
        ex.setReason("Received NAK in response to an OFFSET command.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    m_system_offsets[index] = offset;
}

void SRTBaseMinorServoImpl::clearSystemOffsets()
{
    AUTO_TRACE(m_servo_name + "::clearSystemOffsets()");

    checkLineStatus();

    m_system_offsets = std::vector<double>(m_virtual_axes, 0.0);

    // Update the offsets
    std::vector<double> offsets(m_virtual_axes, 0);
    std::transform(m_user_offsets.begin(), m_user_offsets.end(), m_system_offsets.begin(), offsets.begin(), std::plus<double>());

    if(!m_socket.sendCommand(SRTMinorServoCommandLibrary::offset(m_servo_name, offsets)).checkOutput())
    {
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, ex, (m_servo_name + "::clearSystemOffset()").c_str());
        ex.setReason("Received NAK in response to an OFFSET command.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }
}

void SRTBaseMinorServoImpl::reloadOffsets()
{
    AUTO_TRACE(m_servo_name + "::reloadOffsets()");

    // Sum the user and system DISCOS offsets to check whether they correspond to the Leonardo offsets
    std::vector<double> DISCOS_offsets(m_virtual_axes, 0.0);
    std::transform(m_user_offsets.begin(), m_user_offsets.end(), m_system_offsets.begin(), DISCOS_offsets.begin(), std::plus<double>());

    // Read the Leonardo offsets
    ACS::Time comp;
    ACS::doubleSeq sequence = m_virtual_offsets_devio->read(comp);
    std::vector<double> LEONARDO_offsets(sequence.get_buffer(), sequence.get_buffer() + sequence.length());

    // Check if the offsets correspond or not
    if(!std::equal(DISCOS_offsets.begin(), DISCOS_offsets.end(), LEONARDO_offsets.begin()))
    {
        // Offsets do not correspond, should reset them by sending a offset command
        if(!m_socket.sendCommand(SRTMinorServoCommandLibrary::offset(m_servo_name, DISCOS_offsets)).checkOutput())
        {
            _EXCPT(MinorServoErrors::CommunicationErrorExImpl, ex, (m_servo_name + "::reloadOffsets()").c_str());
            ex.setReason("Received NAK in response to an OFFSET command.");
            ex.log(LM_DEBUG);
            throw ex.getMinorServoErrorsEx();
        }

        ACS_LOG(LM_FULL_INFO, m_servo_name + "::reloadOffsets()", (LM_NOTICE, "Offsets discrepancy, reloading them"));
    }
}

void SRTBaseMinorServoImpl::getAxesInfo(ACS::stringSeq_out axes_names_out, ACS::stringSeq_out axes_units_out)
{
    AUTO_TRACE("SRTBaseMinorServoImpl::getAxesInfo()");

    ACS::stringSeq_var axes_names = new ACS::stringSeq;
    ACS::stringSeq_var axes_units = new ACS::stringSeq;
    axes_names->length(m_virtual_axes);
    axes_units->length(m_virtual_axes);

    for(size_t i = 0; i < m_virtual_axes; i++)
    {
        axes_names[i] = (m_servo_name + "_" + m_virtual_axes_names[i] == "ROTATION" ? "RZ" : m_virtual_axes_names[i]).c_str();
        axes_units[i] = m_virtual_axes_units[i].c_str();
    }

    axes_names_out = axes_names._retn();
    axes_units_out = axes_units._retn();
}

ACS::doubleSeq* SRTBaseMinorServoImpl::getAxesPositions(ACS::Time acs_time)
{
    AUTO_TRACE("SRTBaseMinorServoImpl::getAxesPositions()");

    // Get the latest position
    if(acs_time == 0)
    {
        acs_time = getTimeStamp();
    }

    try
    {
        std::vector<double> p = m_positions_queue.get(acs_time).second;

        ACS::doubleSeq_var positions = new ACS::doubleSeq;
        positions->length(m_virtual_axes);
        std::copy(p.begin(), p.end(), positions->begin());
        return positions._retn();
    }
    catch(std::logic_error& le)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, (m_servo_name + "::getAxesPositions()").c_str());
        ex.setReason("Positions history is empty!");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }
}

ACS::TimeInterval SRTBaseMinorServoImpl::getTravelTime(const ACS::doubleSeq& _s_p, const ACS::doubleSeq& d_p)
{
    AUTO_TRACE(m_servo_name + "::getTravelTime()");

    std::vector<double> c_s = m_c_s;  // Current speed

    ACS::doubleSeq s_p(_s_p);

    // No starting coordinates, it means we have to start from the current position taking into account the current speed
    if(_s_p.length() == 0)
    {
        s_p = *getAxesPositions(0);
    }
    else if(_s_p.length() != m_virtual_axes)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, (m_servo_name + "getTravelTime()").c_str());
        ex.setReason("Wrong number of axes for starting_position.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }
    if(d_p.length() != m_virtual_axes)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, (m_servo_name + "getTravelTime()").c_str());
        ex.setReason("Wrong number of axes for destination_position.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    // d = delta/distance
    std::vector<double> d(m_virtual_axes);
    for(size_t i = 0; i < m_virtual_axes; i++)
    {
        d[i] = d_p[i] - s_p[i];

        // If we sent a starting position, we are just checking the maximum time to get from the start to the desired position
        // We take into account the worst case scenario for the speed, we assume we are moving away from the desired position at maximum speed for each axis
        if(_s_p.length() != 0)
        {
            c_s[i] = m_m_s[i] * (d[i] < 0 ? 1 : (d[i] > 0 ? -1 : 0));
        }
    }

    double total_time = 0;

    // Calculate the distance and time taken to get to the maximum speed towards the desired position
    for(size_t i = 0; i < m_virtual_axes; i++)
    {
        double inversion_time = 0;
        double inversion_distance = 0;

        // We are moving away from our desired position, this is the only case in which we need a deceleration ramp before even starting to move towards our destination
        if(std::signbit(c_s[i]) != std::signbit(d[i]) && c_s[i] != 0)
        {
            inversion_time = std::abs(c_s[i]) / m_a[i];
            inversion_distance = std::abs(c_s[i]) * inversion_time + 0.5 * m_a[i] * std::pow(inversion_time, 2);
            // In this case, we can calculate the next acceleration ramp using a starting speed of 0
            c_s = std::vector<double>(c_s.size(), 0.0);
        }

        double accel_ramp_time = (m_m_s[i] - std::abs(c_s[i])) / m_a[i];
        double accel_ramp_distance = std::abs(c_s[i]) * accel_ramp_time + 0.5 * m_a[i] * std::pow(accel_ramp_time, 2);

        // Total time = eventual inversion time + calculated acceleration ramp time + full deceleration time + linear movement time
        // Linear movement time = linear movement distance / maximum speed
        // Linear movement distance = distance + eventual inversion distance - acceleration ramp distance - full deceleration ramp distance
        double t = inversion_time + accel_ramp_time + m_r_t[i] + (std::abs(d[i]) + inversion_distance - accel_ramp_distance - m_r_d[i]) / m_m_s[i];

        total_time = std::max(total_time, t);

        // This does not take into account any dead time but:
        // we're going to use this only for PROGRAMTRACK purposes and
        // in PROGRAMTRACK mode the SRP servos move faster than the nominal max_speed
        // i.e., IIRC, the max physical speed should be around 12mm/s instead of 4mm/s
        // Therefore we might not need to add any guard time
    }

    return ACS::TimeInterval(total_time * 10000000);
}

void SRTBaseMinorServoImpl::getAxesRanges(ACS::doubleSeq_out min_ranges_out, ACS::doubleSeq_out max_ranges_out)
{
    AUTO_TRACE("SRTBaseMinorServoImpl::getAxesRanges()");

    ACS::doubleSeq_var min_ranges = new ACS::doubleSeq;
    ACS::doubleSeq_var max_ranges = new ACS::doubleSeq;
    min_ranges->length(m_virtual_axes);
    max_ranges->length(m_virtual_axes);
    std::copy(m_min.begin(), m_min.end(), min_ranges->begin());
    std::copy(m_max.begin(), m_max.end(), max_ranges->begin());
    min_ranges_out = min_ranges._retn();
    max_ranges_out = max_ranges._retn();
}

/////////////////// PROTECTED methods
void SRTBaseMinorServoImpl::checkLineStatus()
{
    if(!m_socket.isConnected())
    {
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, ex, (m_servo_name + "checkLineStatus()").c_str());
        ex.setReason("Socket not connected.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    ACS::Time comp;
    if(m_block_devio->read(comp) == Management::MNG_TRUE || m_drive_cabinet_status_devio->read(comp) == DRIVE_CABINET_ERROR)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, ex, (m_servo_name + "::checkLineStatus()").c_str());
        ex.setReason("Servo system blocked or drive cabinet error.");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }
}

std::vector<double> SRTBaseMinorServoImpl::getMotionConstant(SRTBaseMinorServoImpl& object, const std::string& constant)
{
    AUTO_STATIC_TRACE(object.m_servo_name + "::getMotionConstant()");

    std::vector<double> values;

    if(constant == "max_speed" || constant == "acceleration")
    {
        values = getCDBValue<std::vector<double>>(object.getContainerServices(), constant.c_str());
        if(values.size() != object.m_virtual_axes)
        {
            _EXCPT(ComponentErrors::CDBAccessExImpl, ex, (object.m_servo_name + "::getMotionConstant()").c_str());
            ex.setFieldName(constant.c_str());
            ex.log(LM_DEBUG);
            throw ex.getComponentErrorsEx();
        }
        else if(std::any_of(values.begin(), values.end(), [](double value)
        {
            return value == 0.0;
        }))
        {
            _EXCPT(ComponentErrors::NotAllowedExImpl, ex, (object.m_servo_name + "::getMotionConstant()").c_str());
            ex.setReason(("A" + constant == "acceleration" ? "n " : " " + constant + " equals to 0 is not allowed.").c_str());
            ex.log(LM_DEBUG);
            throw ex.getComponentErrorsEx();
        }
    }
    else if(constant == "min_range" || constant == "max_range" || constant == "tracking_delta")
    {
        values = getCDBValue<std::vector<double>>(object.getContainerServices(), constant.c_str());
        if(values.size() != object.m_virtual_axes)
        {
            _EXCPT(ComponentErrors::CDBAccessExImpl, ex, (object.m_servo_name + "::getMotionConstant()").c_str());
            ex.setFieldName(constant.c_str());
            ex.log(LM_DEBUG);
            throw ex.getComponentErrorsEx();
        }
    }
    else if(constant == "ramp_times")       // Acceleration ramp times, 0 to max_speed and vice versa
    {
        values = std::vector<double>(object.m_virtual_axes);
        std::transform(object.m_m_s.begin(), object.m_m_s.end(), object.m_a.begin(), values.begin(), std::divides<double>());
    }
    else if(constant == "ramp_distances")   // Acceleration ramp distances, 0 to max_speed and vice versa
    {
        values = std::vector<double>(object.m_virtual_axes);
        std::transform(object.m_r_t.begin(), object.m_r_t.end(), object.m_a.begin(), values.begin(), [](double acceleration_ramp_time, double acceleration)
        {
            return 0.5 * acceleration * std::pow(acceleration_ramp_time, 2);
        });
    }

    return values;
}

/////////////////// PRIVATE methods
std::vector<std::string> SRTBaseMinorServoImpl::getPropertiesTable(SRTBaseMinorServoImpl& object, const std::string& properties_name)
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


GET_PROPERTY_REFERENCE(Management::ROTBoolean, SRTBaseMinorServoImpl, m_enabled_ptr, enabled);
GET_PROPERTY_REFERENCE(ROSRTMinorServoCabinetStatus, SRTBaseMinorServoImpl, m_drive_cabinet_status_ptr, drive_cabinet_status);
GET_PROPERTY_REFERENCE(Management::ROTBoolean, SRTBaseMinorServoImpl, m_block_ptr, block);
GET_PROPERTY_REFERENCE(ROSRTMinorServoOperativeMode, SRTBaseMinorServoImpl, m_operative_mode_ptr, operative_mode);
GET_PROPERTY_REFERENCE(ACS::RObooleanSeq, SRTBaseMinorServoImpl, m_physical_axes_enabled_ptr, physical_axes_enabled);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq, SRTBaseMinorServoImpl, m_physical_positions_ptr, physical_positions);
GET_PROPERTY_REFERENCE(ACS::ROlong, SRTBaseMinorServoImpl, m_virtual_axes_ptr, virtual_axes);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq, SRTBaseMinorServoImpl, m_plain_virtual_positions_ptr, plain_virtual_positions);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq, SRTBaseMinorServoImpl, m_virtual_positions_ptr, virtual_positions);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq, SRTBaseMinorServoImpl, m_virtual_offsets_ptr, virtual_offsets);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq, SRTBaseMinorServoImpl, m_virtual_user_offsets_ptr, virtual_user_offsets);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq, SRTBaseMinorServoImpl, m_virtual_system_offsets_ptr, virtual_system_offsets);
GET_PROPERTY_REFERENCE(Management::ROTBoolean, SRTBaseMinorServoImpl, m_in_use_ptr, in_use);
GET_PROPERTY_REFERENCE(ACS::ROstring, SRTBaseMinorServoImpl, m_current_setup_ptr, current_setup);
