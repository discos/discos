#include "SRTMinorServoImpl.h"

using namespace maci;

SRTBaseMinorServoImpl::SRTBaseMinorServoImpl(const ACE_CString &componentName, maci::ContainerServices *containerServices) :
    CharacteristicComponentImpl(componentName, containerServices),
    m_enabled_ptr(this),
    m_drive_cabinet_status_ptr(this),
    m_block_ptr(this),
    m_operative_mode_ptr(this),
    m_axes_enabled_ptr(this),
    m_physical_positions_ptr(this),
    m_virtual_positions_ptr(this),
    m_virtual_offsets_ptr(this)
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

    if(!IRA::CIRATools::getDBValue(getContainerServices(), "physical_axes", m_physical_axes))
    {
        ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, (m_servo_name + "::initialize()").c_str());
        exImpl.setFieldName("physical_axes");
        throw exImpl;
    }
    if(!IRA::CIRATools::getDBValue(getContainerServices(), "virtual_axes", m_virtual_axes))
    {
        ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, (m_servo_name + "::initialize()").c_str());
        exImpl.setFieldName("virtual_axes");
        throw exImpl;
    }

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
        m_enabled_ptr = new baci::ROboolean((component_name + ":enabled").c_str(), getComponent(), new MSDevIO<CORBA::Boolean>(dev_io_info), true);

        dev_io_info.property_name = "drive_cabinet_status";
        dev_io_info.property_fields = std::vector<std::string>{ "STATUS" };
        m_drive_cabinet_status_ptr = new ROEnumImpl<ACS_ENUM_T(MinorServo::SRTMinorServoCabinetStatus), POA_MinorServo::ROSRTMinorServoCabinetStatus>((component_name + ":drive_cabinet_status").c_str(), getComponent(), new MSDevIO<MinorServo::SRTMinorServoCabinetStatus>(dev_io_info), true);

        dev_io_info.property_name = "block";
        dev_io_info.property_fields = std::vector<std::string>{ "BLOCK" };
        m_block_ptr = new baci::ROboolean((component_name + ":block").c_str(), getComponent(), new MSDevIO<CORBA::Boolean>(dev_io_info), true);

        dev_io_info.property_name = "operative_mode";
        dev_io_info.property_fields = std::vector<std::string>{ "OPERATIVE_MODE" };
        m_operative_mode_ptr = new ROEnumImpl<ACS_ENUM_T(MinorServo::SRTMinorServoOperativeMode), POA_MinorServo::ROSRTMinorServoOperativeMode>((component_name + ":operative_mode").c_str(), getComponent(), new MSDevIO<MinorServo::SRTMinorServoOperativeMode>(dev_io_info), true);

        dev_io_info.property_name = "axes_enabled";
        dev_io_info.property_fields = getPropertiesTable("axes_enabled");
        if(dev_io_info.property_fields.size() != m_physical_axes)
        {
            // Wrong length for axes_enabled property
        }
        m_axes_enabled_ptr = new baci::RObooleanSeq((component_name + ":axes_enabled").c_str(), getComponent(), new MSDevIO<ACS::booleanSeq>(dev_io_info), true);

        dev_io_info.property_name = "physical_positions";
        dev_io_info.property_fields = getPropertiesTable("physical_positions");
        if(dev_io_info.property_fields.size() != m_physical_axes)
        {
            // Wrong length for physical_positions property
        }
        m_physical_positions_ptr = new baci::ROdoubleSeq((component_name + ":physical_positions").c_str(), getComponent(), new MSDevIO<ACS::doubleSeq>(dev_io_info), true);

        m_virtual_axes_names = getPropertiesTable("virtual_positions");
        dev_io_info.property_name = "virtual_positions";
        dev_io_info.property_fields = m_virtual_axes_names;
        if(m_virtual_axes_names.size() != m_virtual_axes)
        {
            // Wrong length for virtual_axes property
        }
        m_virtual_positions_ptr = new baci::ROdoubleSeq((component_name + ":virtual_positions").c_str(), getComponent(), new MSDevIO<ACS::doubleSeq>(dev_io_info), true);

        dev_io_info.property_name = "virtual_offsets";
        dev_io_info.property_fields = getPropertiesTable("virtual_offsets");
        if(dev_io_info.property_fields.size() != m_virtual_axes)
        {
            // Wrong length for virtual_offsets property
        }
        m_virtual_offsets_ptr = new baci::ROdoubleSeq((component_name + ":virtual_offsets").c_str(), getComponent(), new MSDevIO<ACS::doubleSeq>(dev_io_info), true);
    }
    catch(std::bad_alloc& ex)
    {
		_THROW_EXCPT(ComponentErrors::MemoryAllocationExImpl, std::string(m_servo_name + "::initialize()").c_str());
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
        _THROW_EXCPT(MinorServoErrors::StowErrorExImpl, std::string(m_servo_name + "::stow()").c_str());
    }
}

void SRTBaseMinorServoImpl::stop()
{
    AUTO_TRACE(m_servo_name + "::stop()");
    checkErrors();

    SRTMinorServoAnswerMap answer = m_socket->sendCommand(SRTMinorServoCommandLibrary::stop(m_servo_name));

    if(std::get<std::string>(answer["OUTPUT"]) != "GOOD")
    {
        _THROW_EXCPT(MinorServoErrors::OperationNotPermittedExImpl, std::string(m_servo_name + "::stop()").c_str());
    }
}

void SRTBaseMinorServoImpl::preset(const ACS::doubleSeq& virtual_coordinates)
{
    AUTO_TRACE(m_servo_name + "::preset()");
    checkErrors();

    if(virtual_coordinates.length() != m_virtual_axes)
    {
        //Wrong number of virtual_coordinates
    }
    else if(m_current_lookup_table.empty())
    {
        // Empty lookup table, cannot execute
    }

    std::vector<double> coords(virtual_coordinates.get_buffer(), virtual_coordinates.get_buffer() + virtual_coordinates.length());

    SRTMinorServoAnswerMap answer = m_socket->sendCommand(SRTMinorServoCommandLibrary::preset(m_servo_name, coords));

    if(std::get<std::string>(answer["OUTPUT"]) != "GOOD")
    {
        _THROW_EXCPT(MinorServoErrors::PositioningErrorExImpl, std::string(m_servo_name + "::preset()").c_str());
    }
}

void SRTBaseMinorServoImpl::offset(const ACS::doubleSeq& virtual_offsets)
{
    AUTO_TRACE(m_servo_name + "::offset()");
    checkErrors();

    if(virtual_offsets.length() != m_virtual_axes)
    {
        //Wrong number of virtual_offsets
    }

    std::vector<double> offsets(virtual_offsets.get_buffer(), virtual_offsets.get_buffer() + virtual_offsets.length());

    SRTMinorServoAnswerMap answer = m_socket->sendCommand(SRTMinorServoCommandLibrary::offset(m_servo_name, offsets));

    if(std::get<std::string>(answer["OUTPUT"]) != "GOOD")
    {
        _THROW_EXCPT(MinorServoErrors::OffsetErrorExImpl, std::string(m_servo_name + "::offset()").c_str());
    }
}

void SRTBaseMinorServoImpl::setup(const char* configuration_name)
{
    m_current_lookup_table = getLookupTable(std::string(configuration_name));
}

std::vector<std::string> SRTBaseMinorServoImpl::getPropertiesTable(std::string properties_name)
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
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl, dummy, error);
        dummy.setCode(error.getErrorCode());
        dummy.setDescription((const char *)error.getDescription());
        throw dummy;
    }
    if(!table.openTable(error))
    {
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
        throw dummy;
    }

    table.First();
    for(unsigned int i = 0; i < table.recordCount(); i++, table.Next())
    {
        properties.push_back(std::string(table["property_name"]->asString()));
    }
    table.closeTable();

    return properties;
}

SRTMinorServoLookupTable SRTBaseMinorServoImpl::getLookupTable(std::string configuration_name)
{
    AUTO_TRACE(m_servo_name + "::getLookupTable()");

    IRA::CDBTable table(getContainerServices(), configuration_name.c_str(), std::string("DataBlock/MinorServo/" + m_servo_name).c_str());
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
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl, dummy, error);
        dummy.setCode(error.getErrorCode());
        dummy.setDescription((const char *)error.getDescription());
        throw dummy;
    }
    if(!table.openTable(error))
    {
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
        throw dummy;
    }

    SRTMinorServoLookupTable lookup_table;

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

        lookup_table[axis] = coefficients;
    }
    table.closeTable();

    return lookup_table;
}

void SRTBaseMinorServoImpl::checkErrors()
{
    ACSErr::Completion_var comp;
    if(block()->get_sync(comp.out()) || drive_cabinet_status()->get_sync(comp.out()) == MinorServo::DRIVE_CABINET_ERROR)
    {
        _THROW_EXCPT(MinorServoErrors::StatusErrorExImpl, std::string(m_servo_name + "::checkErrors()").c_str());
    }
}

GET_PROPERTY_REFERENCE(ACS::ROboolean, m_enabled_ptr, enabled);
GET_PROPERTY_REFERENCE(MinorServo::ROSRTMinorServoCabinetStatus, m_drive_cabinet_status_ptr, drive_cabinet_status);
GET_PROPERTY_REFERENCE(ACS::ROboolean, m_block_ptr, block);
GET_PROPERTY_REFERENCE(MinorServo::ROSRTMinorServoOperativeMode, m_operative_mode_ptr, operative_mode);
GET_PROPERTY_REFERENCE(ACS::RObooleanSeq, m_axes_enabled_ptr, axes_enabled);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq, m_physical_positions_ptr, physical_positions);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq, m_virtual_positions_ptr, virtual_positions);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq, m_virtual_offsets_ptr, virtual_offsets);
