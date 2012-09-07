// $Id: DateTime.cpp,v 1.7 2008-12-24 11:27:48 a.orlati Exp $

#include "DateTime.h"
#include <math.h>
#include <slalib.h>
#include <slamac.h>

using namespace IRA;

static const int dayMonth[12]={31,29,31,30,31,30,31,31,30,31,30,31};

CDateTime::CDateTime(const double& jd,const double& dut1)
{
	setJulianDay(jd);
	setDut1(dut1);
}

CDateTime::CDateTime(const int& year,const int& month,const int& day,const long& ms,const double& dut1)
{
	setCalendar(year,month,day,ms);
	setDut1(dut1);
}

CDateTime::CDateTime(const int& year,const int& month,const int& day,const double& ut,const double& dut1)
{
	setCalendar(year,month,day,ut);
	setDut1(dut1);
}

CDateTime::CDateTime(const int& year, const int& month,const int& day,const int& hour,const int& minute,const int& second,const long& ms,const double& dut1)
{
	setCalendar(year,month,day,(hour*3600+minute*60+second)*1000+ms);
	setDut1(dut1);
}

CDateTime::CDateTime(TIMEVALUE& now,const double& dut1)
{
	setDateTime(now,dut1);
}

CDateTime::CDateTime(const CDateTime& dt)
{
	setJulianDay(dt.m_jd);
	m_dut1=dt.m_dut1;	
}

CDateTime::~CDateTime()
{
}

const CDateTime& CDateTime::operator =(const CDateTime& src)
{
	setJulianDay(src.m_jd);
	m_dut1=src.m_dut1;
	return *(this);
}

void CDateTime::setDateTime(TIMEVALUE& now,const double& dut1)
{
	int year,month,day;
	long ms=(now.hour()*3600+now.minute()*60+now.second())*1000+now.microSecond()/1000;
	year=(int)now.year();
	month=(int)now.month();
	day=(int)now.day();
	setCalendar(year,month,day,ms);	
	setDut1(dut1);
}

void CDateTime::setCurrentDateTime(const double& dut1)
{
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	setDateTime(now);
	setDut1(dut1);
}

void CDateTime::setDut1(const double& dut1)
{
	m_dut1=dut1/(double)DT_MSECSPERDAY;
}
	
double CDateTime::getDut1() const
{
	return m_dut1*(double)DT_MSECSPERDAY;
}
	
void CDateTime::getDateTime(TIMEVALUE& time) const
{
	CORBA::Long hour,minute,second,mSecond;
	long tmp;
	time.normalize(true);
	hour=(CORBA::Long)(m_msecs/3600000);
	tmp=m_msecs%3600000;
	minute=(CORBA::Long)(tmp/60000);
	tmp=tmp%60000;
	second=(CORBA::Long)(tmp/1000);
	tmp=tmp%1000;
	// milliseconds
	mSecond=(CORBA::Long)tmp;
	time.year((CORBA::ULong)m_year);
	time.month((CORBA::Long)m_month);
	time.day((CORBA::Long)m_day);
	time.hour(hour);
	time.minute(minute);
	time.second(second);
	time.microSecond(mSecond*1000);
}

void CDateTime::getDateTime(TIMEDIFFERENCE& time) const
{
	CORBA::Long hour,minute,second,mSecond;
	long tmp;
	time.normalize(true);
	hour=(CORBA::Long)(m_msecs/3600000);
	tmp=m_msecs%3600000;
	minute=(CORBA::Long)(tmp/60000);
	tmp=tmp%60000;
	second=(CORBA::Long)(tmp/1000);
	tmp=tmp%1000;
	// milliseconds
	mSecond=(CORBA::Long)tmp;
	time.day((CORBA::Long)getDayOfYear());
	time.hour(hour);
	time.minute(minute);
	time.second(second);
	time.microSecond(mSecond*1000);	
}

int CDateTime::getMilliSeconds() const
{
	return int(m_msecs-(getHours()*3600+getMinutes()*60+getSeconds())*1000);
}

double CDateTime::getDayRadians() const
{
	return msecsToRadians(m_msecs);
}

double CDateTime::getJulianEpoch() const
{
	return slaEpj(m_jd-DT_MJD0);
}

double CDateTime::getBesselianEpoch() const
{
	return slaEpb(m_jd-DT_MJD0);
}

unsigned CDateTime::getDayOfYear() const
{
	unsigned tmp=0;
	for (int i=1;i<m_month;i++){
		tmp+=dayMonth[i-1];
		if ((i==2) && (isLeapYear(m_year))) tmp++;
	}
	return tmp+m_day;
}

bool CDateTime::setCalendar(const int& year,const int& month,const int& day,const double& ut)
{
	int j;
	if (month<1||month>12) return false;
	else if (day<1||day>dayMonth[month-1]) return false;
	if (!isLeapYear(year)) {
		if (day>28 && month==2) return false;
	}
	slaCldj(year,month,day,&m_jd,&j);
	m_jd+=DT_MJD0;
	m_jd+=ut;
	m_msecs=utToMsecs(ut);
	m_day=day;
	m_month=month;
	m_year=year;
	return true;
}

bool CDateTime::setCalendar(const int& year,const int& month,const int& day,const long& ms)
{
	int j;
	if (month<1||month>12) return false;
	else if (day<1||day>dayMonth[month-1]) return false;
	if (!isLeapYear(year)) {
		if (day>28 && month==2) return false;
	}
	slaCldj(year,month,day,&m_jd,&j);
	m_jd+=DT_MJD0;
	m_jd+=msecsToUt(ms);
	m_msecs=ms;
	m_day=day;
	m_month=month;
	m_year=year;
	return true;
}

bool CDateTime::setJulianDay(const double& jd)
{
	int j;
	double frac;
	m_jd=jd;
	slaDjcl(jd-DT_MJD0,&m_year,&m_month,&m_day,&frac,&j);
	if (j!=0) return false;
	m_msecs=utToMsecs(frac);
	return true;
}

double CDateTime::getTAI() const
{
	return m_jd+slaDat(m_jd-DT_MJD0)/DT_SECSPERDAY;
}

double CDateTime::getTT() const
{
	return m_jd+slaDtt(m_jd-DT_MJD0)/DT_SECSPERDAY;
}

double CDateTime::getTDB(const CSite& site) const
{
	double TT=getTT();
	double rcc;
	double westLong=(2*DPI)-site.getLongitude();
	double U,V;
	site.getGeocentricCoordinate(U,V);
	U/=1000.0; V/=1000.0;
	rcc=slaRcc(TT-DT_MJD0,msecsToUt(m_msecs)+m_dut1,westLong,U,V);
	return TT+rcc/DT_SECSPERDAY;
}

const CDateTime CDateTime::GST(const CSite& site) const
{
	double jd;
	double gast;
	double date,frac;
	jd=m_jd-DT_MJD0;
	jd+=m_dut1;  //UT1 as Modified Julian day
	date=floor(jd);
	frac=jd-date;
	gast=slaGmsta(date,frac);  // Greenwich mean sidereal time
	frac=getTDB(site)-DT_MJD0;
	gast+=slaEqeqx(frac); // correct for equotion of equinox: Greenwich apparent sidereal time in radians
	CDateTime dgst(m_year,m_month,m_day,radiansToMsecs(gast));
	return dgst;
}

const CDateTime CDateTime::LST(const CSite& site) const
{
	double jd;
	double gast;
	double date,frac;
	double lon,lat,delta;
	jd=m_jd-DT_MJD0;
	jd+=m_dut1;  //UT1 as Modified Julian day
	date=floor(jd);
	frac=jd-date;
	gast=slaGmsta(date,frac);  // Greenwich mean sidereal time
	frac=getTDB(site)-DT_MJD0;
	gast+=slaEqeqx(frac); // correct for equation of equinox: Greenwich apparent sidereal time in radians
	site.getTrueGeodetic(lon,lat,delta);
	gast+=lon;
	CDateTime dgst(m_year,m_month,m_day,radiansToMsecs(gast));
	return dgst;
}

bool CDateTime::isLeapYear(const int& year) const
{
	if (fmod((double)year,4)==0.)
		if (fmod((double)year,100.)==0. && fmod((double)year,400.)!=0.)
			return false;
		else
			return true;
	else
		return false;
}

void CDateTime::addSeconds(const double& s)
{
	m_jd+=s/DT_SECSPERDAY;
	setJulianDay(m_jd);
}

long CDateTime::utToMsecs(const double& ut) const
{
	return (long)(ut*((double)DT_MSECSPERDAY)); 
}

double CDateTime::msecsToUt(const long &ms) const
{
	return (ms/((double)DT_MSECSPERDAY));
}

long CDateTime::radiansToMsecs(const double& rad) const
{
	double app=(rad*DR2S)*1000.0; // convert in milliseconds of time
	app=fmod(app,DT_MSECSPERDAY); // reduceit
	if (app<0.0) app+=DT_MSECSPERDAY;
	return long(app);
}

double CDateTime::msecsToRadians(const long& ms) const
{
	double app=((double)ms/1000.0)*DS2R; //convert in radians
	app=fmod(app,D2PI);
	if (app<0.0) app+=D2PI;
	return app;
}

double CDateTime::julianEpoch2JulianDay(const double& epoch)
{
	return slaEpj2d(epoch)+DT_MJD0;
} 
	
double CDateTime::julianDay2JulianEpoch(const double& jd)
{
	return slaEpj(jd-DT_MJD0);
}

double CDateTime::besselianEpoch2JulianDay(const double& epoch)
{
	return slaEpb2d(epoch)+DT_MJD0;
} 

double CDateTime::julianDay2BesselianEpoch(const double& jd)
{
	return slaEpb(jd-DT_MJD0);
}

bool CDateTime::sidereal2UT(CDateTime& ut1,CDateTime& ut2,const double& lst,const CSite& site,const int& year,const int& month,const int& day,const double& dut1)
{
	CDateTime midnight(year,month,day,0.0,dut1);
	double gast0; // Greenwich apparent sidereal time at 0h UT
	double gast; // Greenwich apparent sidereal time at the wanted instant
	double lon,lat,delta;
	double dut;
	
	gast0=midnight.GST(site).getDayRadians();
	site.getTrueGeodetic(lon,lat,delta);
	gast=lst-lon; // Greenwich apparent sideral time at the wanted instant
	dut=slaDranrm(gast-gast0)/DT_RAD2SIDERSEC; // difference between gmst and gmst0 in seconds
	dut*=1000.0; //turn it into milliseconds
	dut-=dut1; // ut time elapsed from midnight... (dut1 is given in milliseconds)
	ut1=CDateTime(year,month,day,dut/86400000.0,dut1);
	if (dut<(24.0-DT_SIDERDAY_HOUR)*3600000.0) {  // checks if the ut instant falls in the first 3min 55.9 sec of the day, 
																						// in which case the input lst will repeat during the mean day
			dut+=(86400000.0/DT_SIDERSEC);
			ut2=CDateTime(year,month,day,dut/86400000.0,dut1);
			return false;
	}
	return true;
}

 void CDateTime::UT2Sidereal(TIMEDIFFERENCE& lst,TIMEVALUE& ut,const CSite& site,const double& dut1)
 {
	 CDateTime currentUT;
	 currentUT.setDateTime(ut,dut1);  // transform the current time in a CDateTime object
	 currentUT.LST(site).getDateTime(lst);
	 lst.day(0);
 }


