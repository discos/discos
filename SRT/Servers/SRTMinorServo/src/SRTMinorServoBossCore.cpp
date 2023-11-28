#include "SRTMinorServoBossCore.h"

using namespace maci;

SRTMinorServoBossCore::SRTMinorServoBossCore(SRTMinorServoBossImpl* component)
{
    AUTO_TRACE("SRTMinorServoBossCore::SRTMinorServoBossCore()");

    m_boss_status = MinorServo::BOSS_STATUS_UNCONFIGURED;

    m_current_configuration = MinorServo::CONFIGURATION_UNKNOWN;
    m_requested_configuration = MinorServo::CONFIGURATION_UNKNOWN;
    m_focal_configurations_table.insert(SRTMinorServoFocalConfigurationsTable::value_type("Primario", MinorServo::CONFIGURATION_PRIMARY));
    m_focal_configurations_table.insert(SRTMinorServoFocalConfigurationsTable::value_type("Gregoriano1", MinorServo::CONFIGURATION_GREGORIAN1));
    m_focal_configurations_table.insert(SRTMinorServoFocalConfigurationsTable::value_type("Gregoriano2", MinorServo::CONFIGURATION_GREGORIAN2));
    m_focal_configurations_table.insert(SRTMinorServoFocalConfigurationsTable::value_type("Gregoriano3", MinorServo::CONFIGURATION_GREGORIAN3));
    m_focal_configurations_table.insert(SRTMinorServoFocalConfigurationsTable::value_type("Gregoriano4", MinorServo::CONFIGURATION_GREGORIAN4));
    m_focal_configurations_table.insert(SRTMinorServoFocalConfigurationsTable::value_type("Gregoriano5", MinorServo::CONFIGURATION_GREGORIAN5));
    m_focal_configurations_table.insert(SRTMinorServoFocalConfigurationsTable::value_type("Gregoriano6", MinorServo::CONFIGURATION_GREGORIAN6));
    m_focal_configurations_table.insert(SRTMinorServoFocalConfigurationsTable::value_type("Gregoriano7", MinorServo::CONFIGURATION_GREGORIAN7));
    m_focal_configurations_table.insert(SRTMinorServoFocalConfigurationsTable::value_type("Gregoriano8", MinorServo::CONFIGURATION_GREGORIAN8));
    m_focal_configurations_table.insert(SRTMinorServoFocalConfigurationsTable::value_type("BWG1", MinorServo::CONFIGURATION_BWG1));
    m_focal_configurations_table.insert(SRTMinorServoFocalConfigurationsTable::value_type("BWG2", MinorServo::CONFIGURATION_BWG2));
    m_focal_configurations_table.insert(SRTMinorServoFocalConfigurationsTable::value_type("BWG3", MinorServo::CONFIGURATION_BWG3));
    m_focal_configurations_table.insert(SRTMinorServoFocalConfigurationsTable::value_type("BWG4", MinorServo::CONFIGURATION_BWG4));

    m_component = component;

    m_socket_configuration = &SRTMinorServoSocketConfiguration::getInstance(m_component->getContainerServices());
    m_socket = &SRTMinorServoSocket::getInstance(m_socket_configuration->m_ip_address, m_socket_configuration->m_port, m_socket_configuration->m_timeout);

    m_status = SRTMinorServoAnswerMap();
    m_socket->sendCommand(SRTMinorServoCommandLibrary::status(), m_status);
    m_status_secure_area = new IRA::CSecureArea<SRTMinorServoAnswerMap>(&m_status);

    m_GFR = m_component->getContainerServices()->getComponent<MinorServo::SRTGenericMinorServo>("MINORSERVO/GFR");
    m_SRP = m_component->getContainerServices()->getComponent<MinorServo::SRTProgramTrackMinorServo>("MINORSERVO/SRP");

    m_servos.push_back((MinorServo::SRTBaseMinorServo_ptr)m_GFR);
    m_servos.push_back((MinorServo::SRTBaseMinorServo_ptr)m_SRP);

    m_setup_thread = NULL;
    m_park_thread = NULL;

    m_ready = false;
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
}

void SRTMinorServoBossCore::setup(std::string configuration)
{
    AUTO_TRACE("SRTMinorServoBossCore::setup()");

    ACSErr::Completion_var comp;

    if(!checkControl() || m_component->emergency()->get_sync(comp.out()))
    {
        // Minor servos are controlled by VBrain or there is an emergency button pressed somewhere
        // I raise OperationNotPermitted since there is no other, more fitting error
		_THROW_EXCPT(MinorServoErrors::OperationNotPermittedExImpl, "SRTMinorServoBossCore::setup()");
    }

    MinorServo::SRTMinorServoFocalConfiguration requested_configuration;

    try
    {
        requested_configuration = m_focal_configurations_table.left.at(configuration);
    }
    catch(std::out_of_range& ex)  // Unknown configuration
    {
		_THROW_EXCPT(MinorServoErrors::ConfigurationErrorExImpl, "SRTMinorServoBossCore::setup()");
    }

    // Exit if commanded configuration is already in place
    if(requested_configuration == m_current_configuration && (m_boss_status == MinorServo::BOSS_STATUS_CONFIGURED || m_boss_status == MinorServo::BOSS_STATUS_SETUP_IN_PROGRESS))
    {
        return;
    }

    m_requested_configuration = requested_configuration;

    // Stop the setup and park threads if running
    if(m_setup_thread != NULL)
    {
        m_setup_thread->suspend();
        m_setup_thread->terminate();
    }
    if(m_park_thread != NULL)
    {
        m_park_thread->suspend();
        m_park_thread->terminate();
    }

    m_current_configuration = MinorServo::CONFIGURATION_UNKNOWN;
    m_boss_status = MinorServo::BOSS_STATUS_SETUP_IN_PROGRESS;

    // Send the STOP command to each servo
    for(auto& servo : m_servos)
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
            _THROW_EXCPT(ComponentErrors::ThreadErrorExImpl, "SRTMinorServoBossImpl::setup()");
        }

        m_setup_thread->resume();
    }
}

void SRTMinorServoBossCore::park()
{
    AUTO_TRACE("SRTMinorServoBossCore::park()");

    ACSErr::Completion_var comp;

    if(!checkControl() || m_component->emergency()->get_sync(comp.out()))
    {
        // Minor servos are controlled by VBrain or there is an emergency button pressed somewhere
        // I raise OperationNotPermitted since there is no other, more fitting error
		_THROW_EXCPT(MinorServoErrors::OperationNotPermittedExImpl, "SRTMinorServoBossCore::setup()");
    }

    m_requested_configuration = MinorServos::CONFIGURATION_PARK;

    // Stop the setup and park threads if running
    if(m_setup_thread != NULL)
    {
        m_setup_thread->suspend();
        m_setup_thread->terminate();
    }
    if(m_park_thread != NULL)
    {
        m_park_thread->suspend();
        m_park_thread->terminate();
    }

    m_current_configuration = MinorServo::CONFIGURATION_UNKNOWN;
    m_boss_status = MinorServo::BOSS_STATUS_PARK_IN_PROGRESS;

    // Send the STOW command to the gregorian cover
    if(std::get<std::string>(m_socket->sendCommand(SRTMinorServoCommandLibrary::stow("GREGORIAN_CAP", 2))["OUTPUT"]) != "GOOD")
    {
        _THROW_EXCPT(MinorServoErrors::SetupErrorExImpl, "SRTMinorServoBossCore::park()");
    }

    // Send the STOP command to each servo
    for(auto& servo : m_servos)
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
            _THROW_EXCPT(ComponentErrors::ThreadErrorExImpl, "SRTMinorServoBossImpl::park()");
        }

        m_park_thread->resume();
    }
}

void SRTMinorServoBossCore::status()
{
    AUTO_TRACE("SRTMinorServoBossCore::status()");

    IRA::CSecAreaResourceWrapper<SRTMinorServoAnswerMap> status = m_status_secure_area->Get();
    m_socket->sendCommand(SRTMinorServoCommandLibrary::status(), m_status);
}

bool SRTMinorServoBossCore::checkControl()
{
    ACSErr::Completion_var comp;
    return m_component->control()->get_sync(comp.out()) == MinorServo::CONTROL_DISCOS ? true : false;
}
