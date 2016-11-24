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
	 		m_currentStage=UNLOCKED;
	 		 break;
	 	}
	 	case UNLOCKED: {
	 		m_currentStage=STATUS;
	 		m_core->checkLocalOscillator();
	 		break;
	 	}
	 }
}
