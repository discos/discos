#include "MedWeatherStationImpl.h"

using namespace baci;

using namespace SimpleParser;


MedWeatherStationImpl::MedWeatherStationImpl(
				const ACE_CString &name,
			     maci::ContainerServices * containerServices) :
    CharacteristicComponentImpl(name, containerServices),m_temperature(this),
		       m_winddir(this),
		       m_windspeed(this),
		       m_humidity(this),
		       m_pressure(this)
{	
        AUTO_TRACE("MedWeatherStationImpl::MedWeatherStationImpl");



	 

}

MedWeatherStationImpl::~MedWeatherStationImpl()
{

        AUTO_TRACE("MedWeatherStationImpl::~MedWeatherStationImpl");
//	deleteAll();

    

}


void MedWeatherStationImpl::cleanUp() throw (ACSErr::ACSbaseExImpl)
{
	    CharacteristicComponentImpl::cleanUp();
        AUTO_TRACE("MedWeatherStationImpl::cleanUp()");


}


char * MedWeatherStationImpl::command(const char *cmd) throw (CORBA::SystemException,ManagementErrors::CommandLineErrorEx)
{
	{
		AUTO_TRACE("MedWeatherStationImpl::command()");
		IRA::CString out;
		IRA::CString in;
		CSecAreaResourceWrapper<MeteoSocket> line=m_socket->Get();
		in=IRA::CString(cmd);
		try {
			m_parser->run(in,out);
		}
		catch (ParserErrors::ParserErrorsExImpl &ex) {
			_ADD_BACKTRACE(ManagementErrors::CommandLineErrorExImpl,impl,ex,"MedWeatherStationImpl::command()");
			impl.setCommand(cmd);
			impl.setErrorMessage((const char *)out);
			impl.log(LM_DEBUG);
			throw impl.getCommandLineErrorEx();
		}
		return CORBA::string_dup((const char *)out);
	}

}


void MedWeatherStationImpl::deleteAll()
{
        AUTO_TRACE("MedWeatherStationImpl::deleteAll");
   	CError err;
 	try{
 	CSecAreaResourceWrapper<MeteoSocket> sock=m_socket->Get();
 		if (sock->isConnected())
 		{
			sock->disconnect();
 			delete m_socket;
 		}

 	} catch (...)
 	{
 		cout << "unknown exception in closing component " << endl;


 	}

	ACS_LOG(LM_FULL_INFO,"MedWeatherStationImpl::deleteAll()",(LM_DEBUG,"Disconnecting from socket @%s  ",(const char *)err.getFullDescription()));
	//	delete m_socket;




 }




 void MedWeatherStationImpl::aboutToAbort() throw (ACSErr::ACSbaseExImpl)

{
        AUTO_TRACE("MedWeatherStationImpl::aboutToAbort()");

	cleanUp();

}


CORBA::Double MedWeatherStationImpl::getTemperature() throw (CORBA::SystemException)
{
        AUTO_TRACE("MedWeatherStationImpl::getTemperature");

	double temperature;
	ACSErr::Completion_var completion;
	temperature = m_temperature->get_sync(completion.out());

	return temperature;



}
CORBA::Double MedWeatherStationImpl::getWindspeed() throw (CORBA::SystemException)
{
        AUTO_TRACE("MedWeatherStationImpl::getTemperature");

	double windspeed;
	ACSErr::Completion_var completion;
	windspeed = m_windspeed->get_sync(completion.out());

	return windspeed;

}
CORBA::Double MedWeatherStationImpl::getPressure() throw (CORBA::SystemException)
{
        AUTO_TRACE("MedWeatherStationImpl::getpressure()");

	double pressure;
	ACSErr::Completion_var completion;
	pressure = m_pressure->get_sync(completion.out());
	return pressure;



}
CORBA::Double MedWeatherStationImpl::getHumidity() throw (CORBA::SystemException)
{
        AUTO_TRACE("MedWeatherStationImpl::getHumidity()");

	double humidity;
	ACSErr::Completion_var completion;
	humidity = m_humidity->get_sync(completion.out());
	return humidity;



}

Weather::parameters MedWeatherStationImpl::getData()throw (CORBA::SystemException)
{
	Weather::parameters mp;
        AUTO_TRACE("MedWeatherStationImpl::getData");
	CError err;
	CString rdata;
	CSecAreaResourceWrapper<MeteoSocket> sock=m_socket->Get();

	double temperature;
	double winddir;
	double windspeed;
	double pressure;
	double humidity;
	
	sock->sendCMD(err,CString("spettro\n"));
	sock->receiveData(err,rdata);


	
	ACSErr::Completion_var completion;

	temperature = m_temperature->get_sync(completion.out());
	winddir     = m_winddir->get_sync(completion.out());
	windspeed   = m_windspeed->get_sync(completion.out());
	pressure    = m_pressure->get_sync(completion.out());
	humidity    = m_humidity->get_sync(completion.out());
	

//  	cout <<"received"<< len << endl;
//	string ss;
//	string srecv;
//	srecv=string((const char*)rdata);
//	vector<string> vrecv;
//
//	istringstream  ist(srecv); // string stream
//	while (ist >> ss) vrecv.push_back(ss) ;// split the string
//	int ndata=vrecv.size();
//	if (ndata > 3)
//
//	{
// 		temperature = atof(vrecv[ndata-3].c_str());
//		pressure = atof(vrecv[ndata-2].c_str());
//		humidity  = atof(vrecv[ndata-1].c_str());
//
////		windspeed  = atof(vrecv[ndata-1].c_str());
//
//	} else
//	{
//		 ACS_LOG(LM_FULL_INFO,"MeteoSocket::update()",(LM_ERROR,"Not enough data from meteo server"));
//	}
	

	mp.temperature=temperature;
	mp.humidity   =humidity;
 	mp.wind       =windspeed;
	mp.pressure   =pressure;
	

	return mp;
}
 
 
 
void MedWeatherStationImpl::initialize() throw (ACSErr::ACSbaseExImpl)

{

	MeteoSocket *sock;
	try {
		  if 			(CIRATools::getDBValue(getContainerServices(),"IPAddress",ADDRESS) && CIRATools::getDBValue(getContainerServices(),"port",PORT))
		{
			ACS_LOG(LM_FULL_INFO,"MedWeatherStationImpl::initialize()",(LM_INFO,"IP address %s, Port %d ",(const char *) ADDRESS,PORT));


		} else
		{
			 ACS_LOG(LM_FULL_INFO,"MedWeatherStationImpl::initialize()",(LM_ERROR,"Error getting IP address from CDB" ));
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"MedWeatherStationImpl::initialize()");
		throw dummy;
		}    


		
		sock=new MeteoSocket(ADDRESS,PORT);
		m_socket =new CSecureArea<MeteoSocket>(sock);
		m_temperature=new RWdouble(getContainerServices()->getName()+":temperature", getComponent(), new DevIOTemperature(m_socket),true);
		
		m_winddir=new RWdouble(getContainerServices()->getName()+":winddir", getComponent(), new DevIOWinddir(m_socket),true);
		m_windspeed=new RWdouble(getContainerServices()->getName()+":windspeed", getComponent(), new DevIOWindspeed(m_socket),true);
		m_humidity=new RWdouble(getContainerServices()->getName()+":humidity", getComponent(), new DevIOHumidity(m_socket),true);
		m_pressure=new RWdouble(getContainerServices()->getName()+":pressure", getComponent(), new DevIOPressure(m_socket),true);



		m_parser=new CParser<MeteoSocket>(sock,10);
		m_parser->add<0>("getWindSpeed",new function0<MeteoSocket,non_constant,double_type >(sock,&MeteoSocket::getWindSpeed) );
		m_parser->add<0>("getTemperature",new function0<MeteoSocket,non_constant,double_type >(sock,&MeteoSocket::getTemperature) );
		m_parser->add<0>("getHumidity",new function0<MeteoSocket,non_constant,double_type >(sock,&MeteoSocket::getHumidity) );
		m_parser->add<0>("getPressure",new function0<MeteoSocket,non_constant,double_type >(sock,&MeteoSocket::getPressure) );




	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"MedWeatherStationImpl::MedWeatherStationImpl()");
		throw dummy;
	}catch (ComponentErrors::ComponentErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();
	}




	try {
		CSecAreaResourceWrapper<MeteoSocket> sock=m_socket->Get();
		if (!sock->isConnected())
		{
		sock->connect();
// 		cout << "Connected  to Meteo Station @"<<ADDRESS<<":"<<PORT <<endl;
		ACS_LOG(LM_FULL_INFO,"MedWeatherStationImpl::Disconnect()",(LM_DEBUG,"Connected  to Meteo Station @%s:%d  ",(const char *) ADDRESS,PORT));

		}

	} catch (ComponentErrors::SocketErrorExImpl &x)
	{
		_THROW_EXCPT(ComponentErrors::SocketErrorExImpl,"MedWeatherStationImpl::initialize()");
	}


        AUTO_TRACE("MedWeatherStationImpl::initialize");
        ;
        
  

}
 

 



//pdate();



ACS::RWdouble_ptr
MedWeatherStationImpl::temperature ()
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
MedWeatherStationImpl::winddir ()
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
MedWeatherStationImpl::windspeed ()
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
MedWeatherStationImpl::humidity ()
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
MedWeatherStationImpl::pressure ()
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
MACI_DLL_SUPPORT_FUNCTIONS(MedWeatherStationImpl)
/* ----------------------------------------------------------------*/


