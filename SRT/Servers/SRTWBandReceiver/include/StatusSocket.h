#ifndef STATUSSOCKET_H_
#define STATUSSOCKET_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: StatusSocket.h,v 1.3 2023-07-04 14:02:49 lorenzo.monti Exp $																								*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                		when           What                                         */
/* Lorenzo Monti(lorenzo.monti@inaf.it)  04/07/2023      Creation                                       */


#include <IRA>
#include <ComponentErrors.h>
#include <SecureArea.h>
#include "WBandCommand.h"
#include "Configuration.h"
#include <LogFilter.h>

/**
 * This class is inherited from the IRA::CSocket class. It takes charge of the management of the socket used by the component in order to get from remote interface of  the SRT ACU the status messages.. 
  * if the remote side disconnects or a problem affects the comunication line, this class try to reconnect to the ACU until the communication is up again.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CStatusSocket: public IRA::CSocket {
public:
	/**
	 * Constructor.
	*/
	CStatusSocket();
	/**
	 * Destructor
	*/
	virtual ~CStatusSocket();
	
	/**
     * This member function is used to enstablish and configure the communication channel to the WBand receiver. 
	 * This must be the first call before using any other function of this class.
	 * @param config pointer to the component configuration data structure
	 * @param data pointer to the WBandProtocol
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c IRALibraryResource>
	*/
	void init(CConfiguration *config,IRA::CSecureArea<CWBandCommand> *data) throw (ComponentErrors::SocketErrorExImpl);
	
	/**
	 * Called to close the socket and free everything.
	 */
	void cleanUp();
	
	/**
	 * Force the socket to connect to the ACU. If the operation fails an exception is thrown, if the operation could not succesfully complete immediately 
	 * the control line state is set to <i>WBand Receiver::ACU_CNTDING</i>. It must be called by an external watchdog in order to keep the connection enstablished.
	 * This is not synchronized because this method acts on the socket only by accessing the mutex for the <i>CWBandCommand</i> class as all other method of this object 
	 */
	void connectSocket(IRA::CString IPAddr,WORD Port) throw (ComponentErrors::SocketErrorExImpl);

	void setBinder(void (*binder)(const CWBandCommand *)) { m_customBind=binder; }
	
protected:
	
	virtual void onReceive(DWORD Counter,bool &Boost);
	
	virtual void onTimeout(WORD EventMask);
	
	virtual void onConnect(int ErrorCode);
	
private:
	CConfiguration * m_pConfiguration;
	IRA::CSecureArea<CWBandCommand> * m_pData;
	/**
	 * Keep track of the number of timeout events happened in the socket since the last receive
	 */
	WORD m_wtimeOuts;
	/**
	 * This flag indicates that the data  transfer has been started by the ACU.
	 */
	bool m_btransferStarted;

	/**
	 * If not null this function is called every time a new status massage frame is received
	 * otherwise the default bind is called
	 */
	void (*m_customBind)(const CWBandCommand *);
	
	int receiveBuffer(BYTE *Msg,WORD Len,IRA::CError& err);

	/**
	 * Creates and setup the socket in charge of receiving status data from the ACU
	 */ 
	void createSocket() throw (ComponentErrors::SocketErrorExImpl);
};

#endif /*STATUSSOCKET_H_*/
