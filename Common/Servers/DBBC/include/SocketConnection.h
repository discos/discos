//
// Socket Connection
// Abstract class, must be implemented in pairs (on different programs),
//  a client and a server.
//
//  A server opens a socket, and waits for incoming connections. 
//  A client connects to a client socket
//
//  After this, both parts can send and receive ASCII lines 
//  (terminated by \0, \r or \n) to the other partner. 
//  
//  PutLine is unconditional, GetLine can be blocking (waits for a 
//  line if none available) or non blocking (returns 0 if no line 
//  available), depending on the "wait" parameter. The returned
//  line is allocated with new, so it MUST be disposed with delete[]
//
#ifndef SOCKETC_HPP
#define SOCKETC_HPP
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

class SocketConnection 
{
	// Attributes
 private:
        char *buffer;
        int nread;
        int wdfds;
        fd_set fdset;
 protected:
	int sock;
	int error;
        // Services
 public:
	SocketConnection(int socket=0);
        char * GetLine(bool wait=true);
        const char *PutLine(const char * line);
	~SocketConnection();
 	int LastError() {int err = error; error=0; return err; }
   	bool operator! () const { return (sock == 0); }
		   // Ritorna true se la connessione NON e'aperta
   	bool Disconnected() const { return (bool)!(*this); }
	bool Connected() const { return (sock != 0); }
	int Flush(); // Flushes socket. Returns n. of chars flushed.
	int GetBuffer(char *buf, int len, bool wait = true); 
					// Read binary buffer. 
					// Returns n. of chars read
 protected:
	bool CloseConnection();      // chiude la connessione
                       // E' privato perche' poi non c'e' modo di riaprirla.
        bool InitConnection(int socket); // socket DEVE essere una socket connessa
};

#endif /* SOCKETC_HPP */
