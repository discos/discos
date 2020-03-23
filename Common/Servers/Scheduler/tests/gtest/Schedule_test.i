#include <IRATools.h>
#include "Schedule.h"

#include <stdlib.h>

namespace SchedulerComponentTests {

class Scheduler_Schedule : public ::testing::Test {
public:
	testing::AssertionResult checkSchedule_generalCheck()
	{
		RecordProperty("description","check if the schedule parsing and handling is correct");
		CSchedule sched("./artefacts/",Scheduler_Schedule::scheduleCorrect);
		CSchedule::TRecord rec;
		if (!sched.readAll(true)) {
			return testing::AssertionFailure() << " error reading schedule, error message is: " <<
					(const char *)sched.getLastError();
		}
		if (!sched.isComplete()) {
			return testing::AssertionFailure() << " not all required keyword are present";
		}
		if (sched.getSchedMode()!=CSchedule::SEQ) {
			return testing::AssertionFailure() << " schedule mode differs from expected";
		}
		if (sched.getSubScanCounter("1_10")!=3) {
			return testing::AssertionFailure() << " subscan string id does not correspond";
		}
		if (sched.getSubScanCounter(1,10)!=3) {
			return testing::AssertionFailure() << " subscan ids id does not correspond";
		}
		if (sched.getIdentifiers(4)!="1_11") {
			return testing::AssertionFailure() << " subscan counter does not correspond";
		}
		if (!sched.getSubScan(0,rec)) {
			return testing::AssertionFailure() << " subscan could not be retrieved";
		}		
		return ::testing::AssertionSuccess();
	}
	
	testing::AssertionResult checkSchedule_checkScanConfiguration()
	{
		RecordProperty("description","check if the schedule correctly forms the scan configuration");
		CSchedule sched("./artefacts/",Scheduler_Schedule::scheduleCorrect);
		CSchedule::TRecord rec1,rec2;
		if (!sched.readAll(true)) {
			return testing::AssertionFailure() << " error reading schedule, error message is: " <<
					(const char *)sched.getLastError();
		}
		if (!sched.isComplete()) {
			return testing::AssertionFailure() << " not all required keyword are present";
		}
		if (!sched.getSubScan(1,rec1)) {
			return testing::AssertionFailure() << " cannot retrieve a scan";
		}
		if (!sched.getSubScan(2,rec2)) {
			return testing::AssertionFailure() << " cannot retrieve a scan";
		}
		if ((rec1.scanConf->scanID!=rec2.scanConf->scanID) || 
		  (rec1.scanConf->subScansNumber!=rec2.scanConf->subScansNumber) ||
		  (rec1.scanConf->timeStamp!=rec2.scanConf->timeStamp)) {
			return testing::AssertionFailure() << " cannot verify first scan configuration";		
		} 
		if (!sched.getSubScan(sched.getSubScansNumber()-1,rec2)) {
			return testing::AssertionFailure() << " cannot retrieve a scan";
		}	
		if (rec2.scanConf->subScansNumber!=2) {
			return testing::AssertionFailure() << " cannot verify second scan configuration";		
		} 
		return ::testing::AssertionSuccess();
	}

	testing::AssertionResult checkSchedule_checkTargetConsistency()
	{
		RecordProperty("description","check if the schedule detects correctly a target consistency");
		CSchedule sched("./artefacts/",Scheduler_Schedule::scheduleTargetError);
		CSchedule::TRecord rec;
		if (sched.readAll(true)) {
			return testing::AssertionFailure() << " schedule error was not detected";
		}
		else {
			return ::testing::AssertionSuccess() << " schedule error correctly detected";
		}
		// (const char *)sched.getLastError();		
	}	

	testing::AssertionResult checkScanList_generalCheck()
	{
		RecordProperty("description","check if the list of sub scans handles correctly the data");
		CSchedule sched("./artefacts/",Scheduler_Schedule::scheduleCorrect);
		Schedule::CScanList *scans;
				
		if (!sched.readAll(true)) {
			return testing::AssertionFailure() << " error reading schedule, error message is: " <<
					(const char *)sched.getLastError();
		}
		scans=sched.getScanList();		
		if (scans==NULL) {
			return testing::AssertionFailure() << " list of scans could not be retrieved";
		}
		if (!scans->checkScan(1)) {
			return testing::AssertionFailure() << " subscan 1 could not be checked, error message is " <<
					(const char *)scans->getLastError(); 				
		}
		if (scans->checkScan(0)) { // scan 0 should not be existent
			return testing::AssertionFailure() << " subscan 0 should not be present but it is actually available in the list"; 				
		}		
		return ::testing::AssertionSuccess();
	}

	testing::AssertionResult checkScanList_getScanTarget()
	{
		IRA::CString name;
		double r2000,d2000;
		RecordProperty("description","check if the ability of the list of sub scans to extract scan target");
		CSchedule sched("./artefacts/",Scheduler_Schedule::scheduleCorrect);
		Schedule::CScanList *scans;
				
		if (!sched.readAll(true)) {
			return testing::AssertionFailure() << " error reading schedule, error message is: " <<
					(const char *)sched.getLastError();
		}
		scans=sched.getScanList();		
		if (scans==NULL) {
			return testing::AssertionFailure() << " list of scans could not be retrieved";
		}
		for(int i=1;i<=16;i++) {
			if (!scans->getScanTarget(i,name,r2000,d2000)) {
				IRA::CString msg;
				msg.Format("subscan %d could not be retrieved, error message is %s",i,(const char *)scans->getLastError());
				return testing::AssertionFailure() << (const char *)msg;				
			}
		}
		return ::testing::AssertionSuccess();
	}

protected:
	static IRA::CString scheduleCorrect;
	static IRA::CString scheduleTargetError;

	static void TearDownTestCase()
	{
	}

	static void SetUpTestCase()
	{
	}

	virtual void SetUp() {

	}
	virtual void TearDown() {

	}
};

IRA::CString Scheduler_Schedule::scheduleCorrect = IRA::CString("sched_1_correct.scd");
IRA::CString Scheduler_Schedule::scheduleTargetError = IRA::CString("sched_1_no_correct.scd");

}
