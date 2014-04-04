#ifndef CCONFIGURATION_H
#define CCONFIGURATION_H

/* ************************************************************************ */
/* OAC Osservatorio Astronomico di Cagliari                                 */
/* $Id: Configuration.h,v 1.2 2010-10-15 09:11:54 c.migoni Exp $							    */
/*                                                                          */
/* This code is under GNU General Public Licence (GPL).                     */
/*                                                                          */
/* Who              when            What                                    */
/* Carlo Migoni     14/12/2009      Creation                                */

#include <IRA>
#include <maciContainerServices.h>
#include <ComponentErrors.h>

using namespace IRA;

/**
 * This class implements the component configurator. The data inside this class are initialized at the startup from the
 * configuration database and then are used (read) inside the component.
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>
 * Osservatorio Astronomico di Cagliari, Italia
 * <br> 
  */
class CConfiguration {

public:
	/** Gets socket server ip address */
	inline CString ipAddress() const { return m_sSocketServerAddress; }
		
	/** Gets the socket port to which the server is listening */
	inline WORD Port() const { return m_wPort; }

	/** Get the receive timeout*/
	inline DWORD receiveTimeout() const { return m_dwreceiveTimeout; }

	/** Gets the control thread period between two executions */
	inline DWORD controlThreadPeriod() const { return m_dwcontrolThreadPeriod; }

    /** Gets supervisor name */
	inline CString superVisor() const { return m_sSuperVisor; }
	
	/**
    * This member function is used to configure component by reading the configuration parameter from the CDB.
	 * This must be the first call before using any other function of this class.
	 * @throw ComponentErrors::CDBAccess
	 * @param Services pointer to the container services object
	*/
	void Init(maci::ContainerServices *Services) throw (ComponentErrors::CDBAccessExImpl);
private:
	/** Socket server ip address */	
	CString m_sSocketServerAddress;
	/** ACU listening port */
	WORD m_wPort;
    /** Receive Timeout */
	DWORD m_dwreceiveTimeout;
	/** This is the period of the control Thread (100ns unit) */
	DWORD m_dwcontrolThreadPeriod;
	/** Socket server ip address */	
	CString m_sSuperVisor;
};
#endif
