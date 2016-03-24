#include "SocketConnection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
//#include <sys/param.h>

// Socket libraries
#ifdef WINDOWS
  // Everything in winsock.h, already included
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

const int maxbuf=8192;

//
// Constructors. A socket connection may be built from an 
// already opened socket (usually for a server application)
//
SocketConnection::SocketConnection(int socket)
{
//
// Allocate space for the read buffer
//
    buffer = new char[maxbuf];
    nread = 0;
    error = 0;
//
// Initialize Windows socket
//
#ifdef WINDOWS
    WORD p=MAKEWORD(2,0);
    error=WSAStartup(p,&data);
    if (error == 0) { 
#endif
    if (socket) {
      sock = socket;
  InitConnection();
    }
#ifdef WINDOWS
  }
#endif
}

bool SocketConnection::InitConnection()
{
  if (sock == 0) return false;
#ifndef WINDOWS
  int flags = fcntl(sock, F_GETFL, 0);
  if ((fcntl(sock, F_SETFL, flags | O_NDELAY)) < 0) {
    error = errno;
    return false;
  }
//
// Set arguments for the select() call
//
  FD_ZERO(&fdset);
  FD_SET(sock, &fdset);
#endif
  wdfds = sock+1;
  return true;
}

bool SocketConnection::CloseConnection()
{
  if (sock) 
#ifdef WINDOWS
  closesocket (sock);
#else
  close(sock);
#endif
  sock = 0;
  return true;
}

SocketConnection::~SocketConnection()
{
  CloseConnection();
  delete[] buffer;
}

//
// Reads a line from the socket. 
// Terminates when a whole line has been received, or more than maxlen
// bytes have been read. Always null terminate the line, deleting the \n
//
// In Windows version, wait is always true
//
char *SocketConnection::GetLine(bool wait)
{
  int i;
  int n;
  bool end;
  if (sock == 0) return 0;

//
// Read from socket until a \0 comes in
//
  for (end = false; end == false; ) {
//
// Check if a \0 is present in the buffer
// If no, read a new chunk from input socket, and re-check data 
//
    for (n=0; n < nread; ++n) 
    if (buffer[n] == 0 || buffer[n] == '\n') break;
    if (n == nread) {	// not found. Read a new segment 
#ifndef WINDOWS
      if (wait) {          // Wait for data to become available
        //
        // Set arguments for the select() call
        //
        FD_ZERO(&fdset);
        FD_SET(sock, &fdset);
        wdfds = sock+1;
        select(wdfds, &fdset, 0, 0, 0);
      }
#endif
      n = recv(sock, buffer+nread, maxbuf-nread, 0);
      if (n <= 0) {
#ifdef WINDOWS
        error = errno;
#else
        if (errno != EWOULDBLOCK) error = errno;
        else error = 0;
#endif
        break;  // error or no data
      }
      nread += n;
      n = nread;
      if (nread == maxbuf) break;
    } else {	// end of line found.
      buffer[n]=0;// overrides \n with \0
      n++;	// length including \0
      end = true;
    }
  }
//
// Three cases possible.
//
  char * line=0;
  if (end) {  	// End of line found. Copy line and leave 
  		// everything left in buffer
    line = new char[n];
    strncpy(line, buffer, n);
    for (i=0; i<nread-n; ++i)
    buffer[i] = buffer[i+n];
    nread = nread - n;
  } else if  (n <= 0) {  // Error or nothing read. Return nil
    line = 0;
  } else if (n == maxbuf) {  // Buffer full. Return whole buffer
    line = new char[maxbuf+1];
    strncpy(line, buffer, n);
    line[maxbuf] = 0;
    nread = 0;
  }
  return line;
}

//
// Sends a line over the interface
// Performs multiple write until whole line has been sent
// Converts terminal \0 to \n
//
const char *SocketConnection::PutLine(const char * line, bool newline)
{
  size_t i;
  int n=0;
  if (line == 0) return 0;
  size_t len = strlen(line);
  for (i=0; i<len && n >= 0; i += n) {
    n = send(sock, line+i, len-i, 0);
    if (n < 0) {
      error = errno;
      return 0;
    }
  }
  if (newline) {
    if (send (sock, "\n", 1, 0) <= 0) {
      error = errno;
      return 0;
    }
  }
  return line;
}

int SocketConnection::GetBuffer(char * buf, int length, bool wait)
{
  int len1=length, i;
  if (length == 0) return 0;
  if (sock == 0) return 0;
//
// Copy already available data
//
  if (nread) for (i = 0; i< nread; ++i) {
    buf[i] = buffer[i];
    len1--;
    if (len1 == 0) {
      for (i=length; i< nread; ++i) buffer[i-length]=buffer[i];
      nread -= length;
      return length;
    }
  }
//
// Read from socket until empty or error
//
  bool end = false;
  while (end == false) {
#ifndef WINDOWS          // In windows, wait always true
    if (wait) {          // Wait for data to become available
  //
  // Set arguments for the select() call
  //
      FD_ZERO(&fdset);
      FD_SET(sock, &fdset);
      wdfds = sock+1;
      select(wdfds, &fdset, 0, 0, 0);
    }
#endif
    int n = recv(sock, buf+nread, length-nread, 0);
    if (n < 0) {
      if (errno != EWOULDBLOCK) error = errno;
      else error = 0;
      break;  // error or no data
    }
    nread += n;
    if (nread == length) break;
  }
  if (error == 0) return nread;
  else return -1;
} 

int SocketConnection::PutBuffer(const int * buffer, const int length)
{
    size_t i;
    const char * buf = (const char *)buffer;
    size_t n=0;
    for (i=0; i<(size_t)length && n >= 0; i += n) {
      n = send(sock, buf+i, length-i, 0);
      if (n < 0) {
        error = errno;
        return 0;
      }
    }
    return length;
}
  
int SocketConnection::Flush()
{
  nread = 0;
#ifdef WINDOWS
  return 0;
#else
  int n, ntot=nread;
  bool end;
  if (sock == 0) return 0;

//
// Read from socket until empty or error
//
  for (end = false; end == false; ) {
    n = recv(sock, buffer+nread, maxbuf-nread, 0);
    if (n < 0) {
      if (errno != EWOULDBLOCK) error = errno;
      else error = 0;
      break;  // error or no data
    }
    ntot += n;
  }
  if (error == 0) return ntot;
  else return -1;
#endif
} 
