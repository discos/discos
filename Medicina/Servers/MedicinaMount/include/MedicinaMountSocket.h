#ifndef MEDICINAACUSOCKET_H
#define MEDICINAACUSOCKET_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: MedicinaMountSocket.h,v 1.11 2010-10-14 12:08:28 a.orlati Exp $																								*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  20/01/2005      Creation                                         */
/* Andrea Orlati(aorlati@ira.inaf.it)  23/03/2005      Added the class CACUData as data storage         */
/* Andrea Orlati(aorlati@ira.inaf.it)  10/11/2006      Added member getServoSystemStatus                */
/* Andrea Orlati(aorlati@ira.inaf.it)  18/04/2007      Ported to ACS 6.0.2                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  25/01/2008     added the function getAbsTime                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  17/07/2008     added getAntennaErrors                             */
/* Andrea Orlati(aorlati@ira.inaf.it)  02/04/2010     added getHWAzimuth method							 */

#include <IRA>
#include <MedicinaMountS.h>
#include "ACUData.h"
#include "ACUInterface.h"
#include "acstime.h"
#include <AntennaErrors.h>
#include <ComponentErrors.h>
#include <TimeTaggedCircularArray.h>
#include "Configuration.h"

using namespace IRA;

#define UNSTOW_ACTION 1
#define STOW_ACTION 2

/**
 * This class is inherited from the IRA::CSocket class. It takes charge of getting data from the ACU and parsing 
 * its responses. New data are requested to the ACU only if the data are not up to date any more. if the remote side
 * disconnects or a problem affects the comunication line, this class try to reconnect to the ACU.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CMedicinaMountSocket: public CSocket
{
public:
	/**
	 * Constructor.
	*/
	CMedicinaMountSocket();
	/**
	 * Destructor
	*/
	virtual ~CMedicinaMountSocket();
	/**
     * This member function is used to enstablish and configure the communication channel to the ACU. 
	 * This must be the first call before using any other function of this class.
	 * @param config pointer to the component configuration data structure
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c IRALibraryResource>
	*/
	void Init(CConfiguration *config) throw (ComponentErrors::SocketErrorExImpl);
	
	/**
	 * called to clean up all the allocated resources before closing
	 */
	void cleanUp();
	
	/** 
	 * This function reads the azimuth axis position from the ACU.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @return the azimuth axis position in degrees
	*/
	double getAzimuth() throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);
	/** 
	 * This function reads the elevation axis position from the ACU.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @return the elevation axis position in degrees
	*/
	double getElevation() throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);
	
	/**
	 * This function reads the encoders coordinates (azimuth and elevation) and the relative commanded offsets in one shot
	 * @param az azimuth in degrees read directly from the ACU (encoder coordinate)
	 * @param el elevation in degrees read directly from the ACU (encoder coordinate)
	 * @param azOff present offset for the azimuth coordinate. 
	 * @param elOff present offset for the elevation coordinate.
	 * @param time current time the previuos coordinates refer to.
	 * @param section indicates the current section in whihc the antenna is at the time provided
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 */
	void getEncodersCoordinates(double& az,double& el,double& azOff,double& elOff,ACS::Time& time,CACUInterface::TAntennaSection& section
			) throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);

	/** 
	 * This function reads the errors in both mount coordinates in a single operation. A time mark is also provided.
	 * @param azErr error in azimuth
	 * @param elErr error in elevation
	 * @param time timestamp
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 */
	void getAntennaErrors(double& azErr,double&  elErr,ACS::Time& time) throw (AntennaErrors::ConnectionExImpl,
			ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);
	
	/** 
	 * This function computes the rate of the elevation axe.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @return the rate of the azimuth axis position in degrees/sec
	*/
	double getElevationRate() throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);
	/** 
	 * This function computes the rate of the azimuth axe.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @return the rate of the azimuth axis position in degrees/sec
	*/
	double getAzimuthRate() throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);
	/** 
	 * This function reads the elevation position error from the ACU.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @return the elevation position error in degrees
	*/
	double getElevationError() throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);
	/** 
	 * This function reads the azimuth position error from the ACU.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @return the elevation position error in degrees
	*/
	double getAzimuthError() throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);
	/**
	 * This function reads the time currently reported by the ACU.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @return the time as the number of 100 ns ticks 
	 * @sa ACS Time System
	*/
	ACS::TimeInterval getTime() throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);
	
	/**
	 * This function reads the time currently reported by the ACU and returns it as an absolute time mark (year indication included).
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @return the time as the number of 100 ns ticks since 1582-10-15 00:00:00
	 * @sa ACS Time System
	*/
	ACS::Time getAbsTime() throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);

	/**
	 * This function returns the actual mode for the azimuth axe.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource 	 	 
	 * return a value of <i>TAxeModes</i>.
	*/
	CACUInterface::TAxeModes getAzimuthMode() throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);
	/**
	 * This function returns the actual mode for the elevation axe.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * return a value of <i>TAxeModes</i>.
	*/
	CACUInterface::TAxeModes getElevationMode() throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);
	
	/**
	 * This function checks if the antenna is tracking the commanded position.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @return true if the total sky error is less equal CConfiguration::trackingPrecision.
	 * 
	*/
	//bool isTracking() throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);

	/**
	 * This function returns the servo status word of the elevation axis.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @return 16 bits status word
	 * 
	*/
	WORD getElevationServoStatus() throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);
	
	/**
	 * This function returns the servo status word of the azimuth axis.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource 	 
	 * @return 16 bits status word
	 * 
	*/
	WORD getAzimuthServoStatus() throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);
	
	/**
	 * This function returns the servo system status.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @return the 16 bits status word of wich just the first seven are meaningful.
	*/
	WORD getServoSystemStatus() throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);

	/**
	 * This method returns the section where the antenna is positoned at present.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @return the section of the antenna travel range.
	*/
	CACUInterface::TAntennaSection getSection() throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);
	
	/**
	 * Use this function to get the number of free slots available to store time tagged program track positions.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @return the ammount of stack space.
	*/
	WORD getFreeStackPositions() throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);
	
	/**
	 * Use this method to get the present offset for the azimuth coordinate.
	 * @return the offset in degrees.
	*/
	double getAzimuthOffset() const { return m_Data.getAzimuthOffset(); }
	
	/**
	 * Use this method to get the present offset for the elevation coordinate.
	 * @return the offset in degrees.
	*/
	double getElevationOffset() const { return m_Data.getElevationOffset(); }
	
	/**
	 * Use this method to get the commanded position for the azimuth coordinate.
	 * @param ptEnabled this flag informs if the <i>Antenna::ACU_PROGRAMTRACK</i> mode is the current one. 
	 * @return the position in degrees.
	*/	
	double getCommandedAzimuth(bool ptEnabled);
	
	/**
	 * Use this method to get the commanded position for the elevation coordinate.
	 * @param ptEnabled this flag informs if the <i>Antenna::ACU_PROGRAMTRACK</i> mode is the current one. 
	 * @return the position in degrees.
	*/		
	double getCommandedElevation(bool ptEnabled);
	
	/**
	 * Use this method to get a summary about the status of the mount.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @returns @arg \c <i>Antenna::ACU_OK</i> if no error has been detected
	 *          @arg \c <i>Antenna::ACU_ERROR</i> if a minor error was detected
	 *          @arg \c <i>Antenna::ACU_FAILURE</i> if a failure caused the stop of the antenna activities.
	*/
	Management::TSystemStatus getMountStatus() throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl); 
	
	/**
	 * Use this method to set the current position offset. This property is not directly linked to an ACU feature so
	 * it is simulated by this componet so that every position command will be added by this value before the command itself is
	 * passed to the ACU. Range checks are not performed after the addition of the offsets, so it may be tha case in which the Mount goes against
	 * the limits; in that case an alarm is triggered..
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::PropertyError
	 * @throw ComponentErrors::ValidationError
	 * @param azOff new azimuth offset in degrees
	 * @param elOff new elevation offset in degrees
	*/
	void setPositionOffsets(const double& azOff,const double& elOff) throw (ComponentErrors::TimeoutExImpl,AntennaErrors::NakExImpl,AntennaErrors::ConnectionExImpl,
	  ComponentErrors::SocketErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,AntennaErrors::AntennaBusyExImpl,ComponentErrors::PropertyErrorExImpl,ComponentErrors::ValidationErrorExImpl);
	
	/**
	 * Use this method to get the present time offset.
	 * @return the offset in degrees.
	*/
	double getDeltaTime() const { return m_Data.getDeltaTime(); }
	
	/**
	 * Use this method is a fake. In principle it should be used to load a new time offset for the ACU, but since the Medicina ACU does not support
	 * this feature this method does nothing. 
	 * @param delta new value in milliseconds
	*/
	void setDeltaTime(const double& delta) { return m_Data.setDeltaTime(delta); }
	
	/**
	 * This method is used to load into the ACU a new fixed position. Before anything else it performs a range checking,
	 * The sector used to reach the azimuth coordinate is determined automatically. Current 
	 * azimuth and elevation offsets are added to the coordinates before passing to the ACU.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::ValueOutofRange
	 * @throw AntennaErrors::Nak
	 * @throw AntennaErrors::AntennaBusy
	 * @throw ComponentErrors::PropertyError
	 * @throw AntennaErrors::OperationNotPermitted 	 
	 * @param Azim Azimuth position (degrees, 0..360)
	 * @param Elev Elevation position (degrees, minEl..maxEl)
	*/	
	void Preset(double Azim,double Elev) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::ValueOutofRangeExImpl,AntennaErrors::NakExImpl,
	  AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::AntennaBusyExImpl,ComponentErrors::PropertyErrorExImpl,AntennaErrors::OperationNotPermittedExImpl);

	/**
	 * This method is used to load into the ACU new rates provided that the current mode is RATE.  Before anything else it performs a range checking,
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::ValueOutofRange
	 * @throw AntennaErrors::Nak
	 * @throw AntennaErrors::AntennaBusy 	 
	 * @throw ComponentErrors::PropertyError
	 * @throw AntennaErrors::OperationNotPermitted
	 * @param azRate Azimuth rate (from -0.8 to 0.8 degrees per seconds as ruled by the servo system)
	 * @param elRate Elevation rate (from -0.5 to 0.5 degrees per seconds as ruled by the servo system)
	*/	
	void Rate(double azRate,double elRate) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::ValueOutofRangeExImpl,AntennaErrors::NakExImpl,
	  AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::AntennaBusyExImpl,ComponentErrors::PropertyErrorExImpl,AntennaErrors::OperationNotPermittedExImpl);
	  
	/**
	 * This method will pass to the ACU the next data point in oder to fit a tracking curve. Range checkings are performed: azimuth and elevation
	 * are always kept insided the limits configured in the CDB. The azimuth is then expanded in order to exploit the whole azimuth axe travel range as configured in the CDB. 
	 * The current data point is also stored into the local stack; if a time stamp already elapsed arrives it will be neglected and
	 * and exception is thrown. Current azimuth and elevation offsets are added to the resulting coordinate.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::ValueOutofRange
	 * @throw AntennaErrors::Nak
	 * @throw AntennaErrors::AntennaBusy
	 * @throw ComponentErrors::PropertyError
	 * 		  @arg \c ComponentErrors::Timeout
	 * 		  @arg \c AntennaErrors::Connection	 
	 *           @arg \c ComponentErrors::SocketError
	 * @throw AntennaErrors::OperationNotPermitted
	 * @param az Azimuth position
	 * @param el Elevation position
	 * @param time time stamp for the current position
	 * @param clear indicates wether the postiions stack should be cleared or not.
	*/
	void programTrack(double az,double el,TIMEVALUE& time,bool clear) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::ValueOutofRangeExImpl,
	  AntennaErrors::NakExImpl,AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::AntennaBusyExImpl,
	  AntennaErrors::OperationNotPermittedExImpl,ComponentErrors::PropertyErrorExImpl);
	  
	/**
	 * This function is used to load a time into the ACU. Since the ACU synchronizes to the next PPS time pulse the command will be issued only during the first 500000 microseconds
	 * of the current second. 	
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @throw AntennaErrors::Nak 
	 * @param now time to be loaded, after the operation this value could have been changed.
	*/
	void timeTransfer(TIMEVALUE& now) throw (AntennaErrors::ConnectionExImpl,AntennaErrors::NakExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);
	  
	/**
	 * This function is used to command new axes mode to the ACU. It also resets the offsets.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @throw AntennaErrors::Nak
	 * @throw AntennaErrors::AntennaBusy 	   
	 * @param azMode This parameter selects the mode for azimuth axis
	 * @param elMode This parameter selects the mode for elevation axis	
	 * @param force This flag (default is false) is used to force the  procedure to go ahead even if the antenna is busy. That means that the 
	 *        AntennaBusy excpetion is never thrown. This flag is used when stopping the telescope.
	*/
	void Mode(CACUInterface::TAxeModes azMode,CACUInterface::TAxeModes elMode,bool force=false) throw (ComponentErrors::TimeoutExImpl,
	  AntennaErrors::NakExImpl,AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::AntennaBusyExImpl);
	
	/**
	 * This function immediately stops all mount activities.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @throw AntennaErrors::Nak
	 * @throw AntennaErrors::AntennaBusy 	     
	*/
	void Stop() throw (ComponentErrors::TimeoutExImpl,AntennaErrors::NakExImpl,AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,
	  AntennaErrors::AntennaBusyExImpl);
	  
	/**
	 * This method is used to clear pending hardware failures.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @throw AntennaErrors::Nak
	*/
	void failureReset() throw (ComponentErrors::TimeoutExImpl,AntennaErrors::NakExImpl,AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl);
	
	/**
	 * Computes the real hardware coordinate for the azimuth axis
	 * @param destination the azimuth destination in the range 0..360.
	 * @return the real azimuth coordinate that correspont to the input destination (in degrees)
	 */
	double getHWAzimuth(double destination,const CACUInterface::TAntennaSection& commandedSection);
	
	/**
	 * This method has been added to detect an hardware problem experienced at Medicina. The telescope begin to oscillate in azimuth.
	 * The detection strategy, in order to avoid false detection due to direction changes during normal observing, is:
	 * 1) if the tracking error is beyond a threashold an alarm condition is triggered.  This condition has a certain validity time.
	 * 2) If more than a number of direction changes (of the tracking error with respect to the zero error) are detected during the alarm time window, the oscillation is declared-
	 * 3) The alarm condition is cleared if the time validity window of the alarm elapsed and the tracking error is again under the threshold.
	 * In case of detection counter mesures are immediately taken.
	 */
	void detectOscillation() throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,AntennaErrors::NakExImpl,AntennaErrors::AntennaBusyExImpl);
	
	/** 
	 * This member function is used by the control thread in order to check if a long job that was started as completed or not.
	 * @param job job identifier
	 * @param comp completion of the job
	 * @return true if the job has completed, somehow succesfully or not.
	*/	
	bool updateLongJobs(WORD job,ACSErr::Completion_out comp);
	
	/**
	 * This member function is used by the control Thread in order to keep data updated.
     * It simply asks for some properties.  
	*/
	void updateComponent();

	/**
	 * This function is used to convert from the TAxesModes defined in the ACUInterface to the TModes defined in the Antenna
	 * idl module. 
	 * @param mode ACUInterface mode.
	 * @return the corresponding TCommonModes defined in the Antenna module.
	*/
	static Antenna::TCommonModes acuModes2IDL(const CACUInterface::TAxeModes& mode);
	
	/**
	 * This function is used to convert from the TModes defined in the Antenna idl module to the TAxesModes 
	 * defined in the ACUInterface. 
	 * @param mode TCommonMode mode.
	 * @return the corresponding TAxeModes defined in the ACUInterface.
	*/	
	static CACUInterface::TAxeModes idlModes2ACU(const Antenna::TCommonModes& mode);
	
	/**
	 * This function is used to convert from the TAntennaSection defined in the ACUInterface to the TSections defined in the Antenna
	 * idl module. 
	 * @param section ACUInterface section.
	 * @return the corresponding TSections defined in the Antenna idl module.
	*/
	static Antenna::TSections acuSection2IDL(const CACUInterface::TAntennaSection& section);
	
	/**
	 * This function is used to convert from the TSections defined in the Antenna idl module into TAntennaSection defined in  the ACUInterface
	 * class   
	 * @param section TSections defined in the Antenna idl module
	 * @return the corresponding TAntennaSection
	*/	
	static CACUInterface::TAntennaSection IDLSection2Acu(const Antenna::TSections& section);
	
	/** 
	 * Gets the antenna status 
	 * @return the present antenna status
	*/
	Antenna::TStatus getStatus() const;
	
	/** 
	 * This function is used to check of the antenna is busy with some stuff.
	 * @return true if the antenna is not ready to accept new commands.
	*/
	bool isBusy();
	
	/** 
	 * Sets the antenna status, if the antenna status is set to Antenna::ACU_BSY then the m_bBusy flag is also set,
	 * this flag remains on until the status is set MEDACU::ACU_CNTD esplicitally.
	 * @param sta the new antenna status.
	*/
	void setStatus(Antenna::TStatus sta);
	
	/**
	 * Check if a stopped command was received 
	 * @return true if a stop command has been issued
	*/
	inline bool isStopped() { return m_bStopped; }
	
	/**
	 * Sets and reset the stop command flag.
	 * @param val true turn on the stop command flag.
 	*/
	inline void setStopped(bool val) { m_bStopped=val; }
	
	/**
	 * Sets the current command Section
	 */
	inline void setCommandSection(const Antenna::TSections& section) { m_commandSection=IDLSection2Acu(section); }
	
	/**
	 * This method is used by the component watchdog in order to force an update of the ACU status.
	 * @throw ComponentErrors::Timeout
	 * @throw AntennaErrors::Connection	 
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	*/
	void forceUpdate() throw (AntennaErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);
	
	void printBuffer(BYTE *Buff,WORD Len);
protected:
	/**
	 * Automatically called by the framework as a result of a connection request. See super-class for more details.
	 * @param  ErrorCode connection operation result, zero means operation is succesful.
	*/
	void onConnect(int ErrorCode);
	/**
	 * Automatically called by the framework if the connection doesn't happens before the allotted time expires. See super-class for more details.
	 * @param  EventMask event that timeouted.
	*/
	void onTimeout(WORD EventMask);
private:
	/** ACU data structure */
	CACUData m_Data;
	/** Connection status */
	Antenna::TStatus m_Status;
	/** */
	bool m_bTimedout;
	/** This flag is true if the antenna is busy (turned on when the status is BSY, turned off when the status is CNTD again)*/
	bool m_bBusy;
	/** It contains error information */	
	CError m_Error;
	/** Component configuration data */
	CConfiguration *m_configuration;
	/** It indicates if a stop command has been issued */
	bool m_bStopped;
	/** This is the container of the positions commanded into the ACU by the programTRack mode */
	CTimeTaggedCircularArray *m_trackStack;
	/** This member stores the section that must be used to reach the next commanded position in azimuth */
	CACUInterface::TAntennaSection m_commandSection;
	// memeber user to manage the oscillation problem
	/**
	 * direction of the tracking error with respect to the zero error
	 */
	int m_oscDirection;
	/**
	 * the alarm condition is triggered
	 */
	bool m_oscAlarm;
	/**
	 * number of tracking error direction changes during the alarm window
	 */
	WORD m_oscNumber;
	/**
	 * The oscillation has been detected
	 */
	bool m_oscStop;
	/**
	 * Epoch of the beggining of the alarm condition
	 */
	ACS::Time m_oscTime;
	/**
	 * Epoch when the antenna was stopped
	 */
	ACS::Time m_oscStopTime;
	/**
	 * last commanded mode before oscillation detection;
	 */
	CACUInterface::TAxeModes m_oscMode;
	
	/**
	 * Stores the epoch of the last scan. Used in oscillation prevention
	 */
	ACS::Time m_lastScanEpoch;
	
	/**
    * This member function is called to send a buffer to the ACU.
	 * @param Msg ponter to the byte buffer that contains the message to the ACU
	 * @param Len length of the buffer
	 * @return SUCCESS if the buffer was sent correctly, WOULDBLOCK if the send would block, FAIL and the <i>m_Error</i> member is set accordingly.
	*/
	OperationResult sendBuffer(BYTE *Msg,WORD Len);
	/**
    * This member function is called to receive a buffer from the ACU. if it realizes that the remote side disconnected it sets the member <i>m_Status</i> 
	 * to NOTCNTD.
	 * @param Msg ponter to the byte buffer that will contain the message from the ACU
	 * @param Len length of the buffer, that means the exact number of bytes that can be read
	 * @return the number of bytes read, 0 if the connection fell down, FAIL in case of error and the <i>m_Error</i> member is set accordingly, 
	 *         WOULDBLOCK if the timeout expires
	*/
	int receiveBuffer(BYTE *Msg,WORD Len);
	/**
	 * This function is used to keep update the monitor data buffer. At every request it checks if the last update is enough recent (update time not expired), 
	 * if not it takes charge of collecting a new buffer. 
	 * @param updateTime time, in microseconds that is used to determine wether a new buffer must be collected or not
	 * @return a number greater than zero if the buffer were collected correctly, else -FAIL in case of error, WOULDBLOCK if the timeout expires, 0 if the remoteside
	 *         disconnected.
	*/
	int loadMonitorData(DWORD updateTime);
	/**
	 * This private member function is used to issue an ACU command. It waits for the answer.
	 * @param Buff pointer to the buffer that contains the message to send to the ACU, on exit it will cointains the ACU aswer
	 * @param Len lenght of the input buffer.
	 * @return a positive number if the command was correctly received by the ACU, else -1 in case of error, -2 if the timeout expires
	*/
	int launchCommand(BYTE *Buff,WORD Len);
	/**
	 * This function checks if the ACU link is still enstablished, if so it returns true, else it fixes this problem by trying to reconnect
	 * using asynchronous methods (this avoid the component to freeze while reconnecting). If the operation succeeds the <i>onConnect()</i>
	 * method is automatically called.
	 * @return true if the communication is OK, false if the comunication is not enstablished or is enstablishing.
	*/
	bool checkConnection();

};

#endif
