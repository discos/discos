#include "gtest/gtest.h"
#include <IRA>
#include <DateTime.h>

#include "MedMinorServoProtocol.hpp"
#include "MedMinorServoGeometry.hpp"
#include "PositionQueue.hpp"
#include "MedMinorServoTime.hpp"
#include "testutils.hpp"

class TestPosition : public ::testing::Test
{
    public: 
        TestPosition();
        MedMinorServoPosition primary, offset, secondary;
};

TestPosition::TestPosition() : 
    primary(10, 10, 10, 10, 10, MED_MINOR_SERVO_PRIMARY, 10),
    offset(1,1,1,1,1,MED_MINOR_SERVO_OFFSET, 10),
    secondary(10, 10, 10, 10, 10, MED_MINOR_SERVO_SECONDARY, 10)
{}

TEST_F(TestPosition, creation){
    ASSERT_EQ(primary.x, 10);
    ASSERT_EQ(primary.y, 10);
    ASSERT_EQ(primary.z, 10);
    ASSERT_EQ(primary.theta_x, 10);
    ASSERT_EQ(primary.theta_y, 10);
    ASSERT_EQ(primary.mode, MED_MINOR_SERVO_PRIMARY);
    ASSERT_EQ(primary.time, 10);
}

TEST_F(TestPosition, copy)
{
    //test copy, also for simmetry and transitivity
    MedMinorServoPosition copied = primary;
    ASSERT_EQ(primary, copied);
    ASSERT_EQ(copied, primary);
    MedMinorServoPosition recopied(copied);
    ASSERT_EQ(primary, recopied);
}

TEST_F(TestPosition, sum)
{
    MedMinorServoPosition sum = primary + offset;
    ASSERT_EQ(sum.x, 11);
    ASSERT_EQ(sum.y, 11);
    ASSERT_EQ(sum.z, 11);
    ASSERT_EQ(sum.theta_x, 11);
    ASSERT_EQ(sum.theta_y, 11);
    ASSERT_EQ(sum.mode, MED_MINOR_SERVO_PRIMARY);
}

TEST_F(TestPosition, difference)
{
    MedMinorServoPosition diff = primary - offset;
    ASSERT_EQ(diff.x, 9);
    ASSERT_EQ(diff.y, 9);
    ASSERT_EQ(diff.z, 9);
    ASSERT_EQ(diff.theta_x, 9);
    ASSERT_EQ(diff.theta_y, 9);
    ASSERT_EQ(diff.mode, MED_MINOR_SERVO_PRIMARY);
}

TEST_F(TestPosition, scale)
{
    double scale = 3;
    MedMinorServoPosition scaled= primary * scale;
    ASSERT_EQ(scaled.x, 30);
    ASSERT_EQ(scaled.y, 30);
    ASSERT_EQ(scaled.z, 30);
    ASSERT_EQ(scaled.theta_x, 30);
    ASSERT_EQ(scaled.theta_y, 30);
    ASSERT_EQ(scaled.mode, MED_MINOR_SERVO_PRIMARY);
}

TEST_F(TestPosition, interpolation)
{
    MedMinorServoPosition primary2(20,20,20,20,20,MED_MINOR_SERVO_PRIMARY, 20);
    MedMinorServoPosition interpolated =
        MedMinorServoGeometry::interpolate(primary, primary2, 14);
    MedMinorServoPosition expected(14,14,14,14,14,MED_MINOR_SERVO_PRIMARY, 14);
    ASSERT_EQ(interpolated, expected);
}

