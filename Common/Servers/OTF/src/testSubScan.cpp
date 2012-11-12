#include "SubScan.h"
#include <slamac.h>
#include <IRATools.h>


#define INIT_AZ 18.0 * DD2R
#define INIT_SECTOR Antenna::ACU_CCW
#define INIT_EL 45.0 * DD2R

/*#define INIT_TIME_Y 2011
#define INIT_TIME_M 1
#define INIT_TIME_D 30
#define INIT_TIME_HH 10
#define INIT_TIME_MM 45
#define INIT_TIME_SS 00
#define INIT_TIME_MS 000000*/

#define LON1  202.7845 * DD2R
#define LAT1  30.5092 * DD2R
#define LON2  0.0 * DD2R
#define LAT2  0.7 * DD2R

#define COORD_FRAME Antenna::ANT_EQUATORIAL
#define GEOMETRY Antenna::SUBSCAN_CONSTLON
#define SUBSCAN_FRAME Antenna::ANT_HORIZONTAL
#define DESCRIPTION Antenna::SUBSCAN_CENTER
#define DIRECTION Antenna::SUBSCAN_INCREASE
/*#define DELAY 0.0  // secondi tra tempo di inizializzazione e startUT*/
#define DUT1 0.0
#define DURATION 14.0

int main() {
	IRA::CSite site;
	SubScan scan;
	TIMEVALUE initTime;
	ACS::Time startUT;
	//TIMEDIFFERENCE delay((long double)DELAY);
	TIMEDIFFERENCE duration((long double)DURATION);
	/*initTime.normalize(true);
	initTime.year(INIT_TIME_Y);	
	initTime.month(INIT_TIME_M);	
	initTime.day(INIT_TIME_D);	
	initTime.hour(INIT_TIME_HH);
	initTime.minute(INIT_TIME_MM);
	initTime.second(INIT_TIME_SS);
	initTime.microSecond(INIT_TIME_MS);*/
	IRA::CIRATools::getTime(initTime);
	//IRA::CIRATools::timeCopy(startUT,initTime);
	//startUT+=delay.value();
	startUT=0;
	double init_az,init_el,lon1,lat1,lon2,lat2,dut1,lonOff,latOff;
	Antenna::TCoordinateFrame coordFrame=COORD_FRAME;
	Antenna::TsubScanGeometry geometry=GEOMETRY;
	Antenna::TCoordinateFrame subScanFrame=SUBSCAN_FRAME;
	Antenna::TsubScanDescription description=DESCRIPTION;
	Antenna::TsubScanDirection direction=DIRECTION;
	init_az = INIT_AZ; 
	Antenna::TSections init_Sector = INIT_SECTOR;
	init_el = INIT_EL;
	lon1=LON1; lat1=LAT1;
	lon2=LON2; lat2=LAT2;
	dut1=DUT1;
	lonOff=latOff=0.0;
	double lng=11.64693 * DD2R;
	double lat=44.52049 * DD2R;
	double hght=28.0;
	site.geodeticCoordinates(lng,lat,hght);
	scan.initSite(site);
	//Passing the values for speeds and accelerations (degrees/s or degrees/s^2)
	scan.initAzElRanges(0.8,0.5,0.25,0.25,0.4,0.5,15.0);
	try {
		scan.setOffsets(lonOff,latOff,subScanFrame);
		Antenna::TSections commanded_Sector;
		commanded_Sector=scan.initScan(init_az,init_Sector,init_el,initTime.value().value,lon1,lat1,lon2,lat2,coordFrame,
			geometry,subScanFrame,description,direction,startUT,dut1,
			duration.value().value);
		if (commanded_Sector==Antenna::ACU_CW) printf("Sector is CW\n");
		else printf("Sector is CCW\n");
		Antenna::OTFAttributes att;
		scan.fillAllAttributes(&att);
		/*
		 * HERE FOLLOWS WHAT HAPPENS IN fillAllAttributes:
		att->rightAscension=slaDranrm(appRa);
		att->declination=IRA::CIRATools::latRangeRad(appDec);
		att->julianEpoch=epoch;
		att->azimuth=slaDranrm(az);
		att->elevation=IRA::CIRATools::latRangeRad(el);
		att->gLongitude=slaDranrm(glon);
		att->gLatitude=IRA::CIRATools::latRangeRad(glat);
		att->parallacticAngle=pAngle;
		att->J2000RightAscension=slaDranrm(m_origCenterRA);
		att->J2000Declination=IRA::CIRATools::latRangeRad(m_origCenterDec);
		if (m_offFrame==Antenna::ANT_EQUATORIAL){
			att->userAzimuthOffset=0.0;
			att->userElevationOffset=0.0;
			att->userRightAscensionOffset=m_lonoff;
			att->userDeclinationOffset=m_latoff;
			att->userLongitudeOffset=0.0;
			att->userLatitudeOffset=0.0;
		} else if (m_offFrame==Antenna::ANT_HORIZONTAL){
			att->userAzimuthOffset=m_lonoff;
			att->userElevationOffset=m_latoff;
			att->userRightAscensionOffset=0.0;
			att->userDeclinationOffset=0.0;
			att->userLongitudeOffset=0.0;
			att->userLatitudeOffset=0.0;
		} else if (m_offFrame==Antenna::ANT_GALACTIC){
			att->userAzimuthOffset=0.0;
			att->userElevationOffset=0.0;
			att->userRightAscensionOffset=0.0;
			att->userDeclinationOffset=0.0;
			att->userLongitudeOffset=m_lonoff;
			att->userLatitudeOffset=m_latoff;
		}
		att->startLon=slaDranrm(m_startLon);
		att->startLat=IRA::CIRATools::latRangeRad(m_startLat);
		att->stopLon=slaDranrm(m_stopLon);
		att->stopLat=IRA::CIRATools::latRangeRad(m_stopLat);
		if (m_isPointingScan) {
			att->centerLon=slaDranrm(m_targetAz);
			att->centerLat=IRA::CIRATools::latRangeRad(m_targetEl);
		}
		else {
			att->centerLon=slaDranrm(m_centerLon);
			att->centerLat=IRA::CIRATools::latRangeRad(m_centerLat);
		}

		att->centerRA=slaDranrm(m_origCenterRA);
		att->centerDec=IRA::CIRATools::latRangeRad(m_origCenterDec);
		att->centerGLon=slaDranrm(m_origCenterGLon);
		att->centerGLat=IRA::CIRATools::latRangeRad(m_origCenterGLat);
		att->centerAz=slaDranrm(m_origCenterAz);
		att->centerEl=IRA::CIRATools::latRangeRad(m_origCenterEl);
		att->lonSpan=m_lonSpan;
		att->latSpan=m_latSpan;
		att->skySpan=m_skySpan;
		att->lonRate=m_lonRate;
		att->latRate=m_latRate;
		att->skyRate=m_skyRate;
		att->subScanSpan=m_subScanSpan;
		att->phiRate=m_phiRate;
		att->coordFrame=m_coordFrame;
		att->geometry=m_geometry;
		att->subScanFrame=m_subScanFrame;
		att->description=m_description;
		att->direction=m_direction;
		att->startUT=m_startUT.value().value;
		att->subScanDuration=m_subScanDuration.value().value;
		*/



		printf("rightAscension %lf \n",att.rightAscension);
		printf("declination %lf \n",att.declination);
		printf("azimuth %lf \n",att.azimuth);
		printf("elevation %lf \n",att.elevation);
		printf("julianEpoch %lf \n",att.julianEpoch);
		printf("parallacticAngle %lf \n",att.parallacticAngle);
		printf("J2000RightAscension %lf \n",att.J2000RightAscension);
		printf("J2000Declination %lf \n",att.J2000Declination);
		printf("startLon %lf \n",att.startLon);
		printf("startLat %lf \n",att.startLat);
		printf("stopLon %lf \n",att.stopLon);
		printf("stopLat %lf \n",att.stopLat);
		printf("centerLon %lf \n",att.centerLon);
		printf("centerLat %lf \n",att.centerLat);
		printf("centerRA %lf \n",att.centerRA);
		printf("centerDec %lf \n",att.centerDec);
		printf("lonSpan %lf \n",att.lonSpan);
		printf("latSpan %lf \n",att.latSpan);
		printf("lonRate %lf \n",att.lonRate);
		printf("latRate %lf \n",att.latRate);
		printf("subScanSpan %lf \n",att.subScanSpan);

	}
	catch (ComponentErrors::ValidationErrorExImpl& ex) {
		ex.log();
	}
	catch (AntennaErrors::RateTooHighExImpl& ex) {
		ex.log();
	}
	catch (AntennaErrors::ExceedingSlewingTimeExImpl& ex){
		ex.log();
	}
	catch (...) {
		printf("ERRORE INDEFINITO\n");
	}
	return 0;	
}
