#include "../include/SocketServer.h"
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#include <sys/param.h>

SocketServer::SocketServer(const char *address, int port)
{
    if (OpenConnection(address, port) == false) {
		perror("Opening socket connection");
	 	exit (-1); 
    }
}	

bool SocketServer::OpenConnection(const char * addr, int port)
{ 
	int s,ns;
	
	if ((s=open_socket(addr, port)) <0) {
		perror("Opening Server socket connection");
	 	exit (-1); 
	}
	if ((ns=get_connect(s))<0) {
		perror("Opening Client socket connection");
	 	exit (-1); 
	}
	return InitConnection(ns);
}

int SocketServer::get_connect(int s)
{
	struct sockaddr_in clientadr;
	socklen_t fromlen;
	int ns;
	
	fromlen = sizeof(clientadr);
	if ((ns = accept(s, (struct sockaddr*)&clientadr, &fromlen)) < 0) {
		perror("server accept error");
		exit(1);
	}
	printf("Received connection on fd %d\n", ns);
	return ns;
}

int SocketServer::open_socket(const char * addr, int port)
{	
	
	int s;
	struct hostent *hp;

	if((hp=gethostbyname(addr))==NULL) {
		error = EHOSTUNREACH;
		return false;
	}
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	    error = errno;
	    return false;
	}
	struct sockaddr_in address;
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
}
