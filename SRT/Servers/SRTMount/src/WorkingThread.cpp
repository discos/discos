#include "WorkingThread.h"

CWorkingThread::CWorkingThread(const ACE_CString& name, CCommandSocket* param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_socket(param)
{
	AUTO_TRACE("CWorkingThread::CWorkingThread()");
}

CWorkingThread::~CWorkingThread()
{
	AUTO_TRACE("CWorkingThread::~CWorkingThread()");
}

void CWorkingThread::onStart()
{
	AUTO_TRACE("CWorkingThread::onStart()");
}

void CWorkingThread::onStop()
{
    AUTO_TRACE("CWorkingThread::onStop()");
}

void CWorkingThread::runLoop()
{
    m_socket->sendProgramTrackPoints();
}
