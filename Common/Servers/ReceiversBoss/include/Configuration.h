#ifndef _CCONFIGURATION_H
#define _CCONFIGURATION_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 08/09/2011     Creation                                         */


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

	typedef struct {
		IRA::CString code;
		IRA::CString component;
	} TReceiver;

	/**
	 * Default constructor
	 */
	CConfiguration();

	/**
	 * Destructor
	 */
	~CConfiguration();

	/**
    * This member function is used to configure component by reading the configuration parameter from the CDB.
	 * This must be the first call before using any other function of this class.
	 * @throw ComponentErrors::CDBAccess
	 * @param Services pointer to the container services object
	*/
	void init(maci::ContainerServices *Services)  throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl);
	
	/**
	 * This function checks if the receiver corresponding to the given code is available and in case it returns the name of the instance of the component in charge
	 * of controlling the receiver
	 * @param code requested receiver code
	 * @param component name of the component instance
	 * @return true if the code is available, false otherwise
	 */
	bool getReceiver(const IRA::CString& code,IRA::CString& component) const;

	/**
	 *  Gets the persistence time of the status of the component
	 */
	inline const DDWORD& getStatusPersistenceTime() const { return m_statusPersistenceTime; }

	/** Gets the cache time for the logging repetition filter (microseconds) */
	inline const DDWORD& repetitionCacheTime() const { return m_repetitionCacheTime; }

	/** Gets the expire time for the logging repetition filter (microseconds) */
	inline const DDWORD& expireCacheTime() const { return m_expireCacheTime; }

	/**
	 * Get the time before refreshing a property again (microseconds)
	 */
	inline const DDWORD& propertyUpdateTime() const { return m_propertiesUpdateTime;}

private:
	TReceiver * m_receiver;
	WORD m_receiverNum;
	DDWORD m_expireCacheTime;
	DDWORD m_repetitionCacheTime;
	DDWORD m_statusPersistenceTime;
	DDWORD m_propertiesUpdateTime;
};


#endif
