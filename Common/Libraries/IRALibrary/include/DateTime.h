/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: DateTime.h,v 1.10 2010-07-30 18:56:56 a.orlati Exp $										                                                            */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                 when           What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  17/01/2006     Creation                                          */
/* Andrea Orlati(aorlati@ira.inaf.it)  27/02/2007     added some feature based on slalib                */
/* Andrea Orlati(aorlati@ira.inaf.it)  01/01/2007     Refurbished using slalib                          */
/* Andrea Orlati(aorlati@ira.inaf.it)  05/02/2008     Added m_dut1 in the computation of TDB                    */
/* Andrea Orlati(aorlati@ira.inaf.it)  06/02/2008     added the parameter dut1 in every constructor of the class     */
/* Andrea Orlati(aorlati@ira.inaf.it)  11/12/2008     added static function that allows to convert from sidereal time to UT   */
/* Andrea Orlati(aorlati@ira.inaf.it)  27/07/2010     method isLeapYear moved from private to public methods */

#ifndef DATETIME_H
#define DATETIME_H

#include <acstimeEpochHelper.h>
#include "Site.h"

#define DT_SECSPERDAY 86400.0
#define DT_MSECSPERDAY 86400000
#define DT_MJD0 2400000.5
#define DT_J2000 2451545.0 	
#define DT_B1950 2433282.4235 	
#define DT_SIDERSEC 1.00273790935
#define DT_RAD2SIDERSEC (2*DPI/86164.0905)
#define DT_SIDERDAY_HOUR 23.9344696

namespace IRA {

/**
 * This class stores and manages date and time. Time is stored in local variables both as julian days and
 * years, month, days of then month and milliseconds of day. This allows to change any parameter of the two format. A change
 * in one format causes the change in the other one in order to keep the both of them updated. The time resolution that this class can provide is
 * one millisecond.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CDateTime {
public:
	
	/**
	 * Constructor. It creates the date and time object from a julian day.
	 * @param jd julian day, default value is the J2000(2451545.0)
	 * @param dut1 the difference between UT1 and UTC in milliseconds.
	*/
	CDateTime(const double& jd=DT_J2000,const double& dut1=0.0);

	/**
	 *  Constructor which creates a Date and Time object from the calendar value:
	 *  year, month, day and milliseconds of the day
	 * @param year an integer for the year.
	 * @param month an integer for the month.
	 * @param day an integer for the day.
	 * @param ms an integer for the milliseconds of the day.
	 * @param dut1 the difference between UT1 and UTC in milliseconds.
	*/
	CDateTime(const int& year,const int& month,const int& day,const long& ms,const double& dut1=0.0);

	/**
	* Constructor which creates a Date and Time object from the calendar value:
	* year, month, day and part of the day
	* @param year an integer for the year.
	* @param month an integer for the month.
	* @param day an integer for the day.
	* @param ut a long double for the fraction of the day.
	* @param dut1 the difference between UT1 and UTC in milliseconds.
	*/
	CDateTime(const int& year,const int& month,const int& day,const double& ut,const double& dut1=0.0);

	/**
	* Constructor which creates a Date and Time object from the calendar value:
	* year, month, day and part of the day
	* @param year an integer for the year.
	* @param month an integer for the month.
	* @param day an integer for the day.
	* @param hour an integer for the hour.
	* @param minute an integer for the minute.
	* @param second an integer for the second.
	* @param ms an integer for the milliseconds.
	* @param dut1 the difference between UT1 and UTC in milliseconds.
	*/
	CDateTime(const int& year,const int& month,const int& day,const int& hour,const int& minute,const int& second,const long& ms,const double& dut1=0.0);

	/**
	 * Contructor which creates a Date and Time object from an ACS DurationHelper object.
	 * @param now EpochHelper object
	 * @param dut1 the difference between UT1 and UTC in milliseconds.
	*/
	CDateTime(TIMEVALUE& now,const double& dut1=0.0);
	
	/**
	 *  Copy constructor. Creates a Date and Time object from another Date and Time object.
	 *  @param dt a Date and Time object
	*/
	CDateTime(const CDateTime& dt);

	/**
	 * Destructor
	*/
	~CDateTime();
	
	/**
	 * Copy operator.
	*/
	const CDateTime& operator =(const CDateTime& src);
	
	/**
	 * Sets the current DUT1. Dut1 is the the difference UT1-UTC, if a zero is given, the UTC will be used in place of UT1
	 * that means loss of precision.
	 * @param dut1 the difference between UT1 and UTC in milliseconds.
	*/
	void setDut1(const double& dut1);
	
	/**
	 * @return the current DUT1 in milliseonds.
	*/
	double getDut1() const;
	
	/**
	 * This method is used to load the computer system time as the current time. This should be the current UT time.
	 * @param dut1 the difference between UT1 and UTC in milliseconds.
	*/
	void setCurrentDateTime(const double& dut1=0.0);
	
	/** 
	 * This method sets the time reported by an ACS EpochHelper object.
	 * @param now the time that must be set
	 * @param dut1 the difference between UT1 and UTC in milliseconds.
	*/
	void setDateTime(TIMEVALUE& now,const double& dut1=0.0);
	
	/** 
	 * This method is used to get the current calendar in the form of an ACS EpochHelper object
	 * @param time the ACS EpochHelper object.
	*/ 
	void getDateTime(TIMEVALUE& time) const;
	
	/**
	 * This method is used to get the current calendar in the form of an ACS DurationHelper object. 
	 * The information of the year is lost.
	 * @param time the ACS DurationHelper object.
	*/
	void getDateTime(TIMEDIFFERENCE& time) const;
	
	/**
	 * Returns the julian epoch of the date.
	 * @return the epoch which is the year with the fracional part of the year.
	*/
	double getJulianEpoch() const;

	/**
	 * Returns the besselian epoch of the date.
	 * @return the epoch which is the year with the fracional part of the year.
	*/	
	double getBesselianEpoch() const;
	
	/**
	 * @return the year of the current calendar
	*/
	inline const int getYear() const { return m_year; }
	
	/**
	 * Sets the year of the current calendar
	*/
	void setYear(const int& year) { setCalendar(year,m_month,m_day,m_msecs); }
	
	/**
	 * This method will return the month of the current calendar.
	 * @return 1 for january, ...., 12 for december
	*/
	inline int getMonth() const { return m_month; }
	
	/**
	 * Sets the month of the current calendar.
	*/
	void setMonth(const int& month) { setCalendar(m_year,month,m_day,m_msecs); }
	
	/**
	 * @return the day of the month
	*/
	inline int getDay() const { return m_day; }
	
	/**
	 * Sets the day of the month
	*/
	void setDay(const int& day) { setCalendar(m_year,m_month,day,m_msecs); }
	
	/**
	 * @return the current day of year; 
	*/
	unsigned getDayOfYear() const;
	
	/**
	 * @return the numer of hours of the current dateTime object. No fractionals. 
	*/
	inline int getHours() const { return int(m_msecs/(1000*3600)); }
	
	/**
	 * This method returns the number of minutes. It does not return the total number of minutes since
	 * the begining of the day but the minutes elapsed during the current hour.
	 * @return the number of minutes of the current DateTime object. No fractionals.
	*/
	inline int getMinutes() const { return int((m_msecs/1000)%3600/60); }
	
	/**
	 * This method returns the number of seconds. It does not return the total number of seconds since
	 * the begining of the day but the seconds elapsed during the current minute.
	 * @return the number of seconds of the current DateTime object. No fractionals.
	*/
	inline int getSeconds() const { return int((m_msecs/1000)%3600%60); }
	
	/**
	 * This method returns the number of milliseconds. It does not return the total number of milliseconds since
	 * the begining of the day but the milliseconds elapsed during the current second.
	 * @return the number of milliseconds of the current DateTime object.
	*/
	int getMilliSeconds() const;
	
	/**
	 * @return the milliseconds of the day of the current calendar. Since the begining of the day.
	*/
	inline long getDayMilliSeconds() const { return m_msecs; }
	
	/**
	 * Sets the milliseconds of the day of the current calendar.
	*/
	void setDayMilliSeconds(const long& msecs) { setCalendar(m_year,m_month,m_day,msecs); }
	
	/**
	 * Gets the time of the current day as radians.
	 * @return the number of milliseconds of day converted in radians  
	*/ 
	double getDayRadians() const;
	
	/** 
	 * @return the Date and Time as Julian Day.
	*/
	inline double getJD() const { return m_jd; }
	
	/**
	 * Sets the current Data and Time as Julian Day.
	*/ 
	void setJD(const long double& jd,const double dut1=0.0) { setJulianDay(jd);  setDut1(dut1); }
	
	/** 
	 * @return the Date and Time as a Modified Julian Day.
	*/
	inline long double getMJD() const { return m_jd-DT_MJD0; }
	
	/**
	 * Sets the current Data and Time in the form of Modified Julian Day.
	*/ 
	void setMJD(const long double& jd,const double dut1=0.0) { setJulianDay(jd+DT_MJD0); setDut1(dut1);}
	
	/**
	 * Gets the Iternational Atomic Time. This is the laboratory timescale; The UTC is keeped in pace to TAI by the regular 
	 * introduction of leap secods. At the moment UTC lags TAI by about 33 seconds.
	 * @return the TAI for the object as julian day
	*/ 
	double getTAI() const;
	
	/**
	 * Gets the Terrestrial Time. TT is the theoretical timescale of apparent geocentric ephemerides of solar system bodies.
	 * In practical this time is tied to TAI (International Atomic Time) by the formula TT=TAI+31.184s
	 * @return the TDB for the object as julian day.
	*/
	double getTT() const;
	
	/**
	 * Gets the Barycentric Dynamical Time. The TDB is the Terrestrial Time compensated by periodic terms due to relativistic effects.
	 * The ammount of correction cycles back and forth by 2 milliseconds at maximum.
	 * TDB may be considered to be the coordinate time in solar system barycentre frame of reference.
	 * For most precession/nutation and Earth position and velocity routines this is the most adeguate input time.
	 * @param site contais all the information regarding the observer position on the Earth 
	 * @return the TDB for the object as julian day.
	*/ 
	double getTDB(const CSite& site) const;
	
	/** 
	 * It computes the Greenwich Apparent Sidereal Time (GAST) at the date and time of the object.
	 * The apparent sidereal time is the hour angle of the the real vernal equinox. When the measurement is made
	 * with respect to Greenwich meridian is referred as Mean Greenwich sidereal time. Apparent sidereal time is obtiand from
	 * mean sidereal time by adding a correction factor due to a change in ecliptic longitude due to nutation called 
	 * equation of equinox.
	 * @param site contais all the information regarding the observer position on the Earth     
	 * @return a Date and Time object with the GST.
	*/
	const CDateTime GST(const CSite& site) const;

	/**
	 * It computes the Local Apparent Sidereal Time. This time is calculated as <i>GST()</i> by adding the east longituide of the site.
	 * @param site contais all the information regarding the observer position on the Earth 
	 * @return a Date and Time object with the current LST.
	 */
	const CDateTime LST(const CSite& site) const;
	
	/*
	 * Adds seconds to the object. If seconds are higher than the number of seconds per day, the julian day will be increased.
	 * @param s seconds to add to the object.
	*/
	void addSeconds(const double& s);
	
	/**
 	 * This method checks if a year a leap year or not.
	 * @return true if the argument is a leap year
	 */
	bool isLeapYear(const int& year) const;
	
	/**
	 * This function returns the julian day for a given julian epoch
	 * @param epoch the epoch, the year number with decimals part of year.
	 * @return the correspondig julian day.
	*/
	static double julianEpoch2JulianDay(const double& epoch); 
	
	/**
	 * This function returns the julian epoch for a given julian day. It does exactly the same of
	 * <i>getJulianEpoch()</i> function but it allows to specify the julian day instead of using the one
	 * stored in the object. 
	 * @param jd the julian day to convert
	 * @return the epoch, in the form of year with fracional part of year.
	*/
	static double julianDay2JulianEpoch(const double& jd);
	
	/**
	 * This function returns the julian day for a given Besselian epoch
	 * @param epoch the epoch, the year number with decimals part of year.
	 * @return the correspondig julian day.
	*/
	static double besselianEpoch2JulianDay(const double& epoch); 
	
	/**
	 * This function returns the besselian epoch for a given Julian day. It does exactly the same of
	 * <i>getbesselianEpoch()</i> function but it allows to specify the julian day instead of using the one
	 * stored in the object. 
	 * @param jd the julian day to convert
	 * @return the epoch, in the form of year with fracional part of year.
	*/
	static double julianDay2BesselianEpoch(const double& jd);	
	
	/**
	 * Convert a sidereal time into a UT time given the site and a date. Since the sidereal day is 3 min and 55.9 seconds shorter than
	 * the UT day in a particular case (UT across midnight) there could be two UT time corresponding to one sidereal time. In that case both the UT times are
	 * reported.
	 * @param ut1 output argument, the ut date and time corresponding to the given sidereal time
	 * @param ut2 output argument, the second ut date and time corresponding to the given sidereal time. It is significant only if
	 *                       the function returns false.
	 * @param lst input local sidereal time as radians.
	 * @param site information about the site
	 * @param year this is the year of the observation day
	 * @param month this is the month of the observation day
	 * @param day this is the day of month of the observation day
	 * @param dut1 the difference between UT1 and UTC in milliseconds.
	 * return it will return true if the result is just one, in that case the result if returned in the <i>ut1</i>argument. A false
	 *             is returned in case the result is not unique, in that case the two result are reported in arguments <i>ut1 ut2</i>
	 *            respectively.
	*/
	static bool sidereal2UT(CDateTime& ut1,CDateTime& ut2,const double& lst,const CSite& site,const int& year,
			const int& month,const int& day,const double& dut1=0.0);

	/**
	 * Convert a UT Time to the corresponding LST.
	 * @param lst computed lst value
	 * @param ut  Universal Time to be converted
	 * @param site object that contains site information
	 * @param dut1 the difference between UT1 and UTC in milliseconds.
	 */
	static void UT2Sidereal(TIMEDIFFERENCE& lst,TIMEVALUE& ut,const CSite& site,const double& dut1=0.0);
		
private:
	// Private variable which holds the time of the Time object
	double m_jd;
	int m_day,m_month,m_year;
	long m_msecs;
	double m_dut1; //UT1-UTC as a fraction of the day

	/**
     * Computes the Julian day starting from the calendar parameters.
	 * @param year this is the year of the calendar
	 * @param month this is the month of the calendar
	 * @param day this is the day(of the month) of the calendar
	 * @param ut this the fraction of the day
	 * @return true if the conversion succeeds
	*/
	bool setCalendar(const int& year,const int& month,const int& day,const double& ut); 

	/**
     * Computes the Julian day starting from the calendar parameters.
	 * @param year this is the year of the calendar
	 * @param month this is the month of the calendar
	 * @param day this is the day(of the month) of the calendar
	 * @param ms an integer for the milliseconds of the day.
	 * @return true if the conversion succeeds
	*/
	bool setCalendar(const int& year,const int& month,const int& day,const long& ms);
	
	/**
	 * Computes the calendar from the Julian Day. It may be possible that 
	 * milliseconds may not be exactly computed due to rounding. 
	 * @param jd the JD time (example 2451545.0)
	 * @return true if the input date was ok
	*/
	bool setJulianDay(const double& jd);
			
	/**
	 * Converts from fractional part of the day to milliseconds of the day
	 * @param ut a double representing the fractional part of the day
	 * @return milliseconds of day
	 */
	long utToMsecs(const double &ut) const;

	/**
	 * Converts from milliseconds to fractional part of the day
	 * @param ms a long representing the milliseconds of the day
	 * @return a double with the fractional part of the day
	 */
	 double msecsToUt(const long &ms) const;
	 
	 /**
	  * Converts from milliseconds to radians.
	  * @param ms a long representing the milliseconds of the day
	  * @return the radians of the hour angle( in the range 0..2PI) indicated by the input argument  
	 */
	 double msecsToRadians(const long& ms) const;
	 
	 /**
	 * Converts from radians to milliseconds of the day
	 * @param rad  a double representing an our angle
	 * @return milliseconds of a day 
	 */
	 long radiansToMsecs(const double& rad) const;

};

}


#endif
