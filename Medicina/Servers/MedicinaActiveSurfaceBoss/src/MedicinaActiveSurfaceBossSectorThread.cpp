#include "MedicinaActiveSurfaceBossSectorThread.h"

CMedicinaActiveSurfaceBossSectorThread::CMedicinaActiveSurfaceBossSectorThread(const ACE_CString& name,CMedicinaActiveSurfaceBossCore *param, 
            const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime)
{
    AUTO_TRACE("CMedicinaActiveSurfaceBossSectorThread::CMedicinaActiveSurfaceBossSectorThread()");
    boss = param;
    m_sector = -1;
}

CMedicinaActiveSurfaceBossSectorThread::~CMedicinaActiveSurfaceBossSectorThread()
{
    AUTO_TRACE("CMedicinaActiveSurfaceBossSectorThread::~CMedicinaActiveSurfaceBossSectorThread()");
}

void CMedicinaActiveSurfaceBossSectorThread::onStart()
{
    AUTO_TRACE("CMedicinaActiveSurfaceBossSectorThread::onStart()");
}

void CMedicinaActiveSurfaceBossSectorThread::onStop()
{
    AUTO_TRACE("CMedicinaActiveSurfaceBossSectorThread::onStop()");
}

void CMedicinaActiveSurfaceBossSectorThread::setSector(int sector)
{
    m_sector = sector;
}

void CMedicinaActiveSurfaceBossSectorThread::run()
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
