#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "SRTMinorServoSocket.h"
#include "SRTMinorServoCommandLibrary.h"

// This address and port are the ones set in the simulator
// In order for the test to properly be executed, the simulator should be launched with the following command:
// discos-simulator -s minor_servo start &
#define ADDRESS             std::string("127.0.0.1")
#define PORT                12800
#define TIMEOUT             0.1
#define NOISE_THRESHOLD     0.2
#define TIMEGAP             0.2
#define ADVANCE_TIMEGAP    5


std::atomic<bool> terminate = false;

void sigintHandler(int sig_num)
{
    std::cout << std::endl << "Terminating..." << std::endl;
    terminate = true;
}

std::string serializeStatus(SRTMinorServoAnswerMap map)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(6);
    stream << std::get<double>(map["TIMESTAMP"]);
    stream << "\t" << std::get<double>(map["SRP_TX"]);
    stream << "\t" << std::get<double>(map["SRP_TY"]);
    stream << "\t" << std::get<double>(map["SRP_TZ"]);
    stream << "\t" << std::get<double>(map["SRP_RX"]);
    stream << "\t" << std::get<double>(map["SRP_RY"]);
    stream << "\t" << std::get<double>(map["SRP_RZ"]);
    return stream.str();
}

std::string serializeProgramTrack(double timestamp, std::vector<double> coordinates)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(6) << timestamp;
    for(double coordinate : coordinates)
    {
        stream << "\t" << coordinate;
    }
    return stream.str();
}

class SRPProgramTrackTest : public ::testing::Test
{
protected:
    static std::vector<double> getCoordinates(SRTMinorServoAnswerMap SRPStatus)
    {
        std::vector<double> currentCoordinates;
        currentCoordinates.push_back(std::get<double>(SRPStatus["SRP_TX"]));
        currentCoordinates.push_back(std::get<double>(SRPStatus["SRP_TY"]));
        currentCoordinates.push_back(std::get<double>(SRPStatus["SRP_TZ"]));
        currentCoordinates.push_back(std::get<double>(SRPStatus["SRP_RX"]));
        currentCoordinates.push_back(std::get<double>(SRPStatus["SRP_RY"]));
        currentCoordinates.push_back(std::get<double>(SRPStatus["SRP_RZ"]));
        return currentCoordinates;
    }

    static bool compareCoordinates(std::vector<double> first, std::vector<double> second)
    {
        if(first.size() != second.size())
        {
            return false;
        }

        for(size_t i = 0; i < first.size(); i++)
        {
            if(first[i] != second[i])
            {
                return false;
            }
        }

        return true;
    }

    static void addNoiseToCoordinates(std::vector<double> &coordinates)
    {
        for(size_t i = 0; i < coordinates.size(); i++)
        {
            coordinates[i] += std::max(-1, std::min(1, rand() % 5 - 2)) * (double(rand() % 100) / 100) * NOISE_THRESHOLD;
        }
    }

    void SetUp() override
    {
        signal(SIGINT, sigintHandler);
        srand((int)CIRATools::getUNIXEpoch());
        std::cout << std::fixed << std::setprecision(6);

        try
        {
            SRTMinorServoSocket::getInstance(ADDRESS, PORT, TIMEOUT);
        }
        catch(ComponentErrors::SocketErrorExImpl& ex)
        {
            if(ex.getData("Reason") == std::string("Cannot connect the socket.").c_str())
            {
                FAIL() << "Socket failed to connect. Check if the simulator or the hardware can be reached." << std::endl;
            }
            else
            {
                FAIL() << "Unexpected failure." << std::endl;
            }
        }
    }

    void TearDown() override
    {
        SRTMinorServoSocket::destroyInstance();
    }
};

TEST_F(SRPProgramTrackTest, z_axis_translation)
{
    SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
    SRTMinorServoAnswerMap::iterator iterator;

    SRTMinorServoAnswerMap MSStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status());
    EXPECT_EQ(std::get<std::string>(MSStatus["OUTPUT"]), "GOOD");
    EXPECT_EQ(std::get<int>(MSStatus["CONTROL"]), 1);
    EXPECT_EQ(std::get<int>(MSStatus["POWER"]), 1);
    EXPECT_EQ(std::get<int>(MSStatus["EMERGENCY"]), 2);
    EXPECT_EQ(std::get<int>(MSStatus["ENABLED"]), 1);

    SRTMinorServoAnswerMap SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
    EXPECT_EQ(std::get<int>(SRPStatus["SRP_ENABLED"]), 1);
    EXPECT_EQ(std::get<int>(SRPStatus["SRP_STATUS"]), 1);
    EXPECT_EQ(std::get<int>(SRPStatus["SRP_BLOCK"]), 2);

    /*for(iterator = SRPStatus.begin(); iterator != SRPStatus.end(); ++iterator)
    {
        std::visit([iterator](const auto& var) mutable { std::cout << iterator->first << ": " << var << std::endl; }, iterator->second);
    }*/

    std::vector<double> startingCoordinates = {0, 0, 0, 0, 0, 0};
    SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::preset("SRP", startingCoordinates));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");

    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
        EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
        EXPECT_EQ(std::get<int>(SRPStatus["SRP_OPERATIVE_MODE"]), 40);
    }
    while(!compareCoordinates(startingCoordinates, getCoordinates(SRPStatus)));

    std::thread t([]()
    {
        SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
        SRTMinorServoAnswerMap SRPStatus;

        ofstream statusFile;
        statusFile.open("SRPStatus.txt", ios::out);

        while(!terminate)
        {
            SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
            statusFile << serializeStatus(SRPStatus) << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        statusFile.close();
    });

    double start_time = CIRATools::getUNIXEpoch() + ADVANCE_TIMEGAP;
    long unsigned int trajectory_id = int(start_time);
    unsigned int point_id = 0;
    std::vector<double> programTrackCoordinates = startingCoordinates;

    SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("SRP", trajectory_id, point_id, programTrackCoordinates, start_time));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");

    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
    EXPECT_EQ(std::get<int>(SRPStatus["SRP_OPERATIVE_MODE"]), 50);

    ofstream programTrackFile;
    programTrackFile.open("SRPTrajectory.txt", ios::out);
    programTrackFile << serializeProgramTrack(start_time, programTrackCoordinates) << std::endl;

    double next_expected_time = start_time;

    while(!terminate)
    {
        next_expected_time += TIMEGAP;
        std::cout << next_expected_time << std::endl;

        std::this_thread::sleep_for(std::chrono::microseconds((int)round(1000000 * std::max(0.0, next_expected_time - ADVANCE_TIMEGAP - CIRATools::getUNIXEpoch()))));
        point_id++;

        addNoiseToCoordinates(programTrackCoordinates);
        SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("SRP", trajectory_id, point_id, programTrackCoordinates));
        EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
        programTrackFile << serializeProgramTrack(next_expected_time, programTrackCoordinates) << std::endl;
    }

    programTrackFile.close();
    t.join();
}
