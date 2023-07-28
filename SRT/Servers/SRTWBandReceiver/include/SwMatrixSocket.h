#ifndef SWMATRIXSOCKET_H_
#define SWMATRIXSOCKET_H_


/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id:  $																								*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                      when            What                                        */
/* Lorenzo Monti(lorenzo.monti@inaf.it)     08/06/2023     Creation                                     */


#include <IRA>
#include <AntennaErrors.h>
#include <ComponentErrors.h>
#include "WBandCommand.h"
#include "Configuration.h"

/**
 * This class is inherited from the IRA::CSocket class. It takes charge of the management of the socket used to command the SRT W receiver via the remote interface. 
  * if the remote side disconnects or a problem affects the comunication line, this class try to reconnect to the receiver until the communication is up again..
 * @author <a href=mailto:lorenzo.monti@inaf.it>Lorenzo Monti</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CSwMatrixSocket: public IRA::CSocket
{
public:
	/**
	 * Constructor.
	*/
	CSwMatrixSocket();
	/**
	 * Destructor
	*/
	virtual ~CSwMatrixSocket();
	
	/**
     * This member function is used to enstablish and configure the communication channel to the W receiver. 
	 * This must be the first call before using any other function of this class.
	 * @param config pointer to the component configuration data structure
	 * @param data pointer to the W receiver data structure
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c IRALibraryResource>
	*/
	void init(CConfiguration *config) throw (ComponentErrors::SocketErrorExImpl);
	
	/**
	 * Called to close the socket and free everything.
	 */
	void cleanUp();
	
	/**
	 * Force the socket to connect to the W receiver. If the operation fails an exception is thrown, if the operation could not succesfully complete immediately 
	 * the command line state is set to <i>W receiver::W receiver_CNTDING</i>. It must be called by an external watchdog in order to keep the connection enstablished.
	 * This is not synchronized because this method acts on the socket only by accessing the mutex for the <i>CWBandCommand</i> class as the background thread of this socket @sa <i>onconnect()</i> 
	 */
	void connectSocket(IRA::CString IPAddr,WORD Port) throw (ComponentErrors::SocketErrorExImpl);

	/**
	 * This function can be used to send a command to the W receiver.
	 * In case of problems it changes the component status accordingly. 
	 * @param cmd command to be sent
	 * @param len lenght in bytes of the command
	 * @param outBuff output buffer to store data received
	 * @throw TimeoutExImpl if the operation timeout before completing. 
	 * @thorw SocketErrorExImpl if the connection fails before completing. In that case the status is set to <i>Antenna::W receiver_NOTCNTD</i>
	 */
	void sendCommand(std::string cmd, std::string outBuff, CConfiguration config) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl);

	inline void string2ByteArray(char* input, BYTE* output){ 
		int loop = 0; 
		int i = 0; 
		while(input[loop] != '\0'){ output[i++] = input[loop++]; }
	}

protected:
	/**
	 * Automatically called by the framework as a result of a connection request. See super-class for more details.
	 * @param  ErrorCode connection operation result, zero means operation is succesful.
	*/
	void onConnect(int ErrorCode);
	
private:

	CConfiguration * m_pConfiguration;
	bool m_bTimedout;
	
	/**
	 * This mutex is used to sync external calls to the public methods of the class. The requirements is that just one call at the time must be served if the methos requires to
	 * send a message to the W receiver. the reason for that is that we need to wait to known the result of the previous command to the W receiver that comes form the status socket.
	 */
	ACE_Recursive_Thread_Mutex m_syncMutex;

	/**
	 * Creates and setup the socket in charge of sending commands to the W receiver. 
	 */ 
	void createSocket() throw (ComponentErrors::SocketErrorExImpl);
	
	/**
    * This member function is called to send a buffer to the W receiver.
	 * @param Msg pointer to the byte buffer that contains the message for the W receiver
	 * @param Len length of the buffer
	 * @param error It will contain the error description in case the operation fails.
	 * @return SUCCESS if the buffer was sent correctly, WOULDBLOCK if the send would block, FAIL in case of error, the parameter error is set accordingly.
	*/
	OperationResult sendBuffer(std::string Msg, IRA::CError& error, CConfiguration config);
	
		/**
    * This member function is called to receive a buffer.
	 * @param Msg ponter to the byte buffer that will contain the answer
	 * @param Len length of the buffer, that means the exact number of bytes that can be read
	 * @param error to manage error if occurs.
	 * @return the number of bytes read, 0 if the connection fell down, FAIL in case of error and the <i>m_Error</i> member is set accordingly, 
	 *         WOULDBLOCK if the timeout expires
	*/
	int receiveBuffer(std::string Msg, IRA::CError& error);

	/**
	 * This methos will cycle to check is the W receiver responded to the last command sent. There three possible scenarios:
	 * @arg the W receiver responded and the operation was succesfull
	 * @arg the W receiver did not responded yet (in that case a timeout policy is implemented)
	 * @arg the W receiver responded but the operation was not completed correctly.
	 * @param commandTime marks the epoch the command message was sent to the W receiver, used to apply the timeout mechanism.
	 * @throw AntennaErrors::NakExImpl if an error was repoerted by the W receiver
	 * @throw TimeoutExImpl if the W receiver does not report back the answer within the expected time
	 */
	void waitAck(const ACS::Time& commandTime) throw (AntennaErrors::NakExImpl,ComponentErrors::TimeoutExImpl);

	/**
	 * This function checks if the connection is ok. 
	 * @return true if the connection is ok, false is the connection is not available.
	 */
	bool checkConnection();
	
	/**
	 * This method check if the Antenna is busy with long jobs and could not accept new commands.
	 * @return true if the antenna is busy
	 */
	bool checkIsBusy();
	
};





#endif /*SWMATRIXSOCKET_H_*/
