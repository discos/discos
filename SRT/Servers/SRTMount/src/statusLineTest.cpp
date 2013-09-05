// $Id: statusLineTest.cpp,v 1.3 2011-06-03 18:02:49 a.orlati Exp $

#include "ACUProtocol.h"
#include "StatusSocket.h"
#include "CommonData.h"
#include <LogFilter.h>

using namespace IRA;

_IRA_LOGFILTER_DECLARE;



void binder(const CCommonData *data)
{
	static ACS::Time pastTime=0;
	ACS::Time currentTime;
	ACS::TimeInterval diff;
	IRA::CString strTime;
	CACUProtocol::TGeneralStatus *generalStatus=data->generalStatus();
	CACUProtocol::TAxisStatus *azimuthStatus=data->azimuthStatus();
	CACUProtocol::TAxisStatus *elevationStatus=data->elevationStatus();
	CACUProtocol::TPointingStatus * pointingStatus=data->pointingStatus();
	currentTime=pointingStatus->actualTime();
	IRA::CIRATools::timeToStr(currentTime,strTime);
	printf("Time: %s\n",(const char *)strTime);
	if (pastTime!=0) {
		diff=currentTime-pastTime;
		printf("status frame received in %lld msec\n",diff/10000);
	}
	else {
		printf("status frame received in 0 msec\n");
	}
	pastTime=currentTime;
	CACUProtocol::THardwareInterlock hw=generalStatus->hardware_interlock();
	CACUProtocol::TSoftwareInterlock sw=generalStatus->software_interlock();
	CACUProtocol::TWarningMainAxis azw=azimuthStatus->warnings();
	CACUProtocol::TErrorMainAxis aze=azimuthStatus->errors();
	CACUProtocol::TWarningMainAxis elw=elevationStatus->warnings();
	CACUProtocol::TErrorMainAxis ele=elevationStatus->errors();
	printf("hw interlock\tsw interlock\taz warn\taz err\tel warn\tel err\n");
	for (unsigned i=0;i<sizeof(TDWORD)*8;i++) {
		printf("%02d %d\t%02d %d\t",i,hw.bit(i),i,sw.bit(i));
		printf("%02d %d\t%02d %d\t",i,azw.bit(i),i,aze.bit(i));
		printf("%02d %d\t%02d %d\n",i,elw.bit(i),i,ele.bit(i));
	}
}

int main(long argc,char **argv)
{
	char input[256];
	CStatusSocket socket;
	CConfiguration config;
	CACUProtocol prot;
	IRA::CSecureArea<CCommonData> data(true);

	
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
	
	try {
		socket.init(&config,&data);
	}
	catch (ComponentErrors::SocketErrorExImpl& E) {
		IRA::CString msg;
		_EXCPT_TO_CSTRING(msg,E);
		printf("%s\n",(const char *)msg);
		exit(-1);
	}
	socket.setBinder(&binder);
	scanf("%s",input);
	socket.cleanUp();
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
}

