// $Id: Lecom.cpp,v 1.1.1.1 2007-05-03 15:15:48 a.orlati Exp $

#include <stdlib.h>
#include <stdio.h>
#include <new>
#include <map>
#include "IRA"

#define STX 0x02
#define ETX 0x03
#define EOT 0x04
#define ENQ 0x05
#define ACK 0x06
#define NAK 0x15
#define EXTENDEDFLAG 0x21

#define BUFFERLEN 256

using namespace IRA;

typedef struct {
	WORD Reference;
	CLecom *Object;
	CString PortName;
} THashTableElement;

typedef std::map<CString,THashTableElement*> THashTable;

static THashTable PortsTable;

CLecom::CLecom(CString Port,CError& Err) : CSerialPort(Port), m_ucBuffer(NULL)
{
	try {
		m_ucBuffer=(BYTE *)new BYTE[BUFFERLEN];
	}
	catch (std::bad_alloc& ex) {
		_SET_ERROR(Err,CError::SerialType,CError::MemoryError,"CLecom::CLecom()");
		return;
	}
	if (!m_ucBuffer) {
		_SET_ERROR(Err,CError::SerialType,CError::MemoryError,"CLecom::CLecom()");
		return;
	}
}

CLecom *CLecom::Create(CError& Err,CString Port,BaudRates Rate,DataBits DataB,Parities Par,StopBits Stop,Handshakes HandS,int Timeout)
{
	THashTableElement *El;
	if (!Err.isNoError()) return NULL;
	THashTable::iterator ff=PortsTable.find(Port);
	if (ff!=PortsTable.end()) { // Port already exists
		El=ff->second;
		El->Reference++;
		return El->Object;
	}
	else {
		try {
			El=(THashTableElement *)new THashTableElement;
			El->Object=(CLecom *)new CLecom(Port,Err);
			El->Object->OpenPort(Err,Rate,DataB,Par,Stop,HandS,Timeout);
		}
		catch (std::bad_alloc& ex) {
			_SET_ERROR(Err,CError::SerialType,CError::MemoryError,"CLecom::Create()");
			return NULL;
		}
		El->Reference=1;
		El->PortName=Port;
		PortsTable[El->Object->getPort()]=El;
		return El->Object;
	}
}

void CLecom::Destroy(CLecom *Object) 
{
	THashTableElement *El;
	THashTable::iterator ff=PortsTable.find(Object->getPort());
	if (ff!=PortsTable.end()) { //Port exists
		El=ff->second;
		if (El) {
			El->Reference--;
			if (El->Reference==0) { //Object must be destoryed
				if (El->Object) delete El->Object;
				PortsTable.erase(ff);
				delete El;
			}
		}	
	}
}

CLecom::~CLecom()
{
	if (m_ucBuffer) delete []m_ucBuffer;
}

void CLecom::ReceiveCommand(BYTE Addr,WORD Code,CError& Err)
{
	if (!Err.isNoError()) return;
	baci::ThreadSyncGuard guard(&m_SerialMutex);
	m_ucBuffer[0]=EOT;
	Address(Addr,m_ucBuffer[1],m_ucBuffer[2]);
	CodeNumber(Code,m_ucBuffer[3],m_ucBuffer[4]);
	m_ucBuffer[5]=ENQ;
	Tx(m_ucBuffer,6,Err);
}

void CLecom::ReceiveCommand(BYTE Addr,WORD Code,BYTE SubCode,CError& Err)
{
	if (!Err.isNoError()) return;
	baci::ThreadSyncGuard guard(&m_SerialMutex);
	m_ucBuffer[0]=EOT;
	Address(Addr,m_ucBuffer[1],m_ucBuffer[2]);
	m_ucBuffer[3]=EXTENDEDFLAG;
	//convert code in Hex rapresentation.....
	sprintf((char *)(m_ucBuffer+4),"%04X",Code);
	sprintf((char *)(m_ucBuffer+8),"%02X",SubCode);
	m_ucBuffer[10]=ENQ;
	Tx(m_ucBuffer,11,Err);
}

void CLecom::SendCommand(BYTE Addr,WORD Code,int Val,CError& Err)
{
	if (!Err.isNoError()) return;
	baci::ThreadSyncGuard guard(&m_SerialMutex);
	CString App("");
	App.Concat(Val);
	WORD Len=Pack(m_ucBuffer,Addr,Code,App);
	Tx(m_ucBuffer,Len,Err);
}

void CLecom::SendCommand(BYTE Addr,WORD Code,double Val,CError& Err)
{
	if (!Err.isNoError()) return;
	baci::ThreadSyncGuard guard(&m_SerialMutex);
	CString App("");
	App.Concat(Val);
	WORD Len=Pack(m_ucBuffer,Addr,Code,App);
	Tx(m_ucBuffer,Len,Err);
}

void CLecom::SendCommand(BYTE Addr,WORD Code,BYTE SubCode,double Val,CError& Err)
{
	if (!Err.isNoError()) return;
	baci::ThreadSyncGuard guard(&m_SerialMutex);
	CString App("");
	App.Concat(Val,4);
	WORD Len=Pack(m_ucBuffer,Addr,Code,SubCode,App);
	Tx(m_ucBuffer,Len,Err);
}

void CLecom::SendCommand(BYTE Addr,WORD Code,BYTE SubCode,int Val,CError& Err)
{
	if (!Err.isNoError()) return;
	baci::ThreadSyncGuard guard(&m_SerialMutex);
	CString App("");
	App.Concat(Val);
	WORD Len=Pack(m_ucBuffer,Addr,Code,SubCode,App);
	Tx(m_ucBuffer,Len,Err);
}

bool CLecom::Answer(CError& Err)
{
	if (!Err.isNoError()) return false;
	baci::ThreadSyncGuard guard(&m_SerialMutex);
	BYTE Ans;
	if ((Rx(&Ans,1,Err))>0) {
		if (!Err.isNoError()) {
			return false;
		}
		if (Ans==ACK) {
			return true;
		}
		else if (Ans==NAK) {
			return false;
		}
		else {
			_SET_ERROR(Err,CError::SerialType,CError::BadAnswer,"CLecom::Answer()");
			return false;
		}
	}
	else {
		_SET_ERROR(Err,CError::SerialType,CError::BadAnswer,"CLecom::Answer()");
		return false;
	}
}

double CLecom::Answer(WORD Code,CError& Err)
{
	if (!Err.isNoError()) return 0.0;
	baci::ThreadSyncGuard guard(&m_SerialMutex);
	BYTE C1,C2;
	WORD BytesRead=0;
	WORD DataLen=0;
	bool Ok=false;
	bool Sync=false;
	double Val;
	CodeNumber(Code,C1,C2);
	while (!Ok) {
		if ((Rx(&m_ucBuffer[BytesRead],1,Err))>0) {
			if (!Err.isNoError()) {
				return 0.0;
			}
			BytesRead++;
			if ((BytesRead==1) && m_ucBuffer[0]==STX) {
				Sync=true;
				continue;
			}
			else if (!Sync) {
				continue;
			}
			if (DataLen>0) {
				if (m_ucBuffer[BytesRead-1]!=CheckNumber(m_ucBuffer+1,DataLen+3)) {
					_SET_ERROR(Err,CError::SerialType,CError::BadAnswer,"CLecom::Answer()");
					return 0.0;
				}
				else {
					Ok=true;
				}
			}
			else if ( (BytesRead==3) && ((m_ucBuffer[1]!=C1) || (m_ucBuffer[2]!=C2)) ) {
				Sync=false;
				BytesRead=DataLen=0;
			}
			else if ((BytesRead==4) && (m_ucBuffer[3]=='?')) {
				_SET_ERROR(Err,CError::SerialType,CError::ParityError,"CLecom::Answer()");
				return 0;
			}
			else if ((BytesRead==4) && (m_ucBuffer[3]==EOT)) {
				_SET_ERROR(Err,CError::SerialType,CError::BadParameter,"CLecom::Answer()");
				return 0.0;
			}
			else if ((BytesRead>4) && (m_ucBuffer[BytesRead-1]==ETX)) {
				DataLen=BytesRead-4;
			}
		}
		else {
			_SET_ERROR(Err,CError::SerialType,CError::BadAnswer,"CLecom::Answer()");
			return 0.0;
		}
	}
	if (!UnpackValue(m_ucBuffer+3,DataLen,Val)) {
		_SET_ERROR(Err,CError::SerialType,CError::BadAnswer,"CLecom::Answer()");
		return 0.0;
	}
	else return Val;
}

double CLecom::Answer(WORD Code,BYTE SubCode,CError& Err)
{
	if (!Err.isNoError()) return 0.0;
	baci::ThreadSyncGuard guard(&m_SerialMutex);
	BYTE C[6];
	WORD BytesRead=0;
	WORD DataLen=0;
	bool Ok=false;
	bool Sync=false;
	double Val;
	sprintf((char *)(C),"%04X",Code);
	sprintf((char *)(C+4),"%02X",SubCode);
	while (!Ok) {
		if ((Rx(&m_ucBuffer[BytesRead],1,Err))>0) {
			if (!Err.isNoError()) {
				return 0.0;
			}
			BytesRead++;
			if ((BytesRead==1) && m_ucBuffer[0]==STX) {
				Sync=true;
				continue;
			}
			else if (!Sync) {
				continue;
			}
			if (DataLen>0) {
				if (m_ucBuffer[BytesRead-1]!=CheckNumber(m_ucBuffer+1,DataLen+8)) {
					_SET_ERROR(Err,CError::SerialType,CError::BadAnswer,"CLecom::Answer()");
					return 0.0;
				}
				else {
					Ok=true;
				}
			}
			else if ((BytesRead==2) && (m_ucBuffer[1]!=EXTENDEDFLAG)) {
				Sync=false;
				BytesRead=DataLen=0;
			}
			else if ((BytesRead==3) && (m_ucBuffer[2]!=C[0])) {
				Sync=false;
				BytesRead=DataLen=0;
			}
			else if ((BytesRead==4) && (m_ucBuffer[3]!=C[1])) {
				Sync=false;
				BytesRead=DataLen=0;
			}
			else if ( (BytesRead==5) && (m_ucBuffer[4]!=C[2])) {
				Sync=false;
				BytesRead=DataLen=0;
			}
			else if ( (BytesRead==6) && (m_ucBuffer[5]!=C[3])) {
				Sync=false;
				BytesRead=DataLen=0;
			}
			else if ( (BytesRead==7) && (m_ucBuffer[6]!=C[4])) {
				Sync=false;
				BytesRead=DataLen=0;
			}
			else if ( (BytesRead==8) && (m_ucBuffer[7]!=C[5])) {
				Sync=false;
				BytesRead=DataLen=0;
			}
			else if ((BytesRead==9) && (m_ucBuffer[8]=='?')) {
				_SET_ERROR(Err,CError::SerialType,CError::ParityError,"CLecom::Answer()");
				return 0.0;
			}
			else if ((BytesRead==9) && (m_ucBuffer[8]==EOT)) {
				_SET_ERROR(Err,CError::SerialType,CError::BadParameter,"CLecom::Answer()");
				return 0.0;
			}
			else if ((BytesRead>9) && (m_ucBuffer[BytesRead-1]==ETX)) {
				DataLen=BytesRead-9;
			}
		}
		else {
			_SET_ERROR(Err,CError::SerialType,CError::BadAnswer,"CLecom::Answer()");
			return 0.0;
		}
	}
	if (!UnpackValue(m_ucBuffer+8,DataLen,Val)) {
			_SET_ERROR(Err,CError::SerialType,CError::BadAnswer,"CLecom::Answer()");
			return 0.0;
	}
	else return Val;
}

bool CLecom::Set(BYTE Addr,WORD Code,double Val,CError& Err)
{
	if (!Err.isNoError()) return false;
	// This will work (no deadlock when calling Sendcommand) becuase the Mutex is recursive.
	// In other words the thread owner of the mutex can lock it several times.
	bool Ans;
	baci::ThreadSyncGuard guard(&m_SerialMutex);
	SendCommand(Addr,Code,Val,Err);
	// done to avoid the guard go out of scope and release the mutex!!!
	Ans=Answer(Err);
	return Ans;
}

bool CLecom::Set(BYTE Addr,WORD Code,int Val,CError& Err)
{
	bool Ans;
	if (!Err.isNoError()) return false;
	baci::ThreadSyncGuard guard(&m_SerialMutex);
	SendCommand(Addr,Code,Val,Err);
	Ans=Answer(Err);
	return Ans;
}

bool CLecom::Set(BYTE Addr,WORD Code,BYTE SubCode,double Val,CError& Err)
{
	bool Ans;
	if (!Err.isNoError()) return false;
	baci::ThreadSyncGuard guard(&m_SerialMutex);
	SendCommand(Addr,Code,SubCode,Val,Err);
	Ans=Answer(Err);
	return Ans;
}

bool CLecom::Set(BYTE Addr,WORD Code,BYTE SubCode,int Val,CError& Err)
{
	bool Ans;
	if (!Err.isNoError()) return false;
	baci::ThreadSyncGuard guard(&m_SerialMutex);
	SendCommand(Addr,Code,SubCode,Val,Err);
	Ans=Answer(Err);
	return Ans;
}

double CLecom::Get(BYTE Addr,WORD Code,CError& Err)
{
	double Val;
	if (!Err.isNoError()) return 0.0;
	baci::ThreadSyncGuard guard(&m_SerialMutex);
	ReceiveCommand(Addr,Code,Err);
	Val=Answer(Code,Err);
	return Val;
}

double CLecom::Get(BYTE Addr,WORD Code,BYTE SubCode,CError& Err)
{
	double Val;
	if (!Err.isNoError()) return 0.0;
	baci::ThreadSyncGuard guard(&m_SerialMutex);
	ReceiveCommand(Addr,Code,SubCode,Err);
	Val=Answer(Code,SubCode,Err);
	return Val;
}

WORD CLecom::Pack(BYTE *Buff,BYTE Addr,WORD Code,CString Val)
{
	int ValLen;
	Buff[0]=EOT;
	Address(Addr,Buff[1],Buff[2]);
	Buff[3]=STX;
	CodeNumber(Code,Buff[4],Buff[5]);
	ValLen=Val.GetLength();
	for (int i=0;i<ValLen;i++) {
		Buff[6+i]=Val[i];
	}
	Buff[6+ValLen]=ETX;
	Buff[7+ValLen]=CheckNumber(Buff+4,ValLen+3);
	return ValLen+8;
}

WORD CLecom::Pack(BYTE *Buff,BYTE Addr,WORD Code,BYTE SubCode,CString Val)
{
	int ValLen;
	Buff[0]=EOT;
	Address(Addr,Buff[1],Buff[2]);
	Buff[3]=STX;
	Buff[4]=EXTENDEDFLAG;
	sprintf((char *)(m_ucBuffer+5),"%04X",Code);
	sprintf((char *)(m_ucBuffer+9),"%02X",SubCode);
	ValLen=Val.GetLength();
	for (int i=0;i<ValLen;i++) {
		Buff[11+i]=Val[i];
	}
	Buff[11+ValLen]=ETX;
	Buff[12+ValLen]=CheckNumber(Buff+4,ValLen+8);
	return ValLen+13;
}

bool CLecom::UnpackValue(BYTE *Buff,WORD Len,double &Val)
{
	BYTE App[BUFFERLEN];
	WORD i;
	switch (Buff[0]) {
		case 'H' : {
			int AppVal;
			for(i=0;i<Len-1;App[i]=Buff[i+1],i++);
			App[i]=0;
			if (StrToInteger(App,16,AppVal)) {
				Val=(double)AppVal;
				return true;
			}
			else return false;
			break;
		}
		case 'S' : {
			//not supported yet.
			return false;
			break;
		}
		case 'O' : {
			int AppVal;
			for(i=0;i<Len-1;App[i]=Buff[i+1],i++);
			App[i]=0;
			if (StrToInteger(App,8,AppVal)) {
				Val=(double)AppVal;
				return true;
			}
			else return false;
			break;
		}
		default : {
			for(i=0;i<Len;App[i]=Buff[i],i++);
			App[i]=0;
			if (StrToDouble(App,Val)) return true;
			else return false;
		}
	}
}

void CLecom::CodeNumber(const WORD Code,BYTE &C1,BYTE &C2)
{
	WORD div,mod;
	div=Code/790;
	mod=Code%790;
	C1=(BYTE)int(mod/10)+48;
	C2=(BYTE)(mod%10+div*10+48);
}

void CLecom::Address(const BYTE Addr,BYTE &A1,BYTE &A2)
{
	if (Addr==BROADCAST) {
		A1=A2='0';
	}
	else {
		A1=Addr/10+0x30;
		A2=Addr%10+0x30;
	}
}

BYTE CLecom::CheckNumber(BYTE *Buff,WORD Len)
{
	BYTE App=0;
	for(unsigned i=0;i<Len;i++,App^=*Buff++);
	return App;
}

bool CLecom::StrToInteger(const BYTE *Buff,int Base,int &Val)
{
	char *End;
	Val=strtol((const char *)Buff,&End,Base);
	if ((Val==0) && (End==(char *)Buff)) return false;
	else return true;
}

bool CLecom::StrToDouble(const BYTE *Buff,double &Val)
{
	char *End;
	Val=strtod((const char *)Buff,&End);
	if ((Val==0.0) && (End==(char *)Buff)) return false;
	else return true;
}
