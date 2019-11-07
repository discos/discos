#include <slamac.h>
#include <slalib.h>
#include "RefractionCore.h"

#define _GET_STRING_ATTRIBUTE(ATTRIB,DESCR,FIELD) { \
	CString tmps; \
	if (!CIRATools::getDBValue(Services,ATTRIB,tmps)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"CRefractionCore::initialize()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
	} \
	else { \
		FIELD=tmps; \
		ACS_DEBUG_PARAM("CRefractionCore::initialize()",DESCR" %s",(const char*)tmps); \
	} \
}

CRefractionCore::CRefractionCore(ContainerServices *service,acscomponent::ACSComponentImpl *me) : 
	m_services(service)
{
	m_temperature=0.0;
	m_humidity=0.5;
	m_pressure=1000;
}

CRefractionCore::~CRefractionCore()
{
}

void CRefractionCore::initialize(maci::ContainerServices *Services) throw (ComponentErrors::CDBAccessExImpl)
{
	m_byebye = false;
	_GET_STRING_ATTRIBUTE("ObservatoryInterface","Observatory interface is ",m_observatoryComp);
	_GET_STRING_ATTRIBUTE("WeatherStationInterface","Weather station interface is ",m_weatherComp);
	ACS_LOG(LM_FULL_INFO,"CRefractionCore::initialize()",(LM_INFO,"CRefractionCore::initialize"));	
}

void CRefractionCore::execute() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl)
{
	Antenna::TSiteInformation_var site;
	m_observatory=Antenna::Observatory::_nil();

	try {
		m_observatory=m_services->getDefaultComponent<Antenna::Observatory>((const char*)m_observatoryComp);
	}
	catch (maciErrType::CannotGetComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CRefractionCore::execute()");
		Impl.setComponentName((const char*)m_observatoryComp);
		throw Impl;
	}
	catch (maciErrType::NoPermissionExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CRefractionCore::execute()");
		Impl.setComponentName((const char*)m_observatoryComp);
		throw Impl;
	}
	catch (maciErrType::NoDefaultComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CRefractionCore::execute()");
		Impl.setComponentName((const char*)m_observatoryComp);
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
	ACS_LOG(LM_FULL_INFO,"CBossCore::execute()",(LM_INFO,"OBSERVATORY_RELEASED"));
	m_meteodata = Weather::GenericWeatherStation::_nil();
	try {
		m_meteodata=m_services->getDefaultComponent<Weather::GenericWeatherStation>((const char *)m_weatherComp);
	}
	catch (maciErrType::CannotGetComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CRefractionCore::execute()");
		Impl.setComponentName((const char*)m_weatherComp);
		throw Impl;
	}
	catch (maciErrType::NoPermissionExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CRefractionCore::execute()");
		Impl.setComponentName((const char*)m_weatherComp);
		throw Impl;
	}
	catch (maciErrType::NoDefaultComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CRefractionCore::execute()");
		Impl.setComponentName((const char*)m_weatherComp);
		throw Impl;
	}
	ACS_LOG(LM_FULL_INFO, "CRefractionCore::execute()", (LM_INFO,"WEATHERSTATION_LOCATED"));
}

void CRefractionCore::cleanUp()
{
	baci::ThreadSyncGuard guardMeteoData(&m_meteoDataMutex);
	baci::ThreadSyncGuard guardParameters(&m_parametersMutex);
	try {
		m_services->releaseComponent((const char*)m_meteodata->name());
	}
	catch (maciErrType::CannotReleaseComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CRefractionCore::cleanUp()");
		Impl.setComponentName((const char *)m_meteodata->name());
		Impl.log(LM_DEBUG);
	}
}

void CRefractionCore::getCorrection(double obsZenithDistance,double waveLen, double &corZenithDistance)
{
	AUTO_TRACE("CRefractionCore::getCorrection()");
	//double elevation;
	baci::ThreadSyncGuard guardParametes(&m_parametersMutex);
	//elevation = 90.0 - obsZenithDistance*DR2D;
	//if (elevation >= 0.0 && elevation <= 90.0) {
	if ((obsZenithDistance>=0.0) && (obsZenithDistance<=(DPI/2.0))) {
		double hm = m_site.getHeight(); // meters
		double tdk = m_temperature + 273.0;
		double wl = waveLen*1000000; //micro meters
		double phi = m_site.getLatitude(); // radians
		double tlr = 0.0065;
		double eps = 1e-8;

		slaRefro(obsZenithDistance, hm, tdk, m_pressure, m_humidity, wl, phi, tlr, eps, &corZenithDistance);
	}
	else corZenithDistance = 0.0;
}

void CRefractionCore::getMeteoParameters()
{
	AUTO_TRACE("CRefractionCore::getMeteoParameters()");
	Weather::parameters pars;

	try {
		// keep the meteoData mutex for thread execution to avoid long waits
		// before to call the getData function from meteo component
		baci::ThreadSyncGuard guardMeteoData(&m_meteoDataMutex);
		pars=m_meteodata->getData();
		//m_temperature = m_meteodata->getTemperature();
		//m_humidity = (m_meteodata->getHumidity())/100.0; // because slaRefro needs humidity value beetwen 0.0 and 1.0
		//m_pressure = m_meteodata->getPressure();

		baci::ThreadSyncGuard guardParameters(&m_parametersMutex);
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
