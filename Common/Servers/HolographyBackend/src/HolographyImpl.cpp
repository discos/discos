/*******************************************************************************
* ALMA - Atacama Large Millimiter Array
* (c) UNSPECIFIED - FILL IN, 2009 
* 
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
* 
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
* 
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
* "@(#) $Id: HolographyImpl.cpp,v 1.10 2010-10-20 10:28:28 spoppi Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* almamgr  2009-12-07  created 
*/

/************************************************************************
*   NAME
*   
* 
*   SYNOPSIS
*   
* 
*   DESCRIPTION
*
*   FILES
*
*   ENVIRONMENT
*
*   COMMANDS
*
*   RETURN VALUES
*
*   CAUTIONS 
*
*   EXAMPLES
*
*   SEE ALSO
*
*   BUGS   
* 
*------------------------------------------------------------------------
*/
#include "HolographyImpl.h"
//#include "vltPort.h"
#include <Definitions.h>
#include <ComponentErrors.h>
#include <LogFilter.h>
//#include "Common.h"
#include <IRA>
#include <acsThread.h>

// static char *rcsId="@(#) $Id: HolographyImpl.cpp,v 1.10 2010-10-20 10:28:28 spoppi Exp $"; 
// static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */

/*
 * Local Headers 
 */

/* 
 * Signal catching functions  
 */

/* 
 *Local functions  
 */



//using namespace SimpleParser;
//_IRA_LOGFILTER_DECLARE;
 
HolographyImpl::HolographyImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices):
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
	AUTO_TRACE("HolographyImpl::HolographyImpl");
	m_initialized=false;
	m_senderThread=NULL;

}

HolographyImpl::~HolographyImpl() 
{
	AUTO_TRACE("HolographyImpl::~HolographyImpl()");
	// if the initialization failed...clear everything as well. 
	// otherwise it is called by the cleanUp procedure.
	if (!m_initialized) deleteAll();
}


void HolographyImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	m_configuration.init(getContainerServices()); 
	
	m_LogObservedPositions=m_configuration.getLogObservedPosition();
	m_correlator=NULL;



ACS_LOG(LM_FULL_INFO,"HolographyImpl::initialize()",(LM_INFO,"PROPERTY_CREATION"));
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
		m_initialized=true;
// 		m_parser=new CParser<CCommandLine>(10); 
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"HolographyImpl::initialize()");
		
		throw dummy;
	}

}

void HolographyImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("HolographyImpl::execute()");
	ACS_LOG(LM_FULL_INFO,"HolographyImpl::execute()",(LM_INFO,"BACKEND_INITIAL_CONFIGURATION"));
	ACS::Time time;
	try {
		//sets the property defaults....some of them cannot be changed any more (hardware dependent) 
		m_pbackendName->getDevIO()->write(getComponent()->getName(),time);
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,impl,ex,"TotalPowerImpl::execute()");
		throw impl;
	}

}


void HolographyImpl::sendHeader() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx, 
		ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("HolographyImpl::sendHeader()");
	try{

		DWORD sampling_time;
		sampling_time=m_configuration.getSamplingTime(); 
 		
//		m_correlator=getContainerServices()->getDefaultComponent<DXC::DigitalXCorrelator>("IDL:alma/DXC/DigitalXCorrelator:1.0");



		CSenderThread::TSenderParameter *temp;

		m_correlator=getContainerServices()->getDefaultComponent<DXC::DigitalXCorrelator>("IDL:alma/DXC/DigitalXCorrelator:1.0");

       if (m_LogObservedPositions)
 		{
 	 		m_antennaBoss=getContainerServices()->getDefaultComponent<Antenna::AntennaBoss>(ANTENNA_BOSS_INTERFACE);
 	        	m_sender_thread_param.antennaBoss=m_antennaBoss;
 		} 

	
		m_sender_thread_param.sender=this;
		m_sender_thread_param.dxc_correlator=m_correlator;
		temp=&m_sender_thread_param;    // reference to pass to create thread
		if (m_senderThread==NULL) {
			m_senderThread=getContainerServices()->getThreadManager()->create<CSenderThread,CSenderThread::TSenderParameter*>("CORRELATORDATA",temp);
			m_senderThread->setSleepTime(sampling_time*10000); // the sampling time is read from CDB
//			m_senderThread->setSleepTime(0); // set to 0
		        m_senderThread->setResponseTime(50000000);



		}

	
// 	if (CORBA::is_nil(m_antennaBoss))
// 	{
// 		cout <<"ABOSS NOT REFERENCED " << endl;
// 	
// 	} else
// 	{
// 	  	cout <<"ABOSS REFERENCED " << endl;
// 	
// 	}
// 	




	}   
	 catch(acsthreadErrType::acsthreadErrTypeExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"HolographyImpl::initialize()");
		throw _dummy;
	}
	catch(maciErrType::NoDefaultComponentExImpl &ex)
	{
	   _ADD_BACKTRACE(maciErrType::NoDefaultComponentExImpl,Impl,ex,""); throw Impl;

	}

	catch(maciErrType::CannotGetComponentExImpl &ex)
	{
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"HolographyImpl::sendHeader()");
		throw _dummy;	}

	
	  catch(maciErrType::NoPermissionExImpl &ex)
	{
	   	_ADD_BACKTRACE(maciErrType::NoPermissionExImpl,Impl,ex,"");
	}
 	
		catch (maciErrType::maciErrTypeExImpl &ex)
	{

		ACS_LOG(LM_FULL_INFO,"HolographyImpl::sendHeader()",(LM_ERROR
		,"CannotGetComponentExImpl"));
		ex.log(LM_DEBUG);

		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"HolographyImpl::initialize()");
		throw _dummy;	
	
	}

	 catch(...)
	{
		cout<< "unknown except" <<endl;
	} 
	
	



}



void HolographyImpl::sendData(ACS::Time startTime) throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,
			ComponentErrors::ComponentErrorsEx)
{
	double az,el;
	TIMEVALUE now;
	AUTO_TRACE("HolographyImpl::sendData()");
	try {
     //           m_correlator->save_coeff(az,el);
	   IRA::CIRATools::getTime(now);

	  if (startTime!=0 &&  startTime<now.value().value) {
		ACS_LOG(LM_FULL_INFO,"HolographyImpl::sendData()",(LM_WARNING,"START_TIME_ALREADY_ELAPSED"));
	      } else           
	   { 
  
		IRA::CIRATools::getTime(now);		
		if (m_LogObservedPositions)
 		{
//                m_antennaBoss->getObservedHorizontal(now.value().value,0,az,el);

		} else
		{
		 	az=-0.69; 
			el=0.55;
		}

//		m_correlator->save_coord(az,el);
		m_senderThread->resume();
	        ACS_LOG(LM_FULL_INFO,"HolographyImpl::sendData()",(LM_DEBUG,"starting thread"));
	   }
        }
        catch(ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
	catch (BackendsErrors::BackendsErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getBackendsErrorsEx();		
	}
        catch (CORBA::SystemException &ex)
	{
		ACS_LOG(LM_FULL_INFO,"HolographyImpl::sendData()",(LM_ERROR
		,"SystemException"));
		//ex.log(LM_DEBUG);

	}
        catch (...) {
	   ACS_LOG(LM_FULL_INFO,"HolographyImpl::sendData()",(LM_ERROR,"error"));

        }
	

}

void HolographyImpl::sendStop() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,
			ComponentErrors::ComponentErrorsEx)
    {
	double az,el;
	TIMEVALUE now;

	AUTO_TRACE("HolographyImpl::sendStop()");
	m_senderThread->suspend();
		IRA::CIRATools::getTime(now);		
		if (m_LogObservedPositions)
 		{
//			            m_antennaBoss->getRawCoordinates(now.value().value,az,el);
		} else
		{
		 	az=-0.69; 
			el=0.55;
		}
//	m_correlator->save_coord(az,el);
}

void HolographyImpl::terminate() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,
			ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("HolographyImpl::terminate()");

	try {

		getContainerServices()->releaseComponent(m_correlator->name());
	} catch(maciErrType::CannotReleaseComponentExImpl &ex)
	
	{
		ACS_LOG(LM_FULL_INFO,"HolographyImpl::terminate()",(LM_ERROR
		,"CannotReleaseComponentExImpl"));
		ex.log(LM_DEBUG);


	}
	 
	
	
}





void HolographyImpl::initialize(const char * configuration) throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("HolographyImpl::initialize()");


}
// 
void HolographyImpl::aboutToAbort()
{
	AUTO_TRACE("HolographyImpl::aboutToAbort()");
 	deleteAll();

}
void HolographyImpl::cleanUp()
{
	AUTO_TRACE("HolographyImpl::cleanUp()");
		if (m_senderThread!=NULL) {
			m_senderThread->suspend();
			getContainerServices()->getThreadManager()->destroy(m_senderThread);
			m_senderThread=NULL;

		};	CharacteristicComponentImpl::cleanUp();	
	deleteAll();

}

void HolographyImpl::deleteAll()
{
	AUTO_TRACE("HolographyImpl::deleteAll()");
	IRA::CError err;
	if (m_initialized)
	{
		if (m_senderThread!=NULL) {
			m_senderThread->suspend();
			getContainerServices()->getThreadManager()->destroy(m_senderThread);
			m_senderThread=NULL;

		};
	  	
	
	}



}

 CORBA::Boolean  HolographyImpl::command(const char *cmd,CORBA::String_out answer)   throw (CORBA::SystemException)
 {
	 AUTO_TRACE("HolographyImpl::command()");
	 	IRA::CString out;
	 	bool res;


	 	answer=CORBA::string_dup((const char *)"No command allowed");
	 	return res;
 }




ACS::doubleSeq *HolographyImpl::getTpi () throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
//	AUTO_TRACE("HolographyImpl::getTpi()");
	AUTO_TRACE("HolographyImpl::getTpi() Not used");
	ACS::doubleSeq_var tpi=new ACS::doubleSeq;
	tpi->length(1);
	tpi[0]=0;
	

	return tpi._retn();
}

 ACS::doubleSeq * HolographyImpl::getZero () throw (CORBA::SystemException,
 		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
 {
	ACS::doubleSeq_var tpi=new ACS::doubleSeq;
 	AUTO_TRACE("HolographyImpl::getZero() not used");
 	tpi->length(1);
	tpi[0]=0;
	

	return tpi._retn();
}	
 	 

 
void HolographyImpl::setKelvinCountsRatio(const ACS::doubleSeq& ratio, const ACS::doubleSeq& tsys) throw (CORBA::SystemException)
{
	AUTO_TRACE("HolographyImpl::setKelvinCountsRatio() not used");
// 	
}
void HolographyImpl::enableChannels(const ACS::longSeq& enable) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
		BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("HolographyImpl::enableChannels() not used");
			
}

void HolographyImpl::setTime() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
		BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("HolographyImpl::setTime() not used");
 	
}

void HolographyImpl::setAttenuation(CORBA::Long input,CORBA::Double att) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("HolographyImpl::setAttenutation() not used");
	 
}

CORBA::Long HolographyImpl::getInputs(ACS::doubleSeq_out freq,ACS::doubleSeq_out bandWidth,ACS::longSeq_out feed,ACS::longSeq_out ifNumber) throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("HolographyImpl::getInputs() not used");
	return 0;
}

void HolographyImpl::activateNoiseCalibrationSwitching(CORBA::Long interleave) throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	       AUTO_TRACE("HolographyImpl::activateNoiseCalibrationSwitching() not used");
  	
}

//char * HolographyImpl::command(const char *configCommand)  throw (CORBA::SystemException,ManagementErrors::CommandLineErrorEx)
//{
//	AUTO_TRACE("HolographyImpl::command()");
//	IRA::CString out;
//	IRA::CString in;
//	bool error;
	
//	return CORBA::string_dup((const char *)out);
//}

void HolographyImpl::setSection(CORBA::Long input,CORBA::Double freq,CORBA::Double bw,CORBA::Long feed,CORBA::Long pol,CORBA::Double sr,CORBA::Long bins) throw (
				CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("HolographyImpl::setSection() - not used");
	
}

void HolographyImpl::setIntegration(CORBA::Long Integration) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
		BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("HolographyImpl::setIntegration() - not used");
	 	
}

 _PROPERTY_REFERENCE_CPP(HolographyImpl,ACS::ROuLongLong,m_ptime,time);
 _PROPERTY_REFERENCE_CPP(HolographyImpl,ACS::ROstring,m_pbackendName,backendName);
 _PROPERTY_REFERENCE_CPP(HolographyImpl,ACS::ROdoubleSeq,m_pbandWidth,bandWidth);
 _PROPERTY_REFERENCE_CPP(HolographyImpl,ACS::ROdoubleSeq,m_pfrequency,frequency);
 _PROPERTY_REFERENCE_CPP(HolographyImpl,ACS::ROdoubleSeq,m_psampleRate,sampleRate);
 _PROPERTY_REFERENCE_CPP(HolographyImpl,ACS::ROdoubleSeq,m_pattenuation,attenuation);
 _PROPERTY_REFERENCE_CPP(HolographyImpl,ACS::ROlongSeq,m_ppolarization,polarization);
 _PROPERTY_REFERENCE_CPP(HolographyImpl,ACS::ROlongSeq,m_pbins,bins);
 _PROPERTY_REFERENCE_CPP(HolographyImpl,ACS::ROlong,m_pinputsNumber,inputsNumber);
 _PROPERTY_REFERENCE_CPP(HolographyImpl,ACS::ROlong,m_pintegration,integration);
 _PROPERTY_REFERENCE_CPP(HolographyImpl,ACS::ROpattern,m_pstatus,status);
 _PROPERTY_REFERENCE_CPP(HolographyImpl,Management::ROTBoolean,m_pbusy,busy);
_PROPERTY_REFERENCE_CPP(HolographyImpl,ACS::ROlongSeq,m_pfeed,feed);
_PROPERTY_REFERENCE_CPP(HolographyImpl,ACS::ROdoubleSeq,m_ptsys,systemTemperature);
_PROPERTY_REFERENCE_CPP(HolographyImpl,ACS::ROlong,m_psectionsNumber,sectionsNumber);
_PROPERTY_REFERENCE_CPP(HolographyImpl,ACS::ROlongSeq,m_pinputSection,inputSection);
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(HolographyImpl)
