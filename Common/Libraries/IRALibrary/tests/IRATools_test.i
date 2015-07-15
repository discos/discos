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

protected:
	static IRA::CString simpleDirPath;
	static IRA::CString complexDirPath;
	IRA::CString basePath;
	virtual void SetUp() {
		char buff[256];
		getcwd(buff,256);
		basePath=buff;
	}
	virtual void TearDown() {
		IRA::CString cleanCommand;
		int i;
		cleanCommand="rm -rf "+basePath+simpleDirPath;
		i=system((const char *)cleanCommand);
	}
};

IRA::CString IRALibrary_IRATools::simpleDirPath = IRA::CString("/firstLevelDirectory");
IRA::CString IRALibrary_IRATools::complexDirPath = simpleDirPath+IRA::CString("/secondLevelDirectory");
}
