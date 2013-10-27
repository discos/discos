#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

/* ************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                                             */
/* $Id: Configuration.h,v 1.3 2010-09-14 08:50:07 a.orlati Exp $                                                                                                                                     */
/*                                                                                                                                                */
/* This code is under GNU General Public Licence (GPL).                                                         */
/*                                                                                                                                                 */
/* Who                                                    When                      What                                                  */
/* Andrea Orlati(aorlati@ira.inaf.it)      18/01/2009         Creation                                             */

#include <IRA>
#include <maciContainerServices.h>
#include <ComponentErrors.h>

using namespace IRA;

namespace FitsWriter_private {

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
	 * @return the sleep time of the working thread (microseconds)
	 */
	const long& getWorkingThreadTime() const { return m_workingThreadTime; }
	
	/**
	 * @return the sleep time of the collector thread (microseconds)
	 */
	const long& getCollectorThreadTime() const { return m_collectorThreadTime; }
	
	/**
	 * @return the  time slice of the working thread (microseconds)
	 */	
	const long& getWorkingThreadTimeSlice() const { return m_workingThreadTimeSlice; }
	
	/**
	 * @return the number of microseconds that the log filter will cache a log message
	*/
	const long& getRepetitionCacheTime() const { return m_repetitionCacheTime; }
	
	/**
	 * @return the number of microseconds that the log filter will take as expiration time for a log message
	*/
	const long& getRepetitionExpireTime() const { return m_repetitionExpireTime; }

	/**
	 * @return the number of microseconds that will used as gap between two meteo parameters refreshes
	*/
	const long& getMeteoParameterDutyCycle() const { return m_meteoParameterDutyCycle; }
	
	/**
	 * @return the number of microseconds that will used as gap between two tracking enquires to minor
	 * servo system
	*/
	const long& getMinorServoEnquireMinGap() const { return m_minorServoEnquireMinGap; }

	/**
	 * @return the number of microseconds that will used as gap between two tracking flag refreshes
	*/
	const long& getTrackingFlagDutyCycle() const { return m_trackingFlagDutyCycle; }
	
	/**
	 * @return the name of the interface of the antenna boss component
	*/
	const IRA::CString& getAntennaBossComponent() const { return m_antennaBossComp; }
	/**
	 * @return the name of the interface of the observatory component
	 */
	const IRA::CString& getObservatoryComponent() const { return m_observatoryComp; }
	/**
	 * @return the name of the interface of the receivers boss component.
	*/
	const IRA::CString& getReceiversBossComponent() const { return m_receiversBossComp; }
	/**
	 * @return the name of the interface of the minor servo boss component.
	*/
	const IRA::CString& getMinorServoBossComponent() const { return m_msBossComp; }
	/**
	 * @return the name of the interface of the scheduler component.
	*/	
	const IRA::CString& getSchedulerComponent() const { return m_schedulerComp; }	
	/**
	 * @return the name of the instance of the meteo station component
	*/	
	const IRA::CString& getMeteoInstance() const { return m_meteoInstance; }
	
private:
	long m_workingThreadTime;
	long m_workingThreadTimeSlice;
	long m_collectorThreadTime;
	long m_repetitionCacheTime;
	long m_repetitionExpireTime;
	long m_meteoParameterDutyCycle;
	long m_trackingFlagDutyCycle;
    long m_minorServoEnquireMinGap;
	IRA::CString m_antennaBossComp;
	IRA::CString m_observatoryComp;
	IRA::CString m_receiversBossComp;
	IRA::CString m_schedulerComp;
	IRA::CString m_meteoInstance;
	IRA::CString m_msBossComp;
};

};

#endif /*CONFIGURATION_H_*/
