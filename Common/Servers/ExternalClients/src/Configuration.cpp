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
	// get IP and port number from the database.	
	_GET_STRING_ATTRIBUTE("IPAddress","IP Address:",m_sSocketServerAddress,"");
	_GET_DWORD_ATTRIBUTE("Port","Port:",m_wPort,"");
    _GET_DWORD_ATTRIBUTE("ReceiveTimeout","Receive timeout (uSec):",m_dwreceiveTimeout,"");
	_GET_DWORD_ATTRIBUTE("ControlThreadPeriod","Control thread period  (100ns unit):",m_dwcontrolThreadPeriod,"");
    _GET_STRING_ATTRIBUTE("SuperVisor","Super Visor:",m_sSuperVisor,"");
}
