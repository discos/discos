#include "gtest/gtest.h"
#include <IRA>
#include "SRTMinorServoCommandLibrary.h"

using namespace IRA;

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
    EXPECT_EQ(SRTMinorServoCommandLibrary::preset("PFP", std::vector<double>{0.,1.,2.,3.,4.,5.}), "PRESET=PFP,0,1,2,3,4,5\r\n");
}

TEST(SRTMinorServoCommandLibraryTest, programTrack)
{
    double start_time = CIRATools::getUNIXEpoch() + 3;
    unsigned long int trajectory_id = (unsigned long int)(start_time * 1000);
    std::stringstream expected_answer;
    expected_answer << "PROGRAMTRACK=PFP," << trajectory_id << ",0," << trajectory_id << ",0,1,2,3,4,5\r\n";
    EXPECT_EQ(SRTMinorServoCommandLibrary::programTrack("PFP", trajectory_id, 0, std::vector<double>{0.,1.,2.,3.,4.,5.}, start_time), expected_answer.str());

    for(unsigned int i = 1; i < 10; i++)
    {
        expected_answer.str(std::string());
        expected_answer << "PROGRAMTRACK=PFP," << trajectory_id << "," << i << ",*,0,1,2,3,4,5\r\n";
        EXPECT_EQ(SRTMinorServoCommandLibrary::programTrack("PFP", trajectory_id, i, std::vector<double>{0.,1.,2.,3.,4.,5.}), expected_answer.str());
    }
}

TEST(SRTMinorServoCommandLibraryTest, offset)
{
    EXPECT_EQ(SRTMinorServoCommandLibrary::offset("PFP", std::vector<double>{0.,1.,2.,3.,4.,5.}), "OFFSET=PFP,0,1,2,3,4,5\r\n");
}

TEST(SRTMinorServoCommandLibraryTest, parseAnswer)
{
    // Minimal correct answer
    std::string answer = "OUTPUT:GOOD,1665743366.654321";
    std::map<std::string, std::variant<int, double, std::string> > args;
    args["OUTPUT"] = "GOOD";
    args["TIMESTAMP"] = 1665743366.654321;
    EXPECT_EQ(SRTMinorServoCommandLibrary::parseAnswer(answer), args);

    // Complete correct answer
    answer = "OUTPUT:GOOD,1665743366.123456,CURRENT_CONFIG=21|SIMULATION_ENABLED=34|PLC_TIME=78|PLC_VERSION=69|CONTROL=14|POWER=38|EMERGENCY=69|ENABLED=51|OPERATIVE_MODE=94";
    args.clear();
    args["OUTPUT"] = "GOOD";
    args["TIMESTAMP"] = 1665743366.123456;
    args["CURRENT_CONFIG"] = 21;
    args["SIMULATION_ENABLED"] = 34;
    args["PLC_TIME"] = 78;
    args["PLC_VERSION"] = 69;
    args["CONTROL"] = 14;
    args["POWER"] = 38;
    args["EMERGENCY"] = 69;
    args["ENABLED"] = 51;
    args["OPERATIVE_MODE"] = 94;
    EXPECT_EQ(SRTMinorServoCommandLibrary::parseAnswer(answer), args);

    // Missing timestamp
    answer = "OUTPUT:GOOD,CURRENT_CONFIG=21|SIMULATION_ENABLED=34|PLC_TIME=78|PLC_VERSION=69|CONTROL=14|POWER=38|EMERGENCY=69|ENABLED=51|OPERATIVE_MODE=94";
    args.clear();
    EXPECT_EQ(SRTMinorServoCommandLibrary::parseAnswer(answer), args);

    // Wrong OUTPUT field
    answer = "OUTPUT:123456";
    args.clear();
    EXPECT_EQ(SRTMinorServoCommandLibrary::parseAnswer(answer), args);

    // Multiple values without key, cannot find the correct timestamp
    answer = "OUTPUT:GOOD,12345,67890";
    args.clear();
    EXPECT_EQ(SRTMinorServoCommandLibrary::parseAnswer(answer), args);

    // Wrong timestamp format
    answer = "OUTPUT:GOOD,12345.ABCD";
    args.clear();
    EXPECT_EQ(SRTMinorServoCommandLibrary::parseAnswer(answer), args);
}