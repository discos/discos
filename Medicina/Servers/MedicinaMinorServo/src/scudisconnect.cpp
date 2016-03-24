#include <iostream>

#include "MedMinorServoControl.hpp"

int main(int argc, char *argv[])
{
    try{
        MedMinorServoControl_sp controller = get_servo_control();
        controller->disconnect();
        std::cout << "Subreflector disconnected\n";
    }catch(...){
        std::cout << "Cannot disconnect subreflector\n";
    }
    return 0;
}

