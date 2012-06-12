#include "RefractionWorkingThread.h"


CRefractionWorkingThread::CRefractionWorkingThread(const ACE_CString& name,IRA::CSecureArea<CRefractionCore>  *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_core(param)
{
	AUTO_TRACE("CRefractionWorkingThread::CRefractionWorkingThread()");
}

CRefractionWorkingThread::~CRefractionWorkingThread()
{
	AUTO_TRACE("CRefractionWorkingThread::~CRefractionWorkingThread()");
}

void CRefractionWorkingThread::onStart()
{
	AUTO_TRACE("CRefractionWorkingThread::onStart()");
}

void CRefractionWorkingThread::onStop()
{
	 AUTO_TRACE("CRefractionWorkingThread::onStop()");
}
 
void CRefractionWorkingThread::runLoop()
{
	 IRA::CSecAreaResourceWrapper<CRefractionCore> resource=m_core->Get();
	if (resource->m_byebye == false)
		resource->getMeteoParameters();
}
