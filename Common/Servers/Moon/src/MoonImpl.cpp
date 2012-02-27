// $Id: MoonImpl.cpp,v 1.13 2011-06-21 16:38:57 a.orlati Exp $

#include "MoonImpl.h"
#include <Definitions.h>
#include <ObservatoryS.h>
#include <slamac.h>
#include <slalib.h>
#include <baci.h>
#include <math.h>
#include <acstimeEpochHelper.h>
#include <Site.h>
#include <DateTime.h>
#include <SkySource.h>


using namespace ComponentErrors;
using namespace baci;
using namespace IRA;


static char *rcsId="@(#) $Id: MoonImpl.cpp,v 1.13 2011-06-21 16:38:57 a.orlati Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


MoonImpl::MoonImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) :
	acscomponent::ACSComponentImpl(CompName, containerServices)
{
	AUTO_TRACE("MoonImpl::MoonImpl()");
	m_componentName=CString(CompName);
	m_sourceName="Moon";
}

MoonImpl::~MoonImpl()
{
	AUTO_TRACE("MoonImpl::~MoonImpl()");
}


// initialization 
void MoonImpl::initialize() throw(ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("moonImpl::initilize()");
	ra_off = dec_off = 0.0;
	az_off = el_off = 0.0;
	m_offsetFrame=Antenna::ANT_HORIZONTAL;
	ACS_LOG(LM_FULL_INFO, "MoonImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZING"));
}

void MoonImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	CError error;
	Antenna::TSiteInformation_var site;
	AUTO_TRACE("MoonImpl::execute()");
	
	Antenna::Observatory_var observatory=Antenna::Observatory::_nil();
	try {
		observatory=getContainerServices()->getComponent<Antenna::Observatory>("ANTENNA/Observatory");
	}
	catch (maciErrType::CannotGetComponentExImpl & ex){
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"MoonImpl::execute()");
		Impl.setComponentName("ANTENNA/Observatory");
		throw Impl;
	}
	
	ACS_LOG(LM_FULL_INFO,"MoonImpl::execute()", (LM_INFO, (const char *)CString(m_componentName+"::OBSERVATORY_LOCATED")));
	try {
		site=observatory->getSiteSummary();
	}
	catch (CORBA::SystemException& ex)		{
		_EXCPT(ComponentErrors::CORBAProblemExImpl,_dummy, "MoonImpl::execute()");
		_dummy.setName(ex._name());
		_dummy.setMinor(ex.minor());
		throw _dummy;
	}
    m_site=CSite(site.out());
	m_dut1=site->DUT1;
	m_longitude=site->longitude;
	m_latitude=site->latitude;
	m_height=site->height;
	try {
		getContainerServices()->releaseComponent((const char*)observatory->name());
	}
	catch  (maciErrType::CannotReleaseComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"OTFImpl::initialize()");
		Impl.setComponentName("ANTENNA/Observatory");
		throw Impl;
	} 
	ACS_LOG(LM_FULL_INFO,"MoonImpl::execute()", (LM_INFO,"SITE_INITIALIZED"));
}

void MoonImpl::cleanUp()
{
	AUTO_TRACE("MoonImpl::cleanUp()");
	ACSComponentImpl::cleanUp();
}

void MoonImpl::aboutToAbort()
{
	AUTO_TRACE("MoonImpl::aboutToAbort()");
}


void MoonImpl::computeFlux(CORBA::Double freq, CORBA::Double fwhm, CORBA::Double_out flux) throw (CORBA::SystemException)
{
	flux=0.0;
}

void MoonImpl::getAttributes(Antenna::MoonAttributes_out att) throw(CORBA::SystemException)
{
	
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	Moon_position(now);
				
	/* Returns the julian epoch of the date.
	 * @return the epoch which is the year with the fracional part of the year.
	 */
	double JulianEpoch; 
	IRA::CDateTime currentTime(now,m_dut1);
	JulianEpoch = currentTime.getJulianEpoch();
		
	/* Getting the output field 
	 */
	AUTO_TRACE("MoonImpl::getAttributes()");
	att=new Antenna::MoonAttributes;
	
	att->sourceID=CORBA::string_dup("MOON");
	att->J2000RightAscension=slaDranrm(m_ra2000);
	att->J2000Declination=IRA::CIRATools::latRangeRad(m_dec2000);
	att-> rightAscension = slaDranrm(m_ra);
	att-> declination = IRA::CIRATools::latRangeRad(m_dec) ;
	att-> azimuth = slaDranrm(m_az);
	att-> elevation = IRA::CIRATools::latRangeRad(m_el);
	att-> julianEpoch = JulianEpoch;
	att-> parallacticAngle = m_parallacticAngle;
	att-> userAzimuthOffset = az_off;
	att-> userElevationOffset = el_off;
	att-> userRightAscensionOffset = ra_off;
	att-> userDeclinationOffset = dec_off;
	att->gLongitude=slaDranrm(m_glon);
	att->gLatitude=IRA::CIRATools::latRangeRad(m_glat);
	att->userLatitudeOffset=att->userLongitudeOffset=0.0;
}

void MoonImpl::setOffsets(CORBA::Double lon,CORBA::Double lat,Antenna::TCoordinateFrame frame) throw (CORBA::SystemException,AntennaErrors::AntennaErrorsEx)
{	
	if (frame==Antenna::ANT_HORIZONTAL) {
		az_off=lon;
		el_off=lat;
		ra_off=0.0;
		dec_off=0.0;
	}
	else if (frame==Antenna::ANT_EQUATORIAL) {
		az_off=0.0;
		el_off=0.0;
		ra_off=lon;
		dec_off=lat;
	}
	else {
		_EXCPT(AntennaErrors::OffsetFrameNotSupportedExImpl,impl,"MoonImpl::setOffsets()");
		throw impl.getAntennaErrorsEx();
	}
}

void MoonImpl::getHorizontalCoordinate(ACS::Time time, CORBA::Double_out az, CORBA::Double_out el) throw (CORBA::SystemException)
{
	AUTO_TRACE("MoonImpl::getHorizontalCoordinate()");
	TIMEVALUE val(time);
	Moon_position(val);
	az=slaDranrm(m_az);
	el=IRA::CIRATools::latRangeRad(m_el);
}

void MoonImpl::getAllCoordinates(ACS::Time time,CORBA::Double_out az,CORBA::Double_out el,CORBA::Double_out ra,CORBA::Double_out dec,CORBA::Double_out jepoch,CORBA::Double_out lon,
		CORBA::Double_out lat) throw (CORBA::SystemException)
{
	TIMEVALUE val(time);
	double JulianEpoch;
	Moon_position(val);
	IRA::CDateTime currentTime(val,m_dut1);
	JulianEpoch = currentTime.getJulianEpoch();
	az=slaDranrm(m_az); el=IRA::CIRATools::latRangeRad(m_el);
	ra=slaDranrm(m_ra); dec=IRA::CIRATools::latRangeRad(m_dec); jepoch=JulianEpoch;
	lon=slaDranrm(m_glon); lat=IRA::CIRATools::latRangeRad(m_glat);
}

 bool MoonImpl::checkTracking(ACS::Time time,CORBA::Double az,CORBA::Double el,CORBA::Double HPBW) throw (CORBA::SystemException)
{
 AUTO_TRACE("MoonImpl::CheckTracking()");
 	TIMEVALUE val(time);
 	Moon_position(val);
 	
 	double az_Err,el_Err, delta;
 	bool track;
 	az_Err = (az - slaDranrm(m_az))*cos(IRA::CIRATools::latRangeRad(m_el));
 	el_Err = el - IRA::CIRATools::latRangeRad(m_el);
 	delta = sqrt( az_Err*az_Err + el_Err*el_Err );
 	if (delta <=  HPBW*0.1) {
 		track=true;
 	}
 	else {	
 		track=false;
 	}
 	return (track);	
}
 
/**
         * Gets the Barycentric Dynamical Time. The TDB is the Terrestrial Time compensated by periodic terms due to relativistic effects.
         * The ammount of correction cycles back and forth by 2 milliseconds at maximum.
         * TDB may be considered to be the coordinate time in solar system barycentre frame of reference.
         * For most precession/nutation and Earth position and velocity routines this is the most adeguate input time.
         * @param site contais all the information regarding the observer position on the Earth
         * @return the TDB for the object as julian day.
*/


/*************************************************************************************/

 void MoonImpl::Moon_position(TIMEVALUE &time)
{
	double TDB;
	IRA::CDateTime date(time,m_dut1);
	TDB=date.getTDB(m_site);
	
	/*
	 * TDB as a Modified Julian Date (JD - 2400000.5
	 * )*/
	TDB = TDB - 2400000.5;
		

/* Calculating approximately geocentric position and velocity 
 * of the Moon (double precision)
 */
		
	double pv[6];                   /* Moon [x,y,z,vx,vy,vz], mean equator and equinox of date( AU, AU/sec) */
	slaDmoon (TDB, pv);            /*  geocentric position/velocity of the moon (mean of date) where 
	                                   TDB as a modified julian Date(MJD) (JD - 2400000.5) */

		
/* Form the matrix of nutation for a given date (true eqinox)  */
	
	double rmatn[3][3];            /* Its a 3x3 nutation matrix */
	slaNut (TDB, rmatn);           /* TDB as a MJD */
		

/*  Multiply the position and velocity vector by a rotation matrix ( double presicion )  */ 
		
	double pv_rot[6];                 /* Moon's position and velocity after applying the rotation matrix */
	
	slaDmxv (rmatn, pv, pv_rot );     /* Since rotation matrix is 3X3 therefore applying
	                                         rotation to first to the position*/

	slaDmxv (rmatn, pv+3, pv_rot+3);  /* further applying the rotation matrix to the 
		                                      velocity of moon */

/*   Getting Attribute LST from Class CDateTime 
 	 * It computes the Local Apparent Sidereal Time. This time is calculated as <i>GST()</i> by adding the 
 	   east longituide of the site.
	 * @param site contais all the information regarding the observer position on the Earth 
	 * @return a Date and Time object with the current LST.
	 */  		
			
	double LST;                              /* local apperant sidereal time */
	LST= date.LST(m_site).getDayRadians();   /*  getDayRadians,translating current LST from Date & Time into radians*/
	
		
/*   Geocentric position of the observer (true equator and equinox of the date ) 
     Observatory position & velocity  */		
	 
	double pvo[6];                        /* Position and velocity of the observatory in AU/sec, true 
	                                          equator and equinox of date   */
	slaPvobs (m_latitude, m_height, LST, pvo );	
	
	
/*  Now placing the origin at the observer. get the location of the moon w r to observer */
	int i ; 
	for (i = 0; i < 6; i++) {
		pv_rot[i] -= pvo[i]; 
	}
	
	
	/*  Topocentric RA and Dec  
	 *  Converting the cartesian coordinates in to spherical coordinates in radians (double precision) 	
	 *	Spherical coordinates are longitude m_ra (+ anticlockwise looking form the + latitude pole)
	    and the latitude m_dec
	 */
	slaDcc2s ( pv_rot, &m_ra, &m_dec ); 
	/* now compute the corresponding galactic coordinates, J2000, e paralacticAngle...*/
	IRA::CSkySource::apparentToJ2000(m_ra,m_dec,date,m_ra2000,m_dec2000);
	
	// add the offsets.......
	
	m_dec += dec_off;
	m_ra += ra_off/cos(m_dec);

	// now computes the corresponding galactic coordinate...offsets are included
	IRA::CSkySource::equatorialToGalactic(m_ra2000,m_dec2000,m_glon,m_glat);
	
/* Converting an angle (m_ra) in radians to hours, minutes, seconds, fraction ( double precision ) */
		
/*	char s;                              sign '+' or '-' 
	int ihmsf[4];                       angle in terms of 'hours, minutes,seconds fraction (double precision) 
	slaDr2tf (2, m_ra, &s, ihmsf);*/

	
/*  Calculating the HA and Dec  */
    //slaDranrm ( LST - m_ra );
		
	/* 2 is the  no of decimal places of seconds
	 * slaDranrm (stl-ra), this call normalize an angle (stl - ra) into the range  0-360 (double precision) 
	 *  HA = (LST - m_ra)
	 * 's' is the sign 
	 *  'ihmsf' is the  angle in terms of hours, minutes, seconds and   fraction with double precision  
	 */

	
/*  Converting an angle in radians to degree,arcminute, arcseconds and fraction 
			
	int idmsf[4];                       angle in degrees, arcminutes, arcseconds, fraction 
	slaDr2af ( 2, m_dec, &s, idmsf); */
	
	
/* Equatorial to Horizontal coordinates (double precision) */
		
	slaDe2h (slaDranrm ( LST - m_ra ), m_dec, m_latitude, &m_az, &m_el); /* all the parameters in this argument are in radians */
	m_el += el_off;
	m_az += az_off/cos(m_el);
		
	m_parallacticAngle=IRA::CSkySource::paralacticAngle(date,m_site,m_az,m_el);
}


/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(MoonImpl)


/*************************************************************************************/
