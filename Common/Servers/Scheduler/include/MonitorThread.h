#ifndef _MONITORTHREAD_H_
#define _MONITORTHREAD_H_

#include <Cplusplus11Helper.h>

#define _CPLUSPLUS11_PORTING_

C11_IGNORE_WARNING_PUSH
C11_IGNORE_WARNING("-Wdeprecated-declarations")
#include <acsThread.h>
C11_IGNORE_WARNING_POP

#include "Core.h"

/**
 * This class implements a component monitoring thread . This thread is in charge of update all the attributes of the component in order to make sure the receiver is correctly working.
*/
class CMonitorThread : public ACS::Thread
{
public:
    /**
     * Constructor().
     * @param name thread name
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
     */
	CMonitorThread(const ACE_CString& name, CCore *param, const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime, const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

    /**
     * Destructor.
     */
    ~CMonitorThread();

    /**
     * This method is executed once when the thread starts.
     */
    virtual void onStart();

    /**
     * This method is executed once when the thread stops.
     */
    virtual void onStop();

    /**
     * This method overrides the thread implementation class. The method is executed in a loop until the thread is alive.
     * The thread can be exited by calling ACS::ThreadBase::stop or ACS::ThreadBase::exit command.
    */
    virtual void runLoop();

private:
    CCore *m_core;
    const ACS::TimeInterval m_sleepTime;
    ACS::Time m_nextTime;
};

#endif
