//
// Class based on Altera correlator
//
//
// Constructor requires an already opened AlmaInterface object
// for communication, and the clock frequency used in the chip (default is
// 125 MHz)
//
//
#include "../include/AdcConditioner.h"
#include <unistd.h>
using namespace std;

const double AdcConditioner::lo_resolution=512;
const int AdcConditioner::time_resolution=4;

int AdcConditioner::TestPoint(int a_tp)
{
	int err=0;
    err=WriteFpgaD(0x10,a_tp&0xff);if(err!=0) return err;
	return WriteFpgaD(0x20,(a_tp>>8)&0xff);
}

bool AdcConditioner::CheckTP(int a_tp)
{
	int err=0;
	int a = a_tp & 0xff;
	err=WriteFpgaD(0x10,a);if(err!=0) return false;
	WriteFpgaD(0x10,a);
	if (a != ReadFpga(0x10)) return false;
	a = (a_tp>>8)&0xff;
	err=WriteFpgaD(0x20,a);if(err!=0) return false;
	if (a != ReadFpga(0x20)) return false;
	return true;
}

int AdcConditioner::Control(int val)
{
	return WriteFpgaD(0x31, val);
}

int AdcConditioner::Distribution(int val)
{
	WriteFpgaD(0x12, val);
	WriteFpgaD(0x22, 1);
	return WriteFpgaD(0x22, 0);
}

AdcConditioner::AdcConditioner(Cpld2Intf *a_intf, int addr, double clockFreq)
	: HardwareBlock(a_intf, addr, clockFreq)
{
	int err=0;
	mode=0;//Function ADC //2
	hwFreq=0;
	int_time=250;

	err=Control(mode);
	//TestPoint(0);
	//WriteFpgaD(0x12,0);//registro di distribuzione
	err=Distribution(0);//Sostituisco il comando della linea commentata sopra
}

double AdcConditioner::Freq(double a_freq)
{
	if (a_freq > clockFrequency*0.5 || a_freq < 0.) a_freq = 0;
	hwFreq = (int)((double)a_freq * lo_resolution/clockFrequency+0.5);
	return Freq();
}

int AdcConditioner::Set()
{
	int err=0;
	err=WriteFpgaD(0x19, int_time-1);if(err!=0) return err;
	err=WriteFpgaD(0x31, mode);if(err!=0) return err;
	err=WriteFpgaD(0x33, hwFreq);
	return err;
}

int AdcConditioner::SetMode(int source, bool bypass, bool dc_keep)
{
	mode = source&3;
	if (bypass) mode |= 8;
	if (dc_keep) mode |= 0x10;
	return mode;
}

int AdcConditioner::IntTime(int ms)
{
	int_time=ms/time_resolution;
	return IntTime();
}

int AdcConditioner::ResetTotPow(bool reset)
{
	int err=0;
	if (reset) err=WriteFpgaD(18, 1);
	else	   err=WriteFpgaD(18, 0);
	return err;
}

int AdcConditioner::ReadTotPow(long &xx, long &yy, long &xyre, long &xyim)
{
	int err=0;
	ResetTotPow(false);
	int status;
	while ((status = ReadFpga(0x12) & 0xf) != 0xf) continue;
	int32 bufarray[4];
	int i,j;
	for (j=0; j < 4; ++j) {
		long buf=0;
		for (i=8; i > 0; i-=2) {
			err=WriteFpgaD(0x18, j*16+i);if(err!=0) return -3;
			buf = (buf <<8) + (ReadFpga(13) & 0xff);
		}
		bufarray[j]=buf;
	}
	status = ReadFpga(0x12) & 0xf0;
	xx = bufarray[0];
	yy = bufarray[1];
	xyre = bufarray[2];
	xyim = bufarray[3];
	err=WriteFpgaD(0x1a,0);if(err!=0) return err;
	if (status != 0) return -2;
	return 0;
}

//
// Total power read in normalized units
// Units are hardware counts as seen in Capture method, or as sent on the
// backplane in 8 bit mode
// No correction is made for DC bias (readout is higher by 1/16 than true value)
// Return -1 for integration time  of 0,
// 	  -2 for read error (read too slow)
//
int AdcConditioner::ReadTotPow(double &xx, double &yy, complex<double> &xy)
{
	long xxl, yyl, xyrl, xyil;
	int err = ReadTotPow( xxl, yyl, xyrl, xyil);
	if (int_time == 0) return -1;
	double norm = 256./125.*1.0e-3/(int_time*time_resolution);
	xx = norm*(double)xxl;
	yy = norm*(double)yyl;
	xy = complex<double>(norm*(double)xyrl, norm*(double)xyil);
	return err;
}

// Data capture
// Data are in hardware units, for 8 bit quantization
// without correction for DC bias
// If bypeass mode is selected, samples from ADC are captured
// In normal mode, real-imag pairs are stored in consecutive
// buffer positions
int AdcConditioner::Capture(int buffer[2][2048])
{
	int err=0;
	err=WriteFpgaD(0x28,0);if(err!=0) return err;	// Arm capture
	usleep(2000);		// wait for capture to occur
	char *buf_c = new char[2048];
	int j;
	for (j=0; j<2; ++j) {
	    int mask = (j << 7);
	    int i=0;
	    err=WriteFpgaD(0x2a, mask | (i >> 8));if(err!=0) return err;
	    err=WriteFpgaD(0x29, i & 0xff);if(err!=0) return err;
	    err=ReadFpgaBlock(0x22, buf_c, 2048);if(err==0) return -1;
	    for (i=0; i<2048; ++i) buffer[j][i]=buf_c[i];
	}
	delete[] buf_c;
	return 0;
}

int AdcConditioner::GetRDC(int line)
{
	if (line >= 0) {
	    Distribution(line & 0x1f);	
	    WriteFpgaD(0x22,1);
	    WriteFpgaD(0x22,0);
	    usleep(3000);
	}
	int err = ReadFpga(0x23); // Acquire LSB
	WriteFpgaD(0x22,2);
	err += 256*ReadFpga(0x23); // Acquire MSB
	WriteFpgaD(0x22,0);
	return err;
}
