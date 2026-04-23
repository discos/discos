
#include "gtest/gtest.h"
#include "Configuration_test.i"

using namespace BulkDataZMQTests;

TEST_F(ZMQConfiguration_Configuration, InitializeConfiguration) {
	EXPECT_TRUE(InitializeConfiguration_ResultOk());
}

TEST_F(ZMQConfiguration_Configuration, ReadConfiguration) {
	EXPECT_TRUE(ReadConfiguration());
}

TEST_F(ZMQConfiguration_Configuration, GetByComponentName) {
	EXPECT_TRUE(GetByComponentName());
}
