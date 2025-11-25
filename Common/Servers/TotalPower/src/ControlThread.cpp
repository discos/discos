// $Id: ControlThread.cpp,v 1.1 2011-03-14 14:15:07 a.orlati Exp $

#include "ControlThread.h"
#include <LogFilter.h>

_IRA_LOGFILTER_IMPORT;


CControlThread::CControlThread(const ACE_CString& name,CSecureArea<CCommandLine> *parameter,const ACS::TimeInterval& responseTime, const ACS::TimeInterval& sleepTime) :
    ACS::Thread(name, responseTime, 0),
    m_pData(parameter),
    m_sleepTime(sleepTime)
{
    AUTO_TRACE("CControlThread::CControlThread()");
}
 
CControlThread::~CControlThread()
{
    AUTO_TRACE("CControlThread::~CControlThread()");
}
 
void CControlThread::onStart()
{
    AUTO_TRACE("CControlThread::onStart()");

    m_nextTime = getTimeStamp();
}

void CControlThread::onStop()
{
    AUTO_TRACE("CControlThread::onStop()");
}
 
void CControlThread::runLoop()
{
    IRA::CSecAreaResourceWrapper<CCommandLine> line=m_pData->Get();
    try
    {
        line->checkTime(); 
        line->publishZMQData(m_nextTime);
    }
    catch (ACSErr::ACSbaseExImpl& E)
    {
        _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl,_dummy,E,"CControlThread::runLoop()");
        _dummy.setReason("Could not check time synchronization");
        _IRA_LOGFILTER_LOG_EXCEPTION(_dummy,LM_ERROR);
    }

    m_nextTime += m_sleepTime;
    setSleepTime(ACS::TimeInterval(std::max(long(0), long(m_nextTime - getTimeStamp()))));
}
