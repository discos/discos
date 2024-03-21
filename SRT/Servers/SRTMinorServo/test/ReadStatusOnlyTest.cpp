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
//#define SIMULATION
#ifdef SIMULATION
    #define ADDRESS             std::string("127.0.0.1")
    #define PORT                12800
#else
    #define ADDRESS             std::string("192.168.200.13")
    #define PORT                4758
#endif

#define SOCKET_TIMEOUT          0.5
#define STATUS_PERIOD           0.1
#define SRP_COORDINATES         std::vector<std::string>{ "SRP_TX", "SRP_TY", "SRP_TZ", "SRP_RX", "SRP_RY", "SRP_RZ" }
#define DEROTATOR               std::string("GFR1")
#define DEROTATOR_COORDINATES   std::vector<std::string>{ DEROTATOR + "_ROTATION" }


std::atomic<bool> terminate = false;


class ReadStatusOnlyTest : public ::testing::Test
{
protected:
    std::string directory;

    static void printStatus(std::string directory, bool timestamp_only=false)
    {
        SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance();
        SRTMinorServoAnswerMap SRPStatus, DerotatorStatus;

        ofstream SRPStatusFile, DerotatorStatusFile;
        SRPStatusFile.open(directory + "/SRP/status.txt", ios::out);
        SRPStatusFile << std::fixed << std::setprecision(6);
        DerotatorStatusFile.open(directory + "/DEROTATOR/status.txt", ios::out);
        DerotatorStatusFile << std::fixed << std::setprecision(6);

        long unsigned int counter = 0;

        double tn = CIRATools::getUNIXEpoch();

        while(!terminate)
        {
            SRPStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("SRP"));
            DerotatorStatus = socket.sendCommand(SRTMinorServoCommandLibrary::status("Derotatore" + DEROTATOR));
            std::string SRPStringStatus = serializeSRPStatus(SRPStatus);
            std::string DerotatorStringStatus = serializeDerotatorStatus(DerotatorStatus);

            if(timestamp_only)
            {
                SRPStatusFile << std::get<double>(SRPStatus["TIMESTAMP"]) << std::endl;
                DerotatorStatusFile << std::get<double>(DerotatorStatus["TIMESTAMP"]) << std::endl;
            }
            else
            {
                SRPStatusFile << SRPStringStatus << std::endl;
                DerotatorStatusFile << DerotatorStringStatus << std::endl;
            }

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

    void SetUp() override
    {
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

        signal(SIGINT, ReadStatusOnlyTest::sigintHandler);

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
    }

    void TearDown() override
    {
        SRTMinorServoSocket::destroyInstance();
        terminate = false;
    }
};

TEST_F(ReadStatusOnlyTest, ReadStatusTest)
{
    ReadStatusOnlyTest::printStatus(directory, true);
}
