#include "ActiveSurfaceBossWatchingThread.h"

CActiveSurfaceBossWatchingThread::CActiveSurfaceBossWatchingThread(const ACE_CString& name,IRA::CSecureArea<CActiveSurfaceBossCore>  *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_core(param)
{
	AUTO_TRACE("CActiveSurfaceBossWatchingThread::CActiveSurfaceBossWatchingThread()");
}

CActiveSurfaceBossWatchingThread::~CActiveSurfaceBossWatchingThread()
{
	AUTO_TRACE("CActiveSurfaceBossWatchingThread::~CActiveSurfaceBossWatchingThread()");
}

void CActiveSurfaceBossWatchingThread::onStart()
{
	AUTO_TRACE("CActiveSurfaceBossWatchingThread::onStart()");
}

void CActiveSurfaceBossWatchingThread::onStop()
{
	 AUTO_TRACE("CActiveSurfaceBossWatchingThread::onStop()");
}

void CActiveSurfaceBossWatchingThread::runLoop()
{
    IRA::CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();

    try {
        resource->watchingActiveSurfaceStatus();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
    }
}
