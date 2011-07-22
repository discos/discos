// $Id: TestScheduleTimer.cpp,v 1.1 2008-12-19 17:35:06 a.orlati Exp $


#include <IRA>
#include "ScheduleTimer.h"

using namespace IRA;

bool Runit;

void contCallBack(const ACS::Time& time,const void *par) 
{
	long *index;
	TIMEVALUE epoch(time);
	index=static_cast<long *>(const_cast<void *>(par));
	printf("CONT\t%02d:%02d:%02d.%03d got: %ld\n",epoch.hour(),epoch.minute(),
				epoch.second(),epoch.microSecond()/1000,*index);
}

void oneShotCallBack(const ACS::Time& time,const void *par)
{
	TIMEVALUE epoch(time);
	char *string;
	string=static_cast<char *>(const_cast<void *>(par));
	printf("SHOT\t%02d:%02d:%02d.%03d got: %s\n",epoch.hour(),epoch.minute(),
			epoch.second(),epoch.microSecond()/1000,string);
	Runit=false;
}

int main(int argc, char *argv[]) 
{
	char parameter[128];
	long par1,par2;
	TIMEVALUE now,copy;
	TIMEDIFFERENCE period;
	CScheduleTimer timer;
	now.reset();
	now.normalize(true);
	IRA::CIRATools::getTime(now); //get current time
	Runit=true;
	if (!timer.init()) {
		printf("Error initializing the timer\n");
		return -1;
	}
	// setup a continuos event...every 2.5 seconds
	par1=1;
	period.second(2);
	period.microSecond(500000);
	if (!timer.schedule(&contCallBack,now.value().value,period.value().value,&par1)) {
		printf("Error scheduling the continuos event\n");
		return -1;
	}
	// setup a continuos event...every 0.7 seconds
	par2=2;
	period.reset();
	period.microSecond(700000);
	if (!timer.schedule(&contCallBack,now.value().value,period.value().value,&par2)) {
		printf("Error scheduling the continuos event\n");
		return -1;
	}
	// saves the current time
	copy.value(now.value().value);
	now.second(now.second()+15); // take 15 seconds from now
	now.microSecond(300000); // and sets 300 millisecond to check the resolution of the timer
	// ask the one shot event
	sprintf(parameter,"command asked for %02d:%02d:%02d.%d",now.hour(),now.minute(),now.second(),now.microSecond()/1000);
	if (!timer.schedule(&oneShotCallBack,now.value().value,0,parameter)) {
		printf("Error scheduling the one shot event\n");
		return -1;
	}
	while (Runit) {
		CIRATools::Wait(500000);
	}
	printf("removing the first continuos event\n");
	if (!timer.cancel(copy.value().value)) {
		printf("Error canceling an event\n");
		return -1;		
	}
	CIRATools::Wait(5,0);
	printf("closing up\n");
}

