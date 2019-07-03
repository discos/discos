#include "SRTActiveSurfaceBossSectorThread.h"

CSRTActiveSurfaceBossSectorThread::CSRTActiveSurfaceBossSectorThread(const ACE_CString& name,CSRTActiveSurfaceBossCore *param, 
            const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime)
{
    AUTO_TRACE("CSRTActiveSurfaceBossSectorThread::CSRTActiveSurfaceBossSectorThread()");
    boss = param;
    m_sector = -1;
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

void CSRTActiveSurfaceBossSectorThread::setSector(int sector)
{
    m_sector = sector;
}

void CSRTActiveSurfaceBossSectorThread::run()
{
    try
    {
        boss->sectorActiveSurface(m_sector);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex)
    {
        ex.log(LM_DEBUG);
    }
}
