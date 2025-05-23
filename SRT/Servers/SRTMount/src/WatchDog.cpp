#include "StatusSocket.h"
#include "CommandSocket.h"
#include "WatchDog.h"
#include <LogFilter.h>

using namespace IRA;

_IRA_LOGFILTER_IMPORT;

CWatchDog::CWatchDog(const ACE_CString& name,TThreadParameter *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_param(param)
{
	AUTO_TRACE("CWatchDog::CWatchDog()");
}

CWatchDog::~CWatchDog()
{
	AUTO_TRACE("CWatchDog::~CWatchDog()");
}

void CWatchDog::onStart()
{
	AUTO_TRACE("CWatchDog::onStart()");
}

 void CWatchDog::onStop()
 {
	AUTO_TRACE("CWatchDog::onStop()");
 }

void CWatchDog::runLoop()
{
	try {
		m_param->commandSocket->connectSocket();
	}
	catch (ComponentErrors::SocketErrorExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl,impl,ex,"CWatchDog::runLoop()");
		impl.setReason("Cannot connect command socket");
		_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
	}	 
}
