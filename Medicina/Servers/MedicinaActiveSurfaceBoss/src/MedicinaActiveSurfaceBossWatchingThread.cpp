#include "MedicinaActiveSurfaceBossWatchingThread.h"

CMedicinaActiveSurfaceBossWatchingThread::CMedicinaActiveSurfaceBossWatchingThread(const ACE_CString& name,IRA::CSecureArea<CMedicinaActiveSurfaceBossCore>  *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_core(param)
{
	AUTO_TRACE("CMedicinaActiveSurfaceBossWatchingThread::CMedicinaActiveSurfaceBossWatchingThread()");
}

CMedicinaActiveSurfaceBossWatchingThread::~CMedicinaActiveSurfaceBossWatchingThread()
{
	AUTO_TRACE("CMedicinaActiveSurfaceBossWatchingThread::~CMedicinaActiveSurfaceBossWatchingThread()");
}

void CMedicinaActiveSurfaceBossWatchingThread::onStart()
{
	AUTO_TRACE("CMedicinaActiveSurfaceBossWatchingThread::onStart()");
}

void CMedicinaActiveSurfaceBossWatchingThread::onStop()
{
	 AUTO_TRACE("CMedicinaActiveSurfaceBossWatchingThread::onStop()");
}

void CMedicinaActiveSurfaceBossWatchingThread::runLoop()
{
    IRA::CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();

    try {
        resource->watchingActiveSurfaceStatus();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
    }
}
