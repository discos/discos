#ifndef _HARDWARE_BLOCK_
#define _HARDWARE_BLOCK_

class HardwareBlock
{
    double freq;
  public:
    HardwareBlock(double a_freq) : freq(a_freq) {}
    virtual ~HardwareBlock() {}
    virtual int WriteFpgaD(int chip, int addr, int val) {return 0;}
    virtual int WriteFpgaC(int chip, int val) {return 0;}
    virtual int ReadFpga(int chip, int addr) {return 0;}
    double Freq() { return freq;}
};

#endif  // _HARDWARE_BLOCK_
