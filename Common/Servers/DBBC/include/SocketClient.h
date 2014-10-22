//
// Socket connection, client side
// Implements the client side of the abstract class SocketConnection
//
#ifndef _SOCKET_CLIENT
#define _SOCKET_CLIENT
#include "SocketConnection.h"

class SocketClient 
	: public SocketConnection
{
  // Attributi
  // Servizi
  public:
    SocketClient(const char * address, int port);
    SocketClient(void);
    ~SocketClient();
    bool OpenConnection(const char *address, int port);
    bool CloseConnection() { return SocketConnection::CloseConnection(); }
};

#endif /* _SOCKET_CLIENT */
