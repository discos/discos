//
// Set of routines to interface with the ALMA Control Bus ehternet adapter. 
// 
// Uses a set of commands 
// 1 board chip reg value	Writes <value> on data register <reg> of chip <chip>
// 2 board chip reg 		Reads data register <reg> of chip <chip>
// rm board chip reg length	Reads <length> values from reg <reg> of chip <chip>
//			<length> <= 255
//
// Uses class SocketConnection (SocketServer) to manage the socket. 
//
#include "Cpld2IntfClient.h"
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

/*static int hexdec(const char * &line)
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
}*/

int Cpld2IntfClient::ReadFpgaBlockHw(int brd, int chip, int reg, 
		int val[], int num)
{
  int i = 0;
  if (num > 16384) num = 16384;
  if (num < 0) return 0;
  conn.Flush();
  sprintf(buffer, "3 %x %02x %02x %04x", brd, chip & 0xff, reg & 0xff, num);
  conn.PutLine(buffer);
  conn.GetBuffer((char *)val, num*4);

  return i;
}

int Cpld2IntfClient::WriteFpgaD(int brd, int chip, int reg, int val) 
{
  sprintf(buffer, "1 %x %02x %02x %04x",
              brd, chip & 0xff, reg & 0xff, val & 0xffff);
  if ((conn.PutLine(buffer)) == 0) return conn.LastError(); 
  //int n = GetNumber();
  return 0;
}

int Cpld2IntfClient::WriteFpgaC(int brd, int chip, int val) 
{
  sprintf(buffer, "1 %x %02x 00 %02x",
              brd, chip & 0xff, val & 0xff);
  if ((conn.PutLine(buffer)) == 0) return conn.LastError(); 
  //int n = GetNumber();
  return 0;
}

int Cpld2IntfClient::WriteCpld(int brd, int reg, int val) 
{
  return 0;
}

int Cpld2IntfClient::ReadCpld(int brd, int reg) 
{
  return 0;
}

int Cpld2IntfClient::WReadFpga(int brd, int chip, int reg, int val) 
{
  sprintf(buffer, "1 %x %02x %02x %04x",
              brd, chip & 0xff, reg & 0xff, val & 0xffff);
  conn.PutLine(buffer);
  return GetNumber();
}

int Cpld2IntfClient::ReadFpga(int brd, int chip, int reg) 
{
  sprintf(buffer, "2 %x %02x %02x", brd, chip & 0xff, reg & 0xff);
  conn.PutLine(buffer);
  return GetNumber();
}

int Cpld2IntfClient::GetNumber()
{
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
  return val;
}
