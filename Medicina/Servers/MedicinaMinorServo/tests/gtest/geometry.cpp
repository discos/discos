#include "gtest/gtest.h"
#include <IRA>
#include <DateTime.h>

#include "MedMinorServoProtocol.hpp"
#include "MedMinorServoGeometry.hpp"
#include "MedMinorServoTime.hpp"
#include "testutils.hpp"

class TestGeometry : public ::testing::Test
{
    public: 
        TestGeometry();
        MedMinorServoPosition secondary, offset_max, offset_min;
};

TestGeometry::TestGeometry() : 
    offset_min(0,0,0.01,0,0,MED_MINOR_SERVO_OFFSET, 10),
    offset_max(0,0,14,0,0,MED_MINOR_SERVO_OFFSET, 10),
    secondary(0, 0, 0, 0, 0, MED_MINOR_SERVO_SECONDARY, 10)
{}

TEST_F(TestGeometry, min_time_same_position){
    MedMinorServoPosition starting = secondary;
    MedMinorServoPosition ending = secondary;
    double min_time = MedMinorServoGeometry::min_time(starting, ending);
    ASSERT_EQ(min_time, 0);
}

TEST_F(TestGeometry, max_time_same_position){
    MedMinorServoPosition starting = secondary;
    MedMinorServoPosition ending = secondary;
    double max_time = MedMinorServoGeometry::max_time(starting, ending);
    ASSERT_GT(max_time, 100000000000);
}

TEST_F(TestGeometry, min_time){
    MedMinorServoPosition starting = secondary;
    MedMinorServoPosition ending = secondary + offset_max;
    double min_time = MedMinorServoGeometry::min_time(starting, ending);
    ASSERT_NEAR(min_time, 1, 0.01);
}

TEST_F(TestGeometry, max_time){
    MedMinorServoPosition starting = secondary;
    MedMinorServoPosition ending = secondary + offset_min;
    double max_time = MedMinorServoGeometry::max_time(starting, ending);
    ASSERT_NEAR(max_time, 1, 0.01);
}

