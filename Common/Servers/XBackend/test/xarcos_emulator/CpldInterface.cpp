#include "CpldInterface.h"
#include "iostream"

using namespace std;

#define DEBUG 1

/*******************************************************************************
        Server.c
        Comore, 2005

        A basic server, that when a client connect, decodes simple commands 
        and send back responses
*******************************************************************************/

#include <stdio.h>
#include <string.h>

int CpldInterface::process_buffer(char *buffer, int size)
{
    int i;
    static long address, cur_adr;
    char hextab[17] = "0123456789abcdef";
    static int brd=0; 
    int adr, reg, val, length;
    char * pos;
    char data_stream[4096];
    //FILE * inp_file ;
    buffer[2047]='\0';				/* Terminate string */
    for (i=0; buffer[i] == ' '; ++i) ;  	/* Skip blanks */
    if (buffer[i]=='\0') return 0;		/* Null string */
    pos = buffer+i+2;
/*    printf("Received %s\n", buffer+i); */

    if (download_mode) {
	switch (process_record(buffer+i, data_stream, &address, &length)) {
        case NORMAL_REC:
//            while (address > cur_adr) {
//                write(brd, CPLD2_DATA_BRC, 0);
//                cur_adr++;
//            }
//            for (i=0; i< length; ++i) {
//                write(CPLD2_DATA_BRC, data_stream[i]);
//                cur_adr++;
//            }
//            write_block(brd, CPLD2_DATA_BRC, data_stream, length);
	    cur_adr +=length;
            break;
        case SEGMENT_REC:
        case OFFSET_REC:
            break;
        case ERR:
	    download_mode=0;
	    strcpy(buffer, "Download error");
	    end_prog(brd);
	    return 1;
	case END_REC:
	    download_mode=0;
            strcpy(buffer, "Download completed");
            end_prog(brd);
            return 1;
	}
	return 0;
    }

    switch (buffer[i]) {
    case 'w': case 'W':			/* Write: */
    	switch (buffer[i+1]) {
	case 'l': case 'L':		/* WL: Write CPLD2 reg */
    	    brd = hex2d(&pos);
    	    adr = hex2d(&pos);
    	    val = hex2d(&pos);
    	    if (adr < 0 || val < 0) {
    		strcpy(buffer, "Syntax error\n");
    		return 1;
    	    }
//	    write(brd, adr, val);
	    break;
	case 'c':  case 'C':		/* WC: Write control reg */
    	    brd = hex2d(&pos);
	    adr = hex2d(&pos);
    	    val = hex2d(&pos);
    	    if (adr < 0 || val < 0) {
    		strcpy(buffer, "Syntax error\n");
    		return 1;
    	    }
//	    write(brd, CPLD2_ADDR_REG, adr);
//	    write(brd, CPLD2_CTRL_REG, val);
            writeC(brd,adr, val); 
	    break;
	case 'd': case 'D':
    	    brd = hex2d(&pos);
	    adr = hex2d(&pos);
    	    reg = hex2d(&pos);
    	    val = hex2d(&pos);
    	    if (adr < 0 || val < 0 || reg < 0) {
    		strcpy(buffer, "Syntax error\n");
    		return 1;
    	    }
//	    write(brd, CPLD2_ADDR_REG, adr);
//	    write(brd, CPLD2_CTRL_REG, reg);
//	    write(brd, CPLD2_DATA_REG, val);
            writeD(brd, adr, reg, val); 
            if (DEBUG) cout << "writeD " << brd << " " << adr 
                      << " " << reg << " " << val << endl;
	    break;
	case 'm': case 'M':
    	    brd = hex2d(&pos);
	    adr = hex2d(&pos);
    	    reg = hex2d(&pos);
    	    length = hex2d(&pos);
    	    if (adr < 0 || length <= 0 || reg < 0 || length > 16) {
    		strcpy(buffer, "Syntax error\n");
    		return 1;
	    }
//	    write(brd, CPLD2_ADDR_REG, adr);
//	    write(brd, CPLD2_CTRL_REG, reg);
	    for (i=0; i<length; ++i) data_stream[i]=hex2d(&pos);
	    write_block(brd, adr, reg, data_stream, length);
	    break;
	default:
	    strcpy(buffer, "Syntax error\n");
    	    return 1;
        }
      	break;
     case 'r': case 'R':
	switch (buffer[i+1]) {
  	case 'l': case 'L':
    	    brd = hex2d(&pos);
  	    adr = hex2d(&pos);
    	    if (adr < 0 ) {
    		strcpy(buffer, "Syntax error\n");
    		return 1;
    	    }
//    	    val = read_cpld2(brd, adr);
            val = 0;
    	    sprintf(buffer, "BRD %d CPLD2 reg %d = %x\n", brd, adr, val);
  	    break;
  	case 'd': case 'D':
    	    brd = hex2d(&pos);
  	    adr = hex2d(&pos);
    	    reg = hex2d(&pos);
    	    if (adr < 0 || reg < 0) {
    		strcpy(buffer, "Syntax error\n");
    		return 1;
    	    }
//	    write(brd, CPLD2_ADDR_REG, adr);
//	    write(brd, CPLD2_CTRL_REG, reg);
    	    val = read(brd, adr, reg);
    	    sprintf(buffer, "BRD %d FPGA %d reg %d = %x\n", 
				brd, adr, reg, val);
	    if (DEBUG) cout << "read " << buffer;
  	    break;
  	case 'm': case 'M':
    	    brd = hex2d(&pos);
  	    adr = hex2d(&pos);
  	    reg = hex2d(&pos);
  	    length = hex2d(&pos);
     	    if (adr < 0 || reg < 0 || length < 1 || length > 255) {
    		strcpy(buffer, "Syntax error\n");
    		return 1;
    	    }
//	    write(brd, CPLD2_ADDR_REG, adr);
//	    write(brd, CPLD2_CTRL_REG, reg);
 	    sprintf(buffer, "BRD %d FPGA %2x reg %2x len %2x = ", 
				brd, adr, reg, length);
	    read_block(brd, adr, reg, data_stream, length);
 	    for (i=0; i<length; ++i) {
		val = data_stream[i];
    		buffer[2*i+30] = hextab[(val & 0xf0) >>4];
    		buffer[2*i+31] = hextab[val &0xf];
    	    }
    	    buffer[2*i+30]='\n';
	    buffer[2*i+31]=0;
    	    break;
	case 'b': case 'B':
    	    brd = hex2d(&pos);
  	    adr = hex2d(&pos);
  	    reg = hex2d(&pos);
  	    length = hex2d(&pos);
     	    if (adr < 0 || reg < 0 || length < 2 
			    || length > size) {
    		strcpy(buffer, "Syntax error\n");
    		return 1;
    	    }
//	    write(brd, CPLD2_ADDR_REG, adr);
//	    write(brd, CPLD2_CTRL_REG, reg);
	    read_block(brd, adr, reg, buffer, length);
	    return length;
  	default:
  	    strcpy(buffer, "Syntax error\n");
	    break;
	}
	return 1;
//    case 'd': case 'D':
//    	brd = hex2d(&pos);
//	adr = hex2d(&pos);
//	switch (buffer[i+1]) {
 // 	case 'l': case 'L':
//	    write(brd, CPLD2_ADDR_REG, (adr& 0xff));
//	    write(brd, CPLD2_ADR2_REG, ((adr>>8)& 0xff));
/*	write(CPLD2_C_REG, 2); */
//	    write(brd, CPLD2_C_REG, 0);
//	    download_mode = 1;
//	    address=0;
//	    cur_adr=0;
//	    break;
 // 	case 'f': case 'F':
//	    while (*pos == ' ') pos++;
//	    inp_file = fopen(pos, "r");
//	    if (inp_file == NULL) {
 // 	    	sprintf(buffer, "Error opening %s\n", pos);
//		return -1;
//	    }
//	    write(brd, CPLD2_ADDR_REG, (adr& 0xff));
//	    write(brd, CPLD2_ADR2_REG, ((adr>>8)& 0xff));
/*	write(CPLD2_C_REG, 2); */
//	    write(brd, CPLD2_C_REG, 0);
//	    while (
//		(length=fread(data_stream, 1, sizeof data_stream, inp_file))>0) 
//		write_block(brd, CPLD2_DATA_BRC, data_stream, length);
//	    fclose(inp_file);
 //           strcpy(buffer, "Download completed");
  //          end_prog(brd);
   //         return 1;
//  	default:
//  	    strcpy(buffer, "Syntax error\n");
//	    return 1;
//	}
//	break;
    case 'c':
	if (strncasecmp(buffer, "close", 5) == 0) 
	strcpy(buffer, "Exiting\n");
	    return -1;
    default:
	strcpy(buffer, "Syntax error\n");
      	return 1;
    }
    return 0;
}

int CpldInterface::hex2d(char ** buf)
{
   int val, c;
   char *buf1;
   val=0;
   buf1 = *buf;
   while (*buf1 == ' ') buf1++;
   while (*buf1 != '\0' && *buf1 != ' ') {
      c=*buf1;
      if (c >= '0' && c <= '9') {
      	c = c-'0';
      } else if (c >= 'A' && c <= 'F') {
      	c = c-'A'+10;
      } else if (c >= 'a' && c <= 'f') {
      	c = c-'a'+10;
      } else return -1;
      val = val*16+c;
      buf1++;
   }
   *buf=buf1;
   return val;
}

int CpldInterface::process_record(
        char *record,           /* input record */
        char *data_stream,      /* output data */
        long *address,           /* current address, including segment */
        int *length)
{
    long segment, adr;
    int type, checksum;
    int i;

    segment = *address & 0xffff0000;
    checksum=0;
    adr = 0;
    if (record[0] != ':') return ERR;
    *length = get_rec_byte(&record[1], &checksum);
    adr = get_rec_word(&record[3], &checksum);
    type = get_rec_byte(&record[7], &checksum);
    if (length == 0 && type == NORMAL_REC) type = END_REC;
    switch (type) {
    case NORMAL_REC:
        for (i=0; i< *length; ++i)
            data_stream[i] = get_rec_byte(&record[i+i+9], &checksum);
        *address = segment | adr;
        get_rec_byte(&record[i+i+9], &checksum);
        if (checksum != 0) type = ERR;
        break;
    case SEGMENT_REC:
        adr = get_rec_word(&record[9], &checksum);
        *address = adr << 4;
        get_rec_byte(&record[13], &checksum);
        if (checksum != 0) type = ERR;
        break;
    case OFFSET_REC:
        adr = get_rec_word(&record[9], &checksum);
        *address = adr << 16;
        get_rec_byte(&record[13], &checksum);
        if (checksum != 0) type = ERR;
        break;
    default:
        break;
    }
    return type;
}

int CpldInterface::get_rec_byte(char rec[], int *cks)
{
   int val, c, i;
   val=0;
   for (i=0; i<2; ++i) {
      c=*rec;
      if (c >= '0' && c <= '9') {
        c = c-'0';
      } else if (c >= 'A' && c <= 'F') {
        c = c-'A'+10;
      } else if (c >= 'a' && c <= 'f') {
        c = c-'a'+10;
      } else return -1;
      val = val*16+c;
      rec++;
   }
   *cks = (*cks + val) & 0xff;
   return val;
}

int CpldInterface::get_rec_word(char rec[], int *cks)
{
    return (get_rec_byte(rec, cks)<<8) | get_rec_byte(rec+2, cks);
}

void CpldInterface::end_prog(int brd)
{
}
