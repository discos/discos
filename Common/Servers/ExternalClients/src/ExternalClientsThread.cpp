#include "ExternalClientsThread.h"

using namespace IRA;

CExternalClientsThread::CExternalClientsThread(const ACE_CString& name,IRA::CSecureArea<CExternalClientsSocketServer> *param,
        const ACS::TimeInterval& responseTime, const ACS::TimeInterval& sleepTime) :
        ACS::Thread(name,responseTime,sleepTime),
        m_param(param)
{
	AUTO_TRACE("CExternalClientsThread::CExternalClientsThread()");
}

CExternalClientsThread::~CExternalClientsThread()
{ 
	AUTO_TRACE("CExternalClientsThread::~CExternalClientsThread()");
}

void CExternalClientsThread::onStart()
{
	AUTO_TRACE("CExternalClientsThread::onStart()");
	ACS_DEBUG("CExternalClientsThread::onStart()","External clients thread starts");
}
	
void CExternalClientsThread::onStop()
{
	AUTO_TRACE("CExternalClientsThread::onStop()");
	ACS_DEBUG("CExternalClientsThread::onStop()","External clients thread stops");
}

void CExternalClientsThread::runLoop()
{
    IRA::CSecAreaResourceWrapper<CExternalClientsSocketServer> resource = m_param->Get();

    if (resource->m_byebye == false)
        resource->cmdToScheduler();
}
