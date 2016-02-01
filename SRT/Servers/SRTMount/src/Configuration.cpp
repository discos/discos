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
	_GET_STRING_ATTRIBUTE("ACUIPAddress","IP address:",m_ACUAddress,"");
	_GET_DWORD_ATTRIBUTE("CommandPort","Command port:",m_commandPort,"");
	_GET_DWORD_ATTRIBUTE("StatusPort","Status port:",m_statusPort,"");
	_GET_DWORD_ATTRIBUTE("StatusSocketTimeout","Timeout on status socket (uSec):",m_statusSocketTimeout,"");
	_GET_DWORD_ATTRIBUTE("StatusSocketDutyCycle","Status socket duty cycle (uSec):",m_statusSocketDutyCycle,"");
	_GET_DWORD_ATTRIBUTE("ControlSocketResponseTime","Maximum allowed time for the control thread to respond (uSec):",m_controlSocketResponseTime,"");
	_GET_DWORD_ATTRIBUTE("ControlSocketDutyCycle","Control socket duty cycle (uSec):",m_controlSocketDutyCycle,"");
	_GET_DWORD_ATTRIBUTE("RepetitionCacheTime","RepetitionCacheTime (uSec):",m_dwrepetitionCacheTime,"");
	_GET_DWORD_ATTRIBUTE("RepetitionExpireTime","RepetitionExpireTime (uSec):",m_dwexpireCacheTime,"");
	_GET_DWORD_ATTRIBUTE("WatchDogThreadPeriod","Watch dog thread execution period (uSec):",m_dwwatchDogThreadPeriod,"");
	_GET_DWORD_ATTRIBUTE("StowTimeout","Timeout on stowing operation (uSec):",m_dwstowTimeout,"");
	_GET_DWORD_ATTRIBUTE("UnStowTimeout","Timeout on unstowing operation (uSec):",m_dwunstowTimeout,"");

	_GET_DOUBLE_ATTRIBUTE("minElevation","Elevation lower limit (degrees):",m_elevationLowerLimit,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE("maxElevation","Elevation upper limit (degrees):",m_elevationUpperLimit,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE("minAzimuth","Azimuth lower limit (degrees):",m_azimuthLowerLimit,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE("maxAzimuth","Azimuth upper limit (degrees):",m_azimuthUpperLimit,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE("maxElevationRate","Elevation rate (degrees/sec):",m_elevationRateUpperLimit,"DataBlock/Mount");
	m_elevationRateLowerLimit=-m_elevationRateUpperLimit;
	_GET_DOUBLE_ATTRIBUTE("maxAzimuthRate","Azimuth rate(degrees/sec):",m_azimuthRateUpperLimit,"DataBlock/Mount");
	m_azimuthRateLowerLimit=-m_azimuthRateUpperLimit;
	_GET_DOUBLE_ATTRIBUTE("cw_ccw_limit","CW/CCW limit (degrees):",m_cwLimit,"DataBlock/Mount");
	_GET_STRING_ATTRIBUTE("TimeSource","Time source:",m_timeSource,"");
}

void CConfiguration::init() throw (ComponentErrors::CDBAccessExImpl)
{
	m_ACUAddress="192.168.200.2";
	m_commandPort=9000;
	m_statusPort=9001;
	m_statusSocketTimeout=250000;
	m_statusSocketDutyCycle=25000;
	m_controlSocketResponseTime=1000000; //one second
	m_controlSocketDutyCycle=30000;
	m_dwrepetitionCacheTime=7000000; // 7 seconds
	m_dwexpireCacheTime=10000000; //ten seconds
	m_azimuthLowerLimit=-90.0;
	m_azimuthUpperLimit=450.0;
	m_elevationLowerLimit=5.0;
	m_elevationUpperLimit=90.0;
	m_cwLimit=180.0;
	m_azimuthRateLowerLimit=-0.4;
	m_azimuthRateUpperLimit=0.4;
	m_elevationRateLowerLimit=-0.25;
	m_elevationRateUpperLimit=0.25;
	m_dwwatchDogThreadPeriod=500000; // half a second
	m_dwstowTimeout=360000000; // four minutes...240 seconds
	m_dwunstowTimeout=30000000; // 30 seconds
	m_timeSource="EXT";
}
