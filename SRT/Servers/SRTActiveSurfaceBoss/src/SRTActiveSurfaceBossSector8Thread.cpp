#include "SRTActiveSurfaceBossSector8Thread.h"

CSRTActiveSurfaceBossSector8Thread::CSRTActiveSurfaceBossSector8Thread(const ACE_CString& name,CSRTActiveSurfaceBossCore *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime)
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector8Thread::CSRTActiveSurfaceBossSector8Thread()");
	boss = param;
}

CSRTActiveSurfaceBossSector8Thread::~CSRTActiveSurfaceBossSector8Thread()
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector8Thread::~CSRTActiveSurfaceBossSector8Thread()");
}

void CSRTActiveSurfaceBossSector8Thread::onStart()
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector8Thread::onStart()");
}

void CSRTActiveSurfaceBossSector8Thread::onStop()
{
	 AUTO_TRACE("CSRTActiveSurfaceBossSector8Thread::onStop()");
}

void CSRTActiveSurfaceBossSector8Thread::run()
{
    //IRA::CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();

    try {
        boss->sector8ActiveSurface();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
    }
}
