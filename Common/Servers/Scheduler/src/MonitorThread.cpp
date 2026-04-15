#include "MonitorThread.h"

CMonitorThread::CMonitorThread(const ACE_CString& name, CCore *param, const ACS::TimeInterval& responseTime, const ACS::TimeInterval& sleepTime) :
    ACS::Thread(name, ThreadBase::defaultResponseTime, 0),
    m_core(param),
    m_sleepTime(sleepTime)
{
    AUTO_TRACE("CMonitorThread::CMonitorThread()");
}

CMonitorThread::~CMonitorThread()
{
    AUTO_TRACE("CMonitorThread::~CMonitorThread()");
}

void CMonitorThread::onStart()
{
    AUTO_TRACE("CMonitorThread::onStart()");
    m_nextTime = getTimeStamp();
}

void CMonitorThread::onStop()
{
    AUTO_TRACE("CMonitorThread::onStop()");
}

void CMonitorThread::runLoop()
{
    m_core->publishZMQDictionary();
    m_nextTime += m_sleepTime;
    setSleepTime(ACS::TimeInterval(std::max(long(0), long(m_nextTime - getTimeStamp()))));
}
