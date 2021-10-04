#include "MedicinaKBandDualFConf.h"

using namespace IRA;

#define _GET_DWORD_ATTRIBUTE(ATTRIB,DESCR,FIELD,NAME) { \
    DWORD tmpw; \
    if (!CIRATools::getDBValue(Services,ATTRIB,tmpw,"alma/",NAME)) { \
        _EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"CConfiguration::Init()"); \
        dummy.setFieldName(ATTRIB); \
        throw dummy; \
    } \
    else { \
        FIELD=tmpw; \
        ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %u",tmpw); \
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

CKBandConfiguration::CKBandConfiguration()
{
	m_vertexIPAddress=m_vertexCommand="";
	m_vertexPort=0;	
}

CKBandConfiguration::~CKBandConfiguration()
{}

void CKBandConfiguration::init(maci::ContainerServices *Services) throw (
        ComponentErrors::CDBAccessExImpl,
        ComponentErrors::MemoryAllocationExImpl, 
        ReceiversErrors::ModeErrorExImpl
        )
{
	 _GET_STRING_ATTRIBUTE("VertexIPAddress","Vertex IP address:",m_vertexIPAddress,"");
    _GET_STRING_ATTRIBUTE("VertexCommand","Command to Vertex control:",m_vertexCommand,"");
    _GET_DWORD_ATTRIBUTE("VertexPort","Dewar port:",m_vertexPort,"");
}