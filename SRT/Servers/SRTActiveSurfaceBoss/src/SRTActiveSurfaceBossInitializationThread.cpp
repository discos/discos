#include "SRTActiveSurfaceBossInitializationThread.h"

CSRTActiveSurfaceBossInitializationThread::CSRTActiveSurfaceBossInitializationThread(const ACE_CString& name, CSRTActiveSurfaceBossCore *param,
            const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_boss(param)
{
    m_thread_name = "SRTActiveSurfaceBossInitializationThread";

    this->setSleepTime(1000000);

    AUTO_TRACE(std::string(m_thread_name + "::CSRTActiveSurfaceBossInitializationThread()").c_str());
}

CSRTActiveSurfaceBossInitializationThread::~CSRTActiveSurfaceBossInitializationThread()
{
    for(unsigned int i = 0; i < m_sectorThread.size(); i++)
    {
        if(m_sectorThread[i] != NULL)
        {
            m_sectorThread[i]->suspend();
            m_boss->m_services->getThreadManager()->destroy(m_sectorThread[i]);
        }
    }

    AUTO_TRACE(std::string(m_thread_name + "::~CSRTActiveSurfaceBossInitializationThread()").c_str());
}

void CSRTActiveSurfaceBossInitializationThread::onStart()
{
    AUTO_TRACE(std::string(m_thread_name + "::onStart()").c_str());

    for(int sector = 0; sector < SECTORS; sector++)
    {
        std::stringstream threadName;
        threadName << "SRTACTIVESURFACEBOSSSECTOR";
        threadName << sector+1;
        try
        {
            CSRTActiveSurfaceBossSectorThread* sectorThread = m_boss->m_services->getThreadManager()->create<CSRTActiveSurfaceBossSectorThread,CSRTActiveSurfaceBossCore *> (threadName.str().c_str(), m_boss);
            sectorThread->setSleepTime(SECTORTIME);
            sectorThread->resume();
            m_sectorThread.push_back(sectorThread);
        }
        catch (acsthreadErrType::acsthreadErrTypeExImpl& ex)
        {
            _ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"SRTActiveSurfaceBossInitializationThread::onStart()");
            throw _dummy;
        }
        catch (...)
        {
            _THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"SRTActiveSurfaceBossInitializationThread::onStart()");
        }
    }

    TIMEVALUE now;
    CIRATools::getTime(now);
    this->timestart = now.value().value;
}

void CSRTActiveSurfaceBossInitializationThread::onStop()
{
    m_boss->m_initialized = true;

    ACS_LOG(LM_FULL_INFO, std::string(m_thread_name + "::onStop()").c_str(), (LM_NOTICE, "ACTIVE SURFACE INITIALIZED"));

    AUTO_TRACE(std::string(m_thread_name + "::onStop()").c_str());
}

void CSRTActiveSurfaceBossInitializationThread::runLoop()
{
    for(unsigned int i = 0; i < m_sectorThread.size(); i++)
    {
        if(m_sectorThread[i]->isAlive())
        {
            return;
        }
    }

    // Set CDB calibrate parameter to 0 where needed
    for(int circle = 1; circle <= CIRCLES; circle++)
    {
        for(int actuator = 1; actuator <= m_boss->actuatorsInCircle[circle]; actuator++)
        {
            if(!CORBA::is_nil(m_boss->usd[circle][actuator]))
            {
                int usdStatus = 0;
                m_boss->usd[circle][actuator]->getStatus(usdStatus);

                if(!(usdStatus & CAL))
                {
                    CIRATools::setDBValue(m_boss->m_services, "calibrate", (const long)0, "alma/", m_boss->usd[circle][actuator]->name());
                }
            }
        }
    }

    this->setStopped();
}
