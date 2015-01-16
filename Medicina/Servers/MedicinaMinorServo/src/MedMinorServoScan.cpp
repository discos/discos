#include "MedMinorServoScan.hpp"

MedMinorServoScan::MedMinorServoScan():
                          m_min_time(0),
                          m_max_time(0),
                          m_initialized(false),
                          m_was_tracking(false)
{}

MedMinorServoScan::MedMinorServoScan(const MedMinorServoPosition central_position,
                          const ACS::Time starting_time,
                          double range,
                          const ACS::Time total_time,
                          const string axis_code,
                          bool was_tracking)
{
    init(central_position, 
         starting_time, 
         range,
         total_time,
         axis_code,
         was_tracking);
}

void
MedMinorServoScan::init(const MedMinorServoPosition central_position,
                        const ACS::Time starting_time,
                        double range,
                        const ACS::Time total_time,
                        const string axis_code,
                        bool was_tracking)
{
    m_central_position = central_position;
    m_start_position = central_position;
    m_stop_position = central_position;
    m_was_tracking = was_tracking;
    if(starting_time == 0){
        TIMEVALUE now;
        IRA::CIRATools::getTime(now);
        m_starting_time = now.value().value;
    }else{
        m_starting_time = starting_time;
    }
    m_range = range;
    m_total_time = total_time;
    m_axis_code = axis_code;
    m_min_time = 0;
    m_max_time = 0;
    if(central_position.mode == MED_MINOR_SERVO_PRIMARY)
    {
        if(m_axis_code == "YP"){
            m_start_position.y += range/2;
            m_stop_position.y -= range/2;
        }
        if(m_axis_code == "ZP"){
            m_start_position.z += range/2;
            m_stop_position.z -= range/2;
        }
    }
    if(central_position.mode == MED_MINOR_SERVO_SECONDARY)
    {
        if(m_axis_code == "X"){
            m_start_position.x += range/2;
            m_stop_position.x -= range/2;
        }
        if(m_axis_code == "Y"){
            m_start_position.y += range/2;
            m_stop_position.y -= range/2;
        }
        if(m_axis_code == "Z"){
            m_start_position.z += range/2;
            m_stop_position.z -= range/2;
        }
        if(m_axis_code == "THETA_X"){
            m_start_position.theta_x += range/2;
            m_stop_position.theta_x -= range/2;
        }
        if(m_axis_code == "THETA_Y"){
            m_start_position.theta_y += range/2;
            m_stop_position.theta_y -= range/2;
        }
    }
    m_min_time = MedMinorServoGeometry::min_time(m_start_position,
                                                 m_stop_position);
    m_max_time = MedMinorServoGeometry::max_time(m_start_position,
                                                 m_stop_position);
    m_initialized = true;
}

MedMinorServoScan::~MedMinorServoScan(){}

bool
MedMinorServoScan::check()
throw (MinorServoErrors::ScanErrorEx)
{
    if(!(m_initialized))
        THROW_EX(MinorServoErrors, ScanErrorEx, "Scan is not initialized", true);
    /**
     * check that computed start and stop positions are within bounds
     */
    if(!(MedMinorServoGeometry::check_axes_limits(m_start_position)))
        THROW_EX(MinorServoErrors, ScanErrorEx, "Start position out of limits", true);
    if(!(MedMinorServoGeometry::check_axes_limits(m_stop_position)))
        THROW_EX(MinorServoErrors, ScanErrorEx, "Stop position out of limits", true);
    if(!(MedMinorServoGeometry::check_axes_limits(m_central_position)))
        THROW_EX(MinorServoErrors, ScanErrorEx, "Central position out of limits", true);
    /**
     * check that we can reach start position in time
     */
     double min_start_time = MedMinorServoGeometry::min_time(m_central_position,
                                                             m_start_position);
     TIMEVALUE now;
     IRA::CIRATools::getTime(now);
     if((m_starting_time + START_SCAN_TOLERANCE) >= (now.value().value + min_start_time * 10000000))
        THROW_EX(MinorServoErrors, ScanErrorEx, "Cannot reach start position in time", true);
    /**
     * check that we can perform the scan with given speed
     */
     if(m_total_time >= (static_cast<ACS::Time>(m_max_time * 10000000) +
                                                SCAN_TOLERANCE))
        THROW_EX(MinorServoErrors, ScanErrorEx, "Scan is too slow", true);
     if(m_total_time <= (static_cast<ACS::Time>(m_min_time * 10000000) + 
                                                SCAN_TOLERANCE))
        THROW_EX(MinorServoErrors, ScanErrorEx, "Scan is too fast", true);
    return true;
}

ACS::Time 
MedMinorServoScan::getMovementTime()
{
    double starting_offset_time = MedMinorServoGeometry::min_time(
                                    m_central_position, 
                                    m_start_position);
    return m_starting_time - static_cast<ACS::Time>(starting_offset_time * 10000000);
}


