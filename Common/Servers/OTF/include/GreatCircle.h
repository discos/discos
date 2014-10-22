/* ***************************************************************** */
/* IRA Istituto di Radioastronomia                                   */
/* $Id: GreatCircle.h,v 1.1 2008-05-23 15:47:43 a.orlati Exp $                                                            */
/*                                                                   */
/* This code is under GNU General Public Licence (GPL)               */
/*                                                                   */
/* Who                                  when            What         */
/* Simona Righini						31/03/2008      created      */
/* Simona Righini						21/04/2008		modified	 */

#ifndef GREATCIRCLE_H_
#define GREATCIRCLE_H_

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/**
 * Given the spherical coordinates of two points, class computes the amplitude 
 * of the great circle arc connecting them (psi) and the spherical coordinates 
 * of the arc middle point. 
 * Given an angular separation from the first point (phi), class computes the
 * spherical coordinates of the point on the great circle arc standing at
 * such angular distance. Algorithm works also for phi<0 or phi>psi, i.e for
 * points of the great circle standing out of the arc connecting the input points.
 * @author <a href=mailto:righin_s@ira.inaf.it>Simona Righini</a>,
 * Istituto di Radioastronomia, Italy
 */

class GreatCircle {
public:	

	double psi;
	
	typedef struct {
		double l;
		double b;
	} Tspherical;
	
	GreatCircle();
	
	virtual ~GreatCircle();
	
	/**
	 * Method sets the spherical coordinates of the start and stop positions. 
	 * It also computes the amplitude of the great circle arc connecting them.
	 * spherical->cartesian->spherical coordinates conversions are managed
	 * internally.
 	 */
	void setGreatCircle(const Tspherical& sph_start, const Tspherical& sph_stop);
	
	/**
	 * Method gets the spherical coordinates of the great circle arc middle point. 
	 */	
	const Tspherical& getGreatCircleCenter();

	/**
	 * Method gets the spherical coordinates of the arc point of given angular 
	 * distance from the start position. 
	 */	
	const Tspherical& getGreatCirclePoint(const double& phi); 
	

private:
	typedef struct {
		double x;
		double y;
		double z;
	} Tcartesian;
	
	Tspherical m_sph_start;
	Tspherical m_sph_stop;
	Tcartesian m_cart_start;
	Tcartesian m_cart_stop;
	Tspherical m_sph_center;
	Tspherical m_sph_point;
	double m_psi;
	
	GreatCircle(const GreatCircle& src);
	void operator=(const GreatCircle& src);

	
	/**
	 * Conversion from spherical to cartesian coordinates
	 */
	static void sph2Cart(const Tspherical& sph, Tcartesian& cart); 

	/**
	 * Conversion from cartesian to spherical coordinates
	 */
	static void cart2Sph(const Tcartesian& cart, Tspherical& sph);

};



#endif /*GREATCIRCLE_H_*/
