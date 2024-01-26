/* These tests require a running simulator and were designed        *
 * to test the thread-safeness and singleton design pattern of the  *
 * SRTMinorServoSocket class.                                       */
#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include "SRTMinorServoCommandLibrary.h"

// This address and port are the ones set in the simulator
// In order for the test to properly be executed, the simulator should be launched with the following command:
// discos-simulator -s minor_servo start &
#define ADDRESS std::string("127.0.0.1")
#define PORT    12800

class SRTMinorServoSocketTest;
#define FRIEND_CLASS_DECLARATION friend class ::SRTMinorServoSocketTest;
#include "SRTMinorServoTestingSocket.h"

using namespace MinorServo;

class SRTMinorServoSocketTest : public ::testing::Test
{
protected:
    std::vector<std::string> commands;
    std::vector<std::thread> threads;

    void SetUp() override
    {
        // The following commands yield an articulated answer in return
        commands.push_back(SRTMinorServoCommandLibrary::status());
        commands.push_back(SRTMinorServoCommandLibrary::status("PFP"));
        commands.push_back(SRTMinorServoCommandLibrary::status("SRP"));
        commands.push_back(SRTMinorServoCommandLibrary::status("DerotatoreGFR1"));
        commands.push_back(SRTMinorServoCommandLibrary::status("DerotatoreGFR2"));
        commands.push_back(SRTMinorServoCommandLibrary::status("DerotatoreGFR3"));
        commands.push_back(SRTMinorServoCommandLibrary::status("DerotatorePFP"));
        commands.push_back(SRTMinorServoCommandLibrary::status("M3R"));
        commands.push_back(SRTMinorServoCommandLibrary::status("GFR"));
    }

    void TearDown() override
    {
        SRTMinorServoTestingSocket::destroyInstance();
    }
};

// This test passes the already created instance to some threads
TEST_F(SRTMinorServoSocketTest, instance_passed_to_threads)
{
    SRTMinorServoSocket& socket = SRTMinorServoTestingSocket::getInstance(ADDRESS, PORT);

    if(!socket.isConnected())
    {
        FAIL() << "Socket failed to connect. Check if the simulator or the hardware can be reached." << std::endl;
    }

    for(auto command : this->commands)
    {
        this->threads.push_back(std::thread([command, &socket]()
        {
            auto args = socket.sendCommand(command);
            // By testing if the command was received correctly we also test if the socket is working properly
            // and if the answer was received correctly without being interleaved with the answer from another thread
            EXPECT_TRUE(args.checkOutput());
        }));
    }

    std::for_each(this->threads.begin(), this->threads.end(), [](std::thread &t)
    {
        t.join();
    });
}

// This test spawns some threads, each one retrieves the instance. The first thread which tries to retrieve the instance will also generate it
TEST_F(SRTMinorServoSocketTest, instance_retrieved_in_threads)
{
    std::string error = "";

    for(auto command : this->commands)
    {
        std::mutex mutex;

        this->threads.push_back(std::thread([command, &error, &mutex]()
        {
            try
            {
                auto args = SRTMinorServoTestingSocket::getInstance(ADDRESS, PORT).sendCommand(command);
                // By testing if the command was received correctly we also test if the socket is working properly
                // and if the answer was received correctly without being interleaved with the answer from another thread
                EXPECT_TRUE(args.checkOutput());
            }
            catch(MinorServoErrors::CommunicationErrorExImpl& ex)
            {
                std::lock_guard<std::mutex> guard(mutex);

                if(ex.getReason() == std::string("Cannot connect the socket.").c_str())
                {
                    error = "Socket failed to connect. Check if the simulator or the hardware can be reached.";
                }
                else
                {
                    error = "Unexpected failure.";
                }
            }
        }));
    }

    std::for_each(this->threads.begin(), this->threads.end(), [](std::thread &t)
    {
        t.join();
    });

    if(error != "")
    {
        FAIL() << error << std::endl;
    }
}

// This test generates an instance on the given address and port, then tries to generate another instance with different address and port and fails
TEST_F(SRTMinorServoSocketTest, open_with_args_retrieve_without)
{
    try
    {
        // First let's open the socket with the chosen ADDRESS and PORT
        SRTMinorServoTestingSocket::getInstance(ADDRESS, PORT);

        // Let's try to instance another socket on a different port
        SRTMinorServoTestingSocket::getInstance(ADDRESS, PORT + 1);
    }
    catch(MinorServoErrors::CommunicationErrorExImpl& ex)
    {
        if(ex.getReason() == std::string("Cannot connect the socket.").c_str())
        {
            FAIL() << "Socket failed to connect. Check if the simulator or the hardware can be reached." << std::endl;
            return;
        }
        else
        {
            // Check if we got the correct exception
            EXPECT_EQ(ex.getReason(), std::string("Socket already open on '" + ADDRESS + ":" + std::to_string(PORT) + "' . Use getInstance() (no arguments) to retrieve the object.").c_str());
        }
    }
}

// This test tries to retrieve an instance which has not been generated yet, failing
TEST_F(SRTMinorServoSocketTest, try_open_without_args)
{
    try
    {
        SRTMinorServoTestingSocket::getInstance();
    }
    catch(MinorServoErrors::CommunicationErrorExImpl& ex)
    {
        if(ex.getReason() == std::string("Cannot connect the socket.").c_str())
        {
            FAIL() << "Socket failed to connect. Check if the simulator or the hardware can be reached." << std::endl;
            return;
        }
        else
        {
            // Check if we got the correct exception
            EXPECT_EQ(ex.getReason(), std::string("Socket not yet initialized. Use getInstance(std::string ip_address, int port) to initialize it and retrieve the object.").c_str());
        }
    }
}

// This test tries to generate an instance using a pair of address and port on which the socket fails to open
TEST_F(SRTMinorServoSocketTest, try_open_on_wrong_address)
{
    try
    {
        // The exception is raised only if the given port is wrong
        SRTMinorServoTestingSocket::getInstance(ADDRESS, 0);
    }
    catch(MinorServoErrors::CommunicationErrorExImpl& ex)
    {
        // Check if we got the correct exception
        EXPECT_EQ(ex.getReason(), std::string("Cannot connect the socket.").c_str());
    }
}
