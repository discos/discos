#include "LocalOscillatorImpl.h"
#include <Definitions.h>

using namespace baci;

#define GET_PROPERTY_REFERENCE(TYPE,PROPERTY,PROPERTYNAME) TYPE##_ptr LocalOscillatorImpl::PROPERTYNAME() throw (CORBA::SystemException) \
{ \
	if (PROPERTY==0) return TYPE::_nil(); \
	TYPE##_var tmp=TYPE::_narrow(PROPERTY->getCORBAReference()); \
	return tmp._retn(); \
} 

LocalOscillatorImpl::LocalOscillatorImpl(const ACE_CString &name,			     maci::ContainerServices * containerServices) :
			CharacteristicComponentImpl(name,containerServices),
		m_frequency(this),m_amplitude(this),m_isLocked(this)
{

 	AUTO_TRACE("LocalOscillatorImpl::LocalOscillatorImpl");


}

LocalOscillatorImpl::~LocalOscillatorImpl()
{
	 AUTO_TRACE("LocalOscillatorImpl::~LocalOscillatorImpl");


}

void LocalOscillatorImpl::cleanUp() throw (ACSErr::ACSbaseExImpl)
{
	CharacteristicComponentImpl::cleanUp();
	AUTO_TRACE("LocalOscillatorImpl::cleanUp");

}
void LocalOscillatorImpl::aboutToAbort() throw (ACSErr::ACSbaseExImpl)
{

	AUTO_TRACE("LocalOscillatorImpl::aboutToAbort()");
	cleanUp();

}

void LocalOscillatorImpl::initialize() throw (ACSErr::ACSbaseExImpl) 
{


	AUTO_TRACE("LocalOscillatorImpl::initialize()");
	try {
	     m_frequency= new  ROdouble(getContainerServices()->getName()+":frequency",getComponent());
	     m_amplitude= new ROdouble(getContainerServices()->getName()+":amplitude",getComponent());
	     m_isLocked= new ROlong(getContainerServices()->getName()+":isLocked",getComponent());
	}
	 catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,_dummy,E,"LocalOscillatorImpl::initialize()");
			throw _dummy;
		}
	m_line =new CommandLine();



	try {

	   m_line->configure(GPIBBOARD,PRIMARYADDRESS,SECONDARYADDRESS,TIMEOUT,1,0);
	   

	 } catch (GPIBException& ex)
	 {
			 _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,dummy,"LocalOscillatorImpl::set");

	 	 dummy.log(LM_DEBUG);
		 ACS_LOG(LM_FULL_INFO,"LocalOscillatorImpl::initialize()",(LM_DEBUG,"LocalOscillatorImpl::initialize() %s",ex.what()));

	 }



	
}

void LocalOscillatorImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	
 	AUTO_TRACE("LocalOscillatorImpl::initialize()");

 

}

void LocalOscillatorImpl::set(CORBA::Double rf_ampl, CORBA::Double rf_freq) throw (CORBA::SystemException,ReceiversErrors::ReceiversErrorsEx)
{
 	AUTO_TRACE("LocalOscillatorImpl::set(CORBA::Double rf_ampl, CORBA::Double rf_freq)");
 	try {

 		m_line->setFreq(rf_freq); // set frequency
 		m_line->setPower(rf_ampl); // set  amplitude
 		 } catch (GPIBException& ex)
 		 {
 			 ACS_LOG(LM_FULL_INFO,"LocalOscillatorImpl::set()",(LM_DEBUG,"LocalOscillatorImpl::initialize() %s",ex.what()));
 			 _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,impl,"LocalOscillatorImpl::set");
 //			impl.setReason("Could not set local oscillator");
 			impl.log(LM_DEBUG);
 			throw impl.getReceiversErrorsEx();


 		 }


}

void LocalOscillatorImpl::get(CORBA::Double& rf_ampl, CORBA::Double& rf_freq) throw (CORBA::SystemException,ReceiversErrors::ReceiversErrorsEx)
{
 	AUTO_TRACE("LocalOscillatorImpl::get(CORBA::Double rf_ampl, CORBA::Double rf_freq)");
 	try {

 	 		m_line->getFreq(rf_freq); // set frequency
 	 		m_line->getPower(rf_ampl); // set  amplitude
 	 		 } catch (GPIBException& ex)
 	 		 {
 	 			 ACS_LOG(LM_FULL_INFO,"LocalOscillatorImpl::get()",(LM_DEBUG,"LocalOscillatorImpl::initialize() %s",ex.what()));

 	 			 _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,impl,"LocalOscillatorImpl::get");
  	 			impl.log(LM_DEBUG);
 	 			throw impl.getReceiversErrorsEx();


 	 		 }



}


void LocalOscillatorImpl::rfon() throw (CORBA::SystemException,ReceiversErrors::ReceiversErrorsEx)
{
	
 	AUTO_TRACE("LocalOscillatorImpl::rfon()");

 

}

void LocalOscillatorImpl::rfoff() throw (CORBA::SystemException,ReceiversErrors::ReceiversErrorsEx)
{
	
 	AUTO_TRACE("LocalOscillatorImpl::rfon()");

}

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_frequency,frequency);
GET_PROPERTY_REFERENCE(ACS::ROdouble,m_amplitude,amplitude);
GET_PROPERTY_REFERENCE(ACS::ROlong,m_isLocked,isLocked);


#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(LocalOscillatorImpl)
