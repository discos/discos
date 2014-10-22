#ifndef _HWBLOCK
#define _HWBLOCK

/** 
 *  
 * Osservatorio Astrofisico di Arcetri
 *                                                                           
 * This code is under GNU General Public Licence (GPL).                                                          
 *                                                                                                              
 *  Who                              				  when            What                                       
 * Gianni Comoretto (comore@arcetri.astro.it)                       Creation                                                  
 */

#include "Cpld2Intf.h"
#define X64
typedef int int32;

using namespace std;

/** 
 *
 * This class is a Prototype for hardware block
 * Provides a minimum interface for a piece of hardware.
 * The hw is identified by a Cpld2 interface object (used for actual
 * communication), and an address composed of bard, chip, and address
 * mask (or starting address in chip space)
 * 
 * First address (offset 0) is always the Test Point Register, that can
 * always be read back for simple testing.
 * Second address (offset 1) is always the Control Register, with its
 * high bit driving the PLL Reset flag.
 */

class HardwareBlock {
protected:
   int board;/*!< */
   int chip;/*!< */
   int mask;/*!< */
   int ctrl;/*!< */
   Cpld2Intf *intf;/*!< */
   double clockFrequency;/*!< */
public:
   /** 
    * Constructor
    * @param a_intf is an already opened CpldInterface
     * @param addr
     * @param clock_freq=125.e6, 
     */
	HardwareBlock(Cpld2Intf *a_intf, int addr, double clock_freq=125.e6);
     /**
      * Destructor.
      */
    virtual ~HardwareBlock() {}
   virtual int Init();
   virtual int Set() {return 0;}
   int WriteFpgaC(int val) {
	   return intf->WriteFpgaC(board, chip, val); }
   int WriteFpgaD(int addr, int val) {
	   return intf->WriteFpgaD(board, chip, mask|addr, val); }
   int ReadFpga(int addr) {
	   return intf->ReadFpga(board, chip, mask | addr); }
   int ReadFpgaBlock(int addr, char buf[], int num) {
	   return intf->ReadFpgaBlock(board, chip, mask|addr, buf, num); }

   virtual int TestPoint(int tp);
   virtual bool CheckTP(int tp);
   virtual int Control(int val);
   virtual int Status();
};
#endif // _HWBLOCK

