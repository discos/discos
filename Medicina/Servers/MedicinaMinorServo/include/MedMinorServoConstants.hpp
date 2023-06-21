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

using namespace std;

class MedMinorServoConstants {
public:
    static MedMinorServoConstants *getInstance();
    MedMinorServoAxis MINOR_SERVO_X ;
    MedMinorServoAxis MINOR_SERVO_YP ;
    MedMinorServoAxis MINOR_SERVO_Y ;
    MedMinorServoAxis MINOR_SERVO_ZP ;
    MedMinorServoAxis MINOR_SERVO_Z1 ;
    MedMinorServoAxis MINOR_SERVO_Z2 ;
    MedMinorServoAxis MINOR_SERVO_Z3 ;

private:
    MedMinorServoConstants();
    static MedMinorServoConstants *instance;

};


#endif