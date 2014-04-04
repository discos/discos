#include "../include/SocketClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#include <sys/param.h>

 SocketClient::SocketClient(const char *address, int port)
{
    if (OpenConnection(address, port) == false) {
	perror("Opening socket connection");
 	exit (-1); 
    }
}

bool SocketClient::OpenConnection(const char * addr, int port)
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
    char * caddr = (char *) (&address.sin_addr);
    for (int i=0; i < hp->h_length; ++i) 
	caddr[i] = hp->h_addr[i];
    if (connect(s, (struct sockaddr*)(&address), sizeof(address)) < 0 ) {
        error = errno;
        close (s);
        return false;
    }
    return InitConnection(s);
}

SocketClient::~SocketClient()
{
}
