#include <stdio.h>
#include "SocketServer.h"
#include "ArcosEmulator.h"
#include "string.h"
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char ** argv)
{
    char host[] = "127.0.0.1";
    int port = 1234;
    int n=0;
    const int buf_len = 32768;
    char *buffer= new char[buf_len];
    SocketServer serv(host, port);

    ArcosEmulator interface;

    while(1) {
      char *buf;
      bool stop=false;
      serv.WaitConnection();		// Wait incoming connection
      while (1) {
        if (n > 0) serv.PutLine("> ", false);  // Print a prompt
        buf = serv.GetLine();
        if (buf == 0) {
          serv.CloseConnection(); // close the connection
          cout << "Closed incoming connection\n";
          break;
        }
        int last = strlen(buf)-1;
        if (last >= 0 && buf[last] == '\r') buf[last]=0;
        strcpy(buffer, buf);
        delete[] buf;
        stop = false;     
        n =  interface.process_buffer(buffer, buf_len);
        if (n < 0) {
          serv.PutLine(buffer, false);
          serv.CloseConnection();
          stop = true;
          cout << "Closed incoming connection\n";
          break;
        } else if (n == 1) {
          serv.PutLine(buffer, false);
        } else if ( n > 1) {
	  serv.PutBuffer((int *)buffer, n);
        } else {
        }
      }
    }
    return 0;
}
