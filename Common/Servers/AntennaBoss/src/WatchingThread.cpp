// $Id: WatchingThread.cpp,v 1.5 2008-05-21 14:20:10 a.orlati Exp $

#include "WatchingThread.h"
#include <LogFilter.h>

_IRA_LOGFILTER_IMPORT;

CWatchingThread::CWatchingThread(const ACE_CString& name,IRA::CSecureArea<CBossCore>  *param, 
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
		IRA::CSecAreaResourceWrapper<CBossCore> resource=m_core->Get("WATCHINGTHREAD:runLoop");
		try {
			//printf("updateAttributes\n");
			resource->updateAttributes();
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl,_dummy,E,"CWatchingThread::runLoop()");
			_dummy.setReason("Could not update component attributes");
			_IRA_LOGFILTER_LOG_EXCEPTION(_dummy,LM_ERROR);
		}
		try {
			resource->checkStatus();
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl,_dummy,E,"CWatchingThread::runLoop()");
			_dummy.setReason("Could not verify sub system status");
			_IRA_LOGFILTER_LOG_EXCEPTION(_dummy,LM_ERROR);
		}
		try {
			resource->publishData();
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl,_dummy,E,"CWatchingThread::runLoop()");
			_dummy.setReason("Could not publish data to notification channel");
			_IRA_LOGFILTER_LOG_EXCEPTION(_dummy,LM_ERROR);
		}
 }
 
