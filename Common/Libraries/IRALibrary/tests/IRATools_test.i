#include "IRATools.h"

#include <stdlib.h>

namespace IRALibraryTest {

class IRALibrary_IRATools : public ::testing::Test {
public:
	::testing::AssertionResult makeDirectory_createSimpleDirectory() {
		IRA::CString fullPath;
		fullPath=basePath+simpleDirPath;
		RecordProperty("description","check if a single level folder can be created");
		if (IRA::CIRATools::makeDirectory(fullPath)) {
			if (IRA::CIRATools::directoryExists(fullPath)) {
				return ::testing::AssertionSuccess();
			}
			else {
				return ::testing::AssertionFailure() << (const char *) fullPath << " does not exists";
			}
		}
		else {
			return ::testing::AssertionFailure() << (const char *) fullPath << " creation failed";
		}
	}

	::testing::AssertionResult makeDirectory_createComplexDirectory() {
		IRA::CString fullPath;
		fullPath=basePath+complexDirPath;
		::testing::Test::RecordProperty("description","check if a double level folder can be created");
		if (IRA::CIRATools::makeDirectory(fullPath)) {
			if (IRA::CIRATools::directoryExists(fullPath)) {
				return ::testing::AssertionSuccess();
			}
			else {
				return ::testing::AssertionFailure() << (const char *) fullPath << " does not exists";
			}
		}
		else {
			return ::testing::AssertionFailure() << (const char *) fullPath << " creation failed";
		}
	}

	::testing::AssertionResult fileExists_checkExistance() {
		IRA::CString file;
		::testing::Test::RecordProperty("description","check if an existing file is detected");
		file=basePath+fileName;
		if (IRA::CIRATools::fileExists(file)) {
			return ::testing::AssertionSuccess();
		}
		else {
			return ::testing::AssertionFailure() << (const char *) file << " should be present but (fileExists) fails to detect it";
		}
	}

	::testing::AssertionResult fileExists_checkNoExistance() {
		IRA::CString file;
		::testing::Test::RecordProperty("description","check if a not existing file is correctly not detected");
		file="fooAnddymmyFile.test";
		if (IRA::CIRATools::fileExists(file)) {
			return ::testing::AssertionFailure() << (const char *) file << " should not be present but (fileExists) detects it";
		}
		else {
			return ::testing::AssertionSuccess();
		}
	}


protected:
	static IRA::CString simpleDirPath;
	static IRA::CString complexDirPath;
	static IRA::CString fileName;
	IRA::CString basePath;
	virtual void SetUp() {
		IRA::CString command;
		int i;
		char buff[256];
		getcwd(buff,256);
		basePath=buff;
		command="touch "+basePath+fileName;
		i=system((const char *)command);
	}
	virtual void TearDown() {
		IRA::CString cleanCommand;
		int i;
		cleanCommand="rm -rf "+basePath+simpleDirPath;
		i=system((const char *)cleanCommand);
		cleanCommand="rm "+basePath+fileName;
		i=system((const char *)cleanCommand);
	}
};

IRA::CString IRALibrary_IRATools::simpleDirPath = IRA::CString("/firstLevelDirectory");
IRA::CString IRALibrary_IRATools::complexDirPath = simpleDirPath+IRA::CString("/secondLevelDirectory");
IRA::CString IRALibrary_IRATools::fileName = IRA::CString("/dummyFile.tst");
}
