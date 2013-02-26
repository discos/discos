 //$Id: SlewCheck.cpp,v 1.6 2011-04-22 18:58:08 a.orlati Exp $ 


#include <math.h>
#include <SkySource.h>
#include <IRATools.h>
#include <slamac.h>
#include <slalib.h>
#include "SlewCheck.h"

CSlewCheck::CSlewCheck() {}

CSlewCheck::~CSlewCheck() {}

void CSlewCheck::initSite(IRA::CSite& s,const double& d1)
{
	m_site=s;
	m_dut1=d1;
}


void CSlewCheck::initMount(const double& AzRate,const double& ElRate,const double& minEl,const double& maxEl,const double& maxAzAcc,const double& maxElAcc,const double& lowerEl,const double& upperEl)
{
	m_maxAzimuthRate=AzRate;
	m_maxElevationRate=ElRate;
	m_minElevation=minEl;
	m_maxElevation=maxEl;
	m_maxAzimuthAcceleration=maxAzAcc;
	m_maxElevationAcceleration=maxElAcc;
	m_minSuggestedEl=lowerEl;
	m_maxSuggestedEl=upperEl;
}


CSlewCheck::TCheckResult CSlewCheck::checkLimit(const double& targetEl,const double& minEl,const double& maxEl)
{
	double lower, upper;
	if (minEl<0) lower=m_minSuggestedEl;
	else lower=minEl;
	if (maxEl<0) upper=m_maxSuggestedEl;
	else upper=maxEl;
	if ((targetEl>m_maxElevation)||(targetEl<m_minElevation)) return NOT_VISIBLE;
	else {
		if ((targetEl>upper)||(targetEl<lower)) return AVOIDANCE;
		else return VISIBLE;
	}
}

bool CSlewCheck::checkSlewing(const double& startAz,const double& startEl,const ACS::Time& initTime,const double& targetAz,const double targetEl,
		const ACS::Time& arrivalTime,ACS::TimeInterval& slewingTime)
{
    ACS::Time azSlewingTime,elSlewingTime,allowedTime;    
    //the simple difference works because the startAz and the targetAz are already a hardware coordinate....so we can check them without caring 
    azSlewingTime=computeSlewingTime(startAz,targetAz,m_maxAzimuthRate,m_maxAzimuthAcceleration);
    elSlewingTime=computeSlewingTime(startEl,targetEl,m_maxElevationRate,m_maxElevationAcceleration);    
    if (azSlewingTime>elSlewingTime) slewingTime=azSlewingTime;
    else slewingTime=elSlewingTime;
    if (arrivalTime<=0) return true;   // if the arrival time is negative : get there as soon as possibile...no need to do further checks
    else if (arrivalTime<initTime) return false; // if arrival time is less than start time
    else {
    	allowedTime=arrivalTime-initTime;
    	if ((azSlewingTime>allowedTime) || (elSlewingTime>allowedTime)) {
         	// antenna will not reach the start position within desired time
         	return false;
    	}
    	else return true;
    }      
}

ACS::Time CSlewCheck::computeSlewingTime(const double& start,const double& stop,const double& speed,const double& acc)
{
	double sec;
	double dist;
	ACS::Time time;
	//dist=(start>=stop)?start-stop:stop-start;
	dist=fabs(stop-start);
	sec=(dist/speed)+(speed/acc);
	time=(ACS::Time)(sec*10000000);
	return time;
}

void CSlewCheck::coordConvert(const double& lon,const double& lat,const Antenna::TCoordinateFrame& coordFrame,const Antenna::TSystemEquinox& equinox,const TIMEVALUE& UT,double& az,double& el)
{
	IRA::CDateTime time(const_cast<TIMEVALUE&>(UT),m_dut1);	
	if (coordFrame==Antenna::ANT_EQUATORIAL){
		IRA::CSkySource source;
		if (equinox==Antenna::ANT_B1950) {
			source.setInputEquatorial(lon,lat,IRA::CSkySource::SS_B1950);
		}
		else if (equinox==Antenna::ANT_J2000) {
			source.setInputEquatorial(lon,lat,IRA::CSkySource::SS_J2000);
		}
		else { //apparent
			source.setInputEquatorial(lon,lat,IRA::CSkySource::SS_APPARENT);
		}
		source.process(time,m_site);
		source.getApparentHorizontal(az,el);
	}
	else if (coordFrame==Antenna::ANT_GALACTIC){
    	IRA::CSkySource source;
    	source.setInputGalactic(lon,lat);
    	source.process(time,m_site);
    	source.getApparentHorizontal(az,el);
    }
	else if (coordFrame==Antenna::ANT_HORIZONTAL){
    	az=lon;
    	el=lat;
    }
}

