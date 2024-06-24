#include "NotoActiveSurfaceBossWorkingThread.h"

CNotoActiveSurfaceBossWorkingThread::CNotoActiveSurfaceBossWorkingThread(const ACE_CString& name,IRA::CSecureArea<CNotoActiveSurfaceBossCore>  *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_core(param)
{
	AUTO_TRACE("CNotoActiveSurfaceBossWorkingThread::CNotoActiveSurfaceBossWorkingThread()");
}

CNotoActiveSurfaceBossWorkingThread::~CNotoActiveSurfaceBossWorkingThread()
{
	AUTO_TRACE("CNotoActiveSurfaceBossWorkingThread::~CNotoActiveSurfaceBossWorkingThread()");
}

void CNotoActiveSurfaceBossWorkingThread::onStart()
{
	AUTO_TRACE("CNotoActiveSurfaceBossWorkingThread::onStart()");
	m_sleepTime = this->getSleepTime();
}

void CNotoActiveSurfaceBossWorkingThread::onStop()
{
	 AUTO_TRACE("CNotoActiveSurfaceBossWorkingThread::onStop()");
}

void CNotoActiveSurfaceBossWorkingThread::runLoop()
{
	IRA::CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();

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
