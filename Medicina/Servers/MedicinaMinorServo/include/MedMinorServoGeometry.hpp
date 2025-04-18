#ifndef MEDMINORSERVOGEOMETRY_HPP
#define MEDMINORSERVOGEOMETRY_HPP

#include <cmath>
#include <vector>
#include <string>
#include <limits>

#include <acsutil.h>

#include <IRA>

#include "MedMinorServoProtocol.hpp"
#include "MedMinorServoTime.hpp"
#include "MedMinorServoConstants.hpp"

/**
 * We want to translate hardware generated error codes into software generated
 * ones so that we keep a layer of separation between the two. This is not
 * unuseful replication of MedMinorServoProtocol.hpp
 */
enum MedMinorServoMode {
    MED_MINOR_SERVO_LOCAL,
    MED_MINOR_SERVO_INTERLOCK,
    MED_MINOR_SERVO_SR_BLOCK,
    MED_MINOR_SERVO_PFR_BLOCK,
    MED_MINOR_SERVO_SYSTEM_BLOCK,
    MED_MINOR_SERVO_PRIMARY_DISABLED,
    MED_MINOR_SERVO_SECONDARY_DISABLED,

    MED_MINOR_SERVO_PRIMARY,
    MED_MINOR_SERVO_SECONDARY,
    MED_MINOR_SERVO_OFFSET,
    MED_MINOR_SERVO_TRANSFER_TO_PRIMARY,
    MED_MINOR_SERVO_TRANSFER_TO_SECONDARY,
};

class MinorServoGeometryError: public std::runtime_error
{
    public:
        MinorServoGeometryError(const char *msg): std::runtime_error(std::string(msg)){};
};

class MinorServoAxisNameError: public std::runtime_error
{
    public:
        MinorServoAxisNameError(const char *msg): std::runtime_error(std::string(msg)){};
};

class MinorServoAxisLimitError: public std::runtime_error
{
    public:
        MinorServoAxisLimitError(const char *msg): std::runtime_error(std::string(msg)){};
};

class MedMinorServoPosition
{
    public:
        MedMinorServoPosition();
        MedMinorServoPosition(double x,
                              double y,
                              double z,
                              double theta_x,
                              double theta_y,
                              MedMinorServoMode mode,
                              ACS::Time time);
        virtual ~MedMinorServoPosition();
        MedMinorServoPosition(const MedMinorServoPosition& position);
        MedMinorServoPosition& operator=(const MedMinorServoPosition& position);
        MedMinorServoPosition& operator-=(const MedMinorServoPosition& position);
        MedMinorServoPosition& operator+=(const MedMinorServoPosition& position);
        MedMinorServoPosition& operator*=(double scale);
        bool operator==(const MedMinorServoPosition& position);
        bool earlier(const MedMinorServoPosition& position) const;
        bool earlier(const ACS::Time& time) const;
        bool later(const MedMinorServoPosition& position);
        bool later(const ACS::Time& time);
        const char * getStatusString();
        /**
         * check wether the mode is an error code or a success code
         */ 
        bool is_success_position();
        /**
         * return axes values only for actually used ones
         */
        std::vector<double> get_axes_positions();
        /**
         * return position of named axis
         * @param axis: the name of the axis
         */
        double get_axis_position(const char*) throw (MinorServoAxisNameError);

        double x;
        double y;
        double z;
        double theta_x;
        double theta_y;
        MedMinorServoMode mode;
        ACS::Time time;
    
    private:
        MedMinorServoConstants *medMinorServoConstants;
};

/**
 * rationale for this choice of operators implementation is described at:
 * http://www.boost.org/doc/libs/1_54_0/libs/utility/operators.htm#symmetry
 */

MedMinorServoPosition 
operator+(MedMinorServoPosition, const MedMinorServoPosition&);

MedMinorServoPosition 
operator-(MedMinorServoPosition, const MedMinorServoPosition&);

MedMinorServoPosition 
operator*(MedMinorServoPosition, double);

MedMinorServoPosition 
operator*(double, MedMinorServoPosition);

/**
 * Static methods used for translating coordinates expressed
 * in the cartesian reference into axes actuators values
 */
class MedMinorServoGeometry
{
    public:
        static MEDMINORSERVOSETPOS positionToAxes(const MedMinorServoPosition& position);
        static MedMinorServoPosition positionFromAxes(const MEDMINORSERVOSTATUS& status);
        static MedMinorServoPosition interpolate(
                           const MedMinorServoPosition& before_position,
                           const MedMinorServoPosition& after_position,
                           ACS::Time time);
        static MedMinorServoPosition abs(const MedMinorServoPosition& position);
        static double min_time(const MedMinorServoPosition& start, 
                               const MedMinorServoPosition& stop);
        static double max_time(const MedMinorServoPosition& start, 
                               const MedMinorServoPosition& stop);
        static MedMinorServoPosition get_primary_tolerance();
        static MedMinorServoPosition get_secondary_tolerance();
        static bool check_axes_limits(const MedMinorServoPosition& position);
    private:
        static double _check_axis_limit(const double position, 
                                        const MedMinorServoAxis axis);
        static 
        double _get_inverse_yp(const MedMinorServoPosition& position);
        static 
        double _get_inverse_zp(const MedMinorServoPosition& position);
        static 
        double _get_inverse_x(const MedMinorServoPosition& position);
        static 
        double _get_inverse_y(const MedMinorServoPosition& position);
        static 
        double _get_inverse_z1(const MedMinorServoPosition& position);
        static 
        double _get_inverse_z2(const MedMinorServoPosition& position);
        static 
        double _get_inverse_z3(const MedMinorServoPosition& position);
        static double _get_direct_yp(const MEDMINORSERVOSTATUS actual_position);
        static double _get_direct_zp(const MEDMINORSERVOSTATUS actual_position);
        static double _get_direct_x(const MEDMINORSERVOSTATUS actual_position);
        static double _get_direct_y(const MEDMINORSERVOSTATUS actual_position);
        static double _get_direct_z(const MEDMINORSERVOSTATUS actual_position);
        static double _get_direct_theta_x(const MEDMINORSERVOSTATUS actual_position);
        static double _get_direct_theta_y(const MEDMINORSERVOSTATUS actual_position);
};

#endif

