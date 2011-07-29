#ifndef CCONFIGURATION_H
#define CCONFIGURATION_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  30/09/2009     Creation                                         */
/* Andrea Orlati(aorlati@ira.inaf.it)  27/07/2011     added the acquisition of time source attribute   */

#include <IRA>
#include <maciContainerServices.h>
#include <ComponentErrors.h>

/**
 * This class implements the component configuration. The data inside this class are initialized at the startup from the
 * configuration database and then are used (read) inside the component.
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
  */
class CConfiguration {
public:
	/** Gets the azimuth lower limit */
	inline double azimuthLowerLimit() const { return m_azimuthLowerLimit; }
	
	/** Gets the azimuth upper limit */
	inline double azimuthUpperLimit() const { return m_azimuthUpperLimit; }
	
	/** Gets the elevation lower limit */
	inline double elevationLowerLimit() const { return m_elevationLowerLimit; }
	
	/** Gets the elevation upper limit deg/sec*/
	inline double elevationUpperLimit() const { return m_elevationUpperLimit; }
	
	/** Gets the azimuth rate lower limit deg/sec */
	inline double azimuthRateLowerLimit() const { return m_azimuthRateLowerLimit; }
	
	/** Gets the azimuth rate upper limit deg/sec */
	inline double azimuthRateUpperLimit() const { return m_azimuthRateUpperLimit; }
	
	/** Gets the elevation rate lower limit deg/sec */
	inline double elevationRateLowerLimit() const { return m_elevationRateLowerLimit; }
	
	/** Gets the elevation rate upper limit deg/sec */
	inline double elevationRateUpperLimit() const { return m_elevationRateUpperLimit; }
	
	/** Gets the limit between counter clockwise clockwise sectors */
	inline double cwLimit() const { return m_cwLimit; }
	
	/** Gets ACU ip address */
	inline IRA::CString ipAddress() const { return m_ACUAddress; }
		
	/** Gets the socket port to which the ACU is listening to commands */
	inline WORD commandPort() const { return m_commandPort; }
	
	/** Gets the socket port from which the ACU is sending the status frame */
	inline WORD statusPort() const { return m_statusPort; }
		
	/** Gets the connection timeout, which must be used when trying to connect to the ACU (microseconds)*/
	//inline DWORD connectionTimeout() const { return m_connectTimeout; }
	
	/** Gets the time for the control socket to respond (microseconds) */
	inline DDWORD controlSocketResponseTime() const { return m_controlSocketResponseTime; }
	
	/** Get the duty cycle of the socket sending messages to the the ACU (microseconds) */
	inline DDWORD controlSocketDutyCycle() const { return m_controlSocketDutyCycle; }
	
	/** Get the timeout for socket the status message from the ACU  (microseconds) */
	inline DDWORD statusSocketTimeout() const { return m_statusSocketTimeout; }
	
	/** Get the duty cycle of the socket waiting for status message from the ACU (microseconds) */
	inline DDWORD statusSocketDutyCycle() const { return m_statusSocketDutyCycle; }
	
	/** Gets the control thread 8watch dog) period between two executions (microseconds) */
	inline DWORD watchDogThreadPeriod() const { return m_dwwatchDogThreadPeriod; }
	
	
	/** Gets the cache time for the logging repetition filter (microseconds) */
	inline DDWORD repetitionCacheTime() const { return m_dwrepetitionCacheTime; }

	/** Gets the expire time for the logging repetition filter (microseconds) */
	inline DDWORD expireCacheTime() const { return m_dwexpireCacheTime; }	
	
	/** Gets the time between two consecutive refreshes of the properties */
	//inline DDWORD propertyRefreshTime() const { return m_dwpropertyRefreshTime; }
	
	/**
	 * gets the time allowed to the antenna to go to parking position after a stow command has been issued (microseconds)
	 */
	inline DDWORD stowTimeout() const { return m_dwstowTimeout; }

	/**
	 * gets the time allowed to the antenna to complete the unstow operation (microseconds)
	 */
	inline DDWORD unstowTimeout() const { return m_dwunstowTimeout; }
	
	/**
	 * gets the name of the source used as time synchronization
	 */
	inline const IRA::CString& getTimeSource() const { return m_timeSource; }

	/**
    * This member function is used to configure component by reading the configuration parameter from the CDB.
	 * This must be the first call before using any other function of this class.
	 * @throw ComponentErrors::CDBAccess
	 * @param Services pointer to the container services object
	*/
	void init(maci::ContainerServices *Services) throw (ComponentErrors::CDBAccessExImpl);
	
	/**
	 * This function will load the configuration from hard coded value. Used only for debug purposes
	 */
	void init() throw (ComponentErrors::CDBAccessExImpl);
private:
	/** ACU ip address */	
	IRA::CString m_ACUAddress;
	/** ACU listening port */
	WORD m_commandPort;
	/** ACU status port */
	WORD m_statusPort;
	/** Receive  status Timeout */
	DDWORD m_statusSocketTimeout;
	/** Receive status duty cycle */
	DDWORD m_statusSocketDutyCycle;
	/** Microseconds allows to the control line to respond before declaring a timeout */
	DDWORD m_controlSocketResponseTime;
	/** send command duty cycle */
	DDWORD m_controlSocketDutyCycle;
	
	DDWORD m_dwrepetitionCacheTime;
	
	DDWORD m_dwexpireCacheTime;
	
	DDWORD m_dwwatchDogThreadPeriod;
	
	DDWORD m_dwstowTimeout;
	
	DDWORD m_dwunstowTimeout;
	
	double m_azimuthLowerLimit;
	double m_azimuthUpperLimit;
	double m_elevationLowerLimit;
	double m_elevationUpperLimit;
	double m_azimuthRateLowerLimit;
	double m_azimuthRateUpperLimit;
	double m_elevationRateLowerLimit;
	double m_elevationRateUpperLimit;
	double m_cwLimit;
	IRA::CString m_timeSource;
};


#endif
