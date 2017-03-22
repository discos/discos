#include "FrequencyTracking.h"
#include "DateTime.h"
#include <cstdlib>
#include <slamac.h>
#include <slalib.h>
#include <getopt.h>

using namespace IRA;

#define RESTFREQ 22235.07985

#define SITELON 11.64693056
#define SITELAT 44.52048889
#define SITEHEIGHT 28.0

#define RA2000 36.76954167
#define DEC2000 61.87366667
#define VRAD -48.8

#define _CP_ if (!silent)

void printHelp()
{
	printf("\n");
	printf("FTrack [-h] [-t  yyyy-doy-hh:mm:ss.sss] [-s  site_lon,site_lat,site_heigth] [-g  ra2000,dec2000,vrad] [-f frame] [-d definition] [-r restfrequency] [-m]");
	printf("\n");
	printf("Allows to compute topocentric frequency\n");
	printf("\n");
	printf("[-h]\t\t\t\t\tShows this help\n");
	printf("[-t  yyyy-doy-hh:mm:ss.sss]\t\tReference epoch, default is the present time\n");
	printf("[-s  site_lon,site_lat,site_height]\tObserver position(degrees) and height(m), default is (%lf,%lf,%lf)\n",SITELON,SITELAT,SITEHEIGHT);
	printf("[-g  ra2000,dec2000,vrad]\t\tTarget coordinates(degrees) and radial velocity(km/sec) or redshift, default is (%lf,%lf,%lf km/sec)\n",RA2000,DEC2000,VRAD);
	printf("[-f frame]\t\t\t\tReference frame of the radial velocity: TOPOCEN,BARY,LSRK,LSRD,GALCEN,LGROUP, default is LSRK\n");
	printf("[-d definition]\t\t\t\tVelocity definition: RADIO,OPTICAL,REDSHIFT, default is RADIO\n");
	printf("[-r restfrequency]\t\t\trest frequency(MHz), default is %lf\n",RESTFREQ);
	printf("[-m]\t\t\tturn silent mode on\n");
	printf("\n");
}


int main(int argc, char *argv[])
{
	CFrequencyTracking::TCartesian baryvel,rotvel,framevel,obsvel;
	CFrequencyTracking::TFrame frame=CFrequencyTracking::LSRK;
	CFrequencyTracking::TDefinition def=CFrequencyTracking::RADIO;
	CSite site;
	TIMEVALUE now;
	TIMEDIFFERENCE lst;
	double longitude,latitude,height,ra,dec,vrad,rest=RESTFREQ;
	IRA::CString timeArg="",siteArg="",dirArg="";
	IRA::CString outlst,outnow,outra,outdec,outframe,outdef;
	bool printfFrameVel;
	bool silent=false;
	int out;

	while ((out=getopt(argc,argv,"ht:s:g:f:d:r:m"))!=-1) {
		switch (out) {
			case 'm': {
				silent=true;
				break;
			}
			case 'h': {
				printHelp();
				exit(0);
				break;
			}
			case 'f': {
				IRA::CString cmp(optarg);
				if (!CFrequencyTracking::strToFrame(cmp,frame)) {
					printf("Illegal frame\n");
					printHelp();
					exit(-1);
				}
				break;
			}
			case 'r': {
				IRA::CString ff(optarg);
				rest=ff.ToDouble();
				break;
			}
			case 'd': {
				IRA::CString cmp(optarg);
				if (!CFrequencyTracking::strToDefinition(cmp,def)) {
					printf("Illegal definition\n");
					printHelp();
					exit(-1);
				}
				break;
			}
			case 's': {
				siteArg=optarg;
				break;
			}
			case 't': {
				timeArg=optarg;
				break;
			}
			case 'g': {
				dirArg=optarg;
				break;
			}
		}
	}

	if (timeArg=="") {
		IRA::CIRATools::getTime(now);
	}
	else {
		ACS::Time tt;
		if (IRA::CIRATools::strToTime(timeArg,tt)) {
			now.value(tt);
		}
		else {
			printf("Illegal time format\n");
			printHelp();
			exit(-1);
		}
	}
	if (dirArg=="") {
		ra=RA2000;
		dec=DEC2000;
		vrad=VRAD;
	}
	else {
		int start=0;
		IRA::CString ret;
		if (IRA::CIRATools::getNextToken(dirArg,start,',',ret)) {
			ra=ret.ToDouble();
		}
		else {
			printf("Illegal right ascension format\n");
			printHelp();
			exit(-1);
		}
		if (IRA::CIRATools::getNextToken(dirArg,start,',',ret)) {
			dec=ret.ToDouble();
		}
		else {
			printf("Illegal declination format\n");
			printHelp();
			exit(-1);
		}
		if (IRA::CIRATools::getNextToken(dirArg,start,',',ret)) {
			vrad=ret.ToDouble();
		}
		else {
			printf("Illegal radial velocity format\n");
			printHelp();
			exit(-1);
		}
	}
	if (siteArg=="") {
		longitude=SITELON;
		latitude=SITELAT;
		height=SITEHEIGHT;
	}
	else {
		int start=0;
		IRA::CString ret;
		if (IRA::CIRATools::getNextToken(siteArg,start,',',ret)) {
			longitude=ret.ToDouble();
		}
		else {
			printf("Illegal longitude format\n");
			printHelp();
			exit(-1);
		}
		if (IRA::CIRATools::getNextToken(siteArg,start,',',ret)) {
			latitude=ret.ToDouble();
		}
		else {
			printf("Illegal latitude format\n");
			printHelp();
			exit(-1);
		}
		if (IRA::CIRATools::getNextToken(siteArg,start,',',ret)) {
			height=ret.ToDouble();
		}
		else {
			printf("Illegal height format\n");
			printHelp();
			exit(-1);
		}
	}

	site.geodeticCoordinates(longitude*DD2R,latitude*DD2R,height);
	_CP_ printf("TOPOCENTRIC SITE: lon %lf, lat %lf, height %lf\n",longitude,latitude,height);

	CDateTime time(now,0.0);
	time.LST(site).getDateTime(lst);
	lst.day(0);
	IRA::CIRATools::timeToStr(now.value().value,outnow);
	IRA::CIRATools::intervalToStr(lst.value().value,outlst);
	_CP_ printf("REFERENCE TIME:\t ut %s, lst %s\n",(const char *)outnow,(const char *)outlst);

	IRA::CIRATools::radToHourAngle(ra*DD2R,outra);
	IRA::CIRATools::radToSexagesimalAngle(dec*DD2R,outdec);
	CFrequencyTracking::frameToStr(frame,outframe);
	CFrequencyTracking::definitionToStr(def,outdef);
	if (def!=CFrequencyTracking::REDSHIFT) {
		_CP_ printf("SOURCE:\t\t ra2000 %s, dec2000 %s, Vrad %lf km/sec, %s, %s\n",(const char *)outra,(const char *)outdec,vrad,(const char *)outframe,(const char *)outdef);
	}
	else {
		_CP_ printf("SOURCE:\t\t ra2000 %s, dec2000 %s, z %lf, %s\n",(const char *)outra,(const char *)outdec,vrad,(const char *)outframe);
	}
	_CP_ printf("REST FREQUENCY:\t %lf MHz\n",rest);

	_CP_ printf("\n\n");

	CFrequencyTracking track(site,ra*DD2R,dec*DD2R,frame,def,rest,vrad);

	if (frame!=CFrequencyTracking::TOPOCEN) {
		baryvel=track.getBarycentricVelocity(time);
		_CP_ printf("Velocity of the barycenter wrt geocenter:\t module %lf km/sec, toward source direction %lf km/sec, x %lf km/sec, y %lf km/sec, z %lf km/sec\n",baryvel.m,baryvel.toSource,baryvel.x,baryvel.y,baryvel.z);
		rotvel=track.getEarthRotationVelocity(time);
		_CP_ printf("Earth rotation velocity:\t\t\t module %lf km/sec, toward source direction %lf km/sec,  x %lf km/sec, y %lf km/sec, z %lf km/sec\n",rotvel.m,rotvel.toSource,rotvel.x,rotvel.y,rotvel.z);
	}

	printfFrameVel=true;
	if (frame==CFrequencyTracking::LSRK) {
		framevel=track.getLSRKVelocity(time);
	}
	else if( frame==CFrequencyTracking::TOPOCEN) {
		printfFrameVel=false;
	}
	else if (frame==CFrequencyTracking::LSRD) {
		framevel=track.getLSRDVelocity(time);
	}
	else if (frame==CFrequencyTracking::BARY) {
		printfFrameVel=false;
	}
	else if (frame==CFrequencyTracking::GALCEN) {
		framevel=track.getGALCENVelocity(time);
	}
	else if (frame==CFrequencyTracking::LGROUP) {
		framevel=track.getLGROUPVelocity(time);
	}

	if (printfFrameVel) {
		_CP_ printf("Velocity of the %s wrt barycenter:\t module %lf km/sec, toward source direction %lf km/sec, x %lf km/sec, y %lf km/sec, z %lf km/sec\n",(const char *)outframe,framevel.m,framevel.toSource,framevel.x,framevel.y,framevel.z);
	}

	obsvel=track.getObserverVelocity(time);
	_CP_ printf("Resulting observer velocity:\t\t\t module %lf km/sec, toward source direction %lf km/sec, x %lf km/sec, y %lf km/sec, z %lf km/sec\n",obsvel.m,obsvel.toSource,obsvel.x,obsvel.y,obsvel.z);

	/*if (frame!=CFrequencyTracking::BARY) {*/
		_CP_ printf("Frequency @(%s):\t\t\t (%s) %lf MHz\n",(const char *)outframe,(const char *)outdef,track.getFrequencyAtFrame());
	/*}
	else {
		_CP_ printf("Frequency @(%s):\t\t\t %lf MHz\n",(const char *)outframe,track.getFrequencyAtFrame());
	}*/

	double ftop=track.getTopocentricFrequency(time);

	_CP_ printf("Topocentric frequency:\t\t\t\t %lf MHz\n",ftop);

	_CP_ printf("\n\n");

	//track.test(time);

	if (silent) printf("%lf\n",ftop);

	return 0;

}


