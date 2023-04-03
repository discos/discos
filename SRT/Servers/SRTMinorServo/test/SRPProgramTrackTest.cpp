#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <boost/filesystem.hpp>
#include "SRTMinorServoSocket.h"
#include "SRTMinorServoCommandLibrary.h"

// This address and port are the ones set in the simulator
// In order for the test to properly be executed, the simulator should be launched with the following command:
// discos-simulator -s minor_servo start &
//#define ADDRESS             std::string("192.168.200.13")
//#define PORT                4758
#define ADDRESS             std::string("127.0.0.1")
#define PORT                12800
#define SOCKET_TIMEOUT      0.1
#define NOISE_THRESHOLD     1
#define TIMEGAP             0.2
#define ADVANCE_TIMEGAP     5
#define EPSILON             0.001
#define MAX_RANGES          std::vector<double>{40, 100, 40, 0.2, 0.2, 0.2}


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
    std::vector<std::string> coordinates = { "SRP_TX", "SRP_TY", "SRP_TZ", "SRP_RX", "SRP_RY", "SRP_RZ" };

    for(std::string coordinate : coordinates)
    {
        double value;

        try
        {
            value = std::get<double>(map[coordinate]);
        }
        catch(...)
        {
            value = (double)std::get<long>(map[coordinate]);
        }

        stream << "\t" << value;
    }
    return stream.str();
}

std::string serializeProgramTrack(double timestamp, std::vector<double> coordinates)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(6) << timestamp;
    for(double coordinate : coordinates)
        stream << "\t" << coordinate;
    return stream.str();
}

class SRPProgramTrackTest : public ::testing::Test
{
protected:
    static std::vector<double> getCoordinates(SRTMinorServoAnswerMap SRPStatus)
    {
        std::vector<double> currentCoordinates;
        std::vector<std::string> coordinates = { "SRP_TX", "SRP_TY", "SRP_TZ", "SRP_RX", "SRP_RY", "SRP_RZ" };

        for(std::string coordinate : coordinates)
        {
            double value;

            try
            {
                value = std::get<double>(SRPStatus[coordinate]);
            }
            catch(...)
            {
                value = (double)std::get<long>(SRPStatus[coordinate]);
            }

            currentCoordinates.push_back(value);
        }

        return currentCoordinates;
    }

    static bool compareCoordinates(std::vector<double> first, std::vector<double> second)
    {
        if(first.size() != second.size())
            return false;

        for(size_t i = 0; i < first.size(); i++)
        {
            double diff = fabs(first[i] - second[i]);
            if(diff > EPSILON)
                return false;
        }

        return true;
    }

    static void addNoiseToCoordinates(std::vector<double> &coordinates)
    {
        for(size_t i = 0; i < 3; i++)
            coordinates[i] += std::max(-1, std::min(1, rand() % 5 - 2)) * (double(rand() % 100) / 100) * NOISE_THRESHOLD;
    }

    static bool moveAxis(std::vector<double> &coordinates, int axis_to_move, int sign)
    {
        sign = sign / abs(sign);
        double offset_to_add;
        axis_to_move >= 3 ? offset_to_add = 0.05 : offset_to_add = 1.0;
        coordinates[axis_to_move] += sign * offset_to_add;
        if(fabs(coordinates[axis_to_move]) >= MAX_RANGES[axis_to_move])
            return true;
        return false;
    }

    void SetUp() override
    {
        srand((int)CIRATools::getUNIXEpoch());
        std::cout << std::fixed << std::setprecision(6);

        try
        {
            SRTMinorServoSocket::getInstance(ADDRESS, PORT, SOCKET_TIMEOUT);
            std::cout << "Socket connected." << std::endl;
        }
        catch(ComponentErrors::SocketErrorExImpl& ex)
        {
            if(ex.getData("Reason") == std::string("Cannot connect the socket.").c_str())
                FAIL() << "Socket failed to connect. Check if the simulator or the hardware can be reached." << std::endl;
            else
                FAIL() << "Unexpected failure." << std::endl;
        }
    }

    void TearDown() override
    {
        SRTMinorServoSocket::destroyInstance();
        terminate = false;
    }
};

TEST_F(SRPProgramTrackTest, noise_translation)
{
    SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
    SRTMinorServoAnswerMap::iterator iterator;

    std::cout << "Sending MS STATUS command...";

    SRTMinorServoAnswerMap MSStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status());

    EXPECT_EQ(std::get<std::string>(MSStatus["OUTPUT"]), "GOOD");
    EXPECT_EQ(std::get<long>(MSStatus["CONTROL"]), 1);
    EXPECT_EQ(std::get<long>(MSStatus["POWER"]), 1);
    EXPECT_EQ(std::get<long>(MSStatus["EMERGENCY"]), 2);
    EXPECT_EQ(std::get<long>(MSStatus["ENABLED"]), 1);

    std::cout << "OK." << std::endl;
    std::cout << "Sending initial SRP STATUS command...";

    SRTMinorServoAnswerMap SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
    EXPECT_EQ(std::get<long>(SRPStatus["SRP_STATUS"]), 1);
    EXPECT_EQ(std::get<long>(SRPStatus["SRP_BLOCK"]), 2);

    std::cout << "OK." << std::endl;
    std::cout << "Sending PRESET command...";

    std::vector<double> startingCoordinates = {0, 0, 0, 0, 0, 0};
    SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::preset("SRP", startingCoordinates));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");

    std::cout << "OK." << std::endl;

    signal(SIGINT, sigintHandler);

    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));

        EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
        EXPECT_EQ(std::get<long>(SRPStatus["SRP_OPERATIVE_MODE"]), 40);

        std::cout << serializeStatus(SRPStatus) << std::endl;

        if(terminate)
            return;
    }
    while(!compareCoordinates(startingCoordinates, getCoordinates(SRPStatus)));

    std::time_t tn = std::time(0);
    std::tm* now = std::localtime(&tn);
    std::stringstream directory_ss;
    directory_ss << "NoiseTranslationTest-" << 1900 + now->tm_year << std::setfill('0') << std::setw(2) << now->tm_mon + 1 << now->tm_mday << "-" << now->tm_hour << now->tm_min << now->tm_sec;
    std::string directory = directory_ss.str();
    boost::filesystem::create_directory(directory);

    std::thread t([directory]()
    {
        SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
        SRTMinorServoAnswerMap SRPStatus;

        ofstream statusFile;
        statusFile.open(directory + "/status.txt", ios::out);

        long unsigned int counter = 0;

        double tn = CIRATools::getUNIXEpoch();

        while(!terminate)
        {
            SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
            statusFile << serializeStatus(SRPStatus) << std::endl;
            if(counter % 10 == 0)
                std::cout << serializeStatus(SRPStatus) << std::endl;
            counter++;

            tn += 0.01;
            std::this_thread::sleep_for(std::chrono::microseconds((int)round(1000000 * std::max(0.0, tn - CIRATools::getUNIXEpoch()))));
        }

        statusFile.close();
    });

    double start_time = CIRATools::getUNIXEpoch() + ADVANCE_TIMEGAP;
    std::cout << "PRESET position reached, starting PROGRAMTRACK with start time: " << start_time << std::endl;
    long unsigned int trajectory_id = int(start_time);
    unsigned int point_id = 0;
    std::vector<double> programTrackCoordinates = startingCoordinates;

    SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("SRP", trajectory_id, point_id, programTrackCoordinates, start_time));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");

    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
    EXPECT_EQ(std::get<long>(SRPStatus["SRP_OPERATIVE_MODE"]), 50);

    ofstream programTrackFile;
    programTrackFile.open(directory + "/trajectory.txt", ios::out);
    programTrackFile << serializeProgramTrack(start_time, programTrackCoordinates) << std::endl;

    double next_expected_time = start_time;

    while(!terminate)
    {
        next_expected_time += TIMEGAP;

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

TEST_F(SRPProgramTrackTest, cycle_movement)
{
    SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
    SRTMinorServoAnswerMap::iterator iterator;

    std::cout << "Sending MS STATUS command...";

    SRTMinorServoAnswerMap MSStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status());

    EXPECT_EQ(std::get<std::string>(MSStatus["OUTPUT"]), "GOOD");
    EXPECT_EQ(std::get<long>(MSStatus["CONTROL"]), 1);
    EXPECT_EQ(std::get<long>(MSStatus["POWER"]), 1);
    EXPECT_EQ(std::get<long>(MSStatus["EMERGENCY"]), 2);
    EXPECT_EQ(std::get<long>(MSStatus["ENABLED"]), 1);

    std::cout << "OK." << std::endl;
    std::cout << "Sending initial SRP STATUS command...";

    SRTMinorServoAnswerMap SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
    EXPECT_EQ(std::get<long>(SRPStatus["SRP_STATUS"]), 1);
    EXPECT_EQ(std::get<long>(SRPStatus["SRP_BLOCK"]), 2);

    std::cout << "OK." << std::endl;
    std::cout << "Sending all axes to 0...";

    std::vector<double> startingCoordinates = {0, 0, 0, 0, 0, 0};
    SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::preset("SRP", startingCoordinates));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");

    std::cout << "OK." << std::endl;

    signal(SIGINT, sigintHandler);

    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));

        EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
        EXPECT_EQ(std::get<long>(SRPStatus["SRP_OPERATIVE_MODE"]), 40);

        std::cout << serializeStatus(SRPStatus) << std::endl;

        if(terminate)
            return;
    }
    while(!compareCoordinates(startingCoordinates, getCoordinates(SRPStatus)));

    std::time_t tn = std::time(0);
    std::tm* now = std::localtime(&tn);
    std::stringstream directory_ss;
    directory_ss << "CycleMovementTest-" << 1900 + now->tm_year << std::setfill('0') << std::setw(2) << now->tm_mon + 1 << now->tm_mday << "-" << now->tm_hour << now->tm_min << now->tm_sec;
    std::string directory = directory_ss.str();
    boost::filesystem::create_directory(directory);

    std::thread t([directory]()
    {
        SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
        SRTMinorServoAnswerMap SRPStatus;

        ofstream statusFile;
        statusFile.open(directory + "/status.txt", ios::out);

        long unsigned int counter = 0;

        double tn = CIRATools::getUNIXEpoch();

        while(!terminate)
        {
            SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
            statusFile << serializeStatus(SRPStatus) << std::endl;
            if(counter % 10 == 0)
                std::cout << serializeStatus(SRPStatus) << std::endl;
            counter++;

            tn += 0.01;
            std::this_thread::sleep_for(std::chrono::microseconds((int)round(1000000 * std::max(0.0, tn - CIRATools::getUNIXEpoch()))));
        }

        statusFile.close();
    });

    double start_time = CIRATools::getUNIXEpoch() + ADVANCE_TIMEGAP;
    std::cout << "PRESET position reached, starting PROGRAMTRACK with start time: " << start_time << std::endl;
    long unsigned int trajectory_id = int(start_time);
    unsigned int point_id = 0;
    std::vector<double> programTrackCoordinates = startingCoordinates;

    SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("SRP", trajectory_id, point_id, programTrackCoordinates, start_time));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");

    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
    EXPECT_EQ(std::get<long>(SRPStatus["SRP_OPERATIVE_MODE"]), 50);

    ofstream programTrackFile;
    programTrackFile.open(directory + "/trajectory.txt", ios::out);
    programTrackFile << serializeProgramTrack(start_time, programTrackCoordinates) << std::endl;

    double next_expected_time = start_time;
    unsigned int axis_to_move = 0;
    int sign = 1;
    unsigned int idle_count = 0;
    bool idle = false;

    while(!terminate)
    {
        while(!terminate)
        {
            next_expected_time += TIMEGAP;

            std::this_thread::sleep_for(std::chrono::microseconds((int)round(1000000 * std::max(0.0, next_expected_time - ADVANCE_TIMEGAP - CIRATools::getUNIXEpoch()))));
            point_id++;

            if(idle)
            {
                idle_count++;
                if(idle_count == 5)
                {
                    idle_count = 0;
                    idle = false;
                }
            }
            else if(moveAxis(programTrackCoordinates, axis_to_move, sign))
            {
                sign = sign * -1;
                idle = true;
            }

            SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("SRP", trajectory_id, point_id, programTrackCoordinates));
            EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
            //std::cout << serializeProgramTrack(next_expected_time, programTrackCoordinates) << std::endl;
            programTrackFile << serializeProgramTrack(next_expected_time, programTrackCoordinates) << std::endl;

            if(round(programTrackCoordinates[axis_to_move] * 100) == 0 && sign == 1)
            {
                programTrackCoordinates[axis_to_move] = 0.0;
                break;
            }
        }

        axis_to_move == 5 ? axis_to_move = 0 : axis_to_move++;
    }

    programTrackFile.close();
    t.join();
}
