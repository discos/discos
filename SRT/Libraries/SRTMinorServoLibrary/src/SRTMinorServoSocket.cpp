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
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, "SRTMinorServoSocket::getInstance()");
        impl.setReason("Socket not yet initialized. Use getInstance(std::string ip_address, int port) to initialize it and retrieve the object.");
        throw impl;
    }
    return *m_instance;
}

SRTMinorServoSocket::SRTMinorServoSocket(std::string ip_address, int port, double timeout) :
    m_ip_address(ip_address),
    m_port(port),
    m_timeout(static_cast<long>(timeout * 1000000)),
    m_socket_status(NOTREADY)
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
    std::lock_guard<std::recursive_mutex> guard(m_mutex);
    IRA::CError tmpErr;
    Close(tmpErr);
}

void SRTMinorServoSocket::connect()
{
    std::lock_guard<std::recursive_mutex> guard(m_mutex);

    if(isConnected())
    {
        return;
    }

    m_error.Reset();
    IRA::CError tmpErr;
    Close(tmpErr);
    if(Create(m_error, STREAM) == FAIL)
    {
        tmpErr.Reset();
        Close(tmpErr);
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, "SRTMinorServoSocket::SRTMinorServoSocket()");
        impl.setReason("Cannot create the socket.");
        throw impl;
    }

    if(setSockMode(m_error, BLOCKINGTIMEO, m_timeout, m_timeout) != SUCCESS)
    {
        m_socket_status = NOTREADY;
        tmpErr.Reset();
        Close(tmpErr);
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, "SRTMinorServoSocket::SRTMinorServoSocket()");
        impl.setReason("Cannot set the socket to non-blocking.");
        throw impl;
    }

    if(Connect(m_error, m_ip_address.c_str(), m_port) == FAIL)
    {
        m_socket_status = TIMEOUT;
        tmpErr.Reset();
        Close(tmpErr);
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, "SRTMinorServoSocket::SRTMinorServoSocket()");
        impl.setReason("Cannot connect the socket.");
        throw impl;
    }

    m_socket_status = READY;
}

const bool SRTMinorServoSocket::isConnected() const
{
    return m_socket_status == READY ? true : false;
}

SRTMinorServoAnswerMap SRTMinorServoSocket::sendCommand(std::string command, std::optional<std::reference_wrapper<SRTMinorServoAnswerMap>> map)
{
    std::lock_guard<std::recursive_mutex> guard(m_mutex);

    connect();

    size_t sent_bytes = 0;
    while(sent_bytes < command.size())
    {
        int res = Send(m_error, command.data() + sent_bytes, command.size() - sent_bytes);

        if(res > 0)
        {
            sent_bytes += static_cast<size_t>(res);
        }
        else if(res == WOULDBLOCK)
        {
            m_socket_status = TIMEOUT;
            IRA::CError tmpErr;
            Close(tmpErr);
            _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, "SRTMinorServoSocket::sendCommand()");
            impl.setReason("Timeout when sending command.");
            throw impl;
        }
        else
        {
            m_socket_status = NOTREADY;
            IRA::CError tmpErr;
            Close(tmpErr);
            _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, "SRTMinorServoSocket::sendCommand()");
            impl.setReason((const char *)m_error.getFullDescription());
            throw impl;
        }
    }

    std::string answer;

    while(answer.size() < 2 || !(answer.rfind(CLOSER) == answer.size() - CLOSER.size()))
    {
        char buf;

        int res = Receive(m_error, &buf, 1);

        if(res == 1)
        {
            answer += buf;
        }
        else if(res == WOULDBLOCK)
        {
            m_socket_status = TIMEOUT;
            IRA::CError tmpErr;
            Close(tmpErr);
            _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, "SRTMinorServoSocket::sendCommand()");
            impl.setReason("Timeout when receiving answer.");
            throw impl;
        }
        else
        {
            m_socket_status = NOTREADY;
            IRA::CError tmpErr;
            Close(tmpErr);
            _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, "SRTMinorServoSocket::sendCommand()");
            if(res == 0)
            {
                impl.setReason("Connection closed by remote peer.");
            }
            else
            {
                impl.setReason((const char*)m_error.getFullDescription());
            }
            throw impl;
        }
    }

    SRTMinorServoAnswerMap map_answer = SRTMinorServoCommandLibrary::parseAnswer(answer);
    map_answer.put("PLAIN_COMMAND", command);
    if(map)
    {
        map->get() += map_answer;
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
}
