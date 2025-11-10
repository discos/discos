
#include "gtest/gtest.h"
#include "Configuration_test.i"

using namespace KQWComponentTests;

TEST_F(KQWComponent_Configuration, InitializeConfiguration) {
	EXPECT_TRUE(InitializeConfiguration_ResultOk());
}

TEST_F(KQWComponent_Configuration, ReadConfiguration) {
	EXPECT_TRUE(ReadConfiguration());
}

TEST_F(KQWComponent_Configuration, setMode) {
	EXPECT_TRUE(setMode());
}

TEST_F(KQWComponent_Configuration, LNAbypass_check) {
	EXPECT_TRUE(LNAbypass_check());
}
