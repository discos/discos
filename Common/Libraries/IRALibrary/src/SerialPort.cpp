// $Id: SerialPort.cpp,v 1.1.1.1 2007-05-03 15:15:48 a.orlati Exp $

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <new>
#include "IRA"

using namespace IRA;

CSerialPort::CSerialPort(CString Port) : m_sPortName(Port)
{
	m_bOpened=false;
}

CSerialPort::~CSerialPort()
{
	// try to recover old configuration.....
	ClosePort();
}

void CSerialPort::OpenPort(CError& Err,BaudRates Rate,DataBits DataB,Parities Par,StopBits Stop,Handshakes HandS,int Timeout)
{
	struct termios newtio,chktio;
	if (!Err.isNoError()) return;
	m_iPortD=open((const char *)m_sPortName,O_RDWR|O_NOCTTY);
	if (m_iPortD<0) {
		_SET_SYSTEM_ERROR(Err,CError::SerialType,CError::HardwareFail,"CSerial::OpenPort()",errno);
		return;
	}
	if (tcflush(m_iPortD,TCIOFLUSH)<0) {
		_SET_SYSTEM_ERROR(Err,CError::SerialType,CError::HardwareFail,"CSerial::OpenPort()",errno);
		return;
	}
	m_bOpened=true;
	m_stPolling.fd=m_iPortD;
	m_stPolling.events=POLLIN|POLLOUT;
	m_stPolling.revents=0;
	//save current configuration
	tcgetattr(m_iPortD,&m_stOldConfig);
	bzero(&newtio,sizeof(newtio));
	newtio.c_cc[VTIME]=Timeout/100;
	newtio.c_cc[VMIN]=0;
	newtio.c_iflag=IGNBRK|IGNPAR;
	newtio.c_cflag=CLOCAL|CREAD;
	if (HandS==XONXOFF) {
		newtio.c_iflag|=IXON|IXOFF;
	}
	else if (HandS==HARDWARE) {
		newtio.c_cflag|=CRTSCTS;
	}
	else if (HandS==NONE) {
		// do nothing......
	}
	switch (Rate) {
		case BR50 : {
			newtio.c_cflag|=B50;
			break;
		}
		case BR75 : {
			newtio.c_cflag|=B75;
			break;
		}
		case BR110 : {
			newtio.c_cflag|=B110;
			break;
		}
		case BR134 : {
			newtio.c_cflag|=B134;
			break;
		}
		case BR150 : {
			newtio.c_cflag|=B150;
			break;
		}
		case BR200 : {
			newtio.c_cflag|=B200;
			break;
		}
		case BR300 : {
			newtio.c_cflag|=B300;
			break;
		}
		case BR600 : {
			newtio.c_cflag|=B600;
			break;
		}
		case BR1200 : {
			newtio.c_cflag|=B1200;
			break;
		}
		case BR2400 : {
			newtio.c_cflag|=B2400;
			break;
		}
		case BR4800 : {
			newtio.c_cflag|=B4800;
			break;
		}
		case BR9600 : {
			newtio.c_cflag|=B9600;
			break;
		}
		case BR19200 : {
			newtio.c_cflag|=B19200;
			break;
		}
		case BR38400 : {
			newtio.c_cflag|=B38400;
			break;
		}
		case BR57600 : {
			newtio.c_cflag|=B57600;
			break;
		}
		case BR115200 : {
			newtio.c_cflag|=B115200;
			break;
		}
	}
	switch (DataB) {
		case D5 : {
			newtio.c_cflag|=CS5;
			break;
		}
		case D6 : {
			newtio.c_cflag|=CS6;
			break;
		}
		case D7 : {
			newtio.c_cflag|=CS7;
			break;
		}
		case D8 : {
			newtio.c_cflag|=CS8;
			break;
		}
	}
	if (Par==NOPARITY) {
		newtio.c_cflag&= ~PARENB;
	}
	else if (Par==EVEN) {
		newtio.c_cflag|=PARENB;
		newtio.c_cflag&= ~PARODD;
	}
	else if (Par==ODD) {
		newtio.c_cflag|=PARENB;
		newtio.c_cflag|=PARODD;
	}
	if (Stop==STOP1) {
		newtio.c_cflag&= ~CSTOPB;
	}
	else if (Stop==STOP2) {
		newtio.c_cflag|=CSTOPB;
	}
	if (tcsetattr(m_iPortD,TCSANOW,&newtio)<0) {
		_SET_SYSTEM_ERROR(Err,CError::SerialType,CError::HardwareFail,"CSerial::OpenPort()",errno);
		return;
	}
	tcgetattr(m_iPortD,&chktio);
	if (!memcmp((void *)&newtio,(void *)&chktio,sizeof(newtio))) {
		_SET_ERROR(Err,CError::SerialType,CError::SrlPortConfig,"CSerial::OpenPort()");
		return;
	}
	m_Rate=Rate;m_DataB=DataB,m_Par=Par,m_Stop=Stop,m_HandS=HandS,m_Timeout=Timeout;
}

void CSerialPort::ClosePort()
{
	if (m_bOpened) {
		tcsetattr(m_iPortD,TCSANOW,&m_stOldConfig);
		close(m_iPortD);
		m_bOpened=false;
	}
}

bool CSerialPort::isRxReady(CError& Err)
{
	if (!Err.isNoError()) return false;
	if (m_bOpened) {
		int Res=poll(&m_stPolling,1,1);
		if ((Res>0) && ((m_stPolling.revents&POLLIN)==POLLIN)) {
			m_stPolling.revents=0;
			return true;
		}
		else {
			m_stPolling.revents=0;
			return false;
		}
	}
	else {
		_SET_ERROR(Err,CError::SerialType,CError::SrlPortNOpened,"CSerial::isRxReady()");
		return false;
	}
}

bool CSerialPort::isTxReady(CError& Err)
{
		if (!Err.isNoError()) return false;
	if (m_bOpened) {
		int Res=poll(&m_stPolling,1,1);
		if ((Res>0) && ((m_stPolling.revents&POLLOUT)==POLLOUT)) {
			m_stPolling.revents=0;
			return true;
		}
		else {
			m_stPolling.revents=0;
			return false;
		}
	}
	else {
		_SET_ERROR(Err,CError::SerialType,CError::SrlPortNOpened,"CSerial::isTxReady()");
		return false;
	}
}

int CSerialPort::Rx(BYTE *Buff,WORD ReadBytes,CError& Err)
{
	if (!Err.isNoError()) return 0;
	if (!m_bOpened) {
		_SET_ERROR(Err,CError::SerialType,CError::SrlPortNOpened,"CSerial::Rx()");
		return 0;
	}
	int Res=read(m_iPortD,(void *)Buff,(size_t)ReadBytes);
	if (!Res) {
		_SET_ERROR(Err,CError::SerialType,CError::ReadTimeout,"CSerial::Rx()");
		return 0;
	}
	else if (Res<0) {
		_SET_SYSTEM_ERROR(Err,CError::SerialType,CError::HardwareFail,"CSerial::Rx()",errno);
		return 0;
	}
	else {
		return Res;
	}
}

int CSerialPort::Tx(const BYTE *Buff,WORD WriteBytes,CError& Err)
{
	if (!Err.isNoError()) return 0;
	if (!m_bOpened) {
		_SET_ERROR(Err,CError::SerialType,CError::SrlPortNOpened,"CSerial::Tx()");
		return 0;
	}
	int Res=write(m_iPortD,(const void *)Buff,(size_t)WriteBytes);
	if (Res<0) {
		_SET_SYSTEM_ERROR(Err,CError::SerialType,CError::HardwareFail,"CSerial::Tx()",errno);
		return 0;
	}
	else if ((WORD)Res<WriteBytes) {
		_SET_ERROR(Err,CError::SerialType,CError::WriteTimeout,"CSerial::Tx()");
		return 0;
	}
	else {
		return Res;
	}
}
