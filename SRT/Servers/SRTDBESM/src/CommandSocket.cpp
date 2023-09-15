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
		ACS_LOG(LM_FULL_INFO,"CCommandSocket::connectSocket()",(LM_NOTICE,"COMMAND_SOCKET_CREATING"));	
		createSocket(); // throw (SocketErrorExImpl)
		ACS_LOG(LM_FULL_INFO,"CCommandSocket::connectSocket()",(LM_NOTICE,"COMMAND_SOCKET_CONNECTING"));
		CSocket::OperationResult res=Connect(error,m_pConfiguration->DBESMAddress(),m_pConfiguration->DBESMPort());
		if (res==FAIL) {
			ACS_LOG(LM_FULL_INFO,"CCommandSocket::connectSocket()",(LM_NOTICE,"COMMAND_SOCKET_FAIL"));
			printf("res==FAIL");
			_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
			dummy.setCode(error.getErrorCode());
			dummy.setDescription((const char*)error.getDescription());
		//	data->setControlLineState(Antenna::ACU_NOTCNTD);
			_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CCommandSocket::connectSocket()");
		}	
		else if (res==WOULDBLOCK) {
			ACS_LOG(LM_FULL_INFO,"CCommandSocket::connectSocket()",(LM_NOTICE,"COMMAND_SOCKET_WOULDBLOCK"));
			printf("res==WOULDBLOCK\n");
		//	data->setControlLineState(Antenna::ACU_CNTDING);
		}
		else {
			ACS_LOG(LM_FULL_INFO,"CCommandSocket::connectSocket()",(LM_NOTICE,"COMMAND_SOCKET_SUCCESS"));
			printf("res==SUCCESS");
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
		printf("receiveBuffer returns %d\n", bytes);
	   //	data->clearStatusWord(CCommonData::CONTROL_LINE_ERROR);
	}
}

void CCommandSocket::set_all(const char * cfg_name) throw (BackendsErrors::BackendsErrorsEx)
{
      set_all_command(cfg_name);
}

void CCommandSocket::set_mode(short b_addr, const char * cfg_name) throw (BackendsErrors::BackendsErrorsEx)
{
      set_mode_command(b_addr, cfg_name);
}

void CCommandSocket::set_att(short b_addr, short out_ch, double att_val) throw (BackendsErrors::BackendsErrorsEx)
{
      set_att_command(b_addr, out_ch, att_val);
}

void CCommandSocket::store_allmode(const char * cfg_name) throw (BackendsErrors::BackendsErrorsEx)
{
      store_allmode_command(cfg_name);
}

void CCommandSocket::clr_mode(const char * cfg_name) throw (BackendsErrors::BackendsErrorsEx)
{
      clr_mode_command(cfg_name);
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
			ACS_LOG(LM_FULL_INFO,"CStatusSocket::onConnect()",(LM_NOTICE,"Reconnection failed, exit code is %d",ErrorCode));
		}
	
}

// ********************* PRIVATE **************************************/

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
		if(sent_bytes == Msg.size()){res=CSocket::SUCCESS;}
		else {res=CSocket::FAIL;}
		
	   return res;
   }

}

int CCommandSocket::receiveBuffer(std::string Msg,CError& error, int all)
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

void CCommandSocket::set_all_command(const char * cfg_name) throw (BackendsErrors::BackendsErrorsEx)
{
	 string outBuff;
    string msg = CDBESMCommand::comm_set_allmode(cfg_name);
    cout << "Message to send is: " << msg << endl;
    sendCommand(msg, outBuff, 1);
    cout << "\nfunction sendCommand called from CommandSocket.cpp\n" << endl;
}

void CCommandSocket::set_mode_command(short b_addr, const char * cfg_name) throw (BackendsErrors::BackendsErrorsEx)
{
	 string outBuff;
    string msg = CDBESMCommand::comm_set_mode(to_string(b_addr), cfg_name);
    cout << "Message to send is: " << msg << endl;
    sendCommand(msg, outBuff, 0);
    cout << "\nfunction sendCommand called from CommandSocket.cpp\n" << endl;
}

void CCommandSocket::set_att_command(short b_addr, short out_ch, double att_val) throw (BackendsErrors::BackendsErrorsEx)
{
    string outBuff;
    string msg = CDBESMCommand::comm_set_att(to_string(b_addr), to_string(out_ch), to_string(att_val));
    cout << "Message to send is: " << msg << endl;
    sendCommand(msg, outBuff, 0);
    cout << "\nfunction sendCommand called from CommandSocket.cpp\n" << endl;
} 

void CCommandSocket::store_allmode_command(const char * cfg_name) throw (BackendsErrors::BackendsErrorsEx)
{
    string outBuff;
    string msg = CDBESMCommand::comm_store_allmode(cfg_name);
    cout << "Message to send is: " << msg << endl;
    sendCommand(msg, outBuff, 0);
    cout << "\nfunction sendCommand called from CommandSocket.cpp\n" << endl;
} 

void CCommandSocket::clr_mode_command(const char * cfg_name) throw (BackendsErrors::BackendsErrorsEx)
{
	 string outBuff;
    string msg = CDBESMCommand::comm_clr_mode(cfg_name);
    cout << "Message to send is: " << msg << endl;
    sendCommand(msg, outBuff, 0);
    cout << "\nfunction sendCommand called from CommandSocket.cpp\n" << endl;
}