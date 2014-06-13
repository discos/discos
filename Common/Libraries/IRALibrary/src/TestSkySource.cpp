#include "SkySource.h"
#include "DateTime.h"
#include <cstdlib>
#include <slamac.h>
#include <slalib.h>
#include <getopt.h>

using namespace IRA;

#define RESTFREQ 22235.07985

#define SITELON "11:38:48.97487"
#define SITELAT "44:31:13.78237"
#define SITEHEIGHT 67.155

#define DUT1 0.0

#define NAME "3c123"
#define RA2000 "04:37:04.37"
#define DEC2000 "29:40:15.20"

#define _CP_ if (!silent)

void printHelp()
{
	printf("\n");
	printf("STrackl [-h] [-t  yyyy-doy-hh:mm:ss.sss] [-u dut1] [-s  site_lon,site_lat,site_heigth] [-g  name,ra2000,dec2000]  [-m]");
	printf("\n");
	printf("Allows to compute topocentric coordinates of a source \n");
	printf("\n");
	printf("[-h]\t\t\t\t\tShows this help\n");
	printf("[-t  yyyy-doy-hh:mm:ss.sss]\t\tReference epoch, default is the present time\n");
	printf("[-T mjd]\t\t\t\tReference epoch as modified julian date, default is the present time\n");
	printf("[-u dut1]\t\t\t\tDelta UT-UT1(seconds),default is %lf seconds\n",DUT1);
	printf("[-s  site_lon,site_lat,site_height]\tObserver position(dd:mm:ss.s) and height(m), default is (%s,%s,%lf)\n",SITELON,SITELAT,SITEHEIGHT);
	printf("[-g  name,ra2000,dec2000]\t\tTarget name and coordinates(hh:mm:ss.s,dd:mm:ss.s), default is (%s,%s,%s)\n",NAME,RA2000,DEC2000);
	printf("[-m]  activate silent mode\n");
	printf("\n");
}

int main(int argc, char *argv[])
{
	int out;
	CSite site;
	IRA::CString siteArg="",timeArg="",dirArg="",mjdArg="";
	bool silent=false;
	TIMEVALUE now;
	TIMEDIFFERENCE lst;
	IRA::CString outlst,outnow,outra,outdec;
	double longitude,latitude,height,ra,dec,dut1=DUT1,az,el;
	double raApp,decApp,eph;
	IRA::CString sourceName;
	IRA::CSkySource source;

	while ((out=getopt(argc,argv,"ht:s:g:mu:T:"))!=-1) {
		switch (out) {
			case 'm': {
				silent=true;
				break;
			}
			case 'u': {
				IRA::CString t(optarg);
				dut1=t.ToDouble();
				break;
			}
			case 'h': {
				printHelp();
				exit(0);
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
			case 'T': {
				mjdArg=optarg;
			}
		}
	}

	if (dirArg=="") {
		IRA::CIRATools::hourAngleToRad(RA2000,ra,true);
		IRA::CIRATools::sexagesimalAngleToRad(DEC2000,dec,true);
		sourceName=NAME;
	}
	else {
		int start=0;
		IRA::CString ret;
		if (IRA::CIRATools::getNextToken(dirArg,start,',',ret)) {
			sourceName=ret;
		}
		else {
			printf("Illegal radial velocity format\n");
			printHelp();
			exit(-1);
		}
		if ((IRA::CIRATools::getNextToken(dirArg,start,',',ret)) &&  (IRA::CIRATools::hourAngleToRad(ret,ra,true))) {
		}
		else {
			printf("Illegal right ascension format\n");
			printHelp();
			exit(-1);
		}
		if ((IRA::CIRATools::getNextToken(dirArg,start,',',ret)) && (IRA::CIRATools::sexagesimalAngleToRad(ret,dec,true))) {
		}
		else {
			printf("Illegal declination format\n");
			printHelp();
			exit(-1);
		}
	}

	IRA::CIRATools::radToHourAngle(ra,outra);
	IRA::CIRATools::radToSexagesimalAngle(dec,outdec);

	_CP_ printf("SOURCE:\t\t\tname %s, ra2000 %.8lf° (%s), dec2000 %.8lf° (%s)\n",(const char *)sourceName,ra*DR2D,(const char *)outra,dec*DR2D,(const char *)outdec);

	if (siteArg=="") {
		IRA::CIRATools::sexagesimalAngleToRad(SITELON,longitude,true);
		IRA::CIRATools::sexagesimalAngleToRad(SITELAT,latitude,true);
		height=SITEHEIGHT;
	}
	else {
		int start=0;
		IRA::CString ret;
		if ((IRA::CIRATools::getNextToken(siteArg,start,',',ret)) && (IRA::CIRATools::sexagesimalAngleToRad(ret,longitude,true)))   {
		}
		else {
			printf("Illegal longitude format\n");
			printHelp();
			exit(-1);
		}
		if ((IRA::CIRATools::getNextToken(siteArg,start,',',ret)) && (IRA::CIRATools::sexagesimalAngleToRad(ret,latitude,true))) {
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
	site.geodeticCoordinates(longitude,latitude,height);
	_CP_ printf("TOPOCENTRIC SITE:\tlon %.8lf°, lat %.8lf°, height %lfm\n",longitude*DR2D,latitude*DR2D,height);

	CDateTime reftime(0.0,0.0);

	if ((timeArg=="") && (mjdArg=="")) {
		IRA::CIRATools::getTime(now);
		reftime.setDateTime(now,dut1*1000.0);
	}
	else {
		if (timeArg!="") {
			ACS::Time tt;
			if (IRA::CIRATools::strToTime(timeArg,tt)) {
				now.value(tt);
				reftime.setDateTime(now,dut1*1000.0);
			}
			else {
				printf("Illegal time format\n");
				printHelp();
				exit(-1);
			}
		}
		if (mjdArg!="") {
			long double val=(long double)mjdArg.ToDouble();
			reftime.setMJD(val,dut1*1000.0);
			//printf("%llf\n",reftime.getMJD());
		}
	}
	//CDateTime time(now,dut1*1000.0);
	reftime.LST(site).getDateTime(lst);
	lst.day(0);
	reftime.getDateTime(now);
	IRA::CIRATools::timeToStr(now.value().value,outnow);
	IRA::CIRATools::intervalToStr(lst.value().value,outlst);
	_CP_ printf("REFERENCE TIME:\t\tut %s, lst %s, dut1 %lf\n",(const char *)outnow,(const char *)outlst,dut1);

	source.setInputEquatorial(ra,dec,IRA::CSkySource::SS_J2000);
	source.process(reftime,site);
	source.getApparentHorizontal(az,el);

	source.getApparentEquatorial(raApp,decApp,eph);
	IRA::CIRATools::radToHourAngle(raApp,outra);
	IRA::CIRATools::radToSexagesimalAngle(decApp,outdec);


	_CP_ printf("Equatorial coordinates:\tra %.8lf° (%s), dec %.8lf° (%s), epoch %.8lf\n",raApp*DR2D,(const char *)outra,decApp*DR2D,(const char *)outdec,eph);
	_CP_ printf("Horizontal coordinates:\tazimuth %.8lf°, elevation %.8lf°\n",az*DR2D,el*DR2D);


	double obsZenithDistance=(DPI/2.0)-el;
	double corZenithDistance;
	double tdk = 13 + 273.0; // temperature
	double wl = 36000.0;
	double tlr = 0.0065;
	double eps = 1e-8;

	slaRefro(obsZenithDistance, height, tdk, 1090,1, wl, site.getLatitude(), tlr, eps, &corZenithDistance);

	_CP_ printf("refraction at (20°,1020mbar,0.5RH,8GHz) is %.8lf°\n",corZenithDistance*DR2D);

	_CP_ printf("\n\n");

	if (silent) printf("%.8lf, %.8lf\n",az*DR2D,el*DR2D);

	return 0;
}
