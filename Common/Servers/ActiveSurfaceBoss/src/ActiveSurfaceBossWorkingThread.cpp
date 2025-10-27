#include "ActiveSurfaceBossWorkingThread.h"

CActiveSurfaceBossWorkingThread::CActiveSurfaceBossWorkingThread(const ACE_CString& name, IRA::CSecureArea<CActiveSurfaceBossCore> *core, const ACS::TimeInterval& responseTime, const ACS::TimeInterval& sleepTime) :
    ACS::Thread(name, responseTime, sleepTime),
    m_core(core)
{
    AUTO_TRACE("CActiveSurfaceBossWorkingThread::CActiveSurfaceBossWorkingThread()");
}

CActiveSurfaceBossWorkingThread::~CActiveSurfaceBossWorkingThread()
{
    AUTO_TRACE("CActiveSurfaceBossWorkingThread::~CActiveSurfaceBossWorkingThread()");
}

void CActiveSurfaceBossWorkingThread::onStart()
{
    AUTO_TRACE("CActiveSurfaceBossWorkingThread::onStart()");
    // We divide sleep time by 2 so that we can perform 2 steps:
    // 1 - update the AS
    // 2 - publish the ZMQ message
    // By sleeping half of the original sleeptime we allow the USD statuses to be updated between each movement
    m_sleepTime = ACS::TimeInterval(this->getSleepTime() / 2);
    m_nextTime = getTimeStamp();
    m_status = 0;
}

void CActiveSurfaceBossWorkingThread::onStop()
{
    AUTO_TRACE("CActiveSurfaceBossWorkingThread::onStop()");
}

void CActiveSurfaceBossWorkingThread::runLoop()
{
    IRA::CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();

    switch(m_status)
    {
        case 0:
        {
            resource->workingActiveSurface();
            m_status = 1;
            break;
        }
        case 1:
        {
            resource->publishZMQDictionary(m_nextTime);
            m_status = 0;
            break;
        }
    }

    m_nextTime += m_sleepTime;
    this->setSleepTime(ACS::TimeInterval(std::max(long(0), long(m_nextTime - getTimeStamp()))));
}
