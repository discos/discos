// $Id: StatusSocket.cpp,v 1.5 2011-06-03 18:02:49 a.orlati Exp $

#include "StatusSocket.h"

using namespace IRA;
using namespace ComponentErrors;


_IRA_LOGFILTER_IMPORT;

CStatusSocket::CStatusSocket()
{
	m_pConfiguration=NULL;
	m_pData=NULL;
	m_wtimeOuts=0;
	m_btransferStarted=false;
	m_customBind=NULL;
}

CStatusSocket::~CStatusSocket()
{	
}

void CStatusSocket::init(CConfiguration *config,CSecureArea<CCommonData> *data) throw (SocketErrorExImpl)
{
	m_pConfiguration=config;
	m_pData=data;
	CSecAreaResourceWrapper<CCommonData> commonData=m_pData->Get();
	commonData->setStatusLineState(Antenna::ACU_NOTCNTD);
	connectSocket(); // throw (SocketErrorExImpl)
}

void CStatusSocket::cleanUp()
{
	CSecAreaResourceWrapper<CCommonData> commonData=m_pData->Get();
	commonData->setStatusLineState(Antenna::ACU_NOTCNTD);	
}

void CStatusSocket::onReceive(DWORD Counter,bool &Boost)
{
	BYTE buffer[CACUProtocol::SOCKET_RECV_BUFFER];
	IRA::CError err;
	int len;
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();	
	if ((data->getStatusLineState()==Antenna::ACU_NOTCNTD) || (data->getStatusLineState()==Antenna::ACU_CNTDING)) return;
	int res=receiveBuffer(buffer,CACUProtocol::SOCKET_RECV_BUFFER,err);
	if (res>0) {
		data->clearStatusWord(CCommonData::STATUS_LINE_ERROR);
		if (!m_btransferStarted) m_btransferStarted=true;
		m_wtimeOuts=0;
		if ((len=m_protocol.syncBuffer(buffer,res,data->getStatusBuffer()))>0) {
			data->clearStatusWord(CCommonData::STATUS_MESSAGE_SYNC_ERROR);
			//Boost=true;
			if (m_customBind==NULL) {
				data->reBind();
			}
			else {
				m_customBind((const CCommonData *)data);
			}
		}
		else if (len<0) {
			// error of status message frame synchronization
			data->setStatusWord(CCommonData::STATUS_MESSAGE_SYNC_ERROR);
			ACS_LOG(LM_FULL_INFO,"CStatusSocket::onReceive()",(LM_WARNING,"STATUS_MESSAGE_SYNC_ERROR"));
		}
		else {
			// do nothing the buffer is still not complete so wait for the remaining data.......
		}
	}
	else if (res==0) { // disconnected
		data->setStatusLineState(Antenna::ACU_NOTCNTD);
		ACS_LOG(LM_FULL_INFO,"CStatusSocket::onReceive()",(LM_CRITICAL,"STATUS_SOCKET_DISCONNECTED"));
	}
	else if (res<0) { // reading error
		CString app;
		app.Format("SOCKET_ERROR - %s",(const char *)err.getFullDescription());
		_IRA_LOGFILTER_LOG(LM_CRITICAL,"CStatusSocket::onReceive()",(const char*)app);// this could be very frequent!!!!...so Log filter is used
		data->setStatusWord(CCommonData::STATUS_LINE_ERROR);
	}
}

void CStatusSocket::onConnect(int ErrorCode)
{
	CError Tmp;
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	if (data->getStatusLineState()==Antenna::ACU_CNTDING) {
		if (ErrorCode==0) {
			if (EventSelect(Tmp,E_CONNECT,false)==SUCCESS) { // disable the connect event......
				data->setStatusLineState(Antenna::ACU_CNTD); // set the socket as connected.....
				ACS_LOG(LM_FULL_INFO,"CStatusSocket::onConnect()",(LM_NOTICE,"STATUS_SOCKET_CONNECTED"));			
			}
		}
		else {
			data->setStatusLineState(Antenna::ACU_NOTCNTD); // in case of error declare the disconnection again in order to make another try.......
			ACS_DEBUG_PARAM("CStatusSocket::onConnect()","Reconnection failed, exit code is %d",ErrorCode);
		}
	}
}

void CStatusSocket::onTimeout(WORD EventMask)
{
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	if ((EventMask&CSocket::E_RECEIVE)==CSocket::E_RECEIVE) {
		if (m_btransferStarted) {  //this will filter out the event that the ACU is late in transferring the first data packet when the connection is enstablished
			m_wtimeOuts++;
		}
		if (m_wtimeOuts>=7) {
			data->setStatusLineState(Antenna::ACU_NOTCNTD); /// declare the socket not connected so that the reconnection procedure can take place......	
			ACS_LOG(LM_FULL_INFO,"CStatusSocket::onTimeout()",(LM_CRITICAL,"STATUS_SOCKET_DISCONNECTED_ON_TIMEOUT"));
		}
	}
}

void CStatusSocket::connectSocket() throw (SocketErrorExImpl)
{
	IRA::CError error;	
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	if (data->getStatusLineState()==Antenna::ACU_NOTCNTD) {
		ACS_LOG(LM_FULL_INFO,"CStatusSocket::connectSocket()",(LM_NOTICE,"STATUS_SOCKET_CONNECTING"));
		createSocket(); // throw (SocketErrorExImpl)
		CSocket::OperationResult res=Connect(error,m_pConfiguration->ipAddress(),m_pConfiguration->statusPort());
		if (res==FAIL) {
			_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
			dummy.setCode(error.getErrorCode());
			dummy.setDescription((const char*)error.getDescription());
			data->setStatusLineState(Antenna::ACU_NOTCNTD);
			_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CStatusSocket::connectSocket()");
		}	
		else if (res==WOULDBLOCK) {
			data->setStatusLineState(Antenna::ACU_CNTDING);
		}
		else {
			data->setStatusLineState(Antenna::ACU_CNTD);
		}
	}
}

void CStatusSocket::createSocket() throw (SocketErrorExImpl)
{
	IRA::CError error;
	// first of all close the socket.....just to be sure there is nothing pending
	Close(error);
	error.Reset();
	// this will create the socket in blocking mode.....
	if (Create(error,STREAM)==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CStatusSocket::createSocket()");
	}
	// set socket receive buffer!!!!
	int Val=CACUProtocol::SOCKET_RECV_BUFFER;
	if (setSockOption(error,SO_RCVBUF,&Val,sizeof(int))==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CStatusSocket::createSocket()");
	}
	// set socket in non-blocking mode.
	if (setSockMode(error,NONBLOCKING)==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CStatusSocket::createSocket()");
	}
	setSleepTime(m_pConfiguration->statusSocketDutyCycle());
	// register the event associated to the ready-to-receive event
	if (EventSelect(error,E_RECEIVE,true,m_pConfiguration->statusSocketTimeout())==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CStatusSocket::createSocket()");
	}
	// register the event associated to connection event...no timeout
	if (EventSelect(error,E_CONNECT,true,0)==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CStatusSocket::createSocket()");
	}
}

int CStatusSocket::receiveBuffer(BYTE *Msg,WORD Len,CError& err)
{
	int nRead;
	err.Reset();
	while(true) {
		nRead=Receive(err,(void *)Msg,Len);
		if (nRead==WOULDBLOCK) {
			continue;
		}
		else return nRead; // nRead=FAIL or nRead=0
	}
}


