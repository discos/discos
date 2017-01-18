
#include "gtest/procedurelist.cpp"
#include "ScheduleReport_test.i"

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


