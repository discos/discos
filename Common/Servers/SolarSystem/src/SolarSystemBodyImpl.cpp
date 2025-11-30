#include "SolarSystemBodyImpl.h"
//#include <IRA>
#include <Definitions.h>
#include <ObservatoryS.h>
#include <slamac.h>
#include <slalib.h>
#include <LogFilter.h>
//#include <baci.h>
//#include <acstimeEpochHelper.h>
//#include <Site.h>
//#include <DateTime.h>
//#include <SkySource.h>
#include <iostream>
//#include <Site.h>
#include <exception>
#define R2D 57.29577951308232
#include <memory>


using namespace ComponentErrors;
using namespace IRA;
using namespace baci;


SolarSystemBodyImpl::SolarSystemBodyImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) :
        acscomponent::ACSComponentImpl(CompName, containerServices)
{
        AUTO_TRACE("SolarSystemBodyImpl::SolarSystemBodyImpl()");
        m_componentName=CString(CompName);
        m_bodyName="Unset";
}

SolarSystemBodyImpl::~SolarSystemBodyImpl()
{
        AUTO_TRACE("SolarSystemBodyImpl::~SolarSystemBodyImpl()");

}

void SolarSystemBodyImpl::initialize()  
{
        AUTO_TRACE("SolarSystemBodyImpl::initialize()");
 
      	m_ra_off = m_dec_off = 0.0;
	      m_az_off = m_el_off = 0.0;
	      m_offsetFrame=Antenna::ANT_HORIZONTAL;
         ACS_LOG(LM_FULL_INFO, "SolarSystemBodyImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZING"));
}

void SolarSystemBodyImpl::cleanUp()
{
        AUTO_TRACE("SolarSystemBodyImpl::cleanUp()");
        //delete m_sitex;    //delete not needed for smart pointers
        //delete m_body_xephem; //delete not needed for smart pointers
        ACSComponentImpl::cleanUp();
}

void SolarSystemBodyImpl::aboutToAbort()
{
        AUTO_TRACE("SolarSystemBodyImpl::aboutToAbort()");
}

void SolarSystemBodyImpl::execute()  
{
        AUTO_TRACE("SolarSystemBodyImpl::execute()");
        CError error;
        Antenna::TSiteInformation_var site;
 
 	     Antenna::Observatory_var observatory=Antenna::Observatory::_nil();
	     
	     try {
                 observatory=getContainerServices()->getComponent<Antenna::Observatory>("ANTENNA/Observatory");
	         
	     }
	     catch (maciErrType::CannotGetComponentExImpl & ex){
		         _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"SolarSystemBodyImpl::execute()");
		         Impl.setComponentName("ANTENNA/Observatory");
	            throw Impl;
	     }
	
	     ACS_LOG(LM_FULL_INFO,"SolarSystemBodyImpl::execute()", (LM_INFO, (const char *)CString(m_componentName+"::OBSERVATORY_LOCATED")));
	     try {
		        site=observatory->getSiteSummary();
	     }
	     catch (CORBA::SystemException& ex)		{
		        _EXCPT(ComponentErrors::CORBAProblemExImpl,_dummy, "SolarSystemBodyImpl::execute()");
		        _dummy.setName(ex._name());
		        _dummy.setMinor(ex.minor());
		         throw _dummy;
	     }
         m_site=CSite(site.out());
         m_source=IRA::CSkySource();  // dummy obj for coordinate conversion
         
	     m_dut1=site->DUT1;
	     m_longitude=site->longitude;
	     m_latitude=site->latitude;
	     m_height=site->height;
        m_sitex.reset(new xephemlib::Site());  //smart pointer std::uniq needs reset to initialize.  
	     m_sitex-> setCoordinate(site->longitude,site->latitude,site->height); //coordinates in degrees.
 
        m_body_xephem.reset(new xephemlib::SolarSystemBody());		  
		   
	     try {
		          getContainerServices()->releaseComponent((const char*)observatory->name());
         	 }
	     catch  (maciErrType::CannotReleaseComponentExImpl& ex) {
		         _ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"SolarSystemBodyImpl::initialize()");
		         Impl.setComponentName("ANTENNA/Observatory");
		         throw Impl;
	     } 
	     ACS_LOG(LM_FULL_INFO,"SolarSystemBodyImpl::execute()", (LM_INFO,"SITE_INITIALIZED"));        
}



void SolarSystemBodyImpl::getAttributes(Antenna::SolarSystemBodyAttributes_out att)  
{
    AUTO_TRACE("SolarSystemBodyImpl::getAttributes()");

    // OPTIMIZATION 1: Exception Safety (RAII)
    // Use a smart pointer (_var) instead of a raw pointer. 
    // If BodyPosition throws an exception, this ensures the memory is freed automatically.
    Antenna::SolarSystemBodyAttributes_var safe_att = new Antenna::SolarSystemBodyAttributes();

    // OPTIMIZATION 2: Pre-fill constant values OUTSIDE the lock
    // These values do not depend on shared state, so we don't need to block other threads to set them.
    safe_att->userLatitudeOffset = 0.0;
    safe_att->userLongitudeOffset = 0.0;
    safe_att->axis = Management::MNG_TRACK;
    safe_att->angularSize = 0.0;
    safe_att->radialVelocity = 0.0;
    safe_att->vradFrame = Antenna::ANT_TOPOCEN;
    safe_att->vradDefinition = Antenna::ANT_RADIO;

    TIMEVALUE now;
    IRA::CIRATools::getTime(now);

    // CRITICAL SECTION START
    {
        baci::ThreadSyncGuard guard(&m_mutex);

        // Update the internal state (this requires the lock)
        BodyPosition(now);

        // OPTIMIZATION 3: Minimized Critical Section
        // We only perform operations that strictly require the lock here.
        // (Reading member variables like m_ra, m_dec, m_bodyName)
        
        safe_att->sourceID = CORBA::string_dup(m_bodyName); // Must be locked to read m_bodyName safely
        
        safe_att->J2000RightAscension = slaDranrm(m_ra2000);
        safe_att->J2000Declination = IRA::CIRATools::latRangeRad(m_dec2000);
        
        safe_att->rightAscension = slaDranrm(m_ra);
        safe_att->declination = IRA::CIRATools::latRangeRad(m_dec);
        safe_att->azimuth = slaDranrm(m_az);
        safe_att->elevation = IRA::CIRATools::latRangeRad(m_el);
        
        safe_att->parallacticAngle = m_parallacticAngle;
        safe_att->userAzimuthOffset = m_az_off;
        safe_att->userElevationOffset = m_el_off;
        safe_att->userRightAscensionOffset = m_ra_off;
        safe_att->userDeclinationOffset = m_dec_off;
        
        safe_att->gLongitude = slaDranrm(m_glon);
        safe_att->gLatitude = IRA::CIRATools::latRangeRad(m_glat);
        safe_att->distance = m_distance;

        // Calculate epoch inside lock to ensure m_dut1 consistency (if m_dut1 can change)
        IRA::CDateTime currentTime(now, m_dut1);
        safe_att->julianEpoch = currentTime.getJulianEpoch();
    }
    // CRITICAL SECTION END - Mutex released immediately

    // OPTIMIZATION 4: Transfer Ownership
    // Using ._retn() passes the pointer to the caller (att) and clears the smart pointer
    // preventing double deletion.
    att = safe_att._retn();
}
void SolarSystemBodyImpl::setOffsets(CORBA::Double lon,CORBA::Double lat,Antenna::TCoordinateFrame frame)  
{       
   AUTO_TRACE("SolarSystemBodyImpl::setOffsets()");
   if (frame==Antenna::ANT_HORIZONTAL) {
		m_source.setHorizontalOffsets(lon,lat);		
		m_az_off=lon;
		m_el_off=lat;
		m_ra_off=0.0;
		m_dec_off=0.0;
	}
	else if (frame==Antenna::ANT_EQUATORIAL) {
		m_az_off=0.0;
		m_el_off=0.0;
		m_ra_off=lon;
		m_dec_off=lat;
		m_source.setEquatorialOffsets(lon,lat);

	}
	else {
		_EXCPT(AntennaErrors::OffsetFrameNotSupportedExImpl,impl,"SolarSystemBodyImpl::setOffsets()");
		throw impl.getAntennaErrorsEx();
	}

}

void SolarSystemBodyImpl::getJ2000EquatorialCoordinate(ACS::Time time, CORBA::Double_out ra2000, CORBA::Double_out dec2000)  

{
	  AUTO_TRACE("SolarSystemBodyImpl::getJ2000EquatorialCoordinate()");
	  
	  double _ra,_dec;
	  
	  	  TIMEVALUE val(time);

	  baci::ThreadSyncGuard guard(&m_mutex);

	  BodyPosition(val);
	  
	  IRA::CDateTime ctime(val,m_dut1);	
	 _ra =slaDranrm(m_ra2000);
	 _dec =IRA::CIRATools::latRangeRad(m_dec2000);
     ra2000=_ra;
     dec2000=_dec;


}


void SolarSystemBodyImpl::getHorizontalCoordinate(ACS::Time time, CORBA::Double_out az, CORBA::Double_out el)  

{
        AUTO_TRACE("SolarSystemBodyImpl::getHorizontalCoordinate()");
	     double azi,ele;
        baci::ThreadSyncGuard guard(&m_mutex);

        TIMEVALUE val(time);
	     IRA::CDateTime ctime(val,m_dut1);	
      	BodyPosition(val);
	     m_source.process(ctime,m_site);
	     m_source.getApparentHorizontal(azi,ele);
	     az=azi; el=ele;



}

void SolarSystemBodyImpl::getAllCoordinates(ACS::Time time,CORBA::Double_out az,CORBA::Double_out el,CORBA::Double_out ra,CORBA::Double_out dec,CORBA::Double_out jepoch,CORBA::Double_out lon,
                CORBA::Double_out lat)  
{

    AUTO_TRACE("SolarSystemBodyImpl::getAllCoordinates()")
 
	double azi,ele,rae,dece,lone,late,jepoche;
 	baci::ThreadSyncGuard guard(&m_mutex);
   TIMEVALUE val(time);

	IRA::CDateTime ctime(val,m_dut1);
	BodyPosition(val);
	m_source.process(ctime,m_site);
	m_source.getApparentHorizontal(azi,ele);
	m_source.getApparentEquatorial(rae,dece,jepoche);
	m_source.getApparentGalactic(lone,late);
	az=azi; el=ele;
	ra=rae; dec=dece; jepoch=jepoche;
	lon=lone; lat=late;
}

 
bool SolarSystemBodyImpl::checkTracking(ACS::Time time,CORBA::Double az,CORBA::Double el,CORBA::Double HPBW) 
{
     	AUTO_TRACE("SolarSystemBodyImpl::checkTracking()")
	   double computedAz,computedEl,azErr,elErr,skyErr;
	   baci::ThreadSyncGuard guard(&m_mutex);  // obtain access

    	TIMEVALUE val(time);
	   IRA::CDateTime refTime(val,m_dut1);
   	m_source.process(refTime,m_site);
	   m_source.getApparentHorizontal(computedAz,computedEl);
	   elErr=computedEl-el;
	   azErr=(computedAz-az)*cos(el);
   	skyErr=sqrt(elErr*elErr+azErr*azErr); //total skyError
	   return skyErr<=HPBW*0.1;
     
}


void SolarSystemBodyImpl::setBodyName(const char* bodyName)  
{

        AUTO_TRACE("SolarSystemBodyImpl::setBodyName()");
        PLCode  code; //planet code from xephem astrolib


        m_bodyName=CString(bodyName);
        m_bodyName.MakeUpper();
        code=xephemlib::SolarSystemBody::getPlanetCodeFromName(bodyName);
        CUSTOM_LOG(LM_FULL_INFO,"SolarSystemBodyImpl::setBodyName()",
					  (LM_INFO,"Solar System body name:%s",(const char *)m_bodyName));             


        if (code!=NOBJ){
             try{
                  m_body_xephem ->setObject(code);
             }
         
             
             catch (std::exception& e)
             {
                       std::cout << e.what() << '\n';
             }
              catch (...)
             {
             	std::cout << "except" << std::endl;
             
             }
        
        } else
       {
           _EXCPT(AntennaErrors::SourceNotFoundExImpl, __dummy,"SolarSystemBodyImpl::setBodyName()");
           __dummy.setSourceName(bodyName);
	         CUSTOM_EXCPT_LOG(__dummy,LM_DEBUG);
	         throw __dummy.getAntennaErrorsEx();
         	 
        }
        

}

void SolarSystemBodyImpl::computeFlux(CORBA::Double freq, CORBA::Double fwhm, CORBA::Double_out flux)  
{
        flux=0.0;
        AUTO_TRACE("SolarSystemBodyImpl::computeFlux()");

}

void SolarSystemBodyImpl::getDistance(ACS::Time time,CORBA::Double_out distance)  
{
      AUTO_TRACE("SolarSystemBodyImpl::getDistance()");
      baci::ThreadSyncGuard guard(&m_mutex);
      TIMEVALUE val(time);
	   IRA::CDateTime ctime(val,m_dut1);
	   BodyPosition(val);
     	distance=m_distance;

}



void SolarSystemBodyImpl::BodyPosition(TIMEVALUE &time)
{

        AUTO_TRACE("SolarSystemBodyImpl::BodyPosition()");
        double TDB,time_utc,TT;
     //   double ra,dec,eph,az,el,range,lone,late;
 //       IRA::CSkySource m_source;   // dummy CSkySource only for coordinate conversion  
                                      // kept here as a note to remember not to name a variable with
                                      // local scope like that of a private variable of the class.
                                      
   	  IRA::CDateTime date(time,m_dut1);
	     TDB=date.getTDB(m_site);
	     constexpr double MJD_OFFSET = 2400000.5;

   	  TT=date.getTT() ;
        TDB = TDB - MJD_OFFSET;     //TDB as a Modified Julian Date (JD - 2400000.5
	     TT = TT - MJD_OFFSET;       //TT as a Modified Julian Date (JD - 2400000.5
        
        time_utc=  CDateTime::julianEpoch2JulianDay (date.getJulianEpoch()) - MJD_OFFSET;

        m_sitex -> setTime(time_utc) ;  
        m_body_xephem->compute(m_sitex.get() );


        double ra2000,dec2000,az2000,el2000,range;
        
        m_body_xephem->getCoordinates(ra2000,dec2000,az2000,el2000,range);
        m_ra2000 = ra2000;
        m_dec2000= dec2000;
        m_distance=range;
        m_source.setInputEquatorial(m_ra2000, m_dec2000, IRA::CSkySource::SS_J2000);
        m_source.process(date,m_site);	       
        
         
        double az,el,ra,dec,eph,lone,late;
        
        m_source.getApparentEquatorial (ra,dec,eph);
        m_source.getApparentHorizontal(az,el);
	     m_source.getApparentGalactic(lone,late);            
//        m_source.apparentToHorizontal(date,m_site);	
        m_ra=ra;
        m_dec=dec;
        m_az=az;
        m_el=el;
        m_glon=lone;
        m_glat=late;
        m_parallacticAngle=m_source.getParallacticAngle();
}

#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SolarSystemBodyImpl)



