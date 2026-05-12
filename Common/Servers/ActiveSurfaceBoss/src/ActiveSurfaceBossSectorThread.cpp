#include "ActiveSurfaceBossSectorThread.h"

CActiveSurfaceBossSectorThread::CActiveSurfaceBossSectorThread(const ACE_CString& name,CActiveSurfaceBossCore *param,
            const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_boss(param), m_state(0), m_containerUp(false)
{
    m_sector = std::atoi(name.substring(name.length()-1).c_str())-1;
    std::stringstream thread_name;
    thread_name <<  "CActiveSurfaceBossSector";
    thread_name << m_sector+1;
    thread_name << "Thread";
    m_thread_name = thread_name.str();

    AUTO_TRACE(std::string(m_thread_name + "::CActiveSurfaceBossSectorThread()").c_str());
}

CActiveSurfaceBossSectorThread::~CActiveSurfaceBossSectorThread()
{
    AUTO_TRACE(std::string(m_thread_name + "::~CActiveSurfaceBossSectorThread()").c_str());
}

void CActiveSurfaceBossSectorThread::onStart()
{
    AUTO_TRACE(std::string(m_thread_name + "::onStart()").c_str());

    m_state = 0;
    m_currentLanIndex = 0;

    this->setSleepTime(10000000);
    this->timestart = getTimeStamp();

    std::stringstream table;
    table << CDBPATH;
    table << "alma/AS/tab_convUSD_S";
    table << m_sector+1;
    table << ".txt";

    m_usdTable.open(table.str().c_str());
    if (!m_usdTable)
    {
        ACS_LOG(LM_SOURCE_INFO,std::string(m_thread_name + "::onStart()").c_str(), (LM_ERROR, "Table file %s not found!", table.str().c_str()));
        this->setStopped();
    }
}

void CActiveSurfaceBossSectorThread::onStop()
{
    AUTO_TRACE(std::string(m_thread_name + "::onStop()").c_str());

    if (m_usdTable.is_open())
    {
        m_usdTable.close();
    }

    double elapsed = (double)(getTimeStamp() - this->timestart) / 10000000;
    ACS_LOG(LM_FULL_INFO,std::string(m_thread_name + "::onStop()").c_str(), (LM_NOTICE, "Total boot time: %.3fs", elapsed));
}

void CActiveSurfaceBossSectorThread::runLoop()
{
    switch(m_state)
    {
        case 0:
        {
            try
            {
                m_boss->lan[m_sector][m_currentLanIndex].load();
                m_currentLanIndex++;
                // LAN loaded correctly, it means that the LAN container is up, we can ignore other LANs failures
                this->setSleepTime(0);
                if(!m_containerUp)
                {
                    m_containerUp = true;
                }
            }
            catch(...)
            {
                if(m_containerUp)
                {
                    m_currentLanIndex++;
                }
            }

            if(m_currentLanIndex == m_boss->LANs_per_sector)
            {
                m_state = 1;
                this->setSleepTime(0);
            }

            break;
        }
        case 1:
        {
            std::string serial_usd, graf, mecc;
            int lanIndex;
            int circleIndex;
            int usdCircleIndex;

            if(m_usdTable >> lanIndex >> circleIndex >> usdCircleIndex >> serial_usd >> graf >> mecc)
            {
                ActiveSurface::USD_proxy proxy;
                proxy.setContainerServices(m_boss->m_services);
                proxy.setComponentName(serial_usd.c_str());
                int normalizedLanIndex = (lanIndex - 1) % m_boss->LANs_per_sector;
                m_boss->usdMap.add(circleIndex, usdCircleIndex, lanIndex, m_sector, normalizedLanIndex, serial_usd, proxy);
                try
                {
                    proxy.load();
                }
                catch(...) {}
                m_boss->usdCounters[m_sector]++;
            }
            else
            {
                this->setStopped();
            }
        }
    }
}
