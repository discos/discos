#ifndef COMMANDSOCKET_H_
#define COMMANDSOCKET_H_


#include <IRA>
#include <BackendsErrors.h>
#include <ComponentErrors.h>
#include "Configuration.h"
#include "DBESMprotocol.h"


/**
 * This class is inherited from the IRA::CSocket class. It takes charge of the management of the socket used to command the SRT ACU via the remote interface. 
  * if the remote side disconnects or a problem affects the comunication line, this class try to reconnect to the ACU until the communication is up again..
*/
class CCommandSocket: public IRA::CSocket
{
public:
	/**
	 * Constructor.
	*/
	CCommandSocket();
	/**
	 * Destructor
	*/
	virtual ~CCommandSocket();
	
	/**
     * This member function is used to enstablish and configure the communication channel to the ACU. 
	 * This must be the first call before using any other function of this class.
	 * @param config pointer to the component configuration data structure
	 * @param data pointer to the ACU data structure
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c IRALibraryResource>
	*/
	void init(CConfiguration *config) throw (ComponentErrors::SocketErrorExImpl);
	
	/**
	 * Called to close the socket and free everything.
	 */
	void cleanUp();
	
	
	/**
	 * Force the socket to connect to the ACU. If the operation fails an exception is thrown, if the operation could not succesfully complete immediately 
	 * the command line state is set to <i>ACU::ACU_CNTDING</i>. It must be called by an external watchdog in order to keep the connection enstablished.
	 * This is not synchronized because this method acts on the socket only by accessing the mutex for the <i>CCommonData</i> class as the background thread of this socket @sa <i>onconnect()</i> 
	 */
	void connectSocket() throw (ComponentErrors::SocketErrorExImpl);
	
		/**
	 * This function can be used to send a command to the ACU.
	 * In case of problems it changes the component status accordingly. 
	 * @param cmd command to be sent
	 * @param len lenght in bytes of the command
	 * @throw TimeoutExImpl if the operation timeout before completing. 
	 * @thorw SocketErrorExImpl if the connection fails before completing. In that case the status is set to <i>Antenna::ACU_NOTCNTD</i>
	 */
	void sendCommand(std::string cmd,std::string outBuff, int all=0) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl);

		
protected:
	/**
	 * Automatically called by the framework as a result of a connection request. See super-class for more details.
	 * @param  ErrorCode connection operation result, zero means operation is succesful.
	*/
	void onConnect(int ErrorCode);
	

private:
	
	template <class X> class autoArray {
	public:
		autoArray(): m_message(NULL) {}
		~autoArray() { if (m_message!=NULL) delete [] m_message; }
		operator X *&() { return m_message; }
		operator const X *() const { return m_message; }
		X operator[](WORD pos) const { return m_message[pos]; }
		X& operator[](WORD pos) { return m_message[pos]; }
	private:
		X* m_message;
	};

	CConfiguration * m_pConfiguration;
	IRA::CSecureArea<CDBESMCommand> * m_pData;
	
	CDBESMCommand m_dbesm_command;

	bool m_bTimedout;
	
	/**
	 * This mutex is used to sync external calls to the public methods of the class. The requirements is that just one call at the time must be served if the methos requires to
	 * send a message to the ACU. the reason for that is that we need to wait to known the result of the previous command to the ACU that comes form the status socket.
	 */
	ACE_Recursive_Thread_Mutex m_syncMutex;
	
	
	/**
    * This member function is called to send a buffer to the ACU.
	 * @param Msg pointer to the byte buffer that contains the message for the ACU
	 * @param Len length of the buffer
	 * @param error It will contain the error description in case the operation fails.
	 * @return SUCCESS if the buffer was sent correctly, WOULDBLOCK if the send would block, FAIL in case of error, the parameter error is set accordingly.
	*/
	OperationResult sendBuffer(std::string Msg,CError& error);
	
	/**
	 * This methos will cycle to check is the ACU responded to the last command sent. There three possible scenarios:
	 * @arg the ACU responded and the operation was succesfull
	 * @arg the ACU did not responded yet (in that case a timeout policy is implemented)
	 * @arg the ACU responded but the operation was not completed correctly.
	 * @param commandTime marks the epoch the command message was sent to the ACU, used to apply the timeout mechanism.
	 * @throw AntennaErrors::NakExImpl if an error was repoerted by the ACU
	 * @throw TimeoutExImpl if the ACU does not report back the answer within the expected time
	 */
	void waitAck(const ACS::Time& commandTime) throw (BackendsErrors::NakExImpl,ComponentErrors::TimeoutExImpl);
		
  int CCommandSocket::receiveBuffer(std::string Msg,CError& error, int all=0);
	
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
	
	void createSocket() throw (ComponentErrors::SocketErrorExImpl);
	
};





#endif /*COMMANDSOCKET_H_*/
