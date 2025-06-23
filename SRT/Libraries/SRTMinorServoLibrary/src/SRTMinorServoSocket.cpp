#include "SRTMinorServoSocket.h"

using namespace MinorServo;

std::mutex SRTMinorServoSocket::c_mutex;

SRTMinorServoSocket& SRTMinorServoSocket::getInstance(std::string ip_address, int port, double timeout)
{
    std::lock_guard<std::mutex> guard(SRTMinorServoSocket::c_mutex);

    if(m_instance != nullptr)
    {
        if(m_instance->m_ip_address != ip_address || m_instance->m_port != port)
        {
            _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, "SRTMinorServoSocket::getInstance(std::string, int)");
            impl.setReason(("Socket already open on '" + m_instance->m_ip_address + ":" + std::to_string(m_instance->m_port) + "' . Use getInstance() (no arguments) to retrieve the object.").c_str());
            throw impl.getMinorServoErrorsEx();
        }
    }
    else
    {
        m_instance = new SRTMinorServoSocket(ip_address, port, timeout);
    }
    return *m_instance;
}

SRTMinorServoSocket& SRTMinorServoSocket::getInstance()
{
    std::lock_guard<std::mutex> guard(SRTMinorServoSocket::c_mutex);

    if(m_instance == nullptr)
    {
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, "SRTMinorServoSocket::getInstance()");
        impl.setReason("Socket not yet initialized. Use getInstance(std::string ip_address, int port) to initialize it and retrieve the object.");
        throw impl.getMinorServoErrorsEx();
    }
    return *m_instance;
}

SRTMinorServoSocket::SRTMinorServoSocket(std::string ip_address, int port, double timeout) : m_ip_address(ip_address), m_port(port), m_timeout(timeout), m_socket_status(NOTREADY)
{
    try
    {
        connect();
    }
    catch(...)
    {
        // Not yet connected, we catch the exception in order to go on
    }
}

SRTMinorServoSocket::~SRTMinorServoSocket()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    Close(m_error);
}

void SRTMinorServoSocket::connect()
{
    if(isConnected())
    {
        return;
    }

    std::lock_guard<std::mutex> guard(m_mutex);

    Close(m_error);
    m_error.Reset();
    if(Create(m_error, STREAM) == FAIL)
    {
        Close(m_error);
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, "SRTMinorServoSocket::SRTMinorServoSocket()");
        impl.setReason("Cannot create the socket.");
        throw impl.getMinorServoErrorsEx();
    }

    if(setSockMode(m_error, NONBLOCKING) != SUCCESS)
    {
        m_socket_status = NOTREADY;
        Close(m_error);
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, "SRTMinorServoSocket::SRTMinorServoSocket()");
        impl.setReason("Cannot set the socket to non-blocking.");
        throw impl.getMinorServoErrorsEx();
    }

    if(Connect(m_error, m_ip_address.c_str(), m_port) == FAIL)
    {
        m_socket_status = TIMEOUT;
        Close(m_error);
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, "SRTMinorServoSocket::SRTMinorServoSocket()");
        impl.setReason("Cannot connect the socket.");
        throw impl.getMinorServoErrorsEx();
    }

    m_socket_status = READY;
}

const bool SRTMinorServoSocket::isConnected() const
{
    return m_socket_status == READY ? true : false;
}

SRTMinorServoAnswerMap SRTMinorServoSocket::sendCommand(std::string command, std::optional<std::reference_wrapper<SRTMinorServoAnswerMap>> map)
{
    connect();

    std::lock_guard<std::mutex> guard(m_mutex);

    double start_time = IRA::CIRATools::getUNIXTime();
    size_t sent_bytes = 0;

    while(sent_bytes < command.size())
    {
        size_t sent_now;

        try
        {
            sent_now = Send(m_error, command.substr(sent_bytes, command.size() - sent_bytes).c_str(), command.size() - sent_bytes);
            sent_bytes += sent_now;
        }
        catch(...)
        {
            m_socket_status = NOTREADY;
            Close(m_error);
            _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, "SRTMinorServoSocker::sendCommand()");
            impl.setReason("Something went wrong while sending some bytes.");
            throw impl.getMinorServoErrorsEx();
        }

        if(sent_now > 0)
        {
            // Reset the timer
            start_time = IRA::CIRATools::getUNIXTime();
        }
        else if(IRA::CIRATools::getUNIXTime() - start_time >= m_timeout)
        {
            m_socket_status = TIMEOUT;
            Close(m_error);
            _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, "SRTMinorServoSocket::sendCommand()");
            impl.setReason("Timeout when sending command.");
            throw impl.getMinorServoErrorsEx();
        }
    }

    start_time = IRA::CIRATools::getUNIXTime();
    std::string answer;

    while(answer.size() < 2 || !(answer.rfind(CLOSER) == answer.size() - CLOSER.size()))
    {
        char buf;
        try
        {
            if(Receive(m_error, &buf, 1) == 1)
            {
                answer += buf;

                // Reset the timer
                start_time = IRA::CIRATools::getUNIXTime();
            }
            else if(IRA::CIRATools::getUNIXTime() - start_time >= m_timeout)
            {
                m_socket_status = TIMEOUT;
                Close(m_error);
                _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, "SRTMinorServoSocket::sendCommand()");
                impl.setReason("Timeout when receiving answer.");
                throw impl.getMinorServoErrorsEx();
            }
        }
        catch(...)
        {
            m_socket_status = NOTREADY;
            Close(m_error);
            _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, "SRTMinorServoSocker::sendCommand()");
            impl.setReason(("Something went wrong while receiving some bytes. Command: " + command).c_str());
            throw impl.getMinorServoErrorsEx();
        }
    }

    SRTMinorServoAnswerMap map_answer = SRTMinorServoCommandLibrary::parseAnswer(answer);
    map_answer.put("PLAIN_COMMAND", command);
    if(map)
    {
        map->get() = map_answer;
    }

    return map_answer;
}

SRTMinorServoSocketConfiguration& SRTMinorServoSocketConfiguration::getInstance(maci::ContainerServices* containerServices)
{
    if(m_instance == nullptr)
    {
        m_instance = new SRTMinorServoSocketConfiguration(containerServices);
    }

    return *m_instance;
}

SRTMinorServoSocketConfiguration::SRTMinorServoSocketConfiguration(maci::ContainerServices* containerServices)
{
    AUTO_TRACE("SRTMinorServoSocketConfiguration::SRTMinorServoSocketConfiguration()");

    IRA::CString _ip_address;
    if(!IRA::CIRATools::getDBValue(containerServices, "IPAddress", _ip_address, CONFIG_DOMAIN, CONFIG_DIRNAME))
    {
        _EXCPT(ComponentErrors::CDBAccessExImpl, impl, "SRTMinorServoSocketConfiguration::SRTMinorServoSocketConfiguration()");
        impl.setFieldName("IPAddress");
        throw impl.getComponentErrorsEx();
    }
    m_ip_address = (std::string)_ip_address;

    DWORD port;
    if(!IRA::CIRATools::getDBValue(containerServices, "Port", port, CONFIG_DOMAIN, CONFIG_DIRNAME))
    {
        _EXCPT(ComponentErrors::CDBAccessExImpl, impl, "SRTMinorServoSocketConfiguration::SRTMinorServoSocketConfiguration()");
        impl.setFieldName("Port");
        throw impl.getComponentErrorsEx();
    }
    else
    {
        m_port = port;
    }

    if(!IRA::CIRATools::getDBValue(containerServices, "SocketTimeout", m_timeout, CONFIG_DOMAIN, CONFIG_DIRNAME))
    {
        _EXCPT(ComponentErrors::CDBAccessExImpl, impl, "SRTMinorServoSocketConfiguration::SRTMinorServoSocketConfiguration()");
        impl.setFieldName("SocketTimeout");
        throw impl.getComponentErrorsEx();
    }
}

SRTMinorServoSocketConfiguration::~SRTMinorServoSocketConfiguration()
{
    AUTO_TRACE("SRTMinorServoSocketConfiguration::~SRTMinorServoSocketConfiguration()");

    delete m_instance;
}
