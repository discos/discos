#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Channel
#define BOOST_AUTO_TEST_MAIN

#include <iostream>
#include <boost/test/auto_unit_test.hpp>
//#include <boost/test/unit_test.hpp>

#include <IRA>
#include <DateTime.h>

#include "MedMinorServoProtocol.hpp"
#include "MedMinorServoGeometry.hpp"
#include "PositionQueue.hpp"
#include "MedMinorServoTime.hpp"
#include "testutils.hpp"

BOOST_AUTO_TEST_CASE(position_command_status)
{
    std::cout << "position to command to status to position\n";
    MedMinorServoPosition position = get_random_position_from_status();
    std::cout << "Random position:\n";
    print_position(position);
    MEDMINORSERVOSETPOS setpos = MedMinorServoGeometry::positionToAxes(position);
    print_setpos(setpos);
    MEDMINORSERVOSTATUS status;
    status.mode = setpos.mode;
    status.system_status = (status.mode == 0)?MED_MINOR_SERVO_STATUS_PRIMARY:MED_MINOR_SERVO_STATUS_SECONDARY;
    status.pos_x_yp = setpos.pos_x_yp;
    status.pos_y_zp = setpos.pos_y_zp;
    status.pos_z1 = setpos.pos_z1;
    status.pos_z2 = setpos.pos_z2;
    status.pos_z3 = setpos.pos_z3;
    print_status(status);
    MedMinorServoPosition position2 = MedMinorServoGeometry::positionFromAxes(status);
    std::cout << "Position from status:\n";
    print_position(position2);
    std::cout << "offset:\n";
    MedMinorServoPosition offset = MedMinorServoGeometry::abs(position2 - position);
    print_position(offset);
    /*MedMinorServoPosition primary_tolerance = MedMinorServoGeometry::get_primary_tolerance();
    MedMinorServoPosition secondary_tolerance = MedMinorServoGeometry::get_secondary_tolerance();
    if(position2.mode == 0)
      A*  BOOST_CHECK(difference < );*/
    BOOST_CHECK(position2.mode == position.mode);
}

BOOST_AUTO_TEST_CASE(position_creation)
{
    MedMinorServoPosition position(10,10,10,10,10,MED_MINOR_SERVO_PRIMARY, 10);
    BOOST_CHECK_EQUAL(position.x, 10);
    BOOST_CHECK_EQUAL(position.y, 10);
    BOOST_CHECK_EQUAL(position.z, 10);
    BOOST_CHECK_EQUAL(position.theta_x, 10);
    BOOST_CHECK_EQUAL(position.theta_y, 10);
    BOOST_CHECK_EQUAL(position.mode, MED_MINOR_SERVO_PRIMARY);
    BOOST_CHECK_EQUAL(position.time, 10);
}

BOOST_AUTO_TEST_CASE(position_copy)
{
    MedMinorServoPosition position(10,10,10,10,10,MED_MINOR_SERVO_PRIMARY, 10);
    MedMinorServoPosition position2 = position;
    BOOST_CHECK(position == position2);
    MedMinorServoPosition position3(position2);
    BOOST_CHECK(position == position3);
}


BOOST_AUTO_TEST_CASE(position_sum)
{
    MedMinorServoPosition position(10,10,10,10,10,MED_MINOR_SERVO_PRIMARY, 10);
    MedMinorServoPosition position2(1,1,1,1,1,MED_MINOR_SERVO_OFFSET, 10);
    MedMinorServoPosition position3 = position + position2;
    BOOST_CHECK_EQUAL(position3.x, 11);
    BOOST_CHECK_EQUAL(position3.y, 11);
    BOOST_CHECK_EQUAL(position3.z, 11);
    BOOST_CHECK_EQUAL(position3.theta_x, 11);
    BOOST_CHECK_EQUAL(position3.theta_y, 11);
    BOOST_CHECK_EQUAL(position3.mode, MED_MINOR_SERVO_PRIMARY);
}

BOOST_AUTO_TEST_CASE(position_difference)
{
    MedMinorServoPosition position(10,10,10,10,10,MED_MINOR_SERVO_PRIMARY, 10);
    MedMinorServoPosition position2(1,1,1,1,1,MED_MINOR_SERVO_OFFSET, 10);
    MedMinorServoPosition position3 = position - position2;
    BOOST_CHECK_EQUAL(position3.x, 9);
    BOOST_CHECK_EQUAL(position3.y, 9);
    BOOST_CHECK_EQUAL(position3.z, 9);
    BOOST_CHECK_EQUAL(position3.theta_x, 9);
    BOOST_CHECK_EQUAL(position3.theta_y, 9);
    BOOST_CHECK_EQUAL(position3.mode, MED_MINOR_SERVO_PRIMARY);
}

BOOST_AUTO_TEST_CASE(position_scale)
{
    MedMinorServoPosition position(10,10,10,10,10,MED_MINOR_SERVO_PRIMARY, 10);
    double scale = 3;
    MedMinorServoPosition position2= position * scale;
    BOOST_CHECK_EQUAL(position2.x, 30);
    BOOST_CHECK_EQUAL(position2.y, 30);
    BOOST_CHECK_EQUAL(position2.z, 30);
    BOOST_CHECK_EQUAL(position2.theta_x, 30);
    BOOST_CHECK_EQUAL(position2.theta_y, 30);
    BOOST_CHECK_EQUAL(position2.mode, MED_MINOR_SERVO_PRIMARY);
}


BOOST_AUTO_TEST_CASE(position_interpolation)
{
    MedMinorServoPosition position1(10,10,10,10,10,MED_MINOR_SERVO_PRIMARY, 10);
    MedMinorServoPosition position2(20,20,20,20,20,MED_MINOR_SERVO_PRIMARY, 20);
    MedMinorServoPosition position3 = MedMinorServoGeometry::interpolate(position1, 
                                                                         position2,
                                                                         14);
    MedMinorServoPosition position4(14,14,14,14,14,MED_MINOR_SERVO_PRIMARY, 14);
    //print_position(position3);
    //print_position(position4);
    BOOST_CHECK(position3 == position4);
}

BOOST_AUTO_TEST_CASE(position_queue)
{
    MedMinorServoPosition position1(10,10,10,10,10,MED_MINOR_SERVO_PRIMARY, 10);
    MedMinorServoPosition position2(20,20,20,20,20,MED_MINOR_SERVO_PRIMARY, 20);
    MedMinorServoPosition position3(30,30,30,30,30,MED_MINOR_SERVO_PRIMARY, 30);
    MedMinorServoPosition position4(14,14,14,14,14,MED_MINOR_SERVO_PRIMARY, 14);
    MedMinorServoPosition position5(40,40,40,40,40,MED_MINOR_SERVO_PRIMARY, 40);
    PositionQueue queue(3);
    queue.push(position1);
    BOOST_CHECK(queue.get_position() == position1);
    BOOST_CHECK(queue.get_position(15) == position1);
    queue.push(position2);
    BOOST_CHECK(queue.get_position() == position2);
    BOOST_CHECK(queue.get_position(10) == position1);
    BOOST_CHECK(queue.get_position(14) == position4);
    BOOST_CHECK(queue.get_position(30) == position2);
    queue.push(position3);
    queue.push(position5);
    BOOST_CHECK(queue.get_position() == position5);
    BOOST_CHECK(queue.get_position(10) == position2);
    queue.push(position4);
    BOOST_CHECK(queue.get_position(10) == position2);
    BOOST_CHECK(queue.get_position(100) == position5);
}

BOOST_AUTO_TEST_CASE(offset_creation)
{
    std::cout << "offset creation\n";
    MedMinorServoPosition primary_tolerance = MedMinorServoGeometry::get_primary_tolerance();
    MedMinorServoPosition secondary_tolerance = MedMinorServoGeometry::get_secondary_tolerance();
    std::cout << "Primary Tolerance:\n";
    print_position(primary_tolerance);
    std::cout << "Secondary Tolerance:\n";
    print_position(secondary_tolerance);
}

