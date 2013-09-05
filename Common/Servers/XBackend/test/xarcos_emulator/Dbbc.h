#ifndef _DBBC_H_
#define _DBBC_H_

#include "HardwareBlock.h"
class TpThread; // forward declaration
#include "TpThread.h"

class Dbbc : public HardwareBlock
{
   TpThread int_thread;
   int tpreg;
   int control;
   int enable;
   int tp_ready;
   int rdg_ready;
   int tp_value;
   int rdg_value;
   int bypass;
   int band;
   int gain;
   int freq_buf;
   int freq_reg;
   int phase_reg;
   int rdg_control;
   int input_sel;
   int mode_8bit;
   int tp_sel;
 private:
   void enable_timer(); 
   void ComputeTotalpower(); 
 public:
   Dbbc(double freq);
   ~Dbbc();
   int WriteFpgaD(int chip, int addr, int val);
   int ReadFpga(int chip, int addr);
   double Band();
   double LoFreq();
   int Gain() { return gain;}
   void SetEndIntegration() {
      ComputeTotalpower();
      tp_ready = true; rdg_ready = true; }
};

#endif  // _DBBC_H_
