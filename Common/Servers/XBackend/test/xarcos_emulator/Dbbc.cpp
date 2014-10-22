#include "Dbbc.h"
#include <stdio.h>

using namespace std;

int Dbbc::WriteFpgaD(int chip, int addr, int val)
{
  val &= 0xff;
  // chip is unused
  switch (addr) {
  case 0:	// Test point register
    tpreg = val;
    break;
  case 1:	// Control. Performs also all the necessary operations
    control  = val;
    bypass   = ((control & 0x40 ) != 0x40); // if bit 6=0, bypass mode
    band     = control & 0x7;
    enable   = ((control & 0x08 ) != 0x08);
    if (control & 0x10) tp_ready = 0;
    if (control & 0x10) rdg_ready = 0;
    enable_timer();
    break;
  case 2:	// gain 
    gain = val;
    break;
  case 3:	// load frequency 
    freq_buf = ((freq_buf & 0xffff) << 8 ) | val;
    break;
  case 4:	// Integration length
    freq_reg = freq_buf;
    break;
  case 5: 	// phase register
    phase_reg = val;
    break;
  case 6:	// rdg control
    rdg_control = val;
    break;
  case 7:       // Input and total power control
    input_sel = val & 0x7;
    mode_8bit = ((val & 0x8) == 0x8);
    tp_sel = (val & 0x30) >> 4;
    break;
  default:
    printf("DBBC invalid register %d\n", addr);
    return -1;
  }
  return 0;
}
  
int Dbbc::ReadFpga(int chip, int addr)
{
  int ret_value=0;
  switch (addr) {
  case 0:
    ret_value = tpreg;
    break;
  case 1:
    ret_value = control & 0xcf;
    if (tp_ready) ret_value |= 0x10;
    if (rdg_ready) ret_value |= 0x20;
    break;
  case 2:
    ret_value = rdg_value & 0xff;
    break;
  case 3:
    ret_value = (rdg_value >> 8 ) &0xff;
    break;
  case 4:
    ret_value = tp_value & 0xff;
    break;
  case 5:
    ret_value = (tp_value >> 8 ) &0xff;
    break;
  case 6:
    ret_value = (tp_value >> 16 ) &0xff;
    break;
  case 7:
    ret_value = (tp_value >> 24 ) &0xff;
    break;
  default:
    printf("DBBC invalid register %d\n", addr);
  }
  return ret_value;
}

//
// Constructor simply initializes all elements
//
Dbbc::Dbbc(double freq)
 : HardwareBlock(freq), int_thread(this),  tpreg(0), control(0),
 enable(0), tp_ready(0), rdg_ready(0), tp_value(0), rdg_value(1), 
 bypass(1), band(0), gain(10), freq_buf(0), freq_reg(0), phase_reg(0),
 rdg_control(0), input_sel(0), mode_8bit(0), tp_sel(0) 
{
  // After everything has been initialized, start total power thread
  int_thread.create_thread();
}

void Dbbc::enable_timer()
{
  if (enable) int_thread.start();
  else        int_thread.stop();
}

void Dbbc::ComputeTotalpower()
{
  tp_value = 0x123456;
  rdg_value = 0;
}

double Dbbc::Band()
{
  double div;
  if (bypass) div = 1.0;
  else div = (double) (2 << (7-band));
  return Freq()/div;
}

double Dbbc::LoFreq()
{
  int freq_int = freq_reg;
  if (freq_int > 0x7fffff) freq_int -= 0x1000000;
  return Freq()*freq_int/1.6777216e7;
}

Dbbc::~Dbbc()
{
}
