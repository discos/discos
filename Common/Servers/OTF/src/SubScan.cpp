//$Id: SubScan.cpp,v 1.40 2011-05-13 16:20:02 a.orlati Exp $ 

#include "SubScan.h"
#include <math.h>
#include <SkySource.h>
#include <IRATools.h>
#include <slamac.h>
#include <slalib.h>
//#include <iostream>
//#include <fstream>


SubScan::SubScan() {}

SubScan::~SubScan() {}

IRA::CSite site;
//global attributes initialisation
double az =0.0;
double el =0.0; 
double ra =0.0; 
double decl =0.0; 
double appRa =0.0;
double appDec =0.0;
double epoch =0.0;
double glon =0.0;
double glat =0.0; 
double pAngle =0.0;

/**
 * Initialisation of site parameters
 **/
void SubScan::initSite(const IRA::CSite& s)
{
	site=s;
}

/**
 * Initialisation of antenna mount parameters
 **/
void SubScan::initAzElRanges(const double& AzRate, const double& ElRate, 
		const double& rAzRate, const double& rElRate, const double& AzAcc, 
		const double& ElAcc, const double& AccScF){
	m_maxAzimuthRate=AzRate*DD2R;
	m_maxElevationRate=ElRate*DD2R;
	m_maxReliableAzimuthRate=rAzRate*DD2R;
	m_maxReliableElevationRate=rElRate*DD2R;
	m_maxAzimuthAcceleration=AzAcc*DD2R;
	m_maxElevationAcceleration=ElAcc*DD2R;
	m_accScaleFactor=AccScF;
	//obtaining an "average max acceleration" to be used independently
	//from the coordinate and scan frames
	m_maxAcc=sqrt(m_maxAzimuthAcceleration*m_maxAzimuthAcceleration+
			m_maxElevationAcceleration*m_maxElevationAcceleration);
}

Antenna::TSections SubScan::initScan (
	const double& initAz,
	const Antenna::TSections& initSector, 
	const double& initEl,
	const ACS::Time& initTime,
	const double& lon1,
    const double& lat1,
    const double& lon2,
    const double& lat2,
    const Antenna::TCoordinateFrame& coordFrame,
    const Antenna::TsubScanGeometry& geometry,
    const Antenna::TCoordinateFrame& subScanFrame,
    const Antenna::TsubScanDescription& description,
    const Antenna::TsubScanDirection& direction,
    const ACS::Time& startUT,
    const double& dut1,
    const ACS::TimeInterval& subScanDuration)
    throw (ComponentErrors::ValidationErrorExImpl, 
    		AntennaErrors::RateTooHighExImpl,
    		AntennaErrors::ExceedingSlewingTimeExImpl){


	//Check of the user defined offsets: if they are not both null,
	//it means that some offset has been set, in which case the offset frame
	//must coincide with the subscan frame.
	if (subScanFrame!=m_offFrame) {
		if ((m_lonoff!=0.0)||(m_latoff!=0.0)) {
			_EXCPT(ComponentErrors::ValidationErrorExImpl,err,"SubScan::initScan");
			err.setReason("Offset frame must be equal to subscan frame");
			throw err;
		}
	}

	//Check if the input values are correct and within proper ranges
	if (geometry==Antenna::SUBSCAN_CONSTLON) {
		if (description==Antenna::SUBSCAN_STARTSTOP) {
			if (lon1!=lon2) {
				_EXCPT(ComponentErrors::ValidationErrorExImpl,err,"SubScan::initScan"); 
				err.setReason("Start longitude must be equal to stop longitude");
				throw err;
			 }
		}
		else if (description==Antenna::SUBSCAN_CENTER) {
			if (lon2!=0) {
				_EXCPT(ComponentErrors::ValidationErrorExImpl,err,"SubScan::initScan"); 
				err.setReason("Longitude span must be zero");
				throw err;
			}
		}
	}
	else if (geometry==Antenna::SUBSCAN_CONSTLAT){
		if (description==Antenna::SUBSCAN_STARTSTOP) {
			if (lat1!=lat2) {
				_EXCPT(ComponentErrors::ValidationErrorExImpl,err,"SubScan::initScan"); 
				err.setReason("Start latitude must be equal to stop latitude");
				throw err;
			}
		}
		else if (description==Antenna::SUBSCAN_CENTER) {
			if (lat2!=0) {
				_EXCPT(ComponentErrors::ValidationErrorExImpl,err,"SubScan::initScan"); 
				err.setReason("Latitude span must be zero");
				throw err;
			}
		}
	}
	else if (geometry==Antenna::SUBSCAN_GREATCIRCLE) {
		if (description==Antenna::SUBSCAN_CENTER) {
			_EXCPT(ComponentErrors::ValidationErrorExImpl,err,"SubScan::initScan"); 
			err.setReason("Great circle scans cannot be expressed in center+span mode");
			throw err;
		}
	}
	bool reachOk;
	Antenna::TSections sect;
	if (startUT!=0) {
		// could throw ComponentErrors::ValidationErrorExImpl,AntennaErrors::RateTooHighExImpl,AntennaErrors::ExceedingSlewingTimeExImpl)
		ScanComputer(initAz,initSector,initEl,initTime,lon1,lat1,lon2,lat2,coordFrame,geometry,subScanFrame,description,direction,startUT,
				dut1,subScanDuration,false,sect,reachOk);
		return sect;
	}
	else { //have to try to guess the start UT
		reachOk=false;
		ACS::Time myStartUT;
		myStartUT=initTime+50000000; //first guess....it is now plus 5 seconds;
		while (!reachOk) {
			// could throw ComponentErrors::ValidationErrorExImpl,AntennaErrors::RateTooHighExImpl,AntennaErrors::ExceedingSlewingTimeExImpl)
			ScanComputer(initAz,initSector,initEl,initTime,lon1,lat1,lon2,lat2,coordFrame,geometry,subScanFrame,description,direction,myStartUT,
					dut1,subScanDuration,true,sect,reachOk);
			myStartUT+=20000000; //add other two seconds and check if it is a suitable start time
		}
		return sect;
	}
}

/**
 * @param reachOk true if the antenna is expected to reach the starting position on time
 * @param asap if true the function is called to determine the first useful starting time... so certain errors are not thrown
 */
void SubScan::ScanComputer(
		const double& initAz,
		const Antenna::TSections& initSector,
		const double& initEl,
		const ACS::Time& initTime,
		const double& lon1,
        const double& lat1,
        const double& lon2,
        const double& lat2,
        const Antenna::TCoordinateFrame& coordFrame,
        const Antenna::TsubScanGeometry& geometry,
        const Antenna::TCoordinateFrame& subScanFrame,
        const Antenna::TsubScanDescription& description,
        const Antenna::TsubScanDirection& direction,
        const ACS::Time& startUT,
        const double& dut1,
        const ACS::TimeInterval& subScanDuration,
        bool asap,
        Antenna::TSections& sect,
        bool& reachOk)
		throw (ComponentErrors::ValidationErrorExImpl,
				AntennaErrors::RateTooHighExImpl,
				AntennaErrors::ExceedingSlewingTimeExImpl)
{


	sect=Antenna::ACU_NEUTRAL;

	//initialisation
	m_initTime.value(initTime);
	m_geometry=geometry;
	m_coordFrame=coordFrame;
	m_subScanFrame=subScanFrame;
	m_description=description;
	m_direction=direction;
	m_startUT.value(startUT);
	m_dut1=dut1;
	m_subScanDuration.value(subScanDuration);
	m_stopUT.value(m_startUT.value());
	m_stopUT.add(m_subScanDuration.value());
	m_midUT.value(m_startUT.value().value+m_subScanDuration.value().value/2);
	m_lonRate=0.0;
	m_latRate=0.0;
	m_skyRate=0.0;
	m_skySpan=0.0;
	m_subScanSpan=0.0;
	m_phiRate=0.0;
	m_centerRA=0.0;
	m_centerDec=0.0;
	m_centerGLon=m_centerGLat=0.0;
	m_isPointingScan=(m_coordFrame==Antenna::ANT_EQUATORIAL)&&(m_subScanFrame==Antenna::ANT_HORIZONTAL);

	reachOk=true;
	
	
	/**
	 * Setting the subscan parameters in accordance to the selected
	 * geometry and mode.
	**/
	if (coordFrame!=subScanFrame){
		if (m_isPointingScan) {
			if ((m_geometry==Antenna::SUBSCAN_CONSTLON)||(m_geometry==Antenna::SUBSCAN_CONSTLAT)){
				/* This is a scan for pointing calibrations.
				 * It can only be a center+span subscan, centered on a sidereal
				 * source - identified by means of J2000.0 equatorial coordinates -
				 * and performing the scans along the horizontal coordinates axes */
				m_centerLon=lon1;
				m_centerLat=lat1;
				m_lonSpan=lon2;
				m_latSpan=lat2;
				setPointingScan();
				if (m_initTime < m_rampStartTime.value()) {
					setSector(m_initTime, initAz, initSector, initEl,m_rampStartLon, m_rampStartLat, m_rampStartTime,m_rampStopLon,m_rampStopLat,m_rampStopTime,
						m_subScanFrame, m_dut1,site,asap,sect,reachOk);  //could throw AntennaErrors::ExceedingSlewingTimeExImpl
					if (asap && !reachOk) return; // if the scan is asap and the target cannot be reached on time...exit for another iteration
				}
				else{
					reachOk=false;
					// err: present time is beyond the commanded start time!
					if (!asap) {
						_EXCPT(ComponentErrors::ValidationErrorExImpl,err,"SubScan::ScanComputer");
						err.setReason("Commanded time has already passed");
						throw err;
					}
					else return;
				}

			} else {
				_EXCPT(ComponentErrors::ValidationErrorExImpl,err,"SubScan::ScanComputer");
				err.setReason("Pointing calibration scans can be CONSTLON or CONSTLAT only");
				throw err;
			}

		}
		else {
			_EXCPT(ComponentErrors::ValidationErrorExImpl,err,"SubScan::ScanComputer");
			err.setReason("Input coordinates frame must match the scan frame");
			throw err;
		}
	}


	/* In all the other cases we are dealing with subscans for which
	 * the coordinate frame matches the subscan frame.
	 * The following operations take place depending on the subscan
	 * geometry and other parameters. That's why we have a long list
	 * of "IF" clauses.*/

	else {
		if (description==Antenna::SUBSCAN_STARTSTOP) {
			double midlat = (lat1+lat2)/2;
			m_startLon=lon1+m_lonoff/cos(midlat);
			m_startLat=lat1+m_latoff;
			m_stopLon=lon2+m_lonoff/cos(midlat);
			m_stopLat=lat2+m_latoff;
			if (fabs(m_stopLon-m_startLon)<DPI) {
				m_lonSpan=fabs(m_stopLon-m_startLon);
			} else {
				m_lonSpan=(D2PI-fabs(m_stopLon-m_startLon));
			}
			m_latSpan=fabs(m_stopLat-m_startLat);
				
			if (m_geometry==Antenna::SUBSCAN_CONSTLON) {
				m_centerLat=(m_startLat+m_stopLat)/2.0;
				m_centerLon=m_startLon;

				m_latRate=(m_stopLat-m_startLat)/(IRA::CIRATools::timeMicroSeconds(m_subScanDuration)/1000000.0);
				m_lonRate=0.0;
				m_skyRate=m_latRate;
				m_skySpan=m_latSpan;
			}
			else if (m_geometry==Antenna::SUBSCAN_CONSTLAT) {
				m_centerLon=(m_startLon+m_stopLon)/2.0;
				m_centerLat=m_startLat;
				if (fabs(m_stopLon-m_startLon)<DPI) {
					m_lonRate=((m_stopLon-m_startLon)/(IRA::CIRATools::timeMicroSeconds(m_subScanDuration)/1000000.0));
				} else {
					m_lonRate=(D2PI-fabs(m_stopLon-m_startLon))/(IRA::CIRATools::timeMicroSeconds(m_subScanDuration)/1000000.0);
					if (m_direction==Antenna::SUBSCAN_DECREASE) {
						m_lonRate=-m_lonRate;
					}
				}
				m_latRate=0.0;
				m_skyRate=m_lonRate*cos(m_startLat);
				m_skySpan=m_lonSpan*cos(m_startLat);
			}
			else if (m_geometry==Antenna::SUBSCAN_GREATCIRCLE) {
				GreatCircle::Tspherical sph_start;
				GreatCircle::Tspherical sph_stop;
				sph_start.l=m_startLon+m_lonoff/cos(m_startLat);
				sph_start.b=m_startLat+m_latoff;
				sph_stop.l=m_stopLon+m_lonoff/cos(m_stopLat);
				sph_stop.b=m_stopLat+m_latoff;
				m_greatCircle.setGreatCircle(sph_start, sph_stop);
				m_centerLon=m_greatCircle.getGreatCircleCenter().l;
				m_centerLat=m_greatCircle.getGreatCircleCenter().b;
				m_subScanSpan=m_greatCircle.psi;
				m_phiRate=m_greatCircle.psi/(IRA::CIRATools::timeMicroSeconds(m_subScanDuration)/1000000.0);
				m_skyRate=m_phiRate;
				m_skySpan=m_subScanSpan;
				/*Checking if the azimuth or elevation rate exceeds limits
				 *during great circle subscan.
				 *This is necessary because, depending on the particular path
				 *described by the great circle arc, the azimuth or elevation
				 *rate can have extremely high peaks even if, on average, the
				 *values stay within the imposed limits*/
				double fPhi = 0.0;
				double fLat1 = 0.0;
				double fLon1 = 0.0;
				double fLat2 = 0.0;
				double fLon2 = 0.0;
				double fAz1 = 0.0;
				double fEl1 = 0.0;
				double fAz2 = 0.0;
				double fEl2 = 0.0;
				double fAzRate = 0.0;
				double fElRate = 0.0;
				TIMEVALUE fUtTime;
				long double step = 1.0;
				TIMEDIFFERENCE fStep(step);
				IRA::CIRATools::timeCopy(fUtTime,m_startUT);
				bool azWarningCheck=false;
				bool elWarningCheck=false;
				while (fPhi<m_subScanSpan) {
					//printf("Check su errore Az: %d \n",azWarningCheck);
					//printf("Check su errore El: %d \n",elWarningCheck);
					fLon1=m_greatCircle.getGreatCirclePoint(fPhi).l;
					fLat1=m_greatCircle.getGreatCirclePoint(fPhi).b;
					coordConvert(fLon1, fLat1, m_coordFrame, fUtTime,m_dut1, site);
					fAz1=az;
					fEl1=el;
					//printf("Valore iniziale di fPhi: %lf \n",fPhi);
					//printf("Az,El del primo punto: %lf%lf \n",fAz1,fEl1);
					//Increasing fPhi considering a 1sec step
					fPhi=fPhi+(m_phiRate*step);
					//printf("Valore finale di fPhi: %lf \n",fPhi);
					fUtTime.add(fStep.value());
					fLon2=m_greatCircle.getGreatCirclePoint(fPhi).l;
					fLat2=m_greatCircle.getGreatCirclePoint(fPhi).b;
					coordConvert(fLon2, fLat2, m_coordFrame, fUtTime,m_dut1, site);
					fAz2=az;
					fEl2=el;
					//printf("Az,El del secondo punto: %lf%lf \n",fAz2,fEl2);
					double deltaAzimuth = fabs(fAz2-fAz1);
					if (deltaAzimuth>=DPI){
					   deltaAzimuth = D2PI - deltaAzimuth;
					}
					fAzRate=deltaAzimuth/step;
					fElRate=(fEl2-fEl1)/step;
					//printf("Rate in Az,El: %lf%lf \n",fAzRate,fElRate);
					if (fabs(fAzRate)>m_maxAzimuthRate) {
						_EXCPT(AntennaErrors::RateTooHighExImpl,err,"SubScan::ScanComputer");
						err.setAxis("Azimuth");
						throw err;
					}
					else if (fabs(fAzRate)>m_maxReliableAzimuthRate){
						if (azWarningCheck==false){
							ACS_LOG(LM_FULL_INFO, "SubScan::ScanComputer()", (LM_WARNING,"AZ_RATE_BEYOND_RECOMMENDED_LIMIT"));
							azWarningCheck=true;
						}
					}
					if (fabs(fElRate)>m_maxElevationRate) {
						_EXCPT(AntennaErrors::RateTooHighExImpl,err,"SubScan::ScanComputer");
						err.setAxis("Elevation");
						throw err;
					} else if (fabs(fElRate)>m_maxReliableElevationRate){
						if (elWarningCheck==false){
							ACS_LOG(LM_FULL_INFO, "SubScan::ScanComputer()", (LM_WARNING,"EL_RATE_BEYOND_RECOMMENDED_LIMIT"));
							elWarningCheck=true;
							}
					}
				}
			}
		
		}
		else if (m_description==Antenna::SUBSCAN_CENTER) {
			m_centerLon=lon1+m_lonoff/cos(lat1);
			m_centerLat=lat1+m_latoff;
			m_lonSpan=lon2/cos(lat1);
			m_latSpan=lat2;
			if (m_direction==Antenna::SUBSCAN_INCREASE){
				if (m_geometry==Antenna::SUBSCAN_CONSTLAT) {
					m_startLon=m_centerLon-(m_lonSpan/2);
					m_startLat=m_centerLat;
					m_stopLon=m_centerLon+(m_lonSpan/2);
					m_stopLat=m_centerLat;
					m_latSpan=0;
					if (fabs(m_stopLon-m_startLon)<DPI) {
						m_lonRate=(m_stopLon-m_startLon)/(IRA::CIRATools::timeMicroSeconds(m_subScanDuration)/1000000.0);
					} else {
						m_lonRate=(D2PI-fabs(m_stopLon-m_startLon))/(IRA::CIRATools::timeMicroSeconds(m_subScanDuration)/1000000.0);
					}
					m_latRate=0.0;
				}
				else if (m_geometry==Antenna::SUBSCAN_CONSTLON) {
					m_startLon=m_centerLon;
					m_startLat=m_centerLat-(m_latSpan/2);
					m_stopLon=m_centerLon;
					m_stopLat=m_centerLat+(m_latSpan/2);
					m_lonSpan=0;
					m_latRate=((m_stopLat-m_startLat)/(IRA::CIRATools::timeMicroSeconds(m_subScanDuration)/1000000.0));
					m_lonRate=0.0;
				}
			}
			else if (m_direction==Antenna::SUBSCAN_DECREASE){
				if (m_geometry==Antenna::SUBSCAN_CONSTLAT) {
					m_startLon=m_centerLon+(m_lonSpan/2);
					m_startLat=m_centerLat;
					m_stopLon=m_centerLon-(m_lonSpan/2);
					m_stopLat=m_centerLat;
					m_latSpan=0;
					if (fabs(m_stopLon-m_startLon)<DPI) {
						m_lonRate=(m_stopLon-m_startLon)/(IRA::CIRATools::timeMicroSeconds(m_subScanDuration)/1000000.0);
					} else {
						m_lonRate= -(D2PI-fabs(m_stopLon-m_startLon))/(IRA::CIRATools::timeMicroSeconds(m_subScanDuration)/1000000.0);
					}
					m_latRate=0.0;
				}
				else if (m_geometry==Antenna::SUBSCAN_CONSTLON) {
					m_startLon=m_centerLon;
					m_startLat=m_centerLat+(m_latSpan/2);
					m_stopLon=m_centerLon;
					m_stopLat=m_centerLat-(m_latSpan/2);
					m_lonSpan=0;
					m_latRate=((m_stopLat-m_startLat)/(IRA::CIRATools::timeMicroSeconds(m_subScanDuration)/1000000.0));
					m_lonRate=0.0;
				}
			}
		}
		// Computing azimuth and elevation rates, along the whole subscan,
		// for equatorial and galactic subscans in constlon and constlat geometries
		if (((m_subScanFrame==Antenna::ANT_EQUATORIAL)||(m_subScanFrame==Antenna::ANT_GALACTIC))&&
		  ((m_geometry==Antenna::SUBSCAN_CONSTLAT)||(m_geometry==Antenna::SUBSCAN_CONSTLON))){
			double fAz1 = 0.0;
			double fEl1 = 0.0;
			double fAz2 = 0.0;
			double fEl2 = 0.0;
			double fAzRate = 0.0;
			double fElRate = 0.0;
			TIMEVALUE fUtTime;
			long double step = 1.0;
			TIMEDIFFERENCE fStep(step);
			IRA::CIRATools::timeCopy(fUtTime,m_startUT);
			bool azWarningCheck=false;
			bool elWarningCheck=false;
			while (fUtTime.value().value<(m_stopUT.value().value-fStep.value().value)) {
				computePointingForUT(fUtTime);
				fAz1=az;
				fEl1=el;
				fUtTime.add(fStep.value());
				computePointingForUT(fUtTime);
				fAz2=az;
				fEl2=el;
				double deltaAzimuth = fabs(fAz2-fAz1);
				if (deltaAzimuth>=DPI){
				   deltaAzimuth = D2PI - deltaAzimuth;
				}
				fAzRate=deltaAzimuth/step;
				fElRate=(fEl2-fEl1)/step;
				if (fabs(fAzRate)>m_maxAzimuthRate) {
					_EXCPT(AntennaErrors::RateTooHighExImpl,err,"SubScan::ScanComputer");
					err.setAxis("Azimuth");
					throw err;
				} else if (fabs(fAzRate)>m_maxReliableAzimuthRate){
					if (azWarningCheck==false){
						ACS_LOG(LM_FULL_INFO, "SubScan::ScanComputer()", (LM_WARNING,"AZ_RATE_BEYOND_RECOMMENDED_LIMIT"));
						azWarningCheck=true;
					}
				}
				if (fabs(fElRate)>m_maxElevationRate) {
					_EXCPT(AntennaErrors::RateTooHighExImpl,err,"SubScan::ScanComputer");
					err.setAxis("Elevation");
					throw err;
				} else if (fabs(fElRate)>m_maxReliableElevationRate){
					if (elWarningCheck==false){
						ACS_LOG(LM_FULL_INFO, "SubScan::ScanComputer()", (LM_WARNING,"EL_RATE_BEYOND_RECOMMENDED_LIMIT"));
						elWarningCheck=true;
					}
				}
			}
		}
		// Rate check for horizontal subscans: it is sufficient to confront the
		// subscan boundaries
		if ((m_subScanFrame==Antenna::ANT_HORIZONTAL)&&((m_geometry==Antenna::SUBSCAN_CONSTLAT)||
		  (m_geometry==Antenna::SUBSCAN_CONSTLON))){
			coordConvert(m_startLon, m_startLat, m_coordFrame, m_startUT, m_dut1, site);
			double az1=az;
			double el1=el;
			coordConvert(m_stopLon, m_stopLat, m_coordFrame, m_stopUT, m_dut1, site);
			double az2=az;
			double el2=el;
			double dAz=fabs(az2-az1);
			if (dAz>=DPI){
				 dAz = D2PI - dAz;
			}
			double azRate=(dAz)/(IRA::CIRATools::timeMicroSeconds(m_subScanDuration)/1000000.0);
			double elRate=(el2-el1)/(IRA::CIRATools::timeMicroSeconds(m_subScanDuration)/1000000.0);
			//checking if azimuth and elevation rates are below the limits
			if (fabs(azRate)>=m_maxAzimuthRate) {
				_EXCPT(AntennaErrors::RateTooHighExImpl,err,"SubScan::ScanComputer");
				err.setAxis("Azimuth");
				throw err;
			}
			else if (fabs(azRate)>m_maxReliableAzimuthRate){
				ACS_LOG(LM_FULL_INFO, "SubScan::ScanComputer()", (LM_WARNING,"AZ_RATE_BEYOND_RECOMMENDED_LIMIT"));
			}
			if (fabs(elRate)>=m_maxElevationRate) {
				_EXCPT(AntennaErrors::RateTooHighExImpl,err,"SubScan::ScanComputer");
				err.setAxis("Elevation");
				throw err;
			}
			else if (fabs(elRate)>m_maxReliableElevationRate){
				ACS_LOG(LM_FULL_INFO, "SubScan::ScanComputer()", (LM_WARNING,"EL_RATE_BEYOND_RECOMMENDED_LIMIT"));
			}
		}
	
		/* Implementing a "ramp generator" which imposes a given
		 * acceleration to the antenna properly computing the sequence of pointings
		 * by means of the uniformly accelerated motion law, in order to have the
		 * antenna running at the desired speed when starting data acquisition on the
		 * subscan starting point. Similarly, computing a decelerating ramp
		 * after the end of the subscan.
		 * The acceleration value is derived from the maximum Az-El values and properly
		 * scaled down: this way it guarantees it stays within range, applies to all
		 * frames and produce ramps which are fast but well known and defined.
		 * The antenna pointing is computed depending ot the UT instant, such discerning
		 * if it should be moving along a ramp or acquiring data at "cruise speed"
		 */
	
		if 	(m_geometry==Antenna::SUBSCAN_CONSTLON) {
			if (m_direction==Antenna::SUBSCAN_INCREASE) {
				m_rampLatAcceleration = m_maxAcc/m_accScaleFactor;
			}
			else if (m_direction==Antenna::SUBSCAN_DECREASE){
				m_rampLatAcceleration = -m_maxAcc/m_accScaleFactor;
			}
			m_rampLonAcceleration = 0.0;
			m_rampDuration.value(ACS::TimeInterval((m_latRate/m_rampLatAcceleration)*10000000.0));
			// acceleration ramp before data acquisition
			m_rampStartTime.value(m_startUT.value());
			m_rampStartTime.subtract(m_rampDuration.value());
			m_rampStartLon=m_startLon;
			m_rampStartLat=m_startLat-(0.5*m_rampLatAcceleration*(pow((double(IRA::CIRATools::timeMicroSeconds(m_rampDuration))*0.000001),2.0)));
			//deceleration ramp after data acquisition
			m_rampStopTime.value(m_stopUT.value());
			m_rampStopTime.add(m_rampDuration.value());
			m_rampStopLon=m_stopLon;
			m_rampStopLat=m_stopLat+(0.5*m_rampLatAcceleration*(pow((double(IRA::CIRATools::timeMicroSeconds(m_rampDuration))*0.000001),2.0)));
		}
		else if (m_geometry==Antenna::SUBSCAN_CONSTLAT) {
			if (m_direction==Antenna::SUBSCAN_INCREASE) {
				m_rampLonAcceleration = m_maxAcc/m_accScaleFactor;
			}
			else if (m_direction==Antenna::SUBSCAN_DECREASE){
				m_rampLonAcceleration = -m_maxAcc/m_accScaleFactor;
			}
			m_rampLatAcceleration = 0.0;
			m_rampDuration.value(ACS::TimeInterval((m_lonRate/m_rampLonAcceleration)*10000000.0));
			// acceleration before data acquisition
			m_rampStartTime.value(m_startUT.value());
			m_rampStartTime.subtract(m_rampDuration.value());
			m_rampStartLon=m_startLon-(0.5*m_rampLonAcceleration*(pow((double(IRA::CIRATools::timeMicroSeconds(m_rampDuration))*0.000001),2.0)));
			m_rampStartLat=m_startLat;
			// deceleration after data acquisition
			m_rampStopTime.value(m_stopUT.value());
			m_rampStopTime.add(m_rampDuration.value());
			m_rampStopLat=m_stopLat;
			m_rampStopLon=m_stopLon+(0.5*m_rampLonAcceleration*(pow((double(IRA::CIRATools::timeMicroSeconds(m_rampDuration))*0.000001),2.0)));
		}
		else if (m_geometry==Antenna::SUBSCAN_GREATCIRCLE) {
			/* In analogy to the CONSTLON and CONSTLAT modes ramps, the GREATCIRCLE
			 * ones are computed as a function of phi (the angle along the circle arc)
			 * using the uniformly accelerated motion law. Remember phi=0 means the
			 * antenna is pointing to the subscan acquisition starting point,
			 * phi=psi means the antenna is on the stop point.
			 */
			m_rampPhiAcceleration = m_maxAcc/m_accScaleFactor;
			m_rampDuration.value(ACS::TimeInterval((m_phiRate/m_rampPhiAcceleration)*10000000.0));
			m_rampSpan=(0.5*m_rampPhiAcceleration*(pow((double(IRA::CIRATools::timeMicroSeconds(m_rampDuration))*0.000001),2.0)));
			// acceleration before data acquisition
			m_rampStartTime.value(m_startUT.value());
			m_rampStartTime.subtract(m_rampDuration.value());
			m_rampStartLon=m_greatCircle.getGreatCirclePoint(-fabs(m_rampSpan)).l;
			m_rampStartLat=m_greatCircle.getGreatCirclePoint(-fabs(m_rampSpan)).b;
			// deceleration after data acquisition
			m_rampStopTime.value(m_stopUT.value());
			m_rampStopTime.add(m_rampDuration.value());
			m_rampStopLon=m_greatCircle.getGreatCirclePoint(fabs(m_greatCircle.psi+m_rampSpan)).l;
			m_rampStopLat=m_greatCircle.getGreatCirclePoint(fabs(m_greatCircle.psi+m_rampSpan)).b;
		}
		if (m_initTime < m_rampStartTime.value()){
			setSector(m_initTime, initAz, initSector, initEl,m_rampStartLon, m_rampStartLat, m_rampStartTime,
				m_rampStopLon, m_rampStopLat, m_rampStopTime,
				m_coordFrame,m_dut1,site,asap,sect,reachOk); //could throw AntennaErrors::ExceedingSlewingTimeExImpl
			if (asap && !reachOk) return; // if the scan is asap and the target cannot be reached on time...exit for another iteration
		}
		else{
			reachOk=false;
			// err: present time is beyond the commanded start time!
			if (!asap) {
				_EXCPT(ComponentErrors::ValidationErrorExImpl,err,"SubScan::ScanComputer");
				err.setReason("Commanded time has already passed");
				throw err;
			}
			else return;
		}
		//getting the central point EQ and GAL coordinates 
		coordConvert(m_centerLon, m_centerLat, m_coordFrame, m_midUT,m_dut1, site);
		m_centerRA=ra;
		m_centerDec=decl;
		m_centerGLon=glon;
		m_centerGLat=glat;
	}
}

/*
 * Method considers the acceleration ramp start position and the
 * deceleration ramp stop position and computes if the start point is  
 * in a CW or CCW sector. 
 * It also checks if the antenna is able to reach the acceleration ramp start position on time,
 * considering its position at setup time and the maximum speed it can reach
 */
void SubScan::setSector (const TIMEVALUE& initTime, const double& initAz, const Antenna::TSections& initSector,
		const double& initEl, const double& rampStartLon, const double& rampStartLat, const TIMEVALUE& rampStartTime,
		const double& rampStopLon, const double& rampStopLat, const TIMEVALUE& rampStopTime,
		const Antenna::TCoordinateFrame& subScanFrame, const double& dut1,const IRA::CSite& site,bool asap,Antenna::TSections &sect,bool& reachOk)
        throw (AntennaErrors::ExceedingSlewingTimeExImpl)
{
	    /* Getting the horizontal coordinates of start and stop positions*/
	coordConvert(rampStartLon, rampStartLat, subScanFrame, rampStartTime,
	dut1, site);
	
	/*printf("RampStartLon %lf\n",rampStartLon);
	printf("RampStartLat %lf\n",rampStartLat);*/
	
	double startAz=az;
	double startEl=el;
	coordConvert(rampStopLon, rampStopLat, subScanFrame, rampStopTime,dut1, site);
	double stopAz=az;
	    	 
	/* Converting the 0/360°+sector indication into -90°/450° range */
	double initAzimuth = initAz;
	if ((initAz<=90*DD2R)&&(initSector==Antenna::ACU_CW)){
		initAzimuth += 360*DD2R;
	}
	else if ((initAz>=270*DD2R)&&(initSector==Antenna::ACU_CCW)){
		initAzimuth -= 360*DD2R;
	}

	/* Determining the CW or CCW sector to be commanded with the ramp start
	 * point horizontal coordinates*/
	Antenna::TSections startSector=Antenna::ACU_NEUTRAL;
	if ((startAz>=90*DD2R)&&(startAz<=180*DD2R))
		startSector=Antenna::ACU_CCW;
	if ((startAz>180*DD2R)&&(startAz<=270*DD2R))
	  	startSector=Antenna::ACU_CW;
	if ((((startAz>=0)&&(startAz<=90*DD2R))||((startAz>=270*DD2R)&&(startAz<=360*DD2R)))
	  &&(((stopAz>=0)&&(stopAz<=90*DD2R))||((stopAz>=270*DD2R)&&(stopAz<=360*DD2R))))
		startSector=initSector;
	double azDifference = stopAz-startAz;
	if ((startAz>=0)&&(startAz<=90*DD2R)) {
		if ((stopAz>180*DD2R)&&(stopAz<=270*DD2R)) {
			if (azDifference<180*DD2R) startSector=Antenna::ACU_CCW;
	    	else startSector=Antenna::ACU_CW;
	    }
		else if ((stopAz>=90*DD2R)&&(stopAz<=180*DD2R)) {
	    	startSector=Antenna::ACU_CCW;
		}
	}
	else if ((startAz>=270*DD2R)&&(startAz<=360*DD2R)) {
		if ((stopAz>180*DD2R)&&(stopAz<=270*DD2R)) {
			startSector=Antenna::ACU_CW;
	    }
		else if ((stopAz>=90*DD2R)&&(stopAz<=180*DD2R)) {
			if (azDifference<180*DD2R) startSector=Antenna::ACU_CW;
	    	else startSector=Antenna::ACU_CCW;
	    }
	}
	    
	/* Computing if slewing time exceeds the user-set delay*/
	//TIMEDIFFERENCE azSlewRampTime;
	//TIMEDIFFERENCE elSlewRampTime;
	//TIMEDIFFERENCE azSlewMaxSpeedTime;
    //TIMEDIFFERENCE elSlewMaxSpeedTime;
    TIMEDIFFERENCE azSlewingTime;
    TIMEDIFFERENCE elSlewingTime;
    TIMEDIFFERENCE deltaTime;
    TIMEDIFFERENCE safeGap;
    //azSlewRampTime.reset();
    //elSlewRampTime.reset();
    //azSlewMaxSpeedTime.reset();
    //elSlewMaxSpeedTime.reset();
    azSlewingTime.reset();
    elSlewingTime.reset();
    deltaTime.reset();
    safeGap.reset();
    //azSlewRampTime.normalize(true);
    //elSlewRampTime.normalize(true);
    //azSlewMaxSpeedTime.normalize(true);
    //elSlewMaxSpeedTime.normalize(true);
    azSlewingTime.normalize(true);
    elSlewingTime.normalize(true);
    deltaTime.normalize(true);
    safeGap.normalize(true);
    //copying EpochHelper and DurationHelper to avoid usage of const properties
    TIMEVALUE start;
	IRA::CIRATools::timeCopy(start,rampStartTime);
    TIMEVALUE init;
	IRA::CIRATools::timeCopy(init,initTime);
    deltaTime.value(start.difference(init.value()));
    double deltaAz;
    double deltaEl;
    if ((startAz<=90*DD2R)&&(startSector==Antenna::ACU_CW)){
    	startAz += 360*DD2R;
    } else if ((startAz>=270*DD2R)&&(startSector==Antenna::ACU_CCW)){
 	   	startAz -= 360*DD2R;
 	}
	deltaAz=fabs(startAz-initAzimuth);
	//if (deltaAz > DPI) {deltaAz = D2PI - deltaAz;}
	deltaEl=fabs(startEl-initEl);
	/*printf("initAz %lf\n",initAzimuth);
	printf("initEl %lf\n",initEl);
	printf("startAz %lf\n",startAz);
	printf("startEl %lf\n",startEl);
	printf("deltaAz %lf\n",deltaAz);
	printf("deltaEl %lf\n",deltaEl);*/
	
	// Setting the safety gap to 2 seconds
	safeGap.value((long double)(5.0));
	// The following approximation is conservative as it over-estimates the slewing time
	// in case the slewing path is shorter than the acceleration ramp
    azSlewingTime.value((long double)(deltaAz/m_maxAzimuthRate+m_maxAzimuthRate/m_maxAzimuthAcceleration));
    elSlewingTime.value((long double)(deltaEl/m_maxElevationRate+m_maxElevationRate/m_maxElevationAcceleration));
    // Adding the safety gap
    azSlewingTime.add(safeGap.value());
    elSlewingTime.add(safeGap.value());
    /*printf("initTime %d:%d:%d\n",initTime.hour(),initTime.minute(),initTime.second());
    printf("rampStartTime %d:%d:%d\n",rampStartTime.hour(),rampStartTime.minute(),rampStartTime.second());
    printf("deltaTime %d:%d:%d\n",deltaTime.hour(),deltaTime.minute(),deltaTime.second());
    printf("AzSlewTime %d:%d:%d\n",azSlewingTime.hour(),azSlewingTime.minute(),azSlewingTime.second());
    printf("ElSlewingTime %d:%d:%d\n",elSlewingTime.hour(),elSlewingTime.minute(),elSlewingTime.second());
    printf("\n");*/
    
    
    if ((azSlewingTime > deltaTime.value()) || (elSlewingTime > deltaTime.value())) {
		if (!asap) {
			//error: antenna will not reach the start position within desired time
			_EXCPT(AntennaErrors::ExceedingSlewingTimeExImpl,err,"SubScan::ScanComputer");
			throw err;
		}
	    // asap = true, iterations continue to get a feasible startUT
		reachOk=false;
	}
	else {
	    // The slewing time is estimated to be compatible with the commanded deltaTime
		reachOk=true;
	}
	sect=startSector;
}

/* Given a UT instant this method checks in which section of the path the antenna
 * must be, and then computes the coordinates to be commanded for that time. 
 */
void SubScan::computePointingForUT (const TIMEVALUE& time){
	
	double pointLon, pointLat;
	TIMEVALUE utTime;
	IRA::CIRATools::timeCopy(utTime,time);
	TIMEDIFFERENCE deltaT;
	double deltaTSec, deltaTSq;
	
	
	if (m_geometry==Antenna::SUBSCAN_GREATCIRCLE){
		
		if (utTime < m_rampStartTime.value()){
			//commanding the antenna to the acceleration ramp start position
			coordConvert(m_rampStartLon, m_rampStartLat, m_subScanFrame, m_rampStartTime,
					m_dut1, site);
		}
		else if ((utTime >= m_rampStartTime.value())&&(utTime < m_startUT.value())){
			//running along the acceleration ramp
			deltaT.value(utTime.difference(m_rampStartTime.value()));
			deltaTSec=IRA::CIRATools::timeMicroSeconds(deltaT)*0.000001;
			deltaTSq=pow(double(deltaTSec),2.0);
			pointLon=m_greatCircle.getGreatCirclePoint(-m_rampSpan+0.5*m_rampPhiAcceleration*deltaTSq).l;
			pointLat=m_greatCircle.getGreatCirclePoint(-m_rampSpan+0.5*m_rampPhiAcceleration*deltaTSq).b;
			coordConvert(pointLon, pointLat, m_subScanFrame, utTime, 
					     m_dut1, site);
		}
		else if ((utTime >= m_startUT.value())&&(utTime < m_stopUT.value())){
			//scanning at constant speed
			deltaT.value(utTime.difference(m_startUT.value()));
			deltaTSec=IRA::CIRATools::timeMicroSeconds(deltaT)*0.000001;
			pointLon=m_greatCircle.getGreatCirclePoint(m_phiRate*deltaTSec).l;
			pointLat=m_greatCircle.getGreatCirclePoint(m_phiRate*deltaTSec).b;
			coordConvert(pointLon, pointLat, m_subScanFrame, utTime, 
					     m_dut1, site);
		}
		else if ((utTime >= m_stopUT.value())&&(utTime < m_rampStopTime.value())){
			//running along the deceleration ramp 
			deltaT.value(utTime.difference(m_stopUT.value()));
			deltaTSec=IRA::CIRATools::timeMicroSeconds(deltaT)*0.000001;
			deltaTSq=pow(double(deltaTSec),2.0);
			pointLon=m_greatCircle.getGreatCirclePoint(m_greatCircle.psi+m_phiRate*deltaTSec-0.5*m_rampPhiAcceleration*deltaTSq).l;
			pointLat=m_greatCircle.getGreatCirclePoint(m_greatCircle.psi+m_phiRate*deltaTSec-0.5*m_rampPhiAcceleration*deltaTSq).b;
			coordConvert(pointLon, pointLat, m_subScanFrame, utTime, 
					     m_dut1, site);
		}
		else if (utTime > m_rampStopTime.value()){
			//commanding the antenna to the deceleration ramp stop position
			coordConvert(m_rampStopLon, m_rampStopLat, m_subScanFrame, m_rampStopTime, 
					     m_dut1, site);
		}
	}
	else{
	
	    if (m_coordFrame==m_subScanFrame){	
	    	if (utTime < m_rampStartTime.value()){
	    		//commanding the antenna to the acceleration ramp start position
	    		coordConvert(m_rampStartLon, m_rampStartLat, m_subScanFrame, m_rampStartTime, 
	    				m_dut1, site);
	    	}
	    	else if ((utTime >= m_rampStartTime.value())&&(utTime < m_startUT.value())){
	    		//running along the acceleration ramp
	    		deltaT.value(utTime.difference(m_rampStartTime.value()));
	    		deltaTSec=IRA::CIRATools::timeMicroSeconds(deltaT)*0.000001;
	    		deltaTSq=pow(double(deltaTSec),2.0);
	    		pointLon=m_rampStartLon + 0.5*m_rampLonAcceleration*deltaTSq;
	    		pointLat=m_rampStartLat + 0.5*m_rampLatAcceleration*deltaTSq;
	    		coordConvert(pointLon, pointLat, m_subScanFrame, utTime, 
	    					m_dut1, site);
	    	}
	    	else if ((utTime >= m_startUT.value())&&(utTime < m_stopUT.value())){
	    		//scanning at constant speed
	    		deltaT.value(utTime.difference(m_startUT.value()));
	    		deltaTSec=IRA::CIRATools::timeMicroSeconds(deltaT)*0.000001;
	    		pointLon=m_startLon+m_lonRate*deltaTSec;
	    		pointLat=m_startLat+m_latRate*deltaTSec;
	    		coordConvert(pointLon, pointLat, m_subScanFrame, utTime, 
	    					m_dut1, site);
	    	}
	    	else if ((utTime >= m_stopUT.value())&&(utTime < m_rampStopTime.value())){
	    		//running along the deceleration ramp 
	    		deltaT.value(utTime.difference(m_stopUT.value()));
	    		deltaTSec=IRA::CIRATools::timeMicroSeconds(deltaT)*0.000001;
	    		deltaTSq=pow(double(deltaTSec),2.0);
	    		pointLon=m_stopLon + m_lonRate*deltaTSec - 0.5*m_rampLonAcceleration*deltaTSq;
	    		pointLat=m_stopLat + m_latRate*deltaTSec - 0.5*m_rampLatAcceleration*deltaTSq;
	    		coordConvert(pointLon, pointLat, m_subScanFrame, utTime, 
	    					m_dut1, site);
	    	}
	    	else if (utTime > m_rampStopTime.value()){
	    		//commanding the antenna to the deceleration ramp stop position
	    		coordConvert(m_rampStopLon, m_rampStopLat, m_subScanFrame, m_rampStopTime, 
	    					m_dut1, site);
	    	}
	    }
	    else{
	    	//This is a pointing calibration scan
	    	computePointingScan(time);
	    }
	}     
}

/* Given the present time and the position of the antenna, plus the value
 * of the Half Power Beam Width (in radians), this method
 * calls computePointingForUT and checks if the actual pointing coincides with the
 * foreseen one - within an error of 0.1 HPBW. 
 * If so, it sets attribute "tracking" to 1. 
 * If the actual position differs from the predicted one beyond 0.1 HPBW, 
 * the attribute is set to 0.  
 * */ 
void SubScan::checkPointingForUT (const TIMEVALUE& time, const double& actAz,
					const double& actEl, const double& HPBW, long& tracking) {
	
	double az1;
	double el1;
	double az2;
	double el2;
	if (m_isPointingScan) { //this is a pointing scan.....so ramp coords are given in subscan frame (Horizontal)
		coordConvert(m_startLon, m_startLat, m_subScanFrame, m_startUT, m_dut1, site);
		az1=az;
		el1=el;
		coordConvert(m_stopLon, m_stopLat, m_subScanFrame, m_stopUT, m_dut1, site);
		az2=az;
		el2=el;
	}
	else {
		coordConvert(m_startLon, m_startLat, m_coordFrame, m_startUT, m_dut1, site);
		az1=az;
		el1=el;
		coordConvert(m_stopLon, m_stopLat, m_coordFrame, m_stopUT, m_dut1, site);
		az2=az;
		el2=el;
	}
	double azMin=GETMIN(az1,az2);
	double azMax=GETMAX(az1,az2);
	double elMin=GETMIN(el1,el2);
	double elMax=GETMAX(el1,el2);
	// checking if the present position is within the constant speed scan
	if ( ((actAz>=(azMin-0.1*HPBW))&&(actAz<=(azMax+0.1*HPBW))) && ((actEl>=(elMin-0.1*HPBW))&&(actEl<=(elMax+0.1*HPBW))) ) {
		//computing the pointing error and assessing if it's lower than 0.1*HPBW
		computePointingForUT(time);
		double deltaAz = (az - actAz)*cos(actEl);
		double deltaEl = el - actEl;
		double pointingError = sqrt((deltaAz*deltaAz)+(deltaEl*deltaEl));
		if (pointingError < 0.1*HPBW){
			// antenna is performing the scan and error is < 0.1*HPBW
			tracking = 1;
		}
		else{
			// Error exceeds 0.1 HPBW
			tracking = 0;
		}
	} else {  
		// antenna is out of the constant speed path
		tracking = 0;
	}
}


/** 
 * Method returns an integer value indicating if:
 * 1 - the antenna is running along the constant speed path
 * 0 - the antenna is running outside the constant speed path
 **/
void SubScan::checkPathSection (const TIMEVALUE& time, const double& actAz,
		const double& actEl, long& flag){
	// Checking if position is within start and stop points
		coordConvert(m_startLon, m_startLat, m_coordFrame, m_startUT, m_dut1, site);
		double az1=az;
		double el1=el;
		coordConvert(m_stopLon, m_stopLat, m_coordFrame, m_stopUT, m_dut1, site);
		double az2=az;
		double el2=el;
		double azMin=GETMIN(az1,az2);
		double azMax=GETMAX(az1,az2);
		double elMin=GETMIN(el1,el2);
		double elMax=GETMAX(el1,el2);
		if (((actAz>=azMin)&&(actAz<=azMax))&&((actEl>=elMin)&&(actEl<=elMax)))
		{
			// The antenna is performing the scan
			flag=1;
		}
		else{ 
			// The antenna is outside the constant speed scan
			flag=0;
		}
}


/* Method fills all the attributes in order to make them
 * readable by the OTF component */
void SubScan::fillAllAttributes (Antenna::OTFAttributes* att) {
	
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	computePointingForUT(now);
	//att->sourceID=CORBA::string_dup("OTF");  it will be set by implementation class as it is just a place holder
	att->rightAscension=appRa;
	att->declination=appDec;
	att->azimuth=slaDranrm(az);
	att->elevation=el;
	att->julianEpoch=epoch;
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
	att->parallacticAngle=pAngle;
	att->J2000RightAscension=ra;
	att->J2000Declination=decl;
	att->startLon=m_startLon;
	att->startLat=m_startLat;
	att->stopLon=m_stopLon;
	att->stopLat=m_stopLat;
	if (m_isPointingScan) {
		att->centerLon=m_targetAz;
		att->centerLat=m_targetEl;
	}
	else {
		att->centerLon=m_centerLon;
		att->centerLat=m_centerLat;
	}
	att->centerRA=m_centerRA;
	att->centerDec=m_centerDec;
	att->centerGLon=m_centerGLon;
	att->centerGLat=m_centerGLat;
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
	att->gLongitude=glon;
	att->gLatitude=glat;
}

// This fills the horizontal coordinates attributes only
void SubScan::fillAzEl (double& outAz,double& outEl) const {
	outAz=slaDranrm(az);
	outEl=el;
}


/* Method convertd a given pair of coordinates in all the other
 * supported frames*/
void SubScan::coordConvert(const double& lon, const double& lat, 
    		const Antenna::TCoordinateFrame& coordFrame, const TIMEVALUE& UT, 
    		const double& dut1, const IRA::CSite& site){

	IRA::CDateTime time(const_cast<TIMEVALUE&>(UT), dut1);
	epoch=time.getJulianEpoch();
	
		if (coordFrame==Antenna::ANT_EQUATORIAL){
		ra=lon;
		decl=lat;
		IRA::CSkySource source;
		source.setInputEquatorial(ra, decl, IRA::CSkySource::SS_J2000);
		source.process(time, site);
		source.getApparentEquatorial(appRa, appDec, epoch);
		source.getApparentHorizontal(az,el);
		source.getInputGalactic(glon,glat);
		pAngle=source.getParallacticAngle();
		}
	
	    if (coordFrame==Antenna::ANT_GALACTIC){
		glon=lon;
		glat=lat;
		IRA::CSkySource source;
		source.setInputGalactic(glon, glat);
		source.process(time, site);
		source.getApparentHorizontal(az,el);
		source.getApparentEquatorial(appRa, appDec, epoch);
		double dra, ddec, parallax, rvel;
		source.getInputEquatorial(ra, decl, epoch, dra, ddec, parallax, rvel);
		pAngle=source.getParallacticAngle();
	    }

	    if (coordFrame==Antenna::ANT_HORIZONTAL){
		az=lon;
		el=lat;
		IRA::CSkySource::horizontalToEquatorial(time, site, az, el, appRa, appDec, pAngle);
		IRA::CSkySource::apparentToJ2000(appRa, appDec, time, ra, decl);
		IRA::CSkySource::equatorialToGalactic(ra, decl, glon, glat);
	    }
}

/*
 * Method computes the start and stop positions for the peculiar
 * case of "pointing calibration scans", i.e. subscans performed on a sidereal 
 * position but scanning along the Azimuth or Elevation axis.
 * The sidereal position must be reached at half subscan.
 */
void SubScan::setPointingScan() throw (AntennaErrors::RateTooHighExImpl)
{
	TIMEDIFFERENCE halfDuration(m_subScanDuration.value());
	halfDuration.divide(2);
	//printf("Half Scan Duration: %lf\n",(double)halfDuration.value().value);
	IRA::CIRATools::timeCopy(m_midUT,m_startUT);
	m_midUT.add(halfDuration.value());
	//printf("Central Time: %lf\n",(double)m_centralTime.value().value);
	m_centerRA=m_centerLon;
	m_centerDec=m_centerLat;
	coordConvert(m_centerLon, m_centerLat, m_coordFrame, m_midUT, m_dut1, site);
	m_targetEl=el+m_latoff;
	m_targetAz=az+m_lonoff/cos(m_targetEl);
	m_centerGLon=glon;
	m_centerGLat=glat;
	//printf("Target AZ at mid-scan: %lf\n",m_targetAz);
	//printf("Target EL at mid-scan: %lf\n",m_targetEl);
	if (m_geometry==Antenna::SUBSCAN_CONSTLAT){
		m_startLat=m_targetEl;
		m_stopLat=m_targetEl;
		//printf("Start LAT: %lf\n",m_startLat);
		//printf("Stop LAT: %lf\n",m_stopLat);
		if (m_direction==Antenna::SUBSCAN_INCREASE){
			m_startLon=m_targetAz-(m_lonSpan/(2.0*cos(m_targetEl)));
			m_stopLon=m_targetAz+(m_lonSpan/(2.0*cos(m_targetEl)));
			//printf("Start LON: %lf\n",m_startLon);
			//printf("Stop LON: %lf\n",m_stopLon);
		}else{
			m_startLon=m_targetAz+(m_lonSpan/(2.0*cos(m_targetEl)));
			m_stopLon=m_targetAz-(m_lonSpan/(2.0*cos(m_targetEl)));
			//printf("Start LON: %lf\n",m_startLon);
			//printf("Stop LON: %lf\n",m_stopLon);
		}
	}else if (m_geometry==Antenna::SUBSCAN_CONSTLON){
		m_startLon=m_targetAz;
		m_stopLon=m_targetAz;
		//printf("Start LON: %lf\n",m_startLon);
		//printf("Stop LON: %lf\n",m_stopLon);
		if (m_direction==Antenna::SUBSCAN_INCREASE){
			m_startLat=m_targetEl-(m_latSpan/2.0);
			m_stopLat=m_targetEl+(m_latSpan/2.0);
			//printf("Start LAT: %lf\n",m_startLat);
			//printf("Stop LAT: %lf\n",m_stopLat);
		}else{
			m_startLat=m_targetEl+(m_latSpan/2.0);
			m_stopLat=m_targetEl-(m_latSpan/2.0);
			//printf("Start LAT: %lf\n",m_startLat);
			//printf("Stop LAT: %lf\n",m_stopLat);
		}
	} 
	if (fabs(m_stopLon-m_startLon)<DPI) {
		m_lonRate=(m_stopLon-m_startLon)/(IRA::CIRATools::timeMicroSeconds(m_subScanDuration)/1000000.0);
	} else {
		m_lonRate=(D2PI-fabs(m_stopLon-m_startLon))/(IRA::CIRATools::timeMicroSeconds(m_subScanDuration)/1000000.0);
	    if (m_direction==Antenna::SUBSCAN_DECREASE) {
	    	m_lonRate= - m_lonRate;
	    }
	}
	m_latRate=((m_stopLat-m_startLat)/(IRA::CIRATools::timeMicroSeconds(m_subScanDuration)/1000000.0));
	m_skyRate=m_lonRate*cos(m_targetEl)+m_latRate; // as one of the two is surely zero
	
	//checking if the Az-El rates stay within ranges
	if (fabs(m_lonRate)>=m_maxAzimuthRate) {
		_EXCPT(AntennaErrors::RateTooHighExImpl,err,"SubScan::setPointingScan"); 
		err.setAxis("Azimuth");
		throw err;
	}
	else if (fabs(m_lonRate)>=m_maxReliableAzimuthRate){
		ACS_LOG(LM_FULL_INFO, "SubScan::setPointingScan", (LM_WARNING,"AZ_RATE_BEYOND_RECOMMENDED_LIMIT"));
	}
	if (fabs(m_latRate)>=m_maxElevationRate) {
		_EXCPT(AntennaErrors::RateTooHighExImpl,err,"SubScan::setPointingScan"); 
		err.setAxis("Elevation");
		throw err;
	}
	else if (fabs(m_latRate)>=m_maxReliableElevationRate){
		ACS_LOG(LM_FULL_INFO, "SubScan::setPointingScan()", (LM_WARNING,"EL_RATE_BEYOND_RECOMMENDED_LIMIT"));
	}
	
// Ramps computation
	if 	(m_geometry==Antenna::SUBSCAN_CONSTLON) {
		if (m_direction==Antenna::SUBSCAN_INCREASE) {
			m_rampLatAcceleration = m_maxAcc/m_accScaleFactor;
		}
		else if (m_direction==Antenna::SUBSCAN_DECREASE){
			m_rampLatAcceleration = -m_maxAcc/m_accScaleFactor;
		}

		m_rampLonAcceleration = 0.0;
		m_rampDuration.value(ACS::TimeInterval((m_latRate/m_rampLatAcceleration)*10000000.0));
		// acceleration ramp before data acquisition
		m_rampStartTime.value(m_startUT.value());
		m_rampStartTime.subtract(m_rampDuration.value());
		m_rampStartLon=m_startLon;
		m_rampStartLat=m_startLat-(0.5*m_rampLatAcceleration*(pow((double(IRA::CIRATools::timeMicroSeconds(m_rampDuration))*0.000001),2.0)));
		//deceleration ramp after data acquisition
		m_rampStopTime.value(m_stopUT.value());
		m_rampStopTime.add(m_rampDuration.value());
		m_rampStopLon=m_stopLon;
		m_rampStopLat=m_stopLat+(0.5*m_rampLatAcceleration*(pow((double(IRA::CIRATools::timeMicroSeconds(m_rampDuration))*0.000001),2.0)));
	}
	else if (m_geometry==Antenna::SUBSCAN_CONSTLAT) {
		if (m_direction==Antenna::SUBSCAN_INCREASE) {
			m_rampLonAcceleration = m_maxAcc/m_accScaleFactor;
		}
		else if (m_direction==Antenna::SUBSCAN_DECREASE){
			m_rampLonAcceleration = -m_maxAcc/m_accScaleFactor;
		}
		m_rampLatAcceleration = 0.0;
		m_rampDuration.value(ACS::TimeInterval((m_lonRate/m_rampLonAcceleration)*10000000.0));
		//printf("Ramp Duration: %lf\n",(double)m_rampDuration.value().value);
		// acceleration before data acquisition
		m_rampStartTime.value(m_startUT.value());
		m_rampStartTime.subtract(m_rampDuration.value());
		m_rampStartLon=m_startLon-(0.5*m_rampLonAcceleration*(pow((double(IRA::CIRATools::timeMicroSeconds(m_rampDuration))*0.000001),2.0)));
		m_rampStartLat=m_startLat;
		// deceleration after data acquisition
		m_rampStopTime.value(m_stopUT.value());
		m_rampStopTime.add(m_rampDuration.value());
		m_rampStopLat=m_stopLat;
		m_rampStopLon=m_stopLon+(0.5*m_rampLonAcceleration*(pow((double(IRA::CIRATools::timeMicroSeconds(m_rampDuration))*0.000001),2.0)));
	}
}

/* Method computes the Az,El position to be commanded to the antenna
 * for the given time, if a "pointing calibration scan" is being
 * performed.*/
void SubScan::computePointingScan(const TIMEVALUE& time){
	TIMEVALUE utTime;
	IRA::CIRATools::timeCopy(utTime,time);
	TIMEDIFFERENCE deltaT;
	double deltaTSec, deltaTSq;
    Antenna::TCoordinateFrame tmpCoordFrame;
    tmpCoordFrame=Antenna::ANT_HORIZONTAL;
    double tmpLon, tmpLat;
		
	if (utTime < m_rampStartTime.value()){
		//commanding the antenna to the acceleration ramp start position
	   	tmpLon=m_rampStartLon;
	   	tmpLat=m_rampStartLat;
	   	coordConvert(tmpLon, tmpLat, tmpCoordFrame, m_rampStartTime,
	   				m_dut1, site);
	}
	else if ((utTime >= m_rampStartTime.value())&&(utTime < m_startUT.value())){
	   	//running along the acceleration ramp
	   	deltaT.value(utTime.difference(m_rampStartTime.value()));
	   	deltaTSec=IRA::CIRATools::timeMicroSeconds(deltaT)*0.000001;
	   	deltaTSq=pow(double(deltaTSec),2.0);
	   	tmpLon=m_rampStartLon + 0.5*m_rampLonAcceleration*deltaTSq;
	   	tmpLat=m_rampStartLat + 0.5*m_rampLatAcceleration*deltaTSq;
	   	coordConvert(tmpLon, tmpLat, tmpCoordFrame, utTime,
	   				m_dut1, site);
   	}
   	else if ((utTime >= m_startUT.value())&&(utTime < m_stopUT.value())){
    	//scanning at constant speed
    	deltaT.value(utTime.difference(m_startUT.value()));
    	deltaTSec=IRA::CIRATools::timeMicroSeconds(deltaT)*0.000001;
    	tmpLon=m_startLon+(m_lonRate*deltaTSec);
    	tmpLat=m_startLat+(m_latRate*deltaTSec);
    	coordConvert(tmpLon, tmpLat, tmpCoordFrame, utTime,
    						m_dut1, site);
    }
	else if ((utTime >= m_stopUT.value())&&(utTime < m_rampStopTime.value())){
	   	//running along the deceleration ramp 
	   	deltaT.value(utTime.difference(m_stopUT.value()));
	   	deltaTSec=IRA::CIRATools::timeMicroSeconds(deltaT)*0.000001;
	   	deltaTSq=pow(double(deltaTSec),2.0);
	   	tmpLon=m_stopLon + m_lonRate*deltaTSec - 0.5*m_rampLonAcceleration*deltaTSq;
	   	tmpLat=m_stopLat + m_latRate*deltaTSec - 0.5*m_rampLatAcceleration*deltaTSq;
    	coordConvert(tmpLon, tmpLat, tmpCoordFrame, utTime,
    						m_dut1, site);
   	}
	else if (utTime >= m_rampStopTime.value()){
	   	//commanding the antenna to the deceleration ramp stop position
		tmpLon=m_rampStopLon;
		tmpLat=m_rampStopLat;
    	coordConvert(tmpLon, tmpLat, tmpCoordFrame, m_rampStopTime,
    						m_dut1, site);
   	}
}

void SubScan::setOffsets(const double& lonoff, const double& latoff, Antenna::TCoordinateFrame& offFrame) {
	m_lonoff=lonoff;
	m_latoff=latoff;
	m_offFrame=offFrame;
}





