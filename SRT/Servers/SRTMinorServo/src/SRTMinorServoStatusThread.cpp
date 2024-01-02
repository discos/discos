#include "SRTMinorServoStatusThread.h"

SRTMinorServoStatusThread::SRTMinorServoStatusThread(const ACE_CString& name, SRTMinorServoBossCore* core, const ACS::TimeInterval& responseTime, const ACS::TimeInterval& sleepTime):
    ACS::Thread(name, responseTime, sleepTime),
    m_core(core)
{
    m_thread_name = std::string(name.c_str());
    AUTO_TRACE(m_thread_name + "::SRTMinorServoStatusThread()");
}

SRTMinorServoStatusThread::~SRTMinorServoStatusThread()
{
    AUTO_TRACE(m_thread_name + "::~SRTMinorServoStatusThread()");
}

void SRTMinorServoStatusThread::onStart()
{
    AUTO_TRACE(m_thread_name + "::onStart()");
    m_sleep_time = this->getSleepTime();
}

void SRTMinorServoStatusThread::onStop()
{
    AUTO_TRACE(m_thread_name + "::onStop()");
}

void SRTMinorServoStatusThread::runLoop()
{
    AUTO_TRACE(m_thread_name + "::runLoop()");

    TIMEVALUE now;
    IRA::CIRATools::getTime(now);
    ACS::Time time = now.value().value;
    ACS::Time next = time + m_sleep_time;

    m_core->status();

    for(const auto& [name, servo] : m_core->m_servos)
    {
        servo->status();
    }

    IRA::CIRATools::getTime(now);
    time = now.value().value;
    ACS::TimeInterval sleep_time = next - time;
    this->setSleepTime(std::max(long(sleep_time), long(0)));
}
