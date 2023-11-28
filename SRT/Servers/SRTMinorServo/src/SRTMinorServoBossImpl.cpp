#include "SRTMinorServoBossImpl.h"

using namespace maci;
using namespace SimpleParser;

SRTMinorServoBossImpl::SRTMinorServoBossImpl(const ACE_CString &componentName, maci::ContainerServices *containerServices) :
    CharacteristicComponentImpl(componentName, containerServices),
    m_core(NULL),
    m_parser(NULL),
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

    m_core = new SRTMinorServoBossCore(this);

    try
    {
        std::string component_name = getContainerServices()->getName().c_str();

        SRTMinorServoDevIOInfo dev_io_info;
        dev_io_info.secure_area = m_core->m_status_secure_area;

        dev_io_info.property_name = "current_configuration";
        dev_io_info.property_fields = std::vector<std::string>{ "CURRENT_CONFIG" };
        m_current_configuration_ptr = new ROEnumImpl<ACS_ENUM_T(MinorServo::SRTMinorServoFocalConfiguration), POA_MinorServo::ROSRTMinorServoFocalConfiguration>((component_name + ":current_configuration").c_str(), getComponent(), new MSDevIO<MinorServo::SRTMinorServoFocalConfiguration>(dev_io_info), true);

        dev_io_info.property_name = "simulation_enabled";
        dev_io_info.property_fields = std::vector<std::string>{ "SIMULATION_ENABLED" };
        m_simulation_enabled_ptr = new baci::ROboolean((component_name + ":simulation_enabled").c_str(), getComponent(), new MSDevIO<CORBA::Boolean>(dev_io_info), true);

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
        m_power_ptr = new baci::ROboolean((component_name + ":power").c_str(), getComponent(), new MSDevIO<CORBA::Boolean>(dev_io_info), true);

        dev_io_info.property_name = "emergency";
        dev_io_info.property_fields = std::vector<std::string>{ "EMERGENCY" };
        m_emergency_ptr = new baci::ROboolean((component_name + ":emergency").c_str(), getComponent(), new MSDevIO<CORBA::Boolean>(dev_io_info), true);

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

    //m_GFR = getContainerServices()->getComponent<MinorServo::SRTGenericMinorServo>("MINORSERVO/GFR");
    //m_SRP = getContainerServices()->getComponent<MinorServo::SRTProgramTrackMinorServo>("MINORSERVO/SRP");

    try
    {
        double status_thread_period;
        if(!IRA::CIRATools::getDBValue(getContainerServices(), "status_thread_period", status_thread_period))
        {
            ComponentErrors::CDBAccessExImpl exImpl(__FILE__, __LINE__, "SRTMinorServoBossImpl::initialize()");
            exImpl.setFieldName("status_thread_period");
            throw exImpl;
        }
        m_status_thread = getContainerServices()->getThreadManager()->create<SRTMinorServoStatusThread, SRTMinorServoBossCore*>("SRTMinorServoBossImplStatusThread", m_core);
        m_status_thread->setSleepTime(status_thread_period * 10000000);
        m_status_thread->resume();
    }
    catch(acsthreadErrType::acsthreadErrTypeExImpl& ex)
    {
        _THROW_EXCPT(ComponentErrors::ThreadErrorExImpl, "SRTMinorServoBossImpl::initialize()");
    }
    catch(...)
    {
        _THROW_EXCPT(ComponentErrors::UnexpectedExImpl, "SRTMinorServoBossImpl::initialize()");
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

void SRTMinorServoBossImpl::setElevationTracking(const char* value)
{
    AUTO_TRACE("SRTMinorServoBossImpl::setElevationTracking()");
}

void SRTMinorServoBossImpl::setASConfiguration(const char* value)
{
    AUTO_TRACE("SRTMinorServoBossImpl::setASConfiguration()");
}

void SRTMinorServoBossImpl::setOffsets(const char* axis_code, const double& offset)
{
    AUTO_TRACE("SRTMinorServoBossImpl::setOffset()");
}

void SRTMinorServoBossImpl::clearOffsets()
{
    AUTO_TRACE("SRTMinorServoBossImpl::clearOffsets()");
    // Loop through servos and clear their offsets
}

CORBA::Boolean SRTMinorServoBossImpl::checkScan(const ACS::Time startingTime, const MinorServo::MinorServoScan &scan, const Antenna::TRunTimeParameters &antennaInfo, MinorServo::TRunTimeParameters_out msParameters)
{
    return true;
}

void SRTMinorServoBossImpl::startScan(ACS::Time &startingTime, const MinorServo::MinorServoScan &scan, const Antenna::TRunTimeParameters &antennaInfo)
{
}

void SRTMinorServoBossImpl::closeScan(ACS::Time &timeToStop)
{
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
        ACS_SHORT_LOG((LM_WARNING, "SRTMinorServoBoss::command(): unknown exception."));
        res = false;
    }
    answer = CORBA::string_dup((const char *)out);
    return res;
}

GET_PROPERTY_REFERENCE(MinorServo::ROSRTMinorServoFocalConfiguration, m_current_configuration_ptr, current_configuration);
GET_PROPERTY_REFERENCE(ACS::ROboolean, m_simulation_enabled_ptr, simulation_enabled);
GET_PROPERTY_REFERENCE(ACS::ROdouble, m_plc_time_ptr, plc_time);
GET_PROPERTY_REFERENCE(ACS::ROstring, m_plc_version_ptr, plc_version);
GET_PROPERTY_REFERENCE(MinorServo::ROSRTMinorServoControlStatus, m_control_ptr, control);
GET_PROPERTY_REFERENCE(ACS::ROboolean, m_power_ptr, power);
GET_PROPERTY_REFERENCE(ACS::ROboolean, m_emergency_ptr, emergency);
GET_PROPERTY_REFERENCE(MinorServo::ROSRTMinorServoGregorianCoverStatus, m_gregorian_cover_ptr, gregorian_cover);
GET_PROPERTY_REFERENCE(ACS::ROdouble, m_last_executed_command_ptr, last_executed_command);

#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SRTMinorServoBossImpl)
