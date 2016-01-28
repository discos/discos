#include "SRTWeatherStationImpl.h"

using namespace baci;

using namespace SimpleParser;

SRTWeatherStationImpl::SRTWeatherStationImpl(
				const ACE_CString &name,
			     maci::ContainerServices * containerServices) :
    CharacteristicComponentImpl(name, containerServices),m_temperature(this),
		       m_winddir(this),
		       m_windspeed(this),
		       m_windspeedpeak(this),
		       m_humidity(this),
		       m_pressure(this)
{	

      
        m_containerServices = containerServices;       
        m_scheduler.setComponentName("IDL:alma/Management/Scheduler:1.0");
        m_scheduler.setContainerServices(m_containerServices);
        m_antennaBoss.setComponentName("IDL:alma/Antenna/AntennaBoss:1.0");
        m_antennaBoss.setContainerServices(m_containerServices);
        

        
        AUTO_TRACE("SRTWeatherStationImpl::SRTWeatherStationImpl");
}

SRTWeatherStationImpl::~SRTWeatherStationImpl()
{
	ACS_DEBUG_PARAM("::SRTWeatherStationImpl::~SRTWeatherStationImpl", "Destroying %s...", getComponent()->getName());
	if(m_controlThread_p!=0)
        getContainerServices()->getThreadManager()->destroy(m_controlThread_p);

        AUTO_TRACE("SRTWeatherStationImpl::~SRTWeatherStationImpl");

}


void SRTWeatherStationImpl::cleanUp() throw (ACSErr::ACSbaseExImpl)
{
	CharacteristicComponentImpl::cleanUp();
//         AUTO_TRACE("SRTWeatherStationImpl::cleanUp()");
	m_controlThread_p->suspend();
	getContainerServices()->getThreadManager()->stopAll(); 
	

}

CORBA::Boolean  SRTWeatherStationImpl::command(const char *cmd,CORBA::String_out answer)   throw (CORBA::SystemException)
{
	AUTO_TRACE("SRTWeatherStationImpl::command()");
	IRA::CString out;
	IRA::CString in;
	CSecAreaResourceWrapper<SRTWeatherSocket> line=m_socket->Get();
	//in=IRA::CString(configCommand);
	bool res;

	try {
		m_parser->run(cmd,out);
		res=true;

	}
	catch (ParserErrors::ParserErrorsExImpl &ex) {
		res=false;

//		_ADD_BACKTRACE(ManagementErrors::CommandLineErrorExImpl,impl,ex,"SRTWeatherStationImpl::command()");
//		impl.setCommand(configCommand);
//		impl.setErrorMessage((const char *)out);
//		impl.log(LM_DEBUG);
//		throw impl.getCommandLineErrorEx();

	}
	catch (ACSErr::ACSbaseExImpl& ex) {
			ex.log(LM_ERROR); // the errors resulting from the execution are logged here as stated in the documentation of CommandInterpreter interface, while the parser errors are never logged.
			res=false;
		}
	answer=CORBA::string_dup((const char *)out);
	return res;}



void SRTWeatherStationImpl::deleteAll()
{
        AUTO_TRACE("SRTWeatherStationImpl::deleteAll");
 
 }


void SRTWeatherStationImpl::aboutToAbort() throw (ACSErr::ACSbaseExImpl)

{
        AUTO_TRACE("SRTWeatherStationImpl::aboutToAbort()");

	cleanUp();

}




Weather::parameters SRTWeatherStationImpl::getData()throw (ACSErr::ACSbaseExImpl)
{
	Weather::parameters mp;
        AUTO_TRACE("SRTWeatherStationImpl::getData");
     	ACS::Time timestamp;

	double temperature;
	double winddir;
	double windspeed;
	double pressure;
	double humidity;
	
	try{
		ACSErr::Completion_var completion;
		temperature = m_temperature->get_sync(completion.out());
/*		IRA::CIRATools::Wait(1,0);	*/
	}
	catch (ComponentErrors::ComponentErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();		
	}
	
	try{
		ACSErr::Completion_var completion;
		winddir     = m_winddir->get_sync(completion.out());
 	}
	catch (ComponentErrors::ComponentErrorsExImpl& E){
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();		
	}
	try{
		ACSErr::Completion_var completion;

		windspeed   = m_windspeed->get_sync(completion.out());
 	} catch (ComponentErrors::ComponentErrorsExImpl& E){
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();		
	}

	try{
		ACSErr::Completion_var completion;
		pressure    = m_pressure->get_sync(completion.out());
 	}catch (ComponentErrors::ComponentErrorsExImpl& E){
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();		
	}
	try    {
		ACSErr::Completion_var completion;

		humidity    = m_humidity->get_sync(completion.out());
 	}catch (ComponentErrors::ComponentErrorsExImpl& E){
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();		
	}

		mp.temperature=temperature;
		mp.humidity   =humidity;
 		mp.wind       =windspeed;
		mp.pressure   =pressure;
		return mp;

	  

}
 
CORBA::Double SRTWeatherStationImpl::getWindspeedPeak() throw (ACSErr::ACSbaseExImpl)
{
        AUTO_TRACE("SRTWeatherStationImpl::getWindspeedPeak");

	double val;
	ACSErr::Completion_var completion;
	val = m_windspeedpeak->get_sync(completion.out());

	return val;

}


CORBA::Double SRTWeatherStationImpl::getWindSpeedAverage() throw (ACSErr::ACSbaseExImpl)
{
        AUTO_TRACE("SRTWeatherStationImpl::getWindspeedAverage");
        double windspeed;
        ACSErr::Completion_var completion;
        windspeed = m_windspeed->get_sync(completion.out());

        return windspeed;


}
 

CORBA::Double SRTWeatherStationImpl::getHumidity() throw (ACSErr::ACSbaseExImpl)
{
	
        AUTO_TRACE("SRTWeatherStationImpl::getHumidity");
        double val;
        ACSErr::Completion_var completion;
        val = m_humidity->get_sync(completion.out());

        return val;
	 

}

CORBA::Double SRTWeatherStationImpl::getTemperature() throw (ACSErr::ACSbaseExImpl)
{
	double temperature;

        AUTO_TRACE("SRTWeatherStationImpl::getTemperature");
	try{
		ACSErr::Completion_var completion;
		temperature = m_temperature->get_sync(completion.out());
/*		IRA::CIRATools::Wait(1,0);	*/
	}
	catch (ComponentErrors::ComponentErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();		
	}
	 
	return temperature;



}

CORBA::Double SRTWeatherStationImpl::getPressure() throw (ACSErr::ACSbaseExImpl)
{
	double pressure;

        AUTO_TRACE("SRTWeatherStationImpl::getPressure");

	try{
		ACSErr::Completion_var completion;
		pressure    = m_pressure->get_sync(completion.out());
 	}catch (ComponentErrors::ComponentErrorsExImpl& E){
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();		
	}
	return pressure;


}

CORBA::Double SRTWeatherStationImpl::getWindDir() throw (ACSErr::ACSbaseExImpl)
{
	double windir;

        AUTO_TRACE("SRTWeatherStationImpl::getPressure");

	try{
		ACSErr::Completion_var completion;
		windir    = m_winddir->get_sync(completion.out());
 	}catch (ComponentErrors::ComponentErrorsExImpl& E){
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();		
	}
	return windir;



}
double SRTWeatherStationImpl::getWind()
{
	return 42.0;


}

 
void SRTWeatherStationImpl::initialize() throw (ACSErr::ACSbaseExImpl)

{

	SRTWeatherSocket *sock;
	try {
		  if 			(CIRATools::getDBValue(getContainerServices(),"IPAddress",ADDRESS) && CIRATools::getDBValue(getContainerServices(),"port",PORT))
		  	  {
			  	  ACS_LOG(LM_FULL_INFO,"SRTWeatherStationImpl::initialize()",(LM_INFO,"IP address %s, Port %d ",(const char *) ADDRESS,PORT));


		  	  } else
		  	  {
		  		  ACS_LOG(LM_FULL_INFO,"SRTWeatherStationImpl::initialize()",(LM_ERROR,"Error getting IP address from CDB" ));
		  		  _EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"SRTWeatherStationImpl::initialize()");
		  		  throw dummy;
		  	  }
		sock=new SRTWeatherSocket(ADDRESS,PORT);
		m_socket =new CSecureArea<SRTWeatherSocket>(sock);
		m_temperature=new RWdouble(getContainerServices()->getName()+":temperature", getComponent(), new DevIOTemperature(m_socket),true);
		m_winddir=new RWdouble(getContainerServices()->getName()+":winddir", getComponent(), new DevIOWinddir(m_socket),true);
		m_windspeed=new RWdouble(getContainerServices()->getName()+":windspeed", getComponent(), new DevIOWindspeed(m_socket),true);
		m_windspeedpeak=new RWdouble(getContainerServices()->getName()+":windspeedpeak", getComponent(), new DevIOWindspeedpeak(m_socket),true);
		m_humidity=new RWdouble(getContainerServices()->getName()+":humidity", getComponent(), new DevIOHumidity(m_socket),true);
		m_pressure=new RWdouble(getContainerServices()->getName()+":pressure", getComponent(), new DevIOPressure(m_socket),true);
		
		SRTWeatherStationImpl* self_p =this;
		m_controlThread_p = getContainerServices()->getThreadManager()->create<CWindCheckerThread, SRTWeatherStationImpl*>("MeteoStation",self_p );
	 	m_controlThread_p->setSleepTime  (1*10000000);
//		m_controlThread_p->setResponseTime(60*1000000);
		m_controlThread_p->resume();
		m_parser=new CParser<SRTWeatherSocket>(sock,10); 
		m_parser->add("getWindSpeedAverage",new function0<SRTWeatherSocket,non_constant,double_type >(sock,&SRTWeatherSocket::getWind),0 );
//		m_parser->add<0>("getTemperature",new function0<SRTWeatherSocket,non_constant,double_type >(sock,&SRTWeatherSocket::getTemperature) );

		m_parser->add("getHumidity",new function0<SRTWeatherSocket,non_constant,double_type >(sock,&SRTWeatherSocket::getHumidity),0 );
		m_parser->add("getPressure",new function0<SRTWeatherSocket,non_constant,double_type >(sock,&SRTWeatherSocket::getPressure),0 );
		m_parser->add("getWinDir",new function0<SRTWeatherSocket,non_constant,double_type >(sock,&SRTWeatherSocket::getWinDir),0 );
		m_parser->add("getWindSpeedPeak",new function0<SRTWeatherSocket,non_constant,double_type >(sock,&SRTWeatherSocket::getWindSpeedPeak),0 );



		}	


	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"SRTWeatherStationImpl::SRTWeatherStationImpl()");
		throw dummy;
	}catch (ComponentErrors::ComponentErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();		
	}

	



	try {
		CSecAreaResourceWrapper<SRTWeatherSocket> sock=m_socket->Get();
		if (!sock->isConnected())
		{
		sock->connect();
 		ACS_LOG(LM_FULL_INFO,"SRTWeatherStationImpl::Disconnect()",(LM_DEBUG,"Connected  to Weather Station @%s:%d  ",(const char *) ADDRESS,PORT));

		}

	} catch (ComponentErrors::SocketErrorExImpl &x)
	{
		x.log(LM_DEBUG);
		throw x.getComponentErrorsEx();		
	}

	SRTWeatherStationImpl* self_p =this;
        AUTO_TRACE("SRTWeatherStationImpl::initialize");



  


}
 

 



//pdate();



ACS::RWdouble_ptr
SRTWeatherStationImpl::temperature ()
    throw (CORBA::SystemException)
{
    if (m_temperature == 0)
	{
	return ACS::RWdouble::_nil();
	}
    
    ACS::RWdouble_var prop = ACS::RWdouble::_narrow(m_temperature->getCORBAReference());
    return prop._retn();
}


ACS::RWdouble_ptr
SRTWeatherStationImpl::winddir ()
    throw (CORBA::SystemException)
{
    if (m_winddir == 0)
	{
	return ACS::RWdouble::_nil();
	}
    
    ACS::RWdouble_var prop = ACS::RWdouble::_narrow(m_winddir->getCORBAReference());
    return prop._retn();
}

ACS::RWdouble_ptr
SRTWeatherStationImpl::windspeed ()
    throw (CORBA::SystemException)
{
    if (m_windspeed == 0)
	{
	return ACS::RWdouble::_nil();
	}
    
    ACS::RWdouble_var prop = ACS::RWdouble::_narrow(m_windspeed->getCORBAReference());
    return prop._retn();
}
ACS::RWdouble_ptr
SRTWeatherStationImpl::windspeedpeak ()
    throw (CORBA::SystemException)
{
    if (m_windspeedpeak == 0)
	{
	return ACS::RWdouble::_nil();
	}
    
    ACS::RWdouble_var prop = ACS::RWdouble::_narrow(m_windspeedpeak->getCORBAReference());
    return prop._retn();
}

ACS::RWdouble_ptr
SRTWeatherStationImpl::humidity ()
    throw (CORBA::SystemException)
{
    if (m_humidity == 0)
	{
	return ACS::RWdouble::_nil();
	}
    
    ACS::RWdouble_var prop = ACS::RWdouble::_narrow(m_humidity->getCORBAReference());
    return prop._retn();
}
ACS::RWdouble_ptr
SRTWeatherStationImpl::pressure ()
    throw (CORBA::SystemException)
{
    if (m_pressure == 0)
	{
	return ACS::RWdouble::_nil();
	}
    
    ACS::RWdouble_var prop = ACS::RWdouble::_narrow(m_pressure->getCORBAReference());
    return prop._retn();
}


void SRTWeatherStationImpl::parkAntenna()
{

   m_scheduler->stopSchedule();
   m_antennaBoss->park();
   ACS_LOG(LM_FULL_INFO,"SRTWeatherStationImpl::parkAntenna()",(LM_WARNING,"AUTOSTOWING!!!!!!"));
   


}





/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SRTWeatherStationImpl)
/* ----------------------------------------------------------------*/


