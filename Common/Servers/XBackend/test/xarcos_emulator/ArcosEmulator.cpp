#include "ArcosEmulator.h"

ArcosEmulator::ArcosEmulator() : 
   board1(2048, 125e6), board2(2048, 125e6), adc1(125e6), adc2(125e6) {}

ArcosEmulator::~ArcosEmulator()  {}

void ArcosEmulator::writeD(int card, int chip, int adr, int val)
{
  switch (card) {
  case 0:
    board1.WriteFpgaD(chip, adr, val);
    break;
  case 1:
    adc1.WriteFpgaD(chip, adr, val);
    break;
  case 2: 
    board2.WriteFpgaD(chip, adr, val);
    break;
  case 3: 
    adc2.WriteFpgaD(chip, adr, val);
    break;
  default:
    break;
  }
}

int  ArcosEmulator::read(int card, int chip, int adr)
{
  int ret = -1;
  switch (card) {
  case 0:
    ret = board1.ReadFpga(chip, adr);
    break;
  case 1:
    ret = adc1.ReadFpga(chip, adr);
    break;
  case 2: 
    ret = board2.ReadFpga(chip, adr);
    break;
  case 3: 
    ret = adc2.ReadFpga(chip, adr);
    break;
  default:
    break;
  }
  return ret;
}

void ArcosEmulator::write_block(int board, int chip, int addr, 
     char *val, int num)
{
  int i;
  for (i=0; i<num; ++i) writeD(board, chip, addr, (int) val[i]);
}

void ArcosEmulator::read_block(int board, int chip, int addr, 
     char *val, int num)
{
  int i;
  for (i=0; i<num; ++i) val[i]=(char)(read(board, chip, addr) & 0xff);
}
