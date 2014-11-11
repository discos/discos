// $Id: test_moon.cpp,v 1.2 2008-08-26 08:47:41 r.verma Exp $

//#include "MoonImpl.h"
//#include <iostream.h>
#include <Definitions.h>
#include <ObservatoryS.h>
#include <slamac.h>
#include <slalib.h>
#include <baci.h>
#include <math.h>
#include <acstimeEpochHelper.h>
#include "Site.h"
#include <DateTime.h>

using namespace ComponentErrors;
using namespace baci;
using namespace IRA;
using namespace std;

int main ()
{

		IRA::CSite m_site;
		m_site.geodeticCoordinates(11.6469166*DD2R,44.5205*DD2R,28);
		
		double TDB;
		double m_dut1;// = 0.0;
		TIMEVALUE now;
		IRA::CIRATools::getTime(now);
		IRA::CDateTime date(now,m_dut1);
		TDB=date.getTDB(m_site);
	
		/* Calculating approximately geocentric position and velocity of the Moon (double precision) */
			
		TDB = TDB - 2400000.5; // TDB as a modified Julian Date (JD - 2400000.5)
			
			double pv[6];                          // Moon [x,y,z,vx,vy,vz], mean equator and equinox of date( AU, AU/sec)
			slaDmoon (TDB , pv);               // geocentric position/velocity of the moon (mean of date)
	
/* Form the matrix of nutation for a given date (true eqinox)  */
				
				double rmatn[3][3];                  // Its a 3x3 nutation matrix
				slaNut (TDB , rmatn);            // nutation to the true eq

/*  Multiply the position and velocity vector by a rotation matrix ( double presicion )  */ 
						
					double pv_rot[6];                     // Moon's position and velocity after applying the rotation matrix 
					slaDmxv (rmatn, pv, pv_rot );     /* Since rotation matrix is 3X3 therefore applying
					                                         rotation to first to the position*/
					slaDmxv (rmatn, pv+3, pv_rot+3);  /* further applying the rotation matrix to the 
						                                      velocity of moon */
			//		printf("position_rotation:%e %e %e\n  %e %e %e\n ", pv[1], pv[2],
					//		pv[3],  pv[4], pv[5], pv[6]);
					
			/*   Calling attribute LST from Class CDateTime */		
								
						double LST;                           /* local apperant sidereal time */
						LST= date.LST(m_site).getDayRadians();
						printf("LST: %e \n\n", LST);
						
			/*   Geocentric position of the observer (true equator and equinox of the date ) */
			/*   Observatory position & velocity  */		

						double pvo[6];                        /* Position and velocity of the observatory in AU/sec, true 
							                                          equator and equinox of date   */
		   /*           slaPvobs (m_latitude, m_height, LST, pvo );	*/
						slaPvobs (44.5205*DD2R,28, LST, pvo );	
				
		  /*  Now placing the origin at the observer. get the location of the moon w r to observer */
								
							int i ; 
						       for (i = 0; i < 6; i++) {
								  pv_rot[i] -= pvo[i]; 

						//		printf(" %e  %e \n ", pv_rot[i] ,pvo[i] );
						       }
						
		 /*  Topoentric RA and Dec  */
	  	/*  Converting the cartisian coordinates in to spherical	coordinates in radians	 */
	    		
	   /* spherical coordinates in radians, spherical coordinates are
						       	                                        longitude (+ anticlockwise looking form the + latitude and the latitude )*/
						       double m_rm;
						       double m_dm;
						       int ihmsf[4];
						       char s;
						       slaDcc2s ( pv_rot, &m_rm, &m_dm ); //rm dm in radians
						       slaDr2tf ( 2, m_rm, &s, ihmsf ); // angle in hours, minute, seconds, fraction
						     //  printf(" ra:  %c %2.2d %2.2d %2.2d %2.2d \n",s,ihmsf[0] ,ihmsf[1], ihmsf[2],ihmsf[3]) ;

						       slaDr2tf ( 2, slaDranrm ( LST - m_rm ), &s, ihmsf ); // angle in hours, minute, seconds, fraction
						     //  printf(" topocentric:   %c %2.2d %2.2d %2.2d %2.2d ",s,ihmsf[0] ,ihmsf[1], ihmsf[2],ihmsf[3]) ;
						       
						       slaDr2af (1, m_dm, &s, ihmsf);
						     //  printf("  %c %2.2d %2.2d %2.2d %2.2d ",s,ihmsf[0] ,ihmsf[1], ihmsf[2],ihmsf[3]) ;



}


