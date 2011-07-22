#ifndef MedWeatherSTATIONIMPLIMPL_H
#define MedWeatherSTATIONIMPLIMPL_H

#include <baci.h>
#include <baciCharacteristicComponentImpl.h>
#include "MedWeatherStationImpl.h"
 


///CORBA generated servant stub
#include <MedWeatherStationS.h>

///Includes for each BACI property used in this example
#include <baciROdouble.h>
#include <baciRWdouble.h>


#include "DevIOtemperature.h"
#include "DevIOwinddir.h"
#include "DevIOpressure.h"
#include "DevIOwindspeed.h"

#include "DevIOhumidity.h"
///Include the smart pointer for properties
#include <baciSmartPropertyPointer.h>
#include <ComponentErrors.h>
#include "acstime.h"
 #include <IRA>
#include <cstdlib>
#include "MeteoSocket.h"
#include "MeteoData.h"


/*
const CString ADDRESS="192.167.8.13"; //DEBUG
const int THREADSLEEPSECONDS=10000; // 1 s in unit of 100ns, from thread sleep time*/


using namespace maci;
using namespace baci; 
using namespace std;

using namespace IRA;

 
/** 
 * @mainpage SRT Meteo Station
 * @date 01/11/2010
 * @version 1.0.0
 * @author <a href=mailto:spoppi@oa-cagliari.inaf.it>Sergio Poppi</a>
 * @remarks Last compiled under ACS 7.0.2
 * @remarks compiler version is 4.1.2
*/


 /**
The class implements the Srt Meteo Station.
Not all the paramters from the station have been implemented.


@todo add all the info returned by the HW
*/




class  MedWeatherStationImpl:     public virtual CharacteristicComponentImpl,     //Standard component superclass
			 public virtual POA_Weather::MedWeatherStation //CORBA servant stub
				   

{
public:
	/**
	Constructor 
	
	* @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
	* @param containerServices pointer to the class that exposes all services offered by container
	
	
	*/
	MedWeatherStationImpl(const ACE_CString &CompName,
			maci::ContainerServices *containerServices);


	/**
	 * Destructor.
	*/
	virtual ~MedWeatherStationImpl(); 


	virtual Weather::parameters getData() throw (ACSErr::ACSbaseExImpl);
	virtual CORBA::Double getTemperature() throw (ACSErr::ACSbaseExImpl);
	virtual	CORBA::Double getPressure() throw (ACSErr::ACSbaseExImpl);
	virtual	CORBA::Double getHumidity() throw (ACSErr::ACSbaseExImpl);
	virtual	CORBA::Double getWindspeed() throw (ACSErr::ACSbaseExImpl);


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

   
	/**
	*return  the all the data parameters formatted into a 
	*meteoparameters structure
	* @return  meteoparameters
	*/
 
private:
	
 	
 
	void deleteAll();
	CSecureArea<MeteoSocket> *m_socket;

	CError err;
        CString ADDRESS;
	unsigned int PORT;
	SmartPropertyPointer<RWdouble> m_temperature;
	SmartPropertyPointer<RWdouble> m_winddir;
	SmartPropertyPointer<RWdouble> m_windspeed;
	SmartPropertyPointer<RWdouble> m_humidity;
	SmartPropertyPointer<RWdouble> m_pressure;

    void operator=(const MedWeatherStationImpl&);
		
};


#endif
