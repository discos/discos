// $Id: WatchingThread.cpp,v 1.5 2008-05-21 14:20:10 a.orlati Exp $

#include "WatchingThread.h"
#include <LogFilter.h>

_IRA_LOGFILTER_IMPORT;

CWatchingThread::CWatchingThread(const ACE_CString& name,CBossCore *param,
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), boss(param)
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
			if(!boss->updateAttributes()) return;
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl,_dummy,E,"CWatchingThread::runLoop()");
			_dummy.setReason("Could not update component attributes");
			_IRA_LOGFILTER_LOG_EXCEPTION(_dummy,LM_ERROR);
		}
		try {
			boss->checkStatus();
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl,_dummy,E,"CWatchingThread::runLoop()");
			_dummy.setReason("Could not verify sub system status");
			_IRA_LOGFILTER_LOG_EXCEPTION(_dummy,LM_ERROR);
		}
		try {
			boss->publishData();
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl,_dummy,E,"CWatchingThread::runLoop()");
			_dummy.setReason("Could not publish data to notification channel");
			_IRA_LOGFILTER_LOG_EXCEPTION(_dummy,LM_ERROR);
		}
 }
 
