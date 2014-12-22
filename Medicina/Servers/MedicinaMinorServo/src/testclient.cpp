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
    MedMinorServoPosition little_offset, big_offset;
    int counter = 0;

    little_offset.mode = MED_MINOR_SERVO_OFFSET;
    little_offset.x = 0;
    little_offset.y = 0;
    little_offset.z = 2.0;
    little_offset.theta_x = 0;
    little_offset.theta_y = 0;
    big_offset.mode = MED_MINOR_SERVO_OFFSET;
    big_offset.x = 0;
    big_offset.y = 0;
    big_offset.z = 3.5;
    big_offset.theta_x = 0;
    big_offset.theta_y = 0;

    ACS::Time timeout_value = 10000000ULL * 60ULL * 5ULL;
    std::cout << "set timeout to: " << timeout_value << '\n';
    bool timeout, reached;
    IRA::CTimer timer;
    std::cout << "connect to modbus server\n";
    MedMinorServoControl controller("192.168.51.30");
    std::cout << "reset\n";
    std::cout << "==============================================\n\n";
    controller.reset();
    while(true){
        std::cout << "Iteration: " << ++counter << '\n';
        actual_position = controller.update_position();
        //do{
        //    next_position = get_random_position_from_status();
        //}while(next_position.mode != MED_MINOR_SERVO_SECONDARY);
        do
        {
            next_position = get_random_secondary_position();
            next_position.z = 0;
        }while(!(MedMinorServoGeometry::check_axes_limits(next_position)));
        
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
            else{
                if (timer.elapsed() > (ACS::Time)(timeout_value)){
                    timeout = true;
                }
                controller.update_position();
                std::cout << "offset:\n";
                print_position(controller.get_position_offset());
                IRA::CIRATools::Wait(1,0);
            }
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
        std::cout << "==============================================\n";
        if(timeout)
            throw ServoTimeoutError("Houston abbiamo un problema");
        std::cout << "SLOW OFFSETTING POSITION:\n";
        try{
            next_position = actual_position + little_offset;
            print_position(next_position);
            controller.set_position_with_time(next_position, (double)20.0);
        //}catch(...){
        }catch(std::runtime_error & offset_error){
            std::cout << "\nerror offsetting position\n";
            std::cout << offset_error.what() << "\n";
            break;
        }
        IRA::CIRATools::Wait(20, 0);
        std::cout << "REACHED SLOW OFFSET POSITION:\n";
        actual_position = controller.update_position();
        print_position(actual_position);
        std::cout << "==============================================\n";
        if(timeout)
            throw ServoTimeoutError("Houston abbiamo un problema");
        std::cout << "\nFAST OFFSETTING POSITION:\n";
        for(int i=0; i<100; ++i)
        {
            std::cout << "*" << std::flush;
            try{
                next_position = actual_position + big_offset;
                controller.set_position(next_position);
            }catch(std::runtime_error & offset_error){
                std::cout << "\nerror offsetting position\n";
                std::cout << offset_error.what() << "\n";
                break;
            }
            IRA::CIRATools::Wait((long long)250000);
        }
        std::cout << "\n==============================================\n\n";
    }
}

