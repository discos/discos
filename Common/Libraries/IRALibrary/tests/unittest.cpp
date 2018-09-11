#include "gtest/gtest.h"

#include "IRATools_test.i"
#include "FastQueue_test.i"
#include "Socket_test.i"

#include "CError_regression.i"
#include "CDBTable_regression.i"
#include "CIRATools_regression.i"

#include "CString_test.i"

#include "BaseConverter_test.i"

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

TEST_F(IRALibrary_IRATools, createEmptyFile_checkSuccess){
	EXPECT_TRUE(createEmptyFile_checkSuccess());
}

TEST_F(IRALibrary_IRATools, createEmptyFile_checkFail){
	EXPECT_TRUE(createEmptyFile_checkFail());
}

TEST_F(IRALibrary_IRATools, deleteFile_checkSuccess){
	EXPECT_TRUE(deleteFile_checkSuccess());
}

TEST_F(IRALibrary_IRATools,extractFileName_checkSimpleDirectoryPath){
	EXPECT_TRUE(extractFileName_checkSimpleDirectoryPath());
}

TEST_F(IRALibrary_IRATools,extractFileName_checkSimpleFilePath){
	EXPECT_TRUE(extractFileName_checkSimpleFilePath());
}

TEST_F(IRALibrary_IRATools, skyFrequency_noIntersection){
	EXPECT_TRUE(skyFrequency_noIntersection());
}

TEST_F(IRALibrary_IRATools,skyFrequency_intersection){
	EXPECT_TRUE(skyFrequency_intersection());
}

TEST_F(IRALibrary_FastQueue,FastQueue_checkLimits){
	EXPECT_TRUE(FastQueue_checkLimits());
}

TEST_F(IRALibrary_FastQueue,FastQueue_checkConsistency){
	EXPECT_TRUE(FastQueue_checkConsistency());
}

TEST_F(IRALibrary_Socket,sendWithoutConnection){
	EXPECT_TRUE(sendWithoutConnection());
}

TEST_F(IRALibrary_CError,copyConstructor_segfault){
	EXPECT_TRUE(copyConstructor_segfault());
}

TEST_F(IRALibrary_CDataField,typeConversion_fail){
	EXPECT_TRUE(typeConversion_fail());
}

TEST_F(IRALibrary_String,string_checkLeft){
	EXPECT_TRUE(string_checkLeft());
}

TEST_F(IRALibrary_String,string_checkSplit) {
	EXPECT_TRUE(string_checkSplit());
}

TEST_F(IRALibrary_BaseConverter,decToBin_checkConversion) {
	EXPECT_TRUE(decToBin_checkConversion());
}

TEST_F(IRALibrary_BaseConverter,decToHex_checkConversion) {
	EXPECT_TRUE(decToHex_checkConversion());
}

TEST_F(IRALibrary_CIRATools_regression,radToSexagesimalAngle_badconversion) {
	EXPECT_TRUE(radToSexagesimalAngle_badconversion());
}

TEST_F(IRALibrary_CIRATools_regression,radToHourAngle_badconversion) {
	EXPECT_TRUE(radToHourAngle_badconversion());
}

