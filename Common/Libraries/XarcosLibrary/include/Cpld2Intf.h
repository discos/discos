#ifndef _CPLD2INTF
#define _CPLD2INTF

/** 
 *  
 * Osservatorio Astrofisico di Arcetri
 *                                                                           
 * This code is under GNU General Public Licence (GPL).                                                          
 *                                                                                                              
 *  Who                              				  when            What                                       
 * Gianni Comoretto (comore@arcetri.astro.it)                       Creation                                                  
 */

#include <stdio.h>
#include <string.h> 
using namespace std;
/**
* Class Cpld2Intf is a inteface object
* It relies on a SocketClient object for communication.
* 
* Programs and reads registers in a standard ALMA interface bus
* Routines write CPLD2 register, FPGA control register 
* FPGA data register
* 
* BroadcastBlock() programs an arbitrary number of FPGAs
* (selected by mask bits in <cmask>), on register selected by <rmask>, 
* with <num> values
*
* It is possible to read a register from CPLD2 chip or from a FPGA register
* or an arbitrary number of values from a FPGA reg. 
* 
* 
* Set of routines to interface with the ALMA Control Bus
* Routines implemented in the abstract version
* 
* Uses a set of commands 
* wl reg value		Writes <value> on CPLD2 register REG
* rl reg		Reads CPLD2 register REG
* wc chip value	Writes control register of chip <chip>
* wd chip reg value	Writes <value> on data register <reg> of chip <chip>
* rd chip reg 		Reads data register <reg> of chip <chip>
* rm chip reg length	Reads <length> values from reg <reg> of chip <chip>
*			<length> <= 255
*
* Uses class SocketConnection to manage the socket. 
*/

class Cpld2Intf {
 protected:
  virtual int ReadFpgaBlockHw(int brd, int chip, int reg, 
		  char val[], int num=1)=0;
 public:
  virtual ~Cpld2Intf() {}
  virtual int BroadcastBlock(int brd, int cmask, int rmask, 
		  char val[], int num=1);
  virtual int WriteFpgaD(int brd, int chip, int reg, char val);
  virtual int WriteFpgaC(int brd, int chip, char val);
  virtual int WriteCpld(int brd, int reg, char val)=0;
  virtual int ReadCpld(int brd, int reg)=0;
  virtual int ReadFpga(int brd, int chip, int reg);
  virtual int ReadFpgaBlock(int brd, int chip, int reg, 
		  char val[], int num=1);
};

#endif
