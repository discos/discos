#include "NotoActiveSurfaceBossSectorThread.h"

CNotoActiveSurfaceBossSectorThread::CNotoActiveSurfaceBossSectorThread(const ACE_CString& name,CNotoActiveSurfaceBossCore *param,
            const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_boss(param)
{
    m_sector = std::atoi(name.substring(name.length()-1).c_str())-1;
    std::stringstream thread_name;
    thread_name << "CNotoActiveSurfaceBossSector";
    thread_name << m_sector+1;
    thread_name << "Thread";
    m_thread_name = thread_name.str();

    AUTO_TRACE(std::string(m_thread_name + "::CNotoActiveSurfaceBossSectorThread()").c_str());
}

CNotoActiveSurfaceBossSectorThread::~CNotoActiveSurfaceBossSectorThread()
{
    AUTO_TRACE(std::string(m_thread_name + "::~CNotoActiveSurfaceBossSectorThread()").c_str());
}

void CNotoActiveSurfaceBossSectorThread::onStart()
{
    AUTO_TRACE(std::string(m_thread_name + "::onStart()").c_str());

    this->setSleepTime(0); // No sleeping

    TIMEVALUE now;
    CIRATools::getTime(now);
    this->timestart = now.value().value;

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

void CNotoActiveSurfaceBossSectorThread::onStop()
{
    AUTO_TRACE(std::string(m_thread_name + "::onStop()").c_str());

    if (m_usdTable.is_open())
    {
        m_usdTable.close();
        TIMEVALUE now;
        CIRATools::getTime(now);
        double elapsed = (double)(now.value().value - this->timestart) / 10000000;
        ACS_LOG(LM_FULL_INFO,std::string(m_thread_name + "::onStop()").c_str(), (LM_NOTICE, "Total boot time: %.3fs", elapsed));
    }
}

void CNotoActiveSurfaceBossSectorThread::runLoop()
{
    std::string serial_usd, graf, mecc;
    int lanIndex;
    int circleIndex;
    int usdCircleIndex;

    if(m_usdTable >> lanIndex >> circleIndex >> usdCircleIndex >> serial_usd >> graf >> mecc)
    {
        ActiveSurface::USD_var current_usd = ActiveSurface::USD::_nil();

        try
        {
            current_usd = m_boss->m_services->getComponent<ActiveSurface::USD>(serial_usd.c_str());
        }
        catch (maciErrType::CannotGetComponentExImpl& ex)
        {
            _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,std::string(m_thread_name + "::runLoop()").c_str());
            Impl.setComponentName(serial_usd.c_str());
            Impl.log(LM_DEBUG);
        }

        m_boss->lanradius[circleIndex][lanIndex] = m_boss->usd[circleIndex][usdCircleIndex] = current_usd;
        m_boss->usdCounters[m_sector]++;
    }
    else
    {
        m_boss->m_sector[m_sector] = true;
        this->setStopped();
    }
}
