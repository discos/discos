/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                 when           What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  22/02/2007     Creation                                          */
/* Andrea Orlati(aorlati@ira.inaf.it)  15/10/2007     Added the support for galactic coordinate system  */
/* Andrea Orlati(aorlati@ira.inaf.it)  24/10/2007	  Added the support for fixed azimuth,elevation points */
/* Andrea Orlati(aorlati@ira.inaf.it)  15/02/2008	  Added the function equatorialToGalactic */
/* Andrea Orlati(aorlati@ira.inaf.it)  28/03/2008	  Added the possibility to specify also galactic offsets */
/* Andrea Orlati(aorlati@ira.inaf.it)  27/01/2009	  added the static function paralacticAngle */
/* Andrea Orlati(aorlati@ira.inaf.it)  22/07/2010	  fixed a bug in correction for cosine of elevation in the offsets computation*/ 
/* Andrea Orlati(aorlati@ira.inaf.it)  06/05/2011	  fixed an issue, when galactic offsets were commanded the J2000 coordinates did not correspond to the source any more*/
/* Andrea Orlati(aorlati@ira.inaf.it)  08/04/2013	 added is BeamPark() method*/

#ifndef SKYSOURCE_H_
#define SKYSOURCE_H_

#include "DateTime.h"
#include "Site.h"

namespace IRA {
	
/**
 * This class performs the coordinate manipulation required by a altazimuthal mount in order to point a source. A tracking
 * trajectory can also be obtained computing the coordinates for future times. The source can be identified only via equatorial
 * coordinates, anyway functions to convert from other cordinates system are included. 
 * Two mean places (here referred as input coordinates) are supported (No support for 'any equinox' is supplied) :
 * @arg FK4 equinox at besialian epoch B1950
 * @arg FK5 equinox at julian epoch J2000
 * After creation the <i>J2000ToApparent()</i> method can be used to computes the apparant equatorial coordinates, then the
 * <i>J2000ToHorizontal()</i> method computes the Azimuth and Elevation (horizontal) that can be used to point the telescope. 
 * Horizontal coordinates are not corrected for instrumental effects or refraction. All this factor should be accounted in order
 * to obtain a real observed place. 
 * The conputation is based on the Position Astronomy Library (SLALIB) by Patrick Wallace.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/		
class CSkySource {
public:
	/**
	 *  the equinox which identifies the mean place 
	*/ 
	enum TEquinox {
		SS_B1950=0,      /*!< equinox at besselian epoch 1950 (FK4 catalog) */
		SS_J2000=1,      /*!< equinox at julian epoch J2000 (FK5 catalog) */
		SS_APPARENT=2    /*!< coordinates are already apparent, they do not need precession */
	};

	/**
	 * list of supported frames
	*/
	enum TFrame {
		SS_EQ=0,    /*!< equatorial frame*/
		SS_HOR=1, /*!< horizontal frame */
		SS_GAL=2,  /*!<  galactic frame*/
		SS_NONE=3  /*!< none  */
	};
	
	/**
	 * Constructor.
	*/
	CSkySource();
	
	/** 
	 * Constructor. It requires the initial coordinates
	 * @param ra right ascension in radians
	 * @param dec declination in radians
	 * @param eq equinox which the above coordinates refer to
	 * @param dra proper motion in right ascension (milli arcseconds per tropical year if FK4 is used, per 
	 *        julian year if Fk5 is used). dRA/dt is required not dRA/dt*cos(DEC).
	 * @param ddec proper motion in declination (milli arcseconds per tropical year if FK4 is used, per julian year if Fk5 is used)
	 * @param parallax parallax in milli arcseconds.
	 * @param rvel radial velocity in Km/sec ( positive value means the source is moving away)
	*/
	CSkySource(const double& ra,const double& dec,const TEquinox& eq,
	  const double& dra=0.0,const double& ddec=0.0,const double& parallax=0.0,const double& rvel=0.0);
	  
	 /**
	  * Destructor.
	 */ 
	~CSkySource();
	
 	/** 
 	 * Initilizes the sky point by setting the catalog information. It requires the initial coordinates.
	 * @param ra right ascension in radians
	 * @param dec declination in radians
	 * @param eq equinox which the above coordinates refer to
	 * @param dra proper motion in right ascension (milli arcseconds per tropical year if FK4 is used, per 
	 *        julian year if Fk5 is used). dRA/dt is required not dRA/dt*cos(DEC).
	 * @param ddec proper motion in declination (milli arcseconds per tropical year if FK4 is used, per julian year if Fk5 is used)
	 * @param parallax parallax in milli arcseconds.
	 * @param rvel radial velocity in Km/sec ( positive value means the source is moving away)
	*/
	void setInputEquatorial(const double& ra,const double& dec,const TEquinox& eq,
	  const double& dra=0.0,const double& ddec=0.0,const double& parallax=0.0,const double& rvel=0.0);
	
	/**
	 * Initializes the sky point by setting the catalog information for the galactic frame. Practically it converts
	 * the galactic coordinate into J2000 equatorial one. 
	 * @param latitude the galactic coordinate in radians.
	 * @param longitude the galactic coordinate in radians. 
	*/
	void setInputGalactic(const double& longitude,const double& latitude);
	
	/**
	 * Initializes the sky point by giving its horizontal coordinates. That means that the horizontal coordinates are fixed
	 * and the input coordinates are computed for the present time; the equinox of equatorial frame will 
	 * be <i>SS_APPARENT</i>. The apparent equatorial coordinates are then computed by the <i>process()</i> method. 
	 * If the point is set in such a way the equtorial offsets are not taken into account.
	 * @param az the azimuth of the fixed point
	 * @param el the elevation of the fixed point
	 * @param site the site of the observer 
	*/
	void setInputHorizontal(const double& az,const double& el,const CSite& site);
	
 	/** 
 	 * Read the input information in the equatorial frame.
	 * @param ra right ascension in radians
	 * @param dec declination in radians
	 * @param eph epoch which the coordinates refer to 
	 * @param dra proper motion in right ascension (milli arcseconds per tropical year if FK4 is used, per julian year if Fk5 is used)
	 * @param ddec proper motion in declination (milli arcseconds per tropical year if FK4 is used, per julian year if Fk5 is used)
	 * @param parallax parallax in milli arcseconds.
	 * @param rvel radial velocity in Km/sec ( positive value means the source is moving away)
	*/	  
	void getInputEquatorial(double& ra,double& dec,double& eph,double& dra,double& ddec,double& parallax,double& rvel);
	
	/**
	 * Read input information using the galatctic frame.
	 * @param longitude returned longitude coordinate in radians
	 * @param latitude returned latitude coordinate in radians
	*/
	void getInputGalactic(double& longitude,double& latitude);
	
 	/** 
 	 * Read the catalog information transformed to FK5 J2000 system
	 * @param ra right ascension in radians
	 * @param dec declination in radians
	 * @param eph epoch which the coordinates refer to, always J2000
	 * @param dra proper motion in right ascension (milli seconds per tropical year if FK4 is used, per julian year if Fk5 is used)
	 * @param ddec proper motion in declination (milli arcseconds per tropical year if FK4 is used, per julian year if Fk5 is used)
	 * @param parallax parallax in arcseconds.
	 * @param rvel radial velocity in Km/sec ( positive value means the source is moving away)
	*/	  	
	void getJ2000Equatorial(double& ra,double& dec,double& eph,double& dra,double& ddec,double& parallax,double& rvel);
		
	/**
	 * Read the apparent coordinates of the sky source 
	 * @param ra apparent right ascension
	 * @param dec apparent declination 
	 * @param eph julian epoch the returned coordinates refer to 
	 */
	void getApparentEquatorial(double& ra,double& dec,double& eph);

	/**
	 * Read the galactic coordinates of the sky source as converted directly from apparent equatorial
	 * @param lon galactic longitude
	 * @param lat galactic latitude
	 */
	void getApparentGalactic(double &lon,double& lat);

	/**
	 * Read the horizontal coordinates of the sky source as they come out from the <i>apparentToHorizontal()</i>. 
	 * @param az azimuth in radians
	 * @param el elevation in radians  
	*/
	void getApparentHorizontal(double& az,double& el);
	
	/**
	 * @return the parallactic angle (radians)
	*/ 
	double getParallacticAngle() { return m_parallacticAngle; }
	
	/**
	 * It performs all the computation required to update apparent equatorial and apparent horizontal coordinates for the given time
	 * and for the given observer. In practice, it calls <i>J2000ToApparent()</i> and <i>apparentToHorizontal()</i> in sequence.
	 * @param now a CDateTime representing the current UT date and time
	 * @param site coordinates of the observer* 
	*/
	void process(const CDateTime& now,const CSite& site);
	
	/**
	 * check if the trajectory is resulting in a fixed horizontal position (no antenna movement required).
	 * @return true if the telescope will not be moved
	 */
	bool  isBeamPark() const { return m_fixed; }

	/**
	 * Precess the current FK5 J2000 equatorial coordiantes to geocentric apparent one.
	 * This routine takes into account the light deflection, annual aberration, precession and nutation.
	 * @param now a CDateTime representing the current UT date and time
	 * @param site coordinates of the observer
	 */
	void J2000ToApparent(const CDateTime& now,const CSite& site);
	
	/** 
	 * Converts the geocentric apparent equatorial sky point to horizontal, azimuth and elevation, coordinates.
	 * This routine takes into account the Earth rotation to obtain apparent HA/DEC coordinates, then the diurnal aberration to
	 * get the HA/DEC, finally the conversion HA/DEC to AZ/El is performed. The azimuth coordinate is also corrected
	 * for the effect of polar motion. It also computes the parallactic angle.
	 * @param now a CDateTime representing the current UT date and time
	 * @param site coordinates of the observer
	*/
	void apparentToHorizontal(const CDateTime& now,const CSite& site);
	
	/**
	 * it sets the offests for the galactic frame. The longitude offset is corrected for the cosine of the latitude before applying it.
	 * The Offsets are added to the input galactic coordinate and then the J2000 equatorial are computed.
	 * @param longOff new longitude offset in radians 
	 * @param latOff new latitude offset in radians
	*/
	void setGalacticOffsets(const double& longOff,const double& latOff);
	
	/**
	 * It gets the current offset for the galacticl system
	 * @param longOff the returned longitude offset in radians.
	 * @param latOff the returned latitude offset in radians.
	*/	
	void getGalacticOffsets(double& longOff,double& latOff) { longOff=m_longOff; latOff=m_latOff; }
	
	/**
	 * It sets the offsets for the horizonatal coordinates. The azimuth offset is multiplied for the cosine of the elevation before
	 * applying it
	 * @param azOff new azimuth offset in radians
	 * @param elOff new elevation offset in radians 
	*/
	void setHorizontalOffsets(const double& azOff,const double& elOff);
	
	/**
	 * I gets the current offset for the horizontal system
	 * @param azOff the returned azimuth offset in radians.
	 * @param elOff the returned elevation offset in radians.
	*/
	void getHorizontalOffsets(double& azOff,double& elOff) const { azOff=m_azOff; elOff=m_elOff; }

	/**
	 * It sets the offsets for the equatorial coordinates. The terms are applied to apparent coordinates.
	 * The RA term will be  multiplied for the cosine of the declination before applying it.
	 * @param raOff new right ascension offset in radians
	 * @param decOff new declination offset in radians 
	*/
	void setEquatorialOffsets(const double& raOff,const double& decOff);
	
	/**
	 * It gets the current offset for the equatorial  system.
	 * @param raOff the returned right ascension offset in radians.
	 * @param decOff the returned declination offset in radians.
	*/
	void getEquatorialOffsets(double& raOff,double& decOff) const { raOff=m_raOff; decOff=m_decOff; }
	
	/**
	 * This function converts apparent equatorial coordinates at a given time and site to mean J2000 equatorial ones.
	 * The Terrestrial Time in place of the Barycentric Dynamical Time at a cost of a negligible loss of precision.
	 * @param appRA apparent right ascension (radians)
	 * @param appDec apparent declination (radians)
	 * @param time time the apparent coordinates refers to
	 * @param ra returned mean J2000 right ascension
	 * @param dec returned mean j2000 declination
	*/
	static void apparentToJ2000(const double& appRA,const double& appDec,const CDateTime& time,
			double& ra,double& dec);
	
	/**
	 * Converts horizontal coordinates for the given site and time to equatorial ones. No back-correction are perfomed like
	 * diurnal aberration.
	 * @param now a CDateTime representing the current UT date and time
	 * @param site coordinates of the observer
	 * @param az azimuth coordinate in radians
	 * @param el elevation coordinate in radians
	 * @param ra converted Right Ascension in radians
	 * @param dec converted Declination in radians
	 * @param pAngle corresponding parallactic angle
	*/
	static void horizontalToEquatorial(const CDateTime& now,const CSite& site,const double& az,const double& el,
	   double& ra,double& dec,double& pAngle);

	/**
	 * Computes the paralactic angle from the horizontal coordinates.
	 * @param now a CDateTime representing the current UT date and time
	 * @param site coordinates of the observer
	 * @param az azimuth coordinate in radians
	 * @param el elevation coordinate in radians
	 * @return the corresponding paralactic angle
	 */
	static double paralacticAngle(const CDateTime& now,const CSite& site,const double &az,const double& el);
	
	/**
	 * Converts ecliptic coordinates for the given epoch to equatorial ones. 
	 * @param now a CDateTime representing the current UT date and time
	 * @param site coordinates of the observer
	 * @param lng ecliptic longitude in radians
	 * @param lat ecliptic latitude in radians
	 * @param ra converted right ascension to mean J2000 (radians)
	 * @param dec converted declination to mean J2000 (radians)
	*/
	static void eclipticToEquatorial(const CDateTime& now,const CSite& site,const double& lng,const double& lat,double& ra,double& dec);

	/**
	 * Converts equatorial coordinates for the given epoch to ecliptic ones. 
	 * @param now a CDateTime representing the current UT date and time
	 * @param site coordinates of the observer
 	 * @param ra right ascension to mean J2000 (radians)
	 * @param dec declination to mean J2000 (radians) 
	 * @param lng converted ecliptic longitude in radians
	 * @param lat converted ecliptic latitude in radians
	*/	
	static void equatorialToEcliptic(const CDateTime& now,const CSite& site,const double& ra,const double& dec,double& lng,double& lat);
	
	/**
	 * Converts galactic coordinates to J2000 FK5 equatorial coordinates.
	 * @param lng galactic longitude (radians)
	 * @param lat galactic latitude (radians)
	 * @param ra converted right ascension (radians)
	 * @param dec converted declination (radians)
	*/
	static void galacticToEquatorial(const double& lng,const double& lat,double& ra,double& dec);
	
	/**
	 * Converts galactic coordinates form J2000 FK5 equatorial coordinates. 
	 * @param ra input right ascension coordinate (radians)
	 * @param dec input declination coordinate (radians)
	 * @param lng the converted galactic longitude (radians)
	 * @param lat the converted galactic latitutde (radians)  
	*/
	static void equatorialToGalactic(const double& ra,const double& dec,double& lng,double& lat);
	
private:
	/**
	 * Input equatorial coordinates
	 * ra,dec are in radians
	 * dra,ddec are in radians per year
	 * parallax is in arcseconds
	 * rvel is in km per second
	*/
	double m_ra0,m_dec0,m_dra0,m_ddec0,m_rvel0,m_parallax0,m_epoch0;
	/** Input mean place */
	TEquinox m_equinox0;
	/**
	 * Input galactic coordinates in radians
	 */
	double m_galLong0,m_galLat0;
	/**
	 * Input horizontal coordinates in radians 
	*/
	double m_az0,m_el0;
	/**  Input coordinates transformed to FK5, J2000 */
	double m_ra1,m_dec1,m_dra1,m_ddec1,m_rvel1,m_parallax1,m_julianEpoch1;
	/** FK5 mean place, always J2000 */
	TEquinox m_equinox1;
	/** J2000 coordinates of the source, not affected by offsets */
	double m_raj2000,m_decj2000;
	/** Apparent equatorial coordinates */
	double m_ra2,m_dec2,m_julianEpoch2;
	/** galactic coordinates, included galactic offsets */
	double m_galLong2,m_galLat2;
	/** Horizontal coordinates */
	double m_az,m_el;
	/** Parallactic angle */
	double m_parallacticAngle;
	/** Horizontal offsets in radians */
	double m_azOff,m_elOff;
	/** Equatorial Offsets in radians */
	double m_raOff,m_decOff;
	/** Galactic Offsets in radians */
	double m_longOff,m_latOff;
	/** if true the source was a fixed point in the horizontal frame */
	bool m_fixed;
	/* frame of the input coordinates */
	TFrame m_inputFrame;
	/** frame of the offsets */
	TFrame m_offsetsFrame;
	/**
	 * Transform the input coordinates to FK5 J2000 system (current epoch)
	*/	
	void inputToFK5(const CDateTime& time);
	/**
	 * reinitialize the object when new offsets are commanded
	 */
	void setOffsets();
};

}

#endif /*SKYSOURCE_H_*/
