#include "SRTMinorServoTrackingThread.h"

using namespace MinorServo;

SRTMinorServoTrackingThread::SRTMinorServoTrackingThread(const ACE_CString& name, SRTMinorServoBossCore& core, const ACS::TimeInterval& response_time, const ACS::TimeInterval& sleep_time):
    ACS::Thread(name, response_time, sleep_time),
    m_core(core)
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

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoTrackingThread::onStart()", (LM_NOTICE, "TRACKING THREAD STARTED"));
}

void SRTMinorServoTrackingThread::onStop()
{
    AUTO_TRACE("SRTMinorServoTrackingThread::onStop()");

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoTrackingThread::onStop()", (LM_NOTICE, "TRACKING THREAD STOPPED"));

    m_core.m_elevation_tracking.store(Management::MNG_FALSE);
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
        this->setStopped();
        return;
    }

    if(m_core.m_motion_status.load() != MOTION_STATUS_TRACKING)
    {
        // System is not ready or is not configured for tracking yet, we wait, even though we should never get here
        _IRA_LOGFILTER_LOG(LM_WARNING, "SRTMinorServoTrackingThread::runLoop()", "Waiting for the system to be configured for tracking!");
        return;
    }

    // All checks passed, we have to track
    if(m_point_id == 0)
    {
        m_point_time = getTimeStamp() + PROGRAM_TRACK_FUTURE_TIME;
        m_trajectory_id = (unsigned int)(IRA::CIRATools::ACSTime2UNIXEpoch(m_point_time));
    }
    else if(m_point_id == 1)
    {
        if(std::all_of(m_core.m_current_tracking_servos.begin(), m_core.m_current_tracking_servos.end(), [this](const std::pair<std::string, SRTBaseMinorServo_ptr>& servo) -> bool
        {
            ACSErr::Completion_var comp;
            if(servo.second->operative_mode()->get_sync(comp.out()) != OPERATIVE_MODE_PROGRAMTRACK)
            {
                ACS_LOG(LM_FULL_INFO, "SRTMinorServoTrackingThread::runLoop()", (LM_CRITICAL, (servo.first + ": failed to set PROGRAM_TRACK operative mode!").c_str()));
                return false;
            }
            return true;
        }))
        {
            // All used servos are set to PROGRAMTRACK operative mode, we are tracking the elevation
            m_core.m_elevation_tracking.store(Management::MNG_TRUE);
        }
        else
        {
            m_core.setError(ERROR_CONFIG_ERROR);
            this->setStopped();
            return;
        }
    }

    for(const auto& [servo_name, servo] : m_core.m_current_tracking_servos)
    {
        double elevation = 45.0;

        try
        {
            elevation = m_core.getElevation(m_point_time);
        }
        catch(ComponentErrors::ComponentErrorsEx& ex)
        {
            _IRA_LOGFILTER_LOG(LM_WARNING, "SRTMinorServoTrackingThread::runLoop()", (std::string(getReasonFromEx(ex)) + ": using a fixed elevation of 45° for tracking!").c_str());
            m_core.m_elevation_tracking.store(Management::MNG_FALSE);
        }

        try
        {
            servo->programTrack(m_trajectory_id, m_point_id, m_point_time, *servo->calcCoordinates(elevation));
        }
        catch(MinorServoErrors::MinorServoErrorsEx& ex)
        {
            ACS_SHORT_LOG((LM_ERROR, ex.errorTrace.routine));
            m_core.setError(ERROR_COMMAND_ERROR);
            this->setStopped();
            return;
        }
        catch(std::exception& ex)
        {
            ACS_SHORT_LOG((LM_ERROR, ex.what()));
            m_core.setError(ERROR_COMMAND_ERROR);
            this->setStopped();
            return;
        }
        catch(CORBA::Exception& ex)
        {
            ACS_SHORT_LOG((LM_ERROR, ex._info().c_str()));
            m_core.setError(ERROR_COMMAND_ERROR);
            this->setStopped();
            return;
        }
    }

    m_point_id++;
    m_point_time += PROGRAM_TRACK_TIMEGAP;

    // Sleep until the next point - PROGRAM_TRACK_FUTURE_TIME
    this->setSleepTime((unsigned long)(m_point_time - PROGRAM_TRACK_FUTURE_TIME - getTimeStamp()));
}
