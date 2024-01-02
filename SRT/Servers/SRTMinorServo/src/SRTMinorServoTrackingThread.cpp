#include "SRTMinorServoTrackingThread.h"

SRTMinorServoTrackingThread::SRTMinorServoTrackingThread(const ACE_CString& name, SRTMinorServoBossCore* core, const ACS::TimeInterval& responseTime, const ACS::TimeInterval& sleepTime):
    ACS::Thread(name, responseTime, sleepTime),
    m_core(core)
{
    m_thread_name = std::string(name.c_str());
    AUTO_TRACE(m_thread_name + "::SRTMinorServoTrackingThread()");
}

SRTMinorServoTrackingThread::~SRTMinorServoTrackingThread()
{
    AUTO_TRACE(m_thread_name + "::~SRTMinorServoTrackingThread()");
}

void SRTMinorServoTrackingThread::onStart()
{
    AUTO_TRACE(m_thread_name + "::onStart()");
    resetTracking();
    m_antennaBoss = Antenna::AntennaBoss::_nil();
}

void SRTMinorServoTrackingThread::onStop()
{
    AUTO_TRACE(m_thread_name + "::onStop()");
}

void SRTMinorServoTrackingThread::runLoop()
{
    AUTO_TRACE(m_thread_name + "::runLoop()");

    try
    {
        m_core->checkControl();
        m_core->checkEmergency();
    }
    catch(MinorServoErrors::StatusErrorExImpl& ex)
    {
        ACS_LOG(LM_FULL_INFO, m_thread_name + "::runLoop()", (LM_ERROR, ex.getData("Reason").c_str()));
        this->setStopped();
        return;
    }

    if(m_core->m_elevation_tracking_enabled == Management::MNG_FALSE)
    {
        // We get here if the tracking was disabled while already tracking, we stop
        this->setStopped();
        return;
    }

    if(m_core->m_motion_status != MinorServo::MOTION_STATUS_TRACKING || m_core->m_subsystem_status != Management::MNG_OK)
    {
        // System is not ready or configured for tracking, reset the tracking
        resetTracking();
        return;
    }

    if(CORBA::is_nil(m_antennaBoss))
    {
        try
        {
            m_antennaBoss = m_core->m_component->getContainerServices()->getComponent<Antenna::AntennaBoss>("ANTENNA/Boss");
        }
        catch(maciErrType::CannotGetComponentExImpl& ex)
        {
            _IRA_LOGFILTER_LOG(LM_WARNING, "SRTMinorServoTrackingThread::runLoop()", "cannot get the AntennaBoss component!");
            resetTracking();
            return;
        }
    }

    if(m_point_id == 0)
    {
        m_point_time = CIRATools::getUNIXEpoch() + TRACKING_FUTURE_TIME;
        m_trajectory_id = int(m_point_time);
    }
    else
    {
        if(std::all_of(m_core->m_tracking_servos.begin(), m_core->m_tracking_servos.end(), [this](const std::pair<std::string, MinorServo::SRTBaseMinorServo_ptr>& servo) -> bool
        {
            ACSErr::Completion_var comp;
            if(servo.second->in_use()->get_sync(comp.out()) == Management::MNG_TRUE)
            {
                bool return_value = servo.second->operative_mode()->get_sync(comp.out()) == MinorServo::OPERATIVE_MODE_PROGRAMTRACK ? true : false;
                if(!return_value)
                {
                    _IRA_LOGFILTER_LOG(LM_ERROR, "SRTMinorServoTrackingThread::runLoop()", (servo.first + ": failed to set PROGRAM_TRACK operative mode!").c_str());
                }
                return return_value;
            }
            else
            {
                return true;
            }
        }))
        {
            m_core->m_tracking = Management::MNG_TRUE;
        }
        else
        {
            resetTracking();
            return;
        }
    }

    ACSErr::Completion_var comp;

    bool tracking_error = false;

    for(const auto& [servo_name, servo] : m_core->m_tracking_servos)
    {
        if(servo->in_use()->get_sync(comp.out()) == Management::MNG_TRUE)
        {
            try
            {
                servo->programTrack(m_trajectory_id, m_point_id, m_point_id == 0 ? m_point_time : 0, *servo->calcCoordinates(getElevation(m_point_time)));
            }
            catch(MinorServoErrors::CommunicationErrorExImpl& ex)
            {
                _IRA_LOGFILTER_LOG(LM_ERROR, "SRTMinorServoTrackingThread::runLoop()", (servo_name + ": received NAK while sending the tracking point!").c_str());
                tracking_error = true;
            }
            catch(ComponentErrors::SocketErrorEx& ex)
            {
                _IRA_LOGFILTER_LOG(LM_ERROR, "SRTMinorServoTrackingThread::runLoop()", (servo_name + ": error encountered while sending the tracking point!").c_str());
                tracking_error = true;
            }
        }
    }

    if(tracking_error)
    {
        resetTracking();
        return;
    }

    m_point_id++;
    m_point_time += TRACKING_TIMEGAP;

    unsigned long time_to_sleep = ((m_point_time - TRACKING_FUTURE_TIME) - CIRATools::getUNIXEpoch()) * 10000000;
    this->setSleepTime(time_to_sleep);
}

void SRTMinorServoTrackingThread::resetTracking()
{
    this->setSleepTime(500000);   // 50 milliseconds
    m_point_id = 0;
    m_point_time = 0;
    m_core->m_tracking = Management::MNG_FALSE;
}

double SRTMinorServoTrackingThread::getElevation(double time)
{
    AUTO_TRACE(m_thread_name + "::getElevation()");

    double azimuth, elevation;
    m_antennaBoss->getRawCoordinates(CIRATools::UNIXEpoch2ACSTime(time), azimuth, elevation);
    return elevation * DR2D;
}
