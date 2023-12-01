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
	CError err;
	Close(err);
}

void CCommandSocket::init(CConfiguration *config) throw (SocketErrorExImpl)
{
	m_pConfiguration=config;
	
	connectSocket(); // throw (SocketErrorExImpl)
}


void CCommandSocket::connectSocket() throw (SocketErrorExImpl)
{
	IRA::CError error;
	ACS_LOG(LM_FULL_INFO,"CCommandSocket::connectSocket()",(LM_NOTICE,"COMMAND_SOCKET_CREATING"));	
	
	createSocket(); // throw (SocketErrorExImpl)
	
	ACS_LOG(LM_FULL_INFO,"CCommandSocket::connectSocket()",(LM_NOTICE,"COMMAND_SOCKET_CONNECTING"));
   CSocket::OperationResult res=Connect(error,m_pConfiguration->DBESMAddress(),m_pConfiguration->DBESMPort());
   
		if (res==FAIL) {
			ACS_LOG(LM_FULL_INFO,"CCommandSocket::connectSocket()",(LM_NOTICE,"COMMAND_SOCKET_FAIL"));
			_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
			dummy.setCode(error.getErrorCode());
			dummy.setDescription((const char*)error.getDescription());
			_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CCommandSocket::connectSocket()");
		}	
		else if (res==WOULDBLOCK) {
			ACS_LOG(LM_FULL_INFO,"CCommandSocket::connectSocket()",(LM_NOTICE,"COMMAND_SOCKET_WOULDBLOCK"));
		}
		else {
			ACS_LOG(LM_FULL_INFO,"CCommandSocket::connectSocket()",(LM_NOTICE,"COMMAND_SOCKET_SUCCESS"));
		}
}

void CCommandSocket::sendCommand(std::string cmd,std::string* outBuff, int all = 0) throw (TimeoutExImpl,SocketErrorExImpl)
{
	OperationResult res;
	CError err;
	res = sendBuffer(cmd,err);
	
	if (res==FAIL) {
		_EXCPT_FROM_ERROR(SocketErrorExImpl,dummy,err);
		throw dummy;
	} 
	else if (res==WOULDBLOCK) {
		if (m_bTimedout) { // this is at least the second time the socket times out
			ACS_LOG(LM_FULL_INFO,"CCommandSocket::sendCommand()",(LM_CRITICAL,"COMMAND_SOCKET_DISCONNECTED_ON_TIMEOUT"));
		}
		m_bTimedout=true;
		_THROW_EXCPT(TimeoutExImpl,"CCommandSocket::sendCommand()");
	}
	else if (res==SUCCESS) {	
		m_bTimedout=false;
		receiveBuffer(outBuff, err, all);
	}
}

void CCommandSocket::set_all(const char * cfg_name) //throw (BackendsErrors::BackendsErrorsEx)
{
      set_all_command(cfg_name);
}


void CCommandSocket::set_mode(short b_addr, const char * cfg_name) //throw (BackendsErrors::BackendsErrorsEx)
{
      set_mode_command(b_addr, cfg_name);
}


void CCommandSocket::set_att(short b_addr, short out_ch, double att_val) //throw (BackendsErrors::BackendsErrorsEx)
{
      set_att_command(b_addr, out_ch, att_val);
}


void CCommandSocket::store_allmode(const char * cfg_name) //throw (BackendsErrors::BackendsErrorsEx)
{
      store_allmode_command(cfg_name);
}


void CCommandSocket::delete_file(const char * cfg_name) //throw (BackendsErrors::BackendsErrorsEx)
{
      delete_file_command(cfg_name);
}


string CCommandSocket::get_status(short b_addr) //throw (BackendsErrors::BackendsErrorsEx)
{
    return get_status_command(b_addr);
}

string CCommandSocket::get_comp(short b_addr) //throw (BackendsErrors::BackendsErrorsEx)
{
    return get_comp_command(b_addr);
}


string CCommandSocket::get_diag(short b_addr) //throw (BackendsErrors::BackendsErrorsEx)
{
    return get_diag_command(b_addr);
}

string CCommandSocket::get_cfg() //throw (BackendsErrors::BackendsErrorsEx)
{
    return get_cfg_command();
}

string CCommandSocket::set_dbeatt(const char * out_dbe, const char * att_val) //throw (BackendsErrors::BackendsErrorsEx)
{
    return set_dbeatt_command(out_dbe, att_val);
}

string CCommandSocket::get_dbeatt(const char * out_dbe) //throw (BackendsErrors::BackendsErrorsEx)
{
       return get_dbeatt_command(out_dbe);
}

string CCommandSocket::get_firm(short b_addr) //throw (BackendsErrors::BackendsErrorsEx)
{
    return get_firm_command(b_addr);
}

void CCommandSocket::parse_longSeq_response(string status_str, string start, string end, ACS::longSeq* vals) //throw (BackendsErrors::BackendsErrorsEx)
{
   unsigned start_pos = status_str.find(start);
   unsigned end_pos = status_str.find(end);
   
   try{
   	stringstream in(status_str.substr(start_pos + start.length(), end_pos - (start_pos + start.length())));
   	long temp;   
      for (int i=0; i<vals->length(); i++)
        {
    	   if (in >> temp)
    	     {
             (*vals)[i]=temp;
           }
        }
   }
    catch (std::out_of_range outofrange) { //if the index is out of range
       throw outofrange;            //throw this exception
    }

}

void CCommandSocket::parse_doubleSeq_response(string status_str, string start, string end, ACS::doubleSeq* vals) //throw (BackendsErrors::BackendsErrorsEx)
{
   unsigned start_pos = status_str.find(start);
   unsigned end_pos = status_str.find(end);

   try {
   std::stringstream in(status_str.substr(start_pos + start.length(), end_pos - (start_pos + start.length())));
   
   double temp;   
   for (int i=0; i<vals->length(); i++)
        {
    	   if (in >> temp)
    	     {
             (*vals)[i]=temp;
           }
        }
    }    
    catch (std::out_of_range outofrange) { //if the index is out of range
    throw outofrange;                      //throw this exception
    }     
}

void CCommandSocket::parse_double_response(string status_str, string start, string end, double* val) //throw (BackendsErrors::BackendsErrorsEx)
{
   unsigned start_pos = status_str.find(start);
   unsigned end_pos = status_str.find(end);

   try {
   stringstream in(status_str.substr(start_pos + start.length(), end_pos - (start_pos + start.length())));
   double temp;
      in >> temp;
      *val=temp;
   }    
   catch (std::out_of_range outofrange) { //if the index is out of range
   throw outofrange;                      //throw this exception
   }

}

void CCommandSocket::parse_string_response(string status_str, string start, string end, string* val) //throw (BackendsErrors::BackendsErrorsEx)
{
   unsigned start_pos = status_str.find(start);
   unsigned end_pos = status_str.find(end);

   try {
   *val = status_str.substr(start_pos + start.length(), end_pos - (start_pos + start.length()));
   }    
   catch (std::out_of_range outofrange) { //if the index is out of range
   throw outofrange;                      //throw this exception
   }
}
// ********************* PROTECTED  **************************************/

void CCommandSocket::onConnect(int ErrorCode)
{
	CError Tmp;
	
	if (ErrorCode==0) {
		if (EventSelect(Tmp,E_CONNECT,false)==SUCCESS) {
				ACS_LOG(LM_FULL_INFO,"CCommandSocket::onConnect()",(LM_NOTICE,"COMMAND_SOCKET_CONNECTED"));			
			}
		}
		else {
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

int CCommandSocket::receiveBuffer(std::string* Msg,CError& error, int all)
{

	 int i = 0;
    TIMEVALUE Now;
	 TIMEVALUE Start;
	 CIRATools::getTime(Start);
	 
    while(true)
    { 
        char buf;
        CIRATools::getTime(Now);
        if(Receive(error, &buf, 1) == 1)
         {
            (*Msg) += buf;
			  //cout << "buf " << buf << endl;
            // Reset the timer
            CIRATools::getTime(Start);
                                 }
			if( (*Msg).back() == '\n' && (*Msg)[(*Msg).size()-2] == '\r') { // if return \r\n (=CRLF))
			   i++;
			   if (all == 0) {
                  cout << *Msg << endl;				
				      return SUCCESS;
				        }
				else {
					if (i == 4) {
                  cout << *Msg << endl;				
				      return SUCCESS;
				      i = 0;
				        }
				     }
			}

        else if(CIRATools::timeDifference(Start,Now)>5000000)
        {
        	   m_bTimedout=true;
            Close(error);
            ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, "CCommandSocketSocket::sendCommand()");
            exImpl.addData("Reason", "Timeout when receiving answer.");
            std::cout << "Timeout receiving answer" << std::endl;
            throw exImpl;
			return FAIL;
        }
     }        
}

void CCommandSocket::set_all_command(const char * cfg_name) //throw (BackendsErrors::BackendsErrorsEx)
{
	 string outBuff;
    string msg = CDBESMCommand::comm_set_allmode(cfg_name);
    //cout << "Message to send is: " << msg << endl;
    //ACS_LOG(LM_FULL_INFO,"CCommandSocket::set_all_command()",(LM_INFO,msg));	
    
    sendCommand(msg, &outBuff, 1);

    if (outBuff.find("unreachable") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::set_all_command()");
		impl.setReason("DBESM board unreachable, timeout error");
		throw impl;
	   }       
	 else if (outBuff.find("ERR") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::set_all_command()");
		impl.setReason("Error in setting whole dbesm configuration, check parameters");
		throw impl;
	}     
}

void CCommandSocket::set_mode_command(short b_addr, const char * cfg_name) //throw (BackendsErrors::BackendsErrorsEx)
{
	 string outBuff;
    string msg = CDBESMCommand::comm_set_mode(to_string(b_addr), cfg_name);
    //cout << "Message to send is: " << msg << endl;
    sendCommand(msg, &outBuff, 0);

    if (outBuff.find("unreachable") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::set_mode_command()");
		impl.setReason("DBESM board unreachable, timeout error");
		throw impl;
	   }   
	 else if (outBuff.find("ERR") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::set_mode_command()");
		impl.setReason("Error in setting single dbesm board configuration, check parameters");
		throw impl;
	}    
}

void CCommandSocket::set_att_command(short b_addr, short out_ch, double att_val) //throw (BackendsErrors::BackendsErrorsEx)
{
    string outBuff;
    string msg = CDBESMCommand::comm_set_att(to_string(b_addr), to_string(out_ch), to_string(att_val));
    //cout << "Message to send is: " << msg << endl;
    sendCommand(msg, &outBuff, 0);

    if (outBuff.find("unreachable") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::set_att_command()");
		impl.setReason("DBESM board unreachable, timeout error");
		throw impl;
	   }       
	 else if (outBuff.find("ERR") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::set_att_command()");
		impl.setReason("Error in setting dbesm attenuator, check parameters");
		throw impl;
	}    
} 

void CCommandSocket::store_allmode_command(const char * cfg_name) //throw (BackendsErrors::BackendsErrorsEx)
{
    string outBuff;
    string msg = CDBESMCommand::comm_store_allmode(cfg_name);
    //cout << "Message to send is: " << msg << endl;
    sendCommand(msg, &outBuff, 0);

    if (outBuff.find("unreachable") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::store_allmode_command()");
		impl.setReason("DBESM board unreachable, timeout error");
		throw impl;
	   }   
	 else if (outBuff.find("ERR") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::store_allmode_command()");
		impl.setReason("Error in storing dbesm configuration file, check parameters");
		throw impl;
	}    
} 

void CCommandSocket::delete_file_command(const char * cfg_name) //throw (BackendsErrors::BackendsErrorsEx)
{
	 string outBuff;
    string msg = CDBESMCommand::comm_delete_file(cfg_name);
    //cout << "Message to send is: " << msg << endl;
    sendCommand(msg, &outBuff, 0);

    if (outBuff.find("unreachable") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::delete_file_command()");
		impl.setReason("DBESM board unreachable, timeout error");
		throw impl;
	   }       
	 else if (outBuff.find("ERR") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::delete_file_command()");
		impl.setReason("Error in deleting dbesm configuration file, check parameters");
		throw impl;
	}    
}

string CCommandSocket::get_status_command(short b_addr) //throw (BackendsErrors::BackendsErrorsEx)
{
    string response;
    string msg = CDBESMCommand::comm_get_status(to_string(b_addr));
    //cout << "Message to send is: " << msg << endl;
    sendCommand(msg, &response, 0);

    if (response.find("unreachable") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::get_status_command()");
		impl.setReason("DBESM board unreachable, timeout error");
		throw impl;
	   }   
	 else if (response.find("ERR") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::get_status_command()");
		impl.setReason("Error in getting dbesm status info, check parameters");
		throw impl;
	}
    return response;
}

string CCommandSocket::get_comp_command(short b_addr) //throw (BackendsErrors::BackendsErrorsEx)
{
    string response;
    string msg = CDBESMCommand::comm_get_comp(to_string(b_addr));
    //cout << "Message to send is: " << msg << endl;
    sendCommand(msg, &response, 0);

    if (response.find("unreachable") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::get_comp_command()");
		impl.setReason("DBESM board unreachable, timeout error");
		throw impl;
	   }   
	 else if (response.find("ERR") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::get_comp_command()");
		impl.setReason("Error in getting dbesm components values, check parameters");
		throw impl;
	}
    return response;
}

string CCommandSocket::get_diag_command(short b_addr) //throw (BackendsErrors::BackendsErrorsEx)
{
    string response;
    string msg = CDBESMCommand::comm_get_diag(to_string(b_addr));
    //cout << "Message to send is: " << msg << endl;
    sendCommand(msg, &response, 0);

    if (response.find("unreachable") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::get_diag_command()");
		impl.setReason("DBESM board unreachable, timeout error");
		throw impl;
	   }   
	 else if (response.find("ERR") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::get_diag_command()");
		impl.setReason("Error in getting dbesm diagnostic info, check parameters");
		throw impl;
	}
    return response;
}

string CCommandSocket::get_cfg_command() //throw (BackendsErrors::BackendsErrorsEx)
{
    string response;
    string msg = CDBESMCommand::comm_get_cfg();
    //cout << "Message to send is: " << msg << endl;
    sendCommand(msg, &response, 0);
    
    if (response.find("unreachable") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::get_cfg_command()");
		impl.setReason("DBESM board unreachable, timeout error");
		throw impl;
	   }    
	 else if (response.find("ERR") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::get_cfg_command()");
		impl.setReason("Error in getting dbesm configuration, check parameters");
		throw impl;
	   }
    return response;
}

string CCommandSocket::set_dbeatt_command(const char * out_dbe, const char * att_val) //throw (BackendsErrors::BackendsErrorsEx)
{
    string response;
    string msg = CDBESMCommand::comm_set_dbeatt(out_dbe, att_val);
    //cout << "Message to send is: " << msg << endl;
    sendCommand(msg, &response, 0);

    if (response.find("unreachable") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::set_dbeatt_command()");
		impl.setReason("DBESM board unreachable, timeout error");
		throw impl;
	   }
	 else if (response.find("ERR") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::set_dbeatt_command()");
		impl.setReason("Error in setting dbesm output attenuation, check parameters");
		throw impl;
	  }
    return response;
}

string CCommandSocket::get_dbeatt_command(const char * out_dbe) //throw (BackendsErrors::BackendsErrorsEx)
{
    string response;
    string msg = CDBESMCommand::comm_get_dbeatt(out_dbe);
    //cout << "Message to send is: " << msg << endl;
    sendCommand(msg, &response, 0);

    if (response.find("unreachable") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::get_dbeatt_command()");
		impl.setReason("DBESM board unreachable, timeout error");
		throw impl;
	   }
	 else if (response.find("ERR") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::get_dbeatt_command()");
		impl.setReason("Error in setting dbesm output attenuation, check parameters");
		throw impl;
	  }
	return response;  
}

string CCommandSocket::get_firm_command(short b_addr) //throw (BackendsErrors::BackendsErrorsEx)
{
    string response;
    string msg = CDBESMCommand::comm_get_firm(to_string(b_addr));
    //cout << "Message to send is: " << msg << endl;
    sendCommand(msg, &response, 0);

    if (response.find("unreachable") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::get_firm_command()");
		impl.setReason("DBESM board unreachable, timeout error");
		throw impl;
	   }   
	 else if (response.find("ERR") != string::npos) {
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"CCommandSocket::get_firm_command()");
		impl.setReason("Error in getting dbesm firmware info, check parameters");
		throw impl;
	}
    return response;
}