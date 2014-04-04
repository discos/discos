#include "../include/Spectrometer.h"
using namespace std;

//
// Integration time, specified in seconds.
// Rounded to nearest possible value.
// Integration granularity depends on bandwidth, so it must be set
// AFTER bandwidth.
// Returns actual integration time
//
double Spectrometer::IntTime(double a_time)	// set int. time (s)
{
    integrationTime = a_time;
    return IntTime();
}

//
// Constructor
//
Spectrometer::Spectrometer(Cpld2Intf * a_intf, int addr,double clockFreq, int a_nchans)
  : HardwareBlock(a_intf, addr, clockFreq),
    aborted(false), active(false),
    nChans(a_nchans), resolution(1), bbc(0)
{
   IntTime(2.0);
   Init();
}

int Spectrometer::Init()
{
  int i;
  for (i=0; i<3; ++i) WriteFpgaC(0); // exit from Xilinx Init state
  TestPoint(0);		// Clear TestPoint register
  Control(0xff);		// Pulse all resets, including PLL
  Control(0x3);		// Leave in stopped status
  return 0;			// Update with error code
}

int Spectrometer::Start() 		// Start integration
{
	int err = 0, t1;
	//t1=ResetStatus();
	if ((err = Set()) != 0) return err;
	t1 = HardwareBlock::Status(); 	// clear all bits in control
	t1 = t1 & 0x70;      //	except resolution
        Control(t1);
        Control(t1 | 0x0c);
        Control(t1);       // Clear pending overflow and data ready			
	return 0;
}


int Spectrometer::Stop() 			// Stop integration
{
  return Control(3);
}

int Spectrometer::Abort() 			// Stop integration
{
  Control(3);
  return Control(2);
}

bool Spectrometer::DataReady()		// Check if new data are ready
{
  int err = 0;
  int t1 = ReadFpga(1);
  bool ret = false;
  if (t1 & 0x4) {
    ret = true;
    err = Control(t1 &0x77);
    err = Control(t1 &0x73);
  }
  return ret;
}

bool Spectrometer::Overflow()		// Check if overflow occurred
{
  int err = 0;
  int t1 = ReadFpga(1);
  bool ret = false;
  if (t1 & 0x8) {
    ret = true;
    err = Control(t1 &0x7b);
    err = Control(t1 &0x73);
  }
  return ret;
}

int Spectrometer::ResetStatus()		// polls status bits
{
	int err=0;//fatto per prova
	int t1=0;

	err=Control(0);
	t1=Status();
	return t1;
}

int Spectrometer::Status()		// polls status bits
{
  int err = 0;
  int t1 = HardwareBlock::Status();
  if (t1 & 0x8) {
    err = Control(t1 &0x7b);
    err = Control(t1 &0x73);
  }
  if (t1 & 0x4) {
    err = Control(t1 &0x77);
    err = Control(t1 &0x73);
  }
  return t1;
}

int Spectrometer::GetResults(long *data, int size, int start)
{
  char *databuf;
//
// Correct for actual resolution
//
  start = start * resolution;
  if (size*resolution > nChans) size = nChans/resolution;
//
// Read the data
//
  WriteFpgaD(2, (start & 0xff00) >> 8);
  WriteFpgaD(2, start & 0xff);
  databuf = new char[size*4];
  ReadFpgaBlock(3, databuf, size*4);
//
// Convert to long
//
  int i;
  for (i=0; i< size; ++i) {
    int32 value=0;
    int i2=i*4+3, j;
    for (j=0; j<4; ++j) {
      value = (value <<8) | ((long)(databuf[i2-j]) & 0xff);
    }
    data[i]=value;
  }
  delete[] databuf;
  return size;
}

#include <math.h> 			// for ceil()
int Spectrometer::SetIntTime() 		// Program Int. Time
{
  int err=0;
  if (bbc == 0) return -1;
  cycleTime = (double)nChans/bbc->Band();
  int i, a_ncycles;
  nCycles=(int)ceil(integrationTime/cycleTime);
  a_ncycles=nCycles;
  if (nCycles > 0x3ffff) nCycles = 0x3ffff;
  for (i=0; i<3; ++i) {
    int t1 = (a_ncycles & 0xff0000) >> 16;
    err = WriteFpgaD(4, t1);
    a_ncycles = a_ncycles << 8;
  }
  return err;
}

//
// Integration time limited to 2^24 cycles by hardware
// Each cycle lasts (nchans*tsample) and tsample = 1/band
// Care must be taken not to overflow the integrator,
// this may occur much sooner
//
double Spectrometer::IntTime() const
{
    if (bbc == 0) return integrationTime;
    double a_cycleTime = (double)nChans/bbc->Band();
    unsigned int a_ncycles = (unsigned int)ceil(integrationTime/a_cycleTime);
    if (a_ncycles > 0xffffff) a_ncycles = 0xffffff;
    return (double)a_ncycles*a_cycleTime;
}

int Spectrometer::Set() 		// HW initialization
{
  int err=0;
  Abort();
  if ((err = SetIntTime()) != 0) return err;
  int i, r=1;
  for (i=0; i < 0x80; i += 0x10) {
    if (r >= resolution) break;
    r +=r;
  }
  err = Control(2 | i);
  return err;
}

int Spectrometer::Resolution( int a_res )
{
    if (a_res > 256) a_res = 128;		// Max resolution
    for (resolution=1; resolution >= a_res; resolution += resolution) {}
    return resolution;
}
