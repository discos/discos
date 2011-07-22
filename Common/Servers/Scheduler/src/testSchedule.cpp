// $Id: testSchedule.cpp,v 1.10 2010-09-14 10:25:17 a.orlati Exp $

#include "Schedule.h"
#include <slamac.h>

// this is a quick test for the class CSchedule, in order to test if it works and no memory leakage are behind the corner

IRA::CString toFrame(const Antenna::TCoordinateFrame& frame)
{
	if (frame==Antenna::ANT_EQUATORIAL) return IRA::CString("EQ");
	else if (frame==Antenna::ANT_HORIZONTAL) return IRA::CString("HOR");
	else  return IRA::CString("GAL");
}

IRA::CString toGeometry(const Antenna::TsubScanGeometry& geo)
{
	if (geo==Antenna::SUBSCAN_CONSTLON) return IRA::CString("LON");
	else if (geo==Antenna::SUBSCAN_CONSTLAT) return IRA::CString("LAT");
	else return IRA::CString("GC");
}

IRA::CString toDirection(const Antenna::TsubScanDirection& dir)
{
	if (dir==Antenna::SUBSCAN_INCREASE) return IRA::CString("INC");
	else return IRA::CString("DEC"); 
}

IRA::CString toDescription(const Antenna::TsubScanDescription& des)
{
	if (des==Antenna::SUBSCAN_STARTSTOP) return IRA::CString("SS");
	else return IRA::CString("CEN"); 
}

IRA::CString toType(const Antenna::TGeneratorType& tp)
{
	if (tp==Antenna::ANT_SIDEREAL) return IRA::CString("SIDEREAL");     
	else if (tp==Antenna::ANT_SUN)  return IRA::CString("SUN");   
	else if (tp==Antenna::ANT_MOON) return IRA::CString("MOON");
	else if (tp==Antenna::ANT_SATELLITE) return IRA::CString("SATELLITE");
	else if (tp==Antenna::ANT_SOLARSYTEMBODY) return IRA::CString("SOLARSYSTEMBODY");
	else if (tp==Antenna::ANT_OTF) return IRA::CString("OTF");
	else return IRA::CString("");
}

IRA::CString toEquinox(const Antenna::TSystemEquinox& eq)
{
	if (eq==Antenna::ANT_B1950) return IRA::CString("1950.0");
	else if (eq==Antenna::ANT_J2000) return IRA::CString("2000.0");
	else return IRA::CString("-1");
}

int main(int argc, char *argv[]) 
{
	unsigned long long time;
	double duration;
	DWORD scan;
	IRA::CString pre,post,bck,target,suffix,wrt;
	bool preBlocking,postBlocking;
	std::vector<IRA::CString> procedure;
	Antenna::TGeneratorType type;
	void *scanPar;
	CSchedule sched("../templates/","schedule.tmpl");
	
	if (!sched.readAll(true)) {
		printf("ERROR: %s\n",(const char *)sched.getLastError());
		return -1;
	}
	if (sched.isComplete()) {
		printf("Main schedule done\n");
		printf("Total lines: %u\n",sched.getSchedLines());
	}
	else {
		printf("Main schedule is uncomplete\n");
		return -1;
	}
	for (DWORD i=1;i<=sched.getSchedLines();i++) {
		if (!sched.getLine(i,time,duration,scan,pre,preBlocking,post,postBlocking,bck,wrt,suffix)) {
			printf("Schedule line %u is not present\n",i);
			return -1;
		}
		else {
			printf("%u %llu %lf %u %s %s %s %s %s\n",i,time,duration,scan,(const char *)pre,(const char *)post,(const char *)bck,(const char *)wrt,
					(const char *)suffix);
			if (!sched.getScanList()->getScan(scan,type,scanPar,target)) {
				printf("Scan number %u is not present\n",scan);
				return -1;
			}
			else {
				Antenna::TTrackingParameters *tmp=(Antenna::TTrackingParameters *)scanPar;
				if (type==Antenna::ANT_OTF) {					
					printf("%u OTF %lf %lf %lf %lf %s %s %s %s %s %lld\n",scan,tmp->otf.lon1*DR2D,tmp->otf.lat1*DR2D,tmp->otf.lon2*DR2D,
							tmp->otf.lat2*DR2D,(const char *)toFrame(tmp->otf.coordFrame),
							(const char *)toFrame(tmp->otf.subScanFrame),(const char *)toGeometry(tmp->otf.geometry),
							(const char *)toDescription(tmp->otf.description),
							(const char *)toDirection(tmp->otf.direction),tmp->otf.subScanDuration);
				}
				else {
					if (tmp->paramNumber==0) {
						printf("%u %s %s \n",scan,(const char *)toType(type),(const char *)target);
					}
					else {
						if (tmp->frame==Antenna::ANT_EQUATORIAL) {
							printf("%u %s %s %s %lf %lf %s ",scan,(const char *)toType(type),(const char *)target,(const char *)toFrame(tmp->frame)
								,tmp->parameters[0]*DR2D,tmp->parameters[1]*DR2D,(const char *)toEquinox(tmp->equinox));
							for (int j=2;j<tmp->paramNumber;j++) {
								printf("%lf ",tmp->parameters[j]);
							}
							printf("\n");
						}
						else {
							printf("%u %s %s %s %lf %lf \n",scan,(const char *)toType(type),(const char *)target,(const char *)toFrame(tmp->frame),
								tmp->parameters[0]*DR2D,tmp->parameters[1]*DR2D);							
						}
					}
				}
				if ((tmp->longitudeOffset!=0.0) || (tmp->latitudeOffset!=0.0)) {
					printf("Offsets: %s %lf %lf\n",(const char *)toFrame(tmp->offsetFrame),tmp->longitudeOffset*DR2D,tmp->latitudeOffset*DR2D);
				}
				else {
					printf("No offsets\n");
				}
			}
			printf("\n");
			if (!sched.getBackendList()->getBackend(bck,target,procedure)) {
				printf("bakcend  %s is not present\n",(const char *)bck);
				return -1;
			}
			else {
				printf("backend: %s\n",(const char *)target);
				std::vector<IRA::CString>::iterator k;
				for (k=procedure.begin();k<procedure.end();k++) {
					printf("\t%s",(const char *)(*k));
				}
			}
			printf("\n");
		}
	}
	return 0;
}
