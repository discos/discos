#ifndef SOCKETC_HPP
#define SOCKETC_HPP

/** 
 *  
 * Osservatorio Astrofisico di Arcetri
 *                                                                           
 * This code is under GNU General Public Licence (GPL).                                                          
 *                                                                                                              
 *  Who                              				  when            What                                       
 * 																      Creation                                                  
 */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

  /**
	* Socket Connection
	* Abstract class, must be implemented in pairs (on different programs),
	*  a client and a server.
	*
	*  A server opens a socket, and waits for incoming connections. 
	*  A client connects to a client socket
	*
	*  After this, both parts can send and receive ASCII lines 
	*  (terminated by \0, \r or \n) to the other partner. 
	*  
	*  PutLine is unconditional, GetLine can be blocking (waits for a 
	*  line if none available) or non blocking (returns 0 if no line 
	*  available), depending on the "wait" parameter. The returned
	*  line is allocated with new, so it MUST be disposed with delete[]
	*/

class SocketConnection 
{	
 private:
        char *buffer;/*!< is a pointer of area of memory*/
        int nread;/*!< */
        int wdfds;/*!< */
        fd_set fdset;/*!< */
        
 protected:
	int sock;/*!< is the code of socket connection */
	int error/*!< is the code of error*/;
        
 public:
    /** 
	 * Constructor.
	 * @param socket is the number to creation the connection. The default is 0.
	 */
	 SocketConnection(int socket=0);
    
    /**
	 * Destructor.
	 */
    ~SocketConnection();
    
    /**
     * This method reads a line from the socket. 
     * Terminates when a whole line has been received, or more than maxlen
     * bytes have been read. Always null terminate the line, deleting the \n
     */
    char * GetLine(bool wait=true);
    
    /**
     * This method sends a line over the interface
     * Performs multiple write until whole line has been sent
     * Converts terminal \0 to \n
     */
    const char *PutLine(const char * line);

	 /** 
	  * This method returns the Last Error of connection.
	  * @return a numer code error.
	  */
    int LastError() {int err = error; error=0; return err; }
 	
   	bool operator! () const { return (sock == 0); }

	 /** 
	  * This method returns the state of connection.
	  * @return true if it isn't connect.
	  */
   	bool Disconnected() const { return (bool)!(*this); }
	
    /** 
	  * This method returns the state of connection.
	  * @return true if it is connect.
	  */
   	bool Connected() const { return (sock != 0); }
   	
	 /** 
	  * This method method returns the number of flushes socket.
	  * @return a integer number of chars flushed.
	  */
	int Flush(); 
 protected:
	  /**
	   * The CloseConnection method attempts to close the socket connetion
	   */
	 bool CloseConnection();      
	           
	  /**
	   * The initAttributes method attempts to initialize the connetion object 
	   */
	 bool InitConnection(int socket);
};

#endif /* SOCKETC_HPP */
