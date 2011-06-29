#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

/* ************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                                             */
/* $Id: Configuration.h,v 1.10 2011-05-20 16:52:46 a.orlati Exp $                                                                                                                                     */
/*                                                                                                                                                */
/* This code is under GNU General Public Licence (GPL).                                                         */
/*                                                                                                                                                 */
/* Who                                                    When                 What                                                  */
/* Andrea Orlati(aorlati@ira.inaf.it)          24/01/2008         Creation                                             */

#include <IRA>
#include <maciContainerServices.h>
#include <ComponentErrors.h>

using namespace IRA;

/**
 * This class implements the component configurator. The data inside this class are initialized at the startup from the
 * configuration database and then are used (read) inside the component.
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
  */
class CConfiguration {
public:	
	/**
	 * Constructor 
	*/
	CConfiguration();
	
	/**
	 * Destructor
	*/
	~CConfiguration();
	/**
    * This member function is used to configure component by reading the configuration parameter from the CDB.
	 * This must be the first call before using any other function of this class.
	 * @throwCDBAccess
	 * @param Services pointer to the container services object
	*/
	void init(maci::ContainerServices *Services) throw (ComponentErrors::CDBAccessExImpl);
	
	/**
	 * @return the name of the instance of the mount component
	*/
	const CString& getMountInstance() const { return m_mountInstance; }
	
	/**
	 * @return the name of the instance for the observatory component 
	*/
	const CString& getObservatoryComponent() const { return m_observatoryComp; }
	
	/**
	 * @return the name of the instance for the pointing model component
	*/
	const CString& getPointingModelInstance() const { return m_pointingInstance; }
	
	/**
	 * @return the name of the instance for the refraction component 
	*/
	const CString& getRefractionInstance() const { return m_refractionInstance; }
	
	/**
	 * @return the number of 100ns that the working thread sleeps between 2 executions
	*/
	long getWorkingThreadTime() const { return m_workingThreadTime*10; } // *10 because the variable is in microseonds while 100ns are required

	/**
	 * @return the number of 100ns that the working thread sleeps between 2 executions
	*/
	long getWatchingThreadTime() const { return m_watchingThreadTime*10; } // *10 because the variable is in microseonds while 100ns are required
	
	/**
	 * @return the number of microseconds that the log filter will cache a log message
	*/
	long getRepetitionCacheTime() const { return m_repetitionCacheTime; }
	
	/**
	 * @return the number of microseconds that the log filter will take as expiration time for a log message
	*/
	long getRepetitionExpireTime() const { return m_repetitionExpireTime; }

	/**
	 * @return the minimun number of points that must be loaded into the mount 
	*/
	long getMinPointNumber() const { return m_minPointNumber; }
	
	/**
	 * @return the maximum number of points that must be loaded into the mount 
	*/	
	long getMaxPointNumber() const { return m_maxPointNumber; }
	
	/**
	 * @return the gap (microseconds) between two consecutive points in the tracking trajectory
	*/		
	long getGapTime() const { return m_gapTime; }
	
	/**
	 * @return the interface of the dynamic component that have to used to compute On The Fly scans
	*/
	const CString& getOTFInterface() const { return m_otfInterface; }

	/**
	 * @return the interface of the dynamic component that have to used to compute sidereal tracking
	*/
	const CString& getSiderealInterface() const { return m_siderealInterface; }
	
	/**
	 * @return the interface of the dynamic component that have to used to compute moon tracking
	*/
	const CString& getMoonInterface() const { return m_moonInterface; }
	
	/**
	 * @return the interface of the dynamic component that have to used to compute sun tracking
	*/	
	const CString& getSunInterface() const { return m_sunInterface; }
	
	/**
	 * @return the interface of the dynamic component that have to used to compute tracking of Solar System bodies
	*/
	const CString& getSolarSystemBodyInterface() const { return m_solarSystemBodyInterface; }
	
	/**
	 * @return the interface of the dynamic component that have to used to compute satellite tracking
	*/
	const CString& getSatelliteInterface() const { return m_satelliteInterface; }
	
	/**
	 * @return  the maximum speed in azimuth (rad/sec)
	*/
	const double& getMaxAzimuthRate() const { return m_maxAzimuthRate; }
	
	/**
	 * @return  the maximum speed in elevation (rad/sec)
	*/
	const double& getMaxElevationRate() const { return m_maxElevationRate; }

	/**
	 * @return the upper elevation limit(rad)
	*/
	const double& getMaxElevation() const { return m_maxElevation; }

	/**
	 * @return the lower elevation limit(rad)
	*/
	const double& getMinElevation() const { return m_minElevation; }
	
	/**
	 * @return the telescope diameter in meters
	 */
	const double& getDiameter() const { return m_diameter; }
	
	/**
	 * @return the integration period (100ns) for the coordinates done by the component 
	 */  
	const long& getCoordinateIntegrationPeriod() const { return m_coordinateIntegration; }
	
	/**
	 * @return the maximum acceleration in azimuth (rad/sec^2)
	*/	
	const double& getMaxAzimuthAcceleration() const { return m_maxAzimuthAccelaration; }
	
	/**
	 * @return the maximum acceleration in elevation (rad/sec^2)
	*/
	const double& getMaxElevationAcceleration() const { return m_maxElevationAcceleration; }
	
	/**
	 * @return the cutoff elevation in radians, used to decide is going off source in azimuth or elevation
	*/
	const double& getCutOffElevation() const { return m_cutOffElevation; }
	
private:
	CString m_mountInstance;
	CString m_observatoryComp;
	CString m_pointingInstance;
	CString m_refractionInstance;
	CString m_otfInterface;
	CString m_siderealInterface;
	CString m_moonInterface;
	CString m_sunInterface;
	CString m_solarSystemBodyInterface;
	CString m_satelliteInterface;
	long m_watchingThreadTime;
	long m_workingThreadTime;
	long m_repetitionCacheTime;
	long m_repetitionExpireTime;
	long m_minPointNumber;
	long m_maxPointNumber;
	long m_gapTime;
	double m_maxAzimuthRate;
	double m_maxElevationRate;
	double m_maxElevation;
	double m_minElevation;
	double m_maxAzimuthAccelaration;
	double m_maxElevationAcceleration;
	double m_diameter;
	double m_cutOffElevation;
	long m_coordinateIntegration;
};

#endif /*CONFIGURATION_H_*/
