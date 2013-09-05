#include "SkySource.h"
#include "DateTime.h"
#include <cstdlib>
#include <slamac.h>
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
	printf("[-u dut1]\t\t\t\tDelta UT-UT1(seconds),default is %lf seconds\n",DUT1);
	printf("[-s  site_lon,site_lat,site_height]\tObserver position(dd:mm:ss.s) and height(m), default is (%s,%s,%lf)\n",SITELON,SITELAT,SITEHEIGHT);
	printf("[-g  name,ra2000,dec2000]\t\tTarget name and coordinates(hh:mm:ss.s,dd:mm:ss.s), default is (%s,%s,%s)\n",NAME,RA2000,DEC2000);
	printf("\n");
}

int main(int argc, char *argv[])
{
	int out;
	CSite site;
	IRA::CString siteArg="",timeArg="",dirArg="";
	bool silent=false;
	TIMEVALUE now;
	TIMEDIFFERENCE lst;
	IRA::CString outlst,outnow,outra,outdec;
	double longitude,latitude,height,ra,dec,dut1=DUT1,az,el;
	IRA::CString sourceName;
	IRA::CSkySource source;

	while ((out=getopt(argc,argv,"ht:s:g:mu:"))!=-1) {
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
		if ((IRA::CIRATools::getNextToken(dirArg,start,',',ret)) &&  (IRA::CIRATools::hourAngleToRad(ret,ra,true))){
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

	_CP_ printf("SOURCE:\t\t\tname %s, ra2000 %.8lf°, dec2000 %.8lf°\n",(const char *)sourceName,ra*DR2D,dec*DR2D);

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
	CDateTime time(now,dut1*1000.0);
	time.LST(site).getDateTime(lst);
	lst.day(0);
	IRA::CIRATools::timeToStr(now.value().value,outnow);
	IRA::CIRATools::intervalToStr(lst.value().value,outlst);
	_CP_ printf("REFERENCE TIME:\t\tut %s, lst %s, dut1 %lf\n",(const char *)outnow,(const char *)outlst,dut1);

	source.setInputEquatorial(ra,dec,IRA::CSkySource::SS_J2000);
	source.process(time,site);
	source.getApparentHorizontal(az,el);

	_CP_ printf("Horizontal coordinates:\tazimuth %.8lf°, elevation %.8lf°\n",az*DR2D,el*DR2D);
	_CP_ printf("\n\n");

	if (silent) printf("%.8lf, %.8lf\n",az*DR2D,el*DR2D);

	return 0;
}
