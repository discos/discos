#ifndef _REFRACTIONCORE_H_
#define _REFRACTIONCORE_H_

/* ****************************************************************** */
/* OAC Osservatorio Astronomico di Cagliari                           */
/* $Id: RefractionCore.h,v 1.4 2011-04-18 12:57:06 a.orlati Exp $ */
/*                                                                    */
/* This code is under GNU General Public Licence (GPL).               */
/*                                                                    */
/* Who                                  when        What              */
/* Carlo Migoni (migoni@ca.astro.it)   26/01/2009  Creation           */

#include <acscomponentImpl.h>
#include <ComponentErrors.h>
#include <IRA>
#include <Site.h>
#include <acsContainerServices.h>
#include <ObservatoryC.h>
#include <GenericWeatherStationC.h>
#include <acsThread.h>
#include <IRA>

using namespace IRA;
using namespace maci;

class RefractionImpl;
class CRefractionWorkingThread;

/**
 * This class models the Boss datasets and functionalities. 
 * @author <a href=mailto:migoni@ca.astro.it>Migoni Carlo</a>
 * Osservatorio Astronomico di Cagliari, Italia
 * <br> 
 */
class CRefractionCore {
	friend class RefractionImpl;
	friend class CRefractionWorkingThread;
public:
	/**
	 * Constructor. Default Constructor.
	 * @param service pointer to the continaer services.
	 * @param me pointer to the component itself
	*/
	CRefractionCore(ContainerServices *service,acscomponent::ACSComponentImpl *me);

	/** 
	 * Destructor.
	*/ 
	virtual ~CRefractionCore();
	
	/**
	 * This function initializes the boss core, all preliminary operation are performed here.
	*/
	virtual void initialize(maci::ContainerServices *Services) throw (ComponentErrors::CDBAccessExImpl);

	/**
	 * This function starts the boss core  so that it will available to accept operations and requests.
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	*/
	virtual void execute() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl);
	
	/**
	 * This function performs all the clean up operation required to free al the resources allocated by the class
	*/
	virtual void cleanUp();

	/**
	 * This function computes corrected zenith distance (radians) due to refraction. 
	 * @param observed observed zenith distance
	 * @param waveLen  wave length in meters
	 * @param corrected corrected zenith distance
	*/
    void getCorrection(double observed, double waveLen, double *corrected);

	/**
	 * This function gets meteo parameters from metrology component every one
     * minute
	*/
    void getMeteoParameters();
	

    bool m_byebye;

    void byebye() {m_byebye=true;};

private:
	ContainerServices* m_services;

	/**
	 * This object is in charge of storing the site information
	*/
	CSite m_site;

	/**
	 * This is the reference to the meteodata component
	 */
	Weather::GenericWeatherStation_var m_meteodata;

	/**
	 * This is the reference to the observatory component
	 */
	Antenna::Observatory_var m_observatory;
	
	IRA::CString m_weatherComp;
	IRA::CString m_observatoryComp;

    double m_temperature;
    double m_humidity;
    double m_pressure;

	/**
	 * Thread sync mutexes
	 */
	BACIMutex m_meteoDataMutex;
	BACIMutex m_parametersMutex;

};

#endif /*REFRACTIONBOSSCORE_H_*/
