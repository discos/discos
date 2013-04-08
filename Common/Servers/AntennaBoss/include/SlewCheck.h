/* ****************************************************************** */
/* IRA Istituto di Radioastronomia                                    */
/*                                                                    */
/* This code is under GNU General Public Licence (GPL)                */


#ifndef SLEWCHECK_H_
#define SLEWCHECK_H_

#include <Definitions.h>
#include <acstimeEpochHelper.h>
#include <IRA>
#include <AntennaDefinitionsS.h>
#include <Site.h>


/** 
 * Checks if the antenna is able to reach the commanded 
 * position within the given UT time.
 * 
*/
class CSlewCheck {
public:
	typedef enum {
		NOT_VISIBLE=0, // source is not visible
		AVOIDANCE=1, // source is in avoidance cone
		VISIBLE=2  // source can be observed
	} TCheckResult;

	CSlewCheck();
	
	virtual ~CSlewCheck();

	/**
	 *  Method initialises the site information - longitude, latitude, altitude -
	 * and the dut1 value
	 */
	void initSite(IRA::CSite& s,const double& d1);
	
	/**
	 *  Method initializes the mount slewing speeds, i.e. the velocities the
	 * antenna moves at  - in azimuth and elevation - when a new target position
	 * is commanded. Values are in rad, rad/s rad/s^2
	 */
	void initMount(const double& AzRate,const double& ElRate,const double& minEl,const double& maxEl,const double& maxAzAcc,const double& maxElAcc,const double& lowerEl,const double& upperEl);
	
	/**
	 * Given the object initializations, The current antenna position and
	 * the commanded coordinates+UT time, this method computes the necessary 
	 * slewing time to reach the target, determining if the antenna  
	 * can reach it in time. If so, it returns "true". 
	*/
	bool checkSlewing(const double& startAz,const double& startEl,const ACS::Time& initTime,const double& targetAz,const double targetEl,const ACS::Time& targetTime,ACS::TimeInterval& slewingTime);
	
	/**
	 * This function simply check if the target elevation is within the allowed limits.
	 * @param targetEl elevation coordinate than should be checked.
	 * @param minEl used to override the lower limit (rad) of the  avoidance area, if negative the area is the defined by defaults
	 * @param maxEl used to override the upper limit (rad) of the  avoidance area, if negative the area is the defined by defaults
	 * @return if the source is not visible or it is in the avoidance area or it could be observed regularly
	 */
	TCheckResult checkLimit(const double& targetEl,const double& minEl=-1.0,const double& maxEl=-1.0);
	
	/**
	 * Method converts the input longitude and latitude - which can be given in
	 * equatorial, horizontal or galactic frame - to the horizontal frame.
	 * Coordinates must be accompanied by a UT time.
	 * @param lon input longitude in radians
	 * @param lat input latitude in radians
	 * @param coordFrame reference frame
	 * @param equinox equinox if the reference frame is equatorial
	 * @param UT time which the confersion refers to
	 * @param az output azimuth in radians
	 * @param el output elevation in radians
	*/
	void coordConvert(const double& lon,const double& lat,const Antenna::TCoordinateFrame& coordFrame,const Antenna::TSystemEquinox& equinox,const TIMEVALUE& UT,double& az,double& el) const;
	
private:
	IRA::CSite m_site;
	double m_dut1; // difference between UTC and UT1 (seconds)
	double m_maxAzimuthRate; // maximum slewing speed in azimuth (radians/s)
	double m_maxElevationRate; // maximum slewing speed in elevation (radians/s)
	double m_minElevation; // min elevation limit in radians
	double m_maxElevation; // max elevation limit in radians
	double m_maxAzimuthAcceleration;
	double m_maxElevationAcceleration;
	double m_minSuggestedEl; // lower limit of avoidance area (radians)
	double m_maxSuggestedEl; // upper limit of avoidance area (radians)
	
	/**
	 * This function try to make a simple approximation of the slewing time, tham means the time required by the telescope to go from one point to an other given a cruise speed and an acceleration.
	 * The returned result is the sum of the cruise time plus the accelaration time. In that case the accelaration time is supposed to be the same as the deceleration time. The error is relatively higher
	 * for shorter distances and is maximum in the case that the distance is so short that the telescope cannot reach the cruise speed before slowing dow again.
	 * @param start initial position (it could be in any unit)
	 * @param stop final position (it could be in any unit)
	 * @param speed cruise velocity (it must reflect the unit of the positions per second)
	 * @param acc acceleration ((it must reflect the unit of the positions per square second)
	 * @return the total cruise time in 100 ns units.
	 */
	ACS::Time computeSlewingTime(const double& start,const double& stop,const double& speed,const double& acc);
};

#endif /*SLEWCHECK_H_*/
