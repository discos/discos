#include "MedMinorServoConstants.hpp"

MedMinorServoConstants* MedMinorServoConstants::instance = 0;

MedMinorServoConstants* MedMinorServoConstants::getInstance() {
    if (instance == 0) {
        instance = new MedMinorServoConstants();
    }
    return instance;
}


MedMinorServoConstants::MedMinorServoConstants(){

    MINOR_SERVO_X = { "X", -80, 80, 0.0, 0.05, 0.01, 14.0 };
    MINOR_SERVO_YP = { "YP", -5, 395, 0.0, 0.05, 0.01, 8.0 };
    MINOR_SERVO_Y = { "Y", -80, 80, 0.0, 0.05, 0.01, 14.0 };
    MINOR_SERVO_ZP = { "ZP", -5, 345, 0.0, 0.05, 0.01, 8.0 };
    MINOR_SERVO_Z1 = { "Z1", -125, 125, 0.0, 0.05, 0.01, 14.0 };
    MINOR_SERVO_Z2 = { "Z2", -125, 125, 0.0, 0.05, 0.01, 14.0 };
    MINOR_SERVO_Z3 = { "Z3", -125, 125, 0.0, 0.05, 0.01, 14.0 };

}