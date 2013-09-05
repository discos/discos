#ifndef _ARCOS_EMULATOR_
#define _ARCOS_EMULATOR_

#include "CpldInterface.h"
#include "Spectrometer.h"
#include "Adc.h"

class ArcosEmulator: public CpldInterface 
{
   Spectrometer board1;
   Spectrometer board2;
   Adc adc1;
   Adc adc2;
  public:
   ArcosEmulator();
   ~ArcosEmulator();
   void writeD(int card, int chip, int adr, int val);
   int  read(int card, int chip, int adr);
   void write_block(int card, int chip, int adr, char *val, int num);
   void read_block(int board, int chip, int adr, char *val, int num);
};


#endif // _ARCOS_EMULATOR_

