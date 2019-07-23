#include "SRTActiveSurfaceBossWorkingThread.h"

CSRTActiveSurfaceBossWorkingThread::CSRTActiveSurfaceBossWorkingThread(const ACE_CString& name,CSRTActiveSurfaceBossCore *param,
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), boss(param)
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
    try {
        boss->workingActiveSurface();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
    }
}
