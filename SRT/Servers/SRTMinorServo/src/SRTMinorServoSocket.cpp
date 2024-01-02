#include "SRTMinorServoSocket.h"

std::mutex SRTMinorServoSocket::c_mutex;

SRTMinorServoSocket& SRTMinorServoSocket::getInstance(std::string ip_address, int port, double timeout)
{
    std::lock_guard<std::mutex> guard(SRTMinorServoSocket::c_mutex);

    if(m_instance != nullptr)
    {
        if(m_instance->m_address != ip_address && m_instance->m_port != port)
        {
            _EXCPT(ComponentErrors::SocketErrorExImpl, impl, "SRTMinorServoSocket::getInstance(std::string, int)");
            impl.addData("Reason", "Socket already open on '" + m_instance->m_address + ":" + std::to_string(m_instance->m_port) + "' . Use getInstance() (no arguments) to retrieve the object.");
            throw impl;
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
        _EXCPT(ComponentErrors::SocketErrorExImpl, impl, "SRTMinorServoSocket::getInstance()");
        impl.addData("Reason", "Socket not yet initialized. Use getInstance(std::string ip_address, int port) to initialize it and retrieve the object.");
        throw impl;
    }
    return *m_instance;
}

void SRTMinorServoSocket::destroyInstance()
{
    if(m_instance != nullptr)
    {
        delete m_instance;
        m_instance = nullptr;
    }
}

SRTMinorServoSocket::SRTMinorServoSocket(std::string ip_address, int port, double timeout)
{
    if(Create(m_error, STREAM) == FAIL)
    {
        Close(m_error);
        _EXCPT(ComponentErrors::SocketErrorExImpl, impl, "SRTMinorServoSocket::SRTMinorServoSocket()");
        impl.addData("Reason", "Cannot create the socket.");
        throw impl;
    }

    if(Connect(m_error, ip_address.c_str(), port) == FAIL)
    {
        m_socket_status = TOUT;
        Close(m_error);
        _EXCPT(ComponentErrors::SocketErrorExImpl, impl, "SRTMinorServoSocket::SRTMinorServoSocket()");
        impl.addData("Reason", "Cannot connect the socket.");
        throw impl;
    }

    if(setSockMode(m_error, NONBLOCKING) != SUCCESS)
    {
        m_socket_status = NOTRDY;
        Close(m_error);
        _EXCPT(ComponentErrors::SocketErrorExImpl, impl, "SRTMinorServoSocket::SRTMinorServoSocket()");
        impl.addData("Reason", "Cannot set the socket to non-blocking.");
        throw impl;
    }

    m_address = ip_address;
    m_port = port;
    m_timeout = timeout;
}

SRTMinorServoSocket::~SRTMinorServoSocket()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    Close(m_error);
}

SRTMinorServoAnswerMap SRTMinorServoSocket::sendCommand(std::string command, std::optional<std::reference_wrapper<SRTMinorServoAnswerMap>> map)
{
    std::lock_guard<std::mutex> guard(m_mutex);

    double start_time = CIRATools::getUNIXEpoch();
    size_t sent_bytes = 0;

    while(sent_bytes < command.size())
    {
        size_t sent_now = Send(m_error, command.substr(sent_bytes, command.size() - sent_bytes).c_str(), command.size() - sent_bytes);
        sent_bytes += sent_now;

        if(sent_now > 0)
        {
            // Reset the timer
            start_time = CIRATools::getUNIXEpoch();
        }
        else if(CIRATools::getUNIXEpoch() - start_time >= m_timeout)
        {
            m_socket_status = TOUT;
            Close(m_error);
            _EXCPT(ComponentErrors::SocketErrorExImpl, impl, "SRTMinorServoSocket::sendCommand()");
            impl.addData("Reason", "Timeout when sending command.");
            throw impl;
        }
    }

    start_time = CIRATools::getUNIXEpoch();
    std::string answer;

    while(answer.size() < 2 || !(answer.rfind(CLOSER) == answer.size() - CLOSER.size()))
    {
        char buf;
        if(Receive(m_error, &buf, 1) == 1)
        {
            answer += buf;

            // Reset the timer
            start_time = CIRATools::getUNIXEpoch();
        }
        else if(CIRATools::getUNIXEpoch() - start_time >= m_timeout)
        {
            m_socket_status = TOUT;
            Close(m_error);
            _EXCPT(ComponentErrors::SocketErrorExImpl, impl, "SRTMinorServoSocket::sendCommand()");
            impl.addData("Reason", "Timeout when receiving answer.");
            throw impl;
        }
    }
    SRTMinorServoAnswerMap map_answer = SRTMinorServoCommandLibrary::parseAnswer(answer);
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
        throw impl;
    }
    m_ip_address = (std::string)_ip_address;

    DWORD port;
    if(!IRA::CIRATools::getDBValue(containerServices, "Port", port, CONFIG_DOMAIN, CONFIG_DIRNAME))
    {
        _EXCPT(ComponentErrors::CDBAccessExImpl, impl, "SRTMinorServoSocketConfiguration::SRTMinorServoSocketConfiguration()");
        impl.setFieldName("Port");
        throw impl;
    }
    else
    {
        m_port = port;
    }

    if(!IRA::CIRATools::getDBValue(containerServices, "SocketTimeout", m_timeout, CONFIG_DOMAIN, CONFIG_DIRNAME))
    {
        _EXCPT(ComponentErrors::CDBAccessExImpl, impl, "SRTMinorServoSocketConfiguration::SRTMinorServoSocketConfiguration()");
        impl.setFieldName("SocketTimeout");
        throw impl;
    }
}

SRTMinorServoSocketConfiguration::~SRTMinorServoSocketConfiguration()
{
    AUTO_TRACE("SRTMinorServoSocketConfiguration::~SRTMinorServoSocketConfiguration()");

    delete m_instance;
}
