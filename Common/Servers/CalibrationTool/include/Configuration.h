#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

/* **************************************************************************/
/* OAC - Osservatorio Astronomico di Cagliari                               */
/* $Id: Configuration.h,v 1.4 2011-04-05 13:00:30 c.migoni Exp $            */
/*                                                                          */
/* This code is under GNU General Public Licence (GPL).                     */
/*                                                                          */
/* Who                                when            What                  */
/* Carlo Migoni (migoni@ca.astro.it)  23/11/2010      Creation              */
/* ************************************************************************************* */

#include <IRA>
#include <maciContainerServices.h>
#include <ComponentErrors.h>

using namespace IRA;

namespace CalibrationTool_private {

/**
 * This class implements the component configurator. The data inside this class are initialized at the startup from the
 * configuration database and then are used (read) inside the component.
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>
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
	 * @return the sleep time of the working thread (in 100 ns)
	 */
	const long& getWorkingThreadTime() const { return m_workingThreadTime; }
	
	/**
	 * @return the sleep time of the collector thread (in 100 ns)
	 */
	const long& getCollectorThreadTime() const { return m_collectorThreadTime; }
	
	/**
	 * @return the  time slice of the working thread (in 100 ns))
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
	 * @return the number of microseconds that will used as gap between two tracking flag refreshes
	*/
	const long& getTrackingFlagDutyCycle() const { return m_trackingFlagDutyCycle; }
	
	/**
	 * @return the name of the interface of the antenna boss component
	*/
	const IRA::CString& getAntennaBossComponent() const { return m_antennaBossComp; }
	/**
	 * @return the name of the interface of the scheduler component.
	*/	
	const IRA::CString& getSchedulerComponent() const { return m_schedulerComp; }
    /**
	 * @return the name of the interface of the observatory component.
	*/	
	const IRA::CString& getObservatoryComponent() const { return m_observatoryComp; }

    /**
	 * @return the flag that allows to control the file generation
	*/
	bool outputFile() const { return (m_generateFile==1); }

private:
	long m_workingThreadTime;
	long m_workingThreadTimeSlice;
	long m_collectorThreadTime;
	long m_repetitionCacheTime;
	long m_repetitionExpireTime;
	long m_trackingFlagDutyCycle;
	long m_generateFile;
	IRA::CString m_antennaBossComp;
	IRA::CString m_schedulerComp;
    IRA::CString m_observatoryComp;
};

};

#endif /*CONFIGURATION_H_*/
