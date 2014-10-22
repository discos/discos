#ifndef _SOCKET_CLIENT
#define _SOCKET_CLIENT

/** 
 *  
 * Osservatorio Astrofisico di Arcetri
 *                                                                           
 * This code is under GNU General Public Licence (GPL).                                                          
 *                                                                                                              
 *  Who                              				  when            What                                       
 * 																      Creation                                                  
 */

#include "SocketConnection.h"

/**
 * Socket connection, client side
 * Implements the client side of the abstract class SocketConnection
 */ 

class SocketClient 
	: public SocketConnection
{
  public:
    /** 
	 * Constructor.
	 * @param address is a string that contain IP address
	 * @param port is a number of port of IP connection
	 */
	SocketClient(const char * address, int port);
	
    /** 
	 * Constructor.
	 */
	SocketClient(void);
    
	/**
	 * Destructor.
	 */
    ~SocketClient();	
    
    /**
	 * This method open the socket connetion	 
	 * @param address is a string that contain IP address
	 * @param port is a number of port of IP connection
	 */
    bool OpenConnection(const char *address, int port);
	
    /**
	 * This method close the socket connetion
	 */
    bool CloseConnection() { return SocketConnection::CloseConnection(); }
};

#endif /* _SOCKET_CLIENT */
