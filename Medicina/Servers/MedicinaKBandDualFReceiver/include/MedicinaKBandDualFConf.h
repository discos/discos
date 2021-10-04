#ifndef _MEDKBANDCONFIGURATION_H_
#define _MEDKBANDCONFIGURATION_H_

/*******************************************************************************\
 IRA Istituto di Radioastronomia                                 
 This code is under GNU General Public License (GPL).          
                                                              
 Andrea Orlati (aorlati@ira.inaf.it): Author

\*******************************************************************************/

#include <IRA>
#include <maciContainerServices.h>
#include <ComponentErrors.h>
#include <ReceiversErrors.h>

/**
 * This class implements the component configuration. The data inside this class are initialized at the startup from the
 * configuration database and then are used (read) inside the component.
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
  */
class CKBandConfiguration  {
public:

	/**
	 * Default constructor
	 */
	CKBandConfiguration();

	/**
	 * Destructor
	 */
	~CKBandConfiguration();

	/**
	 * @return the IP address of the vertex control
	 */
	inline const IRA::CString& getVertexIPAddress() const { return m_vertexIPAddress; }

	/**
	 * @return the IP port of the vertex control
	 */
	inline const WORD& getVertexPort() const { return m_vertexPort; }

	/**
	 * @return the IP port of the vertex control
	 */
	inline const IRA::CString& getVertexCommand() const { return m_vertexCommand; }

	/**
    * This member function is used to configure component by reading the configuration parameter from the CDB.
	 * This must be the first call before using any other function of this class.
	 * @throw ComponentErrors::CDBAccess
	 * @param Services pointer to the container services object
	*/
	virtual void init(maci::ContainerServices *Services)  throw (
            ComponentErrors::CDBAccessExImpl,
            ComponentErrors::MemoryAllocationExImpl,
            ReceiversErrors::ModeErrorExImpl
    );


protected:
	IRA::CString m_vertexIPAddress;
	WORD m_vertexPort;
	IRA::CString m_vertexCommand;	
};

#endif
