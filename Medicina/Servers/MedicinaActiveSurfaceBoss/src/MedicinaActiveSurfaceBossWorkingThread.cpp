#include "MedicinaActiveSurfaceBossWorkingThread.h"

CMedicinaActiveSurfaceBossWorkingThread::CMedicinaActiveSurfaceBossWorkingThread(const ACE_CString& name,IRA::CSecureArea<CMedicinaActiveSurfaceBossCore>  *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_core(param)
{
	AUTO_TRACE("CMedicinaActiveSurfaceBossWorkingThread::CMedicinaActiveSurfaceBossWorkingThread()");
}

CMedicinaActiveSurfaceBossWorkingThread::~CMedicinaActiveSurfaceBossWorkingThread()
{
	AUTO_TRACE("CMedicinaActiveSurfaceBossWorkingThread::~CMedicinaActiveSurfaceBossWorkingThread()");
}

void CMedicinaActiveSurfaceBossWorkingThread::onStart()
{
	AUTO_TRACE("CMedicinaActiveSurfaceBossWorkingThread::onStart()");
}

void CMedicinaActiveSurfaceBossWorkingThread::onStop()
{
	 AUTO_TRACE("CMedicinaActiveSurfaceBossWorkingThread::onStop()");
}

void CMedicinaActiveSurfaceBossWorkingThread::runLoop()
{
    IRA::CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();

    try {
        resource->workingActiveSurface();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
    }
}
