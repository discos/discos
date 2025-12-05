#include "SRTDerotatorStatusThread.h"

using namespace MinorServo;

SRTDerotatorStatusThread::SRTDerotatorStatusThread(const ACE_CString& name, SRTDerotatorImpl& component, const ACS::TimeInterval& response_time, const ACS::TimeInterval& sleep_time) :
    ACS::Thread(name, response_time, sleep_time),
    m_component(component),
    m_sleep_time(this->getSleepTime())
{
    AUTO_TRACE("SRTDerotatorStatusThread::SRTDerotatorStatusThread()");
}

SRTDerotatorStatusThread::~SRTDerotatorStatusThread()
{
    AUTO_TRACE("SRTDerotatorStatusThread::~SRTDerotatorStatusThread()");
}

void SRTDerotatorStatusThread::onStart()
{
    AUTO_TRACE("SRTDerotatorStatusThread::onStart()");

    m_next_time = 0;

    ACS_LOG(LM_FULL_INFO, "SRTDerotatorStatusThread::onStart()", (LM_DEBUG, "STATUS THREAD STARTED"));
}

void SRTDerotatorStatusThread::onStop()
{
    AUTO_TRACE("SRTDerotatorStatusThread::onStop()");
    ACS_LOG(LM_FULL_INFO, "SRTDerotatorStatusThread::onStop()", (LM_DEBUG, "STATUS THREAD STOPPED"));
}

void SRTDerotatorStatusThread::runLoop()
{
    AUTO_TRACE("SRTDerotatorStatusThread::runLoop()");

    unsigned long sleep_time = 10000000;

    if(m_component.updateStatus())
    {
        if(m_next_time == 0)
        {
            m_next_time = getTimeStamp();
        }

        m_component.publishZMQDictionary();

        m_next_time += m_sleep_time;

        // Update the sleep time in order to not drift away by adding latency
        sleep_time = ACS::TimeInterval(std::max(long(0), long(m_next_time - getTimeStamp())));
    }
    else
    {
        m_next_time = 0;
    }

    this->setSleepTime(sleep_time);
}
