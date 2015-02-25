#ifndef MEDMINORSERVOSCAN_HPP
#define MEDMINORSERVOSCAN_HPP

#include <string>

#include <IRA>
#include <MinorServoErrors.h>

#include "macros.def"
#include "MedMinorServoGeometry.hpp"
#include "MedMinorServoConstants.hpp"

#define START_SCAN_TOLERANCE 10000000 //1 sec.
#define SCAN_TOLERANCE 1000000 //0.1 sec.

using namespace std; 

class MedMinorServoScan
{
    public:
        MedMinorServoScan();
        MedMinorServoScan(const MedMinorServoPosition central_position,
                          const ACS::Time starting_time,
                          double range,
                          const ACS::Time total_time,
                          const string axis_code,
                          bool was_tracking);
        virtual ~MedMinorServoScan();
        void init(const MedMinorServoPosition central_position,
                  const ACS::Time starting_time,
                  double range,
                  const ACS::Time total_time,
                  const string axis_code,
                  bool was_tracking);
        bool check() throw (MinorServoErrors::ScanErrorEx);
        MedMinorServoPosition getCentralPosition(){return m_central_position;};
        MedMinorServoPosition getStartPosition(){return m_start_position;};
        MedMinorServoPosition getStopPosition(){return m_stop_position;};
        string getAxisCode(){return m_axis_code;};
        ACS::Time getStartingTime(){return m_starting_time;};
        ACS::Time getMovementTime();
        ACS::Time getTotalTime(){return m_total_time;};
        bool was_elevation_tracking(){ return m_was_tracking;};
    private:
        MedMinorServoPosition m_central_position, m_start_position, m_stop_position;
        ACS::Time m_starting_time, m_total_time;
        double m_range, m_min_time, m_max_time;
        string m_axis_code;
        bool m_initialized, m_was_tracking;
};

#endif

