#include "WeatherStationImpl.h"

CWindCheckerThread::CWindCheckerThread (const ACE_CString& name,
			WeatherStationImpl  *  weatherStation,
			const ACS::TimeInterval& responseTime,
			const ACS::TimeInterval& sleepTime) :
			ACS::Thread(name)

		{
			ACS_TRACE("CWindCheckerThread::CWindCheckerThread");
			loopCounter_m = 0;
			m_srtweatherstation_p = (WeatherStationImpl  *) weatherStation;
                        m_threshold=m_srtweatherstation_p->m_threshold;


		}



void
CWindCheckerThread::runLoop()
{
	Weather::parameters wdata;
	double wind;

 	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
  
     wdata = m_srtweatherstation_p->getData();
    wind=wdata.wind; 
     AUTO_TRACE("WindCheckerThread::runLoop()");
     if (wind >m_threshold){
     
         m_srtweatherstation_p->parkAntenna();
         ACS_LOG(LM_FULL_INFO,"CWindCheckerThread::initialize()",(LM_WARNING,"WINDSPEED=%f ", wdata.wind));

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
