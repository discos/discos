#include "WeatherStationImpl.h"

CUpdatingThread::CUpdatingThread (const ACE_CString& name,
    WeatherStationImpl*  weatherStation,
    const ACS::TimeInterval& responseTime,
	const ACS::TimeInterval& sleepTime) :
	ACS::Thread(name)
{
    ACS_TRACE("CUpdatingThread::UpdatingThread");
    loopCounter_m = 0;
	m_weatherstation_p = (WeatherStationImpl*) weatherStation;
    m_threshold = m_weatherstation_p->m_threshold;
}

void CUpdatingThread::runLoop()
{
    AUTO_TRACE("UpdatingThread::runLoop()");

    m_weatherstation_p->updateData();
    Weather::parameters w_pars = m_weatherstation_p->getData();

    if(w_pars.windspeed > m_threshold)
    {
        m_weatherstation_p->parkAntenna();
        ACS_LOG(LM_FULL_INFO,"UpdatingThread::runLoop()",(LM_WARNING,"WINDSPEED=%f ", w_pars.windspeed));
    }
}

void CUpdatingThread::onStart()
{
	AUTO_TRACE("UpdatingThread::onStart()");
}

void CUpdatingThread::onStop()
{
	AUTO_TRACE("UpdatingThread::onStop()");
}
