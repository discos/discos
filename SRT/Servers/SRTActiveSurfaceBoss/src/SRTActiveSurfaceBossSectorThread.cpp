#include "SRTActiveSurfaceBossSectorThread.h"

CSRTActiveSurfaceBossSectorThread::CSRTActiveSurfaceBossSectorThread(const ACE_CString& name,CSRTActiveSurfaceBossCore *param, 
            const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime)
{
    AUTO_TRACE("CSRTActiveSurfaceBossSectorThread::CSRTActiveSurfaceBossSectorThread()");
    boss = param;
    m_sector = -1;
    m_index = 0;
    m_usd_count = 0;
    m_ready = false;
}

CSRTActiveSurfaceBossSectorThread::~CSRTActiveSurfaceBossSectorThread()
{
    AUTO_TRACE("CSRTActiveSurfaceBossSectorThread::~CSRTActiveSurfaceBossSectorThread()");
}

void CSRTActiveSurfaceBossSectorThread::onStart()
{
    AUTO_TRACE("CSRTActiveSurfaceBossSectorThread::onStart()");
}

void CSRTActiveSurfaceBossSectorThread::onStop()
{
    AUTO_TRACE("CSRTActiveSurfaceBossSectorThread::onStop()");
}

void CSRTActiveSurfaceBossSectorThread::prepare(int sector)
{
    if(m_ready) return;
    m_ready = true;

    //for setup phase we set the sleep time to 1ms in order to be fast
    this->setSleepTime(10000);

    m_sector = sector;

    printf("sector%d start\n", m_sector + 1);
    std::stringstream value;
    value << CDBPATH;
    value << "alma/AS/tab_convUSD_S";
    value << m_sector + 1;
    value << ".txt";

    ifstream usdTable(value.str().c_str());
    if (!usdTable)
    {
        ACS_SHORT_LOG ((LM_INFO, "File %s not found", value.str().c_str()));
        ACS::ThreadBase::exit();
    }

    std::string serial_usd, graf, mecc;
    int lanIndex, circleIndex, usdCircleIndex;
    while(usdTable >> lanIndex >> circleIndex >> usdCircleIndex >> serial_usd >> graf >> mecc)
    {
        m_usd_count++;
        m_lanIndexes.push_back(lanIndex);
        m_circleIndexes.push_back(circleIndex);
        m_usdCircleIndexes.push_back(usdCircleIndex);
        m_serial_usds.push_back(serial_usd);
    }
    usdTable.close();
}

void CSRTActiveSurfaceBossSectorThread::runLoop()
{
    try
    {
        boss->checkUSD(m_sector, m_lanIndexes[m_index], m_circleIndexes[m_index], m_usdCircleIndexes[m_index], m_serial_usds[m_index]);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
    }

    m_index++;
    if(m_index == m_usd_count)
    {
        this->setSleepTime(1000000); //setup phase ended, set the sleep time at 100ms
        boss->sectorSetupCompleted(m_sector);
        m_index = 0;
    }
}
