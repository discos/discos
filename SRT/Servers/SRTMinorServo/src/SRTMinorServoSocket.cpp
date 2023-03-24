#include "SRTMinorServoSocket.h"

std::mutex SRTMinorServoSocket::c_mutex;

SRTMinorServoSocket& SRTMinorServoSocket::getInstance(std::string ip_address, int port, double timeout)
{
    std::lock_guard<std::mutex> guard(SRTMinorServoSocket::c_mutex);

    if(m_instance != nullptr)
    {
        if(m_instance->m_address != ip_address && m_instance->m_port != port)
        {
            ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, "SRTMinorServoSocket::getInstance(std::string, int)");
            exImpl.addData("Reason", "Socket already open on '" + m_instance->m_address + ":" + std::to_string(m_instance->m_port) + "' . Use getInstance() (no arguments) to retrieve the object.");
            throw exImpl;
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
        ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, "SRTMinorServoSocket::getInstance()");
        exImpl.addData("Reason", "Socket not yet initialized. Use getInstance(std::string ip_address, int port) to initialize it and retrieve the object.");
        throw exImpl;
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
        ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, "SRTMinorServoSocket::SRTMinorServoSocket()");
        exImpl.addData("Reason", "Cannot create the socket.");
        throw exImpl;
    }

    if(Connect(m_error, ip_address.c_str(), port) == FAIL)
    {
        m_socket_status = TOUT;
        Close(m_error);
        ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, "SRTMinorServoSocket::SRTMinorServoSocket()");
        exImpl.addData("Reason", "Cannot connect the socket.");
        throw exImpl;
    }

    if(setSockMode(m_error, NONBLOCKING) != SUCCESS)
    {
        m_socket_status = NOTRDY;
        Close(m_error);
        ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, "SRTMinorServoSocket::SRTMinorServoSocket()");
        exImpl.addData("Reason", "Cannot set the socket to non-blocking.");
        throw exImpl;
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

SRTMinorServoAnswerMap SRTMinorServoSocket::sendCommand(std::string command)
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
            ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, "SRTMinorServoSocket::sendCommand()");
            exImpl.addData("Reason", "Timeout when sending command.");
            throw exImpl;
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
            ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, "SRTMinorServoSocket::sendCommand()");
            exImpl.addData("Reason", "Timeout when receiving answer.");
            throw exImpl;
        }
    }

    return SRTMinorServoCommandLibrary::parseAnswer(answer);
}
