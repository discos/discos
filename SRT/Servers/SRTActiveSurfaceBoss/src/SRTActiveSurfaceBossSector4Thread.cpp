#include "SRTActiveSurfaceBossSector4Thread.h"

CSRTActiveSurfaceBossSector4Thread::CSRTActiveSurfaceBossSector4Thread(const ACE_CString& name,CSRTActiveSurfaceBossCore *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime)
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector4Thread::CSRTActiveSurfaceBossSector4Thread()");
	boss = param;
}

CSRTActiveSurfaceBossSector4Thread::~CSRTActiveSurfaceBossSector4Thread()
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector4Thread::~CSRTActiveSurfaceBossSector4Thread()");
}

void CSRTActiveSurfaceBossSector4Thread::onStart()
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector4Thread::onStart()");
}

void CSRTActiveSurfaceBossSector4Thread::onStop()
{
	 AUTO_TRACE("CSRTActiveSurfaceBossSector4Thread::onStop()");
}

void CSRTActiveSurfaceBossSector4Thread::run()
{
    //IRA::CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();

    try {
        boss->sector4ActiveSurface();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
    }
}
