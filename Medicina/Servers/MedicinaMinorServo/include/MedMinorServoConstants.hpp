#ifndef MEDMINORSERVOCONSTANTS_HPP
#define MEDMINORSERVOCONSTANTS_HPP

typedef 
struct MedMinorServoAxis_tag
{
    const char *name;
    double position_min;
    double position_max;
    double position_zero;
    double position_error;
    double speed_min;
    double speed_max;
} MedMinorServoAxis;

const MedMinorServoAxis MINOR_SERVO_X = { "X", -80, 70, 0.0, 0.05, 0.01, 14.0 };
const MedMinorServoAxis MINOR_SERVO_YP = { "YP", -365, 0, 0.0, 0.05, 0.01, 8.0 };
const MedMinorServoAxis MINOR_SERVO_Y = { "Y", -80, 80, 0.0, 0.05, 0.01, 14.0 };
const MedMinorServoAxis MINOR_SERVO_ZP = { "ZP", -340, 5, 0.0, 0.05, 0.01, 8.0 };
const MedMinorServoAxis MINOR_SERVO_Z1 = { "Z1", -115, 115, 0.0, 0.05, 0.01, 14.0 };
const MedMinorServoAxis MINOR_SERVO_Z2 = { "Z2", -115, 115, 0.0, 0.05, 0.01, 14.0 };
const MedMinorServoAxis MINOR_SERVO_Z3 = { "Z3", -115, 115, 0.0, 0.05, 0.01, 14.0 };

#endif

