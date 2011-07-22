//$Id: TimeoutSync.cpp,v 1.1 2010-11-05 11:28:17 a.orlati Exp $

#include "TimeoutSync.h"
#include "String.h"
#include "IRATools.h"

using namespace IRA;

CTimeoutSync::CTimeoutSync(ACE_Recursive_Thread_Mutex * mutex,const DDWORD& timeout,const DDWORD& sleepTime): m_mutex(mutex), m_timeout(timeout), m_sleepTime(sleepTime), m_locked(false)
{
}

CTimeoutSync::~CTimeoutSync()
{
	release();
}

void CTimeoutSync::release()
{
	if (m_locked) {
		m_mutex->release();
		m_locked=false;
	}
}

bool CTimeoutSync::acquire()
{
	TIMEVALUE now;
	if (m_timeout==0) {
		m_mutex->acquire();
		m_locked=true;
		return m_locked;
	}
	else {
		IRA::CIRATools::getTime(now);
		ACS::Time expireTime=now.value().value+m_timeout*10;
		while (now.value().value<=expireTime) {
			if (m_mutex->tryacquire()!=-1) { //acquire succeeds
				m_locked=true;
				return m_locked;
			}
			IRA::CIRATools::Wait(m_sleepTime);
			IRA::CIRATools::getTime(now);
		}
		m_locked=false;
		return m_locked;
	}
}

bool CTimeoutSync::isLocked() const
{
	return m_locked;
}
