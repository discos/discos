#include "NotoWeatherStationImpl.h"

using namespace baci;

using namespace SimpleParser;



NotoWeatherStationImpl::NotoWeatherStationImpl(
				const ACE_CString &name,
			     maci::ContainerServices * containerServices) :
    CharacteristicComponentImpl(name, containerServices),m_temperature(this),
		       m_winddir(this),
		       m_windspeed(this),
		       m_humidity(this),
		       m_pressure(this),
             m_windspeedPeak(this),
             m_autoparkThreshold(this)
{	
    m_parameters.temperature = 0.0;
    m_parameters.humidity = 0.0;
    m_parameters.pressure = 0.0;
    m_parameters.windspeed = 0.0;
    m_parameters.winddir = 0.0;
    m_parameters.windspeedpeak = 0.0;

	AUTO_TRACE("NotoWeatherStationImpl::NotoWeatherStationImpl");
}

NotoWeatherStationImpl::~NotoWeatherStationImpl()
{

        /*if(m_controlThread_p!=0)
        getContainerServices()->getThreadManager()->destroy(m_controlThread_p);*/

        AUTO_TRACE("NotoWeatherStationImpl::~NotoWeatherStationImpl");
//	deleteAll();
}


void NotoWeatherStationImpl::cleanUp() throw (ACSErr::ACSbaseExImpl)
{
	if (m_controlThread_p!=NULL) {
		m_controlThread_p->suspend();
		getContainerServices()->getThreadManager()->destroy(m_controlThread_p);
	}
	CharacteristicComponentImpl::cleanUp();	
   AUTO_TRACE("NotoWeatherStationImpl::cleanUp()");
}

void NotoWeatherStationImpl::aboutToAbort() throw (ACSErr::ACSbaseExImpl)
{
	if (m_controlThread_p!=NULL) {
		m_controlThread_p->suspend();
		getContainerServices()->getThreadManager()->destroy(m_controlThread_p);
	}	
	AUTO_TRACE("NotoWeatherStationImpl::aboutToAbort()");
}

void NotoWeatherStationImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	ACS_LOG(LM_FULL_INFO,"NotoWeatherStationImpl::execute()",(LM_INFO,"COMPSTATE_OPERATIONAL"));
}

void NotoWeatherStationImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	MeteoSocket *sock;
	ACS::Time timestamp;
	try
    {
        double threadSleepTime;
        if (CIRATools::getDBValue(getContainerServices(),"IPAddress",ADDRESS) &&
            CIRATools::getDBValue(getContainerServices(),"port",PORT) &&
            CIRATools::getDBValue(getContainerServices(),"UpdatingThreadTime", threadSleepTime))
		{
			ACS_LOG(LM_FULL_INFO,"NotoWeatherStationImpl::initialize()",(LM_INFO,"IP address %s, Port %d ",(const char *) ADDRESS,PORT));
		}
        else
		{
			 ACS_LOG(LM_FULL_INFO,"NotoWeatherStationImpl::initialize()",(LM_ERROR,"Error getting IP address from CDB" ));
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"NotoWeatherStationImpl::initialize()");
		throw dummy;
		}    


		
		sock=new MeteoSocket(ADDRESS,PORT);
 		m_socket =new CSecureArea<MeteoSocket>(sock);
		m_temperature=new RWdouble(getContainerServices()->getName()+":temperature", getComponent(), new DevIOTemperature(m_socket),true);
		
		m_winddir=new RWdouble(getContainerServices()->getName()+":winddir", getComponent(), new DevIOWinddir(m_socket),true);
		m_windspeed=new RWdouble(getContainerServices()->getName()+":windspeed", getComponent(), new DevIOWindspeed(m_socket),true);
		m_humidity=new RWdouble(getContainerServices()->getName()+":humidity", getComponent(), new DevIOHumidity(m_socket),true);
		m_pressure=new RWdouble(getContainerServices()->getName()+":pressure", getComponent(), new DevIOPressure(m_socket),true);
	   m_windspeedPeak=new RWdouble(getContainerServices()->getName()+":windspeedpeak", getComponent(), new DevIOWindspeedPeak(m_socket),true);
   	m_autoparkThreshold=new ROdouble(getContainerServices()->getName()+":autoparkThreshold",getComponent());            
      
      
      m_autoparkThreshold->getDevIO()->write(60.0,timestamp);           

      NotoWeatherStationImpl* self_p = this;
      m_controlThread_p = getContainerServices()->getThreadManager()->create<CMeteoParamUpdaterThread, NotoWeatherStationImpl*>("MeteoParam Updater", self_p);
      m_controlThread_p->setSleepTime(threadSleepTime*10);
//              m_controlThread_p->setResponseTime(60*1000000);
      m_controlThread_p->resume();
                
		m_parser=new CParser<MeteoSocket>(sock,10);
		m_parser->add("getWindSpeed",new function0<MeteoSocket,non_constant,double_type >(sock,&MeteoSocket::getWindSpeed),0 );
		m_parser->add("getTemperature",new function0<MeteoSocket,non_constant,double_type >(sock,&MeteoSocket::getTemperature),0 );
		m_parser->add("getHumidity",new function0<MeteoSocket,non_constant,double_type >(sock,&MeteoSocket::getHumidity),0 );
		m_parser->add("getPressure",new function0<MeteoSocket,non_constant,double_type >(sock,&MeteoSocket::getPressure),0 );
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"NotoWeatherStationImpl::NotoWeatherStationImpl()");
		throw dummy;
	}catch (ComponentErrors::ComponentErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E;
	}

#if 0


	try {
		CSecAreaResourceWrapper<MeteoSocket> sock=m_socket->Get();
		if (!sock->isConnected())
		{
		sock->connection();
// 		cout << "Connected  to Meteo Station @"<<ADDRESS<<":"<<PORT <<endl;
		ACS_LOG(LM_FULL_INFO,"NotoWeatherStationImpl::Disconnect()",(LM_DEBUG,"Connected  to Meteo Station @%s:%d  ",(const char *) ADDRESS,PORT));

		}

	} catch (ComponentErrors::SocketErrorExImpl &x)
	
        {
                 ACS_LOG(LM_FULL_INFO,"NotoWeatherStationImpl::initialize()",
                       (LM_ERROR,"Can not connect  to WeatherStation @%s:%d  ",
                       (const char *) ADDRESS,PORT));

		_THROW_EXCPT(ComponentErrors::SocketErrorExImpl,"NotoWeatherStationImpl::initialize()");
 
	}
#endif 


        AUTO_TRACE("NotoWeatherStationImpl::initialize");
}



CORBA::Boolean NotoWeatherStationImpl::command(const char *cmd,CORBA::String_out answer) throw (CORBA::SystemException)
{
	IRA::CString out;
	bool res;
	CSecAreaResourceWrapper<MeteoSocket> line=m_socket->Get();
	try {
		m_parser->run(cmd,out);
		res=true;
	}
	catch (ParserErrors::ParserErrorsExImpl &ex) {
		res=false;
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		ex.log(LM_ERROR); // the errors resulting from the execution are logged here as stated in the documentation of CommandInterpreter interface, while the parser errors are never logged.
		res=false;
	}
	answer=CORBA::string_dup((const char *)out);
	return res;
}


void NotoWeatherStationImpl::deleteAll()
{
        AUTO_TRACE("NotoWeatherStationImpl::deleteAll");
   	CError err;
               
 	try{
 	CSecAreaResourceWrapper<MeteoSocket> sock=m_socket->Get();
                
                if (sock->isConnected())
 		{
			sock->disconnection();
 			delete m_socket;
 		}
 	} catch (...)
 	{
 		cout << "unknown exception in closing component " << endl;


 	}
 
	ACS_LOG(LM_FULL_INFO,"NotoWeatherStationImpl::deleteAll()",(LM_DEBUG,"Disconnecting from socket @%s  ",(const char *)err.getFullDescription()));
	//	delete m_socket;
 }


CORBA::Double NotoWeatherStationImpl::getTemperature() throw (ACSErr::ACSbaseEx, CORBA::SystemException)
{
        AUTO_TRACE("NotoWeatherStationImpl::getTemperature");

	double temperature;
	ACSErr::Completion_var completion;
	temperature = m_temperature->get_sync(completion.out());

	return temperature;
}

/*CORBA::Double NotoWeatherStationImpl::getWindspeed() throw (ACSErr::ACSbaseEx, CORBA::SystemException)
{
        AUTO_TRACE("NotoWeatherStationImpl::getTemperature");
	double windspeed;
	ACSErr::Completion_var completion;
	windspeed = m_windspeed->get_sync(completion.out());
	return windspeed;

}*/
          
CORBA::Double NotoWeatherStationImpl::getWindDir() throw (ACSErr::ACSbaseEx, CORBA::SystemException)
{
        AUTO_TRACE("NotoWeatherStationImpl::getTemperature");

        double winddir;
        ACSErr::Completion_var completion;
        winddir = m_winddir->get_sync(completion.out());
        return winddir;
        
}
             
          
CORBA::Double NotoWeatherStationImpl::getWindSpeedAverage() throw (ACSErr::ACSbaseEx, CORBA::SystemException)
{
        AUTO_TRACE("NotoWeatherStationImpl::getTemperature");

        double windspeed;
        ACSErr::Completion_var completion;
        windspeed = m_windspeed->get_sync(completion.out());

        return windspeed;

}
          
          
          
CORBA::Double NotoWeatherStationImpl::getWindspeedPeak() throw (ACSErr::ACSbaseEx, CORBA::SystemException)
{
        AUTO_TRACE("NotoWeatherStationImpl::getWindspeedPeak");
        double windspeed;
        ACSErr::Completion_var completion;
        windspeed = m_windspeed->get_sync(completion.out());

        return windspeed;

}
          
          
          
          
CORBA::Double NotoWeatherStationImpl::getPressure() throw (ACSErr::ACSbaseEx, CORBA::SystemException)
{
        AUTO_TRACE("NotoWeatherStationImpl::getpressure()");

	double pressure;
	ACSErr::Completion_var completion;
	pressure = m_pressure->get_sync(completion.out());
	return pressure;



}
CORBA::Double NotoWeatherStationImpl::getHumidity() throw (ACSErr::ACSbaseEx, CORBA::SystemException)
{
	AUTO_TRACE("NotoWeatherStationImpl::getHumidity()");
	double humidity;
	ACSErr::Completion_var completion;
	humidity = m_humidity->get_sync(completion.out());
	return humidity;
}

Weather::parameters NotoWeatherStationImpl::getData()
{
    AUTO_TRACE("NotoWeatherStationImpl::getData");
    baci::ThreadSyncGuard guardMeteoParameters(&m_meteoParametersMutex);
    return m_parameters;
}

void NotoWeatherStationImpl::updateData() throw (ACSErr::ACSbaseEx, CORBA::SystemException)
{
    AUTO_TRACE("NotoWeatherStationImpl::updateData");
    Weather::parameters mp;

    try
    {
        ACSErr::Completion_var completion;
        mp.temperature = m_temperature->get_sync(completion.out());
    }
    catch (ACSErr::ACSbaseExImpl& E)
    {
        E.log(LM_DEBUG);
        throw E.getACSbaseEx();
    }
    catch (...)
    {
        _EXCPT(ComponentErrors::UnexpectedExImpl, E, "NotoWeatherStationImpl::updateData");
        throw E.getACSbaseEx();
    }

    try
    {
        ACSErr::Completion_var completion;
        mp.humidity = m_humidity->get_sync(completion.out());
    }
    catch (ACSErr::ACSbaseExImpl& E)
    {
        E.log(LM_DEBUG);
        throw E.getACSbaseEx();
    }
    catch (...)
    {
        _EXCPT(ComponentErrors::UnexpectedExImpl, E, "NotoWeatherStationImpl::updateData");
        throw E.getACSbaseEx();
    }

    try
    {
        ACSErr::Completion_var completion;
        mp.pressure = m_pressure->get_sync(completion.out());
    }
    catch (ACSErr::ACSbaseExImpl& E)
    {
        E.log(LM_DEBUG);
        throw E.getACSbaseEx();
    }
    catch (...)
    {
        _EXCPT(ComponentErrors::UnexpectedExImpl, E, "NotoWeatherStationImpl::updateData");
        throw E.getACSbaseEx();
    }

    try
    {
        ACSErr::Completion_var completion;
        mp.windspeed = m_windspeed->get_sync(completion.out());
    }
    catch (ACSErr::ACSbaseExImpl& E)
    {
        E.log(LM_DEBUG);
        throw E.getACSbaseEx();
    }
    catch (...)
    {
        _EXCPT(ComponentErrors::UnexpectedExImpl, E, "NotoWeatherStationImpl::updateData");
        throw E.getACSbaseEx();
    }

    try
    {
        ACSErr::Completion_var completion;
        mp.winddir = m_winddir->get_sync(completion.out());
    }
    catch (ACSErr::ACSbaseExImpl& E)
    {
        E.log(LM_DEBUG);
        throw E.getACSbaseEx();
    }
    catch (...)
    {
        _EXCPT(ComponentErrors::UnexpectedExImpl, E, "NotoWeatherStationImpl::updateData");
        throw E.getACSbaseEx();
    }

    try
    {
        ACSErr::Completion_var completion;
        mp.windspeedpeak = m_windspeedPeak->get_sync(completion.out());
    }
    catch (ACSErr::ACSbaseExImpl& E)
    {
        E.log(LM_DEBUG);
        throw E.getACSbaseEx();
    }
    catch (...)
    {
        _EXCPT(ComponentErrors::UnexpectedExImpl, E, "NotoWeatherStationImpl::updateData");
        throw E.getACSbaseEx();
    }

    baci::ThreadSyncGuard guardMeteoParameters(&m_meteoParametersMutex);
    m_parameters = mp;
}

ACS::RWdouble_ptr
NotoWeatherStationImpl::temperature ()
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
NotoWeatherStationImpl::winddir ()
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
NotoWeatherStationImpl::windspeed ()
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
NotoWeatherStationImpl::humidity ()
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
NotoWeatherStationImpl::pressure ()
    throw (CORBA::SystemException)
{
    if (m_pressure == 0)
	{
	return ACS::RWdouble::_nil();
	}
    
    ACS::RWdouble_var prop = ACS::RWdouble::_narrow(m_pressure->getCORBAReference());
    return prop._retn();
}

ACS::RWdouble_ptr
NotoWeatherStationImpl::windspeedpeak ()
    throw (CORBA::SystemException)
{
    if (m_windspeedPeak == 0)
        {
        return ACS::RWdouble::_nil();
        }
    
    ACS::RWdouble_var prop = ACS::RWdouble::_narrow(m_windspeedPeak->getCORBAReference());
    return prop._retn();
}

ACS::ROdouble_ptr
NotoWeatherStationImpl::autoparkThreshold () throw (CORBA::SystemException)
{
	if (m_autoparkThreshold == 0) {
		return ACS::ROdouble::_nil();
	}
	ACS::ROdouble_var prop = ACS::ROdouble::_narrow(m_autoparkThreshold->getCORBAReference());
	return prop._retn();
}

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(NotoWeatherStationImpl)
/* ----------------------------------------------------------------*/


