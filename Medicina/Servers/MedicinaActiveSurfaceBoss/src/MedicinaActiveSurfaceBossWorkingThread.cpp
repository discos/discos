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
	m_sleepTime = this->getSleepTime();
}

void CMedicinaActiveSurfaceBossWorkingThread::onStop()
{
	 AUTO_TRACE("CMedicinaActiveSurfaceBossWorkingThread::onStop()");
}

void CMedicinaActiveSurfaceBossWorkingThread::runLoop()
{
	IRA::CSecAreaResourceWrapper<CMedicinaActiveSurfaceBossCore> resource=m_core->Get();

	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	ACS::Time t0 = now.value().value;

	try
	{
		resource->workingActiveSurface();
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex)
	{
		ex.log(LM_DEBUG);
	}
	resource.Release();

	IRA::CIRATools::getTime(now);
	ACS::Time t1 = now.value().value;
	ACS::TimeInterval elapsed = t1 - t0;
	this->setSleepTime(std::max(long(m_sleepTime - elapsed), (long)0));
}
