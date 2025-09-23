#include "ActiveSurfaceBossWorkingThread.h"

CActiveSurfaceBossWorkingThread::CActiveSurfaceBossWorkingThread(const ACE_CString& name, IRA::CSecureArea<CActiveSurfaceBossCore> *param, const ACS::TimeInterval& responseTime, const ACS::TimeInterval& sleepTime) :
    ACS::Thread(name, responseTime, sleepTime),
    m_core(param)
{
	AUTO_TRACE("CActiveSurfaceBossWorkingThread::CActiveSurfaceBossWorkingThread()");
}

CActiveSurfaceBossWorkingThread::~CActiveSurfaceBossWorkingThread()
{
	AUTO_TRACE("CActiveSurfaceBossWorkingThread::~CActiveSurfaceBossWorkingThread()");
}

void CActiveSurfaceBossWorkingThread::onStart()
{
	AUTO_TRACE("CActiveSurfaceBossWorkingThread::onStart()");
	m_sleepTime = this->getSleepTime();
}

void CActiveSurfaceBossWorkingThread::onStop()
{
	 AUTO_TRACE("CActiveSurfaceBossWorkingThread::onStop()");
}

void CActiveSurfaceBossWorkingThread::runLoop()
{
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	ACS::Time t0 = now.value().value;

	IRA::CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
	resource->workingActiveSurface();
	resource.Release();

	IRA::CIRATools::getTime(now);
	ACS::Time t1 = now.value().value;
	ACS::TimeInterval elapsed = t1 - t0;
	this->setSleepTime(std::max(long(m_sleepTime - elapsed), (long)0));
}
