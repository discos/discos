#include "SRTMinorServoImpl.h"

using namespace maci;

SRTBaseMinorServoImpl::SRTBaseMinorServoImpl(const ACE_CString &componentName, maci::ContainerServices *containerServices) :
    CharacteristicComponentImpl(componentName, containerServices),
    m_enabled_ptr(this),
    m_drive_cabinet_status_ptr(this),
    m_block_ptr(this),
    m_operative_mode_ptr(this),
    m_physical_axes_enabled_ptr(this),
    m_physical_positions_ptr(this),
    m_virtual_axes_ptr(this),
    m_virtual_positions_ptr(this),
    m_virtual_offsets_ptr(this),
    m_virtual_user_offsets_ptr(this),
    m_virtual_system_offsets_ptr(this),
    m_in_use_ptr(this),
    m_current_setup_ptr(this)
{
    std::string component_name(componentName.c_str());
    m_servo_name = component_name.substr(component_name.rfind('/') + 1);

    AUTO_TRACE(m_servo_name + "::SRTBaseMinorServoImpl()");
}

SRTBaseMinorServoImpl::~SRTBaseMinorServoImpl()
{
    AUTO_TRACE(m_servo_name + "::~SRTBaseMinorServoImpl()");
}

void SRTBaseMinorServoImpl::initialize()
{
    AUTO_TRACE(m_servo_name + "::initialize()");

    unsigned int tempvar;
    if(!IRA::CIRATools::getDBValue(getContainerServices(), "physical_axes", tempvar))
    {
        _EXCPT(ComponentErrors::CDBAccessExImpl, impl, (m_servo_name + "::initialize()").c_str());
        impl.setFieldName("physical_axes");
        throw impl;
    }
    m_physical_axes = tempvar;
    if(!IRA::CIRATools::getDBValue(getContainerServices(), "virtual_axes", tempvar))
    {
        _EXCPT(ComponentErrors::CDBAccessExImpl, impl, (m_servo_name + "::initialize()").c_str());
        impl.setFieldName("virtual_axes");
        throw impl;
    }
    m_virtual_axes = tempvar;

    // Initialize offsets
    m_user_offsets = std::vector<double>(m_virtual_axes, 0.0);
    m_system_offsets = std::vector<double>(m_virtual_axes, 0.0);

    m_socket_configuration = &SRTMinorServoSocketConfiguration::getInstance(getContainerServices());
    m_socket = &SRTMinorServoSocket::getInstance(m_socket_configuration->m_ip_address, m_socket_configuration->m_port, m_socket_configuration->m_timeout);

    m_status = SRTMinorServoAnswerMap();
    m_socket->sendCommand(SRTMinorServoCommandLibrary::status(m_servo_name), m_status);
    m_status_securearea = new IRA::CSecureArea<SRTMinorServoAnswerMap>(&m_status);

    try
    {
        std::string component_name = getContainerServices()->getName().c_str();

        SRTMinorServoDevIOInfo dev_io_info;
        dev_io_info.prefix = m_servo_name + "_";
        dev_io_info.secure_area = m_status_securearea;

        dev_io_info.property_name = "enabled";
        dev_io_info.property_fields = std::vector<std::string>{ "ENABLED" };
        m_enabled_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((component_name + ":enabled").c_str(), getComponent(), new MSDevIO<Management::TBoolean>(dev_io_info), true);

        dev_io_info.property_name = "drive_cabinet_status";
        dev_io_info.property_fields = std::vector<std::string>{ "STATUS" };
        m_drive_cabinet_status_ptr = new ROEnumImpl<ACS_ENUM_T(MinorServo::SRTMinorServoCabinetStatus), POA_MinorServo::ROSRTMinorServoCabinetStatus>((component_name + ":drive_cabinet_status").c_str(), getComponent(), new MSDevIO<MinorServo::SRTMinorServoCabinetStatus>(dev_io_info), true);

        dev_io_info.property_name = "block";
        dev_io_info.property_fields = std::vector<std::string>{ "BLOCK" };
        m_block_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((component_name + ":block").c_str(), getComponent(), new MSDevIO<Management::TBoolean>(dev_io_info), true);

        dev_io_info.property_name = "operative_mode";
        dev_io_info.property_fields = std::vector<std::string>{ "OPERATIVE_MODE" };
        m_operative_mode_ptr = new ROEnumImpl<ACS_ENUM_T(MinorServo::SRTMinorServoOperativeMode), POA_MinorServo::ROSRTMinorServoOperativeMode>((component_name + ":operative_mode").c_str(), getComponent(), new MSDevIO<MinorServo::SRTMinorServoOperativeMode>(dev_io_info), true);

        dev_io_info.property_name = "physical_axes_enabled";
        dev_io_info.property_fields = getPropertiesTable("physical_axes_enabled");
        if(dev_io_info.property_fields.size() != m_physical_axes)
        {
            // Wrong length for physical_axes_enabled property
            _EXCPT(ComponentErrors::ValueOutofRangeExImpl, impl, (m_servo_name + "::initialize()").c_str());
            impl.setValueName(dev_io_info.property_name.c_str());
            impl.setValueLimit(m_physical_axes);
            throw impl;
        }
        m_physical_axes_enabled_ptr = new baci::RObooleanSeq((component_name + ":physical_axes_enabled").c_str(), getComponent(), new MSDevIO<ACS::booleanSeq>(dev_io_info), true);

        dev_io_info.property_name = "physical_positions";
        dev_io_info.property_fields = getPropertiesTable("physical_positions");
        if(dev_io_info.property_fields.size() != m_physical_axes)
        {
            // Wrong length for physical_positions property
            _EXCPT(ComponentErrors::ValueOutofRangeExImpl, impl, (m_servo_name + "::initialize()").c_str());
            impl.setValueName(dev_io_info.property_name.c_str());
            impl.setValueLimit(m_physical_axes);
            throw impl;
        }
        m_physical_positions_ptr = new baci::ROdoubleSeq((component_name + ":physical_positions").c_str(), getComponent(), new MSDevIO<ACS::doubleSeq>(dev_io_info), true);

        m_virtual_axes_ptr = new baci::ROlong((component_name + ":virtual_axes").c_str(), getComponent(), new MSGenericDevIO<CORBA::Long, std::atomic<unsigned int> >(&m_virtual_axes), true);

        m_virtual_axes_names = getPropertiesTable("virtual_positions");
        for(const std::string& axis_name : m_virtual_axes_names)
        {
            if(axis_name.at(0) == 'T')
            {
                m_virtual_axes_units.push_back("mm");
            }
            else if(axis_name.at(0) == 'R')
            {
                m_virtual_axes_units.push_back("degrees");
            }
            else
            {
                m_virtual_axes_units.push_back("unknown");
            }
        }

        dev_io_info.property_name = "virtual_positions";
        dev_io_info.property_fields = m_virtual_axes_names;
        if(m_virtual_axes_names.size() != m_virtual_axes)
        {
            // Wrong length for virtual_axes property
            _EXCPT(ComponentErrors::ValueOutofRangeExImpl, impl, (m_servo_name + "::initialize()").c_str());
            impl.setValueName(dev_io_info.property_name.c_str());
            impl.setValueLimit(m_virtual_axes);
            throw impl;
        }
        m_virtual_positions_ptr = new baci::ROdoubleSeq((component_name + ":virtual_positions").c_str(), getComponent(), new MSDevIO<ACS::doubleSeq>(dev_io_info), true);

        dev_io_info.property_name = "virtual_offsets";
        dev_io_info.property_fields = getPropertiesTable("virtual_offsets");
        if(dev_io_info.property_fields.size() != m_virtual_axes)
        {
            // Wrong length for virtual_offsets property
            _EXCPT(ComponentErrors::ValueOutofRangeExImpl, impl, (m_servo_name + "::initialize()").c_str());
            impl.setValueName(dev_io_info.property_name.c_str());
            impl.setValueLimit(m_virtual_axes);
            throw impl;
        }
        m_virtual_offsets_ptr = new baci::ROdoubleSeq((component_name + ":virtual_offsets").c_str(), getComponent(), new MSDevIO<ACS::doubleSeq>(dev_io_info), true);
        m_virtual_user_offsets_ptr = new baci::ROdoubleSeq((component_name + ":virtual_user_offsets").c_str(), getComponent(), new MSGenericDevIO<ACS::doubleSeq, std::vector<double> >(&m_user_offsets), true);
        m_virtual_system_offsets_ptr = new baci::ROdoubleSeq((component_name + ":virtual_system_offsets").c_str(), getComponent(), new MSGenericDevIO<ACS::doubleSeq, std::vector<double> >(&m_system_offsets), true);

        m_in_use_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((component_name + ":in_use").c_str(), getComponent(), new MSGenericDevIO<Management::TBoolean, std::atomic<Management::TBoolean> >(&m_in_use), true);
        m_current_setup_ptr = new baci::ROstring((component_name + ":current_setup").c_str(), getComponent(), new MSGenericDevIO<ACE_CString, std::string>(&m_current_setup), true);
    }
    catch(std::bad_alloc& ex)
    {
		_THROW_EXCPT(ComponentErrors::MemoryAllocationExImpl, std::string(m_servo_name + "::initialize()").c_str());
    }

    m_current_setup = "";
    m_in_use = Management::MNG_FALSE;
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

void SRTBaseMinorServoImpl::status()
{
    AUTO_TRACE(m_servo_name + "::status()");

    IRA::CSecAreaResourceWrapper<SRTMinorServoAnswerMap> area = m_status_securearea->Get();
    m_socket->sendCommand(SRTMinorServoCommandLibrary::status(m_servo_name), *area);
}

void SRTBaseMinorServoImpl::stow(CORBA::Long stow_position)
{
    AUTO_TRACE(m_servo_name + "::stow()");
    checkErrors();

    SRTMinorServoAnswerMap answer = m_socket->sendCommand(SRTMinorServoCommandLibrary::stow(m_servo_name, (unsigned int)stow_position));

    if(std::get<std::string>(answer["OUTPUT"]) != "GOOD")
    {
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, (m_servo_name + "::stow()").c_str());
        impl.setReason("Received NAK in response to a STOW command!");
        throw impl;
    }
}

void SRTBaseMinorServoImpl::stop()
{
    AUTO_TRACE(m_servo_name + "::stop()");
    checkErrors();

    SRTMinorServoAnswerMap answer = m_socket->sendCommand(SRTMinorServoCommandLibrary::stop(m_servo_name));

    if(std::get<std::string>(answer["OUTPUT"]) != "GOOD")
    {
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, (m_servo_name + "::stop()").c_str());
        impl.setReason("Received NAK in response to a STOP command!");
        throw impl;
    }
}

void SRTBaseMinorServoImpl::preset(const ACS::doubleSeq& virtual_coordinates)
{
    AUTO_TRACE(m_servo_name + "::preset()");
    checkErrors();

    if(virtual_coordinates.length() != m_virtual_axes)
    {
        _EXCPT(MinorServoErrors::PositioningErrorExImpl, impl, (m_servo_name + "::preset()").c_str());
        impl.addData("Reason", "Wrong number of values for this servo system!");
        throw impl;
    }

    std::vector<double> coords(virtual_coordinates.get_buffer(), virtual_coordinates.get_buffer() + virtual_coordinates.length());

    SRTMinorServoAnswerMap answer = m_socket->sendCommand(SRTMinorServoCommandLibrary::preset(m_servo_name, coords));

    if(std::get<std::string>(answer["OUTPUT"]) != "GOOD")
    {
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, (m_servo_name + "::preset()").c_str());
        impl.setReason("Received NAK in response to a PRESET command!");
        throw impl;
    }
}

void SRTBaseMinorServoImpl::offset(const ACS::doubleSeq& virtual_offsets)
{
    AUTO_TRACE(m_servo_name + "::offset()");
    checkErrors();

    if(virtual_offsets.length() != m_virtual_axes)
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, impl, (m_servo_name + "::offset()").c_str());
        impl.addData("Reason", "Wrong number of values for this servo system!");
        throw impl;
    }

    std::vector<double> offsets(virtual_offsets.get_buffer(), virtual_offsets.get_buffer() + virtual_offsets.length());

    SRTMinorServoAnswerMap answer = m_socket->sendCommand(SRTMinorServoCommandLibrary::offset(m_servo_name, offsets));

    if(std::get<std::string>(answer["OUTPUT"]) != "GOOD")
    {
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, (m_servo_name + "::offset()").c_str());
        impl.setReason("Received NAK in response to an OFFSET command!");
        throw impl;
    }
}

ACS::doubleSeq* SRTBaseMinorServoImpl::getUserOffsets()
{
    AUTO_TRACE(m_servo_name + "::getUserOffsets()");

    ACS::doubleSeq_var offsets = new ACS::doubleSeq;
    offsets->length(m_user_offsets.size());
    for(size_t i = 0; i < m_user_offsets.size(); i++)
    {
        offsets[i] = m_user_offsets[i];
    }
    return offsets._retn();
}

void SRTBaseMinorServoImpl::setUserOffset(const char* axis_name, CORBA::Double offset)
{
    AUTO_TRACE(m_servo_name + "::setUserOffset()");
    checkErrors();

    std::string axis(axis_name);
    std::transform(axis.begin(), axis.end(), axis.begin(), ::toupper);

    if(axis == "RZ" && m_virtual_axes == 1)
    {
        axis = "ROTATION";
    }

    unsigned int index = std::find(m_virtual_axes_names.begin(), m_virtual_axes_names.end(), axis) - m_virtual_axes_names.begin();

    try
    {
        m_user_offsets.at(index) = (double)offset;
    }
    catch(std::out_of_range const& ex)
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, impl, (m_servo_name + "::setUserOffset()").c_str());
        impl.addData("Reason", ("Unknown axis '" + axis + "'!").c_str());
        throw impl;
    }

    // Update the offsets
    std::vector<double> offsets(m_virtual_axes, 0);
    std::transform(m_user_offsets.begin(), m_user_offsets.end(), m_system_offsets.begin(), offsets.begin(), std::plus<double>());

    SRTMinorServoAnswerMap answer = m_socket->sendCommand(SRTMinorServoCommandLibrary::offset(m_servo_name, offsets));

    if(std::get<std::string>(answer["OUTPUT"]) != "GOOD")
    {
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, (m_servo_name + "::setUserOffset()").c_str());
        impl.setReason("Received NAK in response to an OFFSET command!");
        throw impl;
    }
}

void SRTBaseMinorServoImpl::clearUserOffsets()
{
    AUTO_TRACE(m_servo_name + "::clearUserOffsets()");
    checkErrors();

    m_user_offsets = std::vector<double>(m_virtual_axes, 0.0);

    // Update the offsets
    std::vector<double> offsets(m_virtual_axes, 0);
    std::transform(m_user_offsets.begin(), m_user_offsets.end(), m_system_offsets.begin(), offsets.begin(), std::plus<double>());

    SRTMinorServoAnswerMap answer = m_socket->sendCommand(SRTMinorServoCommandLibrary::offset(m_servo_name, offsets));

    if(std::get<std::string>(answer["OUTPUT"]) != "GOOD")
    {
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, (m_servo_name + "::clearUserOffset()").c_str());
        impl.setReason("Received NAK in response to an OFFSET command!");
        throw impl;
    }
}

ACS::doubleSeq* SRTBaseMinorServoImpl::getSystemOffsets()
{
    AUTO_TRACE(m_servo_name + "::getSystemOffsets()");

    ACS::doubleSeq_var offsets = new ACS::doubleSeq;
    offsets->length(m_system_offsets.size());
    for(size_t i = 0; i < m_system_offsets.size(); i++)
    {
        offsets[i] = m_system_offsets[i];
    }
    return offsets._retn();
}

void SRTBaseMinorServoImpl::setSystemOffset(const char* axis_name, CORBA::Double offset)
{
    AUTO_TRACE(m_servo_name + "::setSystemOffset()");
    checkErrors();

    std::string axis(axis_name);
    std::transform(axis.begin(), axis.end(), axis.begin(), ::toupper);

    if(axis == "RZ" && m_virtual_axes == 1)
    {
        axis = "ROTATION";
    }

    unsigned int index = std::find(m_virtual_axes_names.begin(), m_virtual_axes_names.end(), axis) - m_virtual_axes_names.begin();

    try
    {
        m_system_offsets.at(index) = (double)offset;
    }
    catch(std::out_of_range const& ex)
    {
        _EXCPT(MinorServoErrors::OffsetErrorExImpl, impl, (m_servo_name + "::setSystemOffset()").c_str());
        impl.addData("Reason", ("Unknown axis '" + axis + "'!").c_str());
        throw impl;
    }

    // Update the offsets
    std::vector<double> offsets(m_virtual_axes, 0);
    std::transform(m_user_offsets.begin(), m_user_offsets.end(), m_system_offsets.begin(), offsets.begin(), std::plus<double>());

    SRTMinorServoAnswerMap answer = m_socket->sendCommand(SRTMinorServoCommandLibrary::offset(m_servo_name, offsets));

    if(std::get<std::string>(answer["OUTPUT"]) != "GOOD")
    {
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, (m_servo_name + "::setSystemOffset()").c_str());
        impl.setReason("Received NAK in response to an OFFSET command!");
        throw impl;
    }
}

void SRTBaseMinorServoImpl::clearSystemOffsets()
{
    AUTO_TRACE(m_servo_name + "::clearSystemOffsets()");
    checkErrors();

    m_system_offsets = std::vector<double>(m_virtual_axes, 0.0);

    // Update the offsets
    std::vector<double> offsets(m_virtual_axes, 0);
    std::transform(m_user_offsets.begin(), m_user_offsets.end(), m_system_offsets.begin(), offsets.begin(), std::plus<double>());

    SRTMinorServoAnswerMap answer = m_socket->sendCommand(SRTMinorServoCommandLibrary::offset(m_servo_name, offsets));

    if(std::get<std::string>(answer["OUTPUT"]) != "GOOD")
    {
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, (m_servo_name + "::clearSystemOffset()").c_str());
        impl.setReason("Received NAK in response to an OFFSET command!");
        throw impl;
    }
}

void SRTBaseMinorServoImpl::setup(const char* configuration_name)
{
    AUTO_TRACE(m_servo_name + "::setup()");
    m_in_use = Management::MNG_FALSE;
    m_current_lookup_table.clear();

    m_current_setup = "";
    std::string setup_name(configuration_name);
    std::transform(setup_name.begin(), setup_name.end(), setup_name.begin(), ::toupper);

    if(setup_name.empty())
    {
        return;
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
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl, impl, error);
        impl.setCode(error.getErrorCode());
        impl.setDescription((const char *)error.getDescription());
        throw impl;
    }
    if(!table.openTable(error))
    {
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, impl, error);
        throw impl;
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
        m_in_use = Management::MNG_TRUE;
    }
}

void SRTBaseMinorServoImpl::getAxesInfo(ACS::stringSeq_out axes_names, ACS::stringSeq_out axes_units)
{
    AUTO_TRACE("SRTBaseMinorServoImpl::getAxesInfo()");

    ACS::stringSeq_var axes_names_var = new ACS::stringSeq;
    ACS::stringSeq_var axes_units_var = new ACS::stringSeq;
    axes_names_var->length(m_virtual_axes);
    axes_units_var->length(m_virtual_axes);

    for(size_t i = 0; i < m_virtual_axes; i++)
    {
        axes_names_var[i] = m_virtual_axes_names[i].c_str();
        axes_units_var[i] = m_virtual_axes_units[i].c_str();
    }

    axes_names = axes_names_var._retn();
    axes_units = axes_units_var._retn();
}

ACS::doubleSeq* SRTBaseMinorServoImpl::calcCoordinates(double elevation)
{
    AUTO_TRACE(m_servo_name + "::calcCoordinates()");

    if(m_in_use == Management::MNG_TRUE)
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
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, (m_servo_name + "::calcCoordinates()").c_str());
        impl.setReason("Unable to calculate the coordinates since the servo system has not been configured yet!");
        throw impl;
    }
}

std::vector<std::string> SRTBaseMinorServoImpl::getPropertiesTable(const std::string& properties_name)
{
    AUTO_TRACE(m_servo_name + "::getPropertiesTable()");

    std::vector<std::string> properties;
    
    IRA::CDBTable table(getContainerServices(), properties_name.c_str(), std::string("DataBlock/MinorServo/" + m_servo_name).c_str());
    IRA::CError error;
    error.Reset();

    if(!table.addField(error, "property_name", IRA::CDataField::STRING))
    {
        error.setExtra("Error adding field property_name", 0);
    }
    if(!error.isNoError())
    {
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl, impl, error);
        impl.setCode(error.getErrorCode());
        impl.setDescription((const char *)error.getDescription());
        throw impl;
    }
    if(!table.openTable(error))
    {
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, impl, error);
        throw impl;
    }

    table.First();
    for(unsigned int i = 0; i < table.recordCount(); i++, table.Next())
    {
        properties.push_back(std::string(table["property_name"]->asString()));
    }
    table.closeTable();

    return properties;
}

void SRTBaseMinorServoImpl::checkErrors()
{
    ACSErr::Completion_var comp;
    if(block()->get_sync(comp.out()) == Management::MNG_TRUE || drive_cabinet_status()->get_sync(comp.out()) == MinorServo::DRIVE_CABINET_ERROR)
    {
        _EXCPT(MinorServoErrors::StatusErrorExImpl, impl, (m_servo_name + "::checkErrors()").c_str());
        impl.setReason("Servo system blocked or drive cabinet error!");
        throw impl;
    }
}

GET_PROPERTY_REFERENCE(Management::ROTBoolean, m_enabled_ptr, enabled);
GET_PROPERTY_REFERENCE(MinorServo::ROSRTMinorServoCabinetStatus, m_drive_cabinet_status_ptr, drive_cabinet_status);
GET_PROPERTY_REFERENCE(Management::ROTBoolean, m_block_ptr, block);
GET_PROPERTY_REFERENCE(MinorServo::ROSRTMinorServoOperativeMode, m_operative_mode_ptr, operative_mode);
GET_PROPERTY_REFERENCE(ACS::RObooleanSeq, m_physical_axes_enabled_ptr, physical_axes_enabled);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq, m_physical_positions_ptr, physical_positions);
GET_PROPERTY_REFERENCE(ACS::ROlong, m_virtual_axes_ptr, virtual_axes);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq, m_virtual_positions_ptr, virtual_positions);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq, m_virtual_offsets_ptr, virtual_offsets);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq, m_virtual_user_offsets_ptr, virtual_user_offsets);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq, m_virtual_system_offsets_ptr, virtual_system_offsets);
GET_PROPERTY_REFERENCE(Management::ROTBoolean, m_in_use_ptr, in_use);
GET_PROPERTY_REFERENCE(ACS::ROstring, m_current_setup_ptr, current_setup)
