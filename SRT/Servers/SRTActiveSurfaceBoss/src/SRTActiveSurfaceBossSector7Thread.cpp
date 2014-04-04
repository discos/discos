#include "SRTActiveSurfaceBossSector7Thread.h"

CSRTActiveSurfaceBossSector7Thread::CSRTActiveSurfaceBossSector7Thread(const ACE_CString& name,CSRTActiveSurfaceBossCore *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime)
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector7Thread::CSRTActiveSurfaceBossSector7Thread()");
	boss = param;
}

CSRTActiveSurfaceBossSector7Thread::~CSRTActiveSurfaceBossSector7Thread()
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector7Thread::~CSRTActiveSurfaceBossSector7Thread()");
}

void CSRTActiveSurfaceBossSector7Thread::onStart()
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector7Thread::onStart()");
}

void CSRTActiveSurfaceBossSector7Thread::onStop()
{
	 AUTO_TRACE("CSRTActiveSurfaceBossSector7Thread::onStop()");
}

void CSRTActiveSurfaceBossSector7Thread::run()
{
    //IRA::CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();

    try {
        boss->sector7ActiveSurface();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
    }
}
