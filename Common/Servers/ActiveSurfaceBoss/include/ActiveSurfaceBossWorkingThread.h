#ifndef _ACTIVESURFACEBOSSWORKINGTHREAD_H_
#define _ACTIVESURFACEBOSSWORKINGTHREAD_H_

#include <acsThread.h>
#include <IRA>
#include <ComponentErrors.h>

class ActiveSurfaceBossImpl;

/**
 * This class implements a working thread. This thread is in charge of updating the active surface
 */
class ActiveSurfaceBossWorkingThread : public ACS::Thread
{
public:
    /**
     * Constructor().
     * @param name thread name
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
     */
    ActiveSurfaceBossWorkingThread(const ACE_CString& name, ActiveSurfaceBossImpl& impl, const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

    /**
     * Destructor.
     */
    ~ActiveSurfaceBossWorkingThread();

    /**
     * This method is executed once when the thread starts.
     */
    virtual void onStart();

    /**
     * This method is executed once when the thread stops.
     */
    virtual void onStop();

    /**
     * This method overrides the thread implementation class.
     * The thread can be exited by calling ACS::ThreadBase::stop or ACS::ThreadBase::exit command.
     */
    virtual void runLoop();

private:
    ActiveSurfaceBossImpl& m_impl;
    ACS::TimeInterval m_sleepTime;
    bool m_ready;
    unsigned int m_currentTickIndex;
    ACS::Time m_nextTime;
};

#endif /*_ACTIVESURFACEBOSSWORKINGTHREAD_H_*/
