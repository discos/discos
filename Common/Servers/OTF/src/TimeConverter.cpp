//$Id: TimeConverter.cpp,v 1.1 2009-04-27 08:14:48 s.righini Exp $ 

#include "IRA"
#include <DateTime.h>
#include <slamac.h>
#include <getopt.h>
#include <iostream>



void printHelp() {
	printf("Time converter from/to MJD-UT-LST\n");
	printf("\n");
	printf("TimeConverter [-h help]\n");
	printf("              [-m mjd] \n");
	printf("			  [-d dut1] \n");		
	//printf("              [-u UT]  \n");
	//printf("              [-s Sid]  \n");
	printf("\n");
	printf("[-h help]               Shows this help\n");
	printf("[-m modified julian day]  Accepts MJD as input xxxxx.xxxxx\n");	
	//printf("[-u universal time]  Accepts UT as input year-doy-hh:mm:ss.sss\n");
	//printf("[-s sidereal time]  Accepts Sidereal Time as input  year-doy-hh:mm:ss.sss\n");
	printf("\n");
}

bool parseTime(const IRA::CString& str,ACS::Time& time)
{
	long hh,mm,year,doy;
	double ss;
	double fract;
	if (sscanf((const char*)str,"%ld-%ld-%ld:%ld:%lf",&year,&doy,&hh,&mm,&ss)==5 ) {
		TIMEVALUE tt;
		tt.reset();
		tt.normalize(true);
		tt.year((unsigned long)year);
		tt.dayOfYear(doy);
		tt.hour(hh);
		tt.minute(mm);
		tt.second((long)ss);
		fract=ss-(long)ss;
		tt.microSecond((long)(fract*1000000));
		time=tt.value().value;
		return true;
	}
	else {
		return false;
	}	
}

using namespace IRA;
	
int main(int argc, char *argv[]){
	
	double mjd, dut1;
	TIMEVALUE ut,sid;
	CDateTime jd;
	int out;
	CSite site;
	
	site.geodeticCoordinates(0.203277276,0.777029135,25);
	
	if (argc==1) {
		printf("Missing  arguments.......\n\n");
		printHelp();
		exit(-1);
	}
	
	dut1=0.0;
	
	while ((out=getopt(argc,argv,"hm:d:"))!=-1) {
		switch (out) {
			case 'm' : {
				if (sscanf(optarg,"%lf",&mjd)!=1) {
					printf("incorrect mjd value\n\n");
					printHelp();
					exit(-1);
				}
				break;
			}
			case 'd' : {
				if (sscanf(argv[optind],"%lf",&dut1)!=1) {
					printf("incorrect dut1 value\n\n");
					printHelp();
					exit(-1);
				}
				break;				
			}
			case 'h': {
				printHelp();
				exit(0);
			}
		}
	}
	
	const long double julianday=mjd+DT_MJD0;
	//const double dut1=0.0;
	jd.setJD(julianday);
	jd.getDateTime(ut);
	printf("UT: %04d-%02d-%02d-%02d:%02d:%02d.%03d\n",ut.year(),ut.month(),ut.day(),
			ut.hour(),ut.minute(),ut.second(),ut.microSecond()/1000);
	jd.LST(site).getDateTime(sid);
	printf("LST: %02d:%02d:%02d.%03d\n",sid.hour(),sid.minute(),sid.second(),sid.microSecond()/1000);	
	printf("\n");
	return 0;
	
}

