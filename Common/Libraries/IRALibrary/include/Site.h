/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: Site.h,v 1.5 2011-06-21 16:31:50 a.orlati Exp $										                                                            */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                 when           What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  26/05/2006     Creation                                          */
/* Andrea Orlati(aorlati@ira.inaf.it)  02/03/2007     Changed all angles so that they are now radians   */

#ifndef CSITE_H
#define CSITE_H

//#include <IRA>
#include <AntennaDefinitionsC.h>

namespace IRA {

/**
 * This class stores and manages the coordinates of any place on Earth. This class is taken from the KStars GeoLocation class written by Jason Harris
 * and Pablo de Vincente (@sa http://developer.kde.org/documentation/library/cvs-api/kdeedu-apidocs/kstars/html/geolocation_8cpp-source.html). 
 * The coordinates are stored both as geodetic coordinates (longitude,latitude,height) and Cartesian coordinates (X,Y,Z).
 * A change in one format causes the change in the other one. The conversion among them depends on the
 * ellipsoid model choosen. Five different models are provided. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CSite {
public:
	/**
	 * This enumerates the available ellipsoid, the values (equatorial radius and flattening) for each model  
	 * are taken here: https://gdsg.ngdc.noaa.gov/oracle/apidoc/oracle/sdoapi/sref/GeodeticEllipsoid.html 
	*/
	enum TGeodeticEllipsoid {
		WGS84=0,
		GRS80=1,
		MERIT83=2,
		OSU91A=3,
		SOVIET85=4		
	};
	
	/**
	 * Default constructor.
	*/
	CSite();
	
	/**
	 * Constructor. It constructs the object starting from a structure of type TObservatoryInformation.
	 * @param site IDL defined structure that contains all required information to initialize a site. 
	*/
	CSite(const Antenna::TSiteInformation& site);
	
	/**
	 * It constructs the site starting from the geodetic coordinates.
	 * @param lng east longitude of the site (radians)
	 * @param lat latitude of the site (radians)
	 * @param hght hight of the site (meters)
	 * @param lpsd elipsoid model, defualt is WGS84
	*/
	void geodeticCoordinates(const double& lng,const double& lat,const double& hght,const TGeodeticEllipsoid& lpsd=WGS84);

	/**
	 * It constructs the site starting from the cartesian coordinates referred to the center of Earth.
	 * @param x represents the X coordinate in meters (X=0 is plane of the Greenwich meridian)
	 * @param y represents the Y coordinate in meters (Y=0 is plane of the meridian 90 degrees east of the Greenwich meridian)
	 * @param z represents the Z coordinate in meters (Z=0 is plane of the equator)
	 * @param lpsd elipsoid model, defualt is WGS84
	*/	
	void cartesianCoordinates(const double& x,const double& y,const double& z,const TGeodeticEllipsoid& lpsd=WGS84);
	
	/**
	 * Sets the polar motion with respect to the IERS reference pole. This information can be used to correct the site longitude
	 * and latitude for polar motion and compute the azimuth difference between celestial and terrestial poles. Information
	 * can be retrived from http://hpiers.obspm.fr/eop-pc/
	 * @param xp motion angle along the Greenwich meridian (mas)
	 * @param yp motion angle along the meridian 90° east (mas)
	*/ 
	void poleMotion(const double& xp,const double& yp);
	
	/**
	 * Copy Constructor. It creates a CSite Object starting from another one.
	 * @param src source CSite object.
	 */
	 CSite(const CSite& src);
	 
	 /**
	  * Destructor.
	 */
	 ~CSite();
	 
	 /**
	  * This method is used to retrieve all the salient information about the object in a single shot. The return type is a struct defined
	  * in the idl so that it can be transported to the network via CORBA. 
	  * @return a structure with the following fields:
	  *       @arg \c longitude				
	  *       @arg \c latitude			    
	  *       @arg \c height                
	  *       @arg \c xP                   
	  *       @arg \c yP                   
	  *		  @arg \c elipsoid               
	 */
	 Antenna::TSiteInformation getSummary();
	 
	 /**
	  * Get the geocentric coordiantes of the site.
	  * @param R distance from the Earth rotation axis in meters
	  * @param Z distance from the plane of the equator in meters
	 */
	 void getGeocentricCoordinate(double& R,double& Z) const;
	 
	 /**
	  * Gets the site geodetic coordinates corrected for polar motion. Polar motion parameters must be set by <i>poleMotion()</i>
	  * first. 
	  * @param longitude true east longitude corrected for polar motion (radians)
	  * @param latitude true latitude of the site corrected for polar motion (radians)
	  * @param deltaAz this value is the correction to be added to the "celestial azimtuh" in order to compute a terrestrial azimuth.
	 */
	 void getTrueGeodetic(double& longitude,double& latitude,double& deltaAz) const;
	 
	 /**
	  * This method changes the current ellispoid model adjusting the geodetic coordinates consequently.
	  * @param lpsd new ellipsoid model.
	 */
	 void setEllipsoid(const TGeodeticEllipsoid& lpsd=WGS84);
	 
	 /**
	  * This method changes the current east longitude for the geodetic coordinates adjusting the cartesian coordinates consequently.
	  * @param longitude the geodetic longitude in radians.
	 */	 
	 void setLongitude(const double& longitude);
	 	 
	 /**
	  * This method changes the current latitude for the geodetic coordinates adjusting the cartesian coordinates consequently.
	  * @param latitude geodetic latitude in radinas.
	 */	 
	 void setLatitude(const double& latitude);
	 
	 /**
	  * This method changes the current height of the site adjusting the cartesian coordinates consequently.
	  * @param hgt double indicating the site height in meters.
	 */	 	 	 
	 void setHeight(const double& hgt);
	 
	 /* 
	  * Sets the X polar motion with respect to the IERS reference pole. This information can be used to correct the site longitude
	  * and latitude for polar motion and compute the azimuth difference between celestial and terrestial poles. Information
	  * can be retrived from http://hpiers.obspm.fr/eop-pc/
	  * @param xp motion angle along the Greenwich meridian (mas)
	 */
	 void setXPoleMotion(const double& xp);

	 /* 
	  * Sets the Y polar motion with respect to the IERS reference pole. This information can be used to correct the site longitude
	  * and latitude for polar motion and compute the azimuth difference between celestial and terrestial poles. Information
	  * can be retrived from http://hpiers.obspm.fr/eop-pc/
	  * @param yp motion angle along the meridian 90° east (mas)
	 */	 
	 void setYPoleMotion(const double& yp);
	 
	 /**
	  * Copy operator.  
	  * @param rSrc source CStire object.
	  * @return the new CStine object.
	 */
	 const CSite& operator=(const CSite& rSrc);
	 
	 /**
	  * It gets the longitude of the site.
	  * @return the radians of the current east longitude of the site.
	 */
	 inline const double& getLongitude() const { return m_longitude; }
	 
	 /**
	  * It gets the latitude of the site.
	  * @return the radians of with the current latitude of the site.
	 */
	 inline const double& getLatitude() const { return m_latitude; }
	 
	 /**
	  * It gets the height if the site.
	  * @returns the height of the site above the surface of the ellipsoid (in meters).
	 */ 
	 inline const double& getHeight() const { return m_height; }
	 
	 /*
	  * It gets the X polar motion with respect to IERS IERS pole.
	  * @return the motion angle along the Greenwich meridian (mas)
	 */
	 const double getXPoleMotion() const;

	 /*
	  * It gets the X polar motion with respect to IERS IERS pole.
	  * @return the motion angle along the meridian 90° east (mas)
	 */	 
	 const double getYPoleMotion() const;
	 
	 /**
	  *  It gets the X coordinate of the site.
	  * @return a double that indicates the X position of the cartesian coordinates.
	 */ 
	 inline const double& getXPos() const { return m_cartX; }
	 
	 /**
	  *  It gets the Y coordinate of the site.
	  * @return a double that indicates the Y position of the cartesian coordinates.
	 */ 
	 inline const double& getYPos() const { return m_cartY; }

	 /** 
	  * It gets the Z coordinate of the site.
	  * @return a double that indicates the Z position of the cartesian coordinates.
	 */ 
	 inline const double& getZPos() const { return m_cartZ; }
	 
	 /**
	  * It gets the current ellipsoid.
	  * @return the symbol that identifies the currenct ellipsod model.
	 */
	 inline const TGeodeticEllipsoid& getEllipsoid() const { return m_index; }
	 
protected:
	/** geodetic coordinates */
	double m_longitude;
	double m_latitude;
	double m_height;
	/** cartesian coordinates */
	double m_cartX,m_cartY,m_cartZ;
	/** these are the coordinates of the celestial ephemeris pole with resepct to reference pole. */
	double m_xP,m_yP;
	/** these are the geodetic coordinates corrected for polar motion */
	double m_trueLong,m_trueLat,m_deltaAz;
	/** ellipsoid parameters */
	double m_axis;
	double m_flat;
	TGeodeticEllipsoid m_index;
	static const double equatorialRadius[5];
	static const double flattening[5];
	// used internally to load the parameters when a new ellipsoid is choosen
	void changeEllipsoid(const TGeodeticEllipsoid& index);
	/**
	 * Computes the cartesian coordinates starting from the geodetic ones.
	*/
	void setGeodetic();

	/**
	 * Computes the geodetic coordinates starting from the cartesian ones.
	*/
	void setCartesian();
	
	/**
	 * Computes the true Longitude and latitude starting from the geodetic coordinates by correcting for polar motion
	*/ 
	void setTrueGeodetic();
};

}

#endif
