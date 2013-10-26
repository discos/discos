// $Id: Configuration.cpp,v 1.3 2010-09-14 08:50:08 a.orlati Exp $

#include "Configuration.h"

using namespace IRA;
using namespace FitsWriter_private;

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

#define _GET_DOUBLE_ATTRIBUTE(ATTRIB,DESCR,FIELD,NAME) { \
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
	_GET_DWORD_ATTRIBUTE("WorkingThreadTime","Sleep time of working thread (uSec)",m_workingThreadTime);
	_GET_DWORD_ATTRIBUTE("CollectorThreadTime","Sleep time of collector thread (uSec)",m_collectorThreadTime);	
	_GET_DWORD_ATTRIBUTE("WorkingThreadTimeSlice","Time slice of working thread (uSec)",m_workingThreadTimeSlice);
	_GET_DWORD_ATTRIBUTE("RepetitionCacheTime","Log repetition filter cache time (uSec)",m_repetitionCacheTime);
	_GET_DWORD_ATTRIBUTE("RepetitionExpireTime","Log repetition filter expire time  (uSec)",m_repetitionExpireTime);
	_GET_DWORD_ATTRIBUTE("MeteoParameterDutyCycle","Weather sensor enquire gap  (uSec)",m_meteoParameterDutyCycle);
	_GET_DWORD_ATTRIBUTE("TrackingFlagDutyCycle","Tracking flag enquire gap  (uSec)",m_trackingFlagDutyCycle);
	_GET_DWORD_ATTRIBUTE("MinorServoEnquireMinGap","Minor Servo enquire gap  (uSec)",m_minorServoEnquireMinGap);
	_GET_STRING_ATTRIBUTE("AntennaBossInterface","Antenna Boss component interface is ",m_antennaBossComp);
	_GET_STRING_ATTRIBUTE("ObservatoryInterface","Observatory component is ",m_observatoryComp);
	_GET_STRING_ATTRIBUTE("ReceiversBossInterface","Receivers Boss component interface is ",m_receiversBossComp);
	_GET_STRING_ATTRIBUTE("MinorServoBossInterface","Minor servo Boss component interface is ",m_msBossComp);
	_GET_STRING_ATTRIBUTE("SchedulerInterface","Scheduler component interface is ",m_schedulerComp);
	_GET_STRING_ATTRIBUTE("MeteoInstance","Weather component instance is ",m_meteoInstance);
	m_workingThreadTime*=10;
	m_workingThreadTimeSlice*=10;
	m_collectorThreadTime*=10;
	m_minorServoEnquireMinGap*=10;
}

