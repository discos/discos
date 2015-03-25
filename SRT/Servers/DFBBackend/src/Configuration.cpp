/*
 * Configuration.cpp
 *
 *  Created on: Jul 26, 2013
 *      Author: spoppi
 */

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
        m_configurationTable=NULL;
}

CConfiguration::~CConfiguration()
{
        if (m_configurationTable!=NULL) {
                m_configurationTable->closeTable();
                delete m_configurationTable;
        }
}


void CConfiguration::init(maci::ContainerServices *Services) throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl,ComponentErrors::IRALibraryResourceExImpl)
{
        _GET_STRING_ATTRIBUTE("IPAddress","TCP/IP address is: ",m_sAddress);
        _GET_DWORD_ATTRIBUTE("Port","Port is: ",m_wPort);

        // read the configurations
        IRA::CError error;
        try {
                m_configurationTable=(CDBTable *)new CDBTable(Services,"Configuration","DataBlock/TotalPower");
        }
        catch (std::bad_alloc& ex) {
                _EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"TotalPowerImpl::initialize()");
                throw dummy;
        }
        error.Reset();
        if (!error.isNoError()) {
                _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,error);
                dummy.setCode(error.getErrorCode());
                dummy.setDescription((const char*)error.getDescription());
                throw dummy;
        }
        if (!m_configurationTable->openTable(error))    {
                _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
                throw dummy;
        }
        
}
