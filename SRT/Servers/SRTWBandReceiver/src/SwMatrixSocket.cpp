#include "SwMatrixSocket.h"
#include <LogFilter.h>
#include <TimeoutSync.h>
#include <WBandCommand.h>

using namespace IRA;
using namespace ComponentErrors;
using namespace AntennaErrors;

_IRA_LOGFILTER_IMPORT;

CSwMatrixSocket::CSwMatrixSocket()
{
	m_pConfiguration=NULL;
	m_bTimedout=false;

}

CSwMatrixSocket::~CSwMatrixSocket()
{
}

void CSwMatrixSocket::cleanUp()
{
	//before closing we have to make sure there is no one waiting for services of this object...so we acquire the mutex in blocking mode
	DDWORD timeo=0;
	IRA::CTimeoutSync guard(&m_syncMutex,timeo);
	guard.acquire();
	CError err;
	Close(err);
	guard.release();
}

void CSwMatrixSocket::init(CConfiguration *config) throw (SocketErrorExImpl)
{
	m_pConfiguration=config;	
	connectSocket(m_pConfiguration->SwMatrixAddress(), m_pConfiguration->SwMatrixPort()); // Switch Matrix
}

void CSwMatrixSocket::connectSocket(IRA::CString IPAddr,WORD Port) throw (SocketErrorExImpl) //IRA::CString IPAddr,WORD Port) throw (SocketErrorExImpl)
{
	IRA::CError error;

	createSocket(); // throw (SocketErrorExImpl)
	CSocket::OperationResult res=Connect(error, IPAddr, Port);

	if (res==FAIL) {
		ACS_LOG(LM_FULL_INFO,"CSLOSocket::connectSocket()",(LM_CRITICAL,"FAIL"));
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CSwMatrixSocket::connectSocket()");
	}	
	else if (res==WOULDBLOCK) {
		ACS_LOG(LM_FULL_INFO,"CSwMatrixSocket::connectSocket()",(LM_CRITICAL,"WOULDBLOCK"));
	}
	else {
		ACS_LOG(LM_FULL_INFO,"CSwMatrixSocket::connectSocket()",(LM_CRITICAL,"NOT WOULDBLOCK"));
	}
	
}

// ********************* PROTECTED  **************************************/

void CSwMatrixSocket::onConnect(int ErrorCode)
{
	CError Tmp;
		if (ErrorCode==0) {
			if (EventSelect(Tmp,E_CONNECT,false)==SUCCESS) { // disable the connect event......
				ACS_LOG(LM_FULL_INFO,"CSwMatrixSocket::onConnect()",(LM_NOTICE,"SWMATRIX_SOCKET_CONNECTED"));			
			}
		}
		else {
			ACS_DEBUG_PARAM("CSwMatrixSocket::onConnect()","Reconnection failed, exit code is %d",ErrorCode);
		}
	//}
}

// ********************* PRIVATE **************************************/

void CSwMatrixSocket::waitAck(const ACS::Time& commandTime) throw (AntennaErrors::NakExImpl,ComponentErrors::TimeoutExImpl)
{
	TIMEVALUE now;
	ACS::TimeInterval timeout=m_pConfiguration->getSocketResponseTime()*10;
	bool error=false;
	IRA::CIRATools::getTime(now);
	while (now.value().value<commandTime+timeout) {
		IRA::CIRATools::getTime(now); // get current time again
	}
	printf("LO Answer Error: timeout \n");
	_EXCPT(ComponentErrors::TimeoutExImpl,ex,"CSwMatrixSocket::waitAck()");
	throw ex;
}

bool CSwMatrixSocket::checkConnection()
{
	return 0; 
}

bool CSwMatrixSocket::checkIsBusy()
{
	return 0; //data->isBusy();	
}

void CSwMatrixSocket::createSocket() throw (SocketErrorExImpl)
{
	CError error;
	// first of all close the socket.....just to be sure there is nothing pending
	Close(error);
	error.Reset();
	// this will create the socket in blocking mode.....
	if (Create(error,STREAM)==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CSwMatrixSocket::createSocket()");
	}
	// set socket send buffer!!!!
	int Val=1024; //CACUProtocol::SOCKET_SEND_BUFFER;
	if (setSockOption(error,SO_SNDBUF,&Val,sizeof(int))==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CSwMatrixSocket::createSocket()");
	}
	// set socket in non-blocking mode.
	if (setSockMode(error,NONBLOCKING)==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CSwMatrixSocket::createSocket()");
	}
	if (EventSelect(error,E_CONNECT,true,0)==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CSwMatrixSocket::createSocket()");
	}	
}

void CSwMatrixSocket::sendCommand(std::string cmd, std::string outBuff, CConfiguration config) throw (TimeoutExImpl,SocketErrorExImpl)
{
	OperationResult res;
	CError err;
	
	if ((res=sendBuffer(cmd, err, config))==SUCCESS) {
		ACS_LOG(LM_FULL_INFO,"CSwMatrixSocket::sendCommand()",(LM_CRITICAL,"SUCCESS")); // logs it
		int bytes=receiveBuffer(outBuff, err);
		//return bytes;
	}
	else {  // send fails....m_Error already set by sendBuffer
		ACS_LOG(LM_FULL_INFO,"CSwMatrixSocket::sendCommand()",(LM_CRITICAL,"FAIL")); // logs it
		_EXCPT_FROM_ERROR(SocketErrorExImpl,dummy,err);
		throw dummy;
	}
}

CSocket::OperationResult CSwMatrixSocket::sendBuffer(std::string Msg, CError& error, CConfiguration config)
{

	CSocket::OperationResult res;	
	double start_time = CIRATools::getUNIXEpoch();
    size_t sent_bytes = 0;

	while(sent_bytes < Msg.size())
    {
        size_t sent_now = Send(error, Msg.substr(sent_bytes, Msg.size() - sent_bytes).c_str(), Msg.size() - sent_bytes, config.SwMatrixAddress(), config.SwMatrixPort());
        sent_bytes += sent_now;

		
		if (sent_now==CSocket::WOULDBLOCK) {res=CSocket::WOULDBLOCK;}
		else {res==CSocket::FAIL;}
		if(sent_bytes == Msg.size()){res=CSocket::SUCCESS;}
        
		
		if(sent_now > 0)
        {
            // Reset the timer
            start_time = CIRATools::getUNIXEpoch();
        }
        else if(CIRATools::getUNIXEpoch() - start_time >= 1) // 1 sec
        {
            Close(error);
            ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, "CSwMatrixSocket::sendBuffer()");
            exImpl.addData("Reason", "Timeout when sending command.");
            std::cout << "Timeout sending command" << std::endl;
            throw exImpl;
        }
    }
	return res;
}


int CSwMatrixSocket::receiveBuffer(std::string Msg, CError& error)
{

	double start_time = CIRATools::getUNIXEpoch();

    while(true)
    {
        char buf;
        if(Receive(error, &buf, 1) == 1)
        {
            Msg += buf;
			cout << "buf " << buf << endl;
            // Reset the timer
            start_time = CIRATools::getUNIXEpoch();

			if(int(buf) == 10){ // if return /n char
				return SUCCESS;
			}

        }
        else if(CIRATools::getUNIXEpoch() - start_time >= 1)
        {
            Close(error);
            ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, "CSwMatrixSocket::sendCommand()");
            exImpl.addData("Reason", "Timeout when receiving answer.");
            std::cout << "Timeout receiving answer" << std::endl;
            throw exImpl;
			return FAIL;
        }
    }
}
