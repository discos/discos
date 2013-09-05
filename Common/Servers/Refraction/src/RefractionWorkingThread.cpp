#include "RefractionWorkingThread.h"


//CRefractionWorkingThread::CRefractionWorkingThread(const ACE_CString& name,IRA::CSecureArea<CRefractionCore>  *param, 
//			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_core(param)
CRefractionWorkingThread::CRefractionWorkingThread(const ACE_CString& name,CRefractionCore *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime)
{
	AUTO_TRACE("CRefractionWorkingThread::CRefractionWorkingThread()");
	boss = param;
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
	//IRA::CSecAreaResourceWrapper<CRefractionCore> resource=m_core->Get();
	//if (resource->m_byebye == false)
	//	resource->getMeteoParameters();
	if (boss->m_byebye == false)
		boss->getMeteoParameters();
}
