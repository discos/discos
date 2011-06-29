#include "SRTWeatherStationImpl.h"

using namespace baci;



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
        AUTO_TRACE("SRTWeatherStationImpl::SRTWeatherStationImpl");



	 

}

SRTWeatherStationImpl::~SRTWeatherStationImpl()
{

        AUTO_TRACE("SRTWeatherStationImpl::~SRTWeatherStationImpl");
//	deleteAll();

    

}


void SRTWeatherStationImpl::cleanUp() throw (ACSErr::ACSbaseExImpl)
{
	CharacteristicComponentImpl::cleanUp();
        AUTO_TRACE("SRTWeatherStationImpl::cleanUp()");


}




void SRTWeatherStationImpl::deleteAll()
{
        AUTO_TRACE("SRTWeatherStationImpl::deleteAll");
//	CError err;
//	delete m_socket;
// 	try{
// 	CSecAreaResourceWrapper<SRTWeatherSocket> sock=m_socket->Get();
// 		if (sock->isConnected())
// 		{
// 			sock->disconnect();
// 			delete m_socket;	
// 		} 
//  	
// 	} catch (...)
// 	{
// 		cout << "unknown exception in closing component " << endl;
// 
// 	
// 	}
// 
// 	ACS_LOG(LM_FULL_INFO,"SRTWeatherStationImpl::deleteAll()",(LM_DEBUG,"Disconnecting from socket @%s  ",(const char *)err.getFullDescription()));
// 
// 	 
// 
// 
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
	
	ACSErr::Completion_var completion;
	
	temperature = m_temperature->get_sync(completion.out());
	winddir     = m_winddir->get_sync(completion.out());
	windspeed   = m_windspeed->get_sync(completion.out());
	pressure    = m_pressure->get_sync(completion.out());
	humidity    = m_humidity->get_sync(completion.out());
	
	mp.temperature=temperature;
	mp.humidity   =humidity;
 	mp.wind       =windspeed;
	mp.pressure   =pressure;
	

	return mp;
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


	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"SRTWeatherStationImpl::SRTWeatherStationImpl()");
		throw dummy;
	}




	try {
		CSecAreaResourceWrapper<SRTWeatherSocket> sock=m_socket->Get();
		if (!sock->isConnected())
		{
		sock->connect();
// 		cout << "Connected  to Weather Station @"<<ADDRESS<<":"<<PORT <<endl;
		ACS_LOG(LM_FULL_INFO,"SRTWeatherStationImpl::Disconnect()",(LM_DEBUG,"Connected  to Weather Station @%s:%d  ",(const char *) ADDRESS,PORT));

		}

	} catch (ComponentErrors::SocketErrorExImpl &x)
	{
		_THROW_EXCPT(ComponentErrors::SocketErrorExImpl,"SRTWeatherStationImpl::initialize()");
	}


        AUTO_TRACE("SRTWeatherStationImpl::initialize");


// 	// stop all threads 
//  ACS_LOG(LM_FULL_INFO,"SRTWeatherStationImplImpl::initialize()",(LM_INFO,"init METEO STATION "));
// 	if (CIRATools::getDBValue(getContainerServices(),"IPAddress",ADDRESS) && CIRATools::getDBValue(getContainerServices(),"port",PORT))
// 	{
// 		ACS_LOG(LM_FULL_INFO,"SRTWeatherStationImplImpl::initialize()",(LM_INFO,"IP address %s, Port %d ",(const char *) ADDRESS,PORT));
// 
// 
// 	} else
// 
// 	{
// 		 ACS_LOG(LM_FULL_INFO,"SRTWeatherStationImplImpl::initialize()",(LM_ERROR,"Error getting IP address from CDB" ));
// 	}





;
        
  

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

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SRTWeatherStationImpl)
/* ----------------------------------------------------------------*/


