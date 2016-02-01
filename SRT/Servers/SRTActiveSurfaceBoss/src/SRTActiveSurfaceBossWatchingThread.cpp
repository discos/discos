#include "SRTActiveSurfaceBossWatchingThread.h"

CSRTActiveSurfaceBossWatchingThread::CSRTActiveSurfaceBossWatchingThread(const ACE_CString& name,IRA::CSecureArea<CSRTActiveSurfaceBossCore>  *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_core(param)
{
	AUTO_TRACE("CSRTActiveSurfaceBossWatchingThread::CSRTActiveSurfaceBossWatchingThread()");
}

CSRTActiveSurfaceBossWatchingThread::~CSRTActiveSurfaceBossWatchingThread()
{
	AUTO_TRACE("CSRTActiveSurfaceBossWatchingThread::~CSRTActiveSurfaceBossWatchingThread()");
}

void CSRTActiveSurfaceBossWatchingThread::onStart()
{
	AUTO_TRACE("CSRTActiveSurfaceBossWatchingThread::onStart()");
}

void CSRTActiveSurfaceBossWatchingThread::onStop()
{
	 AUTO_TRACE("CSRTActiveSurfaceBossWatchingThread::onStop()");
}

void CSRTActiveSurfaceBossWatchingThread::runLoop()
{
    IRA::CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();

    try {
        resource->watchingActiveSurfaceStatus();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
    }
}
