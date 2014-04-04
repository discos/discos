#ifndef COMMONDATA_H_
#define COMMONDATA_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                               */
/* $Id: CommonData.h,v 1.9 2011-06-03 18:02:49 a.orlati Exp $																								            */
/*                                                                                                                    */
/* This code is under GNU General Public Licence (GPL).                        */
/*                                                                                                      				*/
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  30/09/2009      Creation                 */

#include <AntennaDefinitionsS.h>
#include <ManagmentDefinitionsS.h>
#include <TimeTaggedCircularArray.h>
#include "ACUProtocol.h"

using namespace IRA;

class CCommonData {
public:
	enum TStatusWordFields {
		STATUS_MESSAGE_SYNC_ERROR=0,             /*!< set if the status message could not be synchronized: Error*/
		STATUS_LINE_ERROR=1,                      /*!< set if there was an error in the status socket: Error  */
		CONTROL_LINE_ERROR=2,					  /*!< set if an error on the control line was detected: Error */
		REMOTE_CONTROL_DISABLED=3,				  /*!< set if the control software is not allowd to control the telescope: Error */
		EMERGENCY_STOP=4,						  /*!< set if an emergency stop has been toggled: Error */
		MAIN_POWER_ERROR=5,						  /*!< set in casa of failure of to the power supply: Error */
		TIME_ERROR=6,							  /*!< set if the the ACU is not synchornized: Error */
		PROGRAM_TRACK_DATA_ERROR=7,				  /*!< set if the ACU reports an error in program track data: Warning */
		REMOTE_COMMAND_ERROR=8					  /*!< set id the ACU did not accept a command, for any reason: Warning */
	};
	
	enum TAxisStatusWordFields {
		AXIS_READY=0,							/*!< set if the axis is ready to be activated: Status*/
		ACTIVE=1,								/*!< set if the axis id active: Status */
		LOW_POWER_MODE=2,						/*!< set if the axis is working in low power mode: Warning*/
		STOWED=3,								/*!< set if the axis is stowed: Status*/
		STOW_PIN_EXTRACTED=4,					/*!< set if the parking pin is extracted: Status*/
		PRELIMIT_UP=5,                          /*!< set if the axis went over the upper prelimit switch : Warning */
		PRELIMIT_DOWN=6,					    /*!< set if the axis went over the lower prelimit switch : Warning */
		FINAL_LIMIT_UP=7,                       /*!< set if the axis went over the upper final limit switch : Error */
		FINAL_LIMIT_DOWN=8,						/*!< set if the axis went over the lower final limit switch : Error */
		RATE_LIMIT=9,                           /*!< set if the axis overspeeds : Warning*/
		ENCODER_FAILURE=10,						/*!< set if the axis encoder failed : Error */
		BRAKE_ERROR=11,                         /*!< set if one or more barkes failed: Error */
		SERVO_ERROR=12                          /*!< set if the servo system failed: Error */
	};
	
	enum TMotorStatusWordFields {
		MOTOR_SELECTED=0,						/*!< set if the motor is selected: Status  */
		MOTOR_BRAKE_OPEN=1,                     /*!< set if motor brake is open: Status */
		POWER_MODULE_ERROR=2,					/*!< set if the motor power module failed: Error */
		MOTOR_ACTIVE=3,							/*!< set if the motor is ready and active: Status */
		MOTOR_SERVO_ERROR=4,					/*!< set if the servo failed: Error */
		SENSOR_ERROR=5,                         /*!< set if the motor sensors failed: Error */
		BUS_ERROR=6,                            /*!< set if the bus failed: Error */
	};
		
	/**
	 * Constructor.
	 */
	CCommonData();
	/**
	 * Destructor
	 */
	virtual ~CCommonData();
	
	/**
	 * Sets the state of the status socket from the ACU.
	 * @param s new status
	 */
	void setStatusLineState(const Antenna::TStatus& s);
	
	/**
	 * Gets the state of the status socket from the ACU.
	 * @return the current state of the status socket 
	 */
	inline const Antenna::TStatus& getStatusLineState() const { return m_statusSocketState; }

	/**
	 * Sets the state of the control socket to the ACU.
	 * @param s new status
	 */
	void setControlLineState(const Antenna::TStatus& s);
	
	/**
	 * Gets the state of the control socket to the ACU.
	 * @return the current state of the control socket 
	 */
	const Antenna::TStatus getControlLineState() const;
	
	/**
	 * It allows the clear the busy state for the control line. The normal control line state is not affected
	 */
	inline void clearControlLineBusyState() { m_bBusy=false; }
	
	/**
	 * Gets the busy status of the control line. 
	 * @return the current flag of the inner member <i>m_bBusy</i>.
	 */
	inline bool isBusy() const { return m_bBusy; }
	
	/**
	 * @return the reference of the buffer in which the status message from the ACU can be stored.
	 */
	BYTE * const & getStatusBuffer() { return m_statusBuffer; }
	
	void reBind(); 
	
	/**
	 * called to inform the common data area about the last command sent to the ACU, the buffer is stored and used to
	 * match the answer of the ACU through the status line
	 * @param command sequence of commands
	 * @param size dimension of the first argument
	 */
	void storeLastCommand(const CACUProtocol::TCommand* command,const WORD& size);
	
	/**
	 * called to check if the last command sent to the acu was completed or not.
	 * @param answer answer code to the last command, meaningful only in case of error
	 * @param bool set if the aCU responded with an error code to the last command message
	 * @return true if the ACU sent a complete answer to the last command
	 */
	bool checkLastCommand(CACUProtocol::TCommandAnswers& answer,bool& error);
	
	/**
	 * I allows to store the last commanded mode for both axis.
	 * @param azMode mode commanded for the azimuth
	 * @param elMode mode commanded for the elevation
	 */
	void setCommandedMode(const Antenna::TCommonModes& azMode,const Antenna::TCommonModes& elMode) { m_commandedAzimuthMode=azMode; m_commandedElevationMode=elMode; }
	
	/**
	 * It allows to known the last commanded mode for both axis.
	 * @param azMode currently commanded mode for the azimuth
	 * @param elMode currently commanded mode for the elevation
	 */	
	void getCommandedMode(Antenna::TCommonModes& azMode,Antenna::TCommonModes& elMode) const { azMode=m_commandedAzimuthMode; elMode=m_commandedElevationMode; }
	
	/**
	 * It stores the last commanded position offsets
	 * @param azOff new azimuth offset
	 * @param elOff new elevation offset
	 */
	void setCommandedOffsets(const double&azOff,const double& elOff) { m_commandedAzOff=azOff; m_commandedElOff=elOff; }
	
	/**
	 * It reports on the last commanded position offsets
	 */
	void getCommandedOffsets(double& azOff,double& elOff) const { azOff=m_commandedAzOff; elOff=m_commandedElOff; }
	
	/**
	 * used to read the the position errors (both axis) in one shot, the time the errors refers to is also returned.
	 */
	void getAntennaErrors(double& azErr,double& elErr,ACS::Time& time);
	
	/**
	 * It allows to know the present mode for each axis.
	 * @param azMode currently mode for the azimuth
	 * @param elMode currently mode for the elevation
	 */
	void getActualMode(Antenna::TCommonModes& azMode,Antenna::TCommonModes& elMode) const;
	
	/**
	 * It allows to get back the positions of each motor of the telescope
	 */
	void getMotorsPosition(ACS::doubleSeq& positions);

	/**
	 * It allows to get back the speed of each motor of the telescope
	 */
	void getMotorsSpeed(ACS::doubleSeq& speeds);

	/**
	 * It allows to get back the torque of each motor of the telescope
	 */
	void getMotorsTorque(ACS::doubleSeq& torques);

	/**
	 * It allows to get back the percentage of utilization of each motor of the telescope
	 */
	void getMotorsUtilization(ACS::doubleSeq& utilization);

	/**
	 * It allows to get back the status word of each motor of the telescope
	 */
	void getMotorsStatus(ACS::longSeq& status);
	
	/**
	 * It allows to clear the stack of currently commanded positions
	 */
	void clearProgramTrackStack() { m_trackStack.empty(); }
	
	/**
	 * @return true if the program track stack is empty, false if not
	 */
	bool isProgramTrackStackEmpty() const { return m_trackStack.isEmpty(); }
		
	/**
	 * Add a point to the program track stack.
	 * @param az azimuth coordinate of the point to be added
	 * @param el elevation coordinate of the point to be added
	 * @param time reference time relative to the point
	 */
	void addProgramTrackStackPoint(const double& az,const double& el,const ACS::Time& time);
	
	/**
	 * It allows to get the last point added to the program track stack
	 * @param az azimuth coordinate
	 * @param el elevation coordinate
	 */
	void getLastProgramTrackPoint(double& az,double& el) const;
	
	/**
	 * It allows to check if the telescope is stowed or not.
	 * @return true if the telescope is stowed
	 */
	bool checkIsStowed() const;
	
	/**
	 * This method computes the real azimuth that correspond to a destination (azimuth in degrees) and a sector of the azimuth range. For example 355° can be mapped into 355° but also into
	 * -5° depending on the current position and sector.
	 * @param destination destination azimuth coordinate
	 * @param commandedSection indicates from which sector we want to reach the destination, if the destination azimuth is not available for this sector, this parameter is ignored.
	 * @param azLowerLimit the lower limit of azimuth 
	 * @param azUpperLimit the upper limit of azimuth
	 * @param azCwLimit the azimuth value that separates the CW from CCW sectors 
	 * @return the destination azimuth in degrees
	 */ 
	double getHWAzimuth(double destination,const Antenna::TSections& commandedSection,double azLowerLimit,double azUpperLimit,double azCwLimit);
	
	/**
	 * This allows to get the encoders coordinates from the mount and the respective offsets. For Azimuth the section is also given.
	 * The limit of the azimuth correspond to the physical limits of the telescope, the offset are eventually included.
	 */
	void getEncodersCoordinates(double& az,double& el,double& azOff,double& elOff,ACS::Time& time,Antenna::TSections& section);
	
	/**
	 * It allows to check if the telescope is unstowed or not.
	 * @return true if the telescope is unstowed
	 */	
	bool checkIsUnstowed() const;
	
	/**
	 * This function will set bit of the status word corresponding to the given field 
	 */
	inline void setStatusWord(TStatusWordFields field) { m_statusWord |= 1 << field; }

	/**
	 * This function will unset (cleared) the bit status of the status word corresponding to the given field 
	 */
	inline void clearStatusWord(TStatusWordFields field) { m_statusWord &= ~(1 << field); }
	
	/**
	 * Set the current commanded azimuth sector
	 */
	inline void setCommandedSector(const Antenna::TSections& sect) { m_commandedSection=sect; }
	
	/**
	 * Get the current commanded antenna section
	 */
	inline const Antenna::TSections& getCommandedSector() const { return m_commandedSection; }
	
	/** 
	 * @return the constant reference to pointer of the azimuth status frame.
	 */ 
	inline CACUProtocol::TAxisStatus * const & azimuthStatus() const { return m_azimuthStatus; }
	
	/** 
	 * @return the constant reference to pointer of the elevation status frame.
	 */ 
	inline CACUProtocol::TAxisStatus * const & elevationStatus() const { return m_elevationStatus; }
	
	/**
	 * @return the constant reference to the pointer of the pointing status frame.
	 */
	inline CACUProtocol::TPointingStatus * const & pointingStatus() const { return m_pointingStatus; }
	
	/**
	 * @return the constant reference to the pointer of the cable wrap status frame.
	 */
	inline CACUProtocol::TAxisStatus * const & cableWrapStatus() const { return m_cableWrap; }

	/**
	 * @return the constant reference to the pointer of the general status frame.
	 */
	inline CACUProtocol::TGeneralStatus * const & generalStatus() const { return m_generalStatus; }
	
	inline const DWORD& getStatusWord() const { return m_statusWord; }
	inline const DWORD& getAzimuthStatusWord() const { return m_azimuthStatusWord; }
	inline const DWORD& getElevationStatusWord() const { return m_elevationStatusWord; }
	 
	inline const Management::TSystemStatus& getMountStatus() const { return m_mountStatus; }
		
private:
	/**
	 * Pointer to the current status buffer 
	 **/
	BYTE *m_statusBuffer;
	
	/** 
	 * list of all commanded position to the Antenna
	 */
	IRA::CTimeTaggedCircularArray m_trackStack;
	
	
	//
	Antenna::TSections m_commandedSection;
	
	/**
	 * sockets status 
	 */
	Antenna::TStatus m_statusSocketState;
	Antenna::TStatus m_controlSocketState;
	
	/**
	 * Mount status
	*/
	Management::TSystemStatus m_mountStatus;
	
	Antenna::TCommonModes m_commandedAzimuthMode;
	Antenna::TCommonModes m_commandedElevationMode;
	double m_commandedAzOff;
	double m_commandedElOff;
	
	/**
	 * 32 bits status words
	 */
	DWORD m_statusWord;
	DWORD m_azimuthStatusWord;
	DWORD m_elevationStatusWord;
	DWORD m_motorsStatusWord[CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS+1]; // we consider also the cable wrap motor
	
	/** This flag is true if the antenna is busy */
	bool m_bBusy;
		
	CACUProtocol::TGeneralStatus * m_generalStatus;
	CACUProtocol::TAxisStatus *m_azimuthStatus;
	CACUProtocol::TAxisStatus *m_elevationStatus;
	CACUProtocol::TAxisStatus *m_cableWrap;
	CACUProtocol::TPointingStatus *m_pointingStatus;
	CACUProtocol::TMotorStatus *m_azimuthMotors[CACUProtocol::AZIMUTH_MOTORS];
	CACUProtocol::TMotorStatus *m_elevationMotors[CACUProtocol::ELEVATION_MOTORS];
	CACUProtocol::TMotorStatus *m_cableWrapMotor;
	CACUProtocol::TCommand m_lastCommand[10];
	WORD m_lastCommandSize;
	
	/**
	 * This function will set or clear a bit of the azimuth status word corresponding to the given field 
	 */	
	void setAzimuthStatusWord(TAxisStatusWordFields field,bool val) { if (val) m_azimuthStatusWord |= 1 << field; else m_azimuthStatusWord &= ~(1 << field); }

	/**
	 * This function will set or clear a bit of the elevation status word corresponding to the given field 
	 */	
	void setElevationStatusWord(TAxisStatusWordFields field,bool val) { if (val) m_elevationStatusWord |= 1 << field; else m_elevationStatusWord &= ~(1 << field); }

	/**
	 * This function will set or clear bit of the motor status word corresponding to the given field. No range checks done.
	 */	
	void setMotorsStatusWord(WORD motor,TMotorStatusWordFields field,bool val) { if (val) m_motorsStatusWord[motor] |= 1 << field; else m_motorsStatusWord[motor] &= ~(1 << field); }
	
	/**
	 * Allows to set the overall status of the component. Three possible states: OK, Warning or failure. If the current status is failure, it cannot be changed in WARNINIG.
	 */
	void setMountStatus(const Management::TSystemStatus& status) { if (status>m_mountStatus) m_mountStatus=status;  }
		
};

#endif /*COMMONDATA_H_*/
