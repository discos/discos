#include "SocketServer.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
//#include <sys/param.h>
// Socket include files
#ifdef WINDOWS
// for Windows everything is in winsock.h
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

SocketServer::SocketServer(const char *address, int port)
{
    if (OpenConnection(address, port) == false) {
		perror("Opening socket connection");
	 	exit (-1); 
    }
}	

bool SocketServer::OpenConnection(const char * addr, int port)
{ 
	SOCKET s;
	if ((s=open_socket(addr, port)) <0) {
		perror("Opening Server socket connection");
	 	exit (-1); 
	}
	sock_serv = s;
	return true;
}

bool SocketServer::WaitConnection()
{
	SOCKET ns;
	if ((ns=get_connect())<0) {
		perror("Opening Client socket connection");
		sock = 0;
	 	exit (-1); 
	}
	sock = ns;
	return InitConnection();
}

SOCKET SocketServer::get_connect()
{
	struct sockaddr_in clientadr;
#ifdef WINDOWS
    int fromlen;
#else
	socklen_t fromlen;
#endif
	SOCKET ns;
	
	fromlen = sizeof(clientadr);
	if ((ns=accept(sock_serv,(struct sockaddr*)&clientadr, &fromlen))<0) {
		perror("server accept error");
		exit(1);
	}
	printf("Received connection on fd %d\n", ns);
	return ns;
}

SOCKET SocketServer::open_socket(const char * addr, int port)
{	
	
	SOCKET s;
	struct hostent *hp;

	if((hp=gethostbyname(addr))==NULL) {
#ifdef WINDOWS
		error = errno;
#else
		error = EHOSTUNREACH;
#endif
		return false;
	}
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	//if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
	    error = errno;
	    return false;
	}
	struct sockaddr_in address;
	//address.sin_family = PF_INET;
	address.sin_family = AF_INET;
	address.sin_port=htons(port);
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(s, (struct sockaddr*)&address, sizeof(address)) < 0 ) {
		perror("Server bind");
		exit(1);
	}
	if (listen(s, 1) <0) {
		perror ("server: listen");
		exit(1);
	}
	return s;
}

SocketServer::~SocketServer()
{
	CloseConnection();
}
