#ifndef _SOCKET_H
#define _SOCKET_H
/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  29/11/2004      Creation                                          */
/* Andrea Orlati(aorlati@ira.inaf.it)  22/12/2004      Added event notification framework				        */
/* Andrea Orlati(aorlati@ira.inaf.it)  24/01/2005      Added error reporting by class CError                */
/* Andrea Orlati(aorlati@ira.inaf.it)  10/02/2005      Class name changed from CAsyncSocket to CSocket      */
/* Andrea Orlati(aorlati@ira.inaf.it)  22/08/2005      timer handling performed through CIRATOOLS functions */
/* Andrea Orlati(aorlati@ira.inaf.it)  05/10/2005      Added the disconnect function	     					     */
/* Andrea Orlati(aorlati@ira.inaf.it)  21/09/2006      Changed thread management in order to port to ACS5.0.4*/

// $Id: Socket.h,v 1.1.1.1 2007-05-03 15:15:48 a.orlati Exp $

#include <map>

namespace IRA {

class CSocketFramework;

/**
 * This code implements a class that can be used to comunicate to any device via sockets. This class can operate using both 
 * TCP and UDP protocols. This means STREAM, sequenced, full duplex, connection-based byte stream sockets and DataGRAM,
 * connectionless, unreliable packets sockets. DGRAM sockets can also send data using broadcast transmission. This class can be
 * used for both client (speaker) and server (listener) side. Depending on the socket type (STREAM or DGRAM) the normal sequence 
 * of call after object creation is : (STREAM CLIENT) <i>Create()</i>, <i>Connect()</i>, <i>Send()</i>, <i>Close()</i>; (STREAM SERVER)
 * <i>Create()</i>, <i>Listen()</i>, <i>Accept()</i>, <i>Receive()</i>, <i>Close()</i>; (DGRAM SPEAKER) <i>Create()</i>, <i>Send()</i>,
 * <i>Close()</i>; (DGRAM LISTENER) <i>Create()</i>, <i>Receive()</i>, <i>Close()</i>. After the call to method <i>Create()</i> it is also
 * possible to configure the semantics of APIs as non-blocking, blocking or blocking with timeout. Blocking mode blocks the program
 * until the operation has finished (successfully or not). Timeout variant has the same behavior of blocking mode except for that API
 * will remain blocked until the operation can be completed or timeout expires. Non-blocking mode forces the APIs to never block, 
 * if they can't complete the operation imediately they will exit with a WOULDBLOCK code. If the programmer chooses non-blocking paradigm he can
 * also adopt the event-driven system; in fact by registering the event he can then override the associated method that is automatically 
 * called when that event takes place. For more details about this tecnique take a look at the <i>CSocketFramework</i> class.  If error conditions 
 * are found, a CError object is filled with the corrispondent code. Table below resumes returned error codes that developers should dealt with.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
 * <b>ERROR CODE TABLE</b><br>  
 * Error type <i>CError::ErrorType</i><br>
 * <table border=2>
 *	<tr><th>#</th><th>Memonic</th><th>ShortMessage</th></tr>
 * <tr><td><a name="Excp1"> 1</a></td><td>InvalidIPAddr</td><td>"Address is not valid or does not exist"</td></tr>
 * <tr><td><a name="Excp2"> 2</a></td><td>SockCreationError</td><td>"Socket creation failed"</td></tr>
 * <tr><td><a name="Excp3"> 3</a></td><td>SockBindError</td><td>"Socket binding failed"</td></tr>
 * <tr><td><a name="Excp4"> 4</a></td><td>ListenError</td><td>"Error encountered while setting up the listening queue"</td></tr>
 * <tr><td><a name="Excp5"> 5</a></td><td>ConnectError</td><td>"Error encountered during connection"</td></tr>
 * <tr><td><a name="Excp6"> 6</a></td><td>SocketNCreated</td><td>"Socket must be created before any operation"</td></tr>
 * <tr><td><a name="Excp7"> 7</a></td><td>AcceptError</td><td>"Error encountered while looking for new connections"</td></tr>
 * <tr><td><a name="Excp8"> 8</a></td><td>SendError</td><td>"Error encountered while trying to send data"</td></tr>
 * <tr><td><a name="Excp9"> 9</a></td><td>ReceiveError</td><td>"Error encountered while trying to receive data"</td></tr>
 * <tr><td><a name="Excp10">10</a></td><td>UnspecifiedHost</td><td>"Destination or starting point was not specified"</td></tr>
 * <tr><td><a name="Excp11">11</a></td><td>SockConfiguration</td><td>"Socket configuration parameter was not configured"</td></tr>  
 * <tr><td><a name="Excp12">12</a></td><td>OperationNPermitted</td><td>"Operation not permitted with this configuration or socket status"</td></tr> 
 * </table>
*/
class CSocket
{
public:
	friend class CSocketFramework;
	/**
	 * <a name="Enum1"> List of all possible socket status. </a>
	*/
	enum SocketStatus {
		NOTCREATED, 	/*!< socket is not created, no operation is possible */
		READY, 			/*!< socket is ready to perform any operation */
		ACCEPTING, 		/*!< socket is looking for incoming connection */
		CONNECTING   	/*!< socket is trying to connect to another machine */
	};
	/**
	 * <a name="Enum2"> List of socket types. </a> 
	*/
	enum SocketType {
		STREAM, /*!< connection-oriented, byte stream socket (tcp) */
		DGRAM   /*!< connectionless, unreliable packets (udp) */
	};
	/**
	 * <a name="Enum3"> List of all possible modes. </a>
	*/
	enum SocketMode {
		NONBLOCKING,   /*!< socket is in non-blocking mode, all operation will exit if they can't be performed imediately */
		BLOCKING,       /*!< socket is in blocking mode, all operation will exit only when their work finishes */ 
		BLOCKINGTIMEO,  /*!< socket id in blocking mode, send and receive will exit when either thier work finishes or timeout expires */
	};
	/**
	 * <a name="Enum4"> List of all possible event in case of the asynchronous mechanism is used. </a>
	*/
	enum AsyncEvent {
		E_RECEIVE=1,		/*!< want to receive notification of readiness for reading */
		E_SEND=2,			/*!< want to receive notification of readiness for writing */
		E_ACCEPT=4,			/*!< want to receive notification of incoming connections*/
		E_CONNECT=8,		/*!< want to receive notification of connection result */
		E_ERROR=16,			/*!< want to receive notification of network asynchronous errors */
	};
	/**
	 * List of possible operation results.
	*/
	enum OperationResult {
		FAIL=-3,			/*!< operation is not pertinent or fails */
		WOULDBLOCK=-2, /*!< operation would block the process while the socket is non-blocking */
		SUCCESS=-1,    /*!< operation completed succesfully */
	};
	/**
	 * Constructs a blank socket object. After the construction you must call its <i>Create</i> member
	 * function to create socket data structure and bind its address. On server side, when the socket is
	 * obtained from an <i>Accept</i> you do not need to call <i>Create</i>
	*/
	CSocket();
	/**
	 * Destructor
	*/
	virtual ~CSocket();
	/**
	 * Call the <i>Create</i> member function after constructing a CSocket object. <i>Create</i>
	 * binds the socket to the specified address and port number. <i>Create</i> supports two socket types
	 * (see SocketTypes): STREAM, sequenced, full duplex, connection-based byte stream, uses the TCP. DGRAM,
	 * connectionless, unreliable packets, uses UDP. if both Address and Port are not specified (left default values)
	 * socket is not binded. By default the socket is created in BLOCKING mode.
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 1 <a href="#Excp1">InvalidIPAddr</a>
	 * 		 @arg \c 2 <a href="#Excp2">SockCreationError</a>
	 * 		 @arg \c 3 <a href="#Excp3">SockBindError</a>
	 * 		 @arg \c 11 <a href="#Excp11">SockConfiguration</a>
	 * @param Type stream socket or datagram socket. Stream socket is the default.
	 * @param SocketPort port to be used with the socket, 0(default) if you want the kernel to select a port for you. This is
	 *        significant only for the server and receiving side. 
	 * @param IPAddr a string containing the network address used for binding the socket, such as "192.168.0.1". If the default
	 *        value of NULL is given, the socket is binded with any address (0.0.0.0)
	 * @param froceBind this will force the operating system kernel to reuse a local address avoiding the "address already in use" error
	 *        during binding.
	 * @return SUCCESS if the operation succeeds, FAIL in case of a problem.
	*/
	OperationResult Create(CError& Err,SocketType Type=STREAM,WORD SocketPort=0,CString *IPAddr=NULL,bool forceBind=false);
	/**
	 * Before <i>Accept</i>ing connection this function expresses the willingness to accept for incoming connection requests.
	 * <i>Listen</i> specifies also the queue limit for incoming connections. This function applies only to STREAM sockets.
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 4 <a href="#Excp4">ListenError</a>
	 * 		 @arg \c 6 <a href="#Excp6">SocketNCreated</a>
	 *       @arg \c 12 <a href="#Excp12">OperationNPermitted</a>
	 * @param Backlog maximum length to which the queue of pending connections can grow.
	 * @return SUCCESS if the socket is a STREAM, FAIL if this function is called on a DGRAM socket or in case of error.
	*/
	OperationResult Listen(CError& Err,int Backlog=5);
	/**
	 * Call this function to accept a connection on the socket. This routine extracts the first connection in the queue, then
	 * creates a new socket with almost the same properties(including Mode) of the original socket. If there are not any pending 
	 * connections and the socket is BLOCKING then the function blocks util the next connection arrives, instead if the socket
	 * is NONBLOCKING the function exits immediately. This functions applies only to STREAM sockets. The original socket 
	 * is not affected by this call. 
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 2 <a href="#Excp2">SockCreationError</a>
	 * 		 @arg \c 6 <a href="#Excp6">SocketNCreated</a>
	 * 		 @arg \c 7 <a href="#Excp7">AcceptError</a>
	 * 		 @arg \c 11 <a href="#Excp11">SockConfiguration</a>
	 *        @arg \c 12 <a href="#Excp12">OperationNPermitted</a>
	 * @param ConnectedSocket reference to the newly created socket.
	 * @return SUCCESS if the function complete and the socket is returned, WOULDBLOCK if the socket is non-blocking 
	 * and the operation would have blocked, FAIL if the socket is a DGRAM or in case of error.
	*/
	virtual OperationResult Accept(CError& Err,CSocket &ConnectedSocket);
	/**
	 * Call this member function to establish a connection to an unconnected stream or datagram socket. This functions is  
	 * suitable for the client side. For STREAM sockets an active connection is effectively initiated, for DGRAM sockets 
	 * a default destination is set for future uses of <i>Send</i>; this default address is eventually overwtitten by calling
	 * <i>setBroadcast</i>.
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 1 <a href="#Excp1">InvalidIPAddr</a> 
	 * 		 @arg \c 5 <a href="#Excp2">ConnectError</a>
	 * 		 @arg \c 6 <a href="#Excp6">SocketNCreated</a>
	 * @param IPAddr a string containing the network address to which this object is connected, such as "192.168.0.1".
	 * @param Port the port that identifies the server socket application
	 * @return SUCCESS if the function complete, else WOULDBLOCK if the socket is non-blocking and the operation would have blocked. FAIL
	 *         if an error occurs.
	*/
	OperationResult Connect(CError& Err,CString IPAddr,WORD Port);
	/**
	 * Use this member function to setup the needed socket mode (blocking, nonblocking, ...) and eventually read/write 
	 * operation timeouts.
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 6 <a href="#Excp6">SocketNCreated</a>
	 * 		 @arg \c 11 <a href="#Excp11">SockConfiguration</a>
	 * @param Mode choose the proper <a href="#Enum2">socket mode</a>
	 * @param recvTimeout time reserved for receive operation to complete (microseconds), this is significant only if BLOCKINGTIMEO mode 
	 *        has been specified. The default value of zero means an infinite time which is semantically the same of BLOCKING mode.
	 * @param sndTimeout time reserved for sned operation to complete (microseconds), this is significant only if BLOCKINGTIMEO mode 
	 *        has been specified. The default value of zero means an infinite time which is semantically the same of BLOCKING mode.
	 * @return SUCCESS if function succeeds, FAIL if the function encountered an error.
	*/
	OperationResult setSockMode(CError& Err,SocketMode Mode,int recvTimeout=0,int sndTimeout=0);
	/**
	 * This function manipulates option associated with the socket. Options can exist at multiple protocol levels. However, at the uppermost socket level
	 * the options are always present . <a name="SockOpt">The following options are supported</a>:
	 * <ul>
	 * <li><i>SO_DEBUG</i> Provides the ability to turn on recording of debugging information. This is a BOOL option</li>
	 * <li><i>SO_BROADCAST</i> Permits sending of broadcast messages. This is a BOOL option </li>
	 * <li><i>SO_REUSEADDR</i> Specifies tha the rules used in validating addresses should allow reuse of local addresses. 
	 * This is a BOOL options</li>
	 * <li><i>SO_KEEPALIVE</i> Keeps connections alive by transmitting periodic messages to the remote side. If the connected socket
	 * fails to respond, teh connection is broken and the writing process is notified with an error. This is a BOOL option</li>
	 * <li><i>SO_LINGER</i> if this option is set the system blocks the process during <i>Close()</i> until it can transmit all
	 * pending data. This option accepts a <i>linger</i> structure</li>
	 * <li><i>SO_SNDBUF</i> Sets buffer size for sending operations. This is an integer option</li>
	 * <li><i>SO_RCVBUF</i> Sets buffer size for receiving oprations. This is an integer option</li></li>
	 * <li><i>SO_DONTROUTE</i> Specifies that outgoing messages should bypass the standard routing facilities. The destination
	 * must be on a directly-connected network. This is a BOOL option.</li>
	 * <li><i>SO_OOBINLINE</i> Receives out-of-band data in the normal data stream. This is a BOOL option</li>
	 * <li><i>SO_ERROR</i> Get and clear pending socket errors. Only valid for <i>getSockOption</i> This is an integer option</li>
	 * <li><i>SO_NODELAY</i> Disables the algorithm used to reduce the number of small packets sent by buffering. This is the only
	 * option that uses IPPROTO_TCP level; all the other uses SOL_SOCKET. This is a BOOL option</li>
	 * </ul>
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 6 <a href="#Excp6">SocketNCreated</a>
	 * 		 @arg \c 11 <a href="#Excp11">SockConfiguration</a>
	 * @param Level the level to which the option is defined; the only supported levels are SOL_SOCKET and IPPROTO_TCP
	 * @param Opt the socket option for which the value is to be set
	 * @param OptVal a pointer to the buffer in which the value for the requested option is supplied
	 * @param Optlen the size of the buffer supplied
	 * @return SUCCESS if the operation succeeded, FAIL in case of error.
	*/
	OperationResult setSockOption(CError& Err,int Opt,const void *OptVal,WORD Optlen,int Level=SOL_SOCKET);
	/**
	 * This function reads the option values associated with the socket. Options can exist at multiple protocol levels. 
	 * However, at the uppermost socket level the options are always present  The <a href="#SockOpt"> list of supported options </a> 
	 * is the same one of <i>setSockOptions</i>.
	 * @param Err eventually used to return an error code:
 	 * 		 @arg \c 6 <a href="#Excp6">SocketNCreated</a>
	 * 		 @arg \c 11 <a href="#Excp11">SockConfiguration</a>
 	 * @param Level the level to which the option is defined; the only supported levels are SOL_SOCKET and IPPROTO_TCP
	 * @param Opt the socket option for which the value must be read
	 * @param OptVal a pointer to the buffer in which the requested is stored
	 * @param Optlen the size of the buffer supplied by the function
	 * @return SUCCESS if the operation succeeded, FAIL in case of error.	 
	*/
	OperationResult getSockOption(CError& Err,int Opt,void *OptVal,WORD *Optlen,int Level=SOL_SOCKET);
	/**
	 * Call this function to send data to another socket. <i>Send</i> is used to write outgoing data on connected STREAM sockets or
	 * to DGRAM sockets. Data are sended to the socket specified by a previous call to <i>Connect</i>. For DGRAM sockets it is also 
	 * possible to use an alternative address by setting up last two parameters. If none of the addressing methods is used, an expception
	 * will be risen. Note that for datagram sockets the successful completion of a <i>Send</i> doesn't mean that the data was delivered.
	 * On objectes of type STREAM, the number of bytes written can be between 1 and the requested length, depending on buffer availability
	 * on both the local and foreign machine; this staff is up to the programmer.
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 1 <a href="#Excp1">InvalidIPAddr</a> 
	 * 		 @arg \c 6 <a href="#Excp6">SocketNCreated</a>
	 * 		 @arg \c 8 <a href="#Excp8">SendError</a>
	 * 		 @arg \c 10 <a href="#Excp10">UnspecifiedHost</a>(in case of DGRAM sockets)
	 *        @arg \c 12 <a href="#Excp12">OperationNPermitted</a>
	 * @param Buff a buffer containing the data to be transmitted
	 * @param BuffLen the length of data in the buffer
	 * @param Port the port number that identifies the application waiting for the data on the remote side. Ignored by STREAM sockets.
	 * @param Addr pointer to a string containing the network address to which send data, such as "192.168.0.1". Ignored by STREAM sockets.
	 * @return if no error occurs, the total number of bytes sent(this could be less than the length of the buffer). WOULDBLOCK is
	 *         returned in case an the socket is NON-BLOCKING and the operation would have blocked or the socket is BLOCKING and the timeout 
	 *         expires. A 0 is returned in case the remote side has shut down the connection (STREAM sockets). FAIL if the socket is not
     * 		   ready for this operation (i.e a connection operation is started in asynchronous mode and not yet completed or a listen 
	 *         has been called) or more generally when an error occurs.
	*/
	virtual int Send(CError& Err,const void *Buff,WORD BuffLen,WORD Port=0,CString *Addr=NULL);
	/**
	 * Call this member function to receive data from a socket. <i>Receive</i> is used to read incoming data from connected STREAM
	 * sockets or DGRAM sockets. For datagram sockets, data is extracted from the first enqueued datagram, up to the size of 
	 * the supplied buffer. If the size of the buffer is smaller than the datagram, the excess of data is lost. If the socket is a
	 * DGRAM the sender of the message is also returned.
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 6 <a href="#Excp6">SocketNCreated</a>
	 * 		 @arg \c 8 <a href="#Excp9">ReceiveError</a>
	 *       @arg \c 12 <a href="#Excp12">OperationNPermitted</a>
	 * @param Buff a buffer for the incoming data
	 * @param BuffLen the length of the buffer
	 * @param Port if not NULL will contain the number that identifies the application who sent the data from the remote side. 
	 *        Ignored by STREAM sockets.
	 * @param Addr pointer to a string that will contain the IP address from which the data was transmitted. Ignored by STREAM sockets.
	 * @return if no error occurs, the total number of bytes received. WOULDBLOCK is returned in case the socket is NON-BLOCKING 
	 *         and the operation would have blocked or the socket is BLOCKING and the timeout expires. a 0 is returned in case the remote 
	 *         side has shut down the connection (STREAM). FAIL if the socket is not ready for this operation (i.e a connection operation 
	 *         is started in asynchronous mode and not yet completed or a listen has been called) or more generally when an error occurs.
	*/
	virtual int Receive(CError& Err,void *Buff,WORD BuffLen,WORD* Port=NULL,CString* Addr=NULL);
	/**
	 * This function closes the socket. More precisely, it releases the file descriptor, so that no further references will
	 * be possible. It reset the socket to its original state, before the call to <i>Create()</i> The semantics of <i>Close()</i> 
	 * is affected by the configuration of the SO_LINGER socket parameter.
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 6 <a href="#Excp6">SocketNCreated</a>
	 * @return SUCCESS if the close operation completed
	*/
	OperationResult Close(CError& Err);	
	/**
	 * Use this function to enable a broadcast communication. After this call, all subsequent calls to <i>Send</i> will transmitt
	 * the data as a broadcast packet. Socket is also configured accordingly (no need to setup the <i>SO_BROADCAST</i> option).
	 * This function is used only for DGRAM sockets.
	 * @param Err eventually used to return an error code:	 
	 * 		 @arg \c 6 <a href="#Excp6">SocketNCreated</a>
	 * 		 @arg \c 11 <a href="#Excp11">SockConfiguration</a>
	 *        @arg \c 12 <a href="#Excp12">OperationNPermitted</a>
	 * @param Port the port that identifies the application listening for the data
	 * @param Addr pointer to the string containing the broadcast address (i.e. 192.167.189.255). If NULL (default) the standard broadcast
	 *        address is used (255.255.255.255).
	 * @return SUCCESS if function succeeds, FAIL if the function is used for a STREAM socket or an error occurs.
	*/
	OperationResult setBroadcast(CError& Err,WORD Port,CString *Addr=NULL);
	/**
	 * Call this member function to receive event notification for the socket. This function is used to specify which functions will
	 * be called for the socket. It also premits to specifies the timeout for each event. This function forces the socket in non-blocking
	 * mode. This configuration will remain even if all event notifications are turned off.
	 * @param Err eventually used to return an error code:
	 * 		 @arg \c 6 <a href="#Excp6">SocketNCreated</a>
	 * 		 @arg \c 11 <a href="#Excp11">SockConfiguration</a>
	 * @param Event specifies the event in which the application is interested. See <a href="#Enum4">enum AsyncEvent</a> 
	 *       for a list of all events. 
	 * @param Enabled turn on(true) and off(false) the notification for the event
	 * @param Timeout time allocated for that event to hapen (microseconds). If the time expires the <i>onTimeout</i> function
	 *        is executed. A zero instructs the socket to wait undefinitly. if Enabled is false or Event is E_ERROR this parameter 
	 *        is ignored.
	 * @return SUCCESS if function succeeds, FAIL if the function encountered an error.
	*/
	OperationResult EventSelect(CError& Err,AsyncEvent Event,bool Enabled=true,DDWORD Timeout=0);
	/**
	 * Call this function to get the local name for a socket. This call provides the only way by which it's possible to
	 * determine the local associations which have been set by the system.
	 * @param HostAddr Reference to a string which receive a dotted number IP address. In case of error this is an empty string
	 * @param Port Reference to word that stores the port number.
	*/
	void getSocketName(CString &HostAddr,WORD &Port);
	/** 
	 * Call this member function to get the address of the peer socket to which the socket is connected.
	 * @param RemoteAddr Reference to a string which receive a dotted number IP address. In case of error this is an empty string
	 * @param RemotePort Reference to word that stores the remote port number.
	*/
	void getPeerName(CString &RemoteAddr,WORD &RemotePort); 
	/**
	 * @return the string containing the name of the host that has the specified IP address. If the address is not valid 
	 *         an empty string is returned.
	*/
	static CString getHostNamebyAddr(CString Addr);
	/**
	 * @return a string containing the address (byte, dotted) returned by the DNS for the specified host name. if the name is 
	 *         unkonw an empty string is returned.
	*/
	static CString getAddrbyHostName(CString Host);
	/**
	 * @return a string containing the name of the host of the current processor. In case of error an empty string is returned.
	*/
	static CString getHostName();
	/**
	 * @return the current socket status. <a href="#Enum3"> See enum SocketStatus. </a>
	*/
	SocketStatus getStatus() const;
	/**
	 * @return the socket mode: BLOCKING, BLOCKINGTIMEO or NONBLOCKING. <a href="#Enum1"> See enum SocketMode. </a>
	*/
	SocketMode getMode() const;
	/**
	 * @return the socket type: STREAM or DGRAM. <a href="#Enum2"> See enum SocketType. </a>
	*/
	SocketType getType() const;
protected:
	/** Socket file discriptor */
	int m_iSocket;
	/** Socket type */
	SocketType m_Type;
	/** Socket current status */
	SocketStatus m_Status;
	/** Socket current mode */
	SocketMode m_Mode;
	// used for dgram sockets....if a default interlocutor to be used with send/receive
	/** Indicates if a default address has been selected */
	bool m_bDefaultRemote;
	/** Remote address used for default remote side address in DGRAM sockets */
	CString m_sRemoteAddress;
	/** Remote port used for default remote side address in DGRAM sockets */
	WORD m_wRemotePort;
	/** Sockaddr struct filled with default remote side information */
	struct sockaddr_in m_RemoteInfo;
	/** Pointer to the socket framework */
	CSocketFramework *m_pHelper;
	/** Bitfield mask indicating the registered events */
	WORD m_wAsyncFlag;
	/** Sets the current socket mode */
	void setMode(SocketMode Mode);
	/** Sets the current socket status */
	void setStatus(SocketStatus Status);
	/** Sets the current socket type */
	void setType(SocketType Type);
	/**
	 * Never call this function  directly. This is called by the framework if a registered event timeout expires. Override this function
	 * to handle this case. The programmer can distinguish which events generated the timeout by watching at the input parameter. 
	 * Timeput are checked only for registered events by <i>AsyncSelect</i>.
	 * @param EventMask bitfiled that specifies the combination of <a href="#Enum4">network events</a> that generated the timeout 
	 *        expiration.
	*/
	virtual void onTimeout(WORD EventMask);
	/**
	 * Never call this function  directly. This is called by the framework when a new connection request is enqueued.
	 * Override this function to perform particular operation when a client connected to the socket. In order for this function
	 * to be called the E_ACCEPT event must be enabled, the socket is a STREAM and the <i>Listen()</i> function must have been called.
	 * @param Counter number of time the event occured from the creation of the socket.
	*/
	virtual void onAccept(DWORD Counter);
	/**
	 * Never call this function  directly. This is called by the framework when an outgoing connection finished. Override this function
	 * to perform particular operation when the connection operation succeeds or fails. In order for this functionto be called the 
	 * E_CONNECT event must be enabled, the socket must be connection-oriented and the <i>Connect()</i> function must have been called.
	 * @param ErrorCode used to determine if the connection completes succesfully or not. Zero means success, otherwise the error code
	 *        is reported. This code is retrieved with the SO_ERROR option. 
	*/
	virtual void onConnect(int ErrorCode);
	/**
	 * Never call this function  directly. This is called by the framework to notify this socket that there is data in the buffer that can
	 * be retrieved by the <i>Receive()</i> member function. Override this function to react to this event. In order for this 
	 * function to be called the E_RECEIVE event must be enabled.
	 * @param Counter number of time the event occured from the creation of the socket.
	 * @param Boost if true the next I/O operation is called as soon as possible; in other words this will ignore the time slice set by the call to
	 *        <i>setSleepTime()</i>. This will optimize the throughput. If not set the default value is false.
	*/
	virtual void onReceive(DWORD Counter,bool &Boost);
	/**
	 * Never call this function  directly. This is called by the framework to notify this socket that it can now send data by 
	 * calling the <i>Send()</i> member function. Override this function to react to this event. In order for this 
	 * function to be called the E_SEND event must be enabled.
	 * @param Counter number of time the event occured from the creation of the socket.
	 * @param Boost if true the next I/O operation is called as soon as possible; in other words this will ignore the time slice set by the call to
	 *        <i>setSleepTime()</i>. This will optimize the throughput. If not set the default value is false.
	*/
	virtual void onSend(DWORD Counter,bool &Boost);
	/**
	 * Never call this function  directly. This is called by the framework to notify an asynchronous error occured. 
	 * Override this function to react to this event. In order for this function to be called the E_ERROR event must be enabled.
	 * @param Counter number of time the event occured from the creation of the socket.
	 * @param ErrorCode used to determine which error happened. This code is retrieved with the SO_ERROR option. 
	*/
	virtual void onError(DWORD Counter,int ErrorCode);
	/**
	 * This function is protected because should be used only when asynchronous mode is used and at least one of the event-driven-function are
	 * overriden. This member is used to setup the sleep time of the framework, in other words it setup the time slice between two checks of 
	 * of possible events. The shorter is this time the more precise is the system to detect events but the heavier is the cpu occupancy.
	 * @param Timebit this is the time slice in microsenconds. The default is 100000 (one tens of second).
	*/
	void setSleepTime(DWORD Timebit=100000);
	/**
	* Initialize the members of this class.
	*/
	void fastInit();
private:
	BACIMutex m_LocalMutex;
   CSocket(const CSocket& rSrc);    // no implementation given
   void operator=(const CSocket& rSrc);  // no implementation given
};

/**
 * This class is for internal use, it builds up a framework for the CSocket class. So don't create any object from this class
 * It basically creates a thread that use the system call poll() to monitor the socket file descriptor and detect the event that may
 * take place. If an event was previously registered, the thread will call the associated CSocket method. The developer just have
 * to override this method in order to react to that event. If a timeout value is specified, a special event will be risen if nothing
 * happens in that period.
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
 */
class CSocketFramework {
public:
	/**
	 * Constructs the framework and it associates it to the socket that must be monitored.
	 * @param Sock pointer to the socket.
	*/
	CSocketFramework(CSocket *Sock);
	/**
	 * Destructor.
	*/
	~CSocketFramework();
	/**
    * Called by CAsyncsocket when the programmer wants to register or unregister an event.
	 * @param Event specifis the event the framework must listen to.
	 * @param Enable true to register the event, false to unregister.
	 * @param Timeout timeout associated to the event (microseconds), if the event does not occur before this time expires,
	 *        a special event is risen.
	 * @return true is the event is succesfully registered, otherwise false.
	*/
	bool Event(WORD Event,bool Enable,DDWORD Timeout);
	/**
	 * Called by CSocket when the programmer specifies the framework time bit.
	 * @param STime time slice in microseconds
	*/
	void setSleep(DWORD STime);
private:
	typedef struct {
		DDWORD Timeout;
		TIMEVALUE LastTime;
		DWORD EventCount;
	} TEventInfo;
	typedef std::map<WORD,TEventInfo*> THashTable;	
	class TThreadArea {
	public:
		TThreadArea();
		~TThreadArea();
		bool addEvent(WORD Event,DDWORD Timeout);
		bool removeEvent(WORD Event);
		DWORD getSleepTime();
		void setSleepTime(DWORD STime);
		// Update events information... when they happen, return the events counter
		DWORD updateEvent(WORD Event);
		WORD getExpiredEvents();
		inline void setSocket(CSocket *Sock) { Socket=Sock; }
		inline CSocket *getSocket() { return Socket; }
		inline WORD getSize() { return ElCount; }
	private:
		BACIMutex Mutex;
		WORD ElCount;
		THashTable Elements;
		CSocket *Socket;
		DWORD SleepTime;
		bool EventExist(WORD Event,THashTable::iterator &ff);
	};
	TThreadArea m_ThreadArea;
	ACS::ThreadBase *m_pThread;
	ACS::ThreadManagerBase m_threadManager;
	CSocketFramework(const CSocketFramework& rSrc);    // no implementation given
   void operator=(const CSocketFramework& rSrc);  // no implementation given
	static void worker(void *threadParam);
};

}

#endif
