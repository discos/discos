#include "WeatherStationImpl.h"

using namespace baci;
using namespace SimpleParser;

WeatherStationImpl::WeatherStationImpl(
    const ACE_CString &name,
    maci::ContainerServices * containerServices) :
    CharacteristicComponentImpl(name, containerServices),
    m_temperature(this),
    m_winddir(this),
    m_windspeed(this),
    m_windspeedpeak(this),
    m_humidity(this),
    m_pressure(this),
    m_autoParkThreshold(this)
{
    m_containerServices = containerServices;
    m_scheduler.setComponentName("IDL:alma/Management/Scheduler:1.0");
    m_scheduler.setContainerServices(m_containerServices);
    m_antennaBoss.setComponentName("IDL:alma/Antenna/AntennaBoss:1.0");
    m_antennaBoss.setContainerServices(m_containerServices);

    m_parameters.temperature = 0.0;
    m_parameters.humidity = 0.0;
    m_parameters.pressure = 0.0;
    m_parameters.windspeed = 0.0;
    m_parameters.winddir = 0.0;
    m_parameters.windspeedpeak = 0.0;

    AUTO_TRACE("WeatherStationImpl::WeatherStationImpl");
}

WeatherStationImpl::~WeatherStationImpl()
{
    ACS_DEBUG_PARAM("::WeatherStationImpl::~WeatherStationImpl", "Destroying %s...", getComponent()->getName());
    AUTO_TRACE("WeatherStationImpl::~WeatherStationImpl");
}


void WeatherStationImpl::cleanUp() throw (ACSErr::ACSbaseExImpl)
{
    if(m_controlThread_p != NULL)
    {
        m_controlThread_p->suspend();
        getContainerServices()->getThreadManager()->destroy(m_controlThread_p);
    }

    CharacteristicComponentImpl::cleanUp();
}

CORBA::Boolean  WeatherStationImpl::command(const char *cmd,CORBA::String_out answer)   throw (CORBA::SystemException)
{
    AUTO_TRACE("WeatherStationImpl::command()");
    IRA::CString out;
    IRA::CString in;
    CSecAreaResourceWrapper<WeatherSocket> line=m_socket->Get();
    bool res;

    try
    {
        m_parser->run(cmd,out);
        res=true;
    }
    catch (ParserErrors::ParserErrorsExImpl &ex)
    {
        res=false;
    }
    catch (ACSErr::ACSbaseExImpl& ex)
    {
        ex.log(LM_ERROR); // the errors resulting from the execution are logged here as stated in the documentation of CommandInterpreter interface, while the parser errors are never logged.
        res=false;
    }
    answer=CORBA::string_dup((const char *)out);
    return res;
}

void WeatherStationImpl::deleteAll()
{
    AUTO_TRACE("WeatherStationImpl::deleteAll");
}

void WeatherStationImpl::aboutToAbort() throw (ACSErr::ACSbaseExImpl)
{
    AUTO_TRACE("WeatherStationImpl::aboutToAbort()");
    if (m_controlThread_p!=NULL)
    {
        m_controlThread_p->suspend();
        getContainerServices()->getThreadManager()->destroy(m_controlThread_p);
    }
}

Weather::parameters WeatherStationImpl::getData()
{
    AUTO_TRACE("WeatherStationImpl::getData");
    baci::ThreadSyncGuard guardMeteoParameters(&m_meteoParametersMutex);
    return m_parameters;
}

void WeatherStationImpl::updateData() throw (ACSErr::ACSbaseEx,CORBA::SystemException)
{
    AUTO_TRACE("WeatherStationImpl::updateData");
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
        _EXCPT(ComponentErrors::UnexpectedExImpl,E,"WeatherStationImpl::updateData");
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
        _EXCPT(ComponentErrors::UnexpectedExImpl,E,"WeatherStationImpl::updateData");
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
        _EXCPT(ComponentErrors::UnexpectedExImpl,E,"WeatherStationImpl::updateData");
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
        _EXCPT(ComponentErrors::UnexpectedExImpl,E,"WeatherStationImpl::updateData");
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
        _EXCPT(ComponentErrors::UnexpectedExImpl,E,"WeatherStationImpl::updateData");
        throw E.getACSbaseEx();
    }

    try
    {
        ACSErr::Completion_var completion;
        mp.windspeedpeak = m_windspeedpeak->get_sync(completion.out());
    }
    catch (ACSErr::ACSbaseExImpl& E)
    {
        E.log(LM_DEBUG);
        throw E.getACSbaseEx();
    }
    catch (...)
    {
        _EXCPT(ComponentErrors::UnexpectedExImpl,E,"WeatherStationImpl::updateData");
        throw E.getACSbaseEx();
    }

    baci::ThreadSyncGuard guardMeteoParameters(&m_meteoParametersMutex);
    m_parameters = mp;
}

CORBA::Double WeatherStationImpl::getWindspeedPeak() throw (ACSErr::ACSbaseEx,CORBA::SystemException)
{
    AUTO_TRACE("WeatherStationImpl::getWindspeedPeak");
    double val;

    try
    {
        ACSErr::Completion_var completion;
        val = m_windspeedpeak->get_sync(completion.out());
    }
    catch (ACSErr::ACSbaseExImpl& E)
    {
        E.log(LM_DEBUG);
        throw E.getACSbaseEx();
    }
    catch (...)
    {
        _EXCPT(ComponentErrors::UnexpectedExImpl,E,"WeatherStationImpl::getWindspeedPeak");
        throw E.getACSbaseEx();
    }
    return val;
}

CORBA::Double WeatherStationImpl::getWindSpeedAverage() throw (ACSErr::ACSbaseEx,CORBA::SystemException)
{
    AUTO_TRACE("WeatherStationImpl::getWindspeedAverage");
    double windspeed;

    try
    {
        ACSErr::Completion_var completion;
        windspeed = m_windspeed->get_sync(completion.out());
    }
    catch (ACSErr::ACSbaseExImpl& E)
    {
        E.log(LM_DEBUG);
        throw E.getACSbaseEx();
    }
    catch (...)
    {
        _EXCPT(ComponentErrors::UnexpectedExImpl,E,"WeatherStationImpl::getWindspeedAverage");
        throw E.getACSbaseEx();
    }
    return windspeed;
}


CORBA::Double WeatherStationImpl::getHumidity() throw (ACSErr::ACSbaseEx,CORBA::SystemException)
{
    AUTO_TRACE("WeatherStationImpl::getHumidity");
    double val;

    try
    {
        ACSErr::Completion_var completion;
        val = m_humidity->get_sync(completion.out());
    }
    catch (ACSErr::ACSbaseExImpl& E)
    {
        E.log(LM_DEBUG);
        throw E.getACSbaseEx();
    }
    catch (...)
    {
        _EXCPT(ComponentErrors::UnexpectedExImpl,E,"WeatherStationImpl::getHumidity");
        throw E.getACSbaseEx();
    }
    return val;
}

CORBA::Double WeatherStationImpl::getTemperature() throw (ACSErr::ACSbaseEx,CORBA::SystemException)
{
    double temperature;

    AUTO_TRACE("WeatherStationImpl::getTemperature");
    try
    {
        ACSErr::Completion_var completion;
        temperature = m_temperature->get_sync(completion.out());
    }
    catch (ACSErr::ACSbaseExImpl& E)
    {
        E.log(LM_DEBUG);
        throw E.getACSbaseEx();
    }
    catch (...)
    {
        _EXCPT(ComponentErrors::UnexpectedExImpl,E,"WeatherStationImpl::getTemperature");
        throw E.getACSbaseEx();
    }
    return temperature;
}

CORBA::Double WeatherStationImpl::getPressure() throw (ACSErr::ACSbaseEx,CORBA::SystemException)
{
    double pressure;
    AUTO_TRACE("WeatherStationImpl::getPressure");

    try
    {
        ACSErr::Completion_var completion;
        pressure = m_pressure->get_sync(completion.out());
    }
    catch (ACSErr::ACSbaseExImpl& E)
    {
        E.log(LM_DEBUG);
        throw E.getACSbaseEx();
    }
    catch (...)
    {
        _EXCPT(ComponentErrors::UnexpectedExImpl,E,"WeatherStationImpl::getPressure");
        throw E.getACSbaseEx();
    }
    return pressure;
}

CORBA::Double WeatherStationImpl::getWindDir() throw (ACSErr::ACSbaseEx,CORBA::SystemException)
{
    double windir;
    AUTO_TRACE("WeatherStationImpl::getWindDir");

    try
    {
        ACSErr::Completion_var completion;
        windir = m_winddir->get_sync(completion.out());
    }
    catch (ACSErr::ACSbaseExImpl& E)
    {
        E.log(LM_DEBUG);
        throw E.getACSbaseEx();
    }
    catch (...)
    {
        _EXCPT(ComponentErrors::UnexpectedExImpl,E,"WeatherStationImpl::getWindDir");
        throw E.getACSbaseEx();
    }
    return windir;
}

double WeatherStationImpl::getWind()
{
    return 42.0;
}

void WeatherStationImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
    ACS_LOG(LM_FULL_INFO,"WeatherStationImpl::execute()",(LM_INFO,"COMPSTATE_OPERATIONAL"));
}

void WeatherStationImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
    ACS::Time timestamp;
    WeatherSocket *sock;
    ACSErr::Completion_var completion;
    IRA::CString mountInterface;
    ACS_LOG(LM_FULL_INFO,"WeatherStationImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZING"));
    try
    {
        double threadSleepTime;
        if (CIRATools::getDBValue(getContainerServices(),"IPAddress",ADDRESS) &&
            CIRATools::getDBValue(getContainerServices(),"port",PORT) &&
            CIRATools::getDBValue(getContainerServices(),"windthreshold",m_threshold) &&
            CIRATools::getDBValue(getContainerServices(),"MountInterface",mountInterface) &&
            CIRATools::getDBValue(getContainerServices(),"UpdatingThreadTime", threadSleepTime))
        {
            ACS_LOG(LM_FULL_INFO,"WeatherStationImpl::initialize()",(LM_INFO,"IP address %s, Port %d ",(const char *) ADDRESS,PORT));
        }
        else
        {
            ACS_LOG(LM_FULL_INFO,"WeatherStationImpl::initialize()",(LM_ERROR,"Error getting configuration from CDB" ));
            _EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"WeatherStationImpl::initialize()");
            throw dummy;
        }
        // "IDL:alma/Antenna/SRTMount:1.0"
        m_mount.setComponentName((const char *)mountInterface);
        m_mount.setContainerServices(getContainerServices());
        m_property=m_mount->elevationMode(); //throw ComponentErrors::CouldntGetComponentExImpl
        //m_elevationStatus=m_property->get_sync(completion)
        sock=new WeatherSocket(ADDRESS,PORT);
        m_socket =new CSecureArea<WeatherSocket>(sock);
        m_temperature=new RWdouble(getContainerServices()->getName()+":temperature", getComponent(), new DevIOTemperature(m_socket),true);
        m_winddir=new RWdouble(getContainerServices()->getName()+":winddir", getComponent(), new DevIOWinddir(m_socket),true);
        m_windspeed=new RWdouble(getContainerServices()->getName()+":windspeed", getComponent(), new DevIOWindspeed(m_socket),true);
        m_windspeedpeak=new RWdouble(getContainerServices()->getName()+":windspeedpeak", getComponent(), new DevIOWindspeedpeak(m_socket),true);
        m_humidity=new RWdouble(getContainerServices()->getName()+":humidity", getComponent(), new DevIOHumidity(m_socket),true);
        m_pressure=new RWdouble(getContainerServices()->getName()+":pressure", getComponent(), new DevIOPressure(m_socket),true);
        m_autoParkThreshold=new ROdouble(getContainerServices()->getName()+":autoparkThreshold", getComponent());

        m_autoParkThreshold->getDevIO()->write(m_threshold,timestamp);

        WeatherStationImpl* self_p =this;
        m_controlThread_p = getContainerServices()->getThreadManager()->create<CUpdatingThread, WeatherStationImpl*>("MeteoStation", self_p);
        m_controlThread_p->setSleepTime(threadSleepTime*10);
        //m_controlThread_p->setResponseTime(60*1000000);
        m_controlThread_p->resume();
        m_parser=new CParser<WeatherSocket>(sock,10);
        m_parser->add("getWindSpeedAverage",new function0<WeatherSocket,non_constant,double_type >(sock,&WeatherSocket::getWind),0 );
        //m_parser->add<0>("getTemperature",new function0<WeatherSocket,non_constant,double_type >(sock,&WeatherSocket::getTemperature) );

        m_parser->add("getHumidity",new function0<WeatherSocket,non_constant,double_type >(sock,&WeatherSocket::getHumidity),0 );
        m_parser->add("getPressure",new function0<WeatherSocket,non_constant,double_type >(sock,&WeatherSocket::getPressure),0 );
        m_parser->add("getWinDir",new function0<WeatherSocket,non_constant,double_type >(sock,&WeatherSocket::getWinDir),0 );
        m_parser->add("getWindSpeedPeak",new function0<WeatherSocket,non_constant,double_type >(sock,&WeatherSocket::getWindSpeedPeak),0 );
    }
    catch (std::bad_alloc& ex)
    {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"WeatherStationImpl::WeatherStationImpl()");
        throw dummy;
    }
    catch (ComponentErrors::ComponentErrorsExImpl& E)
    {
        E.log(LM_DEBUG);
        throw E;
    }

    try
    {
        CSecAreaResourceWrapper<WeatherSocket> sock=m_socket->Get();
        if (!sock->isConnected())
        {
            sock->connect();
            ACS_LOG(LM_FULL_INFO,"WeatherStationImpl::Disconnect()",(LM_DEBUG,"Connected  to Weather Station @%s:%d  ",(const char *) ADDRESS,PORT));
        }
    }
    catch (ComponentErrors::SocketErrorExImpl &x)
    {
        x.log(LM_DEBUG);
        throw x;
    }
    AUTO_TRACE("WeatherStationImpl::initialize");
}

ACS::RWdouble_ptr WeatherStationImpl::temperature() throw (CORBA::SystemException)
{
    if(m_temperature == 0)
    {
        return ACS::RWdouble::_nil();
    }

    ACS::RWdouble_var prop = ACS::RWdouble::_narrow(m_temperature->getCORBAReference());
    return prop._retn();
}

ACS::RWdouble_ptr WeatherStationImpl::winddir() throw (CORBA::SystemException)
{
    if(m_winddir == 0)
    {
        return ACS::RWdouble::_nil();
    }

    ACS::RWdouble_var prop = ACS::RWdouble::_narrow(m_winddir->getCORBAReference());
    return prop._retn();
}

ACS::RWdouble_ptr WeatherStationImpl::windspeed() throw (CORBA::SystemException)
{
    if(m_windspeed == 0)
    {
        return ACS::RWdouble::_nil();
    }

    ACS::RWdouble_var prop = ACS::RWdouble::_narrow(m_windspeed->getCORBAReference());
    return prop._retn();
}

ACS::RWdouble_ptr WeatherStationImpl::windspeedpeak() throw (CORBA::SystemException)
{
    if(m_windspeedpeak == 0)
    {
        return ACS::RWdouble::_nil();
    }

    ACS::RWdouble_var prop = ACS::RWdouble::_narrow(m_windspeedpeak->getCORBAReference());
    return prop._retn();
}

ACS::RWdouble_ptr WeatherStationImpl::humidity() throw (CORBA::SystemException)
{
    if(m_humidity == 0)
    {
        return ACS::RWdouble::_nil();
    }

    ACS::RWdouble_var prop = ACS::RWdouble::_narrow(m_humidity->getCORBAReference());
    return prop._retn();
}

ACS::RWdouble_ptr WeatherStationImpl::pressure() throw (CORBA::SystemException)
{
    if(m_pressure == 0)
    {
        return ACS::RWdouble::_nil();
    }

    ACS::RWdouble_var prop = ACS::RWdouble::_narrow(m_pressure->getCORBAReference());
    return prop._retn();
}

ACS::ROdouble_ptr WeatherStationImpl::autoparkThreshold() throw (CORBA::SystemException)
{
    if(m_autoParkThreshold == 0)
    {
        return ACS::ROdouble::_nil();
    }

    ACS::ROdouble_var prop = ACS::ROdouble::_narrow(m_autoParkThreshold->getCORBAReference());
    return prop._retn();
}

void WeatherStationImpl::parkAntenna()
{
    ACSErr::Completion_var completion;
    m_elevationStatus=m_property->get_sync(completion);

    if(m_elevationStatus != Antenna::ACU_STOW)
    {
        m_scheduler->stopSchedule();
        m_antennaBoss->park();
        ACS_LOG(LM_FULL_INFO,"WeatherStationImpl::parkAntenna()",(LM_WARNING,"Wind above limit:"));
    }
}

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(WeatherStationImpl)
/* ----------------------------------------------------------------*/
