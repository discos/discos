#include "SRTActiveSurfaceBossSector3Thread.h"

CSRTActiveSurfaceBossSector3Thread::CSRTActiveSurfaceBossSector3Thread(const ACE_CString& name,CSRTActiveSurfaceBossCore *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime)
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector3Thread::CSRTActiveSurfaceBossSector3Thread()");
	boss = param;
}

CSRTActiveSurfaceBossSector3Thread::~CSRTActiveSurfaceBossSector3Thread()
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector3Thread::~CSRTActiveSurfaceBossSector3Thread()");
}

void CSRTActiveSurfaceBossSector3Thread::onStart()
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector3Thread::onStart()");
}

void CSRTActiveSurfaceBossSector3Thread::onStop()
{
	 AUTO_TRACE("CSRTActiveSurfaceBossSector3Thread::onStop()");
}

void CSRTActiveSurfaceBossSector3Thread::run()
{
    //IRA::CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();

    try {
        boss->sector3ActiveSurface();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
    }
}
