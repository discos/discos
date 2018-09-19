#include "slalib.h"
#include "slamac.h"
double slaDat ( double utc )
/*
**  - - - - - - -
**   s l a D a t
**  - - - - - - -
**
**  Increment to be applied to Coordinated Universal Time UTC to give
**  International Atomic Time TAI.
**
**  (double precision)
**
**  Given:
**     utc      double      UTC date as a modified JD (JD-2400000.5)
**
**  Result:  TAI-UTC in seconds
**
**  Notes:
**
**  1  The UTC is specified to be a date rather than a time to indicate
**     that care needs to be taken not to specify an instant which lies
**     within a leap second.  Though in most cases the utc argument can
**     include the fractional part, correct behaviour on the day of a
**     leap second can only be guaranteed up to the end of the second
**     23:59:59.
**
**  2  For epochs from 1961 January 1 onwards, the expressions from the
**     file ftp://maia.usno.navy.mil/ser7/tai-utc.dat are used.
**
**  3  The 5ms time step at 1961 January 1 is taken from 2.58.1 (p87) of
**     the 1992 Explanatory Supplement.
**
**  4  UTC began at 1960 January 1.0 (JD 2436934.5) and it is improper
**     to call the function with an earlier epoch.  However, if this
**     is attempted, the TAI-UTC expression for the year 1960 is used.
**
**     :-----------------------------------------:
**     :                                         :
**     :                IMPORTANT                :
**     :                                         :
**     :  This function must be updated on each  :
**     :     occasion that a leap second is      :
**     :                announced                :
**     :                                         :
**     :  Latest leap second:  2015 June 30      :
**     :                                         :
**     :-----------------------------------------:
**
**  Last revision:   9 July 2016
**
**  Copyright P.T.Wallace.  All rights reserved.
*/
{

/* - - - - - - - - - - - - - - - - - - - - - */
/* Add new code here on each occasion that a */
/* leap second is announced, and also update */
/* the preamble comments appropriately.      */
/* - - - - - - - - - - - - - - - - - - - - - */

/* 2017 January 1 */
	if ( utc >= 57754.0 ) return 37.0;

/* 2015 July 1 */
   if ( utc >= 57204.0 ) return 36.0;

/* 2012 July 1 */
   if ( utc >= 56109.0 ) return 35.0;

/* 2009 January 1 */
   if ( utc >= 54832.0 ) return 34.0;

/* 2006 January 1 */
   if ( utc >= 53736.0 ) return 33.0;

/* 1999 January 1 */
   if ( utc >= 51179.0 ) return 32.0;

/* 1997 July 1 */
   if ( utc >= 50630.0 ) return 31.0;

/* 1996 January 1 */
   if ( utc >= 50083.0 ) return 30.0;

/* 1994 July 1 */
   if ( utc >= 49534.0 ) return 29.0;

/* 1993 July 1 */
   if ( utc >= 49169.0 ) return 28.0;

/* 1992 July 1 */
   if ( utc >= 48804.0 ) return 27.0;

/* 1991 January 1 */
   if ( utc >= 48257.0 ) return 26.0;

/* 1990 January 1 */
   if ( utc >= 47892.0 ) return 25.0;

/* 1988 January 1 */
   if ( utc >= 47161.0 ) return 24.0;

/* 1985 July 1 */
   if ( utc >= 46247.0 ) return 23.0;

/* 1983 July 1 */
   if ( utc >= 45516.0 ) return 22.0;

/* 1982 July 1 */
   if ( utc >= 45151.0 ) return 21.0;

/* 1981 July 1 */
   if ( utc >= 44786.0 ) return 20.0;

/* 1980 January 1 */
   if ( utc >= 44239.0 ) return 19.0;

/* 1979 January 1 */
   if ( utc >= 43874.0 ) return 18.0;

/* 1978 January 1 */
   if ( utc >= 43509.0 ) return 17.0;

/* 1977 January 1 */
   if ( utc >= 43144.0 ) return 16.0;

/* 1976 January 1 */
   if ( utc >= 42778.0 ) return 15.0;

/* 1975 January 1 */
   if ( utc >= 42413.0 ) return 14.0;

/* 1974 January 1 */
   if ( utc >= 42048.0 ) return 13.0;

/* 1973 January 1 */
   if ( utc >= 41683.0 ) return 12.0;

/* 1972 July 1 */
   if ( utc >= 41499.0 ) return 11.0;

/* 1972 January 1 */
   if ( utc >= 41317.0 ) return 10.0;

/* 1968 February 1 */
   if ( utc >= 39887.0 ) return 4.2131700 + ( utc - 39126.0 ) * 0.002592;

/* 1966 January 1 */
   if ( utc >= 39126.0 ) return 4.3131700 + ( utc - 39126.0 ) * 0.002592;

/* 1965 September 1 */
   if ( utc >= 39004.0 ) return 3.8401300 + ( utc - 38761.0 ) * 0.001296;

/* 1965 July 1 */
   if ( utc >= 38942.0 ) return 3.7401300 + ( utc - 38761.0 ) * 0.001296;

/* 1965 March 1 */
   if ( utc >= 38820.0 ) return 3.6401300 + ( utc - 38761.0 ) * 0.001296;

/* 1965 January 1 */
   if ( utc >= 38761.0 ) return 3.5401300 + ( utc - 38761.0 ) * 0.001296;

/* 1964 September 1 */
   if ( utc >= 38639.0 ) return 3.4401300 + ( utc - 38761.0 ) * 0.001296;

/* 1964 April 1 */
   if ( utc >= 38486.0 ) return 3.3401300 + ( utc - 38761.0 ) * 0.001296;

/* 1964 January 1 */
   if ( utc >= 38395.0 ) return 3.2401300 + ( utc - 38761.0 ) * 0.001296;

/* 1963 November 1 */
   if ( utc >= 38334.0 ) return 1.9458580 + ( utc - 37665.0 ) * 0.0011232;

/* 1962 January 1 */
   if ( utc >= 37665.0 ) return 1.8458580 + ( utc - 37665.0 ) * 0.0011232;

/* 1961 August 1 */
   if ( utc >= 37512.0 ) return 1.3728180 + ( utc - 37300.0 ) * 0.001296;

/* 1961 January 1 */
   if ( utc >= 37300.0 ) return 1.4228180 + ( utc - 37300.0 ) * 0.001296;

/* Before that. */
                         return 1.4178180 + ( utc - 37300.0 ) * 0.001296;

}
