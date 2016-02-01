#ifndef _BBC_FILTER
#define _BBC_FILTER

/** 
 *  
 * Osservatorio Astrofisico di Arcetri
 *                                                                           
 * This code is under GNU General Public Licence (GPL).                                                          
 *                                                                                                              
 *  Who                              				  when            What                                       
 * Gianni Comoretto (comore@arcetri.astro.it)                       Creation                                                  
 */

#include "HardwareBlock.h"

/** 
 *Class based on Altera correlator
 *
 * Variable decimation filter, input real, output complex
 * Band is selectable using the enumerated type BbcFilter::BandCode
 * or as a double value (in Hz)
 * Central frequency is selected as a double (in Hz)
 * Gain is in hardware units (0 = mute, 255 = max, typ. value around 60)
 * Methods return the current band and value if called without arguments
 * All values are stored and become effective only after a call to Set()
 *
 * TPRead() returns total power counts, with integration time of 200 ms
 * as an integer (24 bit)
 *
 * Constructor asks for an already opened Cpld2Intf object,
 * and a 2 byte address (int*16). MS byte represents chip address, and
 * LS byte an address mask (to select a chip in a group, and a
 * device in a chip). The address mask can be considered as
 * the register space starting address.
 */
class BbcFilter : public HardwareBlock {
public:
protected:
   int lofreq;/*!< */
   int lophase;/*!< */
   int gain;/*!< */
   int band;/*!< */
   int inputChan;/*!< */
   int inputBit;/*!< */
   bool mode8bit;/*!< */
   double bandtable[9];/*!< */
   double lo_resolution;/*!< */
private:
   int SetGain();
   int SetChan();
   int SetBand();
public:
   int Start();
   int SetFreq();			// public to be able to broadcast it
   BbcFilter(Cpld2Intf *a_intf, int addr, double clockFreq=125.e6);
   ~BbcFilter() {}
   double Freq(double a_freq, double a_phase=0.0);
   double Freq() const {		// Retrieve actual freq
   return lofreq*lo_resolution; }
   int Gain(int a_gain);
   int Gain() const {		// Retrieve actual gain
   return gain;}
   double Band(int a_band);
   double Band (double a_band);
   double Band() const { return bandtable[band];}
   int InputChannel() {return inputChan;}
   					// Set input channel,
				// word length, bit to check
   int InputChannel(int chan, bool len8=true, int bit=7);
   double HwBand(double a_band) const;
   int Set();
//       int Init();		Uses default initializator
   int TPRead(bool wait=true);
   int TPReadN(int val[], int num);
   int GetRDC(int line);
};

#endif
