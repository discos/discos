//
// Class based on Altera correlator 
// Cpld2IntfClient
//
// Programs and reads registers in a standard ALMA interface bus
// Routines write CPLD2 register, FPGA control register 
// FPGA data register
// 
// BroadcastBlock() programs an arbitrary number of FPGAs
// (selected by mask bits in <cmask>), on register selected by <rmask>, 
// with <num> values
//
// It is possible to read a register from CPLD2 chip or from a FPGA register
// or an arbitrary number of values from a FPGA reg.  
//
// It relies on a SocketClient object for communication
//
#ifndef _ALMAINTERFACE
#define _ALMAINTERFACE

#include "SocketClient.h"
#include "Cpld2Intf.h"
#include <stdio.h>
#include <string.h> 

using namespace std;

class Cpld2IntfClient : public Cpld2Intf {
 protected:
  //SocketClient conn;
  char buffer[32];
  char * server;
  int port;
  int ReadFpgaBlockHw(int brd, int chip, int reg, int val[], int num=1);
  //int GetNumber(); 
 public:
  
  Cpld2IntfClient(const char * server, int port);
  virtual ~Cpld2IntfClient();
  SocketClient conn;
  int GetNumber(); 
  int Open();
  int Close() { return conn.CloseConnection(); }
  bool IsOpen() { return conn.Connected(); }
  int WriteCpld(int brd, int reg, int val);
  int ReadCpld(int brd, int reg);
  int WriteFpgaD(int brd, int chip, int reg, int val);
  int WriteFpgaC(int brd, int chip, int val);
  int ReadFpga(int brd, int chip, int reg);
  int WReadFpga(int brd, int chip, int reg, int val);
};

#endif
