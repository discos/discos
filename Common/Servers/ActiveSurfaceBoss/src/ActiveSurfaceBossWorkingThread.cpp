#include "ActiveSurfaceBossWorkingThread.h"
#include "ActiveSurfaceBossImpl.h"

ActiveSurfaceBossWorkingThread::ActiveSurfaceBossWorkingThread(const ACE_CString& name, ActiveSurfaceBossImpl& impl, const ACS::TimeInterval& responseTime, const ACS::TimeInterval& sleepTime) :
    ACS::Thread(name, responseTime, sleepTime),
    m_impl(impl),
    m_sleepTime(sleepTime),
    m_ready(false),
    m_currentTickIndex(0)
{
    AUTO_TRACE("CActiveSurfaceBossWorkingThread::CActiveSurfaceBossWorkingThread()");
}

ActiveSurfaceBossWorkingThread::~ActiveSurfaceBossWorkingThread()
{
    AUTO_TRACE("CActiveSurfaceBossWorkingThread::~CActiveSurfaceBossWorkingThread()");
}

void ActiveSurfaceBossWorkingThread::onStart()
{
    AUTO_TRACE("CActiveSurfaceBossWorkingThread::onStart()");
}

void ActiveSurfaceBossWorkingThread::onStop()
{
    AUTO_TRACE("CActiveSurfaceBossWorkingThread::onStop()");
}

void ActiveSurfaceBossWorkingThread::runLoop()
{
    if(!m_ready)
    {
        for(auto& [sectorIndex, sector] : m_impl.m_sectors)
        {
            try
            {
                ACSErr::Completion_var completion;
                if(!sector->ready()->get_sync(completion.out()))
                {
                    // Wait another full second
                    this->setSleepTime(10000000);
                    return;
                }
            }
            catch(...)
            {
                return;
            }
        }

        try
        {
            m_impl._setLUT("DEFAULT");
        }
        catch(ACSErr::ACSbaseExImpl& impl)
        {
            impl.log(LM_WARNING);
        }

        m_ready = true;
        m_nextTime = getTimeStamp();
    }

    ACS::Time now = getTimeStamp();

    if(now > m_nextTime && ACS::TimeInterval(now - m_nextTime) >= m_sleepTime)
    {
        while(m_nextTime + m_sleepTime <= now)
        {
            m_nextTime += m_sleepTime;
        }
    }

    m_impl.pollSectorStatus(m_currentTickIndex);

    if(m_currentTickIndex == m_impl.m_maxTickIndex - 1)
    {
        m_impl.publishZMQDictionary(m_nextTime);
    }

    if(m_impl.m_profile != ActiveSurface::AS_PARK && m_impl.m_trackingEnabled)
    {
        m_impl.update(m_currentTickIndex);
        m_impl.m_tracking = Management::MNG_TRUE;
    }
    else
    {
        m_impl.m_tracking = Management::MNG_FALSE;
    }

    ACS::TimeInterval elapsed = getTimeStamp() - now;

    if(elapsed / 10000 >= 120)
    {
        std::cout << "tick elapsed=" << elapsed / 10000 << "\tms" << std::endl;
    }

    m_currentTickIndex++;
    if(m_currentTickIndex >= m_impl.m_maxTickIndex)
    {
        m_currentTickIndex = 0;
    }

    m_nextTime += m_sleepTime;
    now = getTimeStamp();

    this->setSleepTime(std::max<ACS::TimeInterval>(0, m_nextTime - now));
}
