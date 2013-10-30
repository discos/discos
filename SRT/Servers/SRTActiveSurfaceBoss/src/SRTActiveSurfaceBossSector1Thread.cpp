#include "SRTActiveSurfaceBossSector1Thread.h"

CSRTActiveSurfaceBossSector1Thread::CSRTActiveSurfaceBossSector1Thread(const ACE_CString& name,CSRTActiveSurfaceBossCore *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime)
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector1Thread::CSRTActiveSurfaceBossSector1Thread()");
	boss = param;
}

CSRTActiveSurfaceBossSector1Thread::~CSRTActiveSurfaceBossSector1Thread()
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector1Thread::~CSRTActiveSurfaceBossSector1Thread()");
}

void CSRTActiveSurfaceBossSector1Thread::onStart()
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector1Thread::onStart()");
}

void CSRTActiveSurfaceBossSector1Thread::onStop()
{
	 AUTO_TRACE("CSRTActiveSurfaceBossSector1Thread::onStop()");
}

void CSRTActiveSurfaceBossSector1Thread::run()
{
    //IRA::CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();

    try {
        boss->sector1ActiveSurface();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
    }
}
