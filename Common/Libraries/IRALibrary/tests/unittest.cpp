#include "gtest/gtest.h"

#include "IRATools_test.i"


using namespace IRALibraryTest;

TEST_F(IRALibrary_IRATools, MakeDirectory){
	EXPECT_TRUE(makeDirectory_createSimpleDirectory());
	EXPECT_TRUE(makeDirectory_createComplexDirectory());
}
