#ifndef _ALMAINTERFACE
#define _ALMAINTERFACE
/** 
 *  
 * Osservatorio Astrofisico di Arcetri
 *                                                                           
 * This code is under GNU General Public Licence (GPL).                                                          
 *                                                                                                              
 *  Who                              				  when            What                                       
 * Gianni Comoretto (comore@arcetri.astro.it)                       Creation                                                  
 * Liliana Branciforti(bliliana@arcetri.astro.it)  10/06/2009       Change
 */
#include "SocketClient.h"
#include "Cpld2Intf.h"
#include <stdio.h>
#include <string.h> 
#include "XaMutex.h"

using namespace std;

/**
* Class Cpld2IntfClient
* It relies on a SocketClient object for communication
* base on Cpld2Intf inteface.
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
** 
* Set of routines to interface with the ALMA Control Bus
* Routines implemented in the abstract version
* 
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

class Cpld2IntfClient : public Cpld2Intf {
 protected:  
  SocketClient conn;/*!< */
  char buffer[32];/*!< */
  char * server;/*!< */
  int port;/*!< */
  
  int ReadFpgaBlockHw(int brd, int chip, int reg, char val[], int num=1);
  int GetNumber(); 
 public: 
  Mutex mutex;/*!< */
  
  /**
   * Constructor
   * @param server is a string that contain IP address
   * @param port is a number of port of IP connection
   */
  Cpld2IntfClient(const char * server, int port);
  
  /**
   * Destructor.
   */
  virtual ~Cpld2IntfClient();
  
  int Open();
  int Close() { return conn.CloseConnection(); }
  bool IsOpen() { return conn.Connected(); }
  int WriteCpld(int brd, int reg, char val);
  int ReadCpld(int brd, int reg);
  int WriteFpgaD(int brd, int chip, int reg, char val);
  int WriteFpgaC(int brd, int chip, char val);
  int ReadFpga(int brd, int chip, int reg);
};

#endif
