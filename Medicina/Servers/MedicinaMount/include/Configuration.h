#ifndef CCONFIGURATION_H
#define CCONFIGURATION_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: Configuration.h,v 1.7 2010-10-14 12:08:28 a.orlati Exp $		    									     */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  01/09/2005      Creation                                         */
/* Andrea Orlati(aorlati@ira.inaf.it)  10/11/2006      Added the m_servoSystemStatusValidity member     */
/* Andrea Orlati(aorlati@ira.inaf.it)  14/11/2006      Added the m_sectionValidity and m_freeProgramTrackPositionValidity members */
/* Andrea Orlati(aorlati@ira.inaf.it)  30/11/2006      Added the m_azimuthRateInfo and m_ElevationRateInfo members */
/* Andrea Orlati(aorlati@ira.inaf.it)  01/02/2008      Removed the read of validity time since it not used anymore */
/* Andrea Orlati(aorlati@ira.inaf.it) 04/06/2008       Elev and Az limits and speed are now read from a specific table insted of from */ 
/*                                                                                      the attributes of elevation and azimuth component properties */
/* Andrea Orlati(aorlati@ira.inaf.it) 07/07/2008      ccw cw limit read from CDB */
/* Andrea Orlati(aorlati@ira.inaf.it) 18/07/2008      Removed field trackingPrecision */

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
	/** Gets the azimuth lower limit */
	inline double azimuthLowerLimit() const { return m_azimuthInfo.lowerLimit; }
	
	/** Gets the azimuth upper limit */
	inline double azimuthUpperLimit() const { return m_azimuthInfo.upperLimit; }
	
	/** Gets the elevation lower limit */
	inline double elevationLowerLimit() const { return m_elevationInfo.lowerLimit; }
	
	/** Gets the elevation upper limit */
	inline double elevationUpperLimit() const { return m_elevationInfo.upperLimit; }
	
	/** Gets the azimuth rate lower limit */
	inline double azimuthRateLowerLimit() const { return m_azimuthRateInfo.lowerLimit; }
	
	/** Gets the azimuth rate upper limit */
	inline double azimuthRateUpperLimit() const { return m_azimuthRateInfo.upperLimit; }
	
	/** Gets the elevation rate lower limit */
	inline double elevationRateLowerLimit() const { return m_elevationRateInfo.lowerLimit; }
	
	/** Gets the elevation rate upper limit */
	inline double elevationRateUpperLimit() const { return m_elevationRateInfo.upperLimit; }
	
	/** Gets ACU ip address */
	inline CString ipAddress() const { return m_sACUAddress; }
		
	/** Gets the socket port to which the ACU is listening */
	inline WORD ACUPort() const { return m_wACUPort; }
		
	/** Gets the connection timeout, which must be used when trying to connect to the ACU*/
	inline DWORD connectionTimeout() const { return m_dwconnectTimeout; }
	
	/** Get the receive timeout, which is used when communicating with the ACU */
	inline DWORD receiveTimeout() const { return m_dwreceiveTimeout; }
		
	/** Gets the control thread period between two executions it is given in 100ns units */
	inline DWORD controlThreadPeriod() const { return m_dwcontrolThreadPeriod; }
		
	/** Gets the antenna pointing precision */
	//inline double trackingPrecision() const { return m_dtrackingPrecision; }
	
	/** Gets the cache time for the logging repetition filter */
	inline DDWORD repetitionCacheTime() const { return m_dwrepetitionCacheTime; }

	/** Gets the expire time for the logging repetition filter */
	inline DDWORD expireCacheTime() const { return m_dwexpireCacheTime; }	
	
	/** Gets the time between two consecutive refreshes of the properties */
	inline DDWORD propertyRefreshTime() const { return m_dwpropertyRefreshTime; }
	
	/** Gets the limit between counter clockwise clockwise sectors */
	inline double cwLimit() const { return m_cwLimit; }
	
	/** Gets the oscillation threshold as azimuth tracking error in degrees */
	inline double oscillationThreshold() const { return m_doscThreashold; }
	
	/** Gets the duration of the oscillation alarm when it is first triggered (100 ns unit)*/
	inline DDWORD oscillationAlarmDuration() const { return m_dwoscAlarmDuration; }

	/** Gets the time to wait before recovery from a stop caused by an oscillation(100 ns unit)*/
	inline DDWORD oscillationRecoveryTime() const { return m_dwoscRecoverTime; }
	
	/** Gets the number of times the oscillation has to be hit before an oscillation is declared*/
	inline DWORD oscillationNumberThreashold() const { return m_dwoscNumberThreshold; }
	
	/**
    * This member function is used to configure component by reading the configuration parameter from the CDB.
	 * This must be the first call before using any other function of this class.
	 * @throw ComponentErrors::CDBAccess
	 * @param Services pointer to the container services object
	*/
	void Init(maci::ContainerServices *Services) throw (ComponentErrors::CDBAccessExImpl);
private:
	/** used to store extra information about a coordinate */
	typedef struct {
		double lowerLimit;   /*!< Coordinate maximum value */
		double upperLimit;   /*!< Coordinate minimum value */
		DWORD validity;      /*!< This value specifies how long the last coordinate read could be considered valid */
	} TCoordAttributes;
	/** the refresh time of the component properties */
	DWORD m_dwpropertyRefreshTime; 
	/** ACU ip address */	
	CString m_sACUAddress;
	/** ACU listening port */
	WORD m_wACUPort;
	/** Information related to azimuth */
	TCoordAttributes m_azimuthInfo;
	/** Information related to elevation */
	TCoordAttributes m_elevationInfo;
	/** validity time of azimuth error */
	/** information about the azimuth rate */
	TCoordAttributes m_azimuthRateInfo;
	/** information about the azimuth rate */	
	TCoordAttributes m_elevationRateInfo;
	/** Connection timeout */
	DWORD m_dwconnectTimeout;
	/** Receive Timeout */
	DWORD m_dwreceiveTimeout;
	/** This is the period of the control Thread (100ns unit) */
	DWORD m_dwcontrolThreadPeriod;
	/** This is the precision (degree) of the antenna */
	//double m_dtrackingPrecision; 
	/** This indicates the cache time for the repetion log filter */
	DDWORD m_dwrepetitionCacheTime;
	/** This indicates the expire time for the repetion log filter */
	DDWORD m_dwexpireCacheTime;
	/** this number is the separation value between the CW and CCW, if the azimuth is greater than this the section is CW */
	double m_cwLimit;
	
	double m_doscThreashold;
	DDWORD m_dwoscAlarmDuration;
	WORD m_dwoscNumberThreshold;
	DDWORD m_dwoscRecoverTime;
};


#endif
