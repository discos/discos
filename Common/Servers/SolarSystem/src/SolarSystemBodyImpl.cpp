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
// using namespace baci;
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
        delete m_sitex;
        delete m_body_xephem;
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
	   //  std::cout << "Site:" << site->longitude *R2D << " " << m_latitude ;
	     m_sitex= new xephemlib::Site();
	     m_sitex-> setCoordinate(site->longitude,site->latitude,site->height); //coordinates in degrees.
        m_body_xephem =   new xephemlib::SolarSystemBody();
        		  
		   
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


	     baci::ThreadSyncGuard guard(&m_mutex);
TIMEVALUE now;

	IRA::CIRATools::getTime(now);

	BodyPosition(now);
				
	/* Returns the julian epoch of the date.
	 * @return the epoch which is the year with the fracional part of the year.
	 */
	double JulianEpoch; 
	
	BodyPosition(now);
	IRA::CDateTime currentTime(now,m_dut1);
	JulianEpoch = currentTime.getJulianEpoch();
		
	/* Getting the output field 
	 */
 
	att=new Antenna::SolarSystemBodyAttributes;
	
	att->sourceID=CORBA::string_dup(m_bodyName);
	att->J2000RightAscension=slaDranrm(m_ra2000);
	att->J2000Declination=IRA::CIRATools::latRangeRad(m_dec2000);
	att-> rightAscension = slaDranrm(m_ra);
	att-> declination = IRA::CIRATools::latRangeRad(m_dec) ;
	att-> azimuth = slaDranrm(m_az);
	att-> elevation = IRA::CIRATools::latRangeRad(m_el);
	att-> julianEpoch = JulianEpoch;
	att-> parallacticAngle = m_parallacticAngle;
	att-> userAzimuthOffset = m_az_off;
	att-> userElevationOffset = m_el_off;
	att-> userRightAscensionOffset = m_ra_off;
	att-> userDeclinationOffset = m_dec_off;
	att->gLongitude=slaDranrm(m_glon);
	att->gLatitude=IRA::CIRATools::latRangeRad(m_glat);
	att->userLatitudeOffset=att->userLongitudeOffset=0.0;
	att->axis=Management::MNG_TRACK;
	att->distance=m_distance;
    att->angularSize= att->radialVelocity=0;
    att->vradFrame= Antenna::ANT_TOPOCEN  ;
    att->vradDefinition=Antenna::ANT_RADIO;

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
        TIMEVALUE val(time);
	     IRA::CDateTime ctime(val,m_dut1);	
	     baci::ThreadSyncGuard guard(&m_mutex);
      	BodyPosition(val);
	     m_source.process(ctime,m_site);
	     m_source.getApparentHorizontal(azi,ele);
	     az=azi; el=ele;



}

void SolarSystemBodyImpl::getAllCoordinates(ACS::Time time,CORBA::Double_out az,CORBA::Double_out el,CORBA::Double_out ra,CORBA::Double_out dec,CORBA::Double_out jepoch,CORBA::Double_out lon,
                CORBA::Double_out lat)  
{

    AUTO_TRACE("SolarSystemBodyImpl::getAllCoordinates()")
 
    TIMEVALUE val(time);
	double azi,ele,rae,dece,lone,late,jepoche;
	IRA::CDateTime ctime(val,m_dut1);
//	baci::ThreadSyncGuard guard(&m_mutex);
	BodyPosition(val);
	m_source.process(ctime,m_site);
	m_source.getApparentHorizontal(azi,ele);
	m_source.getApparentEquatorial(rae,dece,jepoche);
	m_source.getApparentGalactic(lone,late);
//	std::cout << "Azi,ele"<< azi<<","<<ele<<std::endl;
//	std::cout << "Time"<< time <<std::endl;
	az=azi; el=ele;
	ra=rae; dec=dece; jepoch=jepoche;
	lon=lone; lat=late;
}

 
bool SolarSystemBodyImpl::checkTracking(ACS::Time time,CORBA::Double az,CORBA::Double el,CORBA::Double HPBW) 
{
     	AUTO_TRACE("SolarSystemBodyImpl::checkTracking()")
	   double computedAz,computedEl,azErr,elErr,skyErr;
    	TIMEVALUE val(time);
	   IRA::CDateTime refTime(val,m_dut1);
	   baci::ThreadSyncGuard guard(&m_mutex);  // obtain access
   	m_source.process(refTime,m_site);
	   m_source.getApparentHorizontal(computedAz,computedEl);
	   elErr=computedEl-el;
	   azErr=(computedAz-az)*cos(el);
   	skyErr=sqrt(elErr*elErr+azErr*azErr); //total skyError
	   return skyErr<=HPBW*0.1;
     
}


void SolarSystemBodyImpl::setBodyName(const char* bodyName)  
{

/*
typedef enum {
    MERCURY,
    VENUS,
    MARS,
    JUPITER,
    SATURN,
    URANUS,
    NEPTUNE,
    PLUTO,
    SUN,
    MOON,
    NOBJ	// total number of basic objects 
} PLCode;
*/


        AUTO_TRACE("SolarSystemBodyImpl::setBodyName()");
        m_bodyName=CString(bodyName);
        m_bodyName.MakeUpper();
//        std::map<CString,int> plan;

        PLCode  code;
        
        code=xephemlib::SolarSystemBody::getPlanetCodeFromName(bodyName);
        CUSTOM_LOG(LM_FULL_INFO,"SolarSystemBodyImpl::setBodyName()",
					  (LM_INFO,"Solar System body name:%s",(const char *)m_bodyName));             


        if (code!=NOBJ){
//             m_body_xephem =   new xephemlib::SolarSystemBody(code);
             try{
             m_body_xephem ->setObject(code);
             }
         
             
             catch (std::exception& e)
             {
                       std::cout << e.what() << '\n';
             }
              catch (...)
             {std::cout << "except" << std::endl;}
             
             
             
            // std::cout << "Body name:" << bodyName <<std::endl;
           //  std::cout << "code:" << code <<std::endl;
        
        } else
       {
          // THROW_EX(AntennaErrors, SourceNotFoundExImpl, "SolarSystemBodyImpl::setBodyName", false);
      
           _EXCPT(AntennaErrors::SourceNotFoundExImpl, __dummy,"SkySourceImpl::loadSourceFromCatalog()");
           __dummy.setSourceName(bodyName);
	         CUSTOM_EXCPT_LOG(__dummy,LM_DEBUG);
        
	         	throw __dummy.getAntennaErrorsEx();
        
      		
         	 
        }
        

//_EXCPT(AntennaErrors::SourceNotFoundExImpl, __dummy,"SkySourceImpl::loadSourceFromCatalog()");
      		//__dummy.setSourceName(bodyName);
	//	       CUSTOM_EXCPT_LOG(__dummy,LM_DEBUG);
        
	  //     	throw __dummy.getAntennaErrorsEx();
        

}

void SolarSystemBodyImpl::computeFlux(CORBA::Double freq, CORBA::Double fwhm, CORBA::Double_out flux)  
{
        flux=0.0;
        AUTO_TRACE("SolarSystemBodyImpl::computeFlux()");

}

void SolarSystemBodyImpl::getDistance(ACS::Time time,CORBA::Double_out distance)  
{
    AUTO_TRACE("SolarSystemBodyImpl::getDistance()");
	     double azi,ele;
       TIMEVALUE val(time);
	     IRA::CDateTime ctime(val,m_dut1);
	     baci::ThreadSyncGuard guard(&m_mutex);
     	BodyPosition(val);
     	distance=m_distance;

}



void SolarSystemBodyImpl::BodyPosition(TIMEVALUE &time)
{

        AUTO_TRACE("SolarSystemBodyImpl::BodyPosition()");
        double TDB,time_utc,TT;
#ifdef DEBUG
//        std::cout <<time.year() << " " << time.month() << " " <<time.day() ;
//        std::cout <<time.hour() << " " << time.minute() << " " <<time.second()  << std::endl  ;
#endif

        baci::ThreadSyncGuard guard(&m_mutex);	
       
        
	    IRA::CDateTime date(time,m_dut1);
	    TDB=date.getTDB(m_site);
	    TT=date.getTT() ;
       
	
	/*
	 * TDB as a Modified Julian Date (JD - 2400000.5
	 * )*/
	   TDB = TDB - 2400000.5;     
	   TT = TT - 2400000.5;     
        
       time_utc=  CDateTime::julianEpoch2JulianDay (date.getJulianEpoch());
       	   time_utc = time_utc - 2400000.5;     
       //std::cout << time_utc-(int)time_utc << "    " <<TDB << std::endl;
       

       
       double ra,dec,eph,az,el,range,lone,late;

//   Site *site = new Site(59319.5,degrad(9.5),degrad(39.5),600);

        m_sitex -> setTime(time_utc) ;  
        m_body_xephem->compute( m_sitex );
//		  m_sitex ->stampa();        
//        m_body_xephem->report();
        m_body_xephem->getCoordinates(ra,dec,az,el,range);
        m_ra2000 = ra;
        m_dec2000= dec;
        m_distance=range;
        m_source.setInputEquatorial(m_ra2000, m_dec2000, IRA::CSkySource::SS_J2000);
                // IRA::CSkySource m_source;   // dummy CSkySource onj for coordinate conversion  
        m_source.process(date,m_site);	       
        m_source.getApparentEquatorial (ra,dec,eph);
        m_source.getApparentHorizontal(az,el);
	     m_source.getApparentGalactic(lone,late);            
        m_source.apparentToHorizontal(date,m_site);	
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



