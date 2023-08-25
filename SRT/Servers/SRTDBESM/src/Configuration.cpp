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
		ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %s",(const char*)tmps); \
	} \
}

#define _GET_STRING_ATTRIBUTE_FROM_DOUBLESEQ(ATTRIB,DESCR,FIELD) { \
	CString tmp_str; \
	if (CIRATools::doubleSeqToStr(FIELD,tmp_str," ")) { \
		_GET_STRING_ATTRIBUTE(ATTRIB,DESCR,FIELD); \
	} \
	else { \
		ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %s",(const char*)tmp_str); \
	} \
}

#define _GET_STRING_ATTRIBUTE_FROM_LONGSEQ(ATTRIB,DESCR,FIELD) { \
	CString tmp_str; \
	if (CIRATools::longSeqToStr(FIELD,tmp_str," ")) { \
		_GET_STRING_ATTRIBUTE(ATTRIB,DESCR,FIELD); \
	} \
	else { \
		ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %s",(const char*)tmp_str); \
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
	_GET_LONG_ATTRIBUTE("addr_1","Board 1 address is ",m_addr_1);   //addresses are in fact of short type
	_GET_LONG_ATTRIBUTE("addr_2","Board 2 address is ",m_addr_2);
	_GET_LONG_ATTRIBUTE("addr_3","Board 3 address is ",m_addr_3);
	_GET_LONG_ATTRIBUTE("addr_4","Board 4 address is ",m_addr_4);
/*	
	_GET_STRING_ATTRIBUTE_FROM_LONGSEQ("regs_1","Board 1 registers values are ",m_regs_1);
	_GET_STRING_ATTRIBUTE_FROM_LONGSEQ("regs_2","Board 2 registers values are ",m_regs_2);
	_GET_STRING_ATTRIBUTE_FROM_LONGSEQ("regs_3","Board 3 registers values are ",m_regs_3);
	_GET_STRING_ATTRIBUTE_FROM_LONGSEQ("regs_4","Board 4 registers values are ",m_regs_4);
	_GET_STRING_ATTRIBUTE_FROM_DOUBLESEQ("atts_1","Board 1 attenuators values are ",m_atts_1);
	_GET_STRING_ATTRIBUTE_FROM_DOUBLESEQ("atts_2","Board 2 attenuators values are ",m_atts_2);
	_GET_STRING_ATTRIBUTE_FROM_DOUBLESEQ("atts_3","Board 3 attenuators values are ",m_atts_3);
	_GET_STRING_ATTRIBUTE_FROM_DOUBLESEQ("atts_4","Board 4 attenuators values are ",m_atts_4);
//	_GET_STRING_ATTRIBUTE("SolarSystemBody","Generator for bodies of the Solar System  is ",m_solarSystemBodyInterface);
//	_GET_STRING_ATTRIBUTE("Satellite","Generator for Satellite  is ",m_satelliteInterface);
	_GET_STRING_ATTRIBUTE_FROM_LONGSEQ("amps_1","Board 1 amplifiers values are ",m_amps_1);
	_GET_STRING_ATTRIBUTE_FROM_LONGSEQ("amps_2","Board 2 amplifiers values are ",m_amps_2);
	_GET_STRING_ATTRIBUTE_FROM_LONGSEQ("amps_3","Board 3 amplifiers values are ",m_amps_3);
	_GET_STRING_ATTRIBUTE_FROM_LONGSEQ("amps_4","Board 4 amplifiers values are ",m_amps_4);
//	_GET_DWORD_ATTRIBUTE("MaxPointNumber","Maximum points in the trajectory",m_maxPointNumber);
//	_GET_DWORD_ATTRIBUTE("GapTime","Gap time between points in trajectory  (uSec)",m_gapTime);
	_GET_STRING_ATTRIBUTE_FROM_LONGSEQ("eqs_1","Board 1 equalizers values are ",m_eqs_1);
	_GET_STRING_ATTRIBUTE_FROM_LONGSEQ("eqs_2","Board 2 equalizers values are ",m_eqs_2);
	_GET_STRING_ATTRIBUTE_FROM_LONGSEQ("eqs_3","Board 3 equalizers values are ",m_eqs_3);
	_GET_STRING_ATTRIBUTE_FROM_LONGSEQ("eqs_4","Board 4 equalizers values are ",m_eqs_4);
//	_GET_DOUBLE_ATTRIBUTE_E("MinElevationAvoidance","Suggested lower elevation limit  for source observation(degrees):",m_minElevationAvoidance,"DataBlock/Mount");
//	_GET_DOUBLE_ATTRIBUTE_E("MaxElevationAvoidance","Suggested upper elevation limit  for source observation(degrees):",m_maxElevationAvoidance,"DataBlock/Mount");
   _GET_STRING_ATTRIBUTE_FROM_LONGSEQ("bpfs_1","Board 1 bpfs values are ",m_bpfs_1);
	_GET_STRING_ATTRIBUTE_FROM_LONGSEQ("bpfs_2","Board 2 bpfs values are ",m_bpfs_2);
	_GET_STRING_ATTRIBUTE_FROM_LONGSEQ("bpfs_3","Board 3 bpfs values are ",m_bpfs_3);
	_GET_STRING_ATTRIBUTE_FROM_LONGSEQ("bpfs_4","Board 4 bpfs values are ",m_bpfs_4);
//	_GET_DOUBLE_ATTRIBUTE("CutOffElevation","The cut off elevation is (degrees):",m_cutOffElevation);
//	_GET_STRING_ATTRIBUTE("SkydipElevationRange","The skydip elevation range is (degrees):",temp);
   _GET_STRING_ATTRIBUTE_FROM_DOUBLESEQ("volts_1","Board 1 voltages are ",m_volts_1);
	_GET_STRING_ATTRIBUTE_FROM_DOUBLESEQ("volts_2","Board 2 voltages are ",m_volts_2);
	_GET_STRING_ATTRIBUTE_FROM_DOUBLESEQ("volts_3","Board 3 voltages are ",m_volts_3);
	_GET_STRING_ATTRIBUTE_FROM_DOUBLESEQ("volts_4","Board 4 voltages are ",m_volts_4);
	
	_GET_STRING_ATTRIBUTE_FROM_DOUBLESEQ("temps_1","Board 1 temperatures are ",m_temps_1);
	_GET_STRING_ATTRIBUTE_FROM_DOUBLESEQ("temps_2","Board 2 temperatures are ",m_temps_2);
	_GET_STRING_ATTRIBUTE_FROM_DOUBLESEQ("temps_3","Board 3 temperatures are ",m_temps_3);
	_GET_STRING_ATTRIBUTE_FROM_DOUBLESEQ("temps_4","Board 4 temperatures are ",m_temps_4);
	
	_GET_STRING_ATTRIBUTE("DBESM IP Address","DBESM IP Address is",m_DBESM_IPAddress);
	_GET_DWORD_ATTRIBUTE("DBESM TCP Port","DBESM TCP Port is",m_DBESM_Port);
*/	

// no other attributes to add at the moment
}

void CConfiguration::init() throw (ComponentErrors::CDBAccessExImpl)
{
	m_addr_1 = 15;
	m_addr_2 = 14;
	m_addr_3 = 13;
	m_addr_4 = 12;
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
        m_temps_1[i] = 0.0;
        m_temps_2[i] = 0.0;
        m_temps_3[i] = 0.0;
        m_temps_4[i] = 0.0;
    }
    /*
   m_regs_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	m_regs_2 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	m_regs_3 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	m_regs_4 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	m_atts_1 = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	m_atts_2 = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	m_atts_3 = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
   m_atts_4 = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
   m_amps_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   m_amps_2 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   m_amps_3 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   m_amps_4 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   m_eqs_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   m_eqs_2 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   m_eqs_3 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   m_eqs_4 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   m_bpfs_1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   m_bpfs_2 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   m_bpfs_3 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   m_bpfs_4 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   m_volts_1 = {0.0, 0.0};
   m_volts_2 = {0.0, 0.0};
   m_volts_3 = {0.0, 0.0};
   m_volts_4 = {0.0, 0.0};
   m_temps_1 = {0.0, 0.0};
   m_temps_2 = {0.0, 0.0};
   m_temps_3 = {0.0, 0.0};
   m_temps_4 = {0.0, 0.0};
   */
   m_DBESM_IPAddress = "127.0.0.1";
	m_DBESM_Port = 11111;
}
