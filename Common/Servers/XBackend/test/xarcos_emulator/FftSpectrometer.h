#ifndef _FFT_SPECTROMETER_
#define _FFT_SPECTROMETER_

#include "HardwareBlock.h"
#include "Dbbc.h"
class IntThread;
#include "IntThread.h"

class FftSpectrometer : public HardwareBlock
{
   Dbbc * dbbc1;
   Dbbc * dbbc2;
   int n_channels;
   int tpreg;
   int control;
   int enable;
   int resolution;
   int end_integ;
   int overflow;
   int int_length;
   int read_addr;
   int mem_value;
   IntThread int_thread;
 public:
   FftSpectrometer(int nchan, Dbbc* bbc1, Dbbc* dbbc2, double freq);
   ~FftSpectrometer();
   int WriteFpgaD(int chip, int addr, int val);
   int ReadFpga(int chip, int addr);
   double IntTime();
   void SetEndIntegration() {end_integ = true;}
 private:
   void enable_timer();
   int read_memory();
   int compute_spectrum(int i, int bank); 
};

#endif  // _FFT_SPECTROMETER_
