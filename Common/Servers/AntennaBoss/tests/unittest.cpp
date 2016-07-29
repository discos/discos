#include "gtest/gtest.h"

#include "Offset_test.i"

using namespace AntennaBossTest;

TEST_F(AntennaBoss_Offset, offset_isSetCheck){
	EXPECT_TRUE(offset_isSetCheck());
}

TEST_F(AntennaBoss_Offset, offset_AssignAndReadout){
	EXPECT_TRUE(offset_AssignAndReadout());
}

TEST_F(AntennaBoss_Offset, offset_LValueAndRValue){
	EXPECT_TRUE(offset_LValueAndRValue());
}

/*TEST_F(AntennaBoss_Offset, offset_scanUserCombination){
	EXPECT_TRUE(offset_scanUserCombination());
}*/

