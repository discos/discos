#ifndef SKYSOURCEIMPL_H_
#define SKYSOURCEIMPL_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                          when        What                                              */
/* Andrea Orlati(a.orlati@ira.inaf.it)           13/09/2007  Creation                                         */
/* Andrea Orlati(a.orlati@ira.inaf.it)           06/02/2008  Fixed a bug: the dut1 was not loaded into the CDateTime objects       */
/* Andrea Orlati(a.orlati@ira.inaf.it)           15/05/2008  Added the implemantation of checkTracking */
/* Andrea Orlati(a.orlati@ira.inaf.it)           30/08/2010  Added generic method setOffsets in place of specific methods setHorizontalOffsets().*/
/* Andrea Orlati(a.orlati@ira.inaf.it)           18/02/2011  Fixed a bug, the source catalog table was not freed when the component closeup */
/* Carlo Migoni (migoni@oa-cagliari.inaf.it)    04/10/2011  Added computeFlux() method */
/* Andrea Orlati(a.orlati@ira.inaf.it)           15/02/2012  implementation of getAllCoordinates() method */
/* Andrea Orlati(a.orlati@ira.inaf.it)           07/09/2012  fixed a bug when reading negative declination '-00' from source catalog*/

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/** 
 * @mainpage SkySource component implementation documentation
 * @date 05/05/2011
 * @version 1.41.0
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>
 * @remarks Last compiled under ACS 7.0.2
 * @remarks compiler version is 3.4.6
*/
#include <SkySourceS.h>
#include <ComponentErrors.h>
#include <acscomponentImpl.h>
#include <SkySource.h>
//#include <AntennaDefinitionsS.h> 
#include <DBTable.h>
#include <AntennaErrors.h>
#include <ComponentErrors.h>
#include <SourceFlux.h>

using namespace IRA;

class SkySourceImpl: public virtual acscomponent::ACSComponentImpl,public virtual POA_Antenna::SkySource
{
public:
	/** 
	 * Constructor.
	 * @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
	 * @param containerServices pointer to the class that exposes all services offered by container
	*/
	SkySourceImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices);

	/**
	 * Destructor.
	*/
	virtual ~SkySourceImpl(); 
	
	/** 
	 * Called to give the component time to initialize itself. The component reads into configuration files/parameters, builds up connection. 
	 * Called before execute. It is implemented as a synchronous (blocking) call.
	 * @throw ComponentCommonErrors::MemoryAllocationExImpl
	*/
	virtual void initialize() throw (ACSErr::ACSbaseExImpl);

	/**
 	 * Called after <i>initialize()</i> to tell the component that it has to be ready to accept incoming functional calls at any time. 
	 * Must be implemented as a synchronous (blocking) call. 	 
	 * In this class the default implementation only logs the COMPSTATE_OPERATIONAL
	*/
	virtual void execute() throw (ACSErr::ACSbaseExImpl);
	 
	/** 
	 * Called by the container before destroying the server in a normal situation. This function takes charge of releasing all resources.
	*/
	virtual void cleanUp();
	 
	/** 
	 * Called by the container in case of error or emergency situation. This function tries to free all resources even though there is no
	 * warranty that the function is completely executed before the component is destroyed.
	*/	
	virtual void aboutToAbort();
	
	/**
	 * This method is used to retrieve all the attribute of the component in one shot.
	 * @throw CORBA::SystemException 
	 * @param att that's the pointer to the structure containing all the data.
	*/
	void getAttributes(Antenna::SkySourceAttributes_out att) throw (CORBA::SystemException);
	
	/**
	 * This method is used to apply new offsets to the generated coordinates in the given frame system. Offsets relative to other frame are cleared.
	 * @throw CORBA::SystemException 
	 * @thorw AntennaErrors::AntennaErrorsEx
	 * @param lat new offset for the azimuth in radians.
	 * @param lon new offset for the elevation in radians.
	 * @param frame allows to choose the frame   
	*/
	void setOffsets(CORBA::Double lon,CORBA::Double lat,Antenna::TCoordinateFrame frame) throw (CORBA::SystemException,AntennaErrors::AntennaErrorsEx);	
				
	/**
	 * It used, given a timestamp, to retrieve the apparent horizontal coordinate in one shot.
	 * @throw CORBA::SystemException 
	 * @param time this parameter is used to pass the exact time the caller wants to know the horizontal coordinates.
	 * @param az that's the returned value in radians of the azimuth for the requested time.
	 * @param el that's the returned value in radians  of the elevation for the requested time.
	*/
	void getHorizontalCoordinate(ACS::Time time,CORBA::Double_out az,CORBA::Double_out el) throw (CORBA::SystemException);

	 /**
	  *  It used, given a timestamp, to retrieve the all apparent coordinates in all frames in one shot.
	 * @throw CORBA::SystemException
	 * @param time this parameter is used to pass the exact time the caller wants to know the horizontal coordinates.
	 * @param az that's the returned value in radians of the azimuth for the requested time.
	 * @param el that's the returned value in radians  of the elevation for the requested time.
	 * @param ra that's the returned value in radians  of the elevation for the requested time.
	 * @param dec that's the returned value in radians  of the right ascension for the requested time.
	 * @param jepoch julian epoch the returned equatorial point refers to
	 * @param lon that's the returned value in radians  of the galactic longitude for the requested time.
	 * @param lat that's the returned value in radians  of the galactic latitude  for the requested time.
	*/
	void getAllCoordinates(ACS::Time time,CORBA::Double_out az,CORBA::Double_out el,CORBA::Double_out ra,CORBA::Double_out dec,CORBA::Double_out jepoch,
			CORBA::Double_out lon,CORBA::Double_out lat) throw (CORBA::SystemException);

	/**
	 * This method is used to check if the given coordinate at the given time belongs to  the trajectory generated by this component with
	 * the tollerance of a tens of HPBW. If the given coordinate is the telescope coordinate we can say that the antenna is tracking.
	 * @throw CORBA::SystemException 
	 * @param time that the time mark the given coordinate refers to.
	 * @param az that's the azimuth of the given coordinate (in radians)
	 * @param el that's the elevation of the given coordinate (in radians)
	 * @param HPBW that's the beam width at half power for the telescope. This parameter, given in radians, changes with
	 *                 the frequency so it must be provided by the caller.
	 * @return true if the given point is in the generated trajectory.
	 */
	CORBA::Boolean checkTracking(ACS::Time time,CORBA::Double az,CORBA::Double el,CORBA::Double HPBW) throw (CORBA::SystemException);
	
	/**
	 * This method is used to load a new source from the catalog.
	 * @throw CORBA::SystemException 
	 * @throw AntennaErrors::SourceNotFoundEx
	 * @throw ComponentErrors:CDBFieldFormatEx
	 * @param sourceName this string represent the source name, that means the keyword used to search the source in the catalog.
	*/
	void loadSourceFromCatalog(const char* sourceName) throw (CORBA::SystemException,
			AntennaErrors::AntennaErrorsEx,ComponentErrors::ComponentErrorsEx);
	
	/**
	 * Sets the source as fixed position in the topocentric system
	 * @throw CORBA::SystemException 
	 * @param sourceName target identifier
	 * @param az fixed azimuth position in radians
	 * @param el fixed elevation position in radians
	*/
	void setFixedPoint(const char* sourceName,CORBA::Double az,CORBA::Double el) throw (CORBA::SystemException);
		
	/** 
	 * This method allows to setup a new source by giving its galactic coordinates. 
	*/
	void setSourceFromGalactic(const char* sourceName,CORBA::Double longitude,CORBA::Double latitude) throw (CORBA::SystemException);

	/**
	 * This method allows to setup a new source by giving its equatorial coordinates.
	 * @throw CORBA::SystemException 
	 * @param sourceName this is the name of the source  
	 * @param ra right ascension of the source in radians
	 * @param dec declination of the source in radians
	 * @param equinox equinox which the above coordinates refer to
	 * @param dra proper motion in right ascension in mas/year (milli seconds of arc year (tropical) if FK4 is used, per 
	 *        year (julian) if Fk5 is used). dRA/dt*cos(DEC) is assumed
	 * @param ddec proper motion in declination in mas/year (milli seconds of arc per year (tropical) if FK4 is used, 
	 * 	      per year (julian) if Fk5 is used)
	 * @param parallax parallax in milli arcseconds.
	 * @param rvel radial velocity in Km/sec ( positive value means the source is moving away)
	*/
	void setSourceFromEquatorial(const char *sourceName,CORBA::Double ra,CORBA::Double dec,Antenna::TSystemEquinox equinox,CORBA::Double dra,
	   CORBA::Double ddec,CORBA::Double parallax,CORBA::Double rvel) throw (CORBA::SystemException);

    /**
     * This method compute the flux of a source based on its flux and model
     * parameters read from the CDB.
     * @throw CORBA::SystemException
     * @param freq frequency observed
     * @param fwhm fwhm
     * @param flux computed flux
     */ 
    void computeFlux(CORBA::Double freq, CORBA::Double fwhm, CORBA::Double_out flux) throw (CORBA::SystemException);


private:
    /**
     * struct containing flux and model parameters
     */
    flux_ds fluxParam;

    /**
     * This object contains the algorithm to calculate the flux
     */ 
    CSourceFlux sourceFlux;

	/**
	 * The mutex allows to manage the mutual exclusion
	*/
	BACIMutex m_mutex;
	
	/** 
	 * This object is in charge of the coordinates generation
	*/
	CSkySource m_source;
	
	/**
	 * This will contains the name or the identifier of the source
	*/
	CString m_sourceName;
	/**
	 * This object is in charge of storing the site information
	 */
	CSite m_site;
	/**
	 * The current DUT1 value
	*/
	double m_dut1;
	/**
	 * This will contain the name of the component......
	 */
	IRA::CString m_componentName;
	/**
	 * This is the table used to access the cDB in order to read the source catalog
	*/
	IRA::CDBTable* m_sourceCatalog;
	
	/**
	 * Used internally to setup the sky source
	*/
	void setSource(CString sourceName,double ra,double dec,Antenna::TSystemEquinox equinox,double dra,
	   double ddec,double parallax,double rvel);

};



#endif /*SKYSOURCEIMPL_H_*/
