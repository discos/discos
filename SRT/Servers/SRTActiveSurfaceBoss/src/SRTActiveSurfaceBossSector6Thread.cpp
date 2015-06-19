#include "SRTActiveSurfaceBossSector6Thread.h"

CSRTActiveSurfaceBossSector6Thread::CSRTActiveSurfaceBossSector6Thread(const ACE_CString& name,CSRTActiveSurfaceBossCore *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime)
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector6Thread::CSRTActiveSurfaceBossSector6Thread()");
	boss = param;
}

CSRTActiveSurfaceBossSector6Thread::~CSRTActiveSurfaceBossSector6Thread()
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector6Thread::~CSRTActiveSurfaceBossSector6Thread()");
}

void CSRTActiveSurfaceBossSector6Thread::onStart()
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector6Thread::onStart()");
}

void CSRTActiveSurfaceBossSector6Thread::onStop()
{
	 AUTO_TRACE("CSRTActiveSurfaceBossSector6Thread::onStop()");
}

void CSRTActiveSurfaceBossSector6Thread::run()
{
    //IRA::CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();

    try {
        boss->sector6ActiveSurface();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
    }
}
