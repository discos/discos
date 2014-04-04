#ifndef _SPECTROMETER_H_
#define _SPECTROMETER_H_

#include "HardwareBlock.h"
#include "SingleSpectrometer.h"

class Spectrometer:public HardwareBlock
{
   SingleSpectrometer *SpectFFT[8];
 public:
   Spectrometer(int nchan, double freq);
   ~Spectrometer();
   int WriteFpgaD(int chip, int addr, int val);
   int WriteFpgaC(int chip, int val) {return 0;}
   int ReadFpga(int chip, int addr);
};
#endif // _SPECTROMETER_H_
