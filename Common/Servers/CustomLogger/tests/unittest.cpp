#include "gtest/gtest.h"
#include "expat_log_parsing.h"

using namespace std;

TEST(Sanitize, GoodString)
{
    string input("<element>this is good XML</element>");
    string sanitized = sanitize_xml(input.c_str());
    EXPECT_STREQ(input.c_str(), sanitized.c_str());
}

TEST(Sanitize, BadChars)
{
    stringstream input_stream;
    input_stream.put(0x01);
    input_stream.put(0x1F);
    input_stream.put(0x81);
    input_stream.put(0xFF);
    string output("????");
    string sanitized = sanitize_xml(input_stream.str().c_str());
    EXPECT_STREQ(output.c_str(), sanitized.c_str());
}

/*
TEST(Sanitize, SquareBrackets)
{
    string input("ciao [Marco]");
    string output("ciao {Marco}");
    string sanitized = sanitize_xml(input.c_str());
    EXPECT_STREQ(output.c_str(), sanitized.c_str());
}*/

