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
#define TIMEGAP             0.2
#define ADVANCE_TIMEGAP     5
#define EPSILON             0.00001
#define STATUS_PERIOD       0.01
#define RANGES              std::vector<double>{ 10.0, 50.0 }
#define DEROTATOR           std::string("GFR1")

std::atomic<bool> terminate = false;


class DerotatorProgramTrackTest : public ::testing::Test
{
protected:
    std::vector<double> startingCoordinates = { RANGES[0] };
    std::string directory;
    std::thread statusThread;

    static void printStatus(std::string filename)
    {
        SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
        SRTMinorServoAnswerMap DerotatorStatus;

        ofstream statusFile;
        statusFile.open(filename, ios::out);

        long unsigned int counter = 0;

        double tn = CIRATools::getUNIXEpoch();

        while(!terminate)
        {
            DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("Derotatore" + DEROTATOR));
            std::string status = serializeStatus(DerotatorStatus);

            statusFile << status << std::endl;
            if(counter % (long unsigned int)(0.1 / STATUS_PERIOD) == 0)
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
        return serializeCoordinates(std::get<double>(map["TIMESTAMP"]), getCoordinates(map));
    }

    static std::string serializeCoordinates(double timestamp, std::vector<double> coordinates)
    {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(6) << timestamp;
        for(double coordinate : coordinates)
            stream << "\t" << coordinate;
        return stream.str();
    }

    static std::vector<double> getCoordinates(SRTMinorServoAnswerMap DerotatorStatus)
    {
        std::vector<double> currentCoordinates;
        std::vector<std::string> coordinates = { DEROTATOR + "_ROTATION" };

        for(std::string coordinate : coordinates)
        {
            auto value = DerotatorStatus[coordinate];

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

    static bool moveAxis(std::vector<double> &coordinates, int axis_to_move, int sign)
    {
        sign = sign / abs(sign);
        double offset_to_add = 3.3 * TIMEGAP;
        coordinates[axis_to_move] += sign * offset_to_add;
        if(sign > 0)
        {
            coordinates[axis_to_move] = std::min(RANGES[1], coordinates[axis_to_move]);
        }
        else
        {
            coordinates[axis_to_move] = std::max(RANGES[0], coordinates[axis_to_move]);
        }
        if(coordinates[axis_to_move] == RANGES[0] || coordinates[axis_to_move] == RANGES[1])
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

        std::cout << "Sending initial Derotator STATUS command...";

        SRTMinorServoAnswerMap DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("Derotatore" + DEROTATOR));
        EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");
        EXPECT_EQ(std::get<long>(DerotatorStatus[DEROTATOR + "_STATUS"]), 1);
        EXPECT_EQ(std::get<long>(DerotatorStatus[DEROTATOR + "_BLOCK"]), 2);
        std::cout << "OK." << std::endl;

        for(iterator = DerotatorStatus.begin(); iterator != DerotatorStatus.end(); ++iterator)
        {
            std::visit([iterator](const auto& var) mutable { std::cout << iterator->first << ": " << var << std::endl; }, iterator->second);
        }

        std::cout << "Sending derotator to the initial position..." << std::endl;

        DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::preset("Derotatore" + DEROTATOR, startingCoordinates));
        EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");

        signal(SIGINT, DerotatorProgramTrackTest::sigintHandler);

        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("Derotatore" + DEROTATOR));
            EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");

            std::cout << serializeStatus(DerotatorStatus) << std::endl;

            if(terminate)
                FAIL() << "Aborting test..." << std::endl;
        }
        while(std::get<long>(DerotatorStatus[DEROTATOR + "_OPERATIVE_MODE"]) != 40);
        EXPECT_EQ(std::get<long>(DerotatorStatus[DEROTATOR + "_OPERATIVE_MODE"]), 40);

        std::cout << "OK." << std::endl;

        std::time_t tn = std::time(0);
        std::tm* now = std::localtime(&tn);
        std::stringstream directory_ss;
        directory_ss << "TESTS/DEROTATOR";
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

        statusThread = std::thread(&DerotatorProgramTrackTest::printStatus, directory + "/status.txt");
    }

    void TearDown() override
    {
        SRTMinorServoSocket::destroyInstance();
        terminate = false;
    }
};

TEST_F(DerotatorProgramTrackTest, ContinuousMovementTest)
{
    double start_time = CIRATools::getUNIXEpoch() + ADVANCE_TIMEGAP;
    std::cout << "PRESET position reached, starting PROGRAMTRACK with start time: " << start_time << std::endl;
    long unsigned int trajectory_id = int(start_time);
    unsigned int point_id = 0;
    std::vector<double> programTrackCoordinates = startingCoordinates;

    SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
    SRTMinorServoAnswerMap DerotatorStatus;
    std::string command = SRTMinorServoCommandLibrary::programTrack("Derotatore" + DEROTATOR, trajectory_id, point_id, programTrackCoordinates, start_time);
    std::cout << command << std::endl;
    DerotatorStatus = socket.sendCommand(command);
    EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("Derotatore" + DEROTATOR));
    EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");
    EXPECT_EQ(std::get<long>(DerotatorStatus[DEROTATOR + "_OPERATIVE_MODE"]), 50);

    ofstream programTrackFile;
    programTrackFile.open(directory + "/trajectory.txt", ios::out);
    programTrackFile << DerotatorProgramTrackTest::serializeCoordinates(start_time, programTrackCoordinates) << std::endl;

    double next_expected_time = start_time;
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
            else if(moveAxis(programTrackCoordinates, 0, sign))
            {
                sign *= -1;
                idle = true;
            }

            DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("Derotatore" + DEROTATOR, trajectory_id, point_id, programTrackCoordinates));
            EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");
            //std::cout << DerotatorProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
            programTrackFile << DerotatorProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;

            if(round(programTrackCoordinates[0] * 100) == 10 && sign == 1)
            {
                programTrackCoordinates[0] = RANGES[0];
                break;
            }
        }
    }

    programTrackFile.close();
    statusThread.join();
}

TEST_F(DerotatorProgramTrackTest, SineWaveMovementTest)
{
    double start_time = CIRATools::getUNIXEpoch() + ADVANCE_TIMEGAP;
    std::cout << "PRESET position reached, starting PROGRAMTRACK with start time: " << start_time << std::endl;
    long unsigned int trajectory_id = int(start_time);
    unsigned int point_id = 0;
    std::vector<double> programTrackCoordinates = startingCoordinates;

    double phase_shift = (double)std::rand() / RAND_MAX * 60;
    double amplitude = (RANGES[1] - RANGES[0]) / 2;
    double center = (RANGES[0] + RANGES[1]) / 2;
    double period = 80;
    programTrackCoordinates[0] = center + amplitude * sin(phase_shift * 2 * M_PI / period);

    SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
    SRTMinorServoAnswerMap DerotatorStatus;
    DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("Derotatore" + DEROTATOR, trajectory_id, point_id, programTrackCoordinates, start_time));
    EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("Derotatore" + DEROTATOR));
    EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");
    EXPECT_EQ(std::get<long>(DerotatorStatus[DEROTATOR + "_OPERATIVE_MODE"]), 50);

    ofstream programTrackFile;
    programTrackFile.open(directory + "/trajectory.txt", ios::out);
    programTrackFile << DerotatorProgramTrackTest::serializeCoordinates(start_time, programTrackCoordinates) << std::endl;

    double next_expected_time = start_time;

    while(!terminate)
    {
        next_expected_time += TIMEGAP;
        double time_delta = next_expected_time - start_time;

        std::this_thread::sleep_for(std::chrono::microseconds((int)round(1000000 * std::max(0.0, next_expected_time - ADVANCE_TIMEGAP - CIRATools::getUNIXEpoch()))));
        point_id++;

        programTrackCoordinates[0] = center + amplitude * sin((time_delta + phase_shift) * 2 * M_PI / period);

        DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("Derotatore" + DEROTATOR, trajectory_id, point_id, programTrackCoordinates));
        EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");
        //std::cout << DerotatorProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
        programTrackFile << DerotatorProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
    }

    programTrackFile.close();
    statusThread.join();
}

TEST_F(DerotatorProgramTrackTest, SeparateMovementTest)
{
    SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
    SRTMinorServoAnswerMap DerotatorStatus;

    ofstream programTrackFile;
    programTrackFile.open(directory + "/trajectory.txt", ios::out);

    int sign = 1;
    unsigned int idle_count = 0;
    bool idle = false;

    std::cout << "PRESET position reached, starting PROGRAMTRACK" << std::endl;
    std::vector<double> programTrackCoordinates = startingCoordinates;

    bool immediate = true;

    while(!terminate)
    {
        double start_time = CIRATools::getUNIXEpoch() + ADVANCE_TIMEGAP;
        long unsigned int trajectory_id = int(start_time);
        double next_expected_time = start_time;
        unsigned int point_id = 0;

        DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("Derotatore" + DEROTATOR, trajectory_id, point_id, programTrackCoordinates, start_time));
        EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");
        programTrackFile << DerotatorProgramTrackTest::serializeCoordinates(start_time, programTrackCoordinates) << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("Derotatore" + DEROTATOR));
        EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");
        EXPECT_EQ(std::get<long>(DerotatorStatus[DEROTATOR + "_OPERATIVE_MODE"]), 50);

        while(!terminate)
        {
            next_expected_time += TIMEGAP;

            std::this_thread::sleep_for(std::chrono::microseconds((int)round(1000000 * std::max(0.0, next_expected_time - ADVANCE_TIMEGAP - CIRATools::getUNIXEpoch()))));
            point_id++;

            if(idle)
            {
                idle_count++;
                //if(idle_count == ADVANCE_TIMEGAP / TIMEGAP)
                if(idle_count == ADVANCE_TIMEGAP / TIMEGAP || immediate)
                {
                    idle_count = 0;
                    idle = false;
                    break;
                }
            }
            else
            {
                if(moveAxis(programTrackCoordinates, 0, sign))
                {
                    sign *= -1;
                    idle = true;
                }

                DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("Derotatore" + DEROTATOR, trajectory_id, point_id, programTrackCoordinates));
                EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");
            }

            //std::cout << DerotatorProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
            programTrackFile << DerotatorProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
        }

        //immediate = immediate ? false : true;
    }

    programTrackFile.close();
    statusThread.join();
}

TEST_F(DerotatorProgramTrackTest, RapidTrajectoryTest)
{
    SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
    SRTMinorServoAnswerMap DerotatorStatus;

    ofstream programTrackFile;
    programTrackFile.open(directory + "/trajectory.txt", ios::out);

    int sign = -1;
    unsigned int idle_count = 0;
    bool idle = false;

    std::cout << "PRESET position reached, starting PROGRAMTRACK" << std::endl;

    while(!terminate)
    {
        std::vector<double> programTrackCoordinates = startingCoordinates;
        programTrackCoordinates[0] = RANGES[1];

        double start_time = CIRATools::getUNIXEpoch() + ADVANCE_TIMEGAP;
        long unsigned int trajectory_id = int(start_time);
        double next_expected_time = start_time;
        unsigned int point_id = 0;

        DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("Derotatore" + DEROTATOR, trajectory_id, point_id, programTrackCoordinates, start_time));
        EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");
        programTrackFile << DerotatorProgramTrackTest::serializeCoordinates(start_time, programTrackCoordinates) << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("Derotatore" + DEROTATOR));
        EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");
        EXPECT_EQ(std::get<long>(DerotatorStatus[DEROTATOR + "_OPERATIVE_MODE"]), 50);

        while(!terminate)
        {
            next_expected_time += TIMEGAP;

            std::this_thread::sleep_for(std::chrono::microseconds((int)round(1000000 * std::max(0.0, next_expected_time - ADVANCE_TIMEGAP - CIRATools::getUNIXEpoch()))));
            point_id++;

            if(idle)
            {
                idle_count++;
                if(idle_count == (ADVANCE_TIMEGAP * 2) / TIMEGAP)
                {
                    idle_count = 0;
                    idle = false;
                    break;
                }
            }
            else
            {
                if(moveAxis(programTrackCoordinates, 0, sign))
                {
                    idle = true;
                }

                DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("Derotatore" + DEROTATOR, trajectory_id, point_id, programTrackCoordinates));
                EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");
            }

            //std::cout << DerotatorProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
            programTrackFile << DerotatorProgramTrackTest::serializeCoordinates(next_expected_time, programTrackCoordinates) << std::endl;
        }
    }

    programTrackFile.close();
    statusThread.join();
}
