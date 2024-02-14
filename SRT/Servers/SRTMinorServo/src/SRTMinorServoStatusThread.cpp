#include "SRTMinorServoStatusThread.h"

using namespace MinorServo;

SRTMinorServoStatusThread::SRTMinorServoStatusThread(const ACE_CString& name, SRTMinorServoBossCore& core, const ACS::TimeInterval& response_time, const ACS::TimeInterval& sleep_time) :
    ACS::Thread(name, response_time, sleep_time),
    m_core(core),
    m_sleep_time(this->getSleepTime()),
    m_notification_channel(NULL)
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

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoStatusThread::onStart()", (LM_INFO, "STATUS THREAD STARTED"));
}

void SRTMinorServoStatusThread::onStop()
{
    AUTO_TRACE("SRTMinorServoStatusThread::onStop()");

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoStatusThread::onStop()", (LM_INFO, "STATUS THREAD STOPPED"));
}

void SRTMinorServoStatusThread::runLoop()
{
    AUTO_TRACE("SRTMinorServoStatusThread::runLoop()");

    ACS::Time t0 = getTimeStamp();
    unsigned long sleep_time = 10000000;

    // Check the status only if the properties are already being monitored by the Boss component.
    // This is necessary since this thread might start before the start of the properties monitoring.
    if(m_core.m_component.isPropertiesMonitoringActive() && m_core.status())
    {
        // Update the sleep time in order to not drift away by adding latency
        sleep_time = std::max(m_sleep_time - (getTimeStamp() - t0), (long unsigned int)0);
    }

    publish();

    this->setSleepTime(sleep_time);
}

void SRTMinorServoStatusThread::publish()
{
    // TODO: maybe update only when anything changed
    MinorServoDataBlock data;
    TIMEVALUE now;
    IRA::CIRATools::getTime(now);
    data.timeMark = now.value().value;
    data.tracking = m_core.m_tracking.load() == Management::MNG_TRUE;
    data.starting = m_core.m_starting.load() == Management::MNG_TRUE;
    data.parking = m_core.m_motion_status.load() == MOTION_STATUS_PARKING;
    data.parked = m_core.m_motion_status.load() == MOTION_STATUS_PARKED;
    data.status = m_core.m_subsystem_status.load();

    if(m_notification_channel == NULL)
    {
        try
        {
            m_notification_channel = new nc::SimpleSupplier(MINORSERVO_DATA_CHANNEL, &m_core.m_component);
        }
        catch(...)
        {
            _IRA_LOGFILTER_LOG(LM_WARNING, "SRTMinorServoStatusThread::publish()", "cannot access the MinorServoData notification channel!");
            return;
        }
    }

    try
    {
        m_notification_channel->publishData<MinorServoDataBlock>(data);
    }
    catch(ComponentErrors::CORBAProblemEx& ex)
    {
        _IRA_LOGFILTER_LOG(LM_WARNING, "SRTMinorServoStatusThread::publish()", "cannot send MinorServoData over the notification channel!");
    }
}
