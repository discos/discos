#ifndef _SINGLE_SPECTROMETER_H_
#define _SINGLE_SPECTROMETER_H_

#include "HardwareBlock.h"
#include "Dbbc.h"
#include "FftSpectrometer.h"

class SingleSpectrometer :public HardwareBlock
{
   Dbbc bbc1;
   Dbbc bbc2;
   FftSpectrometer fft;
 public:
   SingleSpectrometer(int nchan, double freq);
   ~SingleSpectrometer();
   int WriteFpgaD(int chip, int addr, int val);
   int ReadFpga(int chip, int addr);
};

#endif // _SINGLE_SPECTROMETER_H_
