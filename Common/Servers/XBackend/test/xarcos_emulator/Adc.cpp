#include "Adc.h"

Adc::Adc(double freq) : 
  HardwareBlock(freq) {}

Adc::~Adc() {}

int Adc::WriteFpgaD(int chip, int addr, int val) {return 0;}

int Adc::ReadFpga(int chip, int addr) {return 0;}

