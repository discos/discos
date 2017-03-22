#ifndef ACUDATA_H
#define ACUDATA_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: ACUData.h,v 1.7 2010-10-14 12:08:28 a.orlati Exp $                                              */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                              when            What                                             */
/* Andrea Orlati(a.orlati@ira.inaf.it)  23/03/2005     Creation                                        */
/* Andrea Orlati(a.orlati@ira.inaf.it)  14/11/2006     Added getFreeStackPositions() and getSection methods  */
/* Andrea Orlati(a.orlati@ira.inaf.it) 25/01/2008      Added function absTime                 */
/* Andrea Orlati(a.orlati@ira.inaf.it) 14/02/2008      Fixed a bug in absTime                 */

#include <IRA>
#include "ACUInterface.h"

/** buffers dimension */
#define ACU_BUFFERSIZE 128
#define PI 3.141592653589793
#define DEG2RAD 0.0174532925199433

using namespace IRA;

/**
 * This class implements the ACU data structure. These data are used (read and written) inside to <i>MedicinaACUSocket</i> class
 * in order to exchange information with the ACU.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
 */
class CACUData {
public:
	/**
	 * Constructor.
	*/
	CACUData();
	/**
    * Destructor.
   */
	~CACUData();
	
	/** Returns the azimuth in degrees. */
	double azimuth() const;
	
	/** Returns the azimuth in degrees. */
	double elevation() const;
	
	/** Returns the elevation error in degrees. */
	double elevationError() const;
	
	/** Returns the azimuth error in degrees. */
	double azimuthError() const;

   /** Returns the present azimuth rate (deg/sec) */
	double azimuthRate();
	
	/** Returns the preset elevation rate (deg/sec)*/
	double elevationRate();
	
	/** Returns the azimuth actual mode */
	CACUInterface::TAxeModes azimuthMode() const;
	
	/** Returns the elevation actual mode */
	CACUInterface::TAxeModes elevationMode() const;
	
	/** Returns the status word of the elevation axis */
	WORD elevationServoStatus() const;
	
	/** Returns the status word of the elevation axis */
	WORD azimuthServoStatus() const;
	
	/** Returns the servo status word, only the first seven bits are meaningful */
	WORD servoSystemStatus() const;
	
	/** Returns the ACU time,  */
	void time(TIMEDIFFERENCE& tm) const;
	
	/** Returns the ACU time as an absolute mark */
	void absTime(TIMEVALUE& tm) const;
	
	/** Returns the section which the antenna is located in, at present */
	CACUInterface::TAntennaSection getSection() const;
	
	/**Returns the number of free stack slots reserved for time tagged program track positions */
	WORD getFreeStackPositions() const;
	
	/** Returns true if the antenna total sky error is less or equal to Precision */
	bool isTracking(double Precision) const;
	
	/** Fills up the buffer with the monitor data from the ACU */
	void setMonitorBuffer(BYTE* buffer,WORD size=ACU_BUFFERSIZE);
	
	/** Returns the pointer to the TIMEVALUE when the monitor buffer has been updated */
	inline TIMEVALUE* monitorTime() { return &m_MonitorTime; }
	
	/** Returns the current azimuth offset */
	inline double getAzimuthOffset() const { return m_azimuthOff; }

	/** Returns the current elevation offset */
	inline double getElevationOffset() const { return m_elevationOff; }
	
	/** Sets the current azimuth offset */
	inline void setAzimuthOffset(const double& azoff) { m_azimuthOff=azoff; }
	
	/** Sets the current elevation offset */
	inline void setElevationOffset(const double& eloff) { m_elevationOff=eloff; }

	/** 
	 * Returns the current deltaTime. It may represents the UT1-UTC difference, for the Medicina ACU is just a placeholder, so
	 * 0.0 is always returned.
	*/
	inline double getDeltaTime() const { return 0.0; }
	
	/** 
	 * Sets the current deltaTime. For the Medicina ACU is just a placeholder.
	*/
	inline void setDeltaTime(const double& dt) { }
	
	/*
	 * Sets the last commanded azimuth position.
	*/
	inline void setCommandedAzimuth(const double& az) { m_commandedAzimuth=az; m_isCommanded=true; }
	
	/**
	 * Gets the last commanded azimuth position. 
	*/
	inline double getCommandedAzimuth() const { return m_commandedAzimuth; }
	
	/*
	 * Sets the last commanded elevation position.
	*/
	inline void setCommandedElevation(const double& az) { m_commandedElevation=az; m_isCommanded=true; }
	
	/**
	 * Gets the last commanded azimuth position. 
	*/
	inline double getCommandedElevation() const { return m_commandedElevation; }
	
	/**
	 *  Indicates wether at least position has already been commanded or not
	 */
	inline bool getIsCommanded() const { return m_isCommanded; }
	
	/**
	 * allows to set hte last commanded mode.
	 */
	void setLastCommandedMode(const CACUInterface::TAxeModes& mode) { m_lastCommandedMode=mode; }
	
	/**
	 * return the last commanded mode in the ACU
	 */
	inline CACUInterface::TAxeModes getLastCommandedMode() const { return m_lastCommandedMode; }
	
	/**
	 * This function resumes the status of the acu.
	 * @return @arg \c 1 if the ACU is in its normal working status
	 *         @arg \c 2 if an error is present, i.e. a condition that should not affect the overall mount system
	 *         @arg \c 3 if a failure is detected, i.e. a condition that compromises the mount functionality 
	*/
	int getError() const;
	
private:
	/** monitor data buffer */
	BYTE m_MonitorData[ACU_BUFFERSIZE];
	/** Indicates when the monitor data buffer has been updated the last time */
	TIMEVALUE m_MonitorTime;
	/** Indicates the azimuth offsets (not directly read from the ACU) */
	double m_azimuthOff;
	/** Indicates the elevation offsets (not directly read from the ACU) */
	double m_elevationOff;
	/** Last known azimuth position */
	double m_lastAzimuthPosition;
	/** Last known elevation position */
	double m_lastElevationPosition;
   /** time of the last known position */
	TIMEDIFFERENCE m_lastTime;
	/** last commanded azimuth position */
	double m_commandedAzimuth;
	/** last commanded elevation position */
	double m_commandedElevation;
	/** true if at least one position was commanded */
	bool m_isCommanded;
	/**
	 * reports the last commaned mode
	 */
	CACUInterface::TAxeModes m_lastCommandedMode;
};

#endif // ACUDATA_H
