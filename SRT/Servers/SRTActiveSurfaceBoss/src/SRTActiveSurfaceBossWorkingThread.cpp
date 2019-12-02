#include "SRTActiveSurfaceBossWorkingThread.h"

CSRTActiveSurfaceBossWorkingThread::CSRTActiveSurfaceBossWorkingThread(const ACE_CString& name,IRA::CSecureArea<CSRTActiveSurfaceBossCore>  *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_core(param)
{
	AUTO_TRACE("CSRTActiveSurfaceBossWorkingThread::CSRTActiveSurfaceBossWorkingThread()");
}

CSRTActiveSurfaceBossWorkingThread::~CSRTActiveSurfaceBossWorkingThread()
{
	AUTO_TRACE("CSRTActiveSurfaceBossWorkingThread::~CSRTActiveSurfaceBossWorkingThread()");
}

void CSRTActiveSurfaceBossWorkingThread::onStart()
{
	AUTO_TRACE("CSRTActiveSurfaceBossWorkingThread::onStart()");
	m_sleepTime = this->getSleepTime();
}

void CSRTActiveSurfaceBossWorkingThread::onStop()
{
	 AUTO_TRACE("CSRTActiveSurfaceBossWorkingThread::onStop()");
}

void CSRTActiveSurfaceBossWorkingThread::runLoop()
{
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	ACS::Time t0 = now.value().value;

	IRA::CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
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
