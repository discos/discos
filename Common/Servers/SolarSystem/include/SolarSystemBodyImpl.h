#ifndef SOLSYSBODYIMPL_H_
#define SOLSYSBODYIMPL_H_
/**
* This code is under GNU General Public Licence (GPL)
*
* Sergio Poppi    09/05/2017  creation 
*
*
*/

/**
 * @mainpage Solar System bodies  tracking component implementation documentation
 * @date 09/05/2017
 * @version 1.02.0
 * @author <a. href=mailto:sergio.poppi@inaf.it>Sergio Poppi </a>
 * @remarks   
 
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


#include <baci.h>
#include "SolarSystemBodyS.h"
#include <String.h>
#include <Site.h>
#include <IRA>
#include <ComponentErrors.h>
#include <acscomponentImpl.h>
#include <AntennaErrors.h>
#include  <EphemGeneratorS.h>
#include <AntennaDefinitionsS.h>

class SolarSystemBodyImpl: public virtual acscomponent::ACSComponentImpl,public virtual POA_Antenna::SolarSystemBody

{
public:
      
      SolarSystemBodyImpl(const ACE_CString  &CompName,maci::ContainerServices *containerServices);
        
      virtual ~SolarSystemBodyImpl();
        /**
         * Called to give the component time to initialize itself. The component reads into
       configuration files/parameters, builds up connection.
         * Called before execute. It is implimented as a synchronous (blocking) call.
         */
        
        virtual void initialize() throw (ACSErr::ACSbaseExImpl);
        /*
         * Called after <i>initialize()</i> to tell the component that it has to be ready to
           accept incoming function calls at any time.
         * Must be implemented as a synchronous (blocking) call.
         * In this class the default implementation only logs the COMPSTATE_OPERTIONAL
         */
        virtual void execute() throw (ACSErr::ACSbaseExImpl);
        
        
        /** 
         * Called by the container before destroying the server in a normal situation. This function takes charge of releasing all resources.
         */
        virtual void cleanUp();
        
        
        /*
         * Called by the container in the case of error or emergency situation. This function tries to free all the resources even though 
         * there is no warrenty that the function is completly executed before the component is destroyed
         * */
        virtual void aboutToAbort();
        /*
         * This method is used to retrive all the attribute of the component in one shot.
         * @throw CORBA::SystemException
         * @param att that's the pointer to the structure containing all the data.
         */
        void getAttributes(Antenna::SolarSystemBodyAttributes_out att) throw (CORBA::SystemException);     
        
        
        void setBodyName(const char* bodyName) throw (CORBA::SystemException) ;

        /*
         * This method is used to apply new offsets to a givrn frame. Longitude is corrected for latitude cosine before use.
         * @throw CORBA::SystemException 
         * @throw AntennaErrors::AntennaErrorsEx
         * @param longitude new offset for the longitude in radians.
         * @param latitude new offset for the latitude in radians.
         * @param frame frame the offstes refer to, galactic frame not supported and an error is thrown.
         */     
        void setOffsets(CORBA::Double lon,CORBA::Double lat,Antenna::TCoordinateFrame frame) throw (CORBA::SystemException,AntennaErrors::AntennaErrorsEx);
        
        /*
         * It used, given a timestamp, to retrive the apparent topocentric coordinates in one shot.
         * @throw CORBA::SystemException 
         * @param time this parameter is used to pass the exact time the caller wants to know the topocentric coordinates.
         * @param az that's the returned value in radians of the azimuth for the requested time.
         * @param az that's the returned value in radians  of the elevation for the requested time.
         */
        void getHorizontalCoordinate(ACS::Time time, CORBA::Double_out az, CORBA::Double_out el) throw (CORBA::SystemException);
        
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
        
        /*
         * This method is used to check if the given coordinate at the given time belongs to  the trajectory generated by this component with
         * the tollerance of a tens of HPBW. If the given coordinate is the telescope coordinate we can say that the antenna is tracking.
         * @throw CORBA::SystemException 
         * @param time that the time mark the given coordinate refers to.
         * @param az that's the azimuth of the given coordinate (in radians)
         * @param el that's the elevation of the given coordinate (in radians)
         * @param HPBW that's the beam width at half power for the telescope. This parameter, given in radians, changes with
       the frequency so it must be provided by the caller.
         * @return true if the given point is in the generated trajectory.
         */
        bool checkTracking(ACS::Time time,CORBA::Double az,CORBA::Double el,CORBA::Double HPBW) throw (CORBA::SystemException);

    /**
     * This method is supposed to compute the flux of the moon. At present a 0.0 Jy is always returned.
     * @throw CORBA::SystemException
     * @param freq frequency observed
     * @param fwhm fwhm
     * @param flux computed flux (Jy)
     */ 
    void computeFlux(CORBA::Double freq, CORBA::Double fwhm, CORBA::Double_out flux) throw (CORBA::SystemException);
private:
  
      IRA::CString m_bodyName;
      IRA::CSite m_site;

        double m_longitude;
        double m_latitude;
        double m_height;

        /*
         * right ascension, declination, right ascension and declination offset
         * azimuth, elevation, azimuth and elevation offset
         */
        double m_ra, m_dec,ra_off, dec_off;
        double m_az, m_el,az_off, el_off;
        double m_glon, m_glat;
        double m_ra2000,m_dec2000;

        /*
         * Currently used frame for the offsets 
         */
        Antenna::TCoordinateFrame m_offsetFrame;
        double m_parallacticAngle;
        /*
         * This will contain the name of the component......
         */
        IRA::CString m_componentName;

        void BodyPosition(TIMEVALUE &time);
        
};
  



#endif /*SOLSYSBODYIMPL_H_*/
