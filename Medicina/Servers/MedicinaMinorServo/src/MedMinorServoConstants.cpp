#include "MedMinorServoConstants.hpp"

MedMinorServoConstants* MedMinorServoConstants::instance = 0;

MedMinorServoConstants* MedMinorServoConstants::getInstance() {
    if (instance == 0) {
        instance = new MedMinorServoConstants();
    }
    return instance;
}

 
MedMinorServoConstants::MedMinorServoConstants(){

    MINOR_SERVO_X = { "X", 0, 0, 0.0, 0.05, 0.01, 14.0 };
    MINOR_SERVO_YP = { "YP", 0, 0, 0.0, 0.05, 0.01, 8.0 };
    MINOR_SERVO_Y = { "Y", 0, 0, 0.0, 0.05, 0.01, 14.0 };
    MINOR_SERVO_ZP = { "ZP", 0, 0 , 0.0, 0.05, 0.01, 8.0 };
    MINOR_SERVO_Z1 = { "Z1", 0, 0, 0.0, 0.05, 0.01, 14.0 };
    MINOR_SERVO_Z2 = { "Z2", 0, 0, 0.0, 0.05, 0.01, 14.0 };
    MINOR_SERVO_Z3 = { "Z3", 0, 0, 0.0, 0.05, 0.01, 14.0 };

}