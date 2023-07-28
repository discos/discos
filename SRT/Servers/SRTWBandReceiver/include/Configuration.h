#ifndef CCONFIGURATION_H
#define CCONFIGURATION_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who           		                   when         What                                            */
/* Lorenzo Monti (lorenzo.monti@inaf.it)  26/6/23     Creation                                          */

#include <IRA>
#include <maciContainerServices.h>
#include <ComponentErrors.h>

/**
 * This class implements the component configuration. The data inside this class are initialized at the startup from the
 * configuration database and then are used (read) inside the component.
 * @author <a href=mailto:lorenzo.monti@inaf.it>Lorenzo Monti</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
  */
class CConfiguration {
public: 
	
	/** Gets W band LO ip address */
	inline IRA::CString WLOAddress() const { return m_WLOAddress; }
		
	/** Gets the socket port to which the LO is listening to commands */
	inline WORD WLOPort() const { return m_WLOPort; }

	/** Gets Switch Matrix ip address */
	inline IRA::CString SwMatrixAddress() const { return m_WSwMatrixAddress; }
		
	/** Gets the socket port to which the Switch Matrix is listening to commands */
	inline WORD SwMatrixPort() const { return m_WSwMatrixPort; }

	/** Gets Solar Attenuator ip address */
	inline IRA::CString WCALddress() const { return m_WCALAddress; }
		
	/** Gets the socket port to which the Solar Attenuator is listening to commands */
	inline WORD WCALPort() const { return m_WCALPort; }

	/** Gets the time for the control socket to respond (microseconds) */
	inline DDWORD getSocketResponseTime() const { return m_socketResponseTime; }
	
	/**
	 * @return the sleep time of the sender thread in microseconds 
	 */
	inline const DWORD& getSocketSleepTime() const { return m_socketSleepTime; }

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
	/** W-band LO ip address */	
	IRA::CString m_WLOAddress;
	/** W-band LO listening port */
	WORD m_WLOPort;
	/** W-band Switch Matrix ip address */	
	IRA::CString m_WSwMatrixAddress;
	/** W-band Switch Matrix listening port */
	WORD m_WSwMatrixPort;
	/** W-band Solar Attenuator ip address */	
	IRA::CString m_WCALAddress;
	/** W-band Solar Attenuator listening port */
	WORD m_WCALPort;
	/** Microseconds allows to the control line to respond before declaring a timeout */
	DDWORD m_socketResponseTime;
	/***/
	DWORD m_socketSleepTime;
};


#endif
