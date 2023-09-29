// $Id: Configuration.cpp,v 1.0 $

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

#define _GET_LONG_ATTRIBUTE(ATTRIB,DESCR,FIELD) { \
	long tmpl; \
	if (!CIRATools::getDBValue(Services,ATTRIB,tmpl)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"CConfiguration::Init()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
	} \
	else { \
		FIELD=tmpl; \
		ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %ld",tmpl); \
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
	_GET_LONG_ATTRIBUTE("addr_1","Board 1 address is ",m_addr_1);
	_GET_LONG_ATTRIBUTE("addr_2","Board 2 address is ",m_addr_2);
	_GET_LONG_ATTRIBUTE("addr_3","Board 3 address is ",m_addr_3);
	_GET_LONG_ATTRIBUTE("addr_4","Board 4 address is ",m_addr_4);
	_GET_STRING_ATTRIBUTE("DBESM_IPAddress","DBESM IP Address is",m_DBESM_IPAddress);
	_GET_DWORD_ATTRIBUTE("DBESM_Port","DBESM TCP Port is",m_DBESM_Port); 
   
   m_regs_1.length(10);
   m_regs_2.length(10);
	m_regs_3.length(10);
	m_regs_4.length(10);
  		for (int i=0; i<10; i++){
      m_regs_1[i] = 0;
      m_regs_2[i] = 0;
      m_regs_3[i] = 0;
      m_regs_4[i] = 0; } 
       
   m_atts_1.length(17);
   m_atts_2.length(17);
	m_atts_3.length(17);
	m_atts_4.length(17);       
      for (int i=0; i<17; i++){
      m_atts_1[i] = 0.0;
      m_atts_2[i] = 0.0;
      m_atts_3[i] = 0.0;
      m_atts_4[i] = 0.0; }
      
   m_amps_1.length(10);
   m_amps_2.length(10);
	m_amps_3.length(10);
	m_amps_4.length(10);
	m_eqs_1.length(10);
   m_eqs_2.length(10);
	m_eqs_3.length(10);
	m_eqs_4.length(10);      
  		for (int i=0; i<10; i++){
      m_amps_1[i] = 0;
      m_amps_2[i] = 0;
      m_amps_3[i] = 0;
      m_amps_4[i] = 0;
      m_eqs_1[i] = 0;
      m_eqs_2[i] = 0;
      m_eqs_3[i] = 0;
      m_eqs_4[i] = 0; } 
      
   m_bpfs_1.length(11);
   m_bpfs_2.length(11);
	m_bpfs_3.length(11);
	m_bpfs_4.length(11);
      for (int i=0; i<11; i++){
      m_bpfs_1[i] = 0;
      m_bpfs_2[i] = 0;
      m_bpfs_3[i] = 0;
      m_bpfs_4[i] = 0; }

   m_volts_1.length(2);
   m_volts_2.length(2);
	m_volts_3.length(2);
	m_volts_4.length(2);
        for (int i = 0; i < 2; ++i){
        m_volts_1[i] = 0.0;
        m_volts_2[i] = 0.0;
        m_volts_3[i] = 0.0;
        m_volts_4[i] = 0.0;
                         }
                         
   m_temps_1 = 0.0;
   m_temps_2 = 0.0;
   m_temps_3 = 0.0;
   m_temps_4 = 0.0;                          

}

void CConfiguration::init() throw (ComponentErrors::CDBAccessExImpl)
{
	m_addr_1 = 15;
	m_addr_2 = 14;
	m_addr_3 = 13;
	m_addr_4 = 12;
   m_DBESM_IPAddress = "127.0.0.1";
	m_DBESM_Port = 11111;
	
	for (int i = 0; i < 10; ++i)
    {
        m_regs_1[i] = 0;
        m_regs_2[i] = 0;
        m_regs_3[i] = 0;
        m_regs_4[i] = 0;
        m_amps_1[i] = 0;
        m_amps_2[i] = 0;
        m_amps_3[i] = 0;
        m_amps_4[i] = 0;
        m_eqs_1[i] = 0;
        m_eqs_2[i] = 0;
        m_eqs_3[i] = 0;
        m_eqs_4[i] = 0;
    }
    
   for (int i=0; i<11; i++)
     {
      m_bpfs_1[i] = 0;
      m_bpfs_2[i] = 0;
      m_bpfs_3[i] = 0;
      m_bpfs_4[i] = 0; 
     }      
    
   for (int i = 0; i < 17; ++i)
       {
        m_atts_1[i] = 0.0;
        m_atts_2[i] = 0.0;
        m_atts_3[i] = 0.0;
        m_atts_4[i] = 0.0;
       }
   for (int i = 0; i < 2; ++i)
       {
        m_volts_1[i] = 0.0;
        m_volts_2[i] = 0.0;
        m_volts_3[i] = 0.0;
        m_volts_4[i] = 0.0;
       }
                         
    m_temps_1 = 0.0;
    m_temps_2 = 0.0;
    m_temps_3 = 0.0;
    m_temps_4 = 0.0;  
    
}
