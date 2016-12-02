// $Id: Configuration.cpp,v 1.2 2011-05-12 14:14:31 a.orlati Exp $

#include "Configuration.h"

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

CConfiguration::CConfiguration()
{
}

CConfiguration::~CConfiguration()
{
}

void CConfiguration::init(maci::ContainerServices *Services) throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl,ComponentErrors::IRALibraryResourceExImpl)
{
	_GET_STRING_ATTRIBUTE("IPAddress","TCP/IP address is: ",m_sAddress);
	_GET_DWORD_ATTRIBUTE("Port","Port is: ",m_wPort);
	/*_GET_DWORD_ATTRIBUTE("CommandLineTimeout","Time out of the command line is (uSec): ",m_dwCommandLineTimeout);
	_GET_DWORD_ATTRIBUTE("ConnectTimeout","Time out of the command line connection  is (uSec): ",m_dwConnectTimeout);
	_GET_DWORD_ATTRIBUTE("PropertyRefreshTime","The property refresh time is (microseconds): ",m_dwPropertyRefreshTime);
	_GET_STRING_ATTRIBUTE("Configuration","Backend configuration is: ",m_sConfig);
	_GET_DWORD_ATTRIBUTE("TimeTollerance","The time tollerance is  (microseconds): ",m_dwTimeTollerance);	
	_GET_DWORD_ATTRIBUTE("RepetitionCacheTime","Log repetition filter cache time (uSec)",m_dwRepetitionCacheTime);
	_GET_DWORD_ATTRIBUTE("RepetitionExpireTime","Log repetition filter expire time  (uSec)",m_dwRepetitionExpireTime);
	_GET_DWORD_ATTRIBUTE("DataPort","Port of data line is: ",m_wDataPort);
	_GET_STRING_ATTRIBUTE("DataIPAddress","Data line TCP/IP address is: ",	m_sDataAddress);
	_GET_DWORD_ATTRIBUTE("DataLatency","The latency of the backend data line is (uSec)",m_dwDataLatency);
	_GET_DWORD_ATTRIBUTE("SenderSleepTime","The sender thread sleep time is (uSec)",m_dwSenderSleepTime);
	_GET_DWORD_ATTRIBUTE("SenderResponseTime","The sender thread response time is (uSec)",m_dwSenderResponseTime);
	_GET_DWORD_ATTRIBUTE("ControlSleepTime","The control thread sleep time is (uSec)",m_dwControlSleepTime);
	_GET_DWORD_ATTRIBUTE("ControlResponseTime","The control thread response time is (uSec)",m_dwControlResponseTime);
	_GET_DWORD_ATTRIBUTE("BoardsNumber","The number of installed boards is ",m_dwBoardsNumber);
	_GET_DWORD_ATTRIBUTE("DataBufferSize","Size of data packet is (bytes)",m_dwDataBufferSize);*/
}

