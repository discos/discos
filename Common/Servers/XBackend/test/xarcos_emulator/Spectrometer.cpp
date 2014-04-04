#include "Spectrometer.h"
#include <stdio.h>

using namespace std;


int Spectrometer::WriteFpgaD(int chip, int addr, int val)
{
  int ret = 0;
  int index = (chip & 4) + 2*(chip&1);
  switch(chip) {
  case 2:
  case 3:
  case 6:
  case 7:
    if (addr & 0x10) ret=SpectFFT[index  ]->WriteFpgaD(0, addr & 0xf, val); 
    if (addr & 0x20) ret=SpectFFT[index  ]->WriteFpgaD(1, addr & 0xf, val); 
    if (addr & 0x40) ret=SpectFFT[index+1]->WriteFpgaD(0, addr & 0xf, val); 
    if (addr & 0x80) ret=SpectFFT[index+1]->WriteFpgaD(1, addr & 0xf, val); 
    break;
  case 0:
  case 1:
  case 4:
  case 5:
    if (addr & 0x10) ret=SpectFFT[index  ]->WriteFpgaD(2, addr & 0xf, val); 
    if (addr & 0x20) ret=SpectFFT[index+1]->WriteFpgaD(2, addr & 0xf, val); 
    break;
  default:
    printf("Error: invalid chip %d\n", chip);
    ret = -1;
  }
  return ret;
}

int Spectrometer::ReadFpga(int chip, int addr)
{
  int ret = 0;
  int index = (chip & 4) + 2*(chip&1);
  switch(chip) {
  case 2:
  case 3:
  case 6:
  case 7:
    if (addr & 0x10) ret=SpectFFT[index  ]->ReadFpga(0, addr & 0xf); 
    if (addr & 0x20) ret=SpectFFT[index  ]->ReadFpga(1, addr & 0xf); 
    if (addr & 0x40) ret=SpectFFT[index+1]->ReadFpga(0, addr & 0xf); 
    if (addr & 0x80) ret=SpectFFT[index+1]->ReadFpga(1, addr & 0xf); 
    break;
  case 0:
  case 1:
  case 4:
  case 5:
    if (addr & 0x10) ret=SpectFFT[index  ]->ReadFpga(2, addr & 0xf); 
    if (addr & 0x20) ret=SpectFFT[index+1]->ReadFpga(2, addr & 0xf); 
    break;
  default:
    printf("Error: invalid chip %d\n", chip);
    ret = -1;
  }
  return ret;
}

Spectrometer::Spectrometer(int nchan, double freq)
  : HardwareBlock(freq)
{
  int i;
  for (i=0; i<8; ++i) 
    SpectFFT[i] = new SingleSpectrometer(nchan, freq);
}

Spectrometer::~Spectrometer()
{
  int i;
  for (i=0; i<8; ++i) delete SpectFFT[i];
}
