#include "CommandSocket.h"
#include "DBESMprotocol.h"
//#include "StatusSocket.h"
#include <LogFilter.h>
#include <typeinfo>

#define INPUT_SIZE 36
#define TAIL std::string("\r\n");

using namespace IRA;
using namespace std;

_IRA_LOGFILTER_DECLARE;


int main(long argc,char **argv)
{
	//std::string input[INPUT_SIZE] = { 0 };

	CCommandSocket socket;
 //	CStatusSocket status;
	CConfiguration config;
	
//	IRA::CSecureArea<CDBESMCommand> data(true);
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
	
	//strcpy(input,"");
	/*
	try {
		status.init(&config,&data);
	}
	catch (ComponentErrors::SocketErrorExImpl& E) {
		IRA::CString msg;
		_EXCPT_TO_CSTRING(msg,E);
		printf("%s\n",(const char *)msg);
		exit(-1);
	}	*/
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
		string input;
		getline(std::cin, input); 
		printf("> ");
		int all = 0;
		if (input.find("ALL") != string::npos && input.find("STOREALL") == string::npos && input.find("CLRMODE") == string::npos) 
				{ all = 1; } // PERCHE' QUESTO FUNZIONI, ACCERTARSI CHE I NOMI DELLE
	                      // CONFIGURAZIONI POSSIBILI NON CONTENGANO LA PAROLA "ALL"

		if (input == "DBE SETALLMODE MFS_7"){
			try {
				string outBuff;
				string msg = input + TAIL;
				cout << msg << endl;
				socket.sendCommand(msg, outBuff, all);
			   }
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			 }
		  }
		  
		  else if (input == "DBE MODE BOARD 15 MFS_7"){
			try {
				string outBuff;
				string msg = input + TAIL;
				cout << msg << endl;
				socket.sendCommand(msg, outBuff, all);
			   }
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			 }
		  }
		 else if (input == "DBE STOREALLMODE CFG_PROVA"){
			try {
				string outBuff;
				string msg = input + TAIL;
				cout << msg << endl;
				socket.sendCommand(msg, outBuff, all);
			   }
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			 }
		  }
		 else if (input == "DBE CLRMODE CFG_PROVA"){
			try {
				string outBuff;
				string msg = input + TAIL;
				cout << msg << endl;
				socket.sendCommand(msg, outBuff, all);
			   }
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			 }
		  }		  
       else if (input == "DBE GETSTATUS BOARD 15"){
			try {
				string outBuff;
				string msg = input + TAIL;
				cout << msg << endl;
				socket.sendCommand(msg, outBuff, all);
			   }
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			 }
		  }		  
		  else if (input == "DBE GETCOMP BOARD 15"){
			try {
				string outBuff;
				string msg = input + TAIL;
				cout << msg << endl;
				socket.sendCommand(msg, outBuff, all);
			   }
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			 }
		  }		  
		 else if (input == "DBE DIAG BOARD 15"){
			try {
				string outBuff;
				string msg = input + TAIL;
				cout << msg << endl;
				socket.sendCommand(msg, outBuff, all);
			   }
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			 }
		  }	
		 else if (input == "DBE ALLDIAG"){
			try {
				string outBuff;
				string msg = input + TAIL;
				cout << msg << endl;
				socket.sendCommand(msg, outBuff, all);
			   }
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			 }
		  }		  
		 else if (input == "DBE SETATT 0 BOARD 15 VALUE 1.0"){
			try {
				string outBuff;
				string msg = input + TAIL;
				cout << msg << endl;
				socket.sendCommand(msg, outBuff, all);
			   }
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			 }
		  }
		 else if (input == "DBE MODE BOARD BOARD MFS_7"){    // test messaggio di errore
			try {
				string outBuff;
				string msg = input + TAIL;
				cout << msg << endl;
				socket.sendCommand(msg, outBuff, all);
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
