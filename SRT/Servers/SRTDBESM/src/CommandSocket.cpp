#include "CommandSocket.h"
#include <LogFilter.h>
#include <TimeoutSync.h>

using namespace IRA;
using namespace ComponentErrors;
using namespace BackendsErrors;

_IRA_LOGFILTER_IMPORT;

CCommandSocket::CCommandSocket()
{
	m_pConfiguration=NULL;
	m_pData=NULL;
	m_bTimedout=false;
}

CCommandSocket::~CCommandSocket()
{
}

void CCommandSocket::cleanUp()
{
	//before closing we have to make sure there is no one waiting for services of this object...so we acquire the mutex in blocking mode
	DDWORD timeo=0;
	IRA::CTimeoutSync guard(&m_syncMutex,timeo);
	guard.acquire();
	CError err;
	Close(err);
	guard.release();
//	CSecAreaResourceWrapper<CDBESMCommand> data=m_pData->Get();
        //commonData->setControlLineState(Antenna::ACU_NOTCNTD);
}

void CCommandSocket::init(CConfiguration *config) throw (SocketErrorExImpl)
{
	m_pConfiguration=config;
	//m_pData=data;
	//CSecAreaResourceWrapper<CDBESMCommand> data=m_pData->Get();
	//commonData->setControlLineState(Antenna::ACU_NOTCNTD);
	connectSocket(); // throw (SocketErrorExImpl)
	/*m_ptSize=0;
	m_lastScanEpoch=0;
	m_currentScanStartEpoch=0; */
}


void CCommandSocket::connectSocket() throw (SocketErrorExImpl)
{
	IRA::CError error;	
	//CSecAreaResourceWrapper<CDBESMCommand> data=m_pData->Get();
    //	if (data->getControlLineState()==Antenna::ACU_NOTCNTD) {
		ACS_LOG(LM_FULL_INFO,"CCommandSocket::connectSocket()",(LM_NOTICE,"COMMAND_SOCKET_CONNECTING"));	
		createSocket(); // throw (SocketErrorExImpl)
		CSocket::OperationResult res=Connect(error,m_pConfiguration->DBESMAddress(),m_pConfiguration->DBESMPort());
		if (res==FAIL) {
			_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
			dummy.setCode(error.getErrorCode());
			dummy.setDescription((const char*)error.getDescription());
		//	data->setControlLineState(Antenna::ACU_NOTCNTD);
			_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CStatusSocket::connectSocket()");
		}	
		else if (res==WOULDBLOCK) {
		//	data->setControlLineState(Antenna::ACU_CNTDING);
		}
		else {
		//	data->setControlLineState(Antenna::ACU_CNTD);
		}
    //	}
}

void CCommandSocket::sendCommand(std::string cmd,std::string outBuff, int all = 0) throw (TimeoutExImpl,SocketErrorExImpl)
{
	OperationResult res;
	CError err;
	//CSecAreaResourceWrapper<CDBESMCommand> data=m_pData->Get();
	
	res=sendBuffer(cmd,err);
	
	if (res==FAIL) {
	    //	data->setStatusWord(CCommonData::CONTROL_LINE_ERROR);
		_EXCPT_FROM_ERROR(SocketErrorExImpl,dummy,err);
		throw dummy;
	} 
	else if (res==WOULDBLOCK) {
		if (m_bTimedout) { // this is at least the second time the socket times out
		//	data->setControlLineState(Antenna::ACU_NOTCNTD);  //declare the connection fell down
			ACS_LOG(LM_FULL_INFO,"CCommandSocket::sendCommand()",(LM_CRITICAL,"COMMAND_SOCKET_DISCONNECTED_ON_TIMEOUT")); // logs it
		}
		m_bTimedout=true;
		_THROW_EXCPT(TimeoutExImpl,"CCommandSocket::sendCommand()");
	}
	else if (res==SUCCESS) {
      printf("command sent\n");		
		m_bTimedout=false;
		int bytes=receiveBuffer(outBuff, err, all);
		return bytes;
	   //	data->clearStatusWord(CCommonData::CONTROL_LINE_ERROR);
	}
}


// ********************* PROTECTED  **************************************/

void CCommandSocket::onConnect(int ErrorCode)
{
	CError Tmp;
	//CSecAreaResourceWrapper<CDBESMCommand> data=m_pData->Get();
    //	if (data->getControlLineState()==Antenna::ACU_CNTDING) {
		if (ErrorCode==0) {
			if (EventSelect(Tmp,E_CONNECT,false)==SUCCESS) {
			    //	if (data->isBusy()) data->setControlLineState(Antenna::ACU_BSY);
			    //	else data->setControlLineState(Antenna::ACU_CNTD);
				ACS_LOG(LM_FULL_INFO,"CCommandSocket::onConnect()",(LM_NOTICE,"COMMAND_SOCKET_CONNECTED"));			
			}
		}
		else {
		//	data->setControlLineState(Antenna::ACU_NOTCNTD);
			ACS_DEBUG_PARAM("CStatusSocket::onConnect()","Reconnection failed, exit code is %d",ErrorCode);
			//ACS_LOG(LM_FULL_INFO,"CStatusSocket::onConnect()",(LM_NOTICE,"Reconnection failed, exit code is %d",ErrorCode));
		}
	
}

// ********************* PRIVATE **************************************/

/*
void CCommandSocket::waitAck(const ACS::Time& commandTime) throw (AntennaErrors::NakExImpl,ComponentErrors::TimeoutExImpl)
{
	TIMEVALUE now;
	ACS::TimeInterval timeout=m_pConfiguration->controlSocketResponseTime()*10;
	CACUProtocol::TCommandAnswers answer;
	bool error=false;
	IRA::CIRATools::getTime(now);
	while (now.value().value<commandTime+timeout) {
		CSecAreaResourceWrapper<CCommonData> data=m_pData->Get(); // take the lock
		if (data->checkLastCommand(answer,error)) { // we have an answer
			if (error) { // problems.......
				printf("Command Answer Error %d, %s\n",answer,(const char *)CACUProtocol::commandAnswer2String(answer));
				_EXCPT(AntennaErrors::NakExImpl,ex,"CCommandSocket::waitAck()");
				ex.setCode((long)answer);
				ex.setMessage((const char *)CACUProtocol::commandAnswer2String(answer));
				data->setStatusWord(CCommonData::REMOTE_COMMAND_ERROR);
				throw ex;
			}
			else {  //success
				data->clearStatusWord(CCommonData::REMOTE_COMMAND_ERROR);
				return;
			}
		}
		else { // wait a little bit before checking again....
			data.Release();
			IRA::CIRATools::Wait(m_pConfiguration->controlSocketDutyCycle());
		}
		IRA::CIRATools::getTime(now); // get current time again
	}
	printf("Command Answer Error: timeout \n");
	_EXCPT(ComponentErrors::TimeoutExImpl,ex,"CCommandSocket::waitAck()");
	throw ex;
}

bool CCommandSocket::checkConnection()
{
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	return ((data->getControlLineState()==Antenna::ACU_CNTD) || (data->getControlLineState()==Antenna::ACU_BSY));
}

bool CCommandSocket::checkIsBusy()
{
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	return data->isBusy();	
}
*/
void CCommandSocket::createSocket() throw (SocketErrorExImpl)
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
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CCommandSocket::createSocket()");
	}
	// set socket send buffer!!!!
	int Val= 64; //CACUProtocol::SOCKET_SEND_BUFFER
	if (setSockOption(error,SO_SNDBUF,&Val,sizeof(int))==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CCommandSocket::createSocket()");
	}
	// set socket in non-blocking mode.
	if (setSockMode(error,NONBLOCKING)==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CCommandSocket::createSocket()");
	}
	if (EventSelect(error,E_CONNECT,true,0)==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CCommandSocket::createSocket()");
	}	
}


CSocket::OperationResult CCommandSocket::sendBuffer(std::string Msg,CError& error)
{

	CSocket::OperationResult res;	
	//double start_time = CIRATools::getUNIXEpoch();
    size_t sent_bytes = 0;

	while(sent_bytes < Msg.size())
    {
        size_t sent_now = Send(error, Msg.substr(sent_bytes, Msg.size() - sent_bytes).c_str(), Msg.size() - sent_bytes);
        sent_bytes += sent_now;

		
		if (sent_now==CSocket::WOULDBLOCK) {res=CSocket::WOULDBLOCK;}
		else {res==CSocket::FAIL;}
		if(sent_bytes == Msg.size()){res=CSocket::SUCCESS;}
        
		/*
		if(sent_now > 0)
        {
            // Reset the timer
            start_time = CIRATools::getUNIXEpoch();
        }
        else if(CIRATools::getUNIXEpoch() - start_time >= 1) // 1 sec
        {
            Close(error);
            ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, "CLOSocket::sendBuffer()");
            exImpl.addData("Reason", "Timeout when sending command.");
            std::cout << "Timeout sending command" << std::endl;
            throw exImpl;
        } */
    //}
	return res;
   }
}

/*{
	int NWrite;
	int BytesSent;
	BytesSent=0;
	while (BytesSent<Len) {
		if ((NWrite=Send(error,(const void *)(Msg+BytesSent),Len-BytesSent))<=0) {
			if (NWrite==WOULDBLOCK) {
				return WOULDBLOCK;
			}
			else {
				return FAIL;
			}
		}
		else { // success
			BytesSent+=NWrite;
		}
	}
	if (BytesSent==Len) {
		return SUCCESS;
	}
	else {
		_SET_ERROR(error,CError::SocketType,CError::SendError,"CCommandSocket::sendBuffer()");
		return FAIL;
	}
}*/
int CCommandSocket::receiveBuffer(std::string Msg,CError& error, int all = 0)
{

	//double start_time = CIRATools::getUNIXEpoch();
	 int i = 0;
    TIMEVALUE Now;
	 TIMEVALUE Start;
	 CIRATools::getTime(Start);
	 printf("function receiveBuffer called\n");
	 
    while(true)
    { 
        char buf;
        CIRATools::getTime(Now);
        if(Receive(error, &buf, 1) == 1)
        {
            Msg += buf;
			cout << "buf " << buf << endl;
            // Reset the timer
            //start_time = CIRATools::getUNIXEpoch();
            CIRATools::getTime(Start);
                                 }
			if( Msg.back() == '\n' && Msg[Msg.size()-2] == '\r') { // if return \r\n (=CRLF))
			   i++;
			   if (all == 0) {
                  cout << Msg << endl;				
				      return SUCCESS;
				        }
				else {
					if (i == 4) {
                  cout << Msg << endl;				
				      return SUCCESS;
				      i = 0;
				        }
				     }
			}

        else if(CIRATools::timeDifference(Start,Now)>5000000)
        {
        	   m_bTimedout=true;
            Close(error);
            ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, "CLOSocket::sendCommand()");
            exImpl.addData("Reason", "Timeout when receiving answer.");
            std::cout << "Timeout receiving answer" << std::endl;
            throw exImpl;
			return FAIL;
        }
    }
}

/*{
	char inCh;
	int res;
	int nRead=0;
	TIMEVALUE Now;
	TIMEVALUE Start;
	CIRATools::getTime(Start);
	printf("function receiveBuffer called\n");
	while(true) {
		res=Receive(error,&inCh,Len);
		if (res==WOULDBLOCK) {
			CIRATools::getTime(Now);
			if (CIRATools::timeDifference(Start,Now)>5000000) {
				m_bTimedout=true;
				printf("%d\n", res);
            printf("%c\n", inCh);
				printf("no read back from sim wouldblock\n");
				return -2;
			}
			else {
				CIRATools::Wait(0,20000);
				continue;
			}
		}
		else if (res==FAIL) { 
			/*setStatus(NOTCNTD);
			CString app;
			app.Format("SOCKET_DISCONNECTED - %s",(const char *)m_Error.getFullDescription());
			_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommandLine::receiveBuffer()",(const char*)app); */ /*
			printf("no read back from sim fail\n");			
			return res;
		}
		else if (res==0) {
			/*setStatus(NOTCNTD);
			_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommandLine::receiveBuffer()","SOCKET_DISCONNECTED - remote side shutdown"); */ /*
			printf("no read back from sim disconnected\n");			
			return res;	
		}
		else {
			if (inCh!='\r') {
				Msg[nRead]=inCh;
				nRead++;
				printf("read back from sim");
			}
			else {
				Msg[nRead]=0;
				printf("read back from sim");
				return nRead;
			}
		}
	}
	printf("function receive buffer concluded\n");
} */