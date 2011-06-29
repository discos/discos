// $Id: testACU.cpp,v 1.3 2010-12-28 19:07:55 a.orlati Exp $

#include "ACUProtocol.h"
#include "CommandSocket.h"
#include "CommonData.h"
#include "StatusSocket.h"
#include <LogFilter.h>

_IRA_LOGFILTER_DECLARE;


int main(long argc,char **argv)
{
	char input[64];
	CCommandSocket socket;
	CStatusSocket status;
	CConfiguration config;
	IRA::CSecureArea<CCommonData> data(true);
	CACUProtocol prot;
	try {
		config.init();
	}
	catch (ComponentErrors::CDBAccessExImpl& E) {
		IRA::CString msg;
		_EXCPT_TO_CSTRING(msg,E);
		printf("%s\n",(const char *)msg);
		exit(-1);
	}

	_IRA_LOGFILTER_ACTIVATE(config.repetitionCacheTime(),config.expireCacheTime());
	
	strcpy(input,"");
	
	try {
		status.init(&config,&data);
	}
	catch (ComponentErrors::SocketErrorExImpl& E) {
		IRA::CString msg;
		_EXCPT_TO_CSTRING(msg,E);
		printf("%s\n",(const char *)msg);
		exit(-1);
	}	
	try {
		socket.init(&config,&data);
	}
	catch (ComponentErrors::SocketErrorExImpl& E) {
		IRA::CString msg;
		_EXCPT_TO_CSTRING(msg,E);
		printf("%s\n",(const char *)msg);
		exit(-1);
	}
	while (strcmp(input,"exit")!=0) {
		printf("> ");
		scanf("%s",input);
		if (strcmp(input,"presetMode")==0) {
			try {
				socket.changeMode(Antenna::ACU_PRESET,Antenna::ACU_PRESET);
				printf("Done!\n");
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}	
		else if (strcmp(input,"rateMode")==0) {
			try {
				socket.changeMode(Antenna::ACU_RATE,Antenna::ACU_RATE);
				printf("Done!\n");
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
		else if (strcmp(input,"programTrackMode")==0) {
			try {
				socket.changeMode(Antenna::ACU_PROGRAMTRACK,Antenna::ACU_PROGRAMTRACK);
				printf("Done!\n");
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}		
		else if (strcmp(input,"programTrackMode")==0) {
			try {
				socket.changeMode(Antenna::ACU_PROGRAMTRACK,Antenna::ACU_PROGRAMTRACK);
				printf("Done!\n");
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
		else if (strcmp(input,"rates")==0) {
			try {
				socket.rates(0.2,0.12);
				printf("Done!\n");
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
		else if (strcmp(input,"stop")==0) {
			try {
				socket.stop();
				printf("Done!\n");
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
		else if (strcmp(input,"reset")==0) {
			try {
				socket.resetErrors();
				printf("Done!\n");
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}		
		else if (strcmp(input,"setTime")==0) {
			try {
				TIMEVALUE now;
				IRA::CIRATools::getTime(now);
				socket.setTime(now.value().value);
				printf("Done!\n");
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
		else if (strcmp(input,"setTimeOffset")==0) {
			try {
				socket.setTimeOffset(0.03445);
				printf("Done!\n");
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}
		else if (strcmp(input,"programTrack")==0) {
			try {
				TIMEVALUE now;
				IRA::CIRATools::getTime(now);
				socket.programTrack(220.5,33.5,now.value().value,true);
				socket.programTrack(221.5,34.5,now.value().value+10000,false);
				socket.programTrack(222.5,35.5,now.value().value+20000,false);
				socket.programTrack(223.5,36.5,now.value().value+30000,false);
				socket.programTrack(224.5,37.5,now.value().value+40000,false);
				socket.programTrack(225.5,38.5,now.value().value+50000,false);
				printf("Done!\n");
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				IRA::CString msg;
				_EXCPT_TO_CSTRING(msg,E);
				printf("%s\n",(const char *)msg);
			}
		}		
	}
	socket.cleanUp();
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
}

