//
// Set of routines to interface with the ALMA Control Bus
// Routines implemented in the abstract version
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
#include "../include/Cpld2Intf.h"
using namespace std;

int Cpld2Intf::BroadcastBlock(int brd, int cmask, int rmask, 
		char val[], int num)
{
  int err=0;
  int i;
  err=WriteCpld(brd, 1, cmask & 0xff);if(err!=0) return err;
  err=WriteCpld(brd, 2, (cmask & 0xff00) >> 8);if(err!=0) return err;
  err=WriteCpld(brd, 5, rmask & 0xff);if(err!=0) return err;
  for (i=0; i<num; ++i) {
	err = WriteCpld(brd, 7,val[i]);
	if (err < 0) break;
  }
  return i;
}

int Cpld2Intf::ReadFpgaBlock(int brd, int chip, int reg, 
		char val[], int num)
{
  char * val1 = val;
  int numleft = num;
  while (numleft > 0) {
    int n = ReadFpgaBlockHw(brd, chip, reg, val1, numleft);
    if (n < 0) return numleft;
    if (n == 0) break;
    numleft -= n;
    val1 += n;
  }
  return num - numleft;
}

int Cpld2Intf::WriteFpgaD(int brd, int chip, int reg, char val)
{
    int err=0;
    err = WriteCpld(brd, 1, chip);if(err!=0) return err;  
    err = WriteCpld(brd, 4, reg);if(err!=0) return err;  
    err = WriteCpld(brd, 6, val);  
    return err;
}

int Cpld2Intf::WriteFpgaC(int brd, int chip, char val)
{
    int err=0;
    err = WriteCpld(brd, 1, chip);if(err!=0) return err;
    err = WriteCpld(brd, 4, val);
    return err;
}

int Cpld2Intf::ReadFpga(int brd, int chip, int reg)
{
    int err=0;
    err = WriteCpld(brd, 1, chip);if(err!=0) return err; 
    err = WriteCpld(brd, 4, reg);if(err!=0) return err;  
    err = ReadCpld(brd, 11); 
    return err;
}
