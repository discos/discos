#include "SkySource.h"
#include "DateTime.h"
#include <cstdlib>
#include <slamac.h>
#include <slalib.h>
#include <getopt.h>

#define SRT_LAT 39.49304
#define SRT_LON 9.24516
#define SRT_H 650.0
	
#define MC_LAT 44.52048889
#define MC_LON 11.64693056
#define MC_H 28.0
	
#define NT_LAT 36.87585
#define NT_LON 14.9889
#define NT_H 32.0

void printHelp()
{
	printf("\n");
	printf("lst2UT [-h] -t yyyy-mm-dd -l  hh:mm:ss.ss [-s  site]");
	printf("\n");
	printf("Allows to convert Local Sidereal Time into Universal Time \n");
	printf("\n");
	printf("[-h]\t\t\tShows this help\n");
	printf("[-t  yyyy-mm-dd]\tReference day, value is mandatory\n");
	printf("[-s  site]\t\tAllows to select the station: srt,mc,nt, defualt is srt\n");
	printf("[-l  hh:mm:ss.ss]\tLocal Sidereal Time to be converted, value is mandatory\n");
	printf("\n");
}


bool parseLST(const IRA::CString& val,ACS::TimeInterval& lst)
{
	long hh,mm,ss,dec;
	if (sscanf((const char *)val,"%ld:%ld:%ld.%ld",&hh,&mm,&ss,&dec)!=4) {
		return false;
	}
	else {
		if (hh<0 || hh>23) return false;
		if (mm<0 || mm>59) return false;
		if (ss<0 || ss>59) return false;
		TIMEDIFFERENCE tt;
		tt.reset();
		tt.normalize(true);
		tt.hour(hh);
		tt.minute(mm);
		tt.second(ss);
		tt.microSecond((unsigned long)(dec*1000));
		lst=tt.value().value;
		return true;
	}
}

bool parseDay(const IRA::CString& val,ACS::Time& ut)
{
	unsigned yy,mm,dd;
	if (sscanf((const char *)val,"%u-%u-%u",&yy,&mm,&dd)!=3) {
		return false;
	}
	else {
		if (dd<0 || dd>31) return false;
		if (mm<0 || mm>12) return false;
		TIMEVALUE tt;
		tt.reset();
		tt.normalize(true);
		tt.year(yy);
		tt.month(mm);
		tt.day(dd);
		ut=tt.value().value;
		return true;
	}
}

int main(int argc, char *argv[]) 
{
	int out;
	IRA::CString day("");
	IRA::CString station("srt");
	IRA::CString lst("");
	ACS::TimeInterval lsTime;
	ACS::Time utDay;
	TIMEDIFFERENCE myLst;
	TIMEVALUE myUt;
	IRA::CDateTime ut,outUt1,outUt2;
	double lon,lat,height,lstRad;
	IRA::CSite site;
	while ((out=getopt(argc,argv,"ht:s:l:"))!=-1) {
		switch (out) {
			case 't': {
				day=optarg;
				break;
			}
			case 'h': {
				printHelp();
				exit(0);
				break;
			}
			case 's': {
				station=optarg;
				break;
			}
			case 'l': {
				lst=optarg;
				break;
			}
		}
	}
	if (lst=="") {
		printf("no LST to convert provided!\n");
		printHelp();
		exit(-1);
	}
	if (day=="") {
		printf("no reference day provided!\n");
		printHelp();
		exit(-1);
	}
	if (!parseLST(lst,lsTime)) {
		printf("bad LST format!\n");
		printHelp();
		exit(-1);
	}
	if (!parseDay(day,utDay)) {
		printf("bad day format!\n");
		printHelp();
		exit(-1);
	}
	if ((station!="srt") && (station!="mc") & (station!="nt")) {
		printf("station id not known!\n");
		printHelp();
		exit(-1);
	}
	myLst.value(lsTime);
	myUt.value(utDay);
	if (station=="srt") {
		lon=SRT_LON;
		lat=SRT_LAT;
		height=SRT_H;
	}
	else if (station=="mc") {
		lon=MC_LON;
		lat=MC_LAT;
		height=MC_H;
		
	} else {
		lon=NT_LON;
		lat=NT_LAT;
		height=NT_H;		
	}
	printf("Input LST: %02d:%02d:%02d.%03d\n",myLst.hour(),myLst.minute(),myLst.second(),myLst.microSecond()/1000);
	printf("Input Day: %04d-%02d-%02d\n",myUt.year(),myUt.month(),myUt.day());
	printf("Station: %s, lon %lf, lat %lf, height %lf\n",(const char *)station,lon,lat,height);
	printf("\n");
		
	
	site.geodeticCoordinates(lon*DD2R,lat*DD2R,height);
	ut.setDateTime(myUt);	
	lstRad=(myLst.hour()*3600.0+myLst.minute()*60.0+myLst.second()+myLst.microSecond()/1000000.0)*DS2R;	
	
	printf("LST: %lf rad.\n",lstRad);
	
	if (IRA::CDateTime::sidereal2UT(outUt1,outUt2,lstRad,site,myUt.year(),myUt.month(),myUt.day(),0.0)) {
		printf("UT: %04d-%02d-%02d-%02d:%02d:%02d\n",
			outUt1.getYear(),
			outUt1.getMonth(),
			outUt1.getDay(),
			outUt1.getHours(),
			outUt1.getMinutes(),
			outUt1.getSeconds());
	}
	else {
 		printf("UT1: %04d-%02d-%02d-%02d:%02d:%02d\n",
			outUt1.getYear(),
			outUt1.getMonth(),
			outUt1.getDay(),
			outUt1.getHours(),
			outUt1.getMinutes(),
			outUt1.getSeconds());
		printf("UT2: %04d-%02d-%02d-%02d:%02d:%02d\n",
			outUt2.getYear(),
			outUt2.getMonth(),
			outUt2.getDay(),
			outUt2.getHours(),
			outUt2.getMinutes(),
			outUt2.getSeconds());	
	}
	
	/*ACS::TimeInterval lstTime;
	ACS::Time utTime,result;
	IRA::CSite site;
	IRA::CDateTime ut;
	double lst;
	TIMEVALUE myUt;
	TIMEDIFFERENCE myLst;
	if (argc<3) {
		printf ("missing argument\n");
		return 0;
	}
	parseUT(IRA::CString(argv[1]),utTime);
	parseLST(IRA::CString(argv[2]),lstTime);
	
	myUt.value(utTime);
	myLst.value(lstTime);
	printf("Input UT: %d-%03d-%02d:%02d:%02d.%03d\n",myUt.year(),myUt.dayOfYear(),myUt.hour(),myUt.minute(),
			myUt.second(),myUt.microSecond()/1000);
	printf("Input LST: %02d:%02d:%02d.%03d\n",myLst.hour(),myLst.minute(),
			myLst.second(),myLst.microSecond()/1000);
	
	// Medicina coordinates
	site.geodeticCoordinates(11.64693*DD2R,44.52049*DD2R,28.0);
	
	ut.setDateTime(myUt);
	lst=(myLst.hour()*3600.0+myLst.minute()*60.0+myLst.second()+myLst.microSecond()/1000000.0)*DS2R;
	printf("LST in rad: %lf\n",lst);
	
	result=getUTFromLST(ut,ut,lstTime,site,0.0);
	
	TIMEVALUE out;
	out.value(result);
	printf("result UT: %d-%03d-%02d:%02d:%02d.%03d\n\n\n",out.year(),out.dayOfYear(),out.hour(),out.minute(),
			out.second(),out.microSecond()/1000);
	
	
	//printf("ut: %llu, lst: %lld\n",ut,lst);
	
	return 0;*/
}