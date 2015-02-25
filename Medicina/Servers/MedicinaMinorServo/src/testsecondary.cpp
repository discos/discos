
#include <iostream>
#include <stdlib.h>
#include <time.h>

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
    srand48(time(NULL));

    MedMinorServoPosition actual_position, next_position;
    ACS::Time timeout_value = 10000000ULL * 60ULL * 5ULL;
    std::cout << "set timeout to: " << timeout_value << '\n';
    bool timeout, reached;
    IRA::CTimer timer;
    std::cout << "connect to modbus server\n";
    MedMinorServoControl controller("192.168.51.30");
    std::cout << "reset\n";
    std::cout << "==============================================\n\n";
    controller.reset();
    actual_position = controller.update_position();
    next_position = get_random_secondary_position();
    std::cout << "ACTUAL POSITION:\n";
    print_position(actual_position);
    std::cout << "NEXT POSITION:\n";
    print_position(next_position);
    timeout = false;
    reached = false;
    controller.set_position(next_position);
    timer.reset();
    while((!timeout) && 
          (!reached))
    {
        if(controller.is_tracking())
            reached = true;
        else
        {
            if (timer.elapsed() > (ACS::Time)(timeout_value)){
                timeout = true;
            }
            controller.update_position();
            std::cout << "offset:\n";
            print_position(controller.get_position_offset());
        }
        IRA::CIRATools::Wait(1,0);
    }
    if(timeout){
        std::cout << "Timeout ERROR\n";
    }
    else{ //reached position
        std::cout << "Elapsed: " << (double)(timer.elapsed() / 10000000.0) <<  "sec.\n";
    }
    std::cout << "REACHED POSITION:\n";
    actual_position = controller.update_position();
    print_position(actual_position);
    std::cout << "==============================================\n\n";
    if(timeout)
        throw ServoTimeoutError("Houston abbiamo un problema");
}

