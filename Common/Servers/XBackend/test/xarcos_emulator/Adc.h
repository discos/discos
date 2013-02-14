#ifndef _ADC_H_
#define _ADC_H_

#include "HardwareBlock.h"
#include "SingleSpectrometer.h"

class Adc:public HardwareBlock
{
 public:
   Adc(double freq);
   ~Adc();
   int WriteFpgaD(int chip, int addr, int val);
   int ReadFpga(int chip, int addr);
};
#endif // _ADC_H_
