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

    ACS::Time t0 = getTimeStamp();
    unsigned long sleep_time = 10000000;

    if(m_component.updateStatus())
    {
        // Update the sleep time in order to not drift away by adding latency
        sleep_time = std::max(m_sleep_time - (getTimeStamp() - t0), (long unsigned int)0);
    }

    this->setSleepTime(sleep_time);
}
