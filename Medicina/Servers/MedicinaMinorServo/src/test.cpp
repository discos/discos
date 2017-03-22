#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Channel
#define BOOST_AUTO_TEST_MAIN

#include <boost/test/auto_unit_test.hpp>
//#include <boost/test/unit_test.hpp>

#include <IRA>
#include <DateTime.h>

#include "MedMinorServoModbus.hpp"
#include "MedMinorServoProtocol.hpp"
#include "MedMinorServoGeometry.hpp"
#include "MedMinorServoControl.hpp"
#include "MedMinorServoTime.hpp"
#include "testutils.hpp"

BOOST_AUTO_TEST_CASE(create_modbus)
{
    BOOST_MESSAGE("modbus channel creation");
    std::cout << "modbus channel creation\n";
    MedMinorServoModbus *channel = new MedMinorServoModbus("192.168.51.30");
    BOOST_CHECK(channel != NULL);
    delete channel;
}


BOOST_AUTO_TEST_CASE(connect_modbus)
{
    BOOST_MESSAGE("modbus channel connection");
    std::cout << "modbus channel connection\n";
    MedMinorServoModbus_sp channel = get_med_modbus_channel("192.168.51.30");
    BOOST_CHECK(channel->is_connected());
}

/*BOOST_AUTO_TEST_CASE(can_write)
{
    BOOST_MESSAGE("modbus channel write");
    std::cout << "modbus channel write\n";
    MEDMINORSERVOSETPOS command;
    command.enable = 1;
    command.escs = 1;
    MedMinorServoModbus_sp channel = get_med_modbus_channel("192.167.189.207");
    BOOST_CHECK_NO_THROW(channel->write_command(command));
}*/

/*BOOST_AUTO_TEST_CASE(can_write_position)
{
    BOOST_MESSAGE("modbus channel write position");
    std::cout << "modbus channel write position\n";
    MedMinorServoPosition position(10,
                                    10,
                                    10,
                                    10,
                                    10,
                                    MED_MINOR_SERVO_PRIMARY,
                                    200);
    MEDMINORSERVOSETPOS command = MedMinorServoGeometry::positionToAxes(position);
    MedMinorServoModbus_sp channel = get_med_modbus_channel("192.167.189.207");
    BOOST_CHECK_NO_THROW(channel->write_command(command));
    std::cout << "sent position\n";
    print_position(position);
    do{
        std::cout << "offset:\n";
        print_position(controller.get_delta_position());
        IRA::CIRATools::Wait(0,100);
    }while(!(controller.is_tracking()));
}*/

BOOST_AUTO_TEST_CASE(read_time)
{
    BOOST_MESSAGE("modbus channel read time");
    std::cout << "modbus channel read time\n";
    MedMinorServoModbus_sp channel = get_med_modbus_channel("192.168.51.30");
    MEDMINORSERVOSTATUS status = channel->read_status();
    IRA::CString outstring;
    ACS::Time servo_time = MedMinorServoTime::servoToACSTime(status.time);
    BOOST_CHECK(IRA::CIRATools::timeToStr(servo_time, outstring));
    std::cout << "read time: " << outstring << '\n';
}

BOOST_AUTO_TEST_CASE(reset)
{
    BOOST_MESSAGE("minor servo reset test");
    std::cout << "minor servo reset test\n";
    MedMinorServoControl controller("192.168.51.30");
    controller.update_position();
    controller.reset();
    MedMinorServoPosition actual_position = controller.update_position();
    BOOST_CHECK(actual_position.is_success_position());
}
/**
BOOST_AUTO_TEST_CASE(servo_control)
{
    BOOST_MESSAGE("minor servo control test");
    std::cout << "minor servo control test\n";
    MedMinorServoControl controller("192.167.189.207");
    MedMinorServoPosition write_position = get_random_position_from_status();
    controller.update_position();
    MedMinorServoPosition read_position = controller.get_position();
    std::cout << "READ POSITION:\n";
    print_position(read_position);
    MedMinorServoPosition offset_position;
    std::cout << "WRITE POSITION:\n";
    print_position(write_position);
    controller.set_position(write_position);
    do{
        controller.update_position();
        std::cout << "offset:\n";
        print_position(controller.get_position_offset());
        IRA::CIRATools::Wait(1,0);
    }while(!(controller.is_tracking()));
}

BOOST_AUTO_TEST_CASE(commute)
{
    BOOST_MESSAGE("commute test");
    std::cout << "commute test\n";
    MedMinorServoControl controller("192.167.189.207");
    MedMinorServoPosition primary;
    primary.mode = MED_MINOR_SERVO_PRIMARY;
    MedMinorServoPosition secondary;
    secondary.mode = MED_MINOR_SERVO_SECONDARY;
    MedMinorServoPosition destination;
    MedMinorServoPosition actual_position = controller.update_position();
    if(actual_position.mode == MED_MINOR_SERVO_PRIMARY)
    {
        std::cout << "found state: primary\n";
        destination = secondary;
    }else{
        std::cout << "found state: secondary\n";
        destination = primary;
    }
    std::cout << "commuting\n";
    IRA::CTimer _timer;
    controller.set_position(destination);
    do{
        actual_position = controller.update_position();
        //print_position(controller.get_position());
    }while(actual_position.mode != destination.mode);
    std::cout << "commuted in " << _timer.elapsed() / 10000000 << " seconds\n";
    BOOST_CHECK_EQUAL(actual_position.mode, destination.mode);
}
**/
