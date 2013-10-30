#include "SRTActiveSurfaceBossSector5Thread.h"

CSRTActiveSurfaceBossSector5Thread::CSRTActiveSurfaceBossSector5Thread(const ACE_CString& name,CSRTActiveSurfaceBossCore *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime)
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector5Thread::CSRTActiveSurfaceBossSector5Thread()");
	boss = param;
}

CSRTActiveSurfaceBossSector5Thread::~CSRTActiveSurfaceBossSector5Thread()
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector5Thread::~CSRTActiveSurfaceBossSector5Thread()");
}

void CSRTActiveSurfaceBossSector5Thread::onStart()
{
	AUTO_TRACE("CSRTActiveSurfaceBossSector5Thread::onStart()");
}

void CSRTActiveSurfaceBossSector5Thread::onStop()
{
	 AUTO_TRACE("CSRTActiveSurfaceBossSector5Thread::onStop()");
}

void CSRTActiveSurfaceBossSector5Thread::run()
{
    //IRA::CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();

    try {
        boss->sector5ActiveSurface();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
    }
}
