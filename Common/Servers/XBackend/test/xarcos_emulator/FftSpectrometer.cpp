#include "FftSpectrometer.h"
#include <stdio.h>
#include <math.h>

using namespace std;

int FftSpectrometer::WriteFpgaD(int chip, int addr, int val)
{
  int resol_code;
  val &= 0xff;
  // chip is unused
  switch (addr) {
  case 0:	// Test point register
    tpreg = val;
    break;
  case 1:	// Control. Performs also all the necessary operations
    control = val;
    enable = (control & 0x1) == 0;
    resol_code = (control >> 4) & 7;
    resolution = 1 << resol_code;
    if (control & 0x4) end_integ = 0;
    if (control & 0x8) overflow = 0;
    enable_timer();
    break;
  case 2:	// Read address
    read_addr = ((read_addr&0xff) << 8) | (val << 2);
    break;
  case 3:	// Increment address
    read_addr = (read_addr +4) & 0xfffc;
    break;
  case 4:	// Integration length
    int_length = ((int_length << 8)&0xffff00) | val;
    break;
  default:
    printf("FFT invalid register %d\n", addr);
    return -1;
  }
  return 0;
}
  
int FftSpectrometer::ReadFpga(int chip, int addr)
{
  int ret_value=0;
  switch (addr) {
  case 0:
    ret_value = tpreg;
    break;
  case 1:
    ret_value = control & 0xf3;
    if (end_integ) ret_value |= 0x4;
    if (overflow) ret_value |= 0x8;
    break;
  case 2:
    ret_value = read_addr >> 2;
    break;
  case 3:
    ret_value = read_memory();
    break;
  case 4:
    ret_value = int_length;
    break;
  default:
    printf("FFT invalid register %d\n", addr);
  }
  return ret_value;
}

//
// Constructor simply initializes all elements
//
FftSpectrometer::FftSpectrometer(
 int nchan, Dbbc* a_bbc1, Dbbc* a_bbc2, double freq)
 : HardwareBlock(freq), dbbc1(a_bbc1), dbbc2(a_bbc2), 
 n_channels(nchan), control(0),
 enable(0), resolution(1), end_integ(0), overflow(0), int_length(1), 
 read_addr(0), mem_value(0), int_thread(this) 
{
  // After everything has been initialized, start total power thread
  int_thread.create_thread();
}

//
// Compute spectrum as a function of normalized frequency. 
// 
int FftSpectrometer::compute_spectrum(int index, int bank) 
{
  static double coef[] = {1,0.1,0.9,0,3.8,0,-90};
  static double fline = 0.15, wline=0.0000003;
  if (index >= n_channels/2) index -= n_channels;
  double freq=(double)index/(double)n_channels, ampl=coef[0];
  Dbbc *dbbcx, *dbbcy;
  switch (bank & 3) {
  case 0: // XX
    dbbcx=dbbcy=dbbc1;
    break;
  case 1:
    dbbcx=dbbcy=dbbc2;
    break;
  default:
    dbbcx=dbbc1; dbbcy=dbbc2;
    break;
  }
  double band = dbbcx->Band();
  if (band < Freq()) 
    freq = (freq*band + dbbcx->LoFreq())/Freq();
  double fp=freq;
  int i;
  for (i=1; i<7; ++i) {
    ampl += fp * coef[i];
    fp = fp*freq;
  }
  double x = (freq - fline)*(freq - fline)/wline;
  if (x < 5) ampl += 0.2*exp(-x);
  return (int)(ampl*0.025*int_length*dbbcx->Gain()*dbbcy->Gain());
} 

int FftSpectrometer::read_memory()
{
  static int scale_fact[] = {20,16,5,-2}; // scale xx, yy, re(xy), im(xy)
  int mem_bank = (read_addr >> 14) & 3;
  int byte_pos = (read_addr & 3); 
  read_addr++;
  if ((read_addr & 3) == 0) read_addr += (resolution -1) << 2;
  if (byte_pos == 0) {
    int mem_start = ((read_addr>>2) & 0x7ff) & (-resolution);
    int i;
    mem_value = 0;
    for (i=mem_start;  i< mem_start+resolution; ++i) 
      mem_value += compute_spectrum(i, mem_bank);
      mem_value *= scale_fact[mem_bank];
  }
  return (mem_value >> (byte_pos*8)) & 0xff;
}

void FftSpectrometer::enable_timer()
{
  if (enable) int_thread.start();
  else        int_thread.stop();
}

FftSpectrometer::~FftSpectrometer()
{
}

//
// return integration time in seconds
//
double FftSpectrometer::IntTime()
{
   double IntTimeBase = n_channels/dbbc1->Band();
   return int_length*IntTimeBase*2;
}
