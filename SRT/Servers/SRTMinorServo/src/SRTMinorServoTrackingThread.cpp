#include "SRTMinorServoTrackingThread.h"

using namespace MinorServo;

SRTMinorServoTrackingThread::SRTMinorServoTrackingThread(const ACE_CString& name, SRTMinorServoBossCore& core, const ACS::TimeInterval& response_time, const ACS::TimeInterval& sleep_time):
    ACS::Thread(name, response_time, sleep_time),
    m_core(core),
    m_error(false)
{
    AUTO_TRACE("SRTMinorServoTrackingThread::SRTMinorServoTrackingThread()");
}

SRTMinorServoTrackingThread::~SRTMinorServoTrackingThread()
{
    AUTO_TRACE("SRTMinorServoTrackingThread::~SRTMinorServoTrackingThread()");
}

void SRTMinorServoTrackingThread::onStart()
{
    AUTO_TRACE("SRTMinorServoTrackingThread::onStart()");

    this->setSleepTime(500000);   // 50 milliseconds
    m_point_id = 0;
    m_point_time = 0;

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoTrackingThread::onStart()", (LM_INFO, "TRACKING THREAD STARTED"));
}

void SRTMinorServoTrackingThread::onStop()
{
    AUTO_TRACE("SRTMinorServoTrackingThread::onStop()");
    m_core.m_elevation_tracking.store(Management::MNG_FALSE);
    if(m_error)
    {
        m_core.setFailure();
    }

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoTrackingThread::onStop()", (LM_INFO, "TRACKING THREAD STOPPED"));
}

void SRTMinorServoTrackingThread::runLoop()
{
    AUTO_TRACE("SRTMinorServoTrackingThread::runLoop()");

    try
    {
        m_core.checkLineStatus();
    }
    catch(MinorServoErrors::MinorServoErrorsEx& ex)
    {
        ACS_SHORT_LOG((LM_ERROR, ex.errorTrace.routine));
        m_error = true;
        this->setStopped();
        return;
    }

    if(m_core.m_motion_status.load() != MOTION_STATUS_TRACKING)
    {
        // System is not ready or is not configured for tracking yet, we wait, even though we should never get here
        _IRA_LOGFILTER_LOG(LM_WARNING, "SRTMinorServoScanThread::runLoop()", "Waiting for the system to be configured for tracking!");
        return;
    }

    // All checks passed, we have to track
    Management::TBoolean elevation_tracking = Management::MNG_FALSE;

    if(m_point_id == 0)
    {
        m_point_time = getTimeStamp() + PROGRAM_TRACK_FUTURE_TIME;
        m_trajectory_id = (unsigned int)(m_point_time / 1000);
    }
    else
    {
        if(std::all_of(m_core.m_tracking_servos.begin(), m_core.m_tracking_servos.end(), [this](const std::pair<std::string, SRTBaseMinorServo_ptr>& servo) -> bool
        {
            ACSErr::Completion_var comp;
            if(servo.second->in_use()->get_sync(comp.out()) == Management::MNG_TRUE)
            {
                bool return_value = servo.second->operative_mode()->get_sync(comp.out()) == OPERATIVE_MODE_PROGRAMTRACK ? true : false;
                if(!return_value)
                {
                    ACS_LOG(LM_FULL_INFO, "SRTMinorServoTrackingThread::runLoop()", (LM_ERROR, (servo.first + ": failed to set PROGRAM_TRACK operative mode!").c_str()));
                }
                return return_value;
            }
            else
            {
                return true;
            }
        }))
        {
            // All used servos are set to PROGRAMTRACK operative mode, we are tracking the elevation
            elevation_tracking = Management::MNG_TRUE;
        }
        else
        {
            m_error = true;
            this->setStopped();
            return;
        }
    }

    ACSErr::Completion_var comp;
    for(const auto& [servo_name, servo] : m_core.m_tracking_servos)
    {
        if(servo->in_use()->get_sync(comp.out()) == Management::MNG_TRUE)
        {
            double elevation = 45.0;

            try
            {
                elevation = m_core.getElevation(m_point_time);
            }
            catch(ComponentErrors::ComponentErrorsEx& ex)
            {
                _IRA_LOGFILTER_LOG(LM_WARNING, "SRTMinorServoScanThread::runLoop()", (std::string(getReasonFromEx(ex)) + ": using a fixed elevation of 45° for tracking!").c_str());
                elevation_tracking = Management::MNG_FALSE;
            }

            try
            {
                servo->programTrack(m_trajectory_id, m_point_id, m_point_time, *servo->calcCoordinates(elevation));
            }
            catch(MinorServoErrors::MinorServoErrorsEx& ex)
            {
                ACS_SHORT_LOG((LM_ERROR, ex.errorTrace.routine));
                m_error = true;
                this->setStopped();
                return;
            }
        }
    }

    m_core.m_elevation_tracking.store(elevation_tracking);

    m_point_id++;
    m_point_time += PROGRAM_TRACK_TIMEGAP;

    // Sleep until the next point - PROGRAM_TRACK_FUTURE_TIME
    this->setSleepTime((unsigned long)(m_point_time - PROGRAM_TRACK_FUTURE_TIME - getTimeStamp()));
}
