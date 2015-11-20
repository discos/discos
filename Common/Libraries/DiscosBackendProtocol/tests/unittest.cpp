#include "gtest/gtest.h"
#include "DiscosBackendProtocol"

using namespace DiscosBackend;
using namespace std;

TEST(ParsingTest, FirstTest)
{
    string reply_string("!get-tpi,ok,10.5,12.5\r\n");
    Message reply(reply_string, true);
    EXPECT_EQ(reply.get_type(), REPLY); 
    EXPECT_EQ(reply.get_code(), SUCCESS); 
    EXPECT_EQ(reply.get_argument<double>(0), 10.5); 
    EXPECT_EQ(reply.get_argument<double>(1), 12.5); 
}

TEST(FakeTest, Success){
    EXPECT_EQ(1, 1);
}

