#include "MedMinorServoConstants.hpp"

MedMinorServoConstants* MedMinorServoConstants::instance = 0;

MedMinorServoConstants* MedMinorServoConstants::getInstance() {
    if (instance == 0) {
        instance = new MedMinorServoConstants();
    }
    return instance;
}

 
MedMinorServoConstants::MedMinorServoConstants(){
}