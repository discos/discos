/* These tests require a running simulator and were designed        *
 * to test the thread-safeness and singleton design pattern of the  *
 * SRTMinorServoSocket class.                                       */
#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include "SRTMinorServoSocket.h"
#include "SRTMinorServoCommandLibrary.h"

#define ADDRESS "192.168.10.201"
#define PORT    20000

class SRTMinorServoSocketTest : public ::testing::Test
{
protected:
    std::vector<std::string> commands;
    std::vector<std::thread> threads;

    void SetUp() override
    {
        std::cout << std::fixed << std::setprecision(6);

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
        std::for_each(this->threads.begin(), this->threads.end(), [](std::thread &t)
        {
            t.join();
        });
    }
};

TEST_F(SRTMinorServoSocketTest, instance_passed_to_threads)
{
    SRTMinorServoSocket& socket = SRTMinorServoSocket::getInstance(ADDRESS, PORT);

    for(auto command : this->commands)
    {
        this->threads.push_back(std::thread([command, &socket]()
        {
            auto args = SRTMinorServoCommandLibrary::parseAnswer(socket.sendCommand(command));
            // By testing if the command was received correctly we also test if the socket is working properly
            // and if the answer was received correctly without being interleaved with the answer from another thread
            EXPECT_EQ(std::get<std::string>(args["OUTPUT"]), "GOOD");
        }));
    }
}

TEST_F(SRTMinorServoSocketTest, instance_retrieved_in_threads)
{
    for(auto command : this->commands)
    {
        this->threads.push_back(std::thread([command]()
        {
            auto args = SRTMinorServoCommandLibrary::parseAnswer(SRTMinorServoSocket::getInstance(ADDRESS, PORT).sendCommand(command));
            // By testing if the command was received correctly we also test if the socket is working properly
            // and if the answer was received correctly without being interleaved with the answer from another thread
            EXPECT_EQ(std::get<std::string>(args["OUTPUT"]), "GOOD");
        }));
    }
}
