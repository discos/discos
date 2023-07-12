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
#define SIMULATION
#ifdef SIMULATION
    #define ADDRESS             std::string("127.0.0.1")
    #define PORT                12800
#else
    #define ADDRESS             std::string("192.168.200.13")
    #define PORT                4758
#endif
#define SOCKET_TIMEOUT      0.5
#define NOISE_THRESHOLD     1
#define TIMEGAP             0.2
#define ADVANCE_TIMEGAP     5
#define MAX_RANGES          std::vector<double>{ 40, 40, 40, 0.2, 0.2, 0.2 }
#define STATUS_PERIOD       0.01

std::atomic<bool> terminate = false;


class SRPProgramTrackTest : public ::testing::Test
{
protected:
    std::vector<double> startingCoordinates = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    std::string directory;
    std::thread statusThread;

    static void printStatus(std::string filename)
    {
        SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
        SRTMinorServoAnswerMap SRPStatus;

        ofstream statusFile;
        statusFile.open(filename, ios::out);

        long unsigned int counter = 0;

        double tn = CIRATools::getUNIXEpoch();

        while(!terminate)
        {
            SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
            std::string status = serializeStatus(SRPStatus);

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
        terminate = true;
    }

    static std::string serializeStatus(SRTMinorServoAnswerMap map)
    {
        std::string status = serializeCoordinates(std::get<double>(map["TIMESTAMP"]), getCoordinates(map));
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

    static std::vector<double> getCoordinates(SRTMinorServoAnswerMap SRPStatus)
    {
        std::vector<double> currentCoordinates;
        std::vector<std::string> coordinates = { "SRP_TX", "SRP_TY", "SRP_TZ", "SRP_RX", "SRP_RY", "SRP_RZ" };

        for(std::string coordinate : coordinates)
        {
            auto value = SRPStatus[coordinate];

            try
            {
                currentCoordinates.push_back(std::get<double>(value));
            }
            catch(std::bad_variant_access const& ex)
            {
                std::cout << ex.what() << ", variant index: " << value.index() << std::endl;
                currentCoordinates.push_back(200.0);
            }
        }

        return currentCoordinates;
    }

    static std::vector<double> getElongations(SRTMinorServoAnswerMap SRPStatus)
    {
        std::vector<double> currentElongations;
        std::vector<std::string> elongations = { "SRP_ELONG_Z1", "SRP_ELONG_Z2", "SRP_ELONG_Z3", "SRP_ELONG_Y1", "SRP_ELONG_Y2", "SRP_ELONG_X1" };

        for(std::string elongation : elongations)
        {
            auto value = SRPStatus[elongation];

            try
            {
                currentElongations.push_back(std::get<double>(value));
            }
            catch(std::bad_variant_access const& ex)
            {
                std::cout << ex.what() << ", variant index: " << value.index() << std::endl;
                currentElongations.push_back(10000.0);
            }
        }

        return currentElongations;
    }

    static bool moveAxis(std::vector<double> &coordinates, int axis_to_move, int sign)
    {
        sign = sign / abs(sign);
        double offset_to_add;
        axis_to_move >= 3 ? offset_to_add = 0.076 : offset_to_add = 0.8;
        coordinates[axis_to_move] += sign * offset_to_add;
        if(fabs(coordinates[axis_to_move]) >= MAX_RANGES[axis_to_move])
        {
            coordinates[axis_to_move] = sign * MAX_RANGES[axis_to_move];
            return true;
        }
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

        SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();

        std::cout << "Sending MS STATUS command...";

        SRTMinorServoAnswerMap MSStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status());
        EXPECT_EQ(std::get<std::string>(MSStatus["OUTPUT"]), "GOOD");
        EXPECT_EQ(std::get<long>(MSStatus["CONTROL"]), 1);
        EXPECT_EQ(std::get<long>(MSStatus["POWER"]), 1);
        EXPECT_EQ(std::get<long>(MSStatus["EMERGENCY"]), 2);
        EXPECT_EQ(std::get<long>(MSStatus["ENABLED"]), 1);
        std::cout << "OK." << std::endl;

        SRTMinorServoAnswerMap::iterator iterator;
        for(iterator = MSStatus.begin(); iterator != MSStatus.end(); ++iterator)
        {
            std::visit([iterator](const auto& var) mutable { std::cout << iterator->first << ": " << var << std::endl; }, iterator->second);
        }

        std::cout << "Sending initial SRP STATUS command...";

        SRTMinorServoAnswerMap SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
        EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
        EXPECT_EQ(std::get<long>(SRPStatus["SRP_STATUS"]), 1);
        EXPECT_EQ(std::get<long>(SRPStatus["SRP_BLOCK"]), 2);
        std::cout << "OK." << std::endl;

        for(iterator = SRPStatus.begin(); iterator != SRPStatus.end(); ++iterator)
        {
            std::visit([iterator](const auto& var) mutable { std::cout << iterator->first << ": " << var << std::endl; }, iterator->second);
        }

        std::cout << "Sending all axes to the starting position..." << std::endl;

        SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::preset("SRP", startingCoordinates));
        EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");

        signal(SIGINT, SRPProgramTrackTest::sigintHandler);

        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
            EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
            std::cout << serializeStatus(SRPStatus) << std::endl;

            if(terminate)
                FAIL() << "Aborting test..." << std::endl;
        }
        while(std::get<long>(SRPStatus["SRP_OPERATIVE_MODE"]) != 40);
        EXPECT_EQ(std::get<long>(SRPStatus["SRP_OPERATIVE_MODE"]), 40);

        std::cout << "OK." << std::endl;

        startingCoordinates = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

        std::time_t tn = std::time(0);
        std::tm* now = std::localtime(&tn);
        std::stringstream directory_ss;
        directory_ss << "TESTS/SRP";
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

        statusThread = std::thread(&SRPProgramTrackTest::printStatus, directory + "/status.txt");
    }

    void TearDown() override
    {
        SRTMinorServoSocket::destroyInstance();
        terminate = false;
    }
};

TEST_F(SRPProgramTrackTest, ContinuousMovementTest)
{
    double start_time = CIRATools::getUNIXEpoch() + ADVANCE_TIMEGAP;
    std::cout << "PRESET position reached, starting PROGRAMTRACK with start time: " << start_time << std::endl;
    long unsigned int trajectory_id = int(start_time);
    unsigned int point_id = 0;
    std::vector<double> programTrackCoordinates = startingCoordinates;

    SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
    SRTMinorServoAnswerMap SRPStatus;
    SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("SRP", trajectory_id, point_id, programTrackCoordinates, start_time));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
    EXPECT_EQ(std::get<long>(SRPStatus["SRP_OPERATIVE_MODE"]), 50);

    ofstream programTrackFile;
    programTrackFile.open(directory + "/trajectory.txt", ios::out);
    programTrackFile << SRPProgramTrackTest::serializeCoordinates(start_time, programTrackCoordinates) << std::endl;

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
                sign *= -1;
                idle = true;
            }

            SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("SRP", trajectory_id, point_id, programTrackCoordinates));
            EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
            //std::cout << SRPProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
            programTrackFile << SRPProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;

            if(round(programTrackCoordinates[axis_to_move] * 100) == 0 && sign == 1)
            {
                programTrackCoordinates[axis_to_move] = 0.0;
                break;
            }
        }

        axis_to_move == 5 ? axis_to_move = 0 : axis_to_move++;
    }

    programTrackFile.close();
    statusThread.join();
}

TEST_F(SRPProgramTrackTest, AllAxesMovementTest)
{
    double start_time = CIRATools::getUNIXEpoch() + ADVANCE_TIMEGAP;
    std::cout << "PRESET position reached, starting PROGRAMTRACK with start time: " << start_time << std::endl;
    long unsigned int trajectory_id = int(start_time);
    unsigned int point_id = 0;
    std::vector<double> programTrackCoordinates = startingCoordinates;

    SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
    SRTMinorServoAnswerMap SRPStatus;
    SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("SRP", trajectory_id, point_id, programTrackCoordinates, start_time));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
    EXPECT_EQ(std::get<long>(SRPStatus["SRP_OPERATIVE_MODE"]), 50);

    ofstream programTrackFile;
    programTrackFile.open(directory + "/trajectory.txt", ios::out);
    programTrackFile << SRPProgramTrackTest::serializeCoordinates(start_time, programTrackCoordinates) << std::endl;

    double next_expected_time = start_time;
    std::vector<int> sign = { 1, 1, 1 };
    std::vector<unsigned int> idle_count = { 0, 0, 0 };
    std::vector<bool> idle = { false, false, false };

    while(!terminate)
    {
        next_expected_time += TIMEGAP;

        std::this_thread::sleep_for(std::chrono::microseconds((int)round(1000000 * std::max(0.0, next_expected_time - ADVANCE_TIMEGAP - CIRATools::getUNIXEpoch()))));
        point_id++;

        for(size_t axis = 0; axis < 3; axis++)
        {
            if(!idle[axis])
            {
                if(moveAxis(programTrackCoordinates, axis, sign[axis]))
                {
                    sign[axis] *= -1;
                    idle[axis] = true;
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

        SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("SRP", trajectory_id, point_id, programTrackCoordinates));
        EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
        //std::cout << SRPProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
        programTrackFile << SRPProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;

        for(size_t axis = 0; axis < 3; axis++)
        {
            if(round(programTrackCoordinates[axis] * 100) == 0 && sign[axis] == 1)
            {
                programTrackCoordinates[axis] = 0.0;
            }
        }
    }

    programTrackFile.close();
    statusThread.join();
}

TEST_F(SRPProgramTrackTest, SineWaveMovementTest)
{
    double start_time = CIRATools::getUNIXEpoch() + ADVANCE_TIMEGAP;
    std::cout << "PRESET position reached, starting PROGRAMTRACK with start time: " << start_time << std::endl;
    long unsigned int trajectory_id = int(start_time);
    unsigned int point_id = 0;
    std::vector<double> programTrackCoordinates = startingCoordinates;

    std::vector<double> phase_shift;
    for(size_t axis = 0; axis < 3; axis++)
    {
        phase_shift.push_back((double)std::rand() / RAND_MAX * 60);
        programTrackCoordinates[axis] = MAX_RANGES[axis] * sin(phase_shift[axis] * 2 * M_PI / 60);
    }

    SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
    SRTMinorServoAnswerMap SRPStatus;
    SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("SRP", trajectory_id, point_id, programTrackCoordinates, start_time));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
    EXPECT_EQ(std::get<long>(SRPStatus["SRP_OPERATIVE_MODE"]), 50);

    ofstream programTrackFile;
    programTrackFile.open(directory + "/trajectory.txt", ios::out);
    programTrackFile << SRPProgramTrackTest::serializeCoordinates(start_time, programTrackCoordinates) << std::endl;

    double next_expected_time = start_time;

    while(!terminate)
    {
        next_expected_time += TIMEGAP;
        double time_delta = next_expected_time - start_time;

        std::this_thread::sleep_for(std::chrono::microseconds((int)round(1000000 * std::max(0.0, next_expected_time - ADVANCE_TIMEGAP - CIRATools::getUNIXEpoch()))));
        point_id++;

        for(size_t axis = 0; axis < 3; axis++)
        {
            programTrackCoordinates[axis] = MAX_RANGES[axis] * sin((time_delta + phase_shift[axis]) * 2 * M_PI / 60);
        }

        SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("SRP", trajectory_id, point_id, programTrackCoordinates));
        EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
        //std::cout << SRPProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
        programTrackFile << SRPProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
    }

    programTrackFile.close();
    statusThread.join();
}

TEST_F(SRPProgramTrackTest, SeparateMovementTest)
{
    SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
    SRTMinorServoAnswerMap SRPStatus;

    ofstream programTrackFile;
    programTrackFile.open(directory + "/trajectory.txt", ios::out);

    unsigned int axis_to_move = 0;
    int sign = 1;
    unsigned int idle_count = 0;
    bool idle = false;

    std::cout << "PRESET position reached, starting PROGRAMTRACK" << std::endl;
    std::vector<double> programTrackCoordinates = startingCoordinates;

    while(!terminate)
    {
        double start_time = CIRATools::getUNIXEpoch() + ADVANCE_TIMEGAP;
        long unsigned int trajectory_id = int(start_time);
        double next_expected_time = start_time;
        unsigned int point_id = 0;

        SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("SRP", trajectory_id, point_id, programTrackCoordinates, start_time));
        EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
        programTrackFile << SRPProgramTrackTest::serializeCoordinates(start_time, programTrackCoordinates) << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
        EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
        EXPECT_EQ(std::get<long>(SRPStatus["SRP_OPERATIVE_MODE"]), 50);

        while(!terminate)
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
                    sign *= -1;
                    idle = true;
                }
                else if(round(programTrackCoordinates[axis_to_move] * 100) == 0 && sign == 1)
                {
                    programTrackCoordinates[axis_to_move] = 0.0;
                    axis_to_move == 5 ? axis_to_move = 0 : axis_to_move++;
                    idle = true;
                }

                SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("SRP", trajectory_id, point_id, programTrackCoordinates));
                EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
            }

            //std::cout << SRPProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
            programTrackFile << SRPProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
        }
    }

    programTrackFile.close();
    statusThread.join();
}

TEST_F(SRPProgramTrackTest, RapidTrajectoryTest)
{
    SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
    SRTMinorServoAnswerMap SRPStatus;

    ofstream programTrackFile;
    programTrackFile.open(directory + "/trajectory.txt", ios::out);

    unsigned int axis_to_move = 2;  //Always Z
    int sign = -1;
    unsigned int idle_count = 0;
    bool idle = false;

    std::cout << "PRESET position reached, starting PROGRAMTRACK" << std::endl;

    while(!terminate)
    {
        std::vector<double> programTrackCoordinates = startingCoordinates;
        programTrackCoordinates[axis_to_move] = MAX_RANGES[axis_to_move];

        double start_time = CIRATools::getUNIXEpoch() + ADVANCE_TIMEGAP;
        long unsigned int trajectory_id = int(start_time);
        double next_expected_time = start_time;
        unsigned int point_id = 0;

        SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("SRP", trajectory_id, point_id, programTrackCoordinates, start_time));
        EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
        programTrackFile << SRPProgramTrackTest::serializeCoordinates(start_time, programTrackCoordinates) << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
        EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
        EXPECT_EQ(std::get<long>(SRPStatus["SRP_OPERATIVE_MODE"]), 50);

        while(!terminate)
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

                SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("SRP", trajectory_id, point_id, programTrackCoordinates));
                EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
            }

            //std::cout << SRPProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
            programTrackFile << SRPProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
        }
    }

    programTrackFile.close();
    statusThread.join();
}
