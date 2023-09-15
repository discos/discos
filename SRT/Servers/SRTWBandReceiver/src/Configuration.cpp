// $Id: Configuration.cpp,v 1.5 2011-06-01 18:24:44 a.orlati Exp $

#include "Configuration.h"

using namespace IRA;

#define _GET_DOUBLE_ATTRIBUTE(ATTRIB,DESCR,FIELD,NAME) { \
	double tmpd; \
	if (!CIRATools::getDBValue(Services,ATTRIB,tmpd,"alma/",NAME)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"CConfiguration::Init()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
	} \
	else { \
		FIELD=tmpd; \
		ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %lf",tmpd); \
	} \
}

#define _GET_DWORD_ATTRIBUTE(ATTRIB,DESCR,FIELD,NAME) { \
	DWORD tmpw; \
	if (!CIRATools::getDBValue(Services,ATTRIB,tmpw,"alma/",NAME)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"CConfiguration::Init()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
	} \
	else { \
		FIELD=tmpw; \
		ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %lu",tmpw); \
	} \
}

#define _GET_STRING_ATTRIBUTE(ATTRIB,DESCR,FIELD,NAME) { \
	CString tmps; \
	if (!CIRATools::getDBValue(Services,ATTRIB,tmps,"alma/",NAME)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"::CConfiguration::Init()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
	} \
	else { \
		FIELD=tmps; \
		ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %s",(const char*)tmps); \
	} \
}

void CConfiguration::init(maci::ContainerServices *Services) throw (ComponentErrors::CDBAccessExImpl)
{
	cout << "prova" << endl;
	_GET_STRING_ATTRIBUTE("WLOAddress","W LO IP address:",m_WLOAddress,"");
	_GET_DWORD_ATTRIBUTE("WLOPort","W LO port:",m_WLOPort,"");
	_GET_STRING_ATTRIBUTE("WSwitchMatrixAddress","W Band Switch Matrix address:",m_WSwMatrixAddress,"");
	_GET_DWORD_ATTRIBUTE("SwitchMatrixPort","W Band Switch Matrix port:",m_WSwMatrixPort,"");
	_GET_STRING_ATTRIBUTE("WCALAddress","W Band Solar Attenuator address:",m_WCALAddress,"");
	_GET_DWORD_ATTRIBUTE("WCALPort","W Band Solar Attenuator port:",m_WCALPort,"");
	_GET_DWORD_ATTRIBUTE("ControlSocketResponseTime","Socket Response Time:",m_socketResponseTime,"");
	_GET_DWORD_ATTRIBUTE("WatchDogSleepTime","Socket Sleep Time:",m_socketSleepTime,"");
	_GET_DWORD_ATTRIBUTE("RepetitionCacheTime","RepetitionCacheTime (uSec):",m_repetitionCacheTime,"");
	_GET_DWORD_ATTRIBUTE("RepetitionExpireTime","RepetitionExpireTime (uSec):",m_expireCacheTime,"");
	_GET_DWORD_ATTRIBUTE("WatchDogThreadPeriod","Watch dog thread execution period (uSec):",m_watchDogThreadPeriod,"");
	_GET_DWORD_ATTRIBUTE("WatchDogResponseTime","Watch dog thread response time (uSec):",m_watchDogResponseTime,"");
}

void CConfiguration::init() throw (ComponentErrors::CDBAccessExImpl)
{
	m_WLOAddress="127.0.0.1"; //"192.168.200.60";
	m_WLOPort=12703;
	m_WSwMatrixAddress="127.0.0.1"; //"192.168.200.61";
	m_WSwMatrixPort=13200;
	m_WCALAddress="127.0.0.1"; //"192.168.200.62";
	m_WCALPort=13100;
	m_socketResponseTime=1000000; // 1 second
	m_socketSleepTime=1000000; // 1 second
	m_repetitionCacheTime=7000000; // 7 seconds
	m_expireCacheTime=10000000; // 10 seconds
	m_watchDogThreadPeriod=500000; // 1/2 a second
	m_watchDogResponseTime=500000; // 1/2 a second

}
