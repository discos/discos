#ifndef _CORRADC
#define _CORRADC

/** 
 *  
 * Osservatorio Astrofisico di Arcetri
 *                                                                           
 * This code is under GNU General Public Licence (GPL).                                                          
 *                                                                                                              
 *  Who                              				  when            What                                       
 * Gianni Comoretto (comore@arcetri.astro.it)                       Creation                                                  
 */

#include "../include/HardwareBlock.h"
#include <complex>
using namespace std;


/** 
 * Class based on Altera correlator 
 *
 * Constructor requires an already opened AlmaInterface objectfor communication, 
 * and the clock frequency used in the chip (default is 125 MHz)
 * 
 */
class AdcConditioner : public HardwareBlock {
public:
  protected:
       int int_time;/*!< for TP meter */
       int hwFreq;	/*!< for line generator */
       int mode;	/*!< For control register */
   private:

   public:
	   /** 
	    * Constructor
	    * @param a_intf is an already opened CpldInterface
        * @param addr=0xb0c00
        * @param with b=board, 
        * @param c=chip
        * 
        * No chip offset used, it is implied in the methods
        * clockFreq is the base clock frequency. ADC clock is 2x
        */
       AdcConditioner(Cpld2Intf *a_intf, int addr, double clockFreq=125.e6);
       
       /**
        * Destructor.
        */
       ~AdcConditioner() {}
       
       /**
        * This method select test point (16 bit int)
        * 		bit 3-0 fpr tp0, .. 15-12 for tp3
        * @param tp is the value to set.
        * @return error code
        */
       int TestPoint(int tp); 		
      
       /**
        * This method read test point
        * @param tp is the value test point to check.
        * @return error code
        */					
	   bool CheckTP(int tp);
		
	   /**
	    * This method set contro word (for both ch)
	    * @param tp is the value to set.
	    * @return error code
	    */
	   virtual int Control(int val);	
	   
	   /**
	    * This method set distribution word 
	    * @param val is the value to set.
	    * @return error code
	    */
	   int Distribution(int val);	
	   
	   /**
	    * This method set test tone freq (in Hz) 
	    * @param a_freq is the value to set.
	    * @return Frequency
	    */ 
       double Freq(double a_freq);	
	   
       /**
	    * This method retrieve actual freq 
	    * @return Frequency
	    */
       double Freq() const { return hwFreq*clockFrequency/lo_resolution; }
	   
       /**
	    * This method set control & int.time registers 
	    * @return error code
	    */
       int Set();
	   
       /**
	    * This method set control register using meaningful values 
	    * @param source is 0=ADC, 1=test line, 2= RDG 3=Gauss noise
	    * @param bypasses real to complex stage
	    * @param dc_keep removes DC subtraction circuit
	    * @return mode
	    */
       int SetMode(int source, bool bypass=false, bool dc_keep=false);
      
       /**
	    * This method set integration time for TP meter 
	    * @param ms is the value to set.
	    * @return integration time
	    */
       int IntTime(int ms);		
  
       /**
	    * This method get integration time for TP meter 
	    * @return integration time 
	    */
       int IntTime() {return int_time * time_resolution;}

       /**
	    * This method TP control: reset keeps circuit locked
	    * @return error code
	    */
       int ResetTotPow(bool reset);
       
       /**
	    * This method read raw values
	    * @param xx
	    * @param yy
	    * @param xyre
	    * @param xyim
	    * @return error code
	    */
       int ReadTotPow(long &xx, long &yy, long &xyre, long &xyim);

       /**
	    * This method read values normalized to ADC input counts(squared)
	    * Total power read in normalized units
	    * Units are hardware counts as seen in Capture method, or as sent on 
	    * the backplane in 8 bit mode
	    * No correction is made for DC bias (readout is higher by 1/16 than true value)
	    * 
	    * @param xx
	    * @param yy
	    * @param xy
	    * @return error code is -1 for integration time of 0,
	    *  -2 for read error (read too slow)
	    */ 
       int ReadTotPow(double &xx, double &yy, complex<double> &xy);
       
       /**
	    * This method Capture 2048 complex or 4096 real samples
	    * Data are in hardware units, for 8 bit quantization
	    * without correction for DC bias
	    * If bypeass mode is selected, samples from ADC are captured
	    * In normal mode, real-imag pairs are stored in consecutive 
	    * buffer positions
	    * 
	    * @param buf 
		* @return error code
	    */
       int Capture(int buf[2][2048]);	

       /**
	    * This method set and read RD checker. Checks specified line on backplane, 
	    * or just read RD register if line < 0
	    * @param line
	    * @return error code
	    */ 
       int GetRDC(int line);
       static const double lo_resolution;/*!< Hz per LO unit */
       static const int time_resolution; /*!< in seconds */
   };

#endif 
