//$ Id: $

#include <String.h>
#include <Site.h>
#include <DateTime.h>
#include <slamac.h>

using namespace IRA;

ACS::Time getUTFromLST(const IRA::CDateTime& currentUT,const IRA::CDateTime& checkUT,const ACS::TimeInterval& lst,const IRA::CSite& site,const double& dut1)
{
	TIMEVALUE time;
	CDateTime ut1,ut2;
	TIMEDIFFERENCE myLst;
	double lstRad;
	myLst.value(lst);
	lstRad=(myLst.hour()*3600.0+myLst.minute()*60.0+myLst.second()+myLst.microSecond()/1000000.0)*DS2R;
	if (IRA::CDateTime::sidereal2UT(ut1,ut2,lstRad,site,currentUT.getYear(),currentUT.getMonth(),currentUT.getDay(),dut1)) {
		// one corresponding ut
		if (ut1.getJD()<checkUT.getJD()) { //if the ut has already elapsed
			// add one day 
			IRA::CDateTime dayMore(currentUT.getJD()+1.0);
			return getUTFromLST(dayMore,checkUT,lst,site,dut1);
		}
		else {
			ut1.getDateTime(time);
			return time.value().value;
		}
	}
	else {
		// two corresponding ut;
		double jd1,jd2,jdNow;
		
		jd1=ut1.getJD(); jd2=ut2.getJD(); jdNow=checkUT.getJD();
		if ((jdNow>jd1) && (jdNow>jd2)) {
			IRA::CDateTime dayMore(currentUT.getJD()+1.0);
			return getUTFromLST(dayMore,checkUT,lst,site,dut1);			
		}
		else if ((jdNow<jd1) && (jdNow<jd2)) {
			if (jd1==GETMIN(jd1,jd2)) {
				ut1.getDateTime(time);
				return time.value().value;
			}
			else {
				ut2.getDateTime(time);
				return time.value().value;
			}
		} 
		else if  (jd2>jd1) {
			ut2.getDateTime(time);
			return time.value().value;
		} 
		else {
			ut1.getDateTime(time);
			return time.value().value;
		}
	}
}

bool parseUT(const IRA::CString& val,ACS::Time& ut)
{
	long hh,mm,ss;
	unsigned yy,doy;
	double sec;
	if (sscanf((const char *)val,"%u-%u-%ld:%ld:%ld.%lf",&yy,&doy,&hh,&mm,&ss,&sec)!=6) {
		return false;
	}
	else {
		if (hh<0 || hh>23) return false;
		if (mm<0 || mm>59) return false;
		if (ss<0 || ss>59) return false;
		if (doy>366) return false;
		TIMEVALUE tt;
		tt.reset();
		tt.normalize(true);
		tt.year(yy);
		tt.dayOfYear(doy);
		tt.hour(hh);
		tt.minute(mm);
		tt.second(ss);
		tt.microSecond((unsigned long)(sec*1000));
		ut=tt.value().value;
		return true;
	}
}

bool parseLST(const IRA::CString& val,ACS::TimeInterval& lst)
{
	long hh,mm,ss;
	double sec;
	if (sscanf((const char *)val,"%ld:%ld:%ld.%lf",&hh,&mm,&ss,&sec)!=4) {
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
		tt.microSecond((unsigned long)(sec*1000));
		lst=tt.value().value;
		return true;
	}
}

int main(int argc, char *argv[]) 
{
	ACS::TimeInterval lstTime;
	ACS::Time utTime,result;
	IRA::CSite site;
	IRA::CDateTime ut;
	double lst;
	TIMEVALUE myUt;
	TIMEDIFFERENCE myLst;
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
	
	return 0;
}
