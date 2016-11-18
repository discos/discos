#include "gtest/gtest.h"

#include "Tone_test.i"

using namespace NotoReceiversTest;

TEST_F(NotoReceivers_Tone, Tone_checkConnectionTimeOut){
	EXPECT_TRUE(Tone_checkConnectionTimeOut());
}

TEST_F(NotoReceivers_Tone, Tone_checkToneSwitchingOn){
	EXPECT_TRUE(Tone_checkToneSwitchingOn());
}

TEST_F(NotoReceivers_Tone, Tone_checkToneSwitchingOff){
	EXPECT_TRUE(Tone_checkToneSwitchingOff());
}





