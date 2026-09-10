
#include "Configuration.h"

using namespace IRA;
using namespace DataWriter_private;

#define _GET_DWORD_ATTRIBUTE(ATTRIB,DESCR,FIELD) { \
	DWORD tmpw; \
	if (!CIRATools::getDBValue(Services,ATTRIB,tmpw)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"CConfiguration::Init()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
	} \
	else { \
		FIELD=tmpw; \
		ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %u",tmpw); \
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
// throw (ComponentErrors::CDBAccessExImpl)
void CConfiguration::init(maci::ContainerServices *Services)
{
	_GET_DWORD_ATTRIBUTE("WorkingThreadTime","Sleep time of working thread (uSec)",m_workingThreadTime);
	_GET_DWORD_ATTRIBUTE("WorkingThreadTimeSlice","Time slice of working thread (uSec)",m_workingThreadTimeSlice);
	_GET_DWORD_ATTRIBUTE("RepetitionCacheTime","Log repetition filter cache time (uSec)",m_repetitionCacheTime);
	_GET_DWORD_ATTRIBUTE("RepetitionExpireTime","Log repetition filter expire time  (uSec)",m_repetitionExpireTime);
}

