// $Id: SenderThread.cpp,v 1.4 2010-10-17 11:01:19 spoppi Exp $

#include "HolographyImpl.h"
#include "SenderThread.h"
#include <LogFilter.h>

_IRA_LOGFILTER_IMPORT;

//CSenderThread::CSenderThread(const ACE_CString& name,HolographyImpl  *holimpl, 
//			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime)


CSenderThread::CSenderThread(const ACE_CString& name,TSenderParameter *param, const ACS::TimeInterval& responseTime,
			const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime)
{
	
    AUTO_TRACE("CSenderThread::CSenderThread()");
     m_dxc_correlator=param->dxc_correlator;
     m_antenna_boss=param->antennaBoss;

}

CSenderThread::~CSenderThread()
{
	AUTO_TRACE("CSenderThread::~CSenderThread()");
}

void CSenderThread::onStart()
{
	AUTO_TRACE("CSenderThread::onStart()");

}

void CSenderThread::onStop()
{
	 AUTO_TRACE("CSenderThread::onStop()");
}
 
void CSenderThread::runLoop()
{

        double azeloff,eloff,raoff,decoff,lonoff,latoff;

	TIMEVALUE now;
	   double az,el;
	az=-99.;  // DUMMY values 
	el=-99.;

	if (CORBA::is_nil(m_antenna_boss))
	{
	
	        cout <<"ABOSS NOT REFERENCED " << endl;
	
	} else
	{
	  	cout <<"ABOSS REFERENCED " << endl;
         	IRA::CIRATools::getTime(now);
                m_antenna_boss->getAllOffsets(azeloff,eloff,raoff,decoff,lonoff,latoff); //   
	        m_antenna_boss->getObservedHorizontal(now.value().value,0,az,el); // get az and el from bos

	}
    // removed 	
	
	IRA::CError err;
	try{

#define PI 3.14159265358979323846
     //   az=az/PI*180.0;   // conversion to degrees made inside the correlator component
     //   
     //   el=el/PI*180.0;  
	m_dxc_correlator->save_coeff(azeloff,eloff); 
	// AUTO_TRACE("CSenderThread::runLoop()");
	} catch (ACSErrTypeFPGACommunication::ACSErrTypeFPGACommunicationExImpl &ex)
	{
	   ACS_LOG(LM_FULL_INFO,"CSenderThread::runLoop()",(LM_ERROR,"DXC_correlator error"));
	   ex.log(LM_DEBUG);

	} catch (ACSErr::ACSbaseExImpl& ex) {
		_ADD_BACKTRACE(ClientErrors::CouldntPerformActionExImpl,impl,ex,"CSenderThread::runLoop()"); 
		 impl.log();
       
	} catch (CORBA::SystemException& ex) {
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"coordinateGrabber::main()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		impl.log();
	}

	 catch(...)
	{
	   ACS_LOG(LM_FULL_INFO,"CSenderThread::runLoop()",(LM_ERROR,"SenderThread runLopp DXC unknown except"));
	
	}

	ACS_LOG(LM_FULL_INFO,"CSenderThread::runLoop()",(LM_DEBUG,"SenderThread runloop"));
	
	
}
