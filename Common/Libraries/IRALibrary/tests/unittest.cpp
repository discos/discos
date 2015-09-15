#include "gtest/gtest.h"

#include "IRATools_test.i"


using namespace IRALibraryTest;

TEST_F(IRALibrary_IRATools, makeDirectory_createSimpleDirectory){
	EXPECT_TRUE(makeDirectory_createSimpleDirectory());
}

TEST_F(IRALibrary_IRATools, makeDirectory_createComplexDirectory){
	EXPECT_TRUE(makeDirectory_createComplexDirectory());
}

TEST_F(IRALibrary_IRATools, fileExists_checkExistance){
	EXPECT_TRUE(fileExists_checkExistance());
}

TEST_F(IRALibrary_IRATools, fileExists_checkNoExistance){
	EXPECT_TRUE(fileExists_checkNoExistance());
}
