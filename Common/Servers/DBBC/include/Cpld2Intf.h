//
// Class based on Altera correlator 
// Cpld2Intf
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
#ifndef _CPLD2INTF
#define _CPLD2INTF

#include <stdio.h>
#include <string.h> 
using namespace std;

class Cpld2Intf {
 protected:
  virtual int ReadFpgaBlockHw(int brd, int chip, int reg, 
		  int val[], int num=1)=0;
 public:
  virtual ~Cpld2Intf() {}
  virtual int BroadcastBlock(int brd, int cmask, int rmask, 
		  int val[], int num=1);
  virtual int WriteFpgaD(int brd, int chip, int reg, int val);
  virtual int WReadFpga(int brd, int chip, int reg, int val);
  virtual int WriteFpgaC(int brd, int chip, int val);
  virtual int WriteCpld(int brd, int reg, int val)=0;
  virtual int ReadCpld(int brd, int reg)=0;
  virtual int ReadFpga(int brd, int chip, int reg);
  virtual int ReadFpgaBlock(int brd, int chip, int reg, 
		  int val[], int num=1);
};

#endif
