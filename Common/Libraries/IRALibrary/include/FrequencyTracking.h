/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                 when           What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  17/04/2013     Creation                                          */

#ifndef FREQUENCYTRACKING_H_
#define FREQUENCYTRACKING_H_

#include "Site.h"
#include "DateTime.h"

namespace IRA {

/**
 * This class computes the topocentric frequency for spectroscopy observations in order to track the velocity of an astronomical object. the sign convention is that  a velocity V>o means recession.
 * The parameters that influence the computation are: the object coordinates
 * (the J2000 right ascension and declination), the rest frequency and the source radial velocity. In order to define the source velocity a user need to specify:
 * @arg \c 1 the radial velocity value (Vrad)
 * @arg \c 2 the velocity definition (Vdef)
 * @arg \c 3 the reference frame (Vref)
 * The supported reference frames are the topocentric (<i>TOPOCEN</i>), barycentric (<i>BARY</i>), the kinematic LSR (<i>LSRK</i>), the dynamical LSR (<i>LSRD</i>), the Galactocentric (<i>GALCEN</i>)
 * and the Local Group (<i>LGROUP</i>). The case of topocentric frame leads the class to consider the observer velocity as null, that means  the topocentric frequency do not change during the observation and
 * that if  the provided radial velocity (Vrad) is also null, the observed line transition is exactly the rest frequency.
 * The available velocity definitions are <i>RADIO</i>, <i>OPTICAL</i> and <i>REDSHIFT</i>. If <i>REDSHIFT</i> is used the corresponding Vrad value is unitless and is considered to be the Z=V/C (redshift),
 * where C is the speed of light.The other two cases imply a unit of km/sec. The Vdef parameter defines how the Vrad is converted into frequency. The formulas are the following, where f0 is the rest frequency:
 *@HTMLONLY <table border="1"><tr><th>Radio</th><th>Optical</th><th>Redshift</th></tr><tr><td>f(Vrad)=f0(1-V/C)</td><td>f(Vrad)=f0/(1+V/C)</td><td>f(Z)=f0/(1+Z)</td></tr></table><br> @ENDHTMLONLY
 * The velocity of the specified frame (Vref) with respect the barycenter is taken according the following table:
 * @HTMLONLY <table border="1"><tr><th>Frame</th><th>J2000 alfa and delta wrt barycenter</th><th>J2000 cartesian wrt barycenter</th></tr>
 *                                                                   <tr><td><i>LSRK</i></td><td>20.0 Km/sec, 18:03:50.24, +30:00:16.8</td><td>0.28998,-17.31727,10.00141</td></tr>
 *                                                                   <tr><td><i>LSRD</i></td><td>16.55 Km/sec, 17:49:58.66, +28:07:03.92</td><td>-0.63823,-14.58542,+7.80116</td></tr>
 *                                                                   <tr><td><i>GALCEN</i></td><td>232.3 Km/sec, 20:55:26.77, +47:49:23.5</td><td>108.06585, -112.44793, 172.13725</td></tr>
 *                                                                   <tr><td><i>LGROUP</i></td><td>300 Km/sec, 21:12:01.05, +48:19:46.71</td><td>148.23284, -133.44888, 224.09467</td></tr>
 *                                                                   </table><br> @HTMLONLY
 * The computation consists in the following steps:
 * @arg \c A f1=f(Vrad), according the provided definition as reported in the first table
 * @arg \c B The vector velocity Vb of the barycenter with respect the observer is computed with slalib (Earth rotation is taken into account)
 * @arg \c C the vectro velocity Vf of the specified frame (Vref) with respect to the barycenter is computed according the second table
 * @arg \c D the resulting vector Vt=vb+Vf is obtained
 * @arg \c E the topocentric frequency is then calculated with the relativistic formula: Ftop=f1*sqrt(1-(|Vt|/C)^2)/ (1+VtxS/C), where x represents the dot product and S is the unit vector in the direction of the source.
 *
 * @HTMLONLY A reference could be found  <a href="http://www.gb.nrao.edu/~fghigo/gbtdoc/doppler.html">here</a><br> @HTMLONLY
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br>
*/
class CFrequencyTracking {
public:
	typedef struct  {
		double x;
		double y;
		double z;
		double m;
		double toSource;
	} TCartesian;
	typedef enum {
		LSRK,
		LSRD,
		BARY,
		GALCEN,
		LGROUP,
		TOPOCEN
	} TFrame;
	typedef enum {
		RADIO,
		OPTICAL,
		REDSHIFT
	} TDefinition;
	/**
	 * Constructor
	 * @param site information on the observer
	 * @param ra2000 J2000 right ascension of the target
	 * @param dec2000 J2000 declination of the target
	 * @param frame gives the frame the velocity (or redshift) refers to
	 * @param def defines the formula used when converting from velocity to frequency and viceversa
	 * @param rest frequency of the line in laboratory (MHz)
	 * @param v radial velocity of the source, if the definition is RADIO or OPTICAL it is in Km/sec otherwise it is unitless redshift
	 */
	CFrequencyTracking(const CSite& site,const double&ra2000,const double& dec2000,const TFrame& frame,const TDefinition& def,const double& rest,const double& v);
	/**
	 * Get the barycenter velocity with respect the Earth.
	 * @param time reference time
	 * @return the cartesian velocity vector, which represents the velocity along the 3 cartesian axis (Km/sec)
	 */
	TCartesian getBarycentricVelocity(const CDateTime& time) const;

	/**
	 * Get the velocity components due to Earth rotation
	 * @param time reference time
	 * @return the cartesian velocity vector, which represents the velocity along the 3 cartesian axis (Km/sec)
	 */
	TCartesian getEarthRotationVelocity(const CDateTime& time) const;

	/**
	 * Get the velocity of the Kinematic LSR with respect to the barycenter.
	 * @param time reference time
	 * @return the cartesian velocity vector, which represents the velocity along the 3 cartesian axis (Km/sec)
	 */
	TCartesian getLSRKVelocity(const CDateTime& time) const;

	/**
	 * Get the velocity of the Dynamic LSR with respect to the barycenter.
	 * @param time reference time
	 * @return the cartesian velocity vector, which represents the velocity along the 3 cartesian axis (Km/sec)
	 */
	TCartesian getLSRDVelocity(const CDateTime& time) const;

	/**
	 * Get the galactocentric velocity with respect to the barycenter.
	 * @param time reference time
	 * @return the cartesian velocity vector, which represents the velocity along the 3 cartesian axis (Km/sec)
	 */
	TCartesian getGALCENVelocity(const CDateTime& time) const;

	/**
	 * Get the local group  velocity with respect to the barycenter.
	 * @param time reference time
	 * @return the cartesian velocity vector, which represents the velocity along the 3 cartesian axis (Km/sec)
	 */
	TCartesian getLGROUPVelocity(const CDateTime& time) const;

	/**
	 * Get the resulting velocity of the observer after taking into account all the contribution
	 * @param time reference time
	 * @return the cartesian velocity vector, which represents the velocity along the 3 cartesian axis (Km/sec)
	 */
	TCartesian getObserverVelocity(const CDateTime& time) const;

	/**
	 * computes the frequency of the line with respect to selected frame. It start from the provided velocity (or redshift) and definition.
	 * @return the computed frequency (MHz)
	 */
	double getFrequencyAtFrame() const;

	/**
	 * computes the topocentric frequency of the line at the observation spot
	 * @param time reference time
	 * @return the computed frequency (MHz)
	 */
	double getTopocentricFrequency(const CDateTime& time) const;

	static bool frameToStr(const TFrame& frame,IRA::CString& ret);
	static bool strToFrame(const IRA::CString& str,TFrame& frame);
	static bool definitionToStr(const TDefinition& def,IRA::CString& ret);
	static bool strToDefinition(const IRA::CString& str,TDefinition& def);

	void test(const CDateTime& time);

private:
	CSite m_site;
	double m_ra2000;
	double m_dec2000;
	TFrame m_frame;
	TDefinition m_def;
	double m_rest;
	double m_vrad;

	/**
	 * Initialize a cartesian vector from a standard C array.
	 * @param v the standard 3 dimension C array.
	 * @return the new 3 dimensions, cartesian vector
	 */
	TCartesian cCopy(double v[3]) const;

	/**
	 * Multiply a 3 dimensions vector for a scalar
	 * @param vect input vector
	 * @param scalar value to be multiplied
	 * @return the new 3 dimension vector
	 */
	TCartesian cMult(const TCartesian& vect,const double& scalar) const;

	/**
	 * Compute the resulting vector of three, 3-dimensions vectors
	 * @param v1 first input 3-dimensions vector
	 * @param v2 second input 3-dimensions vector
	 * @param v3 third input 3-dimensions vector
	 */
	TCartesian cAdd(const TCartesian& v1,const TCartesian& v2,const TCartesian& v3) const;

	/**
	 * Reset a 3-dimensions vector.
	 * @return the cartesian vector with all components zeroed.
	 */
	TCartesian cZero() const;

	/**
	 * compute the contribution of a 3 dimension velocity vector toward the direction given by the target source. The source is the one given by the constructor of this class.
	 * @param time current time
	 * @param v 3 dimension velocity array
	 * @return the resulting contribution
	 */
	double toSource(const CDateTime& time, const TCartesian& v) const;

	/**
	 * compute the contribution of a 3 dimension velocity vector toward the direction given by the target source. The source is the one given by the constructor of this class.
	 * @param time current time
	 * @param v 3 dimension velocity array
	 * @return the resulting contribution
	 */
	double toSourceJ2000( const TCartesian& v) const;

	/**
	 * Compute the magnitude of a 3 dimensions vector;
	 * @return the magnitude of the vector
	 */
	double magnitude(const TCartesian& vect) const;

	/**
	 * Performs the scalar product(dot product) of two 3-dimensions vectors
	 * @param v1 first  input vector
	 * @param v2 second input vector
	 * @return the dot product
	*/
	double scalar(const TCartesian& v1,const TCartesian& v2) const;


};

};


#endif /* FREQUENCYTRACKING_H_ */
