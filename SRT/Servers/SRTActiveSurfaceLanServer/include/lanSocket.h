#ifndef lanSOCKET_H
#define lanSOCKET_H

#define NAK 0x15
#define ACK 0x06
#define BUFFERSIZE 128
#define TIMEO 3000000 //usec
#define SBUFF 8


#include <IRA>
#include "acstime.h" 

/*******************************************************************************
*
* "@(#) $Id: lanSocket.h,v 1.1 2011-03-24 09:16:27 c.migoni Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* GMM       jul 2005   creation				   
*/

using namespace IRA;

/**
 * This class lanSocket manage the socket stuff of LAN/485 converter.
  * 
 * @author <a href=mailto:g.maccaferri@ira.cnr.it>Giuseppe Maccaferri</a>,
 * IRA, Bologna<br>
 * @version "@(#) $Id: lanSocket.h,v 1.1 2011-03-24 09:16:27 c.migoni Exp $"
 */
class lanSocket: public CSocket
{
public:
	lanSocket();
	
	virtual ~lanSocket();
	
	
	/**
	*	open a lan connections.
	*	@param Address remote address
	*	@param port	remote port
	*		
	*	@return  (0) on estabished connection
	*					 <0  on error
	*/
	int Init(CString Address,WORD Port);
	
	/**
	*	send USD command to lan. Optionally can send 1 parameter(par) of nbytes byte.
	*	par can be of 4 byte at maximum. If nbytes is 0 no par bytes are sent
	*	@param cmd USD command to be sent (see USD manual)
	*	@param addr the addres of usd
	*	@param par	optional parameter
	*	@param nbytes bytes of par to be used 
	*		
	*	@return USD ACK or NAK reply on positive sent 
	* else:
	* 1 
	*	 0 if socket was broken, and sucessful restablished,
	*	-1 if USD was in tout. (USD's buffer cleared!)
	*	-2 socket tout 
	*       -3 unrecoverable socket error
	*	-4 no valid ack/nak resp. code 
	*
	*/
	int sendCmd(BYTE cmd ,BYTE addr=0,long par=0,BYTE nbytes=0);
		
	/**
	* This member function is called to receive a buffer from lan.
	* @param msg ponter to the byte buffer that will contain the message from the lan
	* @param len number of bytes to be read
	* @return the number of bytes read, 
	* else:
	* 1 
	*	0 if socket was broken, and sucessful restablished,
	*	-1 if USD was in tout. USD's buffer cleared!
	*	-2 socket tout 
	*       -3 unrecoverable socket error
	*	-4 partial timeout
	*/
	int receiveBuffer(BYTE * msg,WORD len);
	
	

	
	/** @var connection status */
	enum soStat { READY,NOTRDY,BROKEN,TOUT,BUSY } m_soStat;   
	
protected:
    /**
     * Computes the checksum byte on buffer coming or sent to USd.
	 * It makes the byte sum  and return the 1'complement
     * @param buf the buffer
	 * @param the number of byte to be used, form the beginning
	 * @return the checksum byte
     */
    BYTE checksum(BYTE* buf,WORD len);
	
	/** 
	*   Recovers the socket functionality.
	*	In case of broken socket(err=0), close and try to reopen it
	*	on timeout  (err=-2) check the LAN/485 converter functionality sending a dummy buffer
	*	on fail (err=-3),  close and try to reopen the socket
	*  	@return 0 on succesfull reconnection
	* -1 on USD tout, and USD's buffer clear
	* -2 socket tout 
	* -3 socket fail
	*/
	int reConnect(int);
	
	/**
	*   flush the input buffer to resyncronize the answer with the requests
	*/
	void clearBuffer();

	/**
	*   flush the input buffer of USD's sending 8 dummy bytes
	*	@returns:
	*	0 for socket success write 
	*	-x on socket error
	*/
	int clearUSDBuffer();
		
	void printBuffer(BYTE *,WORD );

	
private:
//	static const int BUFFERSIZE=128;
	
	/** @var contains error information */
	CError m_Error;
	
	/** @var indicates when the monitor data buffer has been updated the last time */
	TIMEVALUE m_MonitorTime;
	
	/** @var lan ip address */	
	CString m_slanAddress;
	
	/** @var lan listening port */
	WORD m_wlanPort;

};

#endif
