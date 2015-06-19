#include "TestClient.h"
#include <getopt.h>
#include <iostream>
#include <Site.h>
#include "SlewCheck.h"

/** 
 * This is a text client aimed at testing the slewCheck class 
 **/

void printHelp() {
	printf("slewCheck class test\n");
	printf("\n");
	printf("[-a initAz(deg)] default: 90.0\n");
	printf("[-e initEl(deg)] default: 45.0\n");
	printf("[-n initTime] default: 2009-1-00:00:00.0\n");
	printf("[-v Az velocity (deg/s)] default: 0.8\n");
	printf("[-s El speed (deg/s)] default: 0.5\n");
	printf("[-o lon (deg)] default: 0.0\n");
	printf("[-l lat (deg)] default: 0.0\n");
	printf("[-c coordFrame (eq, hor, gal)] default: hor\n");
	printf("[-t startUT(hh:mm:ss.s)] default: 2009-1-00:02:00.0\n");
	printf("[-h help]\n\n\n");
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


int main(int argc, char *argv[])
{

	Antenna::TCoordinateFrame coordFrame;
	double lon,lat;
	double initAz,initEl;
	double azSpeed,elSpeed;
	ACS::Time initTime;
	ACS::Time startUT;
	ACS::TimeInterval slewTime;
	//Antenna::TSections initSector;
	TIMEVALUE now;
	IRA::CSite site;
	bool response=false;
	int out;
	CSlewCheck test;
	
	/* Check Parameters */
	if (argc==1) {
		printf("Missing  arguments.......\n\n");
		printHelp();
		exit(-1);
	}
	
	
	//default values
	coordFrame=Antenna::ANT_HORIZONTAL;
	//initSector=Antenna::ACU_CCW;
	initAz=90;
	initEl=45;
	azSpeed=0.8;
	elSpeed=0.5;
	site.geodeticCoordinates(0.203277043,0.777029309,25);
	parseTime("2010-1-00:00:00.0",initTime);
	parseTime("2010-1-00:02:00.0",startUT);
	
	while ((out=getopt(argc,argv,"a:e:n:v:s:o:l:c:t:"))!=-1) {
		switch (out) {
			case 'a':{
				if (sscanf(optarg,"%lf",&initAz) !=1) {
					printf("couldn't read init azimuth\n\n");
					printHelp();
					exit(-1);
				}
				break;
			}
			case 'e':{
				if (sscanf(optarg,"%lf",&initEl) !=1) {
					printf("couldn't read init elevation\n\n");
					printHelp();
					exit(-1);
				} 
				break;
			}
			case 'n': {
				if (!parseTime(optarg,initTime)) {
					printf("wrong date or time format\n\n");
					printHelp();
					exit(-1);					
				}
				break;
			}
			case 'v': {
				if (sscanf(optarg,"%lf",&azSpeed) !=1) {
				printf("couldn't read az Speed\n\n");
				printHelp();
				exit(-1);
				}
				break;
			}
			case 's': {
				if (sscanf(optarg,"%lf",&elSpeed) !=1) {
					printf("couldn't read el Speed\n\n");
					printHelp();
					exit(-1);
					}
					break;
			}
			case 'o': {
				if (sscanf(optarg,"%lf",&lon) !=1) {
					printf("couldn't read longitude\n\n");
					printHelp();
					exit(-1);
					}
					break;
			}			
			case 'l': {
				if (sscanf(optarg,"%lf",&lat) !=1) {
					printf("couldn't read latitude\n\n");
					printHelp();
					exit(-1);
					};
					break;
			}
			case 'c': {
				if (strcmp(optarg,"gal")==0) coordFrame=Antenna::ANT_GALACTIC;
				else if (strcmp(optarg,"eq")==0) coordFrame=Antenna::ANT_EQUATORIAL;
				else if (strcmp(optarg,"hor")==0) coordFrame=Antenna::ANT_HORIZONTAL;
				else {
					printf("no such coordinate frame \n\n");
					printHelp();
					exit(-1);
				}
				break;
			}			
			case 't': {
				if (!parseTime(optarg,startUT)) {
					printf("wrong date or time format\n\n");
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

	// print parameters......
	printf("\ninitAz: %lf\n",initAz );
	printf("initEl: %lf\n",initEl );
	printf("lon: %lf\n",lon );
	printf("lat: %lf\n",lat );
	printf("initTime: %lf\n",(double)initTime);
	printf("startUT: %lf\n",(double)startUT);
	if (coordFrame==Antenna::ANT_EQUATORIAL) printf ("Frame: equatorial\n");
	else if (coordFrame==Antenna::ANT_GALACTIC) printf ("Frame: galactic\n");
	else if (coordFrame==Antenna::ANT_HORIZONTAL) printf ("Frame: horizontal\n");
	//TIMEVALUE printTime(startUT);

	CIRATools::Wait(1,0);
	
	// coordinates in radians
	lon*=DD2R;
	lat*=DD2R;
	initAz*=DD2R;
	initEl*=DD2R;
	azSpeed*=DD2R;
	elSpeed*=DD2R;
	double azimuth,elevation;
	
	test.initSite(site,0.0);
	test.initMount(azSpeed,elSpeed,3.0,88.0,0.4*DD2R,0.4*DD2R,10*DD2R,80*DD2R);
	TIMEVALUE UT(initTime);
	test.coordConvert(lon,lat,coordFrame,Antenna::ANT_J2000,UT,azimuth,elevation);
	printf("coordinates are (%lf %lf)\n\n\n",(double)azimuth*DR2D,(double)elevation*DR2D);
	response=test.checkSlewing(initAz,initEl,initTime,azimuth,elevation,startUT,slewTime);
	printf("slewingTime (sec.): %lf\n\n\n",((double)slewTime)/10000000.0);
	if (response==true) printf("\nRESPONSE: I CAN REACH IT\n\n\n");
	else printf("\nRESPONSE: I CANNOT REACH IT\n\n\n");

	return 0;	
}
