using namespace std;

#include "../include/HardwareBlock.h"

HardwareBlock::HardwareBlock(Cpld2Intf *a_intf, int addr, double clock_freq)
	: ctrl(0), intf(a_intf), clockFrequency(clock_freq)
{
	int err=0;
    board=(addr & 0x7000) >> 12;
    chip =(addr & 0xf00) >> 8;
    mask = addr & 0xff;
    err=Init(); 
}

int HardwareBlock::Init() 
{
    int err=0;
    err=WriteFpgaC(0);if(err!=0) return err;
    err=WriteFpgaC(0);if(err!=0) return err;
    err=WriteFpgaC(0);if(err!=0) return err;
    err=Control(0x80);if(err!=0) return err;
    err=Control(0x0); 
    return err;
}

int HardwareBlock::TestPoint(int tp)
{ return WriteFpgaD(0, tp); }

bool HardwareBlock::CheckTP(int tp)
{
    int err=0;
    err=WriteFpgaD(0, tp);if(err!=0) return err;
    return (ReadFpga(0) == tp);
}

int HardwareBlock::Control(int val)
{ return WriteFpgaD(1, val); }

int HardwareBlock::Status()
{ return ReadFpga(1); }

