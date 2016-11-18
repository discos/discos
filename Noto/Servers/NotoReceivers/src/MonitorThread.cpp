#include "MonitorThread.h"
#include <LogFilter.h>

//_IRA_LOGFILTER_IMPORT;

CMonitorThread::CMonitorThread(const ACE_CString& name,CComponentCore  *param,
                        const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_core(param)
{
	AUTO_TRACE("CMonitorThread::CMonitorThread()");
	m_core=param;
}

CMonitorThread::~CMonitorThread()
{
	AUTO_TRACE("CMonitorThread::~CMonitorThread()");
}

void CMonitorThread::onStart()
{
	AUTO_TRACE("CMonitorThread::onStart()");
	m_currentStage=STATUS;
}

 void CMonitorThread::onStop()
{
	 AUTO_TRACE("CMonitorThread::onStop()");
}

void CMonitorThread::runLoop()
{
	 m_core->updateComponent();
	 switch( m_currentStage) {
	 	case STATUS: {
	 		m_currentStage=REMOTE;
	 		 break;
	 	}
	 	case REMOTE: {
	 		m_currentStage=NOISEMARK;
	 		try {
	 			m_core->updateIsRemote();
	 		}
	 		 catch (ACSErr::ACSbaseExImpl& ex) {
	 			 _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl,impl,ex,"CMonitorThread::runLoop");
	 			 //_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
	 		}
	 		break;
	 	}
	 	case NOISEMARK: {
	 		m_currentStage=UNLOCKED;
	 		try {
	 			m_core->updateNoiseMark();
	 		}
	 		 catch (ACSErr::ACSbaseExImpl& ex) {
	 			 _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl,impl,ex,"CMonitorThread::runLoop");
	 			 //_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
	 		}
	 		break;
	 	}
	 	case UNLOCKED: {
	 		m_currentStage=STATUS;
	 		try {
	 			m_core->checkLocalOscillator();
	 		}
	 		 catch (ACSErr::ACSbaseExImpl& ex) {
	 			 _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl,impl,ex,"CMonitorThread::runLoop");
	 			 //_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
	 		}
	 		break;
	 	}
	 }
}
