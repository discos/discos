// $Id: Configuration.cpp,v 1.8 2010-10-14 12:08:28 a.orlati Exp $

#include "Configuration.h"

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

void CConfiguration::Init(maci::ContainerServices *Services) throw (ComponentErrors::CDBAccessExImpl)
{
	ACS_TRACE("::CConfiguration::Init()");
	// get ACU IP and port number from the database.	
	_GET_STRING_ATTRIBUTE("IPAddress","IP Address:",m_sACUAddress,"");
	_GET_DWORD_ATTRIBUTE("Port","Port:",m_wACUPort,"");
	_GET_DWORD_ATTRIBUTE("ConnectTimeout","Connection timeout (uSec):",m_dwconnectTimeout,"");
	_GET_DWORD_ATTRIBUTE("ReceiveTimeout","Receive timeout (uSec):",m_dwreceiveTimeout,"");
	_GET_DWORD_ATTRIBUTE("ControlThreadPeriod","Control thread period  (uSec):",m_dwcontrolThreadPeriod,"");	
	//_GET_DOUBLE_ATTRIBUTE("TrackingPrecision","TrackingPrecision (degrees):",m_dtrackingPrecision,"");
	_GET_DWORD_ATTRIBUTE("RepetitionCacheTime","RepetitionCacheTime (uSec):",m_dwrepetitionCacheTime,"");
	_GET_DWORD_ATTRIBUTE("RepetitionExpireTime","RepetitionExpireTime (uSec):",m_dwexpireCacheTime,"");
	_GET_DWORD_ATTRIBUTE("PropertyRefreshTime","PropertyRefreshTime (uSec):",m_dwpropertyRefreshTime,"");	
	_GET_DOUBLE_ATTRIBUTE("minElevation","Elevation lower limit (degrees):",m_elevationInfo.lowerLimit,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE("maxElevation","Elevation upper limit (degrees):",m_elevationInfo.upperLimit,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE("minAzimuth","Azimuth lower limit (degrees):",m_azimuthInfo.lowerLimit,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE("maxAzimuth","Azimuth upper limit (degrees):",m_azimuthInfo.upperLimit,"DataBlock/Mount");	
	_GET_DOUBLE_ATTRIBUTE("maxElevationRate","Elevation rate (degrees/sec):",m_elevationRateInfo.upperLimit,"DataBlock/Mount");
	m_elevationRateInfo.lowerLimit=-m_elevationRateInfo.upperLimit;
	_GET_DOUBLE_ATTRIBUTE("maxAzimuthRate","Azimuth rate(degrees/sec):",m_azimuthRateInfo.upperLimit,"DataBlock/Mount");
	m_azimuthRateInfo.lowerLimit=-m_azimuthRateInfo.upperLimit;
	_GET_DOUBLE_ATTRIBUTE("cw_ccw_limit","CW/CCW limit (degrees):",m_cwLimit,"DataBlock/Mount");	
	_GET_DOUBLE_ATTRIBUTE("OscillationThreshold","Oscillation theshold (deg):",m_doscThreashold,"");
	_GET_DWORD_ATTRIBUTE("OscillationAlarmDuration","Oscillation alarm duration (uSec):",m_dwoscAlarmDuration,"");
	_GET_DWORD_ATTRIBUTE("OscillationNumberThreshold","Oscillation number threashold:",m_dwoscNumberThreshold,"");
	_GET_DWORD_ATTRIBUTE("OscillationRecoverWaitTime","Oscillation recovery wait time (uSec):",m_dwoscRecoverTime,"");
	m_dwoscAlarmDuration*=10;
	m_dwoscRecoverTime*=10;
	m_dwcontrolThreadPeriod*=10;
}
