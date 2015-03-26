/*
 * DFBImpl.cpp
 *
 *  Created on: Mar 26, 2012
 *      Author: spoppi
 */



#include "DFBImpl.h"
//#include "vltPort.h"
#include <Definitions.h>
#include <ComponentErrors.h>
#include <LogFilter.h>
//#include "Common.h"
#include <IRA>
#include <acsThread.h>

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
}Configuration



DFBImpl::DFBImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices):
BulkDataSenderDefaultImpl(CompName,containerServices),
         m_ptime(this),
        m_pbackendName(this),
        m_pbandWidth(this),
        m_pfrequency(this),
        m_psampleRate(this),
        m_pattenuation(this),
        m_ppolarization(this),
        m_pbins(this),
        m_pinputsNumber(this),
        m_pintegration(this),
        m_pstatus(this),
        m_pbusy(this),
        m_pfeed(this),
        m_ptsys(this),
        m_psectionsNumber(this),
        m_pinputSection(this)
        

  {
	AUTO_TRACE("DFBImpl::DFBImpl");


  }

DFBImpl::~DFBImpl()
{
	AUTO_TRACE("DFBImpl::~HolographyImpl()");


}

void DFBImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
     AUTO_TRACE("DFBImpl::initialize()");
     try {
             m_ptime=new ROuLongLong(getContainerServices()->getName()+":time",getComponent());
             m_pbackendName=new ROstring(getContainerServices()->getName()+":backendName",getComponent());
             m_pbandWidth=new ROdoubleSeq(getContainerServices()->getName()+":bandWidth",getComponent());
             m_pfrequency=new ROdoubleSeq(getContainerServices()->getName()+":frequency",getComponent());
             m_psampleRate=new ROdoubleSeq(getContainerServices()->getName()+":sampleRate",getComponent());
             m_pattenuation=new ROdoubleSeq(getContainerServices()->getName()+":attenuation",getComponent());
             m_ppolarization=new ROlongSeq(getContainerServices()->getName()+":polarization",getComponent());
             m_pinputsNumber=new ROlong(getContainerServices()->getName()+":inputsNumber",getComponent());
             m_pbins=new ROlongSeq(getContainerServices()->getName()+":bins",getComponent());
             m_pintegration=new ROlong(getContainerServices()->getName()+":integration",getComponent());
             m_pstatus=new ROpattern(getContainerServices()->getName()+":status",getComponent());
             m_pbusy=new ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>
               (getContainerServices()->getName()+":busy",getComponent());
             m_pfeed=new ROlongSeq(getContainerServices()->getName()+":feed",getComponent());
             m_ptsys=new ROdoubleSeq(getContainerServices()->getName()+":systemTemperature",getComponent());
             m_psectionsNumber=new ROlong(getContainerServices()->getName()+":sectionsNumber",getComponent());
//             m_initialized=true;
//              m_parser=new CParser<CCommandLine>(10);
       
       
        
       
/*         	_GET_STRING_ATTRIBUTE("IPAddress","TCP/IP address is: ",m_sAddress);*/
//          	_GET_DWORD_ATTRIBUTE("Port","Port is: ",m_wPort);


     }
     catch (std::bad_alloc& ex) {
             _EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"HolographyImpConfigurationl::initialize()");

             throw dummy;
     }


}

void DFBImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
    AUTO_TRACE("DFBImpl::execute()");

}

void DFBImpl::sendHeader() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,
		ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("DFBImpl::sendHeader()");

}

void DFBImpl::sendData(ACS::Time startTime) throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,
			ComponentErrors::ComponentErrorsEx)

{
    AUTO_TRACE("DFBImpl::sendData()");

}

void DFBImpl::sendStop() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,
                        ComponentErrors::ComponentErrorsEx)
{
    AUTO_TRACE("DFBImpl::sendStop()");
}

void DFBImpl::terminate() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,
                        ComponentErrors::ComponentErrorsEx){


    AUTO_TRACE("DFBImpl::terminate()");

}

void DFBImpl::initialize(const char * configuration) throw (CORBA::SystemException,
                ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx){


}

void DFBImpl::aboutToAbort()
{
    AUTO_TRACE("DFBImpl::aboutToAbort()");

}
 
void DFBImpl::cleanUp(){
    AUTO_TRACE("DFBImpl::cleanUp()");

}
void DFBImpl::deleteAll(){

    AUTO_TRACE("DFBImpl::deleteAll()");

}


ACS::doubleSeq *DFBImpl::getTpi () throw (CORBA::SystemException,
                ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx){

  AUTO_TRACE("DFBImpl::getTpi()"
);
  ACS::doubleSeq_var tpi=new ACS::doubleSeq;
       tpi->length(1);
      tpi[0]=0;
  return tpi._retn();

}

ACS::doubleSeq * DFBImpl::getZero () throw (CORBA::SystemException,
                ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
 {
        ACS::doubleSeq_var tpi=new ACS::doubleSeq;
        AUTO_TRACE("DFBImpl::getZero() not used");
        tpi->length(1);
        tpi[0]=0;


        return tpi._retn();
}

void DFBImpl::setKelvinCountsRatio(const ACS::doubleSeq& ratio, const ACS::doubleSeq& tsys) throw (CORBA::SystemException)
{
        AUTO_TRACE("DFBImpl::setKelvinCountsRatio() not used");
//
}


void DFBImpl::enableChannels(const ACS::longSeq& enable) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
                BackendsErrors::BackendsErrorsEx)
{
        AUTO_TRACE("DFBImpl::enableChannels() not used");

}
void DFBImpl::setTime() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
                BackendsErrors::BackendsErrorsEx)
{
        AUTO_TRACE("DFBImpl::setTime() not used");

}

void DFBImpl::setAttenuation(CORBA::Long input,CORBA::Double att) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
        AUTO_TRACE("DFBImpl::setAttenutation() not used");

}

CORBA::Long DFBImpl::getInputs(ACS::doubleSeq_out freq,ACS::doubleSeq_out bandWidth,ACS::longSeq_out feed,ACS::longSeq_out ifNumber) throw (CORBA::SystemException,
                ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
        AUTO_TRACE("DFBImpl::getInputs() not used");
        return 0;
}


void DFBImpl::activateNoiseCalibrationSwitching(CORBA::Long interleave) throw (CORBA::SystemException,
                ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
               AUTO_TRACE("DFBImpl::activateNoiseCalibrationSwitching() not used");

}


CORBA::Boolean DFBImpl::command(const char *cmd,CORBA::String_out answer) throw (CORBA::SystemException)
{
          
        AUTO_TRACE("DFBImpl::command()");
        IRA::CString out;
        IRA::CString in;
        bool res;
        res=CORBA::string_dup((const char *)out);
        return res;
}

void DFBImpl::setSection(CORBA::Long input,CORBA::Double freq,CORBA::Double bw,CORBA::Long feed,CORBA::Long pol,CORBA::Double sr,CORBA::Long bins) throw (
                                CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
        AUTO_TRACE("DFBImpl::setSection() - not used");

}

void DFBImpl::setIntegration(CORBA::Long Integration) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
                BackendsErrors::BackendsErrorsEx)
{
        AUTO_TRACE("DFBImpl::setIntegration() - not used");

}

 _PROPERTY_REFERENCE_CPP(DFBImpl,ACS::ROuLongLong,m_ptime,time);
 _PROPERTY_REFERENCE_CPP(DFBImpl,ACS::ROstring,m_pbackendName,backendName);
 _PROPERTY_REFERENCE_CPP(DFBImpl,ACS::ROdoubleSeq,m_pbandWidth,bandWidth);
 _PROPERTY_REFERENCE_CPP(DFBImpl,ACS::ROdoubleSeq,m_pfrequency,frequency);
 _PROPERTY_REFERENCE_CPP(DFBImpl,ACS::ROdoubleSeq,m_psampleRate,sampleRate);
 _PROPERTY_REFERENCE_CPP(DFBImpl,ACS::ROdoubleSeq,m_pattenuation,attenuation);
 _PROPERTY_REFERENCE_CPP(DFBImpl,ACS::ROlongSeq,m_ppolarization,polarization);
 _PROPERTY_REFERENCE_CPP(DFBImpl,ACS::ROlongSeq,m_pbins,bins);
 _PROPERTY_REFERENCE_CPP(DFBImpl,ACS::ROlong,m_pinputsNumber,inputsNumber);
 _PROPERTY_REFERENCE_CPP(DFBImpl,ACS::ROlong,m_pintegration,integration);
 _PROPERTY_REFERENCE_CPP(DFBImpl,ACS::ROpattern,m_pstatus,status);
 _PROPERTY_REFERENCE_CPP(DFBImpl,Management::ROTBoolean,m_pbusy,busy);
_PROPERTY_REFERENCE_CPP(DFBImpl,ACS::ROlongSeq,m_pfeed,feed);
_PROPERTY_REFERENCE_CPP(DFBImpl,
ACS::ROdoubleSeq,m_ptsys,systemTemperature);
_PROPERTY_REFERENCE_CPP(DFBImpl,ACS::ROlong,m_psectionsNumber,sectionsNumber);
_PROPERTY_REFERENCE_CPP(DFBImpl,ACS::ROlongSeq,m_pinputSection,inputSection);
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(DFBImpl)

