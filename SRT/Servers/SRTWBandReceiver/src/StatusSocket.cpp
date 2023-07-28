// $Id: StatusSocket.cpp,v 1.5 2023-07-04 14:04:49 lorenzo.monti Exp $

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

void CStatusSocket::init(CConfiguration *config,CSecureArea<CWBandCommand> *data) throw (SocketErrorExImpl)
{
	m_pConfiguration=config;
	m_pData=data;
	CSecAreaResourceWrapper<CWBandCommand> commonData=m_pData->Get();
	connectSocket(m_pConfiguration->WLOAddress(), m_pConfiguration->WLOPort()); // Local Oscillator
	//connectSocket(m_pConfiguration->SwMatrixAddress(), m_pConfiguration->SwMatrixPort()); // Switch Matrix
	//connectSocket(m_pConfiguration->WCALddress(), m_pConfiguration->WCALPort()); // Solar Attenuator
}

void CStatusSocket::cleanUp(){}

void CStatusSocket::onReceive(DWORD Counter,bool &Boost)
{
	BYTE buffer[1000];
	IRA::CError err;
	int len;
	int res=receiveBuffer(buffer,1000,err);
	ACS_LOG(LM_FULL_INFO,"CStatusSocket::onReceive()",(LM_CRITICAL,"STATUS_SOCKET_OK -> res>0"));

	if (res>0) {
		ACS_LOG(LM_FULL_INFO,"CStatusSocket::onReceive()",(LM_CRITICAL,"STATUS_SOCKET_OK -> res>0"));
		if (!m_btransferStarted) m_btransferStarted=true;
		m_wtimeOuts=0;
		cout << "CStatusSocket::onReceive() " << res << endl;

		if (len<0) {
			// error of status message
			ACS_LOG(LM_FULL_INFO,"CStatusSocket::onReceive()",(LM_WARNING,"STATUS_MESSAGE_SYNC_ERROR"));
		}

	}
	else if (res==0) { // disconnected
		ACS_LOG(LM_FULL_INFO,"CStatusSocket::onReceive()",(LM_CRITICAL,"STATUS_SOCKET_DISCONNECTED"));
	}
	else if (res<0) { // reading error
		CString app;
		app.Format("SOCKET_ERROR - %s",(const char *)err.getFullDescription());
		_IRA_LOGFILTER_LOG(LM_CRITICAL,"CStatusSocket::onReceive()",(const char*)app);// this could be very frequent!!!!...so Log filter is used
	}
}

void CStatusSocket::onConnect(int ErrorCode)
{
	CError Tmp;
		if (ErrorCode==0) {
			if (EventSelect(Tmp,E_CONNECT,false)==SUCCESS) { // disable the connect event......
				ACS_LOG(LM_FULL_INFO,"CStatusSocket::onConnect()",(LM_NOTICE,"STATUS_SOCKET_CONNECTED"));			
			}
		}
		else {
			ACS_DEBUG_PARAM("CStatusSocket::onConnect()","Reconnection failed, exit code is %d",ErrorCode);
		}
}

void CStatusSocket::onTimeout(WORD EventMask)
{
	CSecAreaResourceWrapper<CWBandCommand> data=m_pData->Get();
	if ((EventMask&CSocket::E_RECEIVE)==CSocket::E_RECEIVE) {
		if (m_btransferStarted) {  //this will filter out the event that the ACU is late in transferring the first data packet when the connection is enstablished
			m_wtimeOuts++;
		}
		if (m_wtimeOuts>=7) {
			ACS_LOG(LM_FULL_INFO,"CStatusSocket::onTimeout()",(LM_CRITICAL,"STATUS_SOCKET_DISCONNECTED_ON_TIMEOUT"));
		}
	}
}

void CStatusSocket::connectSocket(IRA::CString IPAddr,WORD Port) throw (SocketErrorExImpl)
{
	IRA::CError error;	
	CSecAreaResourceWrapper<CWBandCommand> data=m_pData->Get();
	cout << "Ip address: " << IPAddr << " Port: " << Port << endl;
	ACS_LOG(LM_FULL_INFO,"CStatusSocket::connectSocket()",(LM_CRITICAL,"Entrato"));
		ACS_LOG(LM_FULL_INFO,"CStatusSocket::connectSocket()",(LM_NOTICE,"STATUS_SOCKET_CONNECTING"));
		createSocket(); // throw (SocketErrorExImpl)
		CSocket::OperationResult res=Connect(error,IPAddr, Port);
		if (res==FAIL) {
			ACS_LOG(LM_FULL_INFO,"CStatusSocket::connectSocket()",(LM_CRITICAL,"FAIL"));
			_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
			dummy.setCode(error.getErrorCode());
			dummy.setDescription((const char*)error.getDescription());
			//data->setStatusLineState(Antenna::ACU_NOTCNTD);
			_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CStatusSocket::connectSocket()");
		}	
		else if (res==WOULDBLOCK) {
			ACS_LOG(LM_FULL_INFO,"CStatusSocket::connectSocket()",(LM_CRITICAL,"WOULDBLOCK"));
		}
		else {
			ACS_LOG(LM_FULL_INFO,"CStatusSocket::connectSocket()",(LM_CRITICAL,"NOT WOULDBLOCK"));
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
	int Val=1000;
	if (setSockOption(error,SO_RCVBUF, &Val, sizeof(int))==FAIL) {
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
	// register the event associated to the ready-to-receive event
	/*if (EventSelect(error,E_RECEIVE,true,m_pConfiguration->statusSocketTimeout())==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CStatusSocket::createSocket()");
	}*/
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


