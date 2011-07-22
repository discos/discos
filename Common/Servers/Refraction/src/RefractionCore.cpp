#include <slamac.h>
#include <slalib.h>
#include "RefractionCore.h"

CRefractionCore::CRefractionCore(ContainerServices *service,acscomponent::ACSComponentImpl *me) : 
	m_services(service)
{
}

CRefractionCore::~CRefractionCore()
{
}

void CRefractionCore::initialize()
{
    m_byebye = false;
	ACS_LOG(LM_FULL_INFO,"CRefractionCore::initialize()",(LM_INFO,"CRefractionCore::initialize"));	
}

void CRefractionCore::execute() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl)
{
	IRAIDL::TSiteInformation_var site;

	m_observatory=Antenna::Observatory::_nil();
	try {
		m_observatory=m_services->getComponent<Antenna::Observatory>("ANTENNA/Observatory");
	}
	catch (maciErrType::CannotGetComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CRefractionCore::execute()");
		Impl.setComponentName("ANTENNA/Observatory");
		throw Impl;
	}
	ACS_LOG(LM_FULL_INFO,"CRefractionCore::execute()",(LM_INFO,"CRefractionCore::OBSERVATORY_LOCATED"));
	try	{	
		site = m_observatory->getSiteSummary();
	}
	catch (CORBA::SystemException& ex)	{
		_EXCPT(ComponentErrors::CORBAProblemExImpl,__dummy,"CRefractionCore::execute()");
		__dummy.setName(ex._name());
		__dummy.setMinor(ex.minor());
		throw __dummy;
	}
	m_site=CSite(site.out());
	ACS_LOG(LM_FULL_INFO,"CRefractionCore::execute()",(LM_INFO,"CRefractionCore::SITE_INITIALIZED"));
	try {
		m_services->releaseComponent((const char*)m_observatory->name());
	}
	catch  (maciErrType::CannotReleaseComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CRefractionCore::execute()");
		Impl.setComponentName((const char*)m_observatory->name());
		throw Impl;
	}
    m_meteodata = Weather::GenericWeatherStation::_nil();
    try {
		m_meteodata = m_services->getComponent<Weather::GenericWeatherStation>("WEATHERSTATION/WeatherStation");
	}
	catch (maciErrType::CannotGetComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CRefractionCore::execute()");
		Impl.setComponentName("METROLOGY/Meteo");
		throw Impl;
	}
	ACS_LOG(LM_FULL_INFO, "CRefractionCore::execute()", (LM_INFO,"CRefractionCore::WEATHERSTATION_LOCATED"));
}

void CRefractionCore::cleanUp()
{
    try {
		m_services->releaseComponent((const char*)m_meteodata->name());
	}
	catch (maciErrType::CannotReleaseComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CRefractionCore::cleanUp()");
		Impl.setComponentName((const char *)m_meteodata->name());
		Impl.log(LM_DEBUG);
	}
}

void CRefractionCore::getCorrection(double obsZenithDistance, double *corZenithDistance)
{
    AUTO_TRACE("CRefractionCore::getCorrection()");
	double elevation;
	
	elevation = 90.0 - obsZenithDistance*DR2D;
	if (elevation >= 0.0 && elevation <= 90.0) {
		double hm = m_site.getHeight(); // meters
		double tdk = m_temperature + 273.0;
		double wl = 200.0;
		double phi = m_site.getLatitude(); // radians
		double tlr = 0.0065;
		double eps = 1e-8;
		
		slaRefro(obsZenithDistance, hm, tdk, m_pressure, m_humidity, wl, phi, tlr, eps, corZenithDistance);
	}
	else
		corZenithDistance = 0;
}

void CRefractionCore::getMeteoParameters()
{
    AUTO_TRACE("CRefractionCore::getMeteoParameters()");
    Weather::parameters pars;
    try {
        pars=m_meteodata->getData();
    	//m_temperature = m_meteodata->getTemperature();
    	//m_humidity = (m_meteodata->getHumidity())/100.0; // because slaRefro needs humidity value beetwen 0.0 and 1.0
    	//m_pressure = m_meteodata->getPressure();
	m_temperature=pars.temperature;
	m_humidity=pars.humidity/100.0;
	m_pressure=pars.pressure;
	//printf("t: %lf h: %lf p: %lf\n",m_temperature,m_humidity,m_pressure);
    }
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRefractionCore::getMeteoParameters()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		impl.log(LM_ERROR);
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRefractionCore::getMeteoParameters()");
		impl.log(LM_ERROR);
	}
}
