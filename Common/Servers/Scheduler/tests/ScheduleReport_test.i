#include <IRATools.h>
#include "ScheduleReport.h"

#include <stdlib.h>

namespace SchedulerComponentTests {

class Scheduler_ScheduleReport : public ::testing::Test {
public:
	::testing::AssertionResult checkBuissinessLogic_ReportNone()
	{
		IRA::CString empty("");
		IRA::CString post;
		RecordProperty("description","check the report class when all empty arguments are provided");
		CScheduleReport reportNone(empty,empty,empty,empty);

		// None
		if (reportNone.getPostFix()!="") {
			return ::testing::AssertionFailure() << " postfix token should not be generated";
		}
		reportNone.addScheduleName(Scheduler_ScheduleReport::sampleSchedule);
		post=reportNone.getPostFix();
		if (post=="") {
			return ::testing::AssertionFailure() << " postfix token should not be empty";
		}
		reportNone.addAuxScheduleFile(Scheduler_ScheduleReport::sampleScheduleAux);
		// let's activate the report
		if (!reportNone.activate()) {
			return ::testing::AssertionFailure() << " error activating the report, error message is: " <<
					(const char *)reportNone.getLastError();
		}
		if (!reportNone.deactivate()) {
			return ::testing::AssertionFailure() << " error deactivating the report, error message is: " <<
					(const char *)reportNone.getLastError();
		}
		return ::testing::AssertionSuccess();
	}

	::testing::AssertionResult checkBuissinessLogic_ReportOnlyLock()
	{
		IRA::CString empty("");
		IRA::CString post;
		RecordProperty("description","check the report class when just the lock file argument is provided");
		CScheduleReport reportOnlyLock(empty,empty,empty,Scheduler_ScheduleReport::lockFile);

		if (!reportOnlyLock.activate()) {
			return ::testing::AssertionFailure() << " error activating the report, error message is: " <<
					(const char *)reportOnlyLock.getLastError();
		}
		if (!CIRATools::fileExists(Scheduler_ScheduleReport::lockFile)) {
			return ::testing::AssertionFailure() << " lock file is not correctly created";
		}
		if (!reportOnlyLock.deactivate()) {
			return ::testing::AssertionFailure() << " error deactivating the report, error message is: " <<
					(const char *)reportOnlyLock.getLastError();
		}
		if (CIRATools::fileExists(Scheduler_ScheduleReport::lockFile)) {
			return ::testing::AssertionFailure() << " lock file is not correctly deleted";
		}
		return ::testing::AssertionSuccess();
	}

	::testing::AssertionResult checkBuissinessLogic_ReportFull()
	{
		IRA::CString empty("");
		IRA::CString post;
		RecordProperty("description","check the report class when all arguments are provided");
		CScheduleReport reportFull(Scheduler_ScheduleReport::logPath,Scheduler_ScheduleReport::reportPath,
				Scheduler_ScheduleReport::backupPath,Scheduler_ScheduleReport::lockFile);

		reportFull.addScheduleName(Scheduler_ScheduleReport::sampleSchedule);
		reportFull.addAuxScheduleFile(Scheduler_ScheduleReport::sampleScheduleAux);
		reportFull.addScanPath("scan1Path");
		reportFull.addScanPath("scan2Path");
		reportFull.addScanPath("scan3Path");
		reportFull.addScanPath("scan4Path");
		reportFull.addScanPath("scan5Path");

		if (!reportFull.activate()) {
			return ::testing::AssertionFailure() << " error activating the report, error message is: " <<
					(const char *)reportFull.getLastError();
		}
		// check the lock file----
		if (!CIRATools::fileExists(Scheduler_ScheduleReport::lockFile)) {
			return ::testing::AssertionFailure() << " lock file is not correctly created";
		}
		if (!reportFull.deactivate()) {
			return ::testing::AssertionFailure() << " error deactivating the report, error message is: " <<
					(const char *)reportFull.getLastError();
		}
		// check the report file
		if (!CIRATools::fileExists(reportFull.getReportFileName())) {
			return ::testing::AssertionFailure() << " report file is not correctly created";
		}
		if (!CIRATools::directoryExists(reportFull.getBackupFolder())) {
			return ::testing::AssertionFailure() << " schedule backup not correctly performed";
		}
		if (CIRATools::fileExists(Scheduler_ScheduleReport::lockFile)) {
			return ::testing::AssertionFailure() << " lock file is not correctly deleted";
		}
		return ::testing::AssertionSuccess();
	}

protected:
	static IRA::CString logPath;
	static IRA::CString reportPath;
	static IRA::CString backupPath;
	static IRA::CString lockFile;
	static IRA::CString sampleSchedule;
	static IRA::CString sampleScheduleAux;

	static void TearDownTestCase()
	{
		IRA::CString command;
		int i;
		command="rm -rf "+Scheduler_ScheduleReport::reportPath;
		i=system((const char *)command);
		command="rm -rf "+Scheduler_ScheduleReport::backupPath;
		i=system((const char *)command);
	}

	static void SetUpTestCase()
	{
		IRA::CString command;
		int i;
		command="mkdir -p "+Scheduler_ScheduleReport::reportPath;
		i=system((const char *)command);
		command="mkdir -p "+Scheduler_ScheduleReport::backupPath;
		i=system((const char *)command);
	}

	virtual void SetUp() {

	}
	virtual void TearDown() {

	}
};

IRA::CString Scheduler_ScheduleReport::logPath = IRA::CString("./TestBench/Logs/");
IRA::CString Scheduler_ScheduleReport::reportPath = IRA::CString("./TestBench/Reports/");
IRA::CString Scheduler_ScheduleReport::backupPath = IRA::CString("./TestBench/Backup/");
IRA::CString Scheduler_ScheduleReport::lockFile = IRA::CString("./file.lck");
IRA::CString Scheduler_ScheduleReport::sampleSchedule = IRA::CString("./artefacts/schedule_text.scd");
IRA::CString Scheduler_ScheduleReport::sampleScheduleAux = IRA::CString("./artefacts/schedule_text.aux");

}
