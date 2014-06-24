#include "FrequencyTracking.h"
#include <slalib.h>
#include <slamac.h>
#include <stdio.h>
#include <math.h>
#include "SkySource.h"

using namespace IRA;


#define CSPEED 299792.458
#define AU2KM 149.597870e6
#define DAY2SEC 86400.0

#define LSRK_TO_BARY_X 	0.28998
#define LSRK_TO_BARY_Y -17.31727
#define LSRK_TO_BARY_Z 10.00141

#define LSRD_TO_BARY_X -0.63823
#define LSRD_TO_BARY_Y -14.58542
#define LSRD_TO_BARY_Z 7.80116

#define GALCEN_TO_BARY_X 108.06585
#define GALCEN_TO_BARY_Y -112.44793
#define GALCEN_TO_BARY_Z 172.13725

#define LGROUP_TO_BARY_X 148.23284
#define LGROUP_TO_BARY_Y -133.44888
#define LGROUP_TO_BARY_Z 224.09467


CFrequencyTracking::CFrequencyTracking(const CSite& site,const double&ra2000,const double& dec2000,const CFrequencyTracking::TFrame& frame,
		const CFrequencyTracking::TDefinition& def,const double& rest,const double& v) : m_site(site), m_ra2000(ra2000), m_dec2000(dec2000), m_frame(frame), m_def(def),m_rest(rest),m_vrad(v)
{
}

bool CFrequencyTracking::frameToStr(const TFrame& frame,IRA::CString& ret)
{
	if (frame==CFrequencyTracking::LSRK) {
		ret="Kinematic LSR";
		return true;
	}
	else 	if (frame==CFrequencyTracking::LSRD) {
		ret="Dynamic LSR";
		return true;
	}
	else 	if (frame==CFrequencyTracking::BARY) {
		ret="Barycentric";
		return true;
	}
	else 	if (frame==CFrequencyTracking::GALCEN) {
		ret="Galactocentric";
		return true;
	}
	else 	if (frame==CFrequencyTracking::LGROUP) {
		ret="Local Group";
		return true;
	}
	else if (frame==CFrequencyTracking::TOPOCEN) {
		ret="Topocentric";
		return true;
	}
	else {
		return false;
	}
}

bool CFrequencyTracking::definitionToStr(const TDefinition& def,IRA::CString& ret)
{
	if (def==CFrequencyTracking::RADIO) {
		ret="Radio Definition";
		return true;
	}
	else 	if (def==CFrequencyTracking::OPTICAL) {
		ret="Optical Definition";
		return true;
	}
	else 	if (def==CFrequencyTracking::REDSHIFT) {
		ret="Redshift";
		return true;
	}
	else {
		return false;
	}
}

bool CFrequencyTracking::strToFrame(const IRA::CString& str,TFrame& frame)
{
	IRA::CString cmp(str);
	cmp.MakeUpper();
	if (cmp=="BARY") {
		frame=CFrequencyTracking::BARY;
		return true;
	}
	else if (cmp=="LSRK") {
		frame=CFrequencyTracking::LSRK;
		return true;
	}
	else if (cmp=="LSRD") {
		frame=CFrequencyTracking::LSRD;
		return true;
	}
	else if (cmp=="GALCEN") {
		frame=CFrequencyTracking::GALCEN;
		return true;
	}
	else if (cmp=="LGROUP") {
		frame=CFrequencyTracking::LGROUP;
		return true;
	}
	else if (cmp=="TOPOCEN") {
		frame=CFrequencyTracking::TOPOCEN;
		return true;
	}
	else {
		return false;
	}
}

bool CFrequencyTracking::strToDefinition(const IRA::CString& str,TDefinition& def)
{
	IRA::CString cmp(str);
	cmp.MakeUpper();
	if (cmp=="RADIO") {
		def=CFrequencyTracking::RADIO;
		return true;
	}
	else if (cmp=="OPTICAL") {
		def=CFrequencyTracking::OPTICAL;
		return true;
	}
	else if (cmp=="REDSHIFT") {
		def=CFrequencyTracking::REDSHIFT;
		return true;
	}
	else {
		return false;
	}
}

CFrequencyTracking::TCartesian CFrequencyTracking::getBarycentricVelocity(const CDateTime& time) const
{
	CFrequencyTracking::TCartesian speed;
	double matrix[3][3];
	double vb[3]; //barycentric velocity at the given epoch
	double vb1[3]; //barycentric velocity at the J2000
	double ph[3],vh[3],pb[3];
	slaPrenut(time.getJulianEpoch(),time.getMJD(),matrix);
	slaEpv(time.getTDB(m_site)-DT_MJD0,ph,vh,pb,vb);
	slaDimxv(matrix,vb,vb1);  // multiply for the transponste nutation matrix.....
	speed=cCopy(vb1);
	speed=cMult(speed,AU2KM/DAY2SEC);
	return speed;
}

CFrequencyTracking::TCartesian CFrequencyTracking::getEarthRotationVelocity(const CDateTime& time) const
{
	CFrequencyTracking::TCartesian speed;
	double v[6];
	double lat=m_site.getLatitude();
	double h=m_site.getHeight();
	slaPvobs(lat,h,time.LST(m_site).getDayRadians(),v);
	speed=cCopy(v+3);
	speed=cMult(speed,AU2KM);
	speed.toSource=toSource(time,speed); // toward apparent target
	return speed;
}

CFrequencyTracking::TCartesian CFrequencyTracking::getLSRKVelocity(const CDateTime& time) const
{
	CFrequencyTracking::TCartesian speed;
	speed.x=LSRK_TO_BARY_X;
	speed.y=LSRK_TO_BARY_Y;
	speed.z=LSRK_TO_BARY_Z;
	speed.m=magnitude(speed);
	speed.toSource=toSourceJ2000(speed);
	return speed;
}

CFrequencyTracking::TCartesian CFrequencyTracking::getLSRDVelocity(const CDateTime& time) const
{
	CFrequencyTracking::TCartesian speed;
	speed.x=LSRD_TO_BARY_X;
	speed.y=LSRD_TO_BARY_Y;
	speed.z=LSRD_TO_BARY_Z;
	speed.m=magnitude(speed);
	speed.toSource=toSourceJ2000(speed);
	return speed;
}

CFrequencyTracking::TCartesian CFrequencyTracking::getGALCENVelocity(const CDateTime& time) const
{
	CFrequencyTracking::TCartesian speed;
	speed.x=GALCEN_TO_BARY_X;
	speed.y=GALCEN_TO_BARY_Y;
	speed.z=GALCEN_TO_BARY_Z;
	speed.m=magnitude(speed);
	speed.toSource=toSourceJ2000(speed);
	return speed;
}

CFrequencyTracking::TCartesian CFrequencyTracking::getLGROUPVelocity(const CDateTime& time) const
{
	CFrequencyTracking::TCartesian speed;
	speed.x=LGROUP_TO_BARY_X;
	speed.y=LGROUP_TO_BARY_Y;
	speed.z=LGROUP_TO_BARY_Z;
	speed.m=magnitude(speed);
	speed.toSource=toSourceJ2000(speed);
	return speed;
}

CFrequencyTracking::TCartesian CFrequencyTracking::getObserverVelocity(const CDateTime& time) const
{
	CFrequencyTracking::TCartesian baryvel,rotvel,framevel;
	if (m_frame!=TOPOCEN) {
		rotvel=getEarthRotationVelocity(time);
		baryvel=getBarycentricVelocity(time);
		if (m_frame==LSRK) {
			framevel=getLSRKVelocity(time);
		}
		else if (m_frame==LSRD) {
			framevel=getLSRDVelocity(time);
		}
		else if (m_frame==BARY) {
			framevel.x=framevel.y=framevel.z=0.0;
		}
		else if (m_frame==GALCEN) {
			framevel=getGALCENVelocity(time);
		}
		else if (m_frame==LGROUP) {
			framevel=getLGROUPVelocity(time);
		}
	}
	else {
		rotvel=cZero();
		baryvel=cZero();
		framevel=cZero();
	}
	return cAdd(baryvel,framevel,rotvel);
}

double CFrequencyTracking::getFrequencyAtFrame() const
{
	/*if (m_frame==BARY) {
		return m_rest*sqrt(1-pow(m_vrad/CSPEED,2.0)) / (1+m_vrad/CSPEED);
	}
	else*/ if (m_def==RADIO) {
		return m_rest*(1-m_vrad/CSPEED);
	}
	else if (m_def==OPTICAL) {
		return m_rest/(1+m_vrad/CSPEED);
	}
	else { // REDSHIFT
		return m_rest/(1+m_vrad);
	}
}

double CFrequencyTracking::getTopocentricFrequency(const CDateTime& time) const
{
	double f1;
	CFrequencyTracking::TCartesian res;
	f1=getFrequencyAtFrame();
	res=getObserverVelocity(time);
	//wrt barycenter
	res.x*=-1.0; res.y*=-1.0; res.z*=-1.0;
	res.m=magnitude(res);
	res.toSource=toSource(time,res);
	return f1*sqrt(1-pow(res.m/CSPEED,2.0))/ (1+res.toSource/CSPEED);
}

CFrequencyTracking::TCartesian CFrequencyTracking::cCopy(double v[3]) const
{
	TCartesian vect;
	vect.x=v[0];
	vect.y=v[1];
	vect.z=v[2];
	vect.m=magnitude(vect);
	vect.toSource=toSourceJ2000(vect);
	return vect;
}

CFrequencyTracking::TCartesian CFrequencyTracking::cMult(const TCartesian& vect,const double& scalar) const
{
	TCartesian res;
	res.x=scalar*vect.x;
	res.y=scalar*vect.y;
	res.z=scalar*vect.z;
	res.m=magnitude(res);
	res.toSource=toSourceJ2000(res);
	return res;
}

CFrequencyTracking::TCartesian CFrequencyTracking::cZero() const
{
	TCartesian res;
	res.x=res.y=res.z=res.m=res.toSource=0.0;
	return res;
}

CFrequencyTracking::TCartesian CFrequencyTracking::cAdd(const TCartesian& v1,const TCartesian& v2,const TCartesian& v3) const
{
	TCartesian res;
	res.x=v1.x+v2.x+v3.x;
	res.y=v1.y+v2.y+v3.y;
	res.z=v1.z+v2.z+v3.z;
	res.m=magnitude(res);
	res.toSource=toSourceJ2000(res);
	return res;
}


double CFrequencyTracking::toSource(const CDateTime& time, const TCartesian& v) const
{
	double dir[3],vect[3];
	double ra,dec,eph;
	CSkySource source(m_ra2000,m_dec2000,CSkySource::SS_J2000);
	source.J2000ToApparent(time,m_site);
	source.getApparentEquatorial(ra,dec,eph);
	slaDcs2c(ra,dec,dir);
	vect[0]=v.x; vect[1]=v.y; vect[2]=v.z;
	return slaDvdv(dir,vect);
}

double CFrequencyTracking::toSourceJ2000( const TCartesian& v) const
{
	double dir[3],vect[3];
	slaDcs2c(m_ra2000,m_dec2000,dir);
	vect[0]=v.x; vect[1]=v.y; vect[2]=v.z;
	return slaDvdv(dir,vect);
}

double CFrequencyTracking::magnitude(const TCartesian& vect) const
{
	double p;
	p=vect.x*vect.x+vect.y*vect.y+vect.z*vect.z;
	return sqrt(p);
}

double CFrequencyTracking::scalar(const TCartesian& v1,const TCartesian& v2) const
{
	double p;
	p=v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
	return p;
}

void CFrequencyTracking::test(const CDateTime& time)
{
	double v[3];
	double vb[3],pb[3],vh[3],ph[3];
	double vcorr1,vcorr2,vcorr3,vcorr;
	double ftop;
	double ra,dec,eph;
	CSkySource source(m_ra2000,m_dec2000,CSkySource::SS_J2000);
	source.J2000ToApparent(time,m_site);
	source.getApparentEquatorial(ra,dec,eph);
	slaDcs2c(m_ra2000,m_dec2000,v);
	slaEvp(time.getTDB(m_site)-DT_MJD0,2000.0,vb,pb,vh,ph);
	vcorr1=-slaDvdv(v,vb)*AU2KM;
	printf("velocità rispetto al baricentro %lf\n",vcorr1);
	vcorr2=slaRverot(m_site.getLatitude(),ra,dec,time.LST(m_site).getDayRadians());
	printf("velocità di rotazione: %lf\n",vcorr2);
	vcorr3=slaRvlsrk(m_ra2000,m_dec2000);
	printf("velocità LSRK: %lf\n",vcorr3);
	vcorr=vcorr1+vcorr2+vcorr3;
	printf("Velocità osservatore: %lf\n",vcorr);
	vcorr+=m_vrad;
	printf("Velocità osservata del target: %lf\n",vcorr);
	ftop=m_rest*sqrt((CSPEED-vcorr)/(CSPEED+vcorr));
	printf("frequenza topocentrica: %lf\n",ftop);
}
