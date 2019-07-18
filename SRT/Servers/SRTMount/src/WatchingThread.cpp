#include "WatchingThread.h"
#include <LogFilter.h>

using namespace IRA;

_IRA_LOGFILTER_IMPORT;

CWatchingThread::CWatchingThread(const ACE_CString& name,TThreadParameter *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_param(param)
{
	AUTO_TRACE("CWatchingThread::CWatchingThread()");
}

CWatchingThread::~CWatchingThread()
{
	AUTO_TRACE("CWatchingThread::~CWatchingThread()");
}

void CWatchingThread::onStart()
{
	AUTO_TRACE("CWatchingThread::onStart()");
}

 void CWatchingThread::onStop()
 {
	AUTO_TRACE("CWatchingThread::onStop()");
 }

void CWatchingThread::runLoop()
{
	try {
		m_param->commandSocket->connectSocket();
	}
	catch (ComponentErrors::SocketErrorExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl,impl,ex,"CWatchingThread::runLoop()");
		impl.setReason("Cannot connect command socket");
		_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
	}
}
