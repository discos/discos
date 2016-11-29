#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

/* ************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                                             */
/* $Id: Configuration.h,v 1.2 2011-05-12 14:14:31 a.orlati Exp $                                                                                                                                     */
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
	 * @throw CDBAccess
	 * @throw MemoryAllocation
	 * @throw IRALibraryResource
	 * @param Services pointer to the container services object
	*/
	void init(maci::ContainerServices *Services) throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl,ComponentErrors::IRALibraryResourceExImpl);

	/**
	 * @return the port number
	 */
	inline const WORD& getPort() const { return m_wPort; }
	
	/**
	 * @return the TCP/IP address of the listening server
	 */
	inline const IRA::CString getAddress() const {return m_sAddress; }
	
	/**
	 * @return the timeout when receiving from the command socket(in microseconds)
	 */
	inline const DWORD getCommandLineTimeout() const { return m_dwCommandLineTimeout; }
	
	/**
	 * @return the timeout in microseconds when trying to reconnect to the backend
	*/
	inline const DWORD getConnectTimeout() const { return m_dwConnectTimeout; }
	
	/**
	 * @return the time in microseconds that is taken between two property refreshes
	 */
	inline const DWORD getPropertyRefreshTime() const { return m_dwPropertyRefreshTime; }
	
	/**
	 * @return the name of the configuration that the backend have to implement
	 */
	inline const IRA::CString getConfiguration() const { return m_sConfig; }
	
	/**
	 * @return the number of microseconds that the log filter will cache a log message
	*/
	inline DWORD getRepetitionCacheTime() const { return m_dwRepetitionCacheTime; }
	
	/**
	 * @return the number of microseconds that the log filter will take as expiration time for a log message
	*/
	inline DWORD getRepetitionExpireTime() const { return m_dwRepetitionExpireTime; }

	/**
	 * @return the number of microseconds that the backend will take after a command issue to sent the first data
	*/
	inline DWORD getDataLatency() const { return m_dwDataLatency; }

	/**
	 * @return the port number for the data line
	 */
	inline const WORD& getDataPort() const { return m_wDataPort; }
	
	/**
	 * @return the sleep time of the sender thread in microseconds 
	 */
	inline const DWORD& getSenderSleepTime() const { return m_dwSenderSleepTime; }
	
	/**
	 * @return the response time of the sender thread in microseconds 
	 */
	inline const DWORD& getSenderResponseTime() const { return m_dwSenderResponseTime; }
	
	/**
	 * @return the sleep time of the control thread in microseconds 
	 */
	inline const DWORD& getControlSleepTime() const { return m_dwControlSleepTime; }
	
	/**
	 * @return the response time of the control thread in microseconds 
	 */
	inline const DWORD& getControlResponseTime() const { return m_dwControlResponseTime; }
	
	/**
	 * @return the time tollerance in microsenconds. 
	 */
	inline const DWORD& getTimeTollerance() const { return m_dwTimeTollerance; }
	
	
private:
	WORD m_wPort;
	IRA::CString m_sAddress;
	DWORD m_dwCommandLineTimeout;
	DWORD m_dwConnectTimeout;
	DWORD m_dwPropertyRefreshTime;
	IRA::CString m_sConfig;
	DWORD m_dwRepetitionCacheTime;
	DWORD m_dwRepetitionExpireTime;
	WORD m_wDataPort;
	IRA::CString m_sDataAddress;
	DWORD m_dwDataLatency;
	DWORD m_dwSenderSleepTime;
	DWORD m_dwSenderResponseTime;
	DWORD m_dwTimeTollerance;
	DWORD m_dwControlSleepTime;
	DWORD m_dwControlResponseTime;	
	DWORD m_dwBoardsNumber;
	DWORD m_dwDataBufferSize;
};

#endif /*CONFIGURATION_H_*/
