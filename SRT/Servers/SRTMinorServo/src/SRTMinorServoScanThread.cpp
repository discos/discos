#include "SRTMinorServoScanThread.h"

using namespace MinorServo;

SRTMinorServoScanThread::SRTMinorServoScanThread(const ACE_CString& name, SRTMinorServoBossCore& core, const ACS::TimeInterval& response_time, const ACS::TimeInterval& sleep_time):
    ACS::Thread(name, response_time, sleep_time),
    m_core(core),
    m_error(false)
{
    AUTO_TRACE("SRTMinorServoScanThread::SRTMinorServoScanThread()");
}

SRTMinorServoScanThread::~SRTMinorServoScanThread()
{
    AUTO_TRACE("SRTMinorServoScanThread::~SRTMinorServoScanThread()");
}

void SRTMinorServoScanThread::onStart()
{
    AUTO_TRACE("SRTMinorServoScanThread::onStart()");

    // We want to start moving the scan servo ASAP and bring its scan axis to the start position.
    // The other servos will keep tracking normally.
    // The general implementation accounts for derotators which will be integrated in the future.
    // The requested scan will start at start_time by adding the correct scan offset to each point.

    // This will find the first useful time to send a point synchronized with the scan start time: m_core.m_current_scan.start_time - (PROGRAM_TRACK_TIMEGAP * N) > now + PROGRAM_TRACK_FUTURE_TIME
    size_t pre_scan_points = std::floor((m_core.m_current_scan.start_time - (getTimeStamp() + PROGRAM_TRACK_FUTURE_TIME)) / PROGRAM_TRACK_TIMEGAP);
    m_point_time = m_core.m_current_scan.start_time - pre_scan_points * PROGRAM_TRACK_TIMEGAP;
    m_point_id = 0;
    m_trajectory_id = (unsigned int)(m_point_time / 1000);

    // How many points in the scan trajectory?
    // We don't check the modulo of the division, we assume all the scans duration is always expressed in seconds, therefore the modulo should always be 0
    size_t scan_points = m_core.m_current_scan.scan_duration / PROGRAM_TRACK_TIMEGAP + 1;
    // How big is each offset step? We have N points but N-1 segments
    double scan_delta = m_core.m_current_scan.scan_range / (scan_points - 1);

    m_scan_offsets = SRTMinorServoPositionsQueue(pre_scan_points + scan_points, 1);
    for(size_t i = 0; i < pre_scan_points; i++)
    {
        m_scan_offsets.put(m_point_time + i * PROGRAM_TRACK_TIMEGAP, { -m_core.m_current_scan.scan_range / 2 });
    }
    for(size_t i = 0; i < scan_points; i++)
    {
        m_scan_offsets.put(m_core.m_current_scan.start_time + i * PROGRAM_TRACK_TIMEGAP, {-m_core.m_current_scan.scan_range / 2 + i * scan_delta});
    }

    // We save the starting coordinates to return to them when the scan is finished
    ACSErr::Completion_var comp;
    auto servo = m_core.m_tracking_servos.at(m_core.m_current_scan.servo_name);
    m_starting_coordinates = *servo->virtual_positions()->get_sync(comp.out());

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoScanThread::onStart()", (LM_INFO, "SCAN THREAD STARTED"));
}

void SRTMinorServoScanThread::onStop()
{
    AUTO_TRACE("SRTMinorServoScanThread::onStop()");

    m_core.m_scanning.store(Management::MNG_FALSE);

    if(m_error)  // We didn't reach the end of the scan because of some error
    {
        // Should we set everything to failure?
        m_core.setFailure();
        return;
    }

    m_core.m_scanning.store(Management::MNG_FALSE);
    m_core.m_elevation_tracking.store(Management::MNG_FALSE);

    // Reset m_point_time
    m_point_time -= PROGRAM_TRACK_FUTURE_TIME;

    auto servo = m_core.m_tracking_servos.at(m_core.m_current_scan.servo_name);

    // Update the central scan position using the correct elevation (from the past)
    m_core.m_current_scan.central_position = (*servo->calcCoordinates(m_core.getElevation((m_core.m_current_scan.start_time + m_core.m_current_scan.scan_duration) / 2)))[m_core.m_current_scan.axis_index];

    ACS::doubleSeq return_coordinates; 
    if(m_core.m_motion_status.load() == MOTION_STATUS_TRACKING)
    {
        // Restart the tracking
        m_core.startThread(m_core.m_tracking_thread);
        return_coordinates = *servo->calcCoordinates(m_core.getElevation(getTimeStamp()));
    }
    else
    {
        // We were not tracking, send the servo to the starting position
        servo->preset(m_starting_coordinates);
        return_coordinates = m_starting_coordinates;
    }

    // Calculate the close_time again now that the servos are already moving back to their original position
    m_core.m_current_scan.close_time = getTimeStamp() + servo->getTravelTime(ACS::doubleSeq(), return_coordinates);

    // Copy current_scan into last_scan
    m_core.m_last_scan = m_core.m_current_scan;

    // Finally unlock the scan capabilities
    m_core.m_scan_active.store(Management::MNG_FALSE);

    ACS_LOG(LM_FULL_INFO, "SRTMinorServoScanThread::onStop()", (LM_INFO, "SCAN THREAD STOPPED"));
}

void SRTMinorServoScanThread::runLoop()
{
    AUTO_TRACE("SRTMinorServoScanThread::runLoop()");

    if(m_point_time == m_core.m_current_scan.start_time + PROGRAM_TRACK_FUTURE_TIME)  // The system should be starting to scan right now
    {
        m_core.m_scanning.store(Management::MNG_TRUE);
    }
    else if(m_point_time == m_core.m_current_scan.start_time + m_core.m_current_scan.scan_duration + PROGRAM_TRACK_FUTURE_TIME)  // The system should have finished to scan right now
    {
        this->setStopped();
        return;
    }
    else if(m_point_time > m_core.m_current_scan.start_time + m_core.m_current_scan.scan_duration)  // All points were sent, we can stop sending but we'll wait for the scan to finish
    {
        m_point_time += PROGRAM_TRACK_TIMEGAP;

        // Sleep until the next point - PROGRAM_TRACK_FUTURE_TIME
        this->setSleepTime((m_point_time - PROGRAM_TRACK_FUTURE_TIME) - getTimeStamp());
        return;
    }

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

    Management::TBoolean elevation_tracking = Management::MNG_FALSE;

    if(m_point_id > 0)
    {
        if(std::all_of(m_core.m_tracking_servos.begin(), m_core.m_tracking_servos.end(), [this](const std::pair<std::string, SRTBaseMinorServo_ptr>& servo) -> bool
        {
            ACSErr::Completion_var comp;
            if(servo.second->in_use()->get_sync(comp.out()) == Management::MNG_TRUE)
            {
                if(m_core.m_motion_status.load() == MOTION_STATUS_CONFIGURED && servo.first != m_core.m_current_scan.servo_name)
                {
                    // Not tracking the elevation and this servo is not involved in the current scan
                    return true;
                }

                bool return_value = servo.second->operative_mode()->get_sync(comp.out()) == OPERATIVE_MODE_PROGRAMTRACK ? true : false;
                if(!return_value)
                {
                    ACS_LOG(LM_FULL_INFO, "SRTMinorServoScanThread::runLoop()", (LM_ERROR, (servo.first + ": failed to set PROGRAM_TRACK operative mode!").c_str()));
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

    double elevation = m_core.m_current_scan.starting_elevation;

    if(m_core.m_motion_status.load() == MOTION_STATUS_TRACKING)  // We are in elevation tracking mode
    {
        try
        {
            elevation = m_core.getElevation(m_point_time);
        }
        catch(ComponentErrors::ComponentErrorsEx& ex)
        {
            _IRA_LOGFILTER_LOG(LM_WARNING, "SRTMinorServoScanThread::runLoop()", (std::string(getErrorFromEx(ex)) + " Using a fixed elevation of " + std::to_string(elevation) + "° as scan central coordinate!").c_str());
            elevation_tracking = Management::MNG_FALSE;
        }
    }

    ACSErr::Completion_var comp;
    for(const auto& [servo_name, servo] : m_core.m_tracking_servos)
    {
        if(servo->in_use()->get_sync(comp.out()) == Management::MNG_TRUE)
        {
            if(m_core.m_motion_status.load() == MOTION_STATUS_CONFIGURED && servo_name != m_core.m_current_scan.servo_name)
            {
                // Not tracking the elevation and this servo is not involved in the current scan
                continue;
            }

            ACS::doubleSeq coordinates = m_starting_coordinates;

            if(m_core.m_motion_status.load() == MOTION_STATUS_TRACKING)
            {
                coordinates = *servo->calcCoordinates(elevation);
            }

            if(servo_name == m_core.m_current_scan.servo_name)
            {
                try
                {
                    // Ask for the offset in the exact m_point_time (we don't want to interpolate or retrieve any value before or after the scan time span)
                    coordinates[m_core.m_current_scan.axis_index] += m_scan_offsets.get(m_point_time, true).second[0];
                }
                catch(...)
                {
                    // No offset found for this point_time, ignore
                }
            }

            try
            {
                servo->programTrack(m_trajectory_id, m_point_id, m_point_time, coordinates);
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
    this->setSleepTime((m_point_time - PROGRAM_TRACK_FUTURE_TIME) - getTimeStamp());
}