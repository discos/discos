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
    m_sleepTime = ACS::TimeInterval(this->getSleepTime());
    m_nextTime = getTimeStamp();
}

void CActiveSurfaceBossWorkingThread::onStop()
{
    AUTO_TRACE("CActiveSurfaceBossWorkingThread::onStop()");
}

void CActiveSurfaceBossWorkingThread::runLoop()
{
    IRA::CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
    resource->workingActiveSurface();
    resource->publishZMQDictionary(m_nextTime);

    m_nextTime += m_sleepTime;
    ACS::Time now = getTimeStamp();

    if(m_nextTime < now)
    {
        ACS::TimeInterval diff = now - m_nextTime;
        unsigned int missed_ticks = static_cast<unsigned int>(diff / m_sleepTime);

        if((diff % m_sleepTime) > 0)
        {
            missed_ticks++;
        }

        m_nextTime += missed_ticks * m_sleepTime;
    }
    this->setSleepTime(ACS::TimeInterval(m_nextTime - now));
}
