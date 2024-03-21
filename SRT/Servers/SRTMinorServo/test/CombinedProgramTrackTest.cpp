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

#define SOCKET_TIMEOUT          0.5
#define NOISE_THRESHOLD         1
#define TIMEGAP                 0.2
#define ADVANCE_TIMEGAP         5
#define STATUS_PERIOD           0.01
#define EPSILON                 0.00001

#define SRP_COORDINATES         std::vector<std::string>{ "SRP_TX", "SRP_TY", "SRP_TZ", "SRP_RX", "SRP_RY", "SRP_RZ" }
#define SRP_MAX_RANGES          std::vector<double>{ 50, 110, 50, 0.25, 0.25, 0.25 }
#define SRP_MAX_SPEED           std::vector<double>{ 4.0, 4.0, 4.0, 0.04, 0.04, 0.04 }

#define DEROTATOR               std::string("GFR1")
#define DEROTATOR_COORDINATES   std::vector<std::string>{ DEROTATOR + "_ROTATION" }
#define DEROTATOR_RANGES        std::vector<double>{ 10.0, 100.0 }
#define DEROTATOR_MAX_SPEED     std::vector<double>{ 3.3 }


std::atomic<bool> terminate = false;


class CombinedProgramTrackTest : public ::testing::Test
{
protected:
    std::vector<double> SRPStartingCoordinates = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    std::vector<double> DerotatorStartingCoordinates = { 10.0 };
    std::string directory;
    std::thread statusThread;

    static void printStatus(std::string directory)
    {
        SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
        SRTMinorServoAnswerMap SRPStatus, DerotatorStatus;

        ofstream SRPStatusFile, DerotatorStatusFile;
        SRPStatusFile.open(directory + "/SRP/status.txt", ios::out);
        DerotatorStatusFile.open(directory + "/DEROTATOR/status.txt", ios::out);

        long unsigned int counter = 0;

        double tn = CIRATools::getUNIXEpoch();

        while(!terminate)
        {
            SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
            DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("Derotatore" + DEROTATOR));
            std::string SRPStringStatus = serializeSRPStatus(SRPStatus);
            std::string DerotatorStringStatus = serializeDerotatorStatus(DerotatorStatus);

            SRPStatusFile << SRPStringStatus << std::endl;
            DerotatorStatusFile << DerotatorStringStatus << std::endl;
            if(counter % 10 == 0)
            {
                std::cout << SRPStringStatus << std::endl;
                std::cout << DerotatorStringStatus << std::endl;
            }
            counter++;

            tn += STATUS_PERIOD;
            std::this_thread::sleep_for(std::chrono::microseconds((int)round(1000000 * std::max(0.0, tn - CIRATools::getUNIXEpoch()))));
        }

        SRPStatusFile.close();
        DerotatorStatusFile.close();
    }

    static void printSRPStatus(std::string filename)
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
            std::string status = serializeSRPStatus(SRPStatus);

            statusFile << status << std::endl;
            if(counter % 10 == 0)
                std::cout << status << std::endl;
            counter++;

            tn += STATUS_PERIOD;
            std::this_thread::sleep_for(std::chrono::microseconds((int)round(1000000 * std::max(0.0, tn - CIRATools::getUNIXEpoch()))));
        }

        statusFile.close();
    }

    static void printDerotatorStatus(std::string filename)
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
            std::string status = serializeDerotatorStatus(DerotatorStatus);

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

    static std::string serializeSRPStatus(SRTMinorServoAnswerMap map)
    {
        std::string status = serializeCoordinates(std::get<double>(map["TIMESTAMP"]), getCoordinates(map, SRP_COORDINATES));
        status += serializeElongations(getElongations(map));
        return status;
    }

    static std::string serializeDerotatorStatus(SRTMinorServoAnswerMap map)
    {
        return serializeCoordinates(std::get<double>(map["TIMESTAMP"]), getCoordinates(map, DEROTATOR_COORDINATES));
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

    static std::vector<double> getCoordinates(SRTMinorServoAnswerMap SRPStatus, std::vector<std::string> coordinates)
    {
        std::vector<double> currentCoordinates;

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

        SRTMinorServoAnswerMap DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("Derotatore" + DEROTATOR));
        EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
        EXPECT_EQ(std::get<long>(DerotatorStatus[DEROTATOR + "_STATUS"]), 1);
        EXPECT_EQ(std::get<long>(DerotatorStatus[DEROTATOR + "_BLOCK"]), 2);

        for(iterator = DerotatorStatus.begin(); iterator != DerotatorStatus.end(); ++iterator)
        {
            std::visit([iterator](const auto& var) mutable { std::cout << iterator->first << ": " << var << std::endl; }, iterator->second);
        }

        std::cout << "OK." << std::endl;
        std::cout << "Sending all axes to 0..." << std::endl;

        SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::preset("SRP", SRPStartingCoordinates));
        EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
        DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::preset("Derotatore" + DEROTATOR, DerotatorStartingCoordinates));
        EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");

        signal(SIGINT, CombinedProgramTrackTest::sigintHandler);

        bool SRPReady = false, DerotatorReady = false;

        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
            EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
            std::cout << serializeSRPStatus(SRPStatus) << std::endl;
            SRPReady = std::get<long>(SRPStatus["SRP_OPERATIVE_MODE"]) == 40 ? true : false;

            DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("Derotatore" + DEROTATOR));
            EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");
            std::cout << serializeDerotatorStatus(DerotatorStatus) << std::endl;
            DerotatorReady = std::get<long>(DerotatorStatus[DEROTATOR + "_OPERATIVE_MODE"]) == 40 ? true : false;

            if(terminate)
                FAIL() << "Aborting test..." << std::endl;
        }
        while(!SRPReady || !DerotatorReady);
        EXPECT_EQ(std::get<long>(SRPStatus["SRP_OPERATIVE_MODE"]), 40);
        EXPECT_EQ(std::get<long>(DerotatorStatus[DEROTATOR + "_OPERATIVE_MODE"]), 40);

        std::cout << "OK." << std::endl;

        std::time_t tn = std::time(0);
        std::tm* now = std::localtime(&tn);
        std::stringstream directory_ss;
        directory_ss << "TESTS/COMBINED";
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
        boost::filesystem::create_directory(directory + "/SRP");
        boost::filesystem::create_directory(directory + "/DEROTATOR");

        statusThread = std::thread(&CombinedProgramTrackTest::printStatus, directory);
    }

    void TearDown() override
    {
        SRTMinorServoSocket::destroyInstance();
        terminate = false;
    }
};

TEST_F(CombinedProgramTrackTest, SineWaveMovementTest)
{
    SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
    SRTMinorServoAnswerMap SRPStatus, DerotatorStatus;

    double start_time = CIRATools::getUNIXEpoch() + ADVANCE_TIMEGAP;
    std::cout << "PRESET position reached, starting PROGRAMTRACK with start time: " << start_time << std::endl;
    long unsigned int trajectory_id = int(start_time);
    unsigned int point_id = 0;
    std::vector<double> SRPCoordinates = SRPStartingCoordinates;
    std::vector<double> DerotatorCoordinates = DerotatorStartingCoordinates;

    std::vector<double> phase_shift;
    std::vector<double> period;
    for(size_t axis = 0; axis < 6; axis++)
    {
        period.push_back(SRP_MAX_RANGES[axis] / SRP_MAX_SPEED[axis] * 4);
        phase_shift.push_back((double)std::rand() / RAND_MAX * period[axis]);
        SRPCoordinates[axis] = SRP_MAX_RANGES[axis] * sin(phase_shift[axis] * 2 * M_PI / period[axis]);
    }

    double derotator_amplitude = (DEROTATOR_RANGES[1] - DEROTATOR_RANGES[0]) / 2;
    double derotator_center = (DEROTATOR_RANGES[0] + DEROTATOR_RANGES[1]) / 2;
    // Derotator period
    period.push_back(80);
    phase_shift.push_back((double)std::rand() / RAND_MAX * period[6]);
    DerotatorCoordinates[0] = derotator_center + derotator_amplitude * sin(phase_shift[6] * 2 * M_PI / period[6]);

    SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("SRP", trajectory_id, point_id, SRPCoordinates, start_time));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
    DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("Derotatore" + DEROTATOR, trajectory_id, point_id, DerotatorCoordinates, start_time));
    EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
    EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
    EXPECT_EQ(std::get<long>(SRPStatus["SRP_OPERATIVE_MODE"]), 50);

    DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("Derotatore" + DEROTATOR));
    EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");
    EXPECT_EQ(std::get<long>(DerotatorStatus[DEROTATOR + "_OPERATIVE_MODE"]), 50);

    ofstream SRPProgramTrackFile;
    SRPProgramTrackFile.open(directory + "/SRP/trajectory.txt", ios::out);
    SRPProgramTrackFile << CombinedProgramTrackTest::serializeCoordinates(start_time, SRPCoordinates) << std::endl;

    ofstream DerotatorProgramTrackFile;
    DerotatorProgramTrackFile.open(directory + "/DEROTATOR/trajectory.txt", ios::out);
    DerotatorProgramTrackFile << CombinedProgramTrackTest::serializeCoordinates(start_time, DerotatorCoordinates) << std::endl;

    double next_expected_time = start_time;

    while(!terminate)
    {
        next_expected_time += TIMEGAP;
        double time_delta = next_expected_time - start_time;

        std::this_thread::sleep_for(std::chrono::microseconds((int)round(1000000 * std::max(0.0, next_expected_time - ADVANCE_TIMEGAP - CIRATools::getUNIXEpoch()))));
        point_id++;

        for(size_t axis = 0; axis < 6; axis++)
        {
            SRPCoordinates[axis] = SRP_MAX_RANGES[axis] * sin((time_delta + phase_shift[axis]) * 2 * M_PI / period[axis]);
        }
        DerotatorCoordinates[0] = derotator_center + derotator_amplitude * sin((time_delta + phase_shift[6]) * 2 * M_PI / period[6]);

        SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("SRP", trajectory_id, point_id, SRPCoordinates));
        EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
        SRPProgramTrackFile << CombinedProgramTrackTest::serializeCoordinates(next_expected_time, SRPCoordinates) << std::endl;

        DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("Derotatore" + DEROTATOR, trajectory_id, point_id, DerotatorCoordinates));
        EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");
        DerotatorProgramTrackFile << CombinedProgramTrackTest::serializeCoordinates(next_expected_time, DerotatorCoordinates) << std::endl;
    }

    SRPProgramTrackFile.close();
    DerotatorProgramTrackFile.close();
    statusThread.join();
}

TEST_F(CombinedProgramTrackTest, SineWaveSeparateMovementTest)
{
    SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
    SRTMinorServoAnswerMap SRPStatus, DerotatorStatus;

    ofstream SRPProgramTrackFile;
    SRPProgramTrackFile.open(directory + "/SRP/trajectory.txt", ios::out);

    ofstream DerotatorProgramTrackFile;
    DerotatorProgramTrackFile.open(directory + "/DEROTATOR/trajectory.txt", ios::out);

    while(!terminate)
    {
        double start_time = CIRATools::getUNIXEpoch() + ADVANCE_TIMEGAP;
        std::cout << "Starting new trajectory with start time: " << start_time << std::endl;
        long unsigned int trajectory_id = int(start_time);
        unsigned int point_id = 0;
        std::vector<double> SRPCoordinates = SRPStartingCoordinates;
        std::vector<double> DerotatorCoordinates = DerotatorStartingCoordinates;

        std::vector<double> phase_shift;
        std::vector<double> period;
        for(size_t axis = 0; axis < 6; axis++)
        {
            period.push_back(SRP_MAX_RANGES[axis] / SRP_MAX_SPEED[axis] * 4);
            phase_shift.push_back((double)std::rand() / RAND_MAX * period[axis]);
            SRPCoordinates[axis] = SRP_MAX_RANGES[axis] * sin(phase_shift[axis] * 2 * M_PI / period[axis]);
        }

        double derotator_amplitude = (DEROTATOR_RANGES[1] - DEROTATOR_RANGES[0]) / 2;
        double derotator_center = (DEROTATOR_RANGES[0] + DEROTATOR_RANGES[1]) / 2;
        // Derotator period
        period.push_back(80);
        phase_shift.push_back((double)std::rand() / RAND_MAX * period[6]);
        DerotatorCoordinates[0] = derotator_center + derotator_amplitude * sin(phase_shift[6] * 2 * M_PI / period[6]);

        SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("SRP", trajectory_id, point_id, SRPCoordinates, start_time));
        EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
        DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("Derotatore" + DEROTATOR, trajectory_id, point_id, DerotatorCoordinates, start_time));
        EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");

        SRPProgramTrackFile << CombinedProgramTrackTest::serializeCoordinates(start_time, SRPCoordinates) << std::endl;
        DerotatorProgramTrackFile << CombinedProgramTrackTest::serializeCoordinates(start_time, DerotatorCoordinates) << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
        EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
        EXPECT_EQ(std::get<long>(SRPStatus["SRP_OPERATIVE_MODE"]), 50);

        DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("Derotatore" + DEROTATOR));
        EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");
        EXPECT_EQ(std::get<long>(DerotatorStatus[DEROTATOR + "_OPERATIVE_MODE"]), 50);

        double next_expected_time = start_time;

        while(!terminate)
        {
            next_expected_time += TIMEGAP;
            double time_delta = next_expected_time - start_time;

            std::this_thread::sleep_for(std::chrono::microseconds((int)round(1000000 * std::max(0.0, next_expected_time - ADVANCE_TIMEGAP - CIRATools::getUNIXEpoch()))));
            point_id++;

            for(size_t axis = 0; axis < 6; axis++)
            {
                SRPCoordinates[axis] = SRP_MAX_RANGES[axis] * sin((time_delta + phase_shift[axis]) * 2 * M_PI / period[axis]);
            }
            DerotatorCoordinates[0] = derotator_center + derotator_amplitude * sin((time_delta + phase_shift[6]) * 2 * M_PI / period[6]);

            SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("SRP", trajectory_id, point_id, SRPCoordinates));
            EXPECT_EQ(std::get<std::string>(SRPStatus["OUTPUT"]), "GOOD");
            SRPProgramTrackFile << CombinedProgramTrackTest::serializeCoordinates(next_expected_time, SRPCoordinates) << std::endl;

            DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::programTrack("Derotatore" + DEROTATOR, trajectory_id, point_id, DerotatorCoordinates));
            EXPECT_EQ(std::get<std::string>(DerotatorStatus["OUTPUT"]), "GOOD");
            DerotatorProgramTrackFile << CombinedProgramTrackTest::serializeCoordinates(next_expected_time, DerotatorCoordinates) << std::endl;

            if(point_id == 1000)
                break;
        }
    }

    SRPProgramTrackFile.close();
    DerotatorProgramTrackFile.close();
    statusThread.join();
}
