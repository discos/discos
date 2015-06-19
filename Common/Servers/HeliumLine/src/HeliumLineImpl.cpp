/*
 * HeliumLineImpl.cpp
 *
 *  Created on: Apr 9, 2014
 *      Author: spoppi
 */

#include "HeliumLineImpl.h"
#include <Definitions.h>
#include <maciContainerServices.h>

#define _GET_STRING_ATTRIBUTE(ATTRIB,DESCR,FIELD) { \
	CString tmps; \
	if (!CIRATools::getDBValue(Services,ATTRIB,tmps)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"::HeliumLineImpl::Init()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
	} \
	else { \
		FIELD=tmps; \
		ACS_DEBUG_PARAM("HeliumLineImp::Init()",DESCR" %s",(const char*)tmps); \
	} \
}


#define GET_PROPERTY_REFERENCE(TYPE,PROPERTY,PROPERTYNAME) TYPE##_ptr HeliumLineImpl::PROPERTYNAME() throw (CORBA::SystemException) \
{ \
	if (PROPERTY==0) return TYPE::_nil(); \
	TYPE##_var tmp=TYPE::_narrow(PROPERTY->getCORBAReference()); \
	return tmp._retn(); \
}


HeliumLineImpl::HeliumLineImpl(const ACE_CString &name,maci::ContainerServices * containerServices) :
CharacteristicComponentImpl(name,containerServices),
   m_supplyPressure(this),m_returnPressure(this)
{

 	AUTO_TRACE("HeliumLineImpl::HeliumLineImpl");

}



HeliumLineImpl::~HeliumLineImpl()
{
 	AUTO_TRACE("HeliumLineImpl::~HeliumLineImpl");


}


void HeliumLineImpl::cleanUp() throw (ACSErr::ACSbaseExImpl)

{


	CharacteristicComponentImpl::cleanUp();
	AUTO_TRACE("HeliumLineImpl::cleanUp");

}


void HeliumLineImpl::aboutToAbort() throw (ACSErr::ACSbaseExImpl)
{

	AUTO_TRACE("HeliumLineImpl::aboutToAbort()");
	cleanUp();

}


void HeliumLineImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{

	AUTO_TRACE("HeliumLineImpl::initialize()");
	try {

		CommandLine* line=new CommandLine();
		     m_commandLine=new CSecureArea<CommandLine>(line);
		     m_supplyPressure= new  ROdouble(getContainerServices()->getName()+":supplyPressure",getComponent(),new DevIOsupplyPressure(m_commandLine),true);
		     m_returnPressure= new ROdouble(getContainerServices()->getName()+":returnPressure",getComponent(),new DevIOreturnPressure(m_commandLine),true);
 		}
		 catch (ACSErr::ACSbaseExImpl& E) {
				_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,_dummy,E,"LocalOscillatorImpl::initialize()");
				throw _dummy;
	    } catch (std::bad_alloc& ex) {
				_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"LocalOscillatorImpl::initialize()");
				throw dummy;
	    }
		try {
			CString	ADDRESS;
			int PORT=502;
	//			  CIRATools::getDBValue(getContainerServices(),"port",PORT))

			if 	 (CIRATools::getDBValue(getContainerServices(),"IPAddress",ADDRESS))
				  	  {
					  	  ACS_LOG(LM_FULL_INFO,"HeliumLineImpl::initialize()",(LM_INFO,"IP address %s, Port %d ",(const char *) ADDRESS,PORT));
				  	  } else
				  	  {
				  		  ACS_LOG(LM_FULL_INFO,"HeliumLineImpl::initialize()",(LM_ERROR,"Error getting IP address from CDB" ));
				  		  _EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"HeliumLineImpl::initialize()");
				  		  throw dummy;
				  	  }
			CSecAreaResourceWrapper<CommandLine> line=m_commandLine->Get();
			line->configure((const char *) ADDRESS,PORT,0,4);


		}catch (ComponentErrors::ComponentErrorsExImpl& E) {
			E.log(LM_DEBUG);
			throw E.getComponentErrorsEx();
		}







}

void HeliumLineImpl::execute() throw (ACSErr::ACSbaseExImpl)

{

	AUTO_TRACE("HeliumLineImpl::initialize()");

}

void HeliumLineImpl::getSupplyPressure(CORBA::Double& supplyPrs)   throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx)
{

	double supplyPress;

	CSecAreaResourceWrapper<CommandLine> line=m_commandLine->Get();
    line->getSupplyPressure(supplyPrs);

	AUTO_TRACE("HeliumLineImpl::getSupplyPressure()");

}

void HeliumLineImpl::getReturnPressure(CORBA::Double& retPrs) throw  (CORBA::SystemException,ComponentErrors::ComponentErrorsEx)
		{

	AUTO_TRACE("HeliumLineImpl::getSupplyPressure()");

		}


GET_PROPERTY_REFERENCE(ACS::ROdouble,m_supplyPressure,supplyPressure);
GET_PROPERTY_REFERENCE(ACS::ROdouble,m_returnPressure,returnPressure);

#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(HeliumLineImpl)

