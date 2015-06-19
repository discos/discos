#include "lanSocket.h"


lanSocket::lanSocket() : CSocket()
{
	m_soStat=NOTRDY;
	m_slanAddress="";
	m_wlanPort=0;
}

lanSocket::~lanSocket()
{
	Close(m_Error);
}

int lanSocket::Init(CString Address,WORD Port) 
{
	OperationResult err;
	m_slanAddress=Address;
	m_wlanPort=Port;
	// this will create the socket in blocking mode
	ACS_SHORT_LOG((LM_INFO,"::lanSocket::lanSocket;creating socket!"));
	
	if ((err=Create(m_Error,STREAM))==FAIL) {
		ACS_DEBUG("::lanSocket::lanSocket","Error creating socket");
		return err;
	}
	// the first time, perform a blocking connection....
	ACS_SHORT_LOG((LM_INFO,"::lanSocket::lanSocket;conneting socket..."));
	if ((err=Connect(m_Error,m_slanAddress,m_wlanPort))==FAIL) {
		ACS_DEBUG("::lanSocket::lanSocket","Error connecting socket");
		m_soStat=TOUT;
		Close(m_Error);
		return err;
	} else {
		ACS_SHORT_LOG((LM_INFO,"::lanSocket::lanSocket:Socket connected"));
		
		if((err=setSockMode(m_Error,BLOCKINGTIMEO,TIMEO,TIMEO))!=SUCCESS) {
			ACS_DEBUG("::lanSocket::lanSocket","Error configuring the socket");
			m_soStat=NOTRDY;
			Close(m_Error);
			return err;
		}
		
		int Val=SBUFF; //fixed size buffer of 0xFC,addr+nbytes,cmd ,param(max4 bytes), checksum
        if ((err=setSockOption(m_Error,SO_SNDBUF,&Val,sizeof(int))) != SUCCESS) {
			ACS_DEBUG("::lanSocket::lanSocket","Error configuring buffer size");
			m_soStat=NOTRDY;
			Close(m_Error);
			return err;
		}
		ACS_SHORT_LOG((LM_INFO,"::lanSocket::lanSocket: Socket succesfully configured"));
		return clearUSDBuffer();
	}
}

int lanSocket::sendCmd(BYTE cmd,BYTE addr,long par,BYTE nbytes)
{
	ACS_TRACE("::lanSocket::sendCmd()");
	int res=0,j,i;
	int nWrite=0,err=0;
	int bytesSent=0;
	BYTE resp;	
	WORD blen;
	int firstbyte;

	blen = 3 + nbytes + 1;	//0xFC,addr+nbytes,cmd ,param(max4 bytes), checksum
	firstbyte = 3;
	
	BYTE buff[]={0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	buff[1]=(nbytes==0)?0x20:((nbytes+1)<<5);
	buff[1]|=addr;
	buff[2]=cmd;
	
	for(i=firstbyte,j=nbytes-1;j>=0; j--,i++) buff[i]=*(((BYTE*)&par)+j);  // reverse the par byte order
	buff[blen-1]=checksum(buff,blen-1);
	
    while (bytesSent<blen) {
		if ((nWrite=Send(m_Error,(const void *)(buff+bytesSent),blen-bytesSent))<=0) {
			err=nWrite;
			break;
		} else { // success
			bytesSent+=nWrite;
		}
	}
 	
	if (bytesSent!=blen) {     // socket problem
		//_SET_ERROR(m_Error,CError::SocketType,CError::SendError,"lanSocket::SendCmd()");
		ACS_DEBUG("::lanSocket::sendCmd()","socket problem, not all bytes sent!");
		m_soStat=NOTRDY;
		
		if((res=reConnect(err)))      // try to reconnect
			ACS_DEBUG("::lanSocket::sendCmd()","error recreating the socket connection!");
		return res;
		 
	}	else {
	    ACS_DEBUG("::lanSocket::sendCmd()","all bytes sent!");
        //printf("Sent buffer:\n");
        //printBuffer(buff,blen);
	}

	res=receiveBuffer(&resp,1);
	
	if(res != 1) { // USD timeout or error
		ACS_DEBUG("::lanSocket::lanSocket","problems on usd reply!");
		return res;
	}
	if(resp != ACK && resp != NAK) { // bad response
		ACS_DEBUG_PARAM("::lanSocket::lanSocket","error getting ACK/NAK(%d)",resp);
		clearBuffer();        // syncronize the replies
		return -4;
	}

	return resp;
	
}	
	
int lanSocket::receiveBuffer(BYTE *msg,WORD len)
{
	int nRead,err=0;
	nRead=Receive(m_Error,(void *)msg,len);
	if (nRead <= 0 ) {     // socket or USD problems
		m_soStat=NOTRDY;
		if((err=reConnect(nRead)))      // try to reconnect
			ACS_DEBUG_PARAM("::lanSocket::receiveBuffer()","error %d reconnecting!",err);
		return err;
	} else if (nRead != len) {      // partial timeout
		 ACS_DEBUG("::lanSocket::receiveBuffer","socket problem, not all bytes received");
//		_SET_ERROR(m_Error,CError::SocketType,CError::SendError,"lanSocket::receiveBuffer()");
		return -4;
	} 
//	printBuffer(msg,len);
	
	return  nRead;
}

BYTE lanSocket::checksum(BYTE* buff,WORD len)
{
	int i;
	DWORD temp=0;
	BYTE checksum;
	
	for(i=0; i < len; i++) temp+=buff[i];       
	checksum=~(temp & 0xff);         //it must be the 1 complement
	return checksum;		
}

void lanSocket::printBuffer(BYTE *Buff,WORD Len)
{
	for(int i=0;i<Len;i++) printf("%02X ",Buff[i]);
	printf(" len:%d\n",Len);
}

int lanSocket::reConnect(int error)
{
	int  err=0;
	
	switch(error) {

		/* check the source of tout & clear the input buffer of USD's*/
		case WOULDBLOCK: {
			m_soStat=READY;        // wasn't a socket problem
			err=clearUSDBuffer();
			if(err==0) return -1; //it was a USD tout. USD's buffer cleared	
		}
		case FAIL: 	
		case 0: {	//socket remotely closed. Try to reconnect...
		    err=Init(m_slanAddress,m_wlanPort);
		}
	}
	return err;				
 }
// Protected Methods

void lanSocket::clearBuffer()
	{
		int res=0;
		BYTE buff[BUFFERSIZE];
		
        res=Receive(m_Error,(void *)buff,BUFFERSIZE-1);   //reads all the buffer
		m_soStat=lanSocket::READY;
	}

int lanSocket::clearUSDBuffer()
{
	BYTE buff[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};   //clear USD input buffer
	int err=0,len=8,written=0;
		 
	while (written<len) {
		 err=Send(m_Error,(const void *)buff,len);
		 if (err<0) break; 
		 else written+=err;
	}	

	if(err<0 || written!=len) {      
		m_soStat=NOTRDY;
		return err;	  	
	} 
	m_soStat=READY;
	return 0; //success clearing USD buffer
}
// private methods


