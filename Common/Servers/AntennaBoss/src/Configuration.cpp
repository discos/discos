// $Id: Configuration.cpp,v 1.11 2011-05-20 16:52:46 a.orlati Exp $

#include "Configuration.h"
#include "slamac.h"

#define _GET_DWORD_ATTRIBUTE(ATTRIB,DESCR,FIELD) { \
	DWORD tmpw; \
	if (!CIRATools::getDBValue(Services,ATTRIB,tmpw)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"CConfiguration::Init()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
	} \
	else { \
		FIELD=tmpw; \
		ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %lu",tmpw); \
	} \
}

#define _GET_STRING_ATTRIBUTE(ATTRIB,DESCR,FIELD) { \
	CString tmps; \
	if (!CIRATools::getDBValue(Services,ATTRIB,tmps)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"::CConfiguration::Init()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
	} \
	else { \
		FIELD=tmps; \
		ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %s",(const char*)tmps); \
	} \
}

#define _GET_DOUBLE_ATTRIBUTE(ATTRIB,DESCR,FIELD) { \
	double tmpd; \
	if (!CIRATools::getDBValue(Services,ATTRIB,tmpd)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"CConfiguration::Init()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
	} \
	else { \
		FIELD=tmpd; \
		ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %lf",tmpd); \
	} \
}

#define _GET_DOUBLE_ATTRIBUTE_E(ATTRIB,DESCR,FIELD,NAME) { \
	double tmpd; \
	if (!IRA::CIRATools::getDBValue(Services,ATTRIB,tmpd,"alma/",NAME)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"CConfiguration::init()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
	} \
	else { \
		FIELD=tmpd; \
		ACS_DEBUG_PARAM("CConfiguration::init()",DESCR" %lf",tmpd); \
	} \
}

CConfiguration::CConfiguration()
{
}

CConfiguration::~CConfiguration()
{
}

void CConfiguration::init(maci::ContainerServices *Services) throw (ComponentErrors::CDBAccessExImpl)
{
	IRA::CString temp;
	_GET_STRING_ATTRIBUTE("MountInstance","Mount instance is ",m_mountInstance);
	_GET_STRING_ATTRIBUTE("ObservatoryInterface","Observatory interface is ",m_observatoryComp);
	_GET_STRING_ATTRIBUTE("PointingModelInstance","Pointing model instance is ",m_pointingInstance);
	_GET_STRING_ATTRIBUTE("RefractionInstance","Refraction instance is ",m_refractionInstance);
	_GET_STRING_ATTRIBUTE("Otf","Generator for On The Fly  is ",m_otfInterface);
	_GET_STRING_ATTRIBUTE("Sun","Generator for the Sun  is ",m_sunInterface);
	_GET_STRING_ATTRIBUTE("Moon","Generator for the Moon  is ",m_moonInterface);
	_GET_STRING_ATTRIBUTE("Sidereal","Generator for sidereal tracking  is ",m_siderealInterface);
	_GET_STRING_ATTRIBUTE("SolarSystemBody","Generator for bodies of the Solar System  is ",m_solarSystemBodyInterface);
	_GET_STRING_ATTRIBUTE("Satellite","Generator for Satellite  is ",m_satelliteInterface);
	_GET_DWORD_ATTRIBUTE("WatchingThreadTime","Sleep time of watching thread (uSec)",m_watchingThreadTime);
	_GET_DWORD_ATTRIBUTE("WorkingThreadTime","Sleep time of watching thread (uSec)",m_workingThreadTime);
	_GET_DWORD_ATTRIBUTE("CoordinateIntegration","Integration period for coordinates (uSec)",m_coordinateIntegration);
	_GET_DWORD_ATTRIBUTE("RepetitionCacheTime","Log repetition filter cache time (uSec)",m_repetitionCacheTime);
	_GET_DWORD_ATTRIBUTE("RepetitionExpireTime","Log repetition filter expire time  (uSec)",m_repetitionExpireTime);
	_GET_DWORD_ATTRIBUTE("MinPointNumber","Minimum guaranteed points in the trajectory",m_minPointNumber);
	_GET_DWORD_ATTRIBUTE("MaxPointNumber","Maximum points in the trajectory",m_maxPointNumber);
	_GET_DWORD_ATTRIBUTE("GapTime","Gap time between points in trajectory  (uSec)",m_gapTime);
	_GET_DOUBLE_ATTRIBUTE_E("maxAzimuthRate","Max absolute value for Az rate (degrees/s):",m_maxAzimuthRate,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE_E("maxElevationRate","Max absolute value for El rate (degrees/s):",m_maxElevationRate,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE_E("minElevation","Lower elevation limit (degrees):",m_minElevation,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE_E("maxElevation","Upper elevation limit (degrees):",m_maxElevation,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE_E("maxAzimuthAcceleration","Max value for Az acceleration (degrees/s^2):",m_maxAzimuthAccelaration,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE_E("maxElevationAcceleration","Max value for El acceleration (degrees/s^2):",m_maxElevationAcceleration,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE_E("diameter","Telescope dish diameter (meters):",m_diameter,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE_E("MinElevationAvoidance","Suggested lower elevation limit  for source observation(degrees):",m_minElevationAvoidance,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE_E("MaxElevationAvoidance","Suggested upper elevation limit  for source observation(degrees):",m_maxElevationAvoidance,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE("CutOffElevation","The cut off elevation is (degrees):",m_cutOffElevation);
	_GET_STRING_ATTRIBUTE("SkydipElevationRange","The skydip elevation range is (degrees):",temp);

	if (sscanf((const char *)temp,"%lf-%lf",&m_skydipElDown,&m_skydipElUp)!=2) {
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"CConfiguration::Init()");
		dummy.setFieldName("SkydipElevationRange");
		throw dummy;
	}
	m_skydipElDown*=DD2R;
	m_skydipElUp*=DD2R;
	m_maxElevation*=DD2R;
	m_minElevation*=DD2R;
	m_maxAzimuthRate*=DD2R;
	m_maxElevationRate*=DD2R;
	m_maxAzimuthAccelaration*=DD2R;
	m_maxElevationAcceleration*=DD2R;
	m_minElevationAvoidance*=DD2R;
	m_maxElevationAvoidance*=DD2R;
	m_coordinateIntegration*=10;
	m_cutOffElevation*=DD2R;
}

