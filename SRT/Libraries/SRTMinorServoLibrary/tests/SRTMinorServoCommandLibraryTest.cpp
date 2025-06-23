#include "gtest/gtest.h"
#include <IRA>
#include "SRTMinorServoCommandLibrary.h"

using namespace MinorServo;

TEST(SRTMinorServoCommandLibraryTest, status)
{
    EXPECT_EQ(SRTMinorServoCommandLibrary::status(), "STATUS\r\n");
    EXPECT_EQ(SRTMinorServoCommandLibrary::status("PFP"), "STATUS=PFP\r\n");
}

TEST(SRTMinorServoCommandLibraryTest, setup)
{
    EXPECT_EQ(SRTMinorServoCommandLibrary::setup("CCB"), "SETUP=CCB\r\n");
}

TEST(SRTMinorServoCommandLibraryTest, stow)
{
    EXPECT_EQ(SRTMinorServoCommandLibrary::stow("PFP"), "STOW=PFP,1\r\n");
    EXPECT_EQ(SRTMinorServoCommandLibrary::stow("PFP", 2), "STOW=PFP,2\r\n");
}

TEST(SRTMinorServoCommandLibraryTest, stop)
{
    EXPECT_EQ(SRTMinorServoCommandLibrary::stop("PFP"), "STOP=PFP\r\n");
}

TEST(SRTMinorServoCommandLibraryTest, preset)
{
    EXPECT_EQ(SRTMinorServoCommandLibrary::preset("PFP", std::vector<double>{0.,1.,2.,3.,4.,5.}), "PRESET=PFP,0.000000,1.000000,2.000000,3.000000,4.000000,5.000000\r\n");
}

TEST(SRTMinorServoCommandLibraryTest, programTrack)
{
    double start_time = IRA::CIRATools::getUNIXTime() + 3;
    unsigned long int trajectory_id = (unsigned long int)start_time;
    std::stringstream expected_answer;
    expected_answer << std::fixed << std::setprecision(6);
    expected_answer << "PROGRAMTRACK=PFP," << trajectory_id << ",0," << start_time << ",0.000000,1.000000,2.000000,3.000000,4.000000,5.000000\r\n";
    EXPECT_EQ(SRTMinorServoCommandLibrary::programTrack("PFP", trajectory_id, 0, std::vector<double>{0.,1.,2.,3.,4.,5.}, start_time), expected_answer.str());

    for(unsigned int i = 1; i < 10; i++)
    {
        expected_answer.str(std::string());
        expected_answer << "PROGRAMTRACK=PFP," << trajectory_id << "," << i << ",*,0.000000,1.000000,2.000000,3.000000,4.000000,5.000000\r\n";
        EXPECT_EQ(SRTMinorServoCommandLibrary::programTrack("PFP", trajectory_id, i, std::vector<double>{0.,1.,2.,3.,4.,5.}), expected_answer.str());
    }
}

TEST(SRTMinorServoCommandLibraryTest, offset)
{
    EXPECT_EQ(SRTMinorServoCommandLibrary::offset("PFP", std::vector<double>{0.,1.,2.,3.,4.,5.}), "OFFSET=PFP,0.000000,1.000000,2.000000,3.000000,4.000000,5.000000\r\n");
}

TEST(SRTMinorServoCommandLibraryTest, parseAnswer)
{
    // Minimal correct answer
    std::string answer = "OUTPUT:GOOD,1665743366.654321\r\n";
    SRTMinorServoAnswerMap args;
    args.put("OUTPUT", "GOOD");
    args.put("TIMESTAMP", 1665743366.654321);
    EXPECT_EQ(SRTMinorServoCommandLibrary::parseAnswer(answer), args);
    EXPECT_EQ(args.getTimestamp(), 138850361666543210);
    EXPECT_TRUE(args.checkOutput());
    SRTMinorServoAnswerMap other;
    other.put("OTHER", 123456);
    args += other;
    EXPECT_EQ(args.get<int>("OTHER"), 123456);
    EXPECT_EQ(args.getTimestamp(), 138850361666543210);
    EXPECT_TRUE(args.checkOutput());

    // Complete correct answer
    answer = "OUTPUT:GOOD,1665743366.123456,CURRENT_CONFIG=21|SIMULATION_ENABLED=34|PLC_TIME=78|PLC_VERSION=69|CONTROL=14|POWER=38|EMERGENCY=69|ENABLED=51|OPERATIVE_MODE=94\r\n";
    args.clear();
    args.put("OUTPUT", "GOOD");
    args.put("TIMESTAMP", 1665743366.123456);
    args.put("CURRENT_CONFIG", 21);
    args.put("SIMULATION_ENABLED", 34);
    args.put("PLC_TIME", 78);
    args.put("PLC_VERSION", 69);
    args.put("CONTROL", 14);
    args.put("POWER", 38);
    args.put("EMERGENCY", 69);
    args.put("ENABLED", 51);
    args.put("OPERATIVE_MODE", 94);
    EXPECT_EQ(SRTMinorServoCommandLibrary::parseAnswer(answer), args);

    // Missing timestamp
    answer = "OUTPUT:GOOD,CURRENT_CONFIG=21|SIMULATION_ENABLED=34|PLC_TIME=78|PLC_VERSION=69|CONTROL=14|POWER=38|EMERGENCY=69|ENABLED=51|OPERATIVE_MODE=94\r\n";
    args.clear();
    EXPECT_EQ(SRTMinorServoCommandLibrary::parseAnswer(answer), args);

    // Wrong OUTPUT field
    answer = "OUTPUT:123456\r\n";
    args.clear();
    EXPECT_EQ(SRTMinorServoCommandLibrary::parseAnswer(answer), args);

    // Multiple values without key, cannot find the correct timestamp
    answer = "OUTPUT:GOOD,12345,67890\r\n";
    args.clear();
    EXPECT_EQ(SRTMinorServoCommandLibrary::parseAnswer(answer), args);

    // Wrong timestamp format
    answer = "OUTPUT:GOOD,12345.ABCD\r\n";
    args.clear();
    EXPECT_EQ(SRTMinorServoCommandLibrary::parseAnswer(answer), args);
}
