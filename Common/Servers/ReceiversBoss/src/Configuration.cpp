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

#define MAX_RECV_NUMBER 32

CConfiguration::CConfiguration()
{
	m_receiver=NULL;
	m_receiverNum=0;
}

CConfiguration::~CConfiguration()
{
	if (m_receiver) {
		delete []m_receiver;
	}
}

void CConfiguration::init(maci::ContainerServices *Services) throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl)
{
	IRA::CString strVal;
	IRA::CString fieldPath;
	// read component configuration
	try {
		m_receiver=new TReceiver[MAX_RECV_NUMBER];
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::init()");
		throw dummy;
	}
	for(;;) {
		if (m_receiverNum==0) fieldPath="RECEIVERS/Boss/AvailableReceiver";
		else fieldPath.Format("RECEIVERS/Boss/AvailableReceiver%d",m_receiverNum);
		if (!CIRATools::getDBValue(Services,"Code",strVal,"alma/",fieldPath)) {
			break;
		}
		m_receiver[m_receiverNum].code=strVal;
		if (!CIRATools::getDBValue(Services,"Derotator",strVal,"alma/",fieldPath)) {
			break;
		}
		strVal.MakeUpper();
		m_receiver[m_receiverNum].derotator=strVal;
		if (!CIRATools::getDBValue(Services,"Component",strVal,"alma/",fieldPath)) {
			break;
		}
		m_receiver[m_receiverNum].component=strVal;
		m_receiverNum++;
	}
	ACS_DEBUG_PARAM("CConfiguration::Init()","Total available receivers: %d",m_receiverNum);
	_GET_DWORD_ATTRIBUTE("RepetitionCacheTime","RepetitionCacheTime (uSec):",m_repetitionCacheTime,"");
	_GET_DWORD_ATTRIBUTE("RepetitionExpireTime","RepetitionExpireTime (uSec):",m_expireCacheTime,"");
	_GET_DWORD_ATTRIBUTE("StatusPersistenceTime","StatusPersistenceTime (uSec):",m_statusPersistenceTime,"");
	_GET_DWORD_ATTRIBUTE("PropertiesUpdateTime","PropertiesUpdateTime (uSec):",m_propertiesUpdateTime,"");
	_GET_STRING_ATTRIBUTE("DewarPositionerInterface","DewarPositionerInterface:",m_dewarPositionerInterface,"");
}

bool CConfiguration::getReceiver(const IRA::CString& code,IRA::CString& component,bool& derotator) const
{
	for(WORD j=0;j<m_receiverNum;j++) {
		if (code==m_receiver[j].code) {
			component=m_receiver[j].component;
			if (m_receiver[j].derotator=="TRUE") derotator=true;
			else derotator=false;
			return true;
		}
	}
	return false;
}

