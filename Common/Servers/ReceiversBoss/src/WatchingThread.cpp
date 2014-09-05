#include "WatchingThread.h"
#include <LogFilter.h>

_IRA_LOGFILTER_IMPORT;

CWatchingThread::CWatchingThread(const ACE_CString& name,CRecvBossCore  *param,
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_core(param)
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
			m_core->updateRecvStatus();
			m_core->updateDewarPositionerStatus();
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl,_dummy,E,"CWatchingThread::runLoop()");
			_dummy.setReason("Could not update component attributes");
			_IRA_LOGFILTER_LOG_EXCEPTION(_dummy,LM_ERROR);
		}
		try {
			/*Management::TSystemStatus temp=*/m_core->getStatus();
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl,_dummy,E,"CWatchingThread::runLoop()");
			_dummy.setReason("Could not verify sub system status");
			_IRA_LOGFILTER_LOG_EXCEPTION(_dummy,LM_ERROR);
		}
		try {
			m_core->publishData();
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl,_dummy,E,"CWatchingThread::runLoop()");
			_dummy.setReason("Could not publish data to notification channel");
			_IRA_LOGFILTER_LOG_EXCEPTION(_dummy,LM_ERROR);
		}
 }
 
