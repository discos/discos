
#include "gtest/procedurelist.cpp"
#include "ScheduleReport_test.i"
#include "gtest/Schedule_test.i"

using namespace SchedulerComponentTests;

TEST_F(Scheduler_ScheduleReport, checkBuissinessLogic_ReportNone){
	EXPECT_TRUE(checkBuissinessLogic_ReportNone());
}

TEST_F(Scheduler_ScheduleReport, checkBuissinessLogic_ReportOnlyLock){
	EXPECT_TRUE(checkBuissinessLogic_ReportOnlyLock());
}

TEST_F(Scheduler_ScheduleReport, checkBuissinessLogic_ReportFull){
	EXPECT_TRUE(checkBuissinessLogic_ReportFull());
}

TEST_F(Scheduler_Schedule,checkSchedule_generalCheck) {
	EXPECT_TRUE(checkSchedule_generalCheck());
}

TEST_F(Scheduler_Schedule,checkSchedule_checkScanConfiguration) {
	EXPECT_TRUE(checkSchedule_checkScanConfiguration());
}

TEST_F(Scheduler_Schedule,checkSchedule_checkTargetConsistency) {
	EXPECT_TRUE(checkSchedule_checkTargetConsistency());
}

TEST_F(Scheduler_Schedule, checkScanList_generalCheck) {
	EXPECT_TRUE(checkScanList_generalCheck());
}

TEST_F(Scheduler_Schedule, checkScanList_getScanTarget) {
	EXPECT_TRUE(checkScanList_getScanTarget());
}


