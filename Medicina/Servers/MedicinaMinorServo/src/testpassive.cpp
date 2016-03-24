
#include <iostream>
#include <stdlib.h>

#include <boost/mpl/numeric_cast.hpp>

#include <IRA>
#include <DateTime.h>
#include "MedMinorServoGeometry.hpp"
#include "MedMinorServoControl.hpp"
#include "MedMinorServoModbus.hpp"
#include "MedMinorServoProtocol.hpp"
#include "testutils.hpp"

#define MAX_ACTION_TIME 5*60*10000000
int main(int argc, char *argv[])
{
    MedMinorServoPosition actual_position, next_position;
    ACS::Time timeout_value = 10000000ULL * 60ULL * 5ULL;
    std::cout << "set timeout to: " << timeout_value << '\n';
    bool timeout, again;
    again = true;
    char answer;
    IRA::CTimer timer;
    std::cout << "connect to modbus server\n";
    MedMinorServoControl controller("192.168.51.30");
    std::cout << "reset\n";
    std::cout << "==============================================\n\n";
    controller.reset();
    while(again){
        actual_position = controller.update_position();
        print_position(actual_position);
        std::cout << "read again (y/n)" << std::endl;
        std::cin.get(answer);
        if(answer == 'n')
            again = false;
        else 
            again = true;
    }
    std::cout << "bye" << std::endl;
    return 0;
}

