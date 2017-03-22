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
	m_inputFrame=m_offsetsFrame=SS_NONE;
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
	//save input information
	m_inputFrame=SS_EQ;
	m_ra0=ra; m_dec0=dec;
	m_equinox0=eq;
	m_dra0=dra/3600000.0;
	m_dra0*=DD2R;
	m_ddec0=ddec/3600000.0;
	m_ddec0*=DD2R;
	m_rvel0=rvel;
	m_parallax0=parallax/1000.0;
	// computes Fk5 EQ coordinates (m_ra1 and m_dec1)
	CDateTime now;
	now.setCurrentDateTime();
	inputToFK5(now);
	// store the J2000 equatorial coordinates
	m_raj2000=m_ra1;
	m_decj2000=m_dec1;
	// get corresponding input galactic coordinates
	equatorialToGalactic(m_ra1,m_dec1,m_galLong0,m_galLat0);

	/*if (m_equinox0==SS_B1950) {
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
	}*/
	// compute the galactic coordinates included the offsets
	if (m_offsetsFrame==SS_GAL) {  // apply galactic offsets...if needed
		m_galLat2=m_galLat0+m_latOff;
		m_galLong2=m_galLong0+m_longOff/cos(m_galLat2);
		// if there are offsets recompute the equatorial with new galactic positions
		galacticToEquatorial(m_galLong2,m_galLat2,m_ra1,m_dec1);
	}
	else {
		m_galLat2=m_galLat0;
		m_galLong2=m_galLong0;
	}
	// reset all other variables
	m_ra2=m_dec2=m_julianEpoch2=0.0;
	m_az=m_el=0.0;
	m_parallacticAngle=0.0;
	m_fixed=false;
	m_az0=m_el0=0.0;
 }

void CSkySource::setInputGalactic(const double& longitude,const double& latitude)
{
	m_inputFrame=SS_GAL;
	CDateTime now;
	now.setCurrentDateTime();
	m_galLat0=latitude;
	m_galLong0=longitude;
	// computes the  corresponding input equatorial
	galacticToEquatorial(longitude,latitude,m_ra0,m_dec0);
	//m_epoch0=CDateTime::julianDay2JulianEpoch(DT_J2000);
	m_equinox0=SS_J2000;
	m_dra0=m_ddec0=0.0;
	m_rvel0=0.0;
	m_parallax0=0.0;
	inputToFK5(now);
	//stores the J2000 equatorial
	m_raj2000=m_ra1;
	m_decj2000=m_dec1;
	// compute the galactic coordinates included the offsets
	if (m_offsetsFrame==SS_GAL) {  // apply galactic offsets...if needed
		m_galLat2=m_galLat0+m_latOff;
		m_galLong2=m_galLong0+m_longOff/cos(m_galLat2);
		// if there are offsets recompute the equatorial with new galactic positions
		galacticToEquatorial(m_galLong2,m_galLat2,m_ra1,m_dec1);
	}
	else {
		m_galLat2=m_galLat0;
		m_galLong2=m_galLong0;
	}
	m_ra2=m_dec2=m_julianEpoch2=0.0;
	m_az=m_el=0.0;
	m_parallacticAngle=0.0;
	m_az0=m_el0=0.0;
	m_fixed=false;
}

void CSkySource::setGalacticOffsets(const double& longOff,const double& latOff)
{
	m_offsetsFrame=SS_GAL;
	m_longOff=longOff; m_latOff=latOff;
	m_azOff=m_elOff=m_raOff=m_decOff=0.0;
	setOffsets();
}

void CSkySource::setHorizontalOffsets(const double& azOff,const double& elOff)
{
	m_azOff=azOff;
	m_elOff=elOff;
	m_longOff=m_latOff=m_raOff=m_decOff=0.0;
	//if previous frame was galactic...need to reset everything
	if (m_offsetsFrame==SS_GAL) {
		m_offsetsFrame=SS_HOR;
		setOffsets();
	}
	else {
		m_offsetsFrame=SS_HOR;
	}
}

void CSkySource::setEquatorialOffsets(const double& raOff,const double& decOff)
{
	m_raOff=raOff;
	m_decOff=decOff;
	m_longOff=m_latOff=m_azOff=m_elOff=0.0;
	//if previous frame was galactic...need to reset everything
	if (m_offsetsFrame==SS_GAL) {
		m_offsetsFrame=SS_EQ;
		setOffsets();
	}
	else {
		m_offsetsFrame=SS_EQ;
	}
}

void CSkySource::setOffsets()
{
	if (m_inputFrame==SS_EQ) {
		setInputEquatorial(m_ra0,m_dec0,m_equinox0,m_dra0,m_ddec0,m_parallax0,m_rvel0);
	}
	else if (m_inputFrame==SS_GAL) {
		setInputGalactic(m_galLong0,m_galLat0);
	}
}

void CSkySource::setInputHorizontal(const double& az,const double& el,const CSite& site)
{
	double foo;
	CDateTime now;
	now.setCurrentDateTime();
	m_inputFrame=SS_HOR;
	m_az0=az; m_el0=el;
	m_parallacticAngle=0.0;
	m_az=m_el=0.0;
	// input and J2000 equatorial is the current equatorial position
	//m_epoch0=now.getJulianEpoch();
	horizontalToEquatorial(now,site,m_az0,m_el0,m_ra0,m_dec0,foo);
	m_dra0=m_ddec0=0.0;
	m_rvel0=0.0; m_parallax0=0.0;
	m_equinox0=SS_APPARENT;
	inputToFK5(now);
	// store J2000 equatorial
	m_raj2000=m_ra1;
	m_decj2000=m_dec1;
	//set input galactic
	equatorialToGalactic(m_ra1,m_dec1,m_galLong0,m_galLat0);
	m_galLat2=m_galLat0;
	m_galLong2=m_galLong0;
	m_ra2=m_dec2=m_julianEpoch2=0.0;
	m_fixed=true;
}

void CSkySource::getInputEquatorial(double& ra,double& dec,double& eph,double& dra,double& ddec,double& parallax,double& rvel)
{
 	ra=slaDranrm(m_ra0); dec=IRA::CIRATools::latRangeRad(m_dec0);
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
	longitude=slaDranrm(m_galLong0);
	latitude=IRA::CIRATools::latRangeRad(m_galLat0);
}

void CSkySource::getJ2000Equatorial(double& ra,double& dec,double& eph,double& dra,double& ddec,double& parallax,double& rvel)
{
	// here we return not m_ra1 because they might be affected by galactic offsets and not correspond to the the real source position
 	//ra=m_ra1; dec=m_dec1;
	ra=slaDranrm(m_raj2000); dec=IRA::CIRATools::latRangeRad(m_decj2000);
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
	ra=slaDranrm(m_ra2);
	dec=IRA::CIRATools::latRangeRad(m_dec2);
	eph=m_julianEpoch2;
}

void CSkySource::getApparentHorizontal(double& az,double& el)
{
	az=slaDranrm(m_az); el=IRA::CIRATools::latRangeRad(m_el);
}

void CSkySource::getApparentGalactic(double &lon,double& lat)
{
	lon=slaDranrm(m_galLong2);
	lat=IRA::CIRATools::latRangeRad(m_galLat2);
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
	ra=slaDranrm(ra);
	dec=IRA::CIRATools::latRangeRad(dec);
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
		if (m_offsetsFrame==SS_HOR) {
			m_el=m_el0+m_elOff;
			m_az=m_az0+m_azOff/cos(m_el);
		}
		else {
			m_el=m_el0;
			m_az=m_az0;
		}
		horizontalToEquatorial(now,site,m_az,m_el,m_ra2,m_dec2,m_parallacticAngle);
		m_julianEpoch2=now.getJulianEpoch();
		equatorialToGalactic(m_ra2,m_dec2,m_galLong2,m_galLat2);
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
	ra=slaDranrm(ra);
	dec=IRA::CIRATools::latRangeRad(dec);
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
			if (m_offsetsFrame==SS_EQ) {
				double tempRa,tempDec;
				m_dec2+=m_decOff;
				m_ra2+=m_raOff/cos(m_dec2);
				apparentToJ2000(m_ra2,m_dec2,now,tempRa,tempDec);
				equatorialToGalactic(tempRa,tempDec,m_galLong2,m_galLat2);
			}
		}
		else {
			slaMapqk(m_ra1,m_dec1,m_dra1,m_ddec1,m_parallax1,m_rvel1,amprms,&m_ra2,&m_dec2);
			if (m_offsetsFrame==SS_EQ) {
				double tempRa,tempDec;
				m_dec2+=m_decOff;
				m_ra2+=m_raOff/cos(m_dec2);
				apparentToJ2000(m_ra2,m_dec2,now,tempRa,tempDec);
				equatorialToGalactic(tempRa,tempDec,m_galLong2,m_galLat2);
			}
		}
	}
	else {
		if (m_offsetsFrame==SS_EQ) {
			double tempRa,tempDec;
			m_dec2=m_dec0+m_decOff;
			m_ra2=m_ra0+(m_raOff/cos(m_dec2));
			apparentToJ2000(m_ra2,m_dec2,now,tempRa,tempDec);
			equatorialToGalactic(tempRa,tempDec,m_galLong2,m_galLat2);
		}
		else {
			m_dec2=m_dec0;
			m_ra2=m_ra0;
		}
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
   	if (m_offsetsFrame==SS_HOR) {
   		m_el+=m_elOff;
   		m_az+=m_azOff/cos(m_el);
   	}
}

void CSkySource::inputToFK5(const CDateTime& time)
{
	switch (m_equinox0) {
		case CSkySource::SS_B1950: {
			m_epoch0=CDateTime::julianDay2BesselianEpoch(DT_B1950);      //Besselian epoch
			if ((m_dra0==0.0) && (m_ddec0==0.0) && (m_parallax0==0.0)) { 
				//double epoch;
				//epoch=CDateTime::julianDay2BesselianEpoch(DT_B1950);  //Besselian epoch
				slaFk45z(m_ra0,m_dec0,m_epoch0,&m_ra1,&m_dec1);
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
			m_epoch0=CDateTime::julianDay2JulianEpoch(DT_J2000);    //Julian epoch
			m_ra1=m_ra0; m_dec1=m_dec0;
			m_dra1=m_dra0; m_ddec1=m_ddec0;
			m_rvel1=m_rvel0; m_parallax1=m_parallax0;
			m_julianEpoch1=m_epoch0;
			m_equinox1=SS_J2000;				
			break;
		}
		case SS_APPARENT: {
			m_epoch0=time.getJulianEpoch();
			apparentToJ2000(m_ra0,m_dec0,time,m_ra1,m_dec1);
			m_dra1=m_dra0; m_ddec1=m_ddec0;
			m_rvel1=m_rvel0; m_parallax1=m_parallax0;
			m_julianEpoch1=CDateTime::julianDay2JulianEpoch(DT_J2000);
			m_equinox1=SS_J2000;
			break;			
		}
	} 
}

