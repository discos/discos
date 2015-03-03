#include "gtest/gtest.h"
#include <IRA>
#include <DateTime.h>

#include "MedMinorServoProtocol.hpp"
#include "MedMinorServoGeometry.hpp"
#include "PositionQueue.hpp"
#include "MedMinorServoTime.hpp"
#include "testutils.hpp"

TEST(Position, creation){
    MedMinorServoPosition position(10,10,10,10,10,MED_MINOR_SERVO_PRIMARY, 10);
    ASSERT_EQ(position.x, 10);
    ASSERT_EQ(position.y, 10);
    ASSERT_EQ(position.z, 10);
    ASSERT_EQ(position.theta_x, 10);
    ASSERT_EQ(position.theta_y, 10);
    ASSERT_EQ(position.mode, MED_MINOR_SERVO_PRIMARY);
    ASSERT_EQ(position.time, 10);
}

TEST(Position, copy)
{
    MedMinorServoPosition position(10,10,10,10,10,MED_MINOR_SERVO_PRIMARY, 10);
    MedMinorServoPosition position2 = position;
    ASSERT_EQ(position, position2);
    MedMinorServoPosition position3(position2);
    ASSERT_EQ(position, position3);
}

TEST(Position, sum)
{
    MedMinorServoPosition position(10,10,10,10,10,MED_MINOR_SERVO_PRIMARY, 10);
    MedMinorServoPosition position2(1,1,1,1,1,MED_MINOR_SERVO_OFFSET, 10);
    MedMinorServoPosition position3 = position + position2;
    ASSERT_EQ(position3.x, 11);
    ASSERT_EQ(position3.y, 11);
    ASSERT_EQ(position3.z, 11);
    ASSERT_EQ(position3.theta_x, 11);
    ASSERT_EQ(position3.theta_y, 11);
    ASSERT_EQ(position3.mode, MED_MINOR_SERVO_PRIMARY);
}

TEST(Position, difference)
{
    MedMinorServoPosition position(10,10,10,10,10,MED_MINOR_SERVO_PRIMARY, 10);
    MedMinorServoPosition position2(1,1,1,1,1,MED_MINOR_SERVO_OFFSET, 10);
    MedMinorServoPosition position3 = position - position2;
    ASSERT_EQ(position3.x, 9);
    ASSERT_EQ(position3.y, 9);
    ASSERT_EQ(position3.z, 9);
    ASSERT_EQ(position3.theta_x, 9);
    ASSERT_EQ(position3.theta_y, 9);
    ASSERT_EQ(position3.mode, MED_MINOR_SERVO_PRIMARY);
}

TEST(Position, scale)
{
    MedMinorServoPosition position(10,10,10,10,10,MED_MINOR_SERVO_PRIMARY, 10);
    double scale = 3;
    MedMinorServoPosition position2= position * scale;
    ASSERT_EQ(position2.x, 30);
    ASSERT_EQ(position2.y, 30);
    ASSERT_EQ(position2.z, 30);
    ASSERT_EQ(position2.theta_x, 30);
    ASSERT_EQ(position2.theta_y, 30);
    ASSERT_EQ(position2.mode, MED_MINOR_SERVO_PRIMARY);
}

TEST(Position, interpolation)
{
    MedMinorServoPosition position1(10,10,10,10,10,MED_MINOR_SERVO_PRIMARY, 10);
    MedMinorServoPosition position2(20,20,20,20,20,MED_MINOR_SERVO_PRIMARY, 20);
    MedMinorServoPosition position3 = MedMinorServoGeometry::interpolate(position1, 
                                                                         position2,
                                                                         14);
    MedMinorServoPosition position4(14,14,14,14,14,MED_MINOR_SERVO_PRIMARY, 14);
    ASSERT_EQ(position3, position4);
}

