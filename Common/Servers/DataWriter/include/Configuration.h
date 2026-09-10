#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

/* ************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                       */                                          
/* This code is under GNU General Public Licence (GPL).                                  */
/*                                                                                       */ 
/* Who                 When                      What                                    */
/* Andrea Orlati      27/08/2026         Creation                                        */

#include <IRA>
#include <maciContainerServices.h>
#include <ComponentErrors.h>

using namespace IRA;

namespace DataWriter_private {

/**
 * This class implements the component configurator. The data inside this class are initialized at the startup from the
 * configuration database and then are used (read) inside the component.
 * @author <a href=mailto:andrea.orlati@inaf.it>Andrea Orlati</a>,
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
	 * @throw CDBAccessExImpl if an error occurs while accessing the CDB
	 * @param Services pointer to the container services object
	*/
	void init(maci::ContainerServices *Services);

	/**
	 * @return the sleep time of the working thread (microseconds)
	 */
	const long& getWorkingThreadTime() const { return m_workingThreadTime; }
	

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

	
private:
	long m_workingThreadTime;
	long m_workingThreadTimeSlice;
	long m_repetitionCacheTime;
	long m_repetitionExpireTime;

};

};

#endif /*CONFIGURATION_H_*/
