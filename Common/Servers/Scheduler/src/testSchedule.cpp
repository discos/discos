// $Id: testSchedule.cpp,v 1.12 2011-06-21 16:39:52 a.orlati Exp $

#include "Schedule.h"
#include <slamac.h>
#include <AntennaModule.h>
#include <IRA>

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

IRA::CString toType(const Management::TScanTypes& tp)
{
	if (tp==Management::MNG_SIDEREAL) return IRA::CString("SIDEREAL");     
	else if (tp==Management::MNG_SUN)  return IRA::CString("SUN");   
	else if (tp==Management::MNG_MOON) return IRA::CString("MOON");
	else if (tp==Management::MNG_SATELLITE) return IRA::CString("SATELLITE");
	else if (tp==Management::MNG_SOLARSYTEMBODY) return IRA::CString("SOLARSYSTEMBODY");
	else if (tp==Management::MNG_OTF) return IRA::CString("OTF");
	else if (tp==Management::MNG_OTFC) return IRA::CString("OTFC");
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
	//DWORD scan;
	double minEl,maxEl;
	IRA::CString pre,post,target,outString,outString2;
	std::vector<IRA::CString> procedure;
	Management::TScanTypes type;
	void *scanPar;
	void *secPar;
	void *servoPar;
	void *recvPar;
	Management::TSubScanConfiguration *subScanConf;
	CSchedule::TRecord rec;
	ACS::stringSeq layoutDef;
	CSchedule sched("../templates/","schedule.tmpl");
	//CSchedule sched("/archive/schedules/maintenance/","mbfitstest-CrossEQEQ.scd");
	//CSchedule sched("/archive/schedules/","calibrationSeq.scd");
	
	if (!sched.readAll(true)) {
		printf("ERROR: %s\n",(const char *)sched.getLastError());
		return -1;
	}
	if (sched.isComplete()) {
		printf("Main schedule done\n");
		printf("Total lines: %u\n",sched.getSubScansNumber());
	}
	else {
		printf("Main schedule is uncomplete\n");
		return -1;
	}

	/*WORD pospos=0;
	IRA::CString namename;
	ACS::stringSeq procproc;
	WORD argsargs;
	while(sched.getPostScanProcedureList()->getProcedure(pospos,namename,procproc,argsargs)) {
		//m_parser->addExtraProcedure(name,loader->getFileName(),proc,args);
		printf("%s, %d, %d\n",(const char *)namename,argsargs,procproc.length());
		pospos++;
	}*/

	sched.getElevationLimits(minEl,maxEl);
	printf("elevation limits are %lf,%lf\n",minEl,maxEl);

	for (DWORD i=0;i<sched.getSubScansNumber();i++) {
		if (!sched.getSubScan(i,rec)) {
			printf("Schedule subscan %u is not present\n",i);
			return -1;
		}
		else {
			IRA::CIRATools::intervalToStr(rec.lst,outString);
			printf("%u %u %u %s %lf %u %s %s %s %s %s %s\n",rec.counter,rec.scanid,rec.subscanid,(const char *)outString,rec.duration,rec.scan,(const char *)rec.preScan,(const char *)rec.postScan,(const char *)rec.backendProc,
					(const char *)rec.writerInstance,(const char *)rec.suffix,(const char *)rec.layout);
			if (!sched.getScanList()->getScan(rec.scan,type,scanPar,secPar,servoPar,recvPar,subScanConf)) {
				printf("Scan number %u is not present\n",rec.scan);
				return -1;
			}
			else {
				Antenna::TTrackingParameters *tmp=(Antenna::TTrackingParameters *)scanPar;
				if (type==Management::MNG_OTF) {					
					printf("%u OTF %s %lf %lf %lf %lf %s %s %s %s %s %lld\n",rec.scan,(const char *)tmp->targetName,tmp->otf.lon1*DR2D,tmp->otf.lat1*DR2D,tmp->otf.lon2*DR2D,
							tmp->otf.lat2*DR2D,(const char *)toFrame(tmp->otf.coordFrame),
							(const char *)toFrame(tmp->otf.subScanFrame),(const char *)toGeometry(tmp->otf.geometry),
							(const char *)toDescription(tmp->otf.description),
							(const char *)toDirection(tmp->otf.direction),(long long)tmp->otf.subScanDuration);
				}
				else if (type==Management::MNG_OTFC) {
					printf("%u OTFC %lf %lf %lf %lf %s %s %s %s %s %lld\n",rec.scan,tmp->otf.lon1*DR2D,tmp->otf.lat1*DR2D,tmp->otf.lon2*DR2D,
							tmp->otf.lat2*DR2D,(const char *)toFrame(tmp->otf.coordFrame),
							(const char *)toFrame(tmp->otf.subScanFrame),(const char *)toGeometry(tmp->otf.geometry),
							(const char *)toDescription(tmp->otf.description),
							(const char *)toDirection(tmp->otf.direction),(long long)tmp->otf.subScanDuration);
				}
				else if (type==Management::MNG_SKYDIP) {
					printf("%u SKYDIP %lf %lf %lf %lf %s %s %s %s %s %lld\n",rec.scan,tmp->otf.lon1*DR2D,tmp->otf.lat1*DR2D,tmp->otf.lon2*DR2D,
							tmp->otf.lat2*DR2D,(const char *)toFrame(tmp->otf.coordFrame),
							(const char *)toFrame(tmp->otf.subScanFrame),(const char *)toGeometry(tmp->otf.geometry),
							(const char *)toDescription(tmp->otf.description),
							(const char *)toDirection(tmp->otf.direction),(long long)tmp->otf.subScanDuration);
				}
				else {
					if (tmp->paramNumber==0) {
						printf("%u %s %s \n",rec.scan,(const char *)toType(type),(const char *)tmp->targetName);
					}
					else {
						if (tmp->frame==Antenna::ANT_EQUATORIAL) {
							printf("%u %s %s %s %lf %lf %s ",rec.scan,(const char *)toType(type),(const char *)tmp->targetName,(const char *)toFrame(tmp->frame)
								,tmp->parameters[0]*DR2D,tmp->parameters[1]*DR2D,(const char *)toEquinox(tmp->equinox));
							for (int j=2;j<tmp->paramNumber;j++) {
								printf("%lf ",tmp->parameters[j]);
							}
							printf("\n");
						}
						else {
							printf("%u %s %s %s %lf %lf \n",rec.scan,(const char *)toType(type),(const char *)tmp->targetName,(const char *)toFrame(tmp->frame),
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
				printf("\n");
				printf("radial velocity: %lf, %s, %s\n",tmp->RadialVelocity,Antenna::Definitions::map(tmp->VradFrame),Antenna::Definitions::map(tmp->VradDefinition));
			}
			printf("\n");
			if (!sched.getBackendList()->getBackend(rec.backendProc,target,procedure)) {
				printf("backend  %s is not present\n",(const char *)rec.backendProc);
				return -1;
			}
			else {
				printf("backend: %s\n",(const char *)target);
				std::vector<IRA::CString>::iterator k;
				for (k=procedure.begin();k<procedure.end();k++) {
					printf("\t%s",(const char *)(*k));
				}
				printf("\n");
			}
			if (sched.getLayoutList()) {
				printf("layout name: %s\n",(const char *)rec.layout);
				if (!sched.getLayoutList()->getScanLayout(rec.layout,layoutDef)) {
					printf("layout  %s is not present\n",(const char *)rec.layout);
					return -1;
				}
				else {
					for (DWORD k=0;k<layoutDef.length();k++) {
						printf("\t%s",(const char *)layoutDef[k]);
					}
					printf("\n");
				}
			}
			printf("\n");
		}
	}
	return 0;
}
