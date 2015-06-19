#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

/* ************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                                             */
/* $Id: Configuration.h,v 1.1 2010-09-17 17:11:51 spoppi Exp $                                                                                                                                     */
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
 * configuration database and then are used (read) inside the component. It is the 
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italy
 * Modified by @author <a href=mailto:spoppi@oa-cagliari.inaf.it>Sergio Poppi</a>,
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
	This function gets the sampling time parameter from the
	CDB. It is the thread sleep time.
	@return Sampling Time;
	*/
	inline const DWORD& getSamplingTime() const { return m_samplingTime; }
	/**
	This function returns true if the positions must be logged. 
	If true, a reference to the AntennaBoss is obtaines.
	CDB. It is the thread sleep time.
	@return Sampling Time;
	*/

	inline const bool& getLogObservedPosition() const { return m_logObservedPositions; }
	
private:
		DWORD m_samplingTime;	
		bool m_logObservedPositions;

};

#endif /*CONFIGURATION_H_*/
