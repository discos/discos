#include "MedMinorServoConstants.hpp"
#include "MedMinorServoGeometry.hpp"

MedMinorServoPosition::MedMinorServoPosition() : 
    x(0),
    y(0),
    z(0),
    theta_x(0),
    theta_y(0),
    time(0)
{}

MedMinorServoPosition::MedMinorServoPosition(double x,
                                             double y,
                                             double z,
                                             double theta_x,
                                             double theta_y,
                                             MedMinorServoMode mode,
                                             ACS::Time time):
    x(x),
    y(y),
    z(z),
    theta_x(theta_x),
    theta_y(theta_y),
    mode(mode),
    time(time)
{}

MedMinorServoPosition::~MedMinorServoPosition()
{}

MedMinorServoPosition::MedMinorServoPosition(const MedMinorServoPosition& position)
{
    this->x = position.x;
    this->y = position.y;
    this->z = position.z;
    this->theta_x = position.theta_x;
    this->theta_y = position.theta_y;
    this->mode = position.mode;
    this->time = position.time;
}

MedMinorServoPosition&
MedMinorServoPosition::operator=(const MedMinorServoPosition& position)
{
    this->x = position.x;
    this->y = position.y;
    this->z = position.z;
    this->theta_x = position.theta_x;
    this->theta_y = position.theta_y;
    this->mode = position.mode;
    this->time = position.time;
    return *this;
}

bool
MedMinorServoPosition::operator==(const MedMinorServoPosition& position)
{
    return ((this->x == position.x) &&
            (this->y == position.y) &&
            (this->z == position.z) &&
            (this->theta_x == position.theta_x) &&
            (this->theta_y == position.theta_y) &&
            (this->mode == position.mode) &&
            (this->time == position.time));
}

MedMinorServoPosition&
MedMinorServoPosition::operator+=(const MedMinorServoPosition& position)
{
    if((this->mode != position.mode) &&
       (this->mode != MED_MINOR_SERVO_OFFSET) &&
       (position.mode != MED_MINOR_SERVO_OFFSET))
       throw MinorServoGeometryError("addition of incompatible minor servo positions");
    this->x += position.x;
    this->y += position.y;
    this->z += position.z;
    this->theta_x += position.theta_x;
    this->theta_y += position.theta_y;
    return *this;
}

MedMinorServoPosition&
MedMinorServoPosition::operator-=(const MedMinorServoPosition& position)
{
    if((this->mode != position.mode) &&
       (this->mode != MED_MINOR_SERVO_OFFSET) &&
       (position.mode != MED_MINOR_SERVO_OFFSET))
       throw MinorServoGeometryError("subtraction of incompatible minor servo positions");
    this->x -= position.x;
    this->y -= position.y;
    this->z -= position.z;
    this->theta_x -= position.theta_x;
    this->theta_y -= position.theta_y;
    return *this;
}

MedMinorServoPosition&
MedMinorServoPosition::operator*=(double scale)
{
    this->x *= scale;
    this->y *= scale;
    this->z *= scale;
    this->theta_x *= scale;
    this->theta_y *= scale;
    return *this;
}

bool
MedMinorServoPosition::is_success_position()
{
    if((mode == MED_MINOR_SERVO_PRIMARY)||
       (mode == MED_MINOR_SERVO_SECONDARY)||
       (mode == MED_MINOR_SERVO_OFFSET)||
       (mode == MED_MINOR_SERVO_TRANSFER_TO_PRIMARY)||
       (mode == MED_MINOR_SERVO_TRANSFER_TO_SECONDARY))
        return true;
    else
        return false;
}


MedMinorServoPosition
operator+(MedMinorServoPosition lhs, const MedMinorServoPosition& rhs)
{
    return lhs += rhs;
}

MedMinorServoPosition
operator-(MedMinorServoPosition lhs, const MedMinorServoPosition& rhs)
{
    return lhs -= rhs;
}

MedMinorServoPosition
operator*(MedMinorServoPosition lhs, double scale)
{
    return lhs *= scale;
}

MedMinorServoPosition
operator*(double scale, MedMinorServoPosition rhs)
{
    return rhs *= scale;
}

bool
MedMinorServoPosition::earlier(const ACS::Time& time) const
{
    return (this->time < time);
}

bool
MedMinorServoPosition::earlier(const MedMinorServoPosition& position) const
{
    return earlier(position.time);
}

bool
MedMinorServoPosition::later(const ACS::Time& time)
{
    return (this->time > time);
}

bool
MedMinorServoPosition::later(const MedMinorServoPosition& position)
{
    return later(position.time);
}

std::vector<double>
MedMinorServoPosition::get_axes_positions()
{
    std::vector<double> res;
    if((mode == MED_MINOR_SERVO_PRIMARY)||
       (mode == MED_MINOR_SERVO_TRANSFER_TO_PRIMARY))
    {
        res.push_back(y);
        res.push_back(z);
    }else{
        res.push_back(x);
        res.push_back(y);
        res.push_back(z);
        res.push_back(theta_x);
        res.push_back(theta_y);
    }
    return res;
}

MedMinorServoPosition
MedMinorServoGeometry::interpolate(const MedMinorServoPosition& before_position,
                                   const MedMinorServoPosition& after_position,
                                   ACS::Time time)
{
    if(after_position.earlier(before_position))
       throw MinorServoGeometryError("cannot interpolate with previous time value"); 
    double linear_factor = (double)(time - before_position.time) / 
                           (double)(after_position.time - before_position.time);
    MedMinorServoPosition difference = after_position - before_position;
    MedMinorServoPosition augment = linear_factor * difference;
    MedMinorServoPosition interpolation = before_position + augment;
    //MedMinorServoPosition interpolation = before_position + 
    //                                      (linear_factor * 
    //                                      (after_position - before_position));
    interpolation.time = time;
    interpolation.mode = after_position.mode;
    return interpolation;
}

bool
MedMinorServoGeometry::check_axes_limits(const MedMinorServoPosition& position)
{
    switch(position.mode)
    {
        case MED_MINOR_SERVO_PRIMARY:
            try{
                _get_inverse_yp(position);
                _get_inverse_zp(position);
            }catch(MinorServoAxisLimitError& ex)
            {
                return false;
            }
            break;
        case MED_MINOR_SERVO_SECONDARY:
            try{
                _get_inverse_x(position);
                _get_inverse_y(position);
                _get_inverse_z1(position);
                _get_inverse_z2(position);
                _get_inverse_z3(position);
            }catch(MinorServoAxisLimitError& ex)
            {
                return false;
            }
            break;
        default:
            return true;
    }
    return true;
}

double 
MedMinorServoGeometry::_check_axis_limit(const double position,
                                         const MedMinorServoAxis axis)
{
    if(position < axis.position_min){
        throw MinorServoAxisLimitError("axis too low");
        //return axis.position_min;
    }
    if(position > axis.position_max){
        throw MinorServoAxisLimitError("axis too high");
        //return axis.position_max;
    }
    return position;
}

MEDMINORSERVOSETPOS
MedMinorServoGeometry::positionToAxes(const MedMinorServoPosition& position)
{
    MEDMINORSERVOSETPOS command;
    switch(position.mode)
    {
        case(MED_MINOR_SERVO_PRIMARY):
            command.mode = 0;       
            command.pos_x_yp = _get_inverse_yp(position);
            command.pos_y_zp = _get_inverse_zp(position);
            break;
        case(MED_MINOR_SERVO_SECONDARY):
            command.mode = 1;       
            command.pos_x_yp = _get_inverse_x(position);
            command.pos_y_zp = _get_inverse_y(position);
            command.pos_z1 = _get_inverse_z1(position);
            command.pos_z2 = _get_inverse_z2(position);
            command.pos_z3 = _get_inverse_z3(position);
            break;
        default:
            throw MinorServoGeometryError("Invalid mode converting position to command");
    }
    command.time = MedMinorServoTime::ACSToServoTime(position.time);
    return command;
}


MedMinorServoPosition
MedMinorServoGeometry::positionFromAxes(const MEDMINORSERVOSTATUS& status)
{
    MedMinorServoPosition result;
    switch(status.system_status)
    {
        case(MED_MINOR_SERVO_STATUS_PRIMARY): //PRIMARY FOCUS
            result = MedMinorServoPosition(
                0,
                _get_direct_yp(status),
                _get_direct_zp(status),
                0,
                0,
                MED_MINOR_SERVO_PRIMARY,
                MedMinorServoTime::servoToACSTime(status.time));
            break;
        case(MED_MINOR_SERVO_STATUS_TRANSFER_TO_PRIMARY): //PRIMARY FOCUS
            result = MedMinorServoPosition(
                0,
                _get_direct_yp(status),
                _get_direct_zp(status),
                0,
                0,
                MED_MINOR_SERVO_TRANSFER_TO_PRIMARY,
                MedMinorServoTime::servoToACSTime(status.time));
            break;
        case(MED_MINOR_SERVO_STATUS_PRIMARY_DISABLED): //PRIMARY FOCUS
            result = MedMinorServoPosition(
                0,
                _get_direct_yp(status),
                _get_direct_zp(status),
                0,
                0,
                MED_MINOR_SERVO_PRIMARY_DISABLED,
                MedMinorServoTime::servoToACSTime(status.time));
            break;
        case(MED_MINOR_SERVO_STATUS_PFR_BLOCK): //PRIMARY FOCUS
            result = MedMinorServoPosition(
                0,
                _get_direct_yp(status),
                _get_direct_zp(status),
                0,
                0,
                MED_MINOR_SERVO_PFR_BLOCK,
                MedMinorServoTime::servoToACSTime(status.time));
            break;
        case(MED_MINOR_SERVO_STATUS_SECONDARY):
            result = MedMinorServoPosition(
                _get_direct_x(status),
                _get_direct_y(status),
                _get_direct_z(status),
                _get_direct_theta_x(status),
                _get_direct_theta_y(status),
                MED_MINOR_SERVO_SECONDARY,
                MedMinorServoTime::servoToACSTime(status.time));
            break;
        case(MED_MINOR_SERVO_STATUS_TRANSFER_TO_SECONDARY):
            result = MedMinorServoPosition(
                _get_direct_x(status),
                _get_direct_y(status),
                _get_direct_z(status),
                _get_direct_theta_x(status),
                _get_direct_theta_y(status),
                MED_MINOR_SERVO_TRANSFER_TO_SECONDARY,
                MedMinorServoTime::servoToACSTime(status.time));
            break;
        case(MED_MINOR_SERVO_STATUS_SECONDARY_DISABLED):
            result = MedMinorServoPosition(
                _get_direct_x(status),
                _get_direct_y(status),
                _get_direct_z(status),
                _get_direct_theta_x(status),
                _get_direct_theta_y(status),
                MED_MINOR_SERVO_SECONDARY_DISABLED,
                MedMinorServoTime::servoToACSTime(status.time));
            break;
        case(MED_MINOR_SERVO_STATUS_SR_BLOCK):
            result = MedMinorServoPosition(
                _get_direct_x(status),
                _get_direct_y(status),
                _get_direct_z(status),
                _get_direct_theta_x(status),
                _get_direct_theta_y(status),
                MED_MINOR_SERVO_SR_BLOCK,
                MedMinorServoTime::servoToACSTime(status.time));
            break;
        case(MED_MINOR_SERVO_STATUS_SYSTEM_BLOCK):
            result = MedMinorServoPosition(
                _get_direct_x(status),
                _get_direct_y(status),
                _get_direct_z(status),
                _get_direct_theta_x(status),
                _get_direct_theta_y(status),
                MED_MINOR_SERVO_SYSTEM_BLOCK,
                MedMinorServoTime::servoToACSTime(status.time));
            break;
        case(MED_MINOR_SERVO_STATUS_LOCAL):
            result = MedMinorServoPosition(0,0,0,0,0, MED_MINOR_SERVO_LOCAL,
                MedMinorServoTime::servoToACSTime(status.time));
            break;
        case(MED_MINOR_SERVO_STATUS_INTERLOCK):
            result = MedMinorServoPosition(0,0,0,0,0, MED_MINOR_SERVO_INTERLOCK,
                MedMinorServoTime::servoToACSTime(status.time));
            break;
        default:
            throw MinorServoGeometryError("Invalid system status converting status to position");
    }
    return result;
}

MedMinorServoPosition
MedMinorServoGeometry::abs(const MedMinorServoPosition& position)
{
    MedMinorServoPosition _abs_position = position;
    _abs_position.x = std::abs(position.x);
    _abs_position.y = std::abs(position.y);
    _abs_position.z = std::abs(position.z);
    _abs_position.theta_x = std::abs(position.theta_x);
    _abs_position.theta_y = std::abs(position.theta_y);
    return _abs_position;
}

MedMinorServoPosition
MedMinorServoGeometry::get_primary_tolerance()
{
    MEDMINORSERVOSTATUS status;
    status.mode = 0;
    status.time = 0;
    status.pos_x_yp = MINOR_SERVO_YP.position_error + MINOR_SERVO_YP.position_zero;
    status.pos_y_zp = MINOR_SERVO_ZP.position_error + MINOR_SERVO_ZP.position_zero;
    status.system_status = MED_MINOR_SERVO_STATUS_PRIMARY;
    MedMinorServoPosition position = positionFromAxes(status);
    position.mode = MED_MINOR_SERVO_OFFSET;
    position.time = 0;
    return abs(position);
}

MedMinorServoPosition
MedMinorServoGeometry::get_secondary_tolerance()
{
    MEDMINORSERVOSTATUS status;
    status.mode = 1;
    status.system_status = MED_MINOR_SERVO_STATUS_PRIMARY;
    status.time = 0;
    MedMinorServoPosition position;
    position.mode = MED_MINOR_SERVO_OFFSET;
    status.pos_x_yp = MINOR_SERVO_X.position_error + MINOR_SERVO_X.position_zero;
    status.pos_y_zp = MINOR_SERVO_Y.position_error + MINOR_SERVO_Y.position_zero;
    position.x = std::abs(_get_direct_x(status));
    position.y = std::abs(_get_direct_y(status));
    status.pos_y_zp = MINOR_SERVO_Y.position_zero - MINOR_SERVO_Y.position_error;
    status.pos_z1 = MINOR_SERVO_Z1.position_zero + MINOR_SERVO_Z1.position_error;
    status.pos_z2 = MINOR_SERVO_Z2.position_zero + MINOR_SERVO_Z2.position_error;
    status.pos_z3 = MINOR_SERVO_Z3.position_zero + MINOR_SERVO_Z3.position_error;
    position.z = std::abs(_get_direct_z(status));
    status.pos_z1 = MINOR_SERVO_Z1.position_zero + MINOR_SERVO_Z1.position_error;
    status.pos_z2 = MINOR_SERVO_Z2.position_zero - MINOR_SERVO_Z2.position_error;
    status.pos_z3 = MINOR_SERVO_Z3.position_zero - MINOR_SERVO_Z3.position_error;
    position.theta_x = std::abs(_get_direct_theta_x(status));
    status.pos_z2 = MINOR_SERVO_Z2.position_zero + MINOR_SERVO_Z2.position_error;
    status.pos_z3 = MINOR_SERVO_Z3.position_zero - MINOR_SERVO_Z3.position_error;
    position.theta_y = std::abs(_get_direct_theta_y(status));
    position.time = 0;
    return position;
}

/****************************************************
 * DIRECT EQUATIONS FOR PRIMARY POSITIONS
 ****************************************************/
double
MedMinorServoGeometry::_get_direct_yp(const MEDMINORSERVOSTATUS actual_position)
{
    if(actual_position.mode == 1) //GREGORIAN
        //TODO: raise exception? warning?
        return actual_position.pos_x_yp - MINOR_SERVO_X.position_zero;
    else{
        return actual_position.pos_x_yp - MINOR_SERVO_YP.position_zero;
    }
}

double
MedMinorServoGeometry::_get_direct_zp(const MEDMINORSERVOSTATUS actual_position)
{
    if(actual_position.mode == 1) //GREGORIAN
        //TODO: raise exception? warning?
        return 0.9903 * (actual_position.pos_y_zp - MINOR_SERVO_Y.position_zero);
    else{
        return actual_position.pos_y_zp - MINOR_SERVO_ZP.position_zero;
    }
}

/****************************************************
 * DIRECT EQUATIONS FOR SUBREFLECTOR POSITIONS
 ****************************************************/

double
MedMinorServoGeometry::_get_direct_x(const MEDMINORSERVOSTATUS actual_position)
{
    if(actual_position.mode == 1) //GREGORIAN
        return actual_position.pos_x_yp - MINOR_SERVO_X.position_zero;
    else{
        //TODO: raise exception? warning?
        return actual_position.pos_x_yp - MINOR_SERVO_YP.position_zero;
    }
}

double
MedMinorServoGeometry::_get_direct_y(const MEDMINORSERVOSTATUS actual_position)
{
    if(actual_position.mode == 1) //GREGORIAN
        return 0.9903 * (actual_position.pos_y_zp - MINOR_SERVO_Y.position_zero);
    else{
        //TODO: raise exception? warning?
        return actual_position.pos_y_zp - MINOR_SERVO_ZP.position_zero;
    }
}

double
MedMinorServoGeometry::_get_direct_z(const MEDMINORSERVOSTATUS actual_position)
{
    double z = 0.0;
    if(actual_position.mode == 1) //GREGORIAN
        z = - 0.1392 * (actual_position.pos_y_zp - MINOR_SERVO_Y.position_zero)
            + 0.3333 * (actual_position.pos_z1 - MINOR_SERVO_Z1.position_zero)
            + 0.3333 * (actual_position.pos_z2 - MINOR_SERVO_Z2.position_zero)
            + 0.3333 * (actual_position.pos_z3 - MINOR_SERVO_Z3.position_zero);
    else{
        //TODO: raise exception? warning?
    }
    return z;
}

double
MedMinorServoGeometry::_get_direct_theta_x(const MEDMINORSERVOSTATUS actual_position)
{
    double theta_x = 0.0;
    if(actual_position.mode == 1) //GREGORIAN
        theta_x = (actual_position.pos_z1 - MINOR_SERVO_Z1.position_zero) / 1791
                  -(actual_position.pos_z2 - MINOR_SERVO_Z2.position_zero) / 3582
                  -(actual_position.pos_z3 - MINOR_SERVO_Z3.position_zero) / 3582;
    else{
        //TODO: raise exception? warning?
    }
    return theta_x;
}

double
MedMinorServoGeometry::_get_direct_theta_y(const MEDMINORSERVOSTATUS actual_position)
{
    double theta_y = 0.0;
    if(actual_position.mode == 1) //GREGORIAN
        theta_y = (actual_position.pos_z2 - MINOR_SERVO_Z2.position_zero) / 2068
                  -(actual_position.pos_z3 - MINOR_SERVO_Z3.position_zero) / 2068;
    else{
        //TODO: raise exception? warning?
    }
    return theta_y;
}

/****************************************************
 * INVERSE EQUATIONS FOR PRIMARY FOCUS AXES POSITIONS
 ****************************************************/

double 
MedMinorServoGeometry::_get_inverse_yp(const MedMinorServoPosition& position)
{
    double yp = position.y + MINOR_SERVO_YP.position_zero;
    return _check_axis_limit(yp, MINOR_SERVO_YP);
}

double 
MedMinorServoGeometry::_get_inverse_zp(const MedMinorServoPosition& position)
{
    double zp = position.z + MINOR_SERVO_ZP.position_zero;
    return _check_axis_limit(zp, MINOR_SERVO_ZP);
}

/******************************************************
 * INVERSE EQUATIONS FOR SECONDARY FOCUS AXES POSITIONS
 ******************************************************/

double 
MedMinorServoGeometry::_get_inverse_x(const MedMinorServoPosition& position)
{
    double x = position.x + MINOR_SERVO_X.position_zero;
    return _check_axis_limit(x, MINOR_SERVO_X);
}

double 
MedMinorServoGeometry::_get_inverse_y(const MedMinorServoPosition& position)
{
    double y = 1.0098 * position.y + MINOR_SERVO_Y.position_zero;
    return _check_axis_limit(y, MINOR_SERVO_Y);
}

double 
MedMinorServoGeometry::_get_inverse_z1(const MedMinorServoPosition& position)
{
    double z1 = 0.1405 * position.y 
                + position.z
                + 1194 * position.theta_x
                + MINOR_SERVO_Z1.position_zero;
    return _check_axis_limit(z1, MINOR_SERVO_Z1);
}

double 
MedMinorServoGeometry::_get_inverse_z2(const MedMinorServoPosition& position)
{
    double z2 = 0.1405 * position.y 
                + position.z
                - 597 * position.theta_x
                + 1034 * position.theta_y
                + MINOR_SERVO_Z2.position_zero;
    return _check_axis_limit(z2, MINOR_SERVO_Z2);
}

double 
MedMinorServoGeometry::_get_inverse_z3(const MedMinorServoPosition& position)
{
    double z3 = 0.1405 * position.y 
                + position.z
                - 597 * position.theta_x
                - 1034 * position.theta_y
                + MINOR_SERVO_Z3.position_zero;
    return _check_axis_limit(z3, MINOR_SERVO_Z3);
}

