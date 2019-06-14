#ifndef SRTWEATHERSTATIONIMPLIMPL_H
#define SRTWEATHERSTATIONIMPLIMPL_H

#include <baci.h>
#include <baciCharacteristicComponentImpl.h>
 
 #include <IRA>


///CORBA generated servant stub
#include <GenericWeatherStationS.h>

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
#include "WeatherSocket.h"
#include "WeatherStationData.h"
#include <acsThread.h>
#include <AntennaBossC.h>
#include <SchedulerC.h>

#include "ComponentProxies" 

#include <ComponentProxy.h>
#include "AntennaDefinitionsC.h"



/*
const CString ADDRESS="192.167.8.13"; //DEBUG
const int THREADSLEEPSECONDS=10000; // 1 s in unit of 100ns, from thread sleep time*/

using namespace maci; 
using namespace std; 

using namespace baci; 
using namespace IRA;

 
/** 
 * @mainpage SRT Meteo Station
 * @date 28/11/2018
 * @version 1.1.0
 * @author <a href=mailto:sergio.poppi@inaf.it>Sergio Poppi</a>
 * @remarks Last compiled under ACS 2017.2
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
/*#include <SRTMountC.h>
#include "ComponentProxy.h"

namespace Antenna
{
    PROXY_COMPONENT(SRTMount);
};*/

#include <MountProxy.h>



using ACS::ThreadBase;
class WeatherStationImpl;

class CUpdatingThread : public ACS::Thread
{
public:
    CUpdatingThread (const ACE_CString& name,
                     WeatherStationImpl* weatherStation,
                     const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
                     const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

    ~CUpdatingThread() { ACS_TRACE("CUpdatingThread::~CUpdatingThread"); }
    virtual void onStop();
    virtual void onStart();
    virtual void runLoop();

private:
    int loopCounter_m;
    int m_threshold; // wind threshold in km/h
    WeatherStationImpl* m_weatherstation_p;
};


class  WeatherStationImpl : public virtual CharacteristicComponentImpl,         //Standard component superclass
			                public virtual POA_Weather::GenericWeatherStation   //CORBA servant stub
{
public:
    /**
    Constructor
    * @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
    * @param containerServices p/ACS_SRT/Common/Interfaces/MetrologyInterface/idlointer to the class that exposes all services offered by container
    */
    WeatherStationImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices);

    /**
    * Destructor.
    */
    virtual ~WeatherStationImpl(); 

    /**
    * This method allows the client to interface the component by sending text commands. The command is parsed and executed according the
    * defined command syntax and grammar. This method is required to implement the <i>Managment::CommandInterpreter</i> interface.
    * @throw CORBA::SystemException
    * @throw ManagementErrors::CommandLineErrorEx 
    * @param configCommand this string contains the string that will be executed
    * @return  answer  the string that reports the command execution results or in case, errors
    */
    virtual CORBA::Boolean command(const char *cmd, CORBA::String_out answer) throw (CORBA::SystemException);

    /**
    *return  the all the data parameters formatted into a meteoparameters structure
    * @return  meteoparameters
    */
    virtual Weather::parameters getData();
    virtual void updateData() throw (ACSErr::ACSbaseEx,CORBA::SystemException);

    virtual double getWindSpeedAverage() throw (ACSErr::ACSbaseEx,CORBA::SystemException);
    virtual double getWindspeedPeak() throw (ACSErr::ACSbaseEx,CORBA::SystemException);
    virtual double getWindDir() throw (ACSErr::ACSbaseEx,CORBA::SystemException);
    virtual double getHumidity() throw (ACSErr::ACSbaseEx,CORBA::SystemException);
    virtual double getTemperature() throw (ACSErr::ACSbaseEx,CORBA::SystemException);
    virtual double getPressure() throw (ACSErr::ACSbaseEx,CORBA::SystemException);

    virtual void initialize() throw (ACSErr::ACSbaseExImpl);
    virtual void execute() throw (ACSErr::ACSbaseExImpl);
    virtual void cleanUp()throw (ACSErr::ACSbaseExImpl);
    virtual void aboutToAbort() throw (ACSErr::ACSbaseExImpl);

    void parkAntenna(void);

    /**
    * Returns a reference to the air temperature  property Implementation of IDL interface.
    * @return pointer to read-write double temperature
    */
    virtual ACS::RWdouble_ptr temperature() throw (CORBA::SystemException);

    /**
    * Returns a reference to the wind direction property Implementation of IDL interface.
    * @return pointer to read-write double wind direction
    */
    virtual ACS::RWdouble_ptr winddir() throw (CORBA::SystemException);

    /**
    * Returns a reference to the wind speed property Implementation of IDL interface.
    * @return pointer to read-write double wind speed
    */
    virtual ACS::RWdouble_ptr windspeed() throw (CORBA::SystemException);

    /**
    * Returns a reference to the wind speed peak property Implementation of IDL interface.
    * @return pointer to read-write double wind speed peak
    */
    virtual ACS::RWdouble_ptr windspeedpeak() throw (CORBA::SystemException);

    /**
    * Returns a reference to the relative humidity property Implementation of IDL interface.
    * @return pointer to read-write double humidity
    */
    virtual ACS::RWdouble_ptr humidity() throw (CORBA::SystemException);

    /**
    * Returns a reference to the pressure property Implementation of IDL interface.
    * @return pointer to read-write double pressure
    */
    virtual ACS::RWdouble_ptr pressure() throw (CORBA::SystemException);

    virtual ACS::ROdouble_ptr autoparkThreshold() throw (CORBA::SystemException);

    double getWind();
    unsigned int m_threshold;
 
private:
    maci::ContainerServices * m_containerServices;
    Management::Scheduler_proxy m_scheduler;
    Antenna::AntennaBoss_proxy m_antennaBoss;
    Antenna::Mount_proxy m_mount; 
    //Antenna::SRTMount_proxy m_mount;        	
    void deleteAll();
    CSecureArea<WeatherSocket> *m_socket;
    CUpdatingThread *m_controlThread_p;
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
    SmartPropertyPointer<ROdouble> m_autoParkThreshold;
    SimpleParser::CParser<WeatherSocket> * m_parser;
    unsigned int m_elevationStatus;
    Antenna::ROTCommonModes_ptr m_property;
    void operator=(const WeatherStationImpl&);
    Weather::parameters m_parameters;
    BACIMutex m_meteoParametersMutex;
};

#endif
