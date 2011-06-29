// $id$

#include "SkySource.h"
#include <slalib.h>
#include <slamac.h>

using namespace IRA;


/* Speed of light (meters per day) */
#define SS_C 25902068345107.05
    
/* Ratio between solar and sidereal time */    
#define SS_SOLSID 1.00273790935   


CSkySource::CSkySource()
{
	m_azOff=m_elOff=m_raOff=m_decOff=m_longOff=m_latOff=0.0;
}

CSkySource::CSkySource(const double& ra,const double& dec,const TEquinox& eq,const double& dra,
  const double& ddec,const double& parallax,const double& rvel)
{
	m_azOff=m_elOff=m_raOff=m_decOff=m_longOff=m_latOff=0.0;
	setInputEquatorial(ra,dec,eq,dra,ddec,parallax,rvel);
} 

CSkySource::~CSkySource()
{	
}

void CSkySource::setInputEquatorial(const double& ra,const double& dec,const TEquinox& eq,
  const double& dra,const double& ddec,const double& parallax,const double& rvel)
 {
 	m_ra0=ra; m_dec0=dec;
	m_equinox0=eq;
	CDateTime now;
	now.setCurrentDateTime();
	if (m_equinox0==SS_B1950) {
		m_epoch0=CDateTime::julianDay2BesselianEpoch(DT_B1950);      //Besselian epoch
		slaEg50(ra,dec,&m_galLong0,&m_galLat0);
	}
	else if (m_equinox0==SS_J2000) {
		m_epoch0=CDateTime::julianDay2JulianEpoch(DT_J2000);    //Julian epoch
		slaEqgal(ra,dec,&m_galLong0,&m_galLat0);
	}
	else {    //already precessed (apparent)
		double j2000ra,j2000dec;
		m_epoch0=now.getJulianEpoch();
		apparentToJ2000(ra,dec,now,j2000ra,j2000dec);
		slaEqgal(j2000ra,j2000dec,&m_galLong0,&m_galLat0);
	}
	m_dra0=dra/3600000.0;
	m_dra0*=DD2R;
	m_ddec0=ddec/3600000.0;
	m_ddec0*=DD2R;
	m_rvel0=rvel;
	m_parallax0=parallax/1000.0;
	inputToFK5(now);
	m_raj2000=m_ra1;
	m_decj2000=m_dec1;
	m_ra2=m_dec2=m_julianEpoch2=0.0;
	m_az=m_el=0.0;
	//m_azOff=m_elOff=0.0;
	//m_raOff=m_decOff=0.0;
	//m_longOff=m_latOff=0.0;
	m_parallacticAngle=0.0;
	m_fixed=false;
	m_az0=m_el0=0.0;
 }

void CSkySource::setInputGalactic(const double& longitude,const double& latitude)
{
	CDateTime now;
	now.setCurrentDateTime();
	galacticToEquatorial(longitude,latitude,m_ra0,m_dec0);
	m_epoch0=CDateTime::julianDay2JulianEpoch(DT_J2000);
	m_galLat0=latitude;
	m_galLong0=longitude;
	m_equinox0=SS_J2000;
	m_dra0=m_ddec0=0.0;
	m_rvel0=0.0;
	m_parallax0=0.0;
	inputToFK5(now);
	m_raj2000=m_ra1;
	m_decj2000=m_dec1;
	m_ra2=m_dec2=m_julianEpoch2=0.0;
	m_az=m_el=0.0;
	//m_azOff=m_elOff=0.0;
	//m_raOff=m_decOff=0.0;
	//m_longOff=m_latOff=0.0;
	m_parallacticAngle=0.0;
	m_az0=m_el0=0.0;
	m_fixed=false;
}

void CSkySource::setGalacticOffsets(const double& longOff,const double& latOff)
{
	double longitude,latitude;
	m_longOff=longOff; m_latOff=latOff;
	latitude=m_galLat0+m_latOff;
	longitude=m_galLong0+m_longOff/cos(latitude);
	galacticToEquatorial(longitude,latitude,m_ra1,m_dec1);
}

void CSkySource::setInputHorizontal(const double& az,const double& el,const CSite& site)
{
	double foo;
	CDateTime now;
	now.setCurrentDateTime();
	m_az0=az; m_el0=el;
	//m_azOff=m_elOff=0.0;
	//m_raOff=m_decOff=0.0;
	//m_longOff=m_latOff=0.0;
	m_parallacticAngle=0.0;
	m_az=m_el=0;
	// input and J2000 equatorial is the current equtorial position
	m_epoch0=now.getJulianEpoch();
	horizontalToEquatorial(now,site,m_az0,m_el0,m_ra0,m_dec0,foo);
	m_dra0=m_ddec0=0.0;
	m_rvel0=0.0; m_parallax0=0.0;
	m_equinox0=SS_APPARENT;
	//set input galactic
	slaEqgal(m_ra0,m_dec0,&m_galLong0,&m_galLat0);
	inputToFK5(now);
	m_raj2000=m_ra1;
	m_decj2000=m_dec1;
	m_ra2=m_dec2=m_julianEpoch2=0.0;
	m_fixed=true;
}

void CSkySource::getInputEquatorial(double& ra,double& dec,double& eph,double& dra,double& ddec,double& parallax,double& rvel)
{
 	ra=m_ra0; dec=m_dec0;
	eph=m_epoch0;
	dra=m_dra0*DR2D;
	dra*=3600000.0;
	ddec=m_ddec0*DR2D;
	ddec*=3600000.0;
	rvel=m_rvel0; 
	parallax=m_parallax0*1000.0;		
}

void CSkySource::getInputGalactic(double& longitude,double& latitude)
{
	longitude=m_galLong0;
	latitude=m_galLat0;
}

void CSkySource::getJ2000Equatorial(double& ra,double& dec,double& eph,double& dra,double& ddec,double& parallax,double& rvel)
{
	// here we return not m_ra1 because they might be affected by galactic offsets and not correspond to the the real source position
 	//ra=m_ra1; dec=m_dec1;
	ra=m_raj2000; dec=m_decj2000; 
	eph=m_julianEpoch1;
	dra=m_dra1*DR2D;
	dra*=3600000.0;
	ddec=m_ddec1*DR2D;
	ddec*=3600000.0;
	rvel=m_rvel1; 
	parallax=m_parallax1*1000.0;		
}

void CSkySource::getApparentEquatorial(double& ra,double& dec,double& eph)
{
	ra=m_ra2;
	dec=m_dec2;
	eph=m_julianEpoch2;
}

void CSkySource::getApparentHorizontal(double& az,double& el)
{
	az=m_az; el=m_el;
}

void CSkySource::horizontalToEquatorial(const CDateTime& now,const CSite& site,const double& az,
   const double& el,double& ra,double& dec,double& pAngle)
{
	double lon,lat,deltaAz;
	double s;
	double ha;
	CDateTime LST;	
	/* Observatory coordinates */
	site.getTrueGeodetic(lon,lat,deltaAz); 	
	slaDh2e(az,el,lat,&ha,&dec);
	LST=now.LST(site);
	s=LST.getDayRadians();
	ra=s-ha;
	pAngle=slaPa(ha,dec,lat);
}

double CSkySource::paralacticAngle(const CDateTime& now,const CSite& site,const double& az,const double& el)
{
	double ha,dec;
	double lon,lat,deltaAz;
	site.getTrueGeodetic(lon,lat,deltaAz);
	slaDh2e(az,el,lat,&ha,&dec);
	return slaPa(ha,dec,lat);
}

void CSkySource::equatorialToEcliptic(const CDateTime& now,const CSite& site,const double& ra,const double& dec,
  double& lng,double& lat)
{
	slaEqecl(ra,dec,now.getTDB(site)-DT_MJD0,&lng,&lat);
}

void CSkySource::eclipticToEquatorial(const CDateTime& now,const CSite& site,const double& lng,const double& lat,
  double& ra,double& dec)
{
	slaEcleq(lng,lat,now.getTDB(site)-DT_MJD0,&ra,&dec);
}

void CSkySource::galacticToEquatorial(const double& lng,const double& lat,double& ra,double& dec)
{
	slaGaleq(lng,lat,&ra,&dec);
}

void CSkySource::equatorialToGalactic(const double& ra,const double& dec,double& lng,double& lat)
{
	slaEqgal(ra,dec,&lng,&lat);
}

void CSkySource::process(const CDateTime& now,const CSite& site)
{
	if (m_fixed) {
	 	m_el=m_el0+m_elOff;
	   	m_az=m_az0+m_azOff/cos(m_el);
		horizontalToEquatorial(now,site,m_az,m_el,m_ra2,m_dec2,m_parallacticAngle);
		m_julianEpoch2=now.getJulianEpoch();
	}
	else {
		J2000ToApparent(now,site);
		apparentToHorizontal(now,site);
	}
}

void CSkySource::apparentToJ2000(const double& appRA,const double& appDec,const CDateTime& time,
  double& ra,double& dec)
{
	double TDB;
	TDB=time.getTT()-DT_MJD0;  //IT should be used TDB but the difference is neglectable so we use TT.
	double epoch=CDateTime::julianDay2JulianEpoch(DT_J2000);
	slaAmp(appRA,appDec,TDB,epoch,&ra,&dec);
}

void CSkySource::J2000ToApparent(const CDateTime& now,const CSite& site)
{
	double TDB;
	TDB=now.getTDB(site)-DT_MJD0;
	if (m_equinox1!=SS_APPARENT) {
		double amprms[21];
		// compensate space motion to the apparent epoch
		m_julianEpoch2=now.getJulianEpoch();
		slaMappa(m_julianEpoch1,TDB,amprms);
		if ((m_dra1==0.0) && (m_ddec1==0.0) && (m_parallax1==0.0)) {
			slaMapqkz(m_ra1,m_dec1,amprms,&m_ra2,&m_dec2);
			m_dec2+=m_decOff;
			m_ra2+=m_raOff/cos(m_dec2);
		}
		else {
			slaMapqk(m_ra1,m_dec1,m_dra1,m_ddec1,m_parallax1,m_rvel1,amprms,&m_ra2,&m_dec2);
			m_dec2+=m_decOff;
			m_ra2+=m_raOff/cos(m_dec2);
		}
	}
	else {
		m_dec2=m_dec0+m_decOff;
		m_ra2=m_ra0+(m_raOff/cos(m_dec2));
		m_julianEpoch2=m_epoch0;
	}
}

void CSkySource::apparentToHorizontal(const CDateTime& now,const CSite& site)
{
	double diurnalAberation;
	double r,z;
	double v[3],u[3];
	double st,f;
	double h,d;
	double lon,lat,deltaAz;
	CDateTime LST;

	/* Magnitude of the diurnal aberration vector */
	site.getGeocentricCoordinate(r,z);
	diurnalAberation=D2PI*r*SS_SOLSID/SS_C;
	
	/* Observatory coordinates */
	site.getTrueGeodetic(lon,lat,deltaAz); 	
	
	/* Apparent RA,Dec to Cartesian -HA,Dec */
	LST=now.LST(site);
	st=LST.getDayRadians();
   	slaDcs2c(m_ra2-st,m_dec2,v);

	// Diurnal aberration 
   	f=1.0-diurnalAberation*v[1];
   	u[0]=f*v[0];
   	u[1]=f*(v[1]+diurnalAberation);
   	u[2]=f*v[2];
   	slaDcc2s(u,&h,&d);
   	slaDe2h(-h,d,lat,&m_az,&m_el);
   	m_parallacticAngle=slaPa(-h,d,lat);
   	m_az+=deltaAz;
   	m_el+=m_elOff;
   	m_az+=m_azOff/cos(m_el);
}

void CSkySource::inputToFK5(const CDateTime& time)
{
	switch (m_equinox0) {
		case CSkySource::SS_B1950: {
			if ((m_dra0==0.0) && (m_ddec0==0.0) && (m_parallax0==0.0)) { 
				double epoch;
				epoch=CDateTime::julianDay2BesselianEpoch(DT_B1950);  //Besselian epoch
				slaFk45z(m_ra0,m_dec0,epoch,&m_ra1,&m_dec1);
				m_dra1=0.0;
				m_ddec1=0.0;
				m_rvel1=0.0;
				m_parallax1=0.0;
			}
			else {
				slaFk425(m_ra0,m_dec0,m_dra0,m_ddec0,m_parallax0,m_rvel0,&m_ra1,&m_dec1,&m_dra1,&m_ddec1,&m_parallax1,&m_rvel1);
			}
			m_julianEpoch1=CDateTime::julianDay2JulianEpoch(DT_J2000);
			m_equinox1=SS_J2000;						
			break;
		}
		case SS_J2000: {
			m_ra1=m_ra0; m_dec1=m_dec0;
			m_dra1=m_dra0; m_ddec1=m_ddec0;
			m_rvel1=m_rvel0; m_parallax1=m_parallax0;
			m_julianEpoch1=m_epoch0;
			m_equinox1=SS_J2000;				
			break;
		}
		case SS_APPARENT: {
			apparentToJ2000(m_ra0,m_dec0,time,m_ra1,m_dec1);
			m_dra1=m_dra0; m_ddec1=m_ddec0;
			m_rvel1=m_rvel0; m_parallax1=m_parallax0;
			m_julianEpoch1=CDateTime::julianDay2JulianEpoch(DT_J2000);
			m_equinox1=SS_J2000;
			break;			
		}
	} 
}

