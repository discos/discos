#include "NotoActiveSurfaceBossWatchingThread.h"

CNotoActiveSurfaceBossWatchingThread::CNotoActiveSurfaceBossWatchingThread(const ACE_CString& name,IRA::CSecureArea<CNotoActiveSurfaceBossCore>  *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_core(param)
{
	AUTO_TRACE("CNotoActiveSurfaceBossWatchingThread::CNotoActiveSurfaceBossWatchingThread()");
}

CNotoActiveSurfaceBossWatchingThread::~CNotoActiveSurfaceBossWatchingThread()
{
	AUTO_TRACE("CNotoActiveSurfaceBossWatchingThread::~CNotoActiveSurfaceBossWatchingThread()");
}

void CNotoActiveSurfaceBossWatchingThread::onStart()
{
	AUTO_TRACE("CNotoActiveSurfaceBossWatchingThread::onStart()");
}

void CNotoActiveSurfaceBossWatchingThread::onStop()
{
	 AUTO_TRACE("CNotoActiveSurfaceBossWatchingThread::onStop()");
}

void CNotoActiveSurfaceBossWatchingThread::runLoop()
{
    IRA::CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();

    try {
        resource->watchingActiveSurfaceStatus();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
    }
}
