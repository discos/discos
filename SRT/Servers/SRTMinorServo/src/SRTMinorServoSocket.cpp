#include "SRTMinorServoSocket.h"
#include <iostream>

SRTMinorServoSocket& SRTMinorServoSocket::getInstance(std::string ip_address, int port)
{
    static SRTMinorServoSocket instance(ip_address, port);
    return instance;
}

SRTMinorServoSocket::SRTMinorServoSocket(std::string ip_address, int port)
{
    if(Create(m_Error, STREAM) == FAIL)
    {
        std::cout << "Cannot create" << std::endl;
        Close(m_Error);
    }

    if(Connect(m_Error, ip_address.c_str(), port) == FAIL)
    {
        std::cout << "Cannot connect" << std::endl;
        m_soStat = TOUT;
        Close(m_Error);
    }
}

SRTMinorServoSocket::~SRTMinorServoSocket()
{
    Close(m_Error); 
}

std::string SRTMinorServoSocket::sendCommand(std::string command)
{
    std::lock_guard<std::mutex> guard(m_Mutex);
    Send(m_Error, command.c_str(), command.size());
    char msg[MAXSIZE];
    Receive(m_Error, &msg, MAXSIZE);
    return std::string(msg);
}
