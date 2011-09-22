#ifndef SRTWEATHERSTATIONIMPLIMPL_H
#define SRTWEATHERSTATIONIMPLIMPL_H

#include <baci.h>
#include <baciCharacteristicComponentImpl.h>
 
 #include <IRA>


///CORBA generated servant stub
#include <SRTWeatherStationS.h>

///Includes for each BACI property used in this example
#include <baciROdouble.h>
#include <baciRWdouble.h>


#include "DevIOtemperature.h"
#include "DevIOwinddir.h"
#include "DevIOpressure.h"
#include "DevIOwindspeed.h"
#include "DevIOwindspeedpeak.h"

#include "DevIOhumidity.h"
///Include the smart pointer for properties
#include <baciSmartPropertyPointer.h>

#include <ComponentErrors.h>
#include <ManagementErrors.h>
#include <SP_parser.h>

#include "acstime.h"
#include <cstdlib>
#include "SRTWeatherSocket.h"
#include "WeatherStationData.h"
// #include "WindCheckerThread.h"
#include <acsThread.h>

/*
const CString ADDRESS="192.167.8.13"; //DEBUG
const int THREADSLEEPSECONDS=10000; // 1 s in unit of 100ns, from thread sleep time*/


using namespace maci; 
using namespace std; 

using namespace baci; 
using namespace IRA;

 
/** 
 * @mainpage SRT Meteo Station
 * @date 01/11/2010
 * @version 1.1.0
 * @author <a href=mailto:spoppi@oa-cagliari.inaf.it>Sergio Poppi</a>
 * @remarks Last compiled under ACS 8.2
 * @remarks compiler version is 4.1.2
 * @todo:
 * 			- patch the corba errors;
 * 			- optimize the send/receive sequenze;
 * 			- parametrize the send commands
*/


 /**
The class implements the Srt Meteo Station.
Not all the paramters from the station have been implemented.

*/

using ACS::ThreadBase;
class SRTWeatherStationImpl;

class CWindCheckerThread : public ACS::Thread
{

 public:
	 CWindCheckerThread (const ACE_CString& name,
			SRTWeatherStationImpl*   weatherStation,
			const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
			const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime) ;

	 ~CWindCheckerThread() { ACS_TRACE("CWindCheckerThread::~CWindCheckerThread"); }
	 virtual void onStop();
	 virtual void onStart();
	 virtual void runLoop();

 private:
		int loopCounter_m;
		SRTWeatherStationImpl  * m_srtweatherstation_p;


};




class  SRTWeatherStationImpl:     public virtual CharacteristicComponentImpl,     //Standard component superclass
			 public virtual POA_Weather::SRTStation //CORBA servant stub
				   

{
public:
	/**
	Constructor 
	
	* @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
	* @param containerServices p/ACS_SRT/Common/Interfaces/MetrologyInterface/idlointer to the class that exposes all services offered by container
	
	
	*/
	SRTWeatherStationImpl(const ACE_CString &CompName,
			maci::ContainerServices *containerServices);


	/**
	 * Destructor.
	*/
	virtual ~SRTWeatherStationImpl(); 

	/**
	 * This method allows the client to interface the component by sending text commands. The command is parsed and executed according the
	 * defined command syntax and grammar. This method is required to implement the <i>Managment::CommandInterpreter</i> interface.
        * @throw CORBA::SystemException
	 * @throw ManagementErrors::CommandLineErrorEx 
	 * @param configCommand this string contains the string that will be executed
	 * @return  answer  the string that reports the command execution results or in case, errors
	 */
        virtual char * command(const char *configCommand) throw (CORBA::SystemException,ManagementErrors::CommandLineErrorEx);    

   
	/**
	*return  the all the data parameters formatted into a 
	*meteoparameters structure
	* @return  meteoparameters
	*/


	virtual Weather::parameters getData() throw (ACSErr::ACSbaseExImpl);

	virtual double getWindSpeedAverage() throw (ACSErr::ACSbaseExImpl);
	virtual double getWindspeedPeak() throw (ACSErr::ACSbaseExImpl);
	virtual double getWindDir() throw (ACSErr::ACSbaseExImpl);
	virtual double getHumidity() throw (ACSErr::ACSbaseExImpl);
	virtual double getTemperature() throw (ACSErr::ACSbaseExImpl);
	virtual double getPressure() throw (ACSErr::ACSbaseExImpl);

	virtual void initialize() throw (ACSErr::ACSbaseExImpl);
	virtual void	 cleanUp()throw (ACSErr::ACSbaseExImpl);
	virtual void aboutToAbort() throw (ACSErr::ACSbaseExImpl);
	

	/** 
	 * Returns a reference to the air temperature  property Implementation of IDL interface.
	 * @return pointer to read-write double temperature
	*/
	 virtual ACS::RWdouble_ptr  temperature ()
	throw (CORBA::SystemException);

	 /** 
	 * Returns a reference to the wind direction  property Implementation of IDL interface.
	 * @return pointer to read-write double wind direction
	*/
 	 virtual ACS::RWdouble_ptr  winddir ()
	throw (CORBA::SystemException);
	 /** 
	 * Returns a reference to the wind speed  property Implementation of IDL interface.
	 * @return pointer to read-write double wind speed
	*/

 	 virtual ACS::RWdouble_ptr  windspeed ()
	throw (CORBA::SystemException);
	 /** 
	 * Returns a reference to the wind speed  property Implementation of IDL interface.
	 * @return pointer to read-write double wind speed
	*/

 	 virtual ACS::RWdouble_ptr  windspeedpeak ()
	throw (CORBA::SystemException);
	 /** 
	 * Returns a reference to the relative humidity  property Implementation of IDL interface.
	 * @return pointer to read-write double wind speed
	*/

  	 virtual ACS::RWdouble_ptr  humidity ()
	throw (CORBA::SystemException);
	 /** 
	 * Returns a reference to the wind speed  property Implementation of IDL interface.
	 * @return pointer to read-write double relative humidity
	*/
  	 virtual ACS::RWdouble_ptr  pressure ()
	throw (CORBA::SystemException);
	double getWind();

 
private:
	
 	
 
	void deleteAll();
	CSecureArea<SRTWeatherSocket> *m_socket;
	CWindCheckerThread *m_controlThread_p;
	WeatherStationData m_wsdata;
	CError err;
        CString ADDRESS;
	unsigned int PORT;
	SmartPropertyPointer<RWdouble> m_temperature;
	SmartPropertyPointer<RWdouble> m_winddir;
	SmartPropertyPointer<RWdouble> m_windspeed;
	SmartPropertyPointer<RWdouble> m_windspeedpeak;
	SmartPropertyPointer<RWdouble> m_humidity;
	SmartPropertyPointer<RWdouble> m_pressure;
	SimpleParser::CParser<SRTWeatherSocket> * m_parser;
    void operator=(const SRTWeatherStationImpl&);
		
};


#endif
