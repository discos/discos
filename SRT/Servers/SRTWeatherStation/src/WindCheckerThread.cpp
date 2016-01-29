#include "SRTWeatherStationImpl.h"

CWindCheckerThread::CWindCheckerThread (const ACE_CString& name,
			SRTWeatherStationImpl  *  weatherStation,
			const ACS::TimeInterval& responseTime,
			const ACS::TimeInterval& sleepTime) :
			ACS::Thread(name)

		{
			ACS_TRACE("CWindCheckerThread::CWindCheckerThread");
			loopCounter_m = 0;
			m_srtweatherstation_p = (SRTWeatherStationImpl  *) weatherStation;


		}



void
CWindCheckerThread::runLoop()
{
	Weather::parameters wdata;
	double wind;

 	TIMEVALUE now;
	IRA::CIRATools::getTime(now);

    wdata = m_srtweatherstation_p->getData();
    wind=wdata.wind; // converts wind speed from m/s to km/h
     AUTO_TRACE("WindCheckerThread::runLoop()");
     if (wind > 0){
     
         m_srtweatherstation_p->parkAntenna();
         
         ACS_LOG(LM_FULL_INFO,"SRTWeatherStationImpl::initialize()",(LM_WARNING,"WINDSPEED=%f ", wdata.wind));
     }

}

void
CWindCheckerThread::onStop()
{
 

	AUTO_TRACE("WindCheckerThread::onStop()");

}
void
CWindCheckerThread::onStart()
{


	AUTO_TRACE("WindCheckerThread::onStart()");

}
