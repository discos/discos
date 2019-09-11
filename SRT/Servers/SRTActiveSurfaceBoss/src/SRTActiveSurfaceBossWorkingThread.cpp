#include "SRTActiveSurfaceBossWorkingThread.h"

CSRTActiveSurfaceBossWorkingThread::CSRTActiveSurfaceBossWorkingThread(const ACE_CString& name,IRA::CSecureArea<CSRTActiveSurfaceBossCore>  *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_core(param)
{
	AUTO_TRACE("CSRTActiveSurfaceBossWorkingThread::CSRTActiveSurfaceBossWorkingThread()");
}

CSRTActiveSurfaceBossWorkingThread::~CSRTActiveSurfaceBossWorkingThread()
{
	AUTO_TRACE("CSRTActiveSurfaceBossWorkingThread::~CSRTActiveSurfaceBossWorkingThread()");
}

void CSRTActiveSurfaceBossWorkingThread::onStart()
{
	AUTO_TRACE("CSRTActiveSurfaceBossWorkingThread::onStart()");
}

void CSRTActiveSurfaceBossWorkingThread::onStop()
{
	 AUTO_TRACE("CSRTActiveSurfaceBossWorkingThread::onStop()");
}

void CSRTActiveSurfaceBossWorkingThread::runLoop()
{
    IRA::CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();

    try {
        resource->workingActiveSurface();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
    }
}
