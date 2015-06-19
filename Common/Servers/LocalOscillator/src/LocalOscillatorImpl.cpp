#include "LocalOscillatorImpl.h"
#include <Definitions.h>
#include <maciContainerServices.h>

#define MULTIPLY 1e6


#define _GET_STRING_ATTRIBUTE(ATTRIB,DESCR,FIELD) { \
	CString tmps; \
	if (!CIRATools::getDBValue(Services,ATTRIB,tmps)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"::LocalOscillatorImpl::Init()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
	} \
	else { \
		FIELD=tmps; \
		ACS_DEBUG_PARAM("LocalOscillatorImpl::Init()",DESCR" %s",(const char*)tmps); \
	} \
}



#define GET_PROPERTY_REFERENCE(TYPE,PROPERTY,PROPERTYNAME) TYPE##_ptr LocalOscillatorImpl::PROPERTYNAME() throw (CORBA::SystemException) \
{ \
	if (PROPERTY==0) return TYPE::_nil(); \
	TYPE##_var tmp=TYPE::_narrow(PROPERTY->getCORBAReference()); \
	return tmp._retn(); \
} 

LocalOscillatorImpl::LocalOscillatorImpl(const ACE_CString &name,maci::ContainerServices * containerServices) :
			CharacteristicComponentImpl(name,containerServices),
		m_frequency(this),m_amplitude(this),m_isLocked(this)
{

 	AUTO_TRACE("LocalOscillatorImpl::LocalOscillatorImpl");
 	m_gpibonline=false;
 //	cout << "name:" <<name.c_str() << endl;

}

LocalOscillatorImpl::~LocalOscillatorImpl()
{
	 AUTO_TRACE("LocalOscillatorImpl::~LocalOscillatorImpl");


}

void LocalOscillatorImpl::cleanUp() throw (ACSErr::ACSbaseExImpl)
{

	try {
		CSecAreaResourceWrapper<CommandLine> line=m_commandLine->Get();
		if (m_gpibonline) line->disconnect();
		m_gpibonline=false;

	}
	catch (GPIBException& ex)
	{
	     ACS_LOG(LM_FULL_INFO,"LocalOscillatorImpl::cleanUp()",(LM_DEBUG,"LocalOscillatorImpl::cleanUp() %s",ex.what()));
	 	 _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,impl,"LocalOscillatorImpl::cleanUp()");
	 //			impl.setReason("Could not set local oscillator");
	 	 impl.log(LM_DEBUG);
	 	 throw impl.getReceiversErrorsEx();
	}

	CharacteristicComponentImpl::cleanUp();
	AUTO_TRACE("LocalOscillatorImpl::cleanUp");
	CSecAreaResourceWrapper<CommandLine> line=m_commandLine->Get();

}
void LocalOscillatorImpl::aboutToAbort() throw (ACSErr::ACSbaseExImpl)
{

	AUTO_TRACE("LocalOscillatorImpl::aboutToAbort()");
	cleanUp();

}




void LocalOscillatorImpl::initialize() throw (ACSErr::ACSbaseExImpl) 
{

	CommandLine* line;

	AUTO_TRACE("LocalOscillatorImpl::initialize()");
	try {

		 line=new CommandLine();
	     m_commandLine=new CSecureArea<CommandLine>(line);
	     m_frequency= new  ROdouble(getContainerServices()->getName()+":frequency",getComponent(),new DevIOfrequency(m_commandLine),true);
	     m_amplitude= new ROdouble(getContainerServices()->getName()+":amplitude",getComponent(),new DevIOamplitude(m_commandLine),true);
	     m_isLocked= new ROlong(getContainerServices()->getName()+":isLocked",getComponent(),new DevIOislocked(m_commandLine,this),true);
	}
	 catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,_dummy,E,"LocalOscillatorImpl::initialize()");
			throw _dummy;
    } catch (std::bad_alloc& ex) {
			_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"LocalOscillatorImpl::initialize()");
			throw dummy;
    }




	try {

		unsigned  int GPIBBOARD, PRIMARYADDRESS,SECONDARYADDRESS,TIMEOUT;
		string reply;

		if  ((CIRATools::getDBValue(getContainerServices(),"GPIBBoard",GPIBBOARD)) &&
			   (CIRATools::getDBValue(getContainerServices(),"PrimaryAddress",PRIMARYADDRESS)) &&
			   (CIRATools::getDBValue(getContainerServices(),"SecondaryAddress",SECONDARYADDRESS))&&
			   (CIRATools::getDBValue(getContainerServices(),"Timeout",TIMEOUT)))
		  	  {
					  CUSTOM_LOG(LM_FULL_INFO,"LocalOscillatorImpl::initialize()",(LM_NOTICE,"GPIBBoard %d,Primary address %d, Secondary address %d, Timeout %d",GPIBBOARD, PRIMARYADDRESS,SECONDARYADDRESS,TIMEOUT));


		  	  } else
		  	  {
		  		CUSTOM_LOG(LM_FULL_INFO,"LocalOscillatorImpl::initialize()",(LM_ERROR,"Error getting GPIB setup params  from CDB" ));
			  		  _EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"LocalOscillatorImpl::initialize()");
			  		  dummy.log(LM_DEBUG);
			  		  throw dummy;
		  	  }



	   CSecAreaResourceWrapper<CommandLine> line=m_commandLine->Get();
	   line->configure(GPIBBOARD,PRIMARYADDRESS,SECONDARYADDRESS,T10s,1,0);
	   line->init(reply); // get the identification string from the device.
	   CUSTOM_LOG(LM_FULL_INFO,"LocalOscillatorImpl::initialize()",(LM_NOTICE,"GPIB Device %s",reply.c_str()));
	   m_gpibonline=true;



	 } catch (GPIBException& ex)
	 {
		 _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,dummy,"LocalOscillatorImpl::set");
	 	 dummy.log(LM_DEBUG);
	 	CUSTOM_LOG(LM_FULL_INFO,"LocalOscillatorImpl::initialize()",(LM_DEBUG,"LocalOscillatorImpl::initialize() %s",ex.what()));

	 }



	
}

void LocalOscillatorImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	double freq,ampl;
	
 	AUTO_TRACE("LocalOscillatorImpl::execute()");
 	try{
 		get(ampl,freq);  // get the frequency  and amplitude and set the
 						// values to be used as comparison to check if
 					    // the local oscillator is locked or not.

 		m_freq=freq;
 		m_ampl=ampl;


 	} 	catch (ACSErr::ACSbaseExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,impl,ex,"LocalOscillatorImpl::execute()");
		throw impl;
	}


}

void LocalOscillatorImpl::set(CORBA::Double rf_ampl, CORBA::Double rf_freq) throw (CORBA::SystemException,ReceiversErrors::ReceiversErrorsEx)
{
 	AUTO_TRACE("LocalOscillatorImpl::set(CORBA::Double rf_ampl, CORBA::Double rf_freq)");
 	try {
 		CSecAreaResourceWrapper<CommandLine> line=m_commandLine->Get();

    	 line->setFreq(rf_freq*MULTIPLY); // set frequency
 		 line->setPower(rf_ampl); // set  amplitude
         CUSTOM_LOG(LM_FULL_INFO,"LocalOscillatorImpl::set()",(LM_NOTICE,"SYNTHESIZER SET to freq: %f power:%f ",rf_freq,rf_ampl));


 		 m_freq=rf_freq;
 		 m_ampl=rf_ampl;

 		 } catch (GPIBException& ex)
 		 {
 			CUSTOM_LOG(LM_FULL_INFO,"LocalOscillatorImpl::set()",(LM_DEBUG,"LocalOscillatorImpl::set() %s",ex.what()));
 			 _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,impl,"LocalOscillatorImpl::set()");
 //			impl.setReason("Could not set local oscillator");
 			impl.log(LM_DEBUG);
 			throw impl.getReceiversErrorsEx();


 		 }


}

void LocalOscillatorImpl::get(CORBA::Double& rf_ampl, CORBA::Double& rf_freq) throw (CORBA::SystemException,ReceiversErrors::ReceiversErrorsEx)
{
 	AUTO_TRACE("LocalOscillatorImpl::get(CORBA::Double rf_ampl, CORBA::Double rf_freq)");

 	try
 	{
     		CSecAreaResourceWrapper<CommandLine> line=m_commandLine->Get();
 	 		line->getFreq(rf_freq); // get frequency
 	 		line->getPower(rf_ampl); // get  amplitude
 	        CUSTOM_LOG(LM_FULL_INFO,"LocalOscillatorImpl::get()",(LM_NOTICE,"SYNTHESIZER READ freq: %f power:%f ",rf_freq,rf_ampl));

// 	 		if (rf_freq !=m_freq)
 	       // 	        	 		{
 	       // 		    ACS::Time timestamp;
 	       //		m_isLocked->getDevIO()->write(1,timestamp);

 	       //	} else
 	       //	{
 	       //    ACS::Time timestamp;
 	       //		m_isLocked->getDevIO()->write(0,timestamp); //
 	       //		CUSTOM_LOG(LM_FULL_INFO,"LocalOscillatorImpl::get()",(LM_WARNING,"LocalOscillatorImpl read %f, expected %f",rf_freq,m_freq));
 	       //	}

 	 } catch (GPIBException& ex)
 	 {
 		CUSTOM_LOG(LM_FULL_INFO,"LocalOscillatorImpl::get()",(LM_DEBUG,"LocalOscillatorImpl::get() %s",ex.what()));
            _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,impl,"LocalOscillatorImpl::get");
 			impl.log(LM_DEBUG);
 			throw impl.getReceiversErrorsEx();
 	 } catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ReceiversErrors::LocalOscillatorErrorExImpl,dummy,E,"LocalOscillatorImpl::get()");
			throw dummy.getReceiversErrorsEx();
 	 }

}


void LocalOscillatorImpl::rfon() throw (CORBA::SystemException,ReceiversErrors::ReceiversErrorsEx)
{
	
 	AUTO_TRACE("LocalOscillatorImpl::rfon()");
 	try {
 	     		CSecAreaResourceWrapper<CommandLine> line=m_commandLine->Get();

 	     		line->rfOn();
 	     		CUSTOM_LOG(LM_FULL_INFO,"LocalOscillatorImpl::rfon()",(LM_NOTICE,"RF Signal ON " ));

 	 	 		 } catch (GPIBException& ex)
 	 	 		 {
 	 	 			 ACS_LOG(LM_FULL_INFO,"LocalOscillatorImpl::rfon()",(LM_DEBUG,"LocalOscillatorImpl::rfon() %s",ex.what()));

 	 	 			 _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,impl,"LocalOscillatorImpl::rfon()");
 	  	 			impl.log(LM_DEBUG);
 	 	 			throw impl.getReceiversErrorsEx();


 	 	 		 }


 

}

void LocalOscillatorImpl::rfoff() throw (CORBA::SystemException,ReceiversErrors::ReceiversErrorsEx)
{
	
 	AUTO_TRACE("LocalOscillatorImpl::rfoff()");
  	try {
 	     		CSecAreaResourceWrapper<CommandLine> line=m_commandLine->Get();

 	     		line->rfOff();
	     		CUSTOM_LOG(LM_FULL_INFO,"LocalOscillatorImpl::rfOff()",(LM_NOTICE,"RF Signal OFF " ));

 	 	 		 } catch (GPIBException& ex)
 	 	 		 {
 	 	 			 ACS_LOG(LM_FULL_INFO,"LocalOscillatorImpl::rfOff()",(LM_DEBUG,"LocalOscillatorImpl::rfOff() %s",ex.what()));

 	 	 			 _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,impl,"LocalOscillatorImpl::rfOff()");
 	  	 			impl.log(LM_DEBUG);
 	 	 			throw impl.getReceiversErrorsEx();


 	 	 		 }

}

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_frequency,frequency);
GET_PROPERTY_REFERENCE(ACS::ROdouble,m_amplitude,amplitude);
GET_PROPERTY_REFERENCE(ACS::ROlong,m_isLocked,isLocked);


#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(LocalOscillatorImpl)
