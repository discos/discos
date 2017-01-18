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

	::testing::AssertionResult skyFrequency_noIntersection() {
		double f,bw;
		double rf=100;
		double rbw=500;
		double bf=600;
		double bbw=300;
		::testing::Test::RecordProperty("description","check if the resulting sky frequency is actually empty");
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			return ::testing::AssertionFailure() << "resulting band should be empty but bandwidth is " << bw;
		}
		rf=100;
		rbw=500;
		bf=-800;
		bbw=100;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			return ::testing::AssertionFailure() << "resulting band should be empty but bandwidth is " << bw;
		}
		else {
			return ::testing::AssertionSuccess();
		}
	}

	::testing::AssertionResult skyFrequency_intersection() {
		double f,bw;
		double rf=100;
		double rbw=200;
		double bf=200;
		double bbw=200;
		::testing::Test::RecordProperty("description","check (various data set) if the resulting sky frequency is not empty and band limits are correct");

		// R=100,200(100:300)U B=200,200(200:400)U => 200,100(200:300)
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=200) {
				return ::testing::AssertionFailure() << "1) straight receiver, straight backend, start frequency should be 200 but is " << f << " instead";
			}
			else if (bw!=100) {
				return ::testing::AssertionFailure() << "1) straight receiver, straight backend, start bandwidth should be 100 but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "1) straight receiver, straight backend, resulting band should not be empty" ;
		}
		// R=100,200(100:300)U B=150,50(150:200)U => 150,50(150:200)
		rf=100;
		rbw=200;
		bf=150;
		bbw=50;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=bf) {
				return ::testing::AssertionFailure() << "2) straight receiver, straight backend, backend included, start frequency should be "
						<< bf << " but is " << f << " instead";
			}
			else if (bw!=bbw) {
				return ::testing::AssertionFailure() << "2) straight receiver, straight backend, backend included, start bandwidth should be "
						<< bbw << " but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "2) straight receiver, straight backend, backend included, resulting band should not be empty" ;
		}
		// R=100,200(100:300)U B=50,500(50:550)U => 100,200(100:300)
		rf=100;
		rbw=200;
		bf=50;
		bbw=500;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=rf) {
				return ::testing::AssertionFailure() << "3) straight receiver, straight backend, receiver included, start frequency should be "
						<< rf << " but is " << f << " instead";
			}
			else if (bw!=rbw) {
				return ::testing::AssertionFailure() << "3) straight receiver, straight backend, receiver included, start bandwidth should be "
						<< rbw << " but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "3) straight receiver, straight backend, receiver included, resulting band should not be empty" ;
		}
		// R=-300,200(100:300)L B=200,100(200:300)U => -200,-100
		rf=-300;
		rbw=200;
		bf=200;
		bbw=100;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=-200) {
				return ::testing::AssertionFailure() << "4) inverted receiver, straight backend, start frequency should be -200 but is " << f << " instead";
			}
			else if (bw!=-100) {
				return ::testing::AssertionFailure() << "4) inverted receiver, straight backend, start bandwidth should be -100 but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "4) inverted receiver, straight backend, resulting band should not be empty" ;
		}
		// R=-300,200(100:300)L B=100,100(100:300)U => -100,-100
		rf=-300;
		rbw=200;
		bf=100;
		bbw=100;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=-100) {
				return ::testing::AssertionFailure() << "5) inverted receiver, straight backend, start frequency should be -200 but is " << f << " instead";
			}
			else if (bw!=-100) {
				return ::testing::AssertionFailure() << "5) inverted receiver, straight backend, start bandwidth should be -100 but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "5) inverted receiver, straight backend, resulting band should not be empty" ;
		}
		// R=-300,200(300:100)L B=150,50(150:200)U => -150,-50
		rf=-300;
		rbw=200;
		bf=150;
		bbw=50;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=-bf) {
				return ::testing::AssertionFailure() << "6) inverted receiver, straight backend, backend included, start frequency should be "
						<< -bf << " but is " << f << " instead";
			}
			else if (bw!=-bbw) {
				return ::testing::AssertionFailure() << "6) inverted receiver, straight backend, backend included, start bandwidth should be "
						<< -bbw << " but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "6) inverted receiver, straight backend, backend included, resulting band should not be empty" ;
		}
		// R=-300,200(300:100)L B=50,500(50:550)U => -100,-200
		rf=-300;
		rbw=200;
		bf=50;
		bbw=500;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=-100) {
				return ::testing::AssertionFailure() << "7) inverted receiver, straight backend, receiver included, start frequency should be -100"
						<< " but is " << f << " instead";
			}
			else if (bw!=-200) {
				return ::testing::AssertionFailure() << "7) inverted receiver, straight backend, receiver included, start bandwidth should be -200"
						<< " but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "7) inverted receiver, straight backend, receiver included, resulting band should not be empty" ;
		}
		// R=100,200(100:300)U B=-150,100(50:150)L => 150,-50
		rf=100;
		rbw=200;
		bf=-150;
		bbw=100;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=150) {
				return ::testing::AssertionFailure() << "8) straight receiver, inverted backend, start frequency should be 150"
						<< " but is " << f << " instead";
			}
			else if (bw!=-50) {
				return ::testing::AssertionFailure() << "8) straight receiver, inverted backend, start bandwidth should be -50"
						<< " but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "8) straight receiver, inverted backend, resulting band should not be empty" ;
		}
		// R=-300,200(100:300)L B=-200,100(100:200)L => -200,100
		rf=-300;
		rbw=200;
		bf=-200;
		bbw=100;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=-200) {
				return ::testing::AssertionFailure() << "9) inverted receiver, inverted backend, start frequency should be -200"
						<< " but is " << f << " instead";
			}
			else if (bw!=100) {
				return ::testing::AssertionFailure() << "9) inverted receiver, inverted backend, start bandwidth should be 100"
						<< " but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "9) inverted receiver, inverted backend, resulting band should not be empty" ;
		}
		// R=-300,200(100:300)L B=-200,100(100:200)L => -200,100
		rf=-1000;
		rbw=500;
		bf=50;
		bbw=2000;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=-500) {
				return ::testing::AssertionFailure() << "10) inverted receiver, straight backend, start frequency should be -500"
						<< " but is " << f << " instead";
			}
			else if (bw!=-500) {
				return ::testing::AssertionFailure() << "10) inverted receiver, straight backend, start bandwidth should be -500"
						<< " but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "9) inverted receiver, inverted backend, resulting band should not be empty" ;
		}
		return ::testing::AssertionSuccess();
	}

	::testing::AssertionResult createEmptyFile_checkSuccess() {
		IRA::CString file;
		::testing::Test::RecordProperty("description","check the creation of an empty file");
		file=basePath+emptyFileName;
		if (IRA::CIRATools::createEmptyFile(file)) {
			if (IRA::CIRATools::fileExists(file)) {
				IRA::CString cleanCommand;
				int i;
				cleanCommand="rm "+file;
				i=system((const char *)cleanCommand);
				return ::testing::AssertionSuccess();
			}
			else {
				return ::testing::AssertionFailure() << (const char *) file << " file should not be created but (fileExists) does not detect it";
			}
		}
		else {
			return ::testing::AssertionFailure() << (const char *) file << " file cannot be created";
		}
	}

	::testing::AssertionResult createEmptyFile_checkFail() {
		IRA::CString file;
		::testing::Test::RecordProperty("description","check if the creation fails when it is the case");
		file="/foo/boo/dummy/boogie.tst";
		if (!IRA::CIRATools::createEmptyFile(file)) {
			return ::testing::AssertionSuccess();
		}
		else {
			return ::testing::AssertionFailure() << (const char *) file << " does not fail as expected";
		}
	}

	::testing::AssertionResult deleteFile_checkSuccess() {
		IRA::CString file;
		IRA::CString command;
		int i;
		::testing::Test::RecordProperty("description","check if a file can be deleted from filesystem");
		file=basePath+dummyFileName;
		command="touch "+file;
		i=system((const char *)command);
		if (IRA::CIRATools::deleteFile(file)) {
			return ::testing::AssertionSuccess();
		}
		else {
			return ::testing::AssertionFailure() << (const char *) file << " the file could not be deleted";
		}
	}

	::testing::AssertionResult extractFileName_checkSimpleDirectoryPath() {
		IRA::CString name;
		IRA::CString baseDir,baseName,extension;
		::testing::Test::RecordProperty("description","check is a fully qualified directory path can be decomposed correctly");
		// simple directory name
		name="/system/archive";
		if (!IRA::CIRATools::extractFileName(name,baseDir,baseName,extension)) {
			return ::testing::AssertionFailure() << "simple directory could not be decomposed";
		}
		else if (baseDir!="/system") {
			return ::testing::AssertionFailure() << "base directory of " << (const char *)name << " is unexpectedly " <<
					(const char *) baseDir;
		}
		else if (baseName!="archive") {
			return ::testing::AssertionFailure() << "base name of " << (const char *)name << " is unexpectedly " <<
					(const char *) baseName;
		}
		else if (extension!="") {
			return ::testing::AssertionFailure() << "extension token of " << (const char *)name << " is unexpectedly " <<
					(const char *) extension;
		}
		return ::testing::AssertionSuccess();
	}

	::testing::AssertionResult extractFileName_checkSimpleFilePath() {
		IRA::CString name;
		IRA::CString baseDir,baseName,extension;
		::testing::Test::RecordProperty("description","check is a fully qualified file path can be decomposed correctly");
		// simple directory name
		name="/system/archive/document.foo";
		if (!IRA::CIRATools::extractFileName(name,baseDir,baseName,extension)) {
			return ::testing::AssertionFailure() << "simple file path could not be decomposed";
		}
		else if (baseDir!="/system/archive") {
			return ::testing::AssertionFailure() << "base directory of " << (const char *)name << " is unexpectedly " <<
					(const char *) baseDir;
		}
		else if (baseName!="document") {
			return ::testing::AssertionFailure() << "base name of " << (const char *)name << " is unexpectedly " <<
					(const char *) baseName;
		}
		else if (extension!="foo") {
			return ::testing::AssertionFailure() << "extension token of " << (const char *)name << " is unexpectedly " <<
					(const char *) extension;
		}
		return ::testing::AssertionSuccess();
	}

protected:
	static IRA::CString simpleDirPath;
	static IRA::CString complexDirPath;
	static IRA::CString fileName;
	static IRA::CString emptyFileName;
	static IRA::CString dummyFileName;
	IRA::CString basePath;

	static void TearDownTestCase()
	{
	}

	static void SetUpTestCase()
	{
	}

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
IRA::CString IRALibrary_IRATools::fileName = IRA::CString("/normalFile.tst");
IRA::CString IRALibrary_IRATools::emptyFileName = IRA::CString("/emptyFile.tst");
IRA::CString IRALibrary_IRATools::dummyFileName = IRA::CString("/dummyFile.tst");
}
