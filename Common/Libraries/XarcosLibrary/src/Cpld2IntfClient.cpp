//
// Set of routines to interface with the ALMA Control Bus ehternet adapter.
//
// Uses a set of commands
// wl reg value		Writes <value> on CPLD2 register REG
// rl reg		Reads CPLD2 register REG
// wc chip value	Writes control register of chip <chip>
// wd chip reg value	Writes <value> on data register <reg> of chip <chip>
// rd chip reg 		Reads data register <reg> of chip <chip>
// rm chip reg length	Reads <length> values from reg <reg> of chip <chip>
//			<length> <= 255
//
// Uses class SocketConnection (SocketServer) to manage the socket.
//
#include "../include/Cpld2IntfClient.h"

using namespace std;

Cpld2IntfClient::Cpld2IntfClient(const char * a_server, int a_port)
	: conn(a_server,a_port)
{	
  server=new char[strlen(a_server)+1];
  strcpy(server, a_server);
  port=a_port;
}

Cpld2IntfClient::~Cpld2IntfClient()
{
  delete[] server;
}

int Cpld2IntfClient::Open()
{
  int err = conn.OpenConnection(server, port);
  if (err) return err;
  conn.PutLine("");
  char *retval;
  while ((retval=conn.GetLine(false)) != 0) delete[] retval;
  return 0;
}

static int hexdec(const char * &line)
{
  while  (*line == ' ') line++;		// skip blanks
  int val;
  char ch1 = *line++;
  if (ch1 >= '0' && ch1 <= '9') val =  (ch1-'0') << 4;
  else if (ch1 >= 'a' && ch1 <= 'f') val = (ch1 - 'a' + 10) << 4;
  else return -1;
  ch1 = *line++;
  if (ch1 >= '0' && ch1 <= '9') val +=  ch1-'0';
  else if (ch1 >= 'a' && ch1 <= 'f') val += ch1 - 'a' + 10;
  else return -1;
  return val;
}

int Cpld2IntfClient::ReadFpgaBlockHw(int brd, int chip, int reg,
		char val[], int num)
{
//  mux=mutex.trylock(10);**************************bisogna provare!!
//  if (mux){
	 mutex.lock();
	  int i;
	  if (num > 255) num = 255;
	  if (num < 0) return 0;
	  sprintf(buffer, "rm %x %02x %02x %03x", brd, chip & 0xff, reg & 0xff, num);
	  conn.PutLine(buffer);
	
	  const char *line = conn.GetLine();
	  if (line==0) return 0;
	  const char * line1 = strchr(line, '=');//********************************************
	  if (*line1++ == 0) return 0;
	  for (i=0; i<num; ++i) {
	    int byteval;
	    if ((byteval = hexdec(line1)) == -1) break;
	    val[i] = byteval;
	  }
	  delete[] line;
	  mutex.unlock();
  return i;
}

int Cpld2IntfClient::WriteFpgaD(int brd, int chip, int reg, char val)
{ 
 mutex.lock();
  sprintf(buffer, "wd %x %02x %02x %02x",
              brd, chip & 0xff, reg & 0xff, val & 0xff);
  if ((conn.PutLine(buffer)) == 0) return conn.LastError();
  mutex.unlock();
  return 0;
}

int Cpld2IntfClient::WriteFpgaC(int brd, int chip, char val)
{ 
  mutex.lock();
  sprintf(buffer, "wc %x %02x %02x",
              brd, chip & 0xff, val & 0xff);
  if ((conn.PutLine(buffer)) == 0) return conn.LastError();
  mutex.unlock();
  return 0;
}

int Cpld2IntfClient::WriteCpld(int brd, int reg, char val)
{
  mutex.lock();
  sprintf(buffer, "wl %x %02x %02x",
              brd, reg & 0xff, val & 0xff);
  if ((conn.PutLine(buffer)) == 0) return conn.LastError();
  mutex.unlock();
  return 0;
}

int Cpld2IntfClient::ReadCpld(int brd, int reg)
{ 
  mutex.lock();
  sprintf(buffer, "rl %x %02x", brd, reg & 0xff);
  conn.PutLine(buffer);
  mutex.unlock();
  return GetNumber();
}

int Cpld2IntfClient::ReadFpga(int brd, int chip, int reg)
{
  mutex.lock();
  sprintf(buffer, "rd %x %02x %02x", brd, chip & 0xff, reg & 0xff);
  conn.PutLine(buffer);
  mutex.unlock();
  return GetNumber();
}

int Cpld2IntfClient::GetNumber()
{
  mutex.lock();
  int val=0;
  const char *line;
  while ((line=conn.GetLine(true)) != 0) {
    const char * num = strchr(line, '=');
    if ( (num != 0) && (*num++ != 0)) {
      sscanf(num, "%x", &val);
      break;
    }
    if (line) delete[] line;
  }
  if (line) delete[] line;
  mutex.unlock();
  return val;
}
