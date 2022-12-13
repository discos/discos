#ifndef __SRTMINORSERVOSOCKET_H__
#define __SRTMINORSERVOSOCKET_H__

#include <IRA>
#include <mutex>

#define MAXSIZE 1024

using namespace IRA;

class SRTMinorServoSocket: public IRA::CSocket {

public:
    static SRTMinorServoSocket& getInstance(std::string ip_address, int port);

    std::string sendCommand(std::string command);

    SRTMinorServoSocket(SRTMinorServoSocket const&) = delete;
    void operator=(SRTMinorServoSocket const&) = delete;
protected:
    enum soStat { READY, NOTRDY, BROKEN, TOUT, BUSY } m_soStat;   
private:
    SRTMinorServoSocket(std::string ip_address, int port);
    ~SRTMinorServoSocket();

    std::mutex m_Mutex;
    IRA::CError m_Error;
};

#endif
