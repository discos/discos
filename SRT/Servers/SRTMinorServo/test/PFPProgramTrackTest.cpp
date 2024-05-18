#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <boost/filesystem.hpp>
#include <MinorServoErrors.h>
#include "SRTMinorServoUtils.h"
#include "SRTMinorServoTestingSocket.h"
#include "SRTMinorServoCommandLibrary.h"

// This address and port are the ones set in the simulator
// In order for the test to properly be executed, the simulator should be launched with the following command:
// discos-simulator -s minor_servo start &
//#define SIMULATION
#ifdef SIMULATION
    #define ADDRESS             std::string("127.0.0.1")
    #define PORT                12800
#else
    #define ADDRESS             std::string("192.168.200.13")
    #define PORT                4758
#endif
#define TIMEGAP             0.2
#define ADVANCE_TIMEGAP     2.6
//#define MIN_RANGES          std::vector<double>{ -1490, -200, -1 }
//#define MAX_RANGES          std::vector<double>{ 1490, 50, 76 }
#define MIN_RANGES          std::vector<double>{ -1000, -150, 0 }
#define MAX_RANGES          std::vector<double>{ 1000, 40, 70 }
#define MAX_SPEED           std::vector<double>{ 25, 5, 0.42 }
#define STATUS_PERIOD       0.01

std::atomic<bool> terminate = false;

using namespace MinorServo;
using namespace IRA;


class PFPProgramTrackTest : public ::testing::Test
{
protected:
    std::vector<double> startingCoordinates = { 0.0, 0.0, 0.0 };
    std::string directory;
    std::thread statusThread;

    static void printStatus(std::string filename)
    {
        SRTMinorServoSocket& socket = SRTMinorServoTestingSocket::getInstance();
        SRTMinorServoAnswerMap PFPStatus;

        ofstream statusFile;
        statusFile.open(filename, ios::out);

        long unsigned int counter = 0;

        double tn = CIRATools::getUNIXEpoch();

        while(!terminate.load())
        {
            try
            {
                PFPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("PFP"));
            }
            catch(MinorServoErrors::MinorServoErrorsEx& ex)
            {
                //MinorServoErrors::MinorServoErrorsExImpl impl(ex);
                std::cout << getErrorFromEx(ex) << std::endl;
                terminate.store(true);
                break;
            }

            std::string status = serializeStatus(PFPStatus);

            statusFile << status << std::endl;
            if(counter % 10 == 0)
                std::cout << status << std::endl;
            counter++;

            tn += STATUS_PERIOD;
            std::this_thread::sleep_for(std::chrono::microseconds((int)round(1000000 * std::max(0.0, tn - CIRATools::getUNIXEpoch()))));
        }

        statusFile.close();
    }

    static void sigintHandler(int sig_num)
    {
        std::cout << std::endl << "Terminating..." << std::endl;
        terminate.store(true);
    }

    static std::string serializeStatus(SRTMinorServoAnswerMap map)
    {
        std::string status;
        try
        {
            status = serializeCoordinates(CIRATools::ACSTime2UNIXEpoch(map.getTimestamp()), getCoordinates(map));
        }
        catch(std::bad_variant_access const& ex)
        {
            std::cout << "Bad timestamp!" << std::endl;

            /*SRTMinorServoAnswerMap::iterator iterator;
            for(iterator = map.begin(); iterator != map.end(); ++iterator)
            {
                std::visit([iterator](const auto& var) mutable { std::cout << iterator->first << ": " << var << std::endl; }, iterator->second);
            }*/
        }
        status += serializeElongations(getElongations(map));
        return status;
    }

    static std::string serializeCoordinates(double timestamp, std::vector<double> coordinates)
    {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(6) << timestamp;
        for(double coordinate : coordinates)
            stream << "\t" << coordinate;
        return stream.str();
    }

    static std::string serializeElongations(std::vector<double> elongations)
    {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(6);
        for(double elongation : elongations)
            stream << "\t" << elongation;
        return stream.str();
    }

    static std::vector<double> getCoordinates(SRTMinorServoAnswerMap PFPStatus)
    {
        std::vector<double> currentCoordinates;
        std::vector<std::string> coordinates = { "PFP_TX", "PFP_TZ", "PFP_RTHETA" };

        for(std::string coordinate : coordinates)
        {
            try
            {
                currentCoordinates.push_back(PFPStatus.get<double>(coordinate));
            }
            catch(std::bad_variant_access const& ex)
            {
                std::cout << "Bad floating point coordinate:" << PFPStatus.get<long>(coordinate) << std::endl;
                currentCoordinates.push_back(double(PFPStatus.get<long>(coordinate)));
            }
        }

        return currentCoordinates;
    }

    static std::vector<double> getElongations(SRTMinorServoAnswerMap PFPStatus)
    {
        std::vector<double> currentElongations;
        std::vector<std::string> elongations = { "PFP_ELONG_X", "PFP_ELONG_Z_MASTER", "PFP_ELONG_Z_SLAVE", "PFP_ELONG_THETA_MASTER", "PFP_ELONG_THETA_SLAVE" };

        for(std::string elongation : elongations)
        {
            try
            {
                currentElongations.push_back(PFPStatus.get<double>(elongation));
            }
            catch(std::bad_variant_access const& ex)
            {
                std::cout << "Bad floating point elongation:" << PFPStatus.get<long>(elongation) << std::endl;
                currentElongations.push_back(double(PFPStatus.get<long>(elongation)));
            }
        }

        return currentElongations;
    }

    bool moveAxis(std::vector<double>& coordinates, unsigned int& axis_to_move, int& sign)
    {
        bool last_segment = false;
        bool go_idle = false;
        double starting_position = coordinates[axis_to_move];

        if(startingCoordinates[axis_to_move] == MIN_RANGES[axis_to_move] || startingCoordinates[axis_to_move] == MAX_RANGES[axis_to_move])
        {
            if(starting_position == startingCoordinates[axis_to_move])
            {
                // We just started the trajectory for the current axis, correct the sign eventually
                if(startingCoordinates[axis_to_move] == MIN_RANGES[axis_to_move])
                {
                    // Only above
                    sign = 1;
                }
                else
                {
                    // Only below
                    sign = -1;
                }
            }
            else
            {
                // We are executing the trajectory, check if we are already headed towards the starting position
                if((startingCoordinates[axis_to_move] == MIN_RANGES[axis_to_move] && sign < 0) || (startingCoordinates[axis_to_move] == MAX_RANGES[axis_to_move] && sign > 0))
                {
                    last_segment = true;
                }
            }
        }
        else if(starting_position < startingCoordinates[axis_to_move] && sign > 0)
        {
            last_segment = true;
        }

        double offset_to_add = MAX_SPEED[axis_to_move] / 5;
        double ending_position = starting_position + sign * offset_to_add;

        if(ending_position >= MAX_RANGES[axis_to_move])
        {
            // Top reached, go down
            ending_position = MAX_RANGES[axis_to_move];
            go_idle = true;
        }
        else if(ending_position <= MIN_RANGES[axis_to_move])
        {
            // Bottom reached, go up
            ending_position = MIN_RANGES[axis_to_move];
            go_idle = true;
        }

        // Check if we finished the current axis' trajectory
        if(sign == 1 && starting_position < startingCoordinates[axis_to_move] && startingCoordinates[axis_to_move] <= ending_position)
        {
            // Crossed zero while going up, round to zero and change axis
            ending_position = startingCoordinates[axis_to_move];
            go_idle = true;
        }

        coordinates[axis_to_move] = ending_position;

        if(go_idle)
        {
            if(last_segment)
            {
                sign = 1;
                axis_to_move == 2 ? axis_to_move = 0 : axis_to_move++;
            }
            else
            {
                sign *= -1;
            }
        }

        return go_idle;
    }

    void SetUp() override
    {
        srand((int)CIRATools::getUNIXEpoch());
        std::cout << std::fixed << std::setprecision(6);

        SRTMinorServoSocket& socket = SRTMinorServoTestingSocket::getInstance(ADDRESS, PORT, 0.2);
        if(!socket.isConnected())
        {
            FAIL() << "Socket failed to connect. Check if the simulator or the hardware can be reached." << std::endl;
        }

        std::cout << "Sending MS STATUS command...";

        SRTMinorServoAnswerMap MSStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status());
        EXPECT_TRUE(MSStatus.checkOutput());
        EXPECT_EQ(MSStatus.get<long>("CONTROL"), 1);
        //EXPECT_EQ(MSStatus.get<long>("POWER"), 1);
        EXPECT_EQ(MSStatus.get<long>("EMERGENCY"), 2);
        //EXPECT_EQ(MSStatus.get<long>("ENABLED"), 1);
        std::cout << "OK." << std::endl;

        /*SRTMinorServoAnswerMap::iterator iterator;
        for(iterator = MSStatus.begin(); iterator != MSStatus.end(); ++iterator)
        {
            std::visit([iterator](const auto& var) mutable { std::cout << iterator->first << ": " << var << std::endl; }, iterator->second);
        }*/

        std::cout << "Sending initial PFP STATUS command...";

        SRTMinorServoAnswerMap PFPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("PFP"));
        EXPECT_TRUE(PFPStatus.checkOutput());
        //EXPECT_EQ(PFPStatus.get<long>("PFP_STATUS"), 1);
        EXPECT_EQ(PFPStatus.get<long>("PFP_BLOCK"), 2);
        std::cout << "OK." << std::endl;

        /*for(iterator = PFPStatus.begin(); iterator != PFPStatus.end(); ++iterator)
        {
            std::visit([iterator](const auto& var) mutable { std::cout << iterator->first << ": " << var << std::endl; }, iterator->second);
        }*/

        std::cout << "Sending all axes to the starting position..." << std::endl;

        PFPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::preset("PFP", startingCoordinates));
        EXPECT_TRUE(PFPStatus.checkOutput());

        signal(SIGINT, PFPProgramTrackTest::sigintHandler);

        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            PFPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("PFP"));
            EXPECT_TRUE(PFPStatus.checkOutput());
            std::cout << serializeStatus(PFPStatus) << std::endl;

            if(terminate.load())
                FAIL() << "Aborting test..." << std::endl;
        }
        while(PFPStatus.get<unsigned int>("PFP_OPERATIVE_MODE") != 40);
        EXPECT_EQ(PFPStatus.get<unsigned int>("PFP_OPERATIVE_MODE"), 40);

        std::cout << "OK." << std::endl;

        startingCoordinates = { 0.0, 0.0, 0.0 };

        std::time_t tn = std::time(0);
        std::tm* now = std::localtime(&tn);
        std::stringstream directory_ss;
        directory_ss << "TESTS/PFP";
        boost::filesystem::create_directory(directory_ss.str());
        directory_ss << "/";
        directory_ss << ::testing::UnitTest::GetInstance()->current_test_info()->name();
        directory_ss << "-";
        directory_ss << 1900 + now->tm_year;
        directory_ss << std::setfill('0') << std::setw(2) << now->tm_mon + 1;
        directory_ss << std::setfill('0') << std::setw(2) << now->tm_mday;
        directory_ss << "-";
        directory_ss << std::setfill('0') << std::setw(2) << now->tm_hour;
        directory_ss << std::setfill('0') << std::setw(2) << now->tm_min;
        directory_ss << std::setfill('0') << std::setw(2) << now->tm_sec;
        directory = directory_ss.str();
        boost::filesystem::create_directory(directory);

        statusThread = std::thread(&PFPProgramTrackTest::printStatus, directory + "/status.txt");
    }

    void TearDown() override
    {
        SRTMinorServoTestingSocket::destroyInstance();
        terminate.store(false);
    }
};

TEST_F(PFPProgramTrackTest, ContinuousMovementTest)
{
    double start_time = CIRATools::getUNIXEpoch() + ADVANCE_TIMEGAP;
    std::cout << "PRESET position reached, starting PROGRAMTRACK with start time: " << start_time << std::endl;
    long unsigned int trajectory_id = int(start_time);
    unsigned int point_id = 0;
    std::vector<double> programTrackCoordinates = startingCoordinates;

    SRTMinorServoSocket& socket = SRTMinorServoTestingSocket::getInstance();
    SRTMinorServoAnswerMap PFPStatus;
    PFPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("PFP", trajectory_id, point_id, programTrackCoordinates, start_time));
    EXPECT_TRUE(PFPStatus.checkOutput());

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    PFPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("PFP"));
    EXPECT_TRUE(PFPStatus.checkOutput());
    EXPECT_EQ(PFPStatus.get<unsigned int>("PFP_OPERATIVE_MODE"), 50);

    ofstream programTrackFile;
    programTrackFile.open(directory + "/trajectory.txt", ios::out);
    programTrackFile << PFPProgramTrackTest::serializeCoordinates(start_time, programTrackCoordinates) << std::endl;

    double next_expected_time = start_time;
    unsigned int axis_to_move = 0;
    int sign = 1;
    unsigned int idle_count = 0;
    bool idle = false;

    while(!terminate.load())
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
            idle = true;
        }

        PFPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("PFP", trajectory_id, point_id, programTrackCoordinates));
        EXPECT_TRUE(PFPStatus.checkOutput());
        //std::cout << PFPProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
        programTrackFile << PFPProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
    }

    programTrackFile.close();
    statusThread.join();
}

TEST_F(PFPProgramTrackTest, AllAxesMovementTest)
{
    double start_time = CIRATools::getUNIXEpoch() + ADVANCE_TIMEGAP;
    std::cout << "PRESET position reached, starting PROGRAMTRACK with start time: " << start_time << std::endl;
    long unsigned int trajectory_id = int(start_time);
    unsigned int point_id = 0;
    std::vector<double> programTrackCoordinates = startingCoordinates;

    SRTMinorServoSocket& socket = SRTMinorServoTestingSocket::getInstance();
    SRTMinorServoAnswerMap PFPStatus;
    PFPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("PFP", trajectory_id, point_id, programTrackCoordinates, start_time));
    EXPECT_TRUE(PFPStatus.checkOutput());

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    PFPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("PFP"));
    EXPECT_TRUE(PFPStatus.checkOutput());
    EXPECT_EQ(PFPStatus.get<unsigned int>("PFP_OPERATIVE_MODE"), 50);

    ofstream programTrackFile;
    programTrackFile.open(directory + "/trajectory.txt", ios::out);
    programTrackFile << PFPProgramTrackTest::serializeCoordinates(start_time, programTrackCoordinates) << std::endl;

    double next_expected_time = start_time;
    std::vector<int> sign = { 1, 1, 1 };
    std::vector<unsigned int> idle_count = { 0, 0, 0 };
    std::vector<bool> idle = { false, false, false };

    while(!terminate.load())
    {
        next_expected_time += TIMEGAP;

        std::this_thread::sleep_for(std::chrono::microseconds((int)round(1000000 * std::max(0.0, next_expected_time - ADVANCE_TIMEGAP - CIRATools::getUNIXEpoch()))));
        point_id++;

        for(size_t axis = 0; axis < 3; axis++)
        {
            if(!idle[axis])
            {
                unsigned int axis_to_move = axis;
                if(moveAxis(programTrackCoordinates, axis_to_move, sign[axis]))
                {
                    if(programTrackCoordinates[axis] != startingCoordinates[axis] || programTrackCoordinates[axis] == MIN_RANGES[axis] || programTrackCoordinates[axis] == MAX_RANGES[axis])
                    {
                        idle[axis] = true;
                    }
                }
            }
            else
            {
                idle_count[axis]++;
                if(idle_count[axis] == 5)
                {
                    idle_count[axis] = 0;
                    idle[axis] = false;
                }
            }
        }

        PFPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("PFP", trajectory_id, point_id, programTrackCoordinates));
        EXPECT_TRUE(PFPStatus.checkOutput());
        //std::cout << PFPProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
        programTrackFile << PFPProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
    }

    programTrackFile.close();
    statusThread.join();
}

TEST_F(PFPProgramTrackTest, SineWaveMovementTest)
{
    double start_time = CIRATools::getUNIXEpoch() + ADVANCE_TIMEGAP;
    std::cout << "PRESET position reached, starting PROGRAMTRACK with start time: " << start_time << std::endl;
    long unsigned int trajectory_id = int(start_time);
    unsigned int point_id = 0;
    std::vector<double> programTrackCoordinates = startingCoordinates;

    std::vector<double> phase_shift;
    std::vector<double> period;
    for(size_t axis = 0; axis < 3; axis++)
    {
        double amplitude = (MAX_RANGES[axis] - MIN_RANGES[axis]) / 2;
        double period_multiplier = 4;
        period.push_back(2 * amplitude / MAX_SPEED[axis] * period_multiplier);
        phase_shift.push_back((double)std::rand() / RAND_MAX * period[axis]);
        double center = (MAX_RANGES[axis] + MIN_RANGES[axis]) / 2;
        std::cout << center << std::endl;
        programTrackCoordinates[axis] = center + amplitude * sin(phase_shift[axis] * 2 * M_PI / period[axis]);
    }

    SRTMinorServoSocket& socket = SRTMinorServoTestingSocket::getInstance();
    SRTMinorServoAnswerMap PFPStatus;
    PFPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("PFP", trajectory_id, point_id, programTrackCoordinates, start_time));
    EXPECT_TRUE(PFPStatus.checkOutput());

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    PFPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("PFP"));
    EXPECT_TRUE(PFPStatus.checkOutput());
    EXPECT_EQ(PFPStatus.get<unsigned int>("PFP_OPERATIVE_MODE"), 50);

    ofstream programTrackFile;
    programTrackFile.open(directory + "/trajectory.txt", ios::out);
    programTrackFile << PFPProgramTrackTest::serializeCoordinates(start_time, programTrackCoordinates) << std::endl;

    double next_expected_time = start_time;

    while(!terminate.load())
    {
        next_expected_time += TIMEGAP;
        double time_delta = next_expected_time - start_time;

        std::this_thread::sleep_for(std::chrono::microseconds((int)round(1000000 * std::max(0.0, next_expected_time - ADVANCE_TIMEGAP - CIRATools::getUNIXEpoch()))));
        point_id++;

        for(size_t axis = 0; axis < 3; axis++)
        {
            double center = (MAX_RANGES[axis] + MIN_RANGES[axis]) / 2;
            double amplitude = (MAX_RANGES[axis] - MIN_RANGES[axis]) / 2;
            programTrackCoordinates[axis] = center + amplitude * sin((time_delta + phase_shift[axis]) * 2 * M_PI / period[axis]);
        }

        PFPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("PFP", trajectory_id, point_id, programTrackCoordinates));
        EXPECT_TRUE(PFPStatus.checkOutput());
        programTrackFile << PFPProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
    }

    programTrackFile.close();
    statusThread.join();
}

TEST_F(PFPProgramTrackTest, SeparateMovementTest)
{
    SRTMinorServoSocket& socket = SRTMinorServoTestingSocket::getInstance();
    SRTMinorServoAnswerMap PFPStatus;

    ofstream programTrackFile;
    programTrackFile.open(directory + "/trajectory.txt", ios::out);

    unsigned int axis_to_move = 1;
    int sign = 1;
    unsigned int idle_count = 0;
    bool idle = false;

    std::cout << "PRESET position reached" << std::endl;
    std::vector<double> programTrackCoordinates = startingCoordinates;

    bool immediate = false;

    while(!terminate.load())
    {
        double start_time = CIRATools::getUNIXEpoch() + ADVANCE_TIMEGAP;
        std::cout << "starting PROGRAMTRACK with timestamp " << std::to_string(start_time) << std::endl;
        long unsigned int trajectory_id = int(start_time);
        double next_expected_time = start_time;
        unsigned int point_id = 0;

        std::string command = SRTMinorServoCommandLibrary::programTrack("PFP", trajectory_id, point_id, programTrackCoordinates, start_time);
        //std::cout << command;
        PFPStatus = socket.sendCommand(command);
        EXPECT_TRUE(PFPStatus.checkOutput());
        programTrackFile << PFPProgramTrackTest::serializeCoordinates(start_time, programTrackCoordinates) << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        PFPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("PFP"));
        EXPECT_TRUE(PFPStatus.checkOutput());
        EXPECT_EQ(PFPStatus.get<unsigned int>("PFP_OPERATIVE_MODE"), 50);

        while(!terminate.load())
        {
            next_expected_time += TIMEGAP;

            std::this_thread::sleep_for(std::chrono::microseconds((int)round(1000000 * std::max(0.0, next_expected_time - ADVANCE_TIMEGAP - CIRATools::getUNIXEpoch()))));
            point_id++;

            if(idle)
            {
                idle_count++;
                if(idle_count == (ADVANCE_TIMEGAP / TIMEGAP) || immediate)
                {
                    idle_count = 0;
                    idle = false;
                    break;
                }
            }
            else
            {
                if(moveAxis(programTrackCoordinates, axis_to_move, sign))
                {
                    idle = true;
                }

                command = SRTMinorServoCommandLibrary::programTrack("PFP", trajectory_id, point_id, programTrackCoordinates);
                PFPStatus = socket.sendCommand(command);
                //std::cout << command;
                //PFPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("PFP", trajectory_id, point_id, programTrackCoordinates));
                EXPECT_TRUE(PFPStatus.checkOutput());
            }

            //std::cout << PFPProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
            programTrackFile << PFPProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
        }

        //immediate = immediate ? false : true;
    }

    programTrackFile.close();
    statusThread.join();
}

TEST_F(PFPProgramTrackTest, RapidTrajectoryTest)
{
    SRTMinorServoSocket& socket = SRTMinorServoTestingSocket::getInstance();
    SRTMinorServoAnswerMap PFPStatus;

    ofstream programTrackFile;
    programTrackFile.open(directory + "/trajectory.txt", ios::out);

    unsigned int idle_count = 0;
    bool idle = false;

    std::cout << "PRESET position reached, starting PROGRAMTRACK" << std::endl;

    while(!terminate.load())
    {
        unsigned int axis_to_move = 1;
        int sign = -1;

        std::vector<double> programTrackCoordinates = startingCoordinates;
        programTrackCoordinates[axis_to_move] = MAX_RANGES[axis_to_move];

        double start_time = CIRATools::getUNIXEpoch() + ADVANCE_TIMEGAP;
        long unsigned int trajectory_id = int(start_time);
        double next_expected_time = start_time;
        unsigned int point_id = 0;

        PFPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("PFP", trajectory_id, point_id, programTrackCoordinates, start_time));
        EXPECT_TRUE(PFPStatus.checkOutput());
        programTrackFile << PFPProgramTrackTest::serializeCoordinates(start_time, programTrackCoordinates) << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        PFPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("PFP"));
        EXPECT_TRUE(PFPStatus.checkOutput());
        EXPECT_EQ(PFPStatus.get<unsigned int>("PFP_OPERATIVE_MODE"), 50);

        while(!terminate.load())
        {
            next_expected_time += TIMEGAP;

            std::this_thread::sleep_for(std::chrono::microseconds((int)round(1000000 * std::max(0.0, next_expected_time - ADVANCE_TIMEGAP - CIRATools::getUNIXEpoch()))));
            point_id++;

            if(idle)
            {
                idle_count++;
                if(idle_count == ADVANCE_TIMEGAP / TIMEGAP)
                {
                    idle_count = 0;
                    idle = false;
                    break;
                }
            }
            else
            {
                if(moveAxis(programTrackCoordinates, axis_to_move, sign))
                {
                    idle = true;
                }

                PFPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("PFP", trajectory_id, point_id, programTrackCoordinates));
                EXPECT_TRUE(PFPStatus.checkOutput());
            }

            //std::cout << PFPProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
            programTrackFile << PFPProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
        }
    }

    programTrackFile.close();
    statusThread.join();
}
