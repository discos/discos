#ifndef SOCKETSERVER_H_
#define SOCKETSERVER_H_

/** 
 *  
 * Osservatorio Astrofisico di Arcetri
 *                                                                           
 * This code is under GNU General Public Licence (GPL).                                                          
 *                                                                                                              
 *  Who                              				  when            What                                       
 * 	Liliana Branciforti(bliliana@arcetri.astro.it)	11/05/2009		 Creation                                                  
 */

#include "SocketConnection.h"

/**
 * Socket connection, server side
 * Implements the server side of the abstract class SocketConnection
 */ 
class SocketServer
	: public SocketConnection
{
public:
    /** 
	 * Constructor.
	 * @param address is a string that contain IP address
	 * @param port is a number of port of IP connection
	 */
	SocketServer(const char *address, int port);
    
	/** 
	 * Constructor.
	 */
	SocketServer(void);
    
	/**
	 * Destructor.
	 */
	~SocketServer();
    
	/**
	 * This method open the socket connetion	 
	 * @param address is a string that contain IP address
	 * @param port is a number of port of IP connection
	 */
	bool OpenConnection(const char * addr, int port);
    
	/**
	 * This method close the socket connetion
	 */
	bool CloseConnection() { return SocketConnection::CloseConnection(); }
protected:
	
	/**
	 * This method open the socket connetion	 
	 * @param address is a string that contain IP address
	 * @param port is a number of port of IP connection
	 */
	int open_socket (const char * addr,int port);
	
	/**
	 * This method wait the connetion request of client 
	 * @param s is a code of socket connetion
	 */
	int get_connect(int s);
};

#endif /*SOCKETSERVER_H_*/
