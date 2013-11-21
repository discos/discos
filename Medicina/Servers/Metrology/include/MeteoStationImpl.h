#ifndef MeteoStationImpl_h
#define MeteoStationImpl_h


#include <baciCharacteristicComponentImpl.h>
 

///CORBA generated servant stub
#include <MeteoStationImplS.h>

///Includes for each BACI property used in this example
#include <baciROdouble.h>

///Include the smart pointer for properties
#include <baciSmartPropertyPointer.h>


using namespace baci; 

lass acsexmpl_EXPORT PowerSupply: public CharacteristicComponentImpl,     //Standard component superclass
				   public virtual POA_Metrology::MeteoStation    //CORBA servant stub
				   

{
public:
	/**
	Constructor 
	
	* @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
	* @param containerServices pointer to the class that exposes all services offered by container
	
	
	*/
	MeteoStationImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices);


	/**
	 * Destructor.
	*/
	virtual ~MeteoStationImpl(); 

	/**
	Enable the automatic parameters readout:
	Cyclicly it updates the meteo station properties.
	
	*/
	
	
	virtual void autoOn();
	/**
	Disable the automatic readouts
	
	*/
	
	virtual void autoOff();
	/**
	reset the meteoStation
	
	*/
	
	virtual void reset();
	
private:
	
	/** temperature */
	
	SmartPropertyPointer<ROWdouble> temperature;
	/** wind speed x direction */

	SmartPropertyPointer<ROWdouble> xwind;
	/** wind speed y direction */
	
	SmartPropertyPointer<ROWdouble> ywind;
	/** wind speed z direction */
	
	SmartPropertyPointer<ROWdouble> zwind;
	/** pressure */
	
	SmartPropertyPointer<ROWdouble> pressure;
	/** opacity */
	
	SmartPropertyPointer<ROWdouble> opacity;
	/** Sky brightness temperature */
	
	SmartPropertyPointer<ROWdouble> skyBrightnessTemperature;
		 
		
		 
	
		
}











#endif
