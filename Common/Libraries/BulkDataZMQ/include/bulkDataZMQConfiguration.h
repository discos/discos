#ifndef _CZMQCONFIGURATION_H_
#define _CZMQCONFIGURATION_H_

/*******************************************************************************\
 IRA Istituto di Radioastronomia
 This code is under GNU General Public License (GPL).

 Author: Andrea Orlati (andrea.orlati@inaf.it)
\*******************************************************************************/


#include <IRA>
#include <ComponentErrors.h>
#include <string>

#define CONFIG_PATH "BULKDATA/Flows"

using namespace IRA;

namespace bulkdataZMQImpl {

typedef struct {
	IRA::CString backendName;
	IRA::CString endPoint;
	CORBA::ULong flowNumber;
	bool tcpNoDelay;
    bool cpuAffinity;
} TZMQConfig;

/**
 * This class implements the BulkData configuration. The data inside this class are initialized at the startup 
 * @author <a href=mailto:andrea.orlati@inaf.it>Andrea Orlati</a>, Istituto di Radioastronomia, Italia
 * <br>
  */
template <class T>
class CZMQConfiguration {
public:

	/**
	 * Default constructor
	 */
	CZMQConfiguration();

	/**
	 * Destructor
	 */
	~CZMQConfiguration();


	
	/**
	 * This member function is used to configure the bulk Data transfer by reading the configuration parameter from the CDB.
	 * This must be the first call before using any other function of this class.
	 * @throw ComponentErrors::CDBAccessExImpl,
	 * @throw ComponentErrors::MemoryAllocationExImpl,
	 * @throw ReceiversErrors::ModeErrorExImpl
	 * @param Services pointer to the container services object or to simpleClient
	 */
	void init(const T *Services);

    /**
     * Returns the configuration for a specific flow number.
     * @param flowNumber the flow identifier.
     * @return the configuration structure for the requested flow.
     * @throw ComponentErrors::CDBAccessExImpl if the flow number is not found.
     */
    TZMQConfig getConfiguration(const CORBA::ULong& flowNumber);
	
    /**
     * Returns the configuration for a specific component name.
     * @param comp_name the name of the component.
     * @return the configuration structure for the requested flow.
     * @throw ComponentErrors::CDBAccessExImpl if the component name is not found.
     */
    TZMQConfig getConfiguration(const IRA::CString& comp_name);


private:	

	const T* m_services;

	IRA::CDBTable *m_table;
	
};

#include "bulkDataZMQConfiguration.tpp"

}

#endif /*_CZMQCONFIGURATION_H_*/