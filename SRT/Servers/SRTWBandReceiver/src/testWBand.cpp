#include "LOSocket.h"
#include "CALSocket.h"
#include "SwMatrixSocket.h"
#include "WBandCommand.h"
#include "StatusSocket.h"
#include <LogFilter.h>
#include <typeinfo>

#define TAIL std::string("\r\n");

using namespace IRA;

_IRA_LOGFILTER_DECLARE;


int main(long argc,char **argv)
{

	CLOSocket socket;
	CCALSocket CAL_socket;
	CSwMatrixSocket SwMatrix_socket;
	CConfiguration config;
	
	try {
		config.init();
	}
	catch (ComponentErrors::CDBAccessExImpl& E) {
		IRA::CString msg;
		_EXCPT_TO_CSTRING(msg,E);
		printf("%s\n",(const char *)msg);
		exit(-1);
	}

	//_IRA_LOGFILTER_ACTIVATE(config.repetitionCacheTime(),config.expireCacheTime());
		
	try {
		CAL_socket.init(&config);
	}
	catch (ComponentErrors::SocketErrorExImpl& E) {
		IRA::CString msg;
		_EXCPT_TO_CSTRING(msg,E);
		printf("%s\n",(const char *)msg);
		exit(-1);
	}
	try{
		SwMatrix_socket.init(&config);
	}
	catch (ComponentErrors::SocketErrorExImpl& E) {
		IRA::CString msg;
		_EXCPT_TO_CSTRING(msg,E);
		printf("%s\n",(const char *)msg);
		exit(-1);
	}
	try {
		socket.init(&config);
	}
	catch (ComponentErrors::SocketErrorExImpl& E) {
		IRA::CString msg;
		_EXCPT_TO_CSTRING(msg,E);
		printf("%s\n",(const char *)msg);
		exit(-1);
	}

	while (true) { 
		std::string input;
		std::getline(std::cin, input); 
		printf("> ");

		if (input == "enable USB_devs"){
			try {
				std::string outBuff;
				std::string msg = input + TAIL;
				socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}	
		else if (input == "disable USB_devs") {
			try {
				std::string outBuff;
				std::string msg = input + TAIL;
				socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
		else if (input == "set W_LO_freq_PolH=15000") {
			try {
			
				std::string outBuff;
				std::string msg = input + TAIL;
				socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}		
		else if (input == "set W_LO_freq_PolV=15000") {
			try {
			
				std::string outBuff;
				std::string msg = input + TAIL;
				socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
		else if (input == "get W_LO_PolH") {
			try {
			
				std::string outBuff;
				std::string msg = input + TAIL;
				socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
		else if (input == "get W_LO_PolV") {
			try {
			
				std::string outBuff;
				std::string msg = input + TAIL;
				socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
		else if (input == "get W_LO_Pols") {
			try {
			
				std::string outBuff;
				std::string msg = input + TAIL;
				socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}		
		else if (input == "set W_LO_RefH=INT") {
			try {
			
				std::string outBuff;
				std::string msg = input + TAIL;
				socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
		else if (input == "set W_LO_RefV=INT") {
			try {
			
				std::string outBuff;
				std::string msg = input + TAIL;
				socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
		else if (input == "get W_LO_RefH") {
			try {
			
				std::string outBuff;
				std::string msg = input + TAIL;
				socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
        else if (input == "get W_LO_RefV") {
			try {
			
				std::string outBuff;
				std::string msg = input + TAIL;
				socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
        else if (input == "get W_LO_Synths_Temp") {
			try {
			
				std::string outBuff;
				std::string msg = input + TAIL;
				socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
        else if (input == "get W_LO_HKP_Temp") {
			try {
			
				std::string outBuff;
				std::string msg = input + TAIL;
				socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
        else if (input == "get W_LO_status") {
			try {
			
				std::string outBuff;
				std::string msg = input + TAIL;
				socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
        else if (input == "set LO_att_PolH=63") {
			try {
			
				std::string outBuff;
				std::string msg = input + TAIL;
				socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
        else if (input == "set LO_att_PolV=63") {
			try {
			
				std::string outBuff;
				std::string msg = input + TAIL;
				socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
        else if (input == "get LO_att_PolH") {
			try {
			
				std::string outBuff;
				std::string msg = input + TAIL;
				socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
        else if (input == "get LO_att_PolV") {
			try {
			
				std::string outBuff;
				std::string msg = input + TAIL;
				socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
        else if (input == "get LO_atts") {
			try {
			
				std::string outBuff;
				std::string msg = input + TAIL;
				socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
        else if (input == "set W_home") {
			try {
				
				std::string outBuff;
				std::string msg = input + TAIL;
				SwMatrix_socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
        else if (input == "set W_solar_attn") {
			try {
				
				std::string outBuff;
				std::string msg = input + TAIL;
				SwMatrix_socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
        else if (input == "set W_cal") {
			try {
				
				std::string outBuff;
				std::string msg = input + TAIL;
				SwMatrix_socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
        else if (input == "set W_passthrough") {
			try {
				
				std::string outBuff;
				std::string msg = input + TAIL;
				SwMatrix_socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
        else if (input == "get W_mode") {
			try {
				std::string outBuff;
				std::string msg = input + TAIL;
				SwMatrix_socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
        else if (input == "get IF_switch_config") {
			try {

				std::string outBuff;
				std::string msg = input + TAIL;
				SwMatrix_socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
        else if (input == "set IF_switch_config=1") {
			try {
				std::string outBuff;
				std::string msg = input + TAIL;
				SwMatrix_socket.sendCommand(msg, outBuff, config);
				cout << "Done" << endl;
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
		else{ cout << "Command not found, try again" << endl; }

	}
	socket.cleanUp();
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
}