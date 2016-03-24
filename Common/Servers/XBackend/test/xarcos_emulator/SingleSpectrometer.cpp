#include "SingleSpectrometer.h"
#include <stdio.h>

using namespace std;

SingleSpectrometer::SingleSpectrometer(int nchan, double freq)
 : HardwareBlock(freq), bbc1(freq), bbc2(freq), 
   fft(nchan, &bbc1, &bbc2, freq)
{ 
}
SingleSpectrometer::~SingleSpectrometer()
{ 
}

int SingleSpectrometer::WriteFpgaD(int chip, int addr, int val)
{ 
  int retval;
  switch (chip) {
  case 0:
    retval=bbc1.WriteFpgaD(chip,addr,val);
    break;
  case 1:
    retval=bbc2.WriteFpgaD(chip,addr,val);
    break;
  case 2:
    retval=fft.WriteFpgaD(chip,addr,val);
    break;
  default: 
    printf("Error: invalid chip %d\n", chip);
    retval = -1;
  }
  return retval;
}

int SingleSpectrometer::ReadFpga(int chip, int addr)
{ 
  int retval;
  switch (chip) {
  case 0:
    retval=bbc1.ReadFpga(chip,addr);
    break;
  case 1:
    retval=bbc2.ReadFpga(chip,addr);
    break;
  case 2:
    retval=fft.ReadFpga(chip,addr);
    break;
  default:
    printf("Error: invalid chip %d\n", chip);
    retval = -1;
  }
  return retval;
}
