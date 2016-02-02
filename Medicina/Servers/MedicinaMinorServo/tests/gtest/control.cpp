#include "gtest/gtest.h"
#include <IRA>
#include <DateTime.h>

#include "MedMinorServoProtocol.hpp"
#include "MedMinorServoGeometry.hpp"
#include "PositionQueue.hpp"
#include "MedMinorServoControl.hpp"
#include "MedMinorServoTime.hpp"
#include "testutils.hpp"

class TestControl : public ::testing::Test
{
    public: 
        TestControl();
        MedMinorServoControl_sp m_control;
};

TestControl::TestControl()
{
    m_control = get_servo_control();
}

TEST_F(TestControl, can_get_position)
{
    MedMinorServoPosition position = m_control->get_position();
    ASSERT_NE(position.time, (ACS::Time)0);
}

TEST_F(TestControl, servo_time_passes_by_correctly)
{
    int wait_sec = 5;
    unsigned long long delta = 5 * 10000000;
    unsigned long long error = 1000000; // tolerate 1/100 sec error 
    m_control->update_position();
    MedMinorServoPosition position = m_control->get_position();
    IRA::CIRATools::Wait(wait_sec, 0);
    m_control->update_position();
    MedMinorServoPosition position_final = m_control->get_position();
    ASSERT_NEAR(position.time,
                position_final.time - delta,
                error);
}

TEST_F(TestControl, get_recent_position_time_correctly)
{
    m_control->update_position();
    MedMinorServoPosition position = m_control->get_position();
    position.z += 5;
    position.mode = MED_MINOR_SERVO_SECONDARY;
    m_control->set_position(position);
    m_control->update_position();
    position = m_control->get_position();
    ACS::Time now = IRA::CIRATools::getACSTime();
    ASSERT_NEAR(position.time, now, (ACS::Time)100000);
}

