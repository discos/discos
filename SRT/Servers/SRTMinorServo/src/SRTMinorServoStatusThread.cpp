#include "SRTMinorServoStatusThread.h"

using namespace MinorServo;

SRTMinorServoStatusThread::SRTMinorServoStatusThread(const ACE_CString& name, SRTMinorServoBossCore& core, const ACS::TimeInterval& response_time, const ACS::TimeInterval& sleep_time) :
    ACS::Thread(name, response_time, 10000000),  // Initialize the thread with 1 second period time. We will update this value as soon as the component starts monitoring the properties.
    m_core(core),
    m_sleep_time(sleep_time)
{
    AUTO_TRACE("SRTMinorServoStatusThread::SRTMinorServoStatusThread()");
}

SRTMinorServoStatusThread::~SRTMinorServoStatusThread()
{
    AUTO_TRACE("SRTMinorServoStatusThread::~SRTMinorServoStatusThread()");
}

void SRTMinorServoStatusThread::onStart()
{
    AUTO_TRACE("SRTMinorServoStatusThread::onStart()");

    m_next_time = 0;

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoStatusThread::onStart()", (LM_NOTICE, "STATUS THREAD STARTED"));
}

void SRTMinorServoStatusThread::onStop()
{
    AUTO_TRACE("SRTMinorServoStatusThread::onStop()");
    ACS_LOG(LM_FULL_INFO, "SRTMinorServoStatusThread::onStop()", (LM_NOTICE, "STATUS THREAD STOPPED"));
}

void SRTMinorServoStatusThread::runLoop()
{
    AUTO_TRACE("SRTMinorServoStatusThread::runLoop()");

    // Wait for the properties to be monitored by the Boss component.
    // This is necessary since this thread might start before the start of the properties monitoring.
    if(!m_core.m_component.isPropertiesMonitoringActive())
    {
        return;
    }

    if(m_next_time == 0)
    {
        m_next_time = getTimeStamp();
    }

    m_core.status();
    m_core.publishData();

    m_next_time += m_sleep_time;
    // Update the sleep time in order to not drift away by adding latency from previous calls
    this->setSleepTime(ACS::TimeInterval(std::max(long(0), long(m_next_time - getTimeStamp()))));
}
