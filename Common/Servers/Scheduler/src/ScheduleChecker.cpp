//$Id: ScheduleChecker.cpp,v 1.1 2008-12-30 17:15:00 a.orlati Exp $

#include "Schedule.h"
#include <getopt.h>
#include <libgen.h>

using namespace IRA;

bool g_batch=false;

void printMessage(const char *msg)
{
	if (!g_batch) {
		printf(msg);
	}
}

void printHelp() {
	printMessage("Check the correctness of a schedule\n");
	printMessage("\n");
	printMessage("ScheduleChecker [-h] [-b] [-d] scheduleFile \n");
	printMessage("\n");
	printMessage("-h	Shows this help\n");
	printMessage("-b	Selects the batch mode, no output message are shown. In that case the program will exit with 0\n");
	printMessage("    if the schedule is correct, with -1 if not \n");
	printMessage("-d  Sets the debug mode, the subscans parameters and configuration from the schedule are printed out \n");
	printMessage("    '-b' overloads this argument");	
	printMessage("\n");
}

int main(int argc, char *argv[]) 
{
	int out;
	char scheduleFile[256];
	char dir[256];
	char name[256];
	bool debugmode=false;
	if (argc==1) {
		printMessage("Arguments are missing....\n\n");
		printHelp();
		exit(-1);
	}
	while ((out=getopt(argc,argv,"vbd"))!=-1) {
		switch (out) {
			case 'h' : {
				printHelp();
				break;
			}
			case 'b': {
				g_batch=true;
				break;
			}
			case 'd': {
				debugmode=true;
				break;
			}
			default: {
				printMessage("Invalid argument....\n\n");
				printHelp();
				exit(-1);
			}
		}
	}
	if (g_batch) debugmode=false;
	strcpy(scheduleFile,argv[optind]);
	if (strlen(scheduleFile)==0) {
		printMessage("A schedule file must be provided....\n\n");
		printHelp();
		exit(-1);
	}
	strcpy(name,basename(scheduleFile));
	strcpy(dir,dirname(scheduleFile));
	strcat(dir,"/");
	CSchedule sched(dir,name);
	
	if (!sched.readAll(true)) {
		CString err;
		err.Format("Error found: %s\n",(const char *)sched.getLastError());
		printMessage((const char *)err);
		exit(-1);
	}
	if (sched.isComplete()) {
		CString msg;
		msg.Format("%u subscans were successfully parsed!\n",sched.getSubScansNumber());
		printMessage((const char *)msg);
		printMessage("Schedule is correct\n");
		if (debugmode) sched.printAll();
		return 0;
	}
	else {
		CString err;
		err.Format("Schedule is missing some section, please check it again!\n");
		printMessage((const char *)err);
		exit(-1);
	}
	return 0;
}
