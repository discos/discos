#include "SRTActiveSurfaceBossSector2Thread.h"

CSRTActiveSurfaceBossSector2Thread::CSRTActiveSurfaceBossSector2Thread(const ACE_CString& name,CSRTActiveSurfaceBossCore *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime)
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector2Thread::CSRTActiveSurfaceBossSector2Thread()");
	boss = param;
}

CSRTActiveSurfaceBossSector2Thread::~CSRTActiveSurfaceBossSector2Thread()
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector2Thread::~CSRTActiveSurfaceBossSector2Thread()");
}

void CSRTActiveSurfaceBossSector2Thread::onStart()
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector2Thread::onStart()");
}

void CSRTActiveSurfaceBossSector2Thread::onStop()
{
	 AUTO_TRACE("CSRTActiveSurfaceBossSector2Thread::onStop()");
}

void CSRTActiveSurfaceBossSector2Thread::run()
{
    //IRA::CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();

    try {
        boss->sector2ActiveSurface();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
    }
}
