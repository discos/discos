#pragma once

#define NORMAL_REC 0
#define END_REC 1
#define SEGMENT_REC 2
#define OFFSET_REC 4
#define OK 0
#define ERR -1

class CpldInterface
{
//Private methods
  int download_mode;
  int hex2d(char **buf);
  int process_record(char *record, char *data_stream, long *address, 
      int *lenght);
  int get_rec_byte(char rec[], int *cks);
  int get_rec_word(char rec[], int *cks);
  void end_prog(int brd);

   public:
   CpldInterface() { download_mode = 0;}
   virtual ~CpldInterface(){ }
   virtual void writeC(int card, int chip, int val) {};
   virtual void writeD(int card, int chip, int adr, int val)=0;
   virtual int  read(int card, int chip, int adr)=0;
   virtual void write_block(int card, int chip, int adr, 
           char *val, int num)=0;
   virtual void read_block(int board, int chip, int adr, 
           char *val, int num)=0;
   int process_buffer(char *buffer, int size);
};

