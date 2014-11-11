#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

/* ************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                                             */
/* $Id: Configuration.h,v 1.1 2011-03-14 15:16:22 a.orlati Exp $                                                                                                                                     */
/*                                                                                                                                                */
/* This code is under GNU General Public Licence (GPL).                                                         */
/*                                                                                                                                                 */
/* Who                                                    When                 What                                                  */
/* Andrea Orlati(aorlati@ira.inaf.it)      02/10/2008         Creation                                             */

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
	 * @return the sleep time of the sender thread in microseconds 
	 */
	inline const DWORD& getSenderSleepTime() const { return m_dwSenderSleepTime; }
	
	/**
	 * @return the response time of the sender thread in microseconds 
	 */
	inline const DWORD& getSenderResponseTime() const { return m_dwSenderResponseTime; }
		
private:
	DWORD m_dwSenderSleepTime;
	DWORD m_dwSenderResponseTime;
};

#endif /*CONFIGURATION_H_*/
