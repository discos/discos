//
// BBC filter
//
// Class based on Altera correlator
// Implements a tunable BBC and filter
//
#include "../include/BbcFilter.h"
#include <math.h>       // for floor()
#include <unistd.h>       // for usleep()

using namespace std;

static const double maxval=16777216.0;      // 2^24

//
// Methods to update hardware registers
// Return error code or 0
//
int BbcFilter::SetGain() {
    int err=0;
    err = WriteFpgaD(2, gain);
    return err;
}

int BbcFilter::SetFreq()
{
    int err=0;
    int i, a_freq=lofreq;
    for (i=0; i<3; ++i) {
	int t1 = (a_freq & 0xff0000) >> 16;
	err = WriteFpgaD(3, t1);if(err!=0) return err;
	a_freq = a_freq << 8;
    }
    err = WriteFpgaD(5, lophase&0xff);if(err!=0) return err;
    err = WriteFpgaD(4, 0);
    return err;
}

int BbcFilter::SetBand()
{
    int err=0;
    if (band <= 7) {
    	err = Control(band | 0x40);
    } else {
	err = Control(0); // bypass mode
    }
    return err;
}
//
// Constructor
//
BbcFilter::BbcFilter(Cpld2Intf *a_intf, int addr, double a_clock)
    : HardwareBlock(a_intf, addr, a_clock),
    	lofreq(0x200000), lophase(0), gain(40), band(8), inputChan(3),
		inputBit(7), mode8bit(true)
{
	lo_resolution=clockFrequency/maxval;
	int i;
	double band1=clockFrequency;
	for (i=8; i >=0; --i) {
	    bandtable[i]=band1;
	    band1 *= 0.5;
	}
}

int BbcFilter::Gain(int a_gain)
{
    if (a_gain < 0) a_gain = 0;
    if (a_gain > 255) a_gain = 255;
    return  gain = a_gain;
}

double BbcFilter::Band(int a_band)
{
    band = a_band;
    if (band < 0) band = 0;
    if (band > 8) band = 8;
    return bandtable[band];
}

// Set LO frequency
// Can be set from -fsample/2 to + fsample/2
// Negative LO frequency reverses frequency scale
double BbcFilter::Freq(double a_freq, double a_phase)
{
    const double maxlo=clockFrequency*0.5;
    if (a_freq < -maxlo) a_freq=-maxlo;
    if (a_freq >  maxlo) a_freq= maxlo;
    lofreq=int (floor(a_freq/lo_resolution) + 0.5);
    lophase = int (floor(a_phase*256.)+0.5);
    return lofreq*lo_resolution;
}

//
// Set band to minimum avaliable >= than the argument
// a_band is expressed in Hz
// Return actual band
//
double BbcFilter::Band (double a_band)
{
    int i;
    for (i=0; i<8; ++i) {
	if (bandtable[i] >= a_band*0.99) break;
    }
    if (i > 8) i=8;
    band = i;//***********************************************************non Serve!!
    return bandtable[i];
}
//
// Return corresponding band, in Hz
//
double BbcFilter::HwBand(double a_band) const
{
    int i;
    for (i=8; i>=0; --i) {
	if (bandtable[i] >= a_band*0.99) break;
    }
    if (i < 0) i=0;
    return bandtable[i];
}
//
// Set input channel and bit for RD checker
//
int BbcFilter::InputChannel(int chan, bool len8, int bit)
{
    if (bit < 0) bit = 0;
    if (bit > 15) bit = 15;//Questo non lo so è da chiedere
    inputChan = chan & 0x1b;
    inputBit = bit;
    mode8bit = len8;//true
    SetChan();
    return chan;
}
//
// Set input channel, 8 bit mode, and RDC bit.
// Reset RDC
//
int BbcFilter::SetChan()
{
	int err=0;
	WriteFpgaD(6, (inputBit & 0xf) | 0x80);if(err!=0) return err;
	WriteFpgaD(6, (inputBit & 0xf) | 0x00);if(err!=0) return err;
	int r = (inputChan & 0x3f ) ;
	  
//    WriteFpgaD(6, (inputBit & 7) | 0x80);if(err!=0) return err;
//    WriteFpgaD(6, (inputBit & 7) | 0x00);if(err!=0) return err;
//    int r = (inputChan & 0x1b ) ;
    if (mode8bit) r |= 0x8;
    return WriteFpgaD(7, r);
}

int BbcFilter::Set()
{
    int err=0;
    err = SetBand();if(err!=0) return err;
    err = SetGain();if(err!=0) return err;
    err = SetFreq();if(err!=0) return err;
    err = SetChan();
    return err;
}

#include <iostream>	// for usleep?
//
// Read Total power
// If wait is true, reset total power and wait for 1 integration time
// Else begin to test for data available
//
int BbcFilter::TPRead(bool wait)
{
	int count=0;
	int err = 0;
    int bandcode = band | 0x40;
    if (band == 8) bandcode= 0;
    if (wait) {//azzero e aspetto per effettuare la misura
    	Control(0x18+bandcode);
    	Control(0x00+bandcode);
    	usleep(200000);
    }
	while ((count<20)&&(err = Status() & 0x10) == 0) {
		count++;//per evitare il loop infinito
		usleep(10000);
	}
    if (count==20) return 0;
    err = (( ReadFpga(7)& 0xff) << 24) +
    	  (( ReadFpga(6)& 0xff) << 16) +
	  (( ReadFpga(5)& 0xff) << 8) +
	   ( ReadFpga(4)& 0xff);
    Control(0x10+bandcode);	// reset ready bit
    Control(0x00+bandcode);
    return err;
}

int BbcFilter::Start()
{

	int err = 0;
    int bandcode = band | 0x40;
    if (band == 8) bandcode= 0;

	 Control(0x18+bandcode);//azzero solo senza effettuare la misura
	 Control(0x00+bandcode);
	return err;
}
