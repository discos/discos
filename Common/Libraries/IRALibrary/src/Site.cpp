// $Id: Site.cpp,v 1.6 2011-06-21 16:31:50 a.orlati Exp $

#include "Site.h"
#include <math.h>
#include <slalib.h>
#include <slamac.h>

using namespace IRA;

const double CSite::equatorialRadius[5]={6378137.0,6378137.0,6378137.0,6378136.3,6378136.0};
const double CSite::flattening[5]={298.257223563,298.257222101,298.257,298.25722,298.257};


CSite::CSite(): m_longitude(0),m_latitude(0),m_height(0),m_cartX(0),m_cartY(0),m_cartZ(0),m_xP(0),m_yP(0),m_trueLong(0),m_trueLat(0),m_deltaAz(0)
{
	changeEllipsoid(WGS84);
	setCartesian();
	setTrueGeodetic();
}

CSite::CSite(const Antenna::TSiteInformation& site): m_longitude(0),m_latitude(0),m_height(0),m_cartX(0),m_cartY(0),m_cartZ(0),m_xP(0),m_yP(0),m_trueLong(0),m_trueLat(0),m_deltaAz(0)
{
	TGeodeticEllipsoid el;
	el=(TGeodeticEllipsoid) site.elipsoid;
	m_xP=site.xP;
	m_yP=site.yP;
	geodeticCoordinates(site.longitude,site.latitude,site.height,el);
	//poleMotion(site.xP,site.yP);	
}

CSite::CSite(const CSite& src): m_longitude(0),m_latitude(0),m_height(0),m_cartX(0),m_cartY(0),m_cartZ(0),m_xP(0),m_yP(0),m_trueLong(0),m_trueLat(0),m_deltaAz(0)
{
	m_longitude=src.m_longitude;
	m_latitude=src.m_latitude;
	m_height=src.m_height;
	m_yP=src.m_yP;
	m_xP=src.m_xP;		
	changeEllipsoid(src.m_index);
	setGeodetic();
	setTrueGeodetic();
}

CSite::~CSite()
{
}

void CSite::geodeticCoordinates(const double& lng,const double& lat,const double& hght,const TGeodeticEllipsoid& lpsd)
{
	m_longitude=lng;
	m_latitude=lat;
	m_height=hght;
	changeEllipsoid(lpsd);
	setGeodetic();
	setTrueGeodetic();
}

void CSite::cartesianCoordinates(const double& x,const double& y,const double& z,const TGeodeticEllipsoid& lpsd)
{
	m_cartX=x;
	m_cartY=y;
	m_cartZ=z;
	changeEllipsoid(lpsd);
	setCartesian();
	setTrueGeodetic();	
}

void CSite::poleMotion(const double& xp,const double& yp)
{
	double y,x;
	y=yp/3600000.0; //Degrees;
	x=xp/3600000.0; //Degrees;
	m_yP=y*DD2R; m_xP=x*DD2R;
	setTrueGeodetic();	
}

Antenna::TSiteInformation CSite::getSummary()
{
	Antenna::TSiteInformation temp;
	temp.latitude=m_latitude;
	temp.longitude=m_longitude;
	temp.height=m_height;
	temp.xP=m_xP;
	temp.yP=m_yP;
	temp.elipsoid=(long)m_index;
	return temp;
}

void CSite::getGeocentricCoordinate(double& R,double& Z) const
{
	double sp,cp,c,s;
 	double b=(1.0-m_flat)*(1.0-m_flat);
	/* Geodetic to geocentric conversion */
	sp=sin(m_latitude);
	cp=cos(m_latitude);
	c=1.0/sqrt(cp*cp+b*sp*sp);
	s=b*c;
	R=(m_axis*c+m_height)*cp;
	Z=(m_axis*s+m_height)*sp;
}

void CSite::getTrueGeodetic(double& longitude,double& latitude,double& deltaAz) const
{
	longitude=m_trueLong;
	latitude=m_trueLat;
	deltaAz=m_deltaAz;
}

const CSite& CSite::operator =(const CSite& src)
{
	m_longitude=src.m_longitude;
	m_latitude=src.m_latitude;
	m_height=src.m_height;
	m_yP=src.m_yP;
	m_xP=src.m_xP;			
	changeEllipsoid(src.m_index);
	setGeodetic();
	setTrueGeodetic();
	return *(this);
}

void CSite::setEllipsoid(const TGeodeticEllipsoid& lpsd)
{
	changeEllipsoid(lpsd);
	setGeodetic();
}

void CSite::setLongitude(const double& longitude)
{
	m_longitude=longitude;
	setGeodetic();
}
	 
void CSite::setLatitude(const double& latitude)
{
	m_latitude=latitude;
	setGeodetic();
}
	 
void CSite::setHeight(const double& hgt)
{
	m_height=hgt;
	setGeodetic();
}

void CSite::setYPoleMotion(const double& yp)
{
	double y;
	y=yp/3600000.0; //Degrees;
	m_yP=y*DD2R;
	setTrueGeodetic();		
}

void CSite::setXPoleMotion(const double& xp)
{
	double x;
	x=xp/3600000.0; //Degrees;
	m_xP=x*DD2R;
	setTrueGeodetic();	
}

const double CSite::getXPoleMotion() const
{
	return (m_xP*DR2D)*3600000.0;
}

const double CSite::getYPoleMotion() const
{
	return (m_yP*DR2D)*3600000.0;
}

void CSite::setGeodetic()
{
	double e2,xn;
	double sinLong,cosLong,sinLat,cosLat;
	//ecentricity
	e2=2*m_flat-m_flat*m_flat;
	sinLong=sin(m_longitude); cosLong=cos(m_longitude);
	sinLat=sin(m_latitude); cosLat=cos(m_latitude);
	xn=m_axis/(sqrt(1-e2*sinLat*sinLat));
	m_cartX=(xn+m_height)*cosLat*cosLong;
	m_cartY=(xn+m_height)*cosLat*sinLong;
	m_cartZ=(xn*(1-e2)+m_height)*sinLat;
}

void CSite::setCartesian()
{
	static const double RIT=2.7778e-6;
	double e2,rpro,lat1,xn,s1,sqrtP2,latd,sinl;
	e2=2*m_flat-m_flat*m_flat;
	sqrtP2=sqrt(m_cartX*m_cartX+m_cartY*m_cartY);
	rpro=m_cartZ/sqrtP2;
	latd=atan(rpro/(1-e2));
	lat1=0.;
	while (fabs(latd-lat1)>RIT) {
		lat1=latd;
		s1=sin(lat1);
		xn=m_axis/(sqrt(1-e2*s1*s1));
		latd=atan(rpro*(1+e2*xn*s1/m_cartZ));
	}
	sinl=sin(latd);
	xn=m_axis/(sqrt(1-e2*sinl*sinl));
	m_height=sqrtP2/cos(latd)-xn;
	m_longitude=atan2(m_cartY,m_cartX);
	m_latitude=latd;	
}

void CSite::setTrueGeodetic()
{
	slaPolmo(m_longitude,m_latitude,m_xP,m_yP,&m_trueLong,&m_trueLat,&m_deltaAz);
}

void CSite::changeEllipsoid(const TGeodeticEllipsoid& index)
{
	m_index=index;
	m_axis=equatorialRadius[m_index];
	m_flat=1.0/flattening[m_index];
}
