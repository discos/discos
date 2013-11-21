// $Id: ACUInterface.cpp,v 1.4 2010-09-24 15:38:53 a.orlati Exp $

#include "ACUInterface.h"
#include <math.h>

#define STX 0x02
#define ETX 0x03
#define ACK 6
#define NAK 21

WORD CACUInterface::getMonitorBuffer(BYTE *Buff)
{
	short int Checksum=0;
	WORD DataLength=7;
	int n=0;
	Buff[0]=STX;
	Buff[1]='q';
	memcpy((void *)(Buff+2),(const void *)&DataLength,2);
	for (n=1;n<4;n++)
		Checksum=Checksum+Buff[n];
	memcpy((void *)(Buff+4),(const void *)&Checksum,2);
	Buff[6]=ETX;
	return DataLength;
}

WORD CACUInterface::getFailureResetBuffer(BYTE* Buff)
{
	WORD DataLength=7;
	short int Checksum=0;
	int n;
	Buff[0]=STX;
	Buff[1]='X';
	memcpy((void *)(Buff+2),(const void *)&DataLength,2);
   for (n=1;n<4;n++) Checksum=Checksum+Buff[n];
	memcpy ((void *)(Buff+4),(const void *)&Checksum,2);
	Buff[6]=ETX;
	return DataLength;
}

WORD CACUInterface::getTimeTransferBuffer(BYTE* Buff,int dayTime,short int yearDay,short int year)
{
	short int Checksum = 0;
	WORD DataLength = 15;
	int n;
	Buff[0]=STX;
	Buff[1]='T';
	memcpy((void *)(Buff+2),(const void *)&DataLength,2);
	memcpy((void *)(Buff+4),(const void *)&dayTime,4);
	memcpy((void *)(Buff+8),(const void *)&yearDay,2);
	memcpy((void *)(Buff+10),(const void *)&year,2);
	for (n=1;n<12;n++) Checksum=Checksum+Buff[n];
	memcpy((void *)(Buff+12),(const void *)&Checksum,2);
	Buff[14]=ETX;
	return DataLength;
}	
	
WORD CACUInterface::getPresetBuffer(BYTE *Buff,long Azim,long Elev)
{
	WORD DataLength=15;
	short int Checksum=0;
	int n;
	Buff[0]=STX;
	Buff[1]='P';
	memcpy((void *)(Buff+2),(const void *)&DataLength,2);
   memcpy((void *)(Buff+4),(const void *)&(Azim),4);
   memcpy((void *)(Buff+8),(const void *)&(Elev),4);
	for (n=1;n<12;n++) Checksum=Checksum+Buff[n];
	memcpy ((void *)(Buff+12),(const void *)&Checksum,2);
	Buff[14]=ETX;
	return DataLength;
}

WORD CACUInterface::getRateBuffer(BYTE *Buff,long azVel,long elVel)
{
	WORD DataLength=15;
	short int Checksum=0;
	int n;
	Buff[0]=STX;
	Buff[1]='R';
	memcpy((void *)(Buff+2),(const void *)&DataLength,2);
   memcpy((void *)(Buff+4),(const void *)&azVel,4);
   memcpy((void *)(Buff+8),(const void *)&elVel,4);
	for (n=1;n<12;n++) Checksum=Checksum+Buff[n];
	memcpy((void *)(Buff+12),(const void *)&Checksum,2);
	Buff[14]=ETX;
	return DataLength;
}

WORD CACUInterface::getModeSelectionBuffer(BYTE *Buff,TAxeModes Az,TAxeModes El,bool DrivePower)
{
	unsigned short int DataLength=11;
	int n=0;
	short int Checksum=0;
	unsigned char LSB_control=1;
	unsigned char MSB_control;
	Buff[0]=STX;
	Buff[1]='M';
	if (DrivePower) MSB_control=128;
	else MSB_control=1;
	memcpy ((void *)(Buff+2),(const void *)&DataLength,2);
	Buff[4]=(BYTE)Az;
	Buff[5]=(BYTE)El;
	Buff[6]=LSB_control;
	Buff[7]=MSB_control;
	for (n=1;n<8;n++)
		Checksum=Checksum+Buff[n];
	memcpy((void *)(Buff+8),(const void *)&Checksum,2);
	Buff[10]=ETX;
	return DataLength;
}

WORD CACUInterface::getPositionTrackBuffer(BYTE *Buff,long Azim,long Elev,long azVel,long elVel,bool Fit)
{
	unsigned short int DataLength=25;
	short int Checksum=0;
	int n;
	Buff[0]=STX;
	Buff[1]='o';
	if (Fit) {
		azVel=elVel=-11111000;
	}
	memcpy((void *)(Buff+2),(const void *)&DataLength,2);
    memcpy((void *)(Buff+6),(const void *)&(Azim),4);
    memcpy((void *)(Buff+10),(const void *)&(azVel),4);
    memcpy((void *)(Buff+14),(const void *)&(Elev),4);
    memcpy((void *)(Buff+18),(const void *)&(elVel),4);
    for (n=1;n<22;n++) Checksum=Checksum+Buff[n];
    memcpy((void *)(Buff+22),(const void *)&Checksum,2);
    Buff[24]=ETX;
	return DataLength;
}

WORD CACUInterface::getProgramTrackBuffer(BYTE *Buff,long Azim,long Elev,long azVel,long elVel,long dayTime,short yearDay,bool Fit,bool Clear)
{
	WORD DataLength=31;
	short int cl=0;
	short int Checksum=0;
	int n=0;
	char cmd='O';
	Buff[0]=STX;
	Buff[1]=cmd;
	memcpy((void *)(Buff+2),(const void *)&DataLength,2);
	if (Clear) cl=0;
	else cl=1;
	if (Fit) {
		azVel=-11111000;
		elVel=-11111000;
	}
	memcpy((void *)(Buff+4),(const void *)&cl,2);
	memcpy((void *)(Buff+6),(const void *)&dayTime,4);
	memcpy((void *)(Buff+10),(const void *)&yearDay,2);
    memcpy((void *)(Buff+12),(const void *)&Azim,4);
    memcpy((void *)(Buff+16),(const void *)&azVel,4);
    memcpy((void *)(Buff+20),(const void *)&Elev,4);
    memcpy((void *)(Buff+24),(const void *)&elVel,4);
    for (n=1;n<28;n++) Checksum=Checksum+Buff[n];
    memcpy((void *)(Buff+28),(const void *)&Checksum,2);
    Buff[30]=ETX;
	return DataLength;
}

bool CACUInterface::isAck(const BYTE *Buff,BYTE &Err)
{
	if (Buff[0]==ACK) {
		Err=0;
		return true;
	}
	else {
		Err=(BYTE)Buff[1];
		return false;
	}
}

CString CACUInterface::messageFromError(BYTE Err)
{
	if (Err==0x43) {
		return CString("Checksum error");
	}
	else if (Err==0x45) {
		return CString("ETX not received at expected position");
	}
	else if (Err==0x49) {
		return CString("Unknown identifier");
	}
	else if (Err==0x4c) {
		return CString("Wrong Length");
	}
	else if (Err==0x4d) {
		return CString("Command ignored in present operating mode");
	}
	else if (Err==0x53) {
		return CString("Missing or STX");
	}
	else if (Err==0x6c) {
		return CString("Specified length does not match identifier");
	}
	else if (Err==0x6f) {
		return CString("Other reasons");
	}
	else if (Err==0x72) {
		return CString("Value out of range");
	}
	else if (Err==0x73) {
		return CString("Unknown sender");
	}
	else return CString("Unknown error");
}

CString CACUInterface::messageFromMode(TAxeModes mode)
{
	if (mode==STANDBY) { return CString("Standby"); }
	else if (mode==STOP) { return CString("Stop"); }
	else if (mode==PRESET) { return CString("Preset"); }
	else if (mode==PROGRAMTRACK) { return CString("Program Track"); }
	else if (mode==RATE) { return CString("Rate"); }
	else if (mode==SECTORSCAN) { return CString("Sector Scan"); }
	else if (mode==STOW) { return CString("Stow Position"); }
	else if (mode==BORESIGHT) { return CString("Boresight"); }
	else if (mode==STARTRACK) { return CString("Star Track"); }
	else if (mode==POSITIONTRACK) { return CString("Position Track"); }
	else if (mode==UNSTOW) { return CString("Unstow"); }
	else { return CString("Unknown"); }		  
}

CACUInterface::TAxeModes CACUInterface::getMode(BYTE mode)
{
	if (mode==STANDBY) return STANDBY;
	else if (mode==PRESET) return PRESET;
	else if (mode==STOP) return STOP;
	else if (mode==PROGRAMTRACK) return PROGRAMTRACK;
	else if (mode==RATE) return RATE;
	else if (mode==SECTORSCAN) return SECTORSCAN;
	else if (mode==STOW) return STOW;
	else if (mode==BORESIGHT) return BORESIGHT;
	else if (mode==STARTRACK) return STARTRACK;
	else if (mode==POSITIONTRACK) return POSITIONTRACK;
	else if (mode==UNSTOW) return UNSTOW;	
	else return UNKNOWN;	
}

CACUInterface::TAntennaSection CACUInterface::getSection(WORD controlWord)
{
	BYTE tmp=0;
	// extract the LS bit 
	tmp=controlWord&0x01;
	if (tmp==1) return CACUInterface::CW;
	else return CACUInterface::CCW;
}

