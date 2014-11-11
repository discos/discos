/* ****************************************************************** */
/* IRA Istituto di Radioastronomia                                    */
/* $Id: SubScan.h,v 1.17 2011-06-21 16:39:14 a.orlati Exp $                                                             */
/*                                                                    */
/* This code is under GNU General Public Licence (GPL)                */
/*                                                                    */
/* Who                                  when            What          */
/* Simona Righini						28/03/2008      created       */
/* Simona Righini						21/04/2008      ramps added   */
/* Simona Righini						28/04/2008		methods added */
/* Simona Righini						15/07/2008		CW/CCW  added */
/* Simona Righini						30/07/2008		point cal. scan added */
/* Simona Righini						29/12/2008		checkPath added*/
/* Simona Righini						02/02/2009		bug fixing*/
/* Simona Righini						06/09/2010      offsets handling added*/
/* Simona Righini                       06/09/2010      0-azimuth bug fixing*/
/* Simona Righini                       08/04/2013      support for computation of ScanAxis*/

#ifndef SUBSCAN_H_
#define SUBSCAN_H_

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <OTFS.h>
#include <Definitions.h>
#include <acstimeEpochHelper.h>
#include <IRA>
//#include <baci.h>
#include "GreatCircle.h"
#include <Site.h>
#include <ComponentErrors.h>
#include <AntennaErrors.h>

/**
 * Class computes the subscan path, given the start/stop positions
 * - or the central position plus the subscan span - and the subscan duration.
 * Available reference frames: Equatorial (J2000), Galactic and Horizontal.
 * Available scan geometries: along great circle (only for start/stop input
 * positions), along longitude or latitude of the selected reference frame. 
 * Only one "mixed mode" is allowed: az/el scans across a RA,Dec position 
 * (for pointing calibration); otherwise the coordinate reference frame must 
 * coincide with the scanning frame. 
 * @author <a href=mailto:righin_s@ira.inaf.it>Simona Righini</a>,
 * Istituto di Radioastronomia, Italy
 */

class SubScan {
public:
	SubScan();
	
	virtual ~SubScan();

	void initSite(const IRA::CSite& s);
	
	void initAzElRanges(const double& AzRate, const double& ElRate, 
			const double& rAzRate, const double& rElRate, const double& AzAcc, 
			const double& ElAcc, const double& AccScF);
	
	/**
	 * Method initializes subscan parameters. Given the input subscan geometry, 
	 * it computes the main parameters also for the other ones (e.g. computes
	 * start+stop positions from center+span and viceversa). 
	 * @throw ComponentErrors::ValidationErrorExImpl  
	 * @throw ComponentErrors::ValueOutofRangeExImpl 
	 * @param initAz Azimuth position of antenna at subscan setup time (radians)
	 * @param initEl Elevation position of antenna at subscan setup time (radians)
	 * @param initTime UT instant of subscan setup call (100 ns, absolute)
	 * @param lon1 If description is STARTSTOP: longitude of the subscan starting point, 
	 * if description is CENTER: longitude of subscan central point
	 * @param lat1 If description is STARTSTOP: latitude of the subscan starting point, 
	 * if description is CENTER: latitude of subscan central point
	 * @param lon2 If description is STARTSTOP: longitude of the subscan stopping point, 
	 * if description is CENTER: subscan full span in longitude
	 * @param lat2 If description is STARTSTOP: latitude of the subscan stopping point, 
	 * if description is CENTER: subscan full span in latitude
	 * @param coordFrame Reference frame for input coordinates
	 * @param geometry Geometry of subscan: CONSTLON, CONSTLAT or GREATCIRCLE
	 * @param subScanFrame Reference frame for subscan
	 * @param description Subscan definition (STARTSTOP or CENTER)
	 * @param direction Subscan direction (INCREASING or DECREASING, relative to what 
	 * happens to the varying coordinate when scanning in longitude or latitude)
	 * @param startUT Subscan start UT (absolute: 100ns since 1582-10-15 00:00:00)
	 * @param dut1 Difference between UTC and UT1 (seconds)
	 * @param subScanDuration Subscan duration (100ns)	
	 */
	Antenna::TSections initScan(
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
				AntennaErrors::ExceedingSlewingTimeExImpl);

	/**
	 * Iterative version of the previous method
	 */
	void ScanComputer(
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
				AntennaErrors::ExceedingSlewingTimeExImpl);

	/**
	 * Method computes the coordinates to be commanded to the antenna, as 
	 * a function of the UT time given in input. 
	 * It confronts the input time with the start/stop instants 
	 * predicted for the different scan phases, to determine which kind of 
	 * algorithm gives the wanted coordinates. 
	 **/
	void computePointingForUT (const TIMEVALUE& time);
	
	
	/**
	 *  Given the present time and the position of the antenna, plus the value
	 * of the Half Power Beam Width (in radians), this method
	 * calls computePointingForUT and checks if the actual pointing coincides with the
	 * foreseen one - within an error of 0.1 HPBW. 
	 * If so, it sets attribute "tracking" to 1. 
	 * If the actual position differs from the predicted one beyond 0.1 HPBW, 
	 * the attribute is set to 0.  
	 **/ 
	void checkPointingForUT (const TIMEVALUE& time, const double& actAz,
							const double& actEl, const double& HPBW, long& tracking);
	
	
	/** 
	 * Method returns an integer value indicating if:
	 * 1 - the antenna is running along the constant speed path
	 * 0 - the antenna is running outside the constant speed path
	 **/
	void checkPathSection (const TIMEVALUE& time, const double& actAz,
			const double& actEl, long& flag);
	
	
	/* Method fills all the attributes in order to make them
	 * readable by the OTF component */
	void fillAllAttributes (Antenna::OTFAttributes* att);
	
	// This fills the horizontal coordinates attributes only
	void fillAzEl (double& outAz,double& outEl) const;
	
	// fill the apparent coordinates in all supported frames
	void fillApparent(double& outAz,double& outEl,double& outRa,double& outDec,double& outJepoch,double& outLon,double& outLat);

	/* Method computes the Az,El position to be commanded to the antenna
	 * for the given time, if a "pointing calibration scan" is being
	 * performed.*/
	void computePointingScan(const TIMEVALUE& time);

	/* Method sets the value for longitude and latitude offsets.
	 * Longitude offset is given "on sky", and is corrected for cos(latitude) inside the SubScan computations.
	 */
	void setOffsets(const double& lonoff, const double& latoff, Antenna::TCoordinateFrame& offFrame);


private:

	TIMEVALUE m_initTime; // subscan initialization UT (absolute: 100ns since 1582-10-15 00:00:00)
	double m_startLon; //longitude of subscan starting point (radians)
	double m_stopLon; //longitude of subscan stopping point (radians)
	double m_startLat; //latitude of subscan starting point (radians)
	double m_stopLat; //latitude of subscan stopping point (radians)
	double m_centerLon; //longitude of subscan central point (radians)
	double m_centerLat; //latitude of subscan central point (radians)
	double m_centerRA; //J2000 RA of subscan central point (radians)
	double m_centerDec; //J2000 Dec of subscan central point (radians)
	double m_centerGLon; //Glon of subscan central point (radians)
	double m_centerGLat; //Glat of subscan central point (radians)
	double m_targetAz; //azimuth of sidereal source in pointing cal. subscan (radians)
	double m_targetEl; //elevation of sidereal source in pointing cal. subscan (radians)
	double m_lonSpan; //subscan extension in longitude (radians)
	double m_latSpan; //subscan extension in latitude (radians)
	double m_skySpan; //amplitude of subscan in sky (radians)
	double m_lonRate; //subscan speed in longitude (radians/s)
	double m_latRate; //subscan speed in latitude (radians/s)
	double m_skyRate; //subscan speed relative to sky (radians/s)
	Antenna::TsubScanGeometry m_geometry; // geometry of subscan
	Antenna::TCoordinateFrame m_coordFrame; // reference frame for input coordinates
	Antenna::TCoordinateFrame m_subScanFrame; // reference frame for subscan
	Antenna::TsubScanDescription m_description; // subscan definition (STARTSTOP or CENTER)
	Antenna::TsubScanDirection m_direction; // subscan direction (INCREASING or DECREASING)
	GreatCircle m_greatCircle; // great circle connecting the start and stop positions
	double m_subScanSpan; //amplitude of great circle arc connecting the two points
	TIMEVALUE m_startUT; // subscan start UT (absolute: 100ns since 1582-10-15 00:00:00)
    TIMEVALUE m_stopUT;  // subscan stop UT (absolute: 100ns since 1582-10-15 00:00:00)
    TIMEVALUE m_midUT;  // subscan middle point UT (absolute: 100ns since 1582-10-15 00:00:00)
    double m_dut1; // difference between UTC and UT1 (seconds)
    TIMEDIFFERENCE m_subScanDuration; // subscan duration (100ns)	
    double m_phiRate; // greatcircle subscan speed (radians/s)
    double m_rampLonAcceleration; // ramp longitude accel. (radians/s^2)
    double m_rampLatAcceleration; // ramp latitude accel. (radians/s^2)
    double m_rampPhiAcceleration;  // ramp greatcircle accel. (radians/s^2)
    double m_rampStartLon; // longitude of accel. ramp starting point (radians)
    double m_rampStartLat; // latitude of accel. ramp starting point (radians)
    double m_rampStopLon; // longitude of decel. ramp stopping point (radians)
    double m_rampStopLat; // latitude of decel. ramp stopping point (radians)
    double m_rampSpan; // extension of greatcircle ramp (radians)
    TIMEDIFFERENCE m_rampDuration; // duration of accel./decel. ramp (100ns)
    TIMEVALUE m_rampStartTime; // accel. ramp start UT (absolute: 100ns since 1582-10-15 00:00:00) 
    TIMEVALUE m_rampStopTime; // decel. ramp stop UT (absolute: 100ns since 1582-10-15 00:00:00)  
	double m_maxAzimuthRate; // max azimuth rate allowed for slewing (radians/s)
	double m_maxElevationRate; //max elevation rate allowed for slewing (radians/s)
	double m_maxReliableAzimuthRate; // max azimuth rate guaranteed for scanning (radians/s)
	double m_maxReliableElevationRate; // max elevation rate guaranteed for scanning (radians/s)
	double m_maxAzimuthAcceleration; // max azimuth acceleration allowed by mechanics (radians/s^2)
	double m_maxElevationAcceleration;// max elevation acceleration allowed by mechanics (radians/s^2)
	double m_accScaleFactor; // factor to be used to scale the acceleration and use it for ramps
	double m_maxAcc; // composition of azimuth and elevation accelerations (radians/s^2) 
	double m_lonoff; //longitude offset (radians)
	double m_latoff; //latitude offset (radians)
	Antenna::TCoordinateFrame m_offFrame; // reference frame for offsets
	bool m_isPointingScan;
	// scan center coordinates without any offset
	double m_origCenterRA;
	double m_origCenterDec;
	double m_origCenterGLon;
	double m_origCenterGLat;
	double m_origCenterAz;
	double m_origCenterEl;
	Management::TScanAxis m_scanType; //code for Scan Type

	
	/* Method computes the start and stop positions for the peculiar
	 * case of "pointing calibration scans", i.e. subscans performed on a sidereal
	 * position but scanning along the Azimuth or Elevation axis.
	 * The sidereal position must be reached at half subscan.*/
	void setPointingScan() throw (AntennaErrors::RateTooHighExImpl);

	/**
	 * Method converts the input longitude and latitude - which can be given in
	 * the J2000-equatorial, horizontal or galactic frame - to the other frames.
	 * Coordinates must be accompanied by a UT time, the dut1 value and the 
	 * information about the observing site, in order to perform the 
	 * conversion to/from the horizontal frame.  
	 * Method also computes the parallactic angle. 
	 */
    static void coordConvert(const double& lon, const double& lat, 
    		const Antenna::TCoordinateFrame& coordFrame, const TIMEVALUE& UT, 
    		const double& dut1, const IRA::CSite& site);

	/**
	 * Method considers the acceleration ramp start position and the
	 * deceleration ramp stop position and computes if the start point is
	 * in a CW or CCW sector.
	 * It also checks if the antenna is able to reach the acceleration ramp start position on time,
	 * considering its position at setup time and the maximum speed it can reach
	 **/
	void setSector (const TIMEVALUE& initTime, const double& initAz, const Antenna::TSections& initSector,
			const double& initEl,
			const double& rampStartLon, const double& rampStartLat, const TIMEVALUE& rampStartTime,
			const double& rampStopLon, const double& rampStopLat, const TIMEVALUE& rampStopTime,
			const Antenna::TCoordinateFrame& coordFrame, const double& dut1, const IRA::CSite& site,
			bool asap,Antenna::TSections &sect,bool& reachOk)
	        throw (AntennaErrors::ExceedingSlewingTimeExImpl);

};



#endif /*SUBSCAN_H_*/
