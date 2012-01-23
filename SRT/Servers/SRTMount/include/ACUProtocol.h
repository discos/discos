#ifndef ACUPROT_H
#define ACUPROT_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                 when            What                                             */
/* Andrea Orlati(aorlati@ira.inaf.it)  28/09/2009      Creation                                         */

#include <AntennaDefinitionsS.h>
#include <IRA>
#include <DateTime.h>
#include <bitset>

#define TUINT32     unsigned int
#define TUINT16     unsigned short int
#define TREAL32     float
#define TREAL64     double
#define TWORD       TUINT16
#define TDWORD		TUINT32
#define TINT32      int
#define TINT16      short int
#define TINT8		char
#define TUINT8	    unsigned char 
#define TBOOL       TINT8

namespace Protocol_Internal {

class CCircularArray {
public:
	/**
	 * Constructor.
	 * @param dim dimension of the array in bytes.
	 */
	CCircularArray(const DWORD& dim);
	/**
	 * Destructor
	 */
	virtual ~CCircularArray();
	
	/**
	 * Add a buffer to the current circular array
	 * @param buffer pointer to the buffer 
	 * @param len lenght in bytes of the incoming buffer
	 */
	void add(BYTE *buffer,const DWORD& len);
	
	/**
	 * Copys a specified number of bytes from the circular array to a destination buffer.
	 * @param outBuffer pointer to the destination buffer, it must be large enough to contain all the required bytes
	 * @param index starting byte from which to copy. It can be from 0 to <i>getLength()</i>-1.
	 * @param len number of bytes to be copied. 
	 */
	void get(BYTE *outBuffer,const DWORD& index,const DWORD& len) const;

	/**
	 * Copys a specified number of bytes from the beggining of the  circular array to a destination buffer. The copied data will be removed from the current circular array.
	 * @param outBuffer pointer to the destination buffer, it must be large enough to contain all the required bytes 
	 * @param len number of bytes to be copied. 
	 */
	void pop(BYTE *outBuffer,const DWORD& len);
	
	/**
	 * It clears form the array the specified number of bytes from the beggining.
	 * @param len number of bytes to erase. If len is greater equal to the number of elements in the array it does the same thing of <i>reset()</i>.
	 */
	void erase(const DWORD& len);
	
	/**
	 * It resets and emptys the array. 
	 */
	void reset();
	
	/**
	 * @return the number of bytes actually stored in the buffer 
	 */
	DWORD getLenght() const;

	
private:
	DWORD m_dimension;
	/**
	 * Points to the first element of the array.
	 */ 
	DWORD m_head;
	/**
	 * Points to the last element of the array.
	 */ 
	DWORD m_tail;
	/**
	 * Number of bytes stored in the array
	 */
	DWORD m_lenght;
	/**
	 * Circular array
	*/
	BYTE * m_buffer;
	
	/**
	 * It copies a linear array into the current circular array
	 * @param linear pointer to the source linear buffer
	 * @param len dimension of the source buffer
	 */
	void linearToCircular(BYTE *linear,const DWORD& len);
	
	/**
	 * It copies a  portion of the current circular array into a linear array.
	 * @param linear pointer to the destination linear array
	 * @pram start index of the first byte to be copied
	 * @param len number of byte to copy
	 */
	void circularToLinear(BYTE *linear,const DWORD& start,const DWORD& len) const;
};

};

/**
 * This class implements the interface toward the ACU. It supplies some methods that allow to comunicate to the 
 * ACU using the right protocol. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
 */
class CACUProtocol  {
public:
	
	/** This is enlist all possible operating modes of the ACU. */
	enum TModes {
		MODE_IGNORE=0,						   /*!< The ignore mode */
		MODE_INACTIVE=1,                         /*!< The axis will be deactivated, control loop is open, brakes closed and servo amplifiers are disabled */
		MODE_ACTIVE=2,			                   /*!< The axis will be activated, the control loop is closed, brakes opened and servo amplifiers enabled */
		MODE_PRESET_ABS=3,                    /*!< This mode allows to drive to an absolute fixed position. */
		MODE_PRESET_REL=4,                    /*!<  Same as PRESET_ABS but the commanded position will be added to the current one */
		MODE_RATE=5,				                  /*!< It allows to drive the telescope with a flexible velocity. The movement is countinuos until a STOP is issued or the limit switch is reached */
		MODE_POSITIONTRACK=6,           /*!< TBD */
		MODE_STOP=7,				             /*!< Stops the movement of the telescope */
		MODE_PROGRAMTRACK=8,          /*!<  It moves the telescope to the first position of the program track table */
		MODE_RESET=15,                          /*!<  It resets all pending errors */
		MODE_STOW=50,							 /*!< Engage the stow pins, the axis must be in stow position */
		MODE_UNSTOW=51,                      /*!< Disengage the stow pins */
		MODE_DRIVETOSTOW=52             /*!< The telescope is driven to the prefixed stow position. The stow pin is engaged automatically */
	};
	
	enum TTimeSources {
		TS_ACUTIME=1,
		TS_IRIGB=2,
		TS_EXTERNAL=3
	};

	enum TParameters {
		PAR_IGNORE=0,
		PAR_ABS_POS_OFFSET=11,
		PAR_REL_POS_OFFSET=12,
		PAR_TIME_SOURCE=50,
		PAR_TIME_OFFSET=51,
		PAR_PROGRAM_TRACK_TIME_CORRECTION=60,
		PAR_PROGRAM_TRACK_TABLE=61
	};
	
	/**
	 * Enlist all possible subsystem identifier
	 */ 
	enum TSubsystems {
		SUBSYSTEM_ID_AZIMUTH=1,
		SUBSYSTEM_ID_ELEVATION=2,
		SUBSYSTEM_ID_TRACKING=4,
		SUBSYSTEM_ID_POINTING=5
	};
	
	enum TCommandAnswers {
		NO_COMMAND=0,
		COMMAND_EXECUTED=1,
		COMMAND_ACTIVE=2,
		COMMAND_EXECUTION_ERROR=3,
		COMMAND_IN_WRONG_MODE=4,
		COMMAND_INVALID_PARAMETERS=5,
		COMMAND_ACCEPTED=9
	};
	
	enum TAxisState {
		STATE_INACTIVE=0,
		STATE_DEACTIVATING=1,
		STATE_ACTIVATING=2,
		STATE_ACTIVE=3
	};
	
	enum TConstants {
		ELEVATION_MOTORS=4,					  // number of elevation motors	
		AZIMUTH_MOTORS=8,					  // number of azimuth motors
		MESSAGE_FRAME_START_BYTE=12,          // this is the start byte, inside the status message frame, of the effective data
		PROGRAMTRACK_TABLE_MINIMUM_LENGTH=5,  // the minimum number of points that have to loaded into the program track table
		SOCKET_SEND_BUFFER=512,			      // size in bytes if the sending buffer							
		SOCKET_RECV_BUFFER=1000 ,              // size in bytes of the receiving buffer
		PROGRAMTRACK_STACK_POSITIONS=2500			  // number of free position of the stack of the program track mode
	};
	
	typedef struct {
		double azimuth;
		double elevation;
		ACS::Time timeMark;
	} TProgramTrackPoint;

	class THardwareInterlock {
	public:
		THardwareInterlock(TDWORD statusWord): m_statusWord((unsigned long)statusWord) {}
		THardwareInterlock(const THardwareInterlock& src): m_statusWord(src.m_statusWord.to_ulong()) { }
		int bit(const long& pos) { return m_statusWord.test(pos)?1:0; }
	private:
		std::bitset<sizeof(TDWORD)*8> m_statusWord;
		const THardwareInterlock& operator =(const THardwareInterlock& src);
	};
	
	class TSoftwareInterlock {
	public:
		TSoftwareInterlock(TDWORD statusWord): m_statusWord((unsigned long)statusWord) {}
		TSoftwareInterlock(const TSoftwareInterlock& src): m_statusWord(src.m_statusWord.to_ulong()) { }
		int bit(const long& pos) { return m_statusWord.test(pos)?1:0; }
		inline bool emergencyStop() {return m_statusWord.test(13); }
		inline bool mainPower() { return m_statusWord.test(20); }
	private:
		std::bitset<sizeof(TDWORD)*8> m_statusWord;
		const TSoftwareInterlock& operator =(const TSoftwareInterlock& src);
	};
	
	class TErrorsTracking {
	public:
		TErrorsTracking(TWORD statusWord): m_statusWord((unsigned long)statusWord) {}
		TErrorsTracking(const TErrorsTracking& src): m_statusWord(src.m_statusWord.to_ulong()) { }
		int bit(const long& pos) { return m_statusWord.test(pos)?1:0; }
		inline bool dataOverflow() {return m_statusWord.test(0); }
		inline bool timeDistanceFault() { return m_statusWord.test(1); }
		inline bool noDataAvailable() { return m_statusWord.test(2); }
	private:
		std::bitset<sizeof(TWORD)*8> m_statusWord;
		const TErrorsTracking& operator =(const TErrorsTracking& src);
	};	
	
	class TWarningMainAxis {
	public:
		TWarningMainAxis(TDWORD statusWord): m_statusWord((unsigned long)statusWord) {}
		TWarningMainAxis(const TWarningMainAxis& src): m_statusWord(src.m_statusWord.to_ulong()) { }
		int bit(const long& pos) { return m_statusWord.test(pos)?1:0; }
		inline bool preLimitUp() {return m_statusWord.test(19); }
		inline bool preLimitDown() {return m_statusWord.test(20); }
		inline bool finLimitUp() {return m_statusWord.test(21); }
		inline bool finLimitDown() {return m_statusWord.test(22); }
		inline bool rateLimit() {return m_statusWord.test(23); }
		inline bool stowPinExtracted() { return !m_statusWord.test(25); }
	private:
		std::bitset<sizeof(TDWORD)*8> m_statusWord;
		const TWarningMainAxis& operator =(const TWarningMainAxis& src);
	};	

	class TErrorMainAxis {
	public:
		TErrorMainAxis(TDWORD statusWord): m_statusWord((unsigned long)statusWord) {}
		TErrorMainAxis(const TErrorMainAxis& src): m_statusWord(src.m_statusWord.to_ulong()) { }
		int bit(const long& pos) { return m_statusWord.test(pos)?1:0; }
		inline bool brakeError() {return m_statusWord.test(6); }
		inline bool servoError() {return m_statusWord.test(8); }
		inline bool encoderFailure() {return m_statusWord.test(13); }
	private:
		std::bitset<sizeof(TDWORD)*8> m_statusWord;
		const TErrorMainAxis& operator =(const TErrorMainAxis& src);
	};	
	
	class TGeneralStatus {
	public: 
		TGeneralStatus(BYTE *buffer,DWORD disp): m_buffer(buffer),m_disp(disp) {}
		virtual ~TGeneralStatus() {}
		inline TUINT16 version() const { return CACUProtocol::readStatusField<TUINT16>(m_buffer,m_disp+0); }
		inline TUINT8 master() const { return CACUProtocol::readStatusField<TUINT8>(m_buffer,m_disp+2); }
		inline TUINT16 statusHMI() const { return CACUProtocol::readStatusField<TUINT16>(m_buffer,m_disp+3); }
		inline TBOOL softwareIO() const { return CACUProtocol::readStatusField<TBOOL>(m_buffer,m_disp+5); }
		inline TBOOL simulation() const { return CACUProtocol::readStatusField<TBOOL>(m_buffer,m_disp+6); }
		inline TBOOL control_system() const { return CACUProtocol::readStatusField<TBOOL>(m_buffer,m_disp+7); }
		inline TBOOL service() const { return CACUProtocol::readStatusField<TBOOL>(m_buffer,m_disp+8); }
		inline THardwareInterlock hardware_interlock() const { return THardwareInterlock((TDWORD)CACUProtocol::readStatusField<TDWORD>(m_buffer,m_disp+9)); }
		inline TSoftwareInterlock software_interlock() const { return TSoftwareInterlock((TDWORD)CACUProtocol::readStatusField<TDWORD>(m_buffer,m_disp+13)); }
		inline TREAL64 functionGeneratorSignal() const { return CACUProtocol::readStatusField<TREAL64>(m_buffer,m_disp+17); }
		enum TConstant {
			SIZE=25
		};
	private:
		BYTE *m_buffer;
		DWORD m_disp;
	};
	
	class TModeCommandStatus {
	public:
		TModeCommandStatus(BYTE *buffer,DWORD disp): m_buffer(buffer),m_disp(disp) {}
		virtual ~TModeCommandStatus() {}
		inline TUINT32 recvModeCommandCounter() const { return CACUProtocol::readStatusField<TUINT32>(m_buffer,m_disp+0); }
		inline TModes recvModeCommand() const { return (TModes)CACUProtocol::readStatusField<TUINT16>(m_buffer,m_disp+4); }
		inline TCommandAnswers recvModeCommandAnswer() const { return (TCommandAnswers)CACUProtocol::readStatusField<TUINT16>(m_buffer,m_disp+6); }
		inline TUINT32 executedModeCommandCounter() const { return CACUProtocol::readStatusField<TUINT32>(m_buffer,m_disp+8); }
		inline TModes executedModeCommand() const { return (TModes)CACUProtocol::readStatusField<TUINT16>(m_buffer,m_disp+12); }
		inline TCommandAnswers executedModeCommandAnswer() const { return (TCommandAnswers)CACUProtocol::readStatusField<TUINT16>(m_buffer,m_disp+14); }
		enum TConstant {
			SIZE=16
		};
	private:
		BYTE *m_buffer;
		DWORD m_disp;				
	};
	
	class TParameterCommandStatus {
	public:
		TParameterCommandStatus(BYTE *buffer,DWORD disp): m_buffer(buffer),m_disp(disp) {}
		virtual ~TParameterCommandStatus() {}
		inline TUINT32 parameterCommandCounter() const { return CACUProtocol::readStatusField<TUINT32>(m_buffer,m_disp+0); }
		inline TParameters parameterCommand() const { return (TParameters)CACUProtocol::readStatusField<TUINT16>(m_buffer,m_disp+4); }
		inline TCommandAnswers parameterCommandAnswer() const { return (TCommandAnswers)CACUProtocol::readStatusField<TUINT16>(m_buffer,m_disp+6); }
		enum TConstant {
			SIZE=8
		};		
	private:
		BYTE *m_buffer;
		DWORD m_disp;						
	};
	
	class TAxisStatus {
	public:
		TAxisStatus(BYTE *buffer,DWORD disp): m_buffer(buffer),m_disp(disp), m_modeCommandStatus(buffer,m_disp+68), m_parameterCommandStatus(buffer,m_disp+84) {}
		virtual ~TAxisStatus() {}
		inline TBOOL simulation() const { return CACUProtocol::readStatusField<TBOOL>(m_buffer,m_disp+0); }
		inline bool axisReady() const { return (bool)CACUProtocol::readStatusField<TBOOL>(m_buffer,m_disp+1); }
		inline TBOOL confOk() const { return CACUProtocol::readStatusField<TBOOL>(m_buffer,m_disp+2); } 
		inline TBOOL initOk() const { return CACUProtocol::readStatusField<TBOOL>(m_buffer,m_disp+3); }
		inline TBOOL override() const { return CACUProtocol::readStatusField<TBOOL>(m_buffer,m_disp+4); }
		inline bool lowPowerMode() const { return (bool)CACUProtocol::readStatusField<TBOOL>(m_buffer,m_disp+5); }
		inline TWarningMainAxis warnings() const { return TWarningMainAxis((TDWORD)CACUProtocol::readStatusField<TDWORD>(m_buffer,m_disp+6)); }
		inline TErrorMainAxis errors() const { return TErrorMainAxis((TDWORD)CACUProtocol::readStatusField<TDWORD>(m_buffer,m_disp+10)); }
		inline TAxisState axisState() const { return (TAxisState)CACUProtocol::readStatusField<TUINT16>(m_buffer,m_disp+14); }
		inline TUINT16 axisTrajectoryState() const { return CACUProtocol::readStatusField<TUINT16>(m_buffer,m_disp+16); }
		inline double commandedPosition() const { return (double)CACUProtocol::readStatusField<TINT32>(m_buffer,m_disp+18)/1000000.0; }
		inline TINT32 generatorPosition() const { return CACUProtocol::readStatusField<TINT32>(m_buffer,m_disp+22); }
		inline double actualPosition() const { return (double)CACUProtocol::readStatusField<TINT32>(m_buffer,m_disp+26)/1000000.0; }  // returns the acutal position in degrees
		inline double positionError() const { return (double)CACUProtocol::readStatusField<TINT32>(m_buffer,m_disp+30)/1000000.0; }
		inline double commandedVelocity() const { return (double)CACUProtocol::readStatusField<TINT32>(m_buffer,m_disp+34)/1000000.0; }
		inline TINT32 generatorVelocity() const { return CACUProtocol::readStatusField<TINT32>(m_buffer,m_disp+38); }
		inline double actualVelocity() const { return (double)CACUProtocol::readStatusField<TINT32>(m_buffer,m_disp+42)/1000000.0; }
		inline TINT32 generatorAccelaration()  { return CACUProtocol::readStatusField<TINT32>(m_buffer,m_disp+46); }
		inline double positionOffset() const { return (double)CACUProtocol::readStatusField<TINT32>(m_buffer,m_disp+50)/1000000.0; }
		inline TWORD motorSelection() const { return CACUProtocol::readStatusField<TBOOL>(m_buffer,m_disp+54); }
		inline TWORD brakesOpen() const { return CACUProtocol::readStatusField<TWORD>(m_buffer,m_disp+56); }
		inline TWORD powerModuleOk() const { return CACUProtocol::readStatusField<TWORD>(m_buffer,m_disp+58); }
		inline bool stowed() const { return (bool)CACUProtocol::readStatusField<TBOOL>(m_buffer,m_disp+60); }
		inline TBOOL stowPosOk() const { return CACUProtocol::readStatusField<TBOOL>(m_buffer,m_disp+61); }
		inline TWORD stowPinIn() const { return CACUProtocol::readStatusField<TWORD>(m_buffer,m_disp+62); }
		inline TWORD stowPinOut() const { return CACUProtocol::readStatusField<TWORD>(m_buffer,m_disp+64); }
		inline TWORD stowPinSelection() const { return CACUProtocol::readStatusField<TWORD>(m_buffer,m_disp+66); }
		inline const TModeCommandStatus& modeCommandStatus() const { return m_modeCommandStatus; }
		inline const TParameterCommandStatus& parameterCommandStatus() const { return m_parameterCommandStatus; }		
		enum TConstant {
			SIZE=92
		};
	private:
		BYTE *m_buffer;
		DWORD m_disp;	
		TModeCommandStatus m_modeCommandStatus;
		TParameterCommandStatus m_parameterCommandStatus;
	};	

	class TMotorStatus {
	public:
		TMotorStatus(BYTE *buffer,DWORD disp): m_buffer(buffer),m_disp(disp) {}
		virtual ~TMotorStatus() {}
		inline double actualPosition() const { return (double)CACUProtocol::readStatusField<TREAL32>(m_buffer,m_disp+0); }
		inline double actualVelocity() const { return (double)CACUProtocol::readStatusField<TREAL32>(m_buffer,m_disp+4); }
		inline double actualTorque() const { return (double)CACUProtocol::readStatusField<TREAL32>(m_buffer,m_disp+8); }
		inline double utilization() const { return (double)CACUProtocol::readStatusField<TREAL32>(m_buffer,m_disp+12); }
		inline bool active() const { return (bool)CACUProtocol::readStatusField<TUINT8>(m_buffer,m_disp+16); }
		inline TUINT8 speedOfRotation() const { return CACUProtocol::readStatusField<TUINT8>(m_buffer,m_disp+17); }
		inline TUINT8 speedOfRotationOk() const { return CACUProtocol::readStatusField<TUINT8>(m_buffer,m_disp+18); }
		inline bool positionError() const { return CACUProtocol::readStatusField<TUINT8>(m_buffer,m_disp+19)==0; } // the filed return 1 if the position has been reached...so negate
		inline bool busError() const { return (bool)CACUProtocol::readStatusField<TUINT8>(m_buffer,m_disp+20); }
		inline bool servoError() const { return (bool)CACUProtocol::readStatusField<TUINT8>(m_buffer,m_disp+21); }
		inline bool sensorError() const { return (bool)CACUProtocol::readStatusField<TUINT8>(m_buffer,m_disp+22); }
		inline TDWORD warningCode() const { return CACUProtocol::readStatusField<TDWORD>(m_buffer,m_disp+24); }
		enum TConstant {
			SIZE=27
		};		
	private:
		BYTE *m_buffer;
		DWORD m_disp;	
	};	
	
	class TPointingStatus {
	public:
		TPointingStatus(BYTE *buffer,DWORD disp): m_buffer(buffer),m_disp(disp), m_parameterCommandStatus(buffer,m_disp+121) {}
		virtual ~TPointingStatus() {}
		inline TREAL64 confVersion() const { return CACUProtocol::readStatusField<TREAL64>(m_buffer,m_disp+0); }
		inline TBOOL confOk() const { return CACUProtocol::readStatusField<TBOOL>(m_buffer,m_disp+8); }
		inline TINT32 encoderAzimuth() const { return CACUProtocol::readStatusField<TINT32>(m_buffer,m_disp+9); }
		Antenna::TSections azimuthSector() const { 
			TBOOL sector=CACUProtocol::readStatusField<TBOOL>(m_buffer,m_disp+27);
			return (sector==0)?Antenna::ACU_CW:Antenna::ACU_CCW;
		}
		/**********************************************************/
		inline TINT32 encoderElevation() const { return CACUProtocol::readStatusField<TINT32>(m_buffer,m_disp+28); }
		/***********************************************************/
		ACS::Time actualTime() const {
			IRA::CDateTime time; TIMEVALUE ret;
			double mjd=(double)CACUProtocol::readStatusField<TREAL64>(m_buffer,m_disp+57);
			time.setMJD(mjd); time.getDateTime(ret);
			return ret.value().value;
		}
		double actualTimeOffset() const {
			double ut=(double)CACUProtocol::readStatusField<TREAL64>(m_buffer,m_disp+65);
			return ut*DT_MSECSPERDAY;
		}
		/***********************************************************/
		inline bool clockOnLine() const { return (CACUProtocol::readStatusField<TUINT8>(m_buffer,m_disp+73)==1); }
		inline bool clockOK() const { return (CACUProtocol::readStatusField<TUINT8>(m_buffer,m_disp+74)==1); }
		inline TUINT16 year() const { return CACUProtocol::readStatusField<TUINT16>(m_buffer,m_disp+75); }
		inline TUINT16 month() const { return CACUProtocol::readStatusField<TUINT16>(m_buffer,m_disp+77); }
		inline TUINT16 day() const { return CACUProtocol::readStatusField<TUINT16>(m_buffer,m_disp+79); }
		inline TUINT16 hour() const { return CACUProtocol::readStatusField<TUINT16>(m_buffer,m_disp+81); }
		inline TUINT16 minute() const { return CACUProtocol::readStatusField<TUINT16>(m_buffer,m_disp+83); }
		inline TUINT16 second() const { return CACUProtocol::readStatusField<TUINT16>(m_buffer,m_disp+85); }
		/***************************************************************/
		inline TErrorsTracking pointingErrors() const { return TErrorsTracking((TWORD)CACUProtocol::readStatusField<TWORD>(m_buffer,m_disp+97)); }
		inline TINT32 pTTTimeOffset() const { return CACUProtocol::readStatusField<TUINT32>(m_buffer,m_disp+99); }
		/*******************************************************************/
		inline TUINT32 pTTCurrentIndex() const { return CACUProtocol::readStatusField<TUINT32>(m_buffer,m_disp+109); }
		inline TUINT32 pTTEndIndex() const { return CACUProtocol::readStatusField<TUINT32>(m_buffer,m_disp+113); }
		inline TUINT32 pTTLength() const { return CACUProtocol::readStatusField<TUINT32>(m_buffer,m_disp+117); }
		inline const TParameterCommandStatus& parameterCommandStatus() const {return m_parameterCommandStatus; }
		enum TConstant {
			SIZE=129
		};				
	private:
		BYTE *m_buffer;
		DWORD m_disp;
		TParameterCommandStatus m_parameterCommandStatus;
	};
	
	typedef struct {
		TSubsystems subsystem;
		TUINT16	command;
		TUINT32 counter;
		TCommandAnswers answer;
		bool executed;
		bool parameterCommand;
		bool error;
	} TCommand;

	/**
	 * Constructor.
	 */
	CACUProtocol();
	
	/**
	 * Destructor.
	 */
	~CACUProtocol();
		
	 /**
	 * It allows to activate the ACU.
	 * @param azimuth true if the azimuth axis must be activated
	 * @param elevation true if the elevation axis must be activated
	 * @param buff a reference to the buffer containing the command to be sent. It need to be freed by the caller
	 * @param command return the basic information about the commands that can be used to check the answer from the status socket. It must be freed by caller.
	 * @param commNumber number of elements of the command array
	 * @return the length in bytes of the buffer, if zero an error occurred and the <i>outBuffer</i> argument could contain impredictable results and need not to be freed	
	 */	
	 WORD activate(bool azimuth,bool elevation,BYTE *& buff,TCommand *& command,WORD& commNumber);
	 
	 /**
	 * It allows to deactivate the ACU.  
 	 * @param azimuth true if the azimuth axis must be deactivated
	 * @param elevation true if the elevation axis must be deactivated 
	 * @param buff a reference to the buffer containing the command to be sent. It need to be freed by the caller
	 * @param command return the basic information about the commands that can be used to check the answer from the status socket. It must be freed by caller.
	 * @param commNumber number of elements of the command array
	 * @return the length in bytes of the buffer, if zero an error occurred and the <i>outBuffer</i> argument could contain impredictable results and need not to be freed	
	 */	
	 WORD deactivate(bool azimuth,bool elevation,BYTE *& buff,TCommand *& command,WORD& commNumber);
	 
	 /**
	  * This will allow to prepare the message to command a preset mode and position to the antenna.
	  * @param azPos new azimuth position of the telescope. It has to be in degrees.
	  * @param elPos new elevation position of the telescope. It has to be in degrees.
	  * @param azRate rate to be applied for azimuth. Degrees per second.
	  * @param elRate rate to be applied for elevation. Degrees per second.
	  * @param buff a reference to the buffer containing the command to be sent.
	  * @param command return the basic information about the commands that can be used to check the answer from the status socket. It must be freed by caller.
	  * @param commNumber number of elements of the command array
	  * @return the length in bytes of the buffer, if zero an error occurred and the <i>outBuffer</i> argument could contain impredictable results and need not to be freed	
	  */
	 WORD preset(const double& azPos,const double& elPos,const double& azRate,const double& elRate,BYTE *& buff,TCommand *& command,WORD& commNumber);
	 
	 /**
	  * This will allow to prepare the message to command a rate mode and velocity to the antenna.
	  * @param azRate new azimuth position of the telescope. if a zero is passed the telescope is not moved. It has to be in degrees per second.
	  * @param elRate new elevation position of the telescope. if a zero is passed the telescope is not moved. It has to be in degrees per second.
	  * @param buff a reference to the buffer containing the command to be sent.
	  * @param command return the basic information about the commands that can be used to check the answer from the status socket. It must be freed by caller.
	  * @param commNumber number of elements of the command array
	  * @return the length in bytes of the buffer, if zero an error occurred and the <i>outBuffer</i> argument could contain impredictable results and need not to be freed	
	  */
	 WORD rate(const double& azRate,const double& elRate,BYTE *& buff,TCommand *& command,WORD& commNumber);
	 
	 /**
	  * This will allow to prepare the message to command a program track mode to the antenna
	  * @param azMaxRate maximum allowed azimuth speed during program track (degrees/sec)
	  * @param elMaxRate maximum allowed elation speed during program track (degrees/sec)
	  * @param buff a reference to the buffer containing the command to be sent.
	  * @param command return the basic information about the commands that can be used to check the answer from the status socket. It must be freed by caller.
	  * @param commNumber number of elements of the command array
	  * @return the length in bytes of the buffer, if zero an error occurred and the <i>outBuffer</i> argument could contain impredictable results and need not to be freed	
	  */
	 WORD programTrack(const double& azMaxRate,const double& elMaxRate,BYTE *& buff,TCommand *& command,WORD& commNumber);
	 
	 /*
	 * It creates a buffer to be sent to the ACU in order to stop the telescope motion.  
	 * @param buff a reference to the buffer containing the command to be sent. It need to be freed by the caller
	 * @param command return the basic information about the commands that can be used to check the answer from the status socket. It must be freed by caller.
	 * @param commNumber number of elements of the command array
	 * @return the length in bytes of the buffer, if zero an error occurred and the <i>outBuffer</i> argument could contain impredictable results and need not to be freed	
	 */
	 WORD stop(BYTE *& buff,TCommand *& command,WORD& commNumber);

	 /**
	  * This will allow to prepare the message to be sent to the ACU in order to park the antenna.
	  * @param stowSpeed speed of the antenna during slewing to stow position (degrees/sec)	
	  * @param buff a reference to the buffer containing the command to be sent.
	  * @param command return the basic information about the commands that can be used to check the answer from the status socket. It must be freed by caller.
	  * @param commNumber number of elements of the command array
	  * @return the length in bytes of the buffer, if zero an error occurred and the <i>outBuffer</i> argument could contain impredictable results and need not to be freed	
	  */	 
	 WORD stow(const double& stowSpeed,BYTE *& buff,TCommand *& command,WORD& commNumber);

	 /**
	  * This will allow to prepare the message to be sent to the ACU in order to extract the stow pins.
	  * @param buff a reference to the buffer containing the command to be sent.
	  * @param command return the basic information about the commands that can be used to check the answer from the status socket. It must be freed by caller.
	  * @param commNumber number of elements of the command array
	  * @return the length in bytes of the buffer, if zero an error occurred and the <i>outBuffer</i> argument could contain impredictable results and need not to be freed	
	  */	 
	 WORD unstow(BYTE *& buff,TCommand *& command,WORD& commNumber);

	 /**
	  * This will allow to prepare the message to be sent to the ACU in order to reset all active errors on axis
	  * @param buff a reference to the buffer containing the command to be sent.
	  * @param command return the basic information about the commands that can be used to check the answer from the status socket. It must be freed by caller.
	  * @param commNumber number of elements of the command array
	  * @return the length in bytes of the buffer, if zero an error occurred and the <i>outBuffer</i> argument could contain impredictable results and need not to be freed	
	  */	 	 
	 WORD resetErrors(BYTE *& buff,TCommand *& command,WORD& commNumber);
	 
	 /**
	  * This will allow to prepare the message to be sent to the ACU in order to set the position offsets in both axis.
	  * @param azOff azimuth offset (degrees)
	  * @param elOff elevationOffset (degrees) 
	  * @param buff a reference to the buffer containing the command to be sent.
	  * @param command return the basic information about the commands that can be used to check the answer from the status socket. It must be freed by caller.
	  * @param commNumber number of elements of the command array
	  * @return the length in bytes of the buffer, if zero an error occurred and the <i>outBuffer</i> argument could contain unpredictable results and need not to be freed
	  */	 	 
	 WORD positionOffsets(const double& azOff,const double& elOff,BYTE *& buff,TCommand *& command,WORD& commNumber);
	 
	 /**
	  * This will allow to prepare the message to be sent to the ACU in order to set the servo system time source and synchronize the ACU.
	  * @param time reference time 
	  * @param name of the time source to be used for synchronization
	  * @param buff a reference to the buffer containing the command to be sent.
	  * @param command return the basic information about the commands that can be used to check the answer from the status socket. It must be freed by caller.
	  * @param commNumber number of elements of the command array
	  * @return the length in bytes of the buffer, if zero an error occurred and the <i>outBuffer</i> argument could contain unpredictable results and need not to be freed
	 */	 	 	 
	 WORD setTime(const ACS::Time& time,const IRA::CString& timeSource,BYTE *& buff,TCommand *& command,WORD& commNumber);
	 
	 /**
	  * This will allow to prepare the message to be sent to the ACU in order to set the time offset used to shift the start time of program track mode
	  * @param seconds number of seconds and fraction of the time offsets 
	  * @param buff a reference to the buffer containing the command to be sent.
	  * @param command return the basic information about the commands that can be used to check the answer from the status socket. It must be freed by caller.
	  * @param commNumber number of elements of the command array
	  * @return the length in bytes of the buffer, if zero an error occurred and the <i>outBuffer</i> argument could contain unpredictable results and need not to be freed
	 */	 	 	 
	 WORD setTimeOffset(const double& seconds,BYTE *& buff,TCommand *& command,WORD& commNumber);
	 
	 /**
	  * This will allow to prepare the message to be sent to the ACU in order to load the program track table into the ACU.
	  * @param sartEpoch reprsent the time of the first point in the tracking curve
	  * @param seq pointer to the sequence of program track points to be loaded into the ACU.
	  * @param size number of points in the sequence above, if newTable is true the points list must have at least a minimum number of elements (actually 5)
	  * @param newTable true if a new table has to be started, if false the new point is attached to the existing table
	  * @param azRate maximum rate in azimuth (degrees per second)
	  * @param elRate maximum rate in elevation (degrees per second)
	  * @param buff a reference to the buffer containing the command to be sent.
	  * @param command return the basic information about the commands that can be used to check the answer from the status socket. It must be freed by caller.
	  * @param commNumber number of elements of the command array
	  * @return the length in bytes of the buffer, if zero an error occurred and the <i>outBuffer</i> argument could contain unpredictable results and need not to be freed
	 */	 	 	 	 
	 WORD loadProgramTrack(const ACS::Time& startEpoch,const TProgramTrackPoint *seq,const WORD& size,bool newTable,const double& azRate,const double& elRate,BYTE *& buff,TCommand *& command,WORD& commNumber);
	 
	 /**
	  * This method is used to check the status buffer sent by the ACU.
	  * @param inBuffer pointer to the buffer that contains the status data coming from the ACU, it may contains more than one or a part  of the whole ACU status record.
	  * @param inLen length in bytes of the input buffer
	  * @param outBuffer if the method could locate (in case also  sTCommandAnswersynchronizing with previous buffers) a valid status record, this buffer will store it.
	  * @return a code that describes the result of the operation:
	  * 			  @arg \c  >0 the length of the buffer in output if the operation succeeds 
	  *              @arg \c 0 the buffer is apparently ok but is not complete, it will be probably completed with the next incoming buffer
	  * 			 @arg \c <0 the buffer contains errors and must be discarded
	  */  
	 int syncBuffer(BYTE * inBuffer,const WORD& inLen,BYTE *outBuffer);
	 
	 /**
	  * This method can be called in order to match the command answers coming from the status line socket and the last send command in order to check the execution result of the command
	  * The command structure is modified properly.
	  * @param command sequence of commands that composes the last message sent to the ACU
	  * @param size dimension of the previous argument
	  * @param subsystem name of the subsystem involved in the command
	  * @param cmd command identifier
	  * @param counter command counter
	  * @param answer answer to the command
	  */
	 static void matchCommandAnswer(TCommand* command,const WORD& size,const TSubsystems& subsystem,const TUINT16& cmd,const TUINT32& counter,const TCommandAnswers& answer); 
	 
	 /**
	  * This method can be called in order to check if the ACU gave an answer to a command message.
	  * @param command list of commands that composes the command message
	  * @param size size of the commands list
	  * @param answer answer code from the ACU
	  * @param error true if the answer is an error code
	  * @return true if the ACU sent the answer, false if the answer has not been sent yet
	  */
	 static bool checkCommandAnswer(TCommand* command,const WORD& size,TCommandAnswers& answer,bool& error);
	 
	 static TModes modesIDL2Protocol(const Antenna::TCommonModes& mode);

	 static Antenna::TCommonModes modesProtocol2IDL(const TModes& mode);
	 
	 static IRA::CString modesIDL2String(const Antenna::TCommonModes& mode);
	 
	 static IRA::CString commandAnswer2String(const TCommandAnswers& answer);
	 
	 static double time2MJD(const ACS::Time& time);
	 
	 static TTimeSources str2TimeSource(const IRA::CString ts);

	 template <class T> static T readStatusField(BYTE* statusBuffer,DWORD offset) {
		BYTE *pos=statusBuffer+offset;
		return (T) *((T *)pos);
	 }

private:
	/** 
	 * This buffer will be used to store a status buffer in order to realize the buffer synchornization.
	 */  
	Protocol_Internal::CCircularArray *m_syncBuffer;
	
	/**
	 * used during buffer synchronization, in order to check is the start flag has alaready been found
	 */
	bool m_bbufferStarted;
	
	/**
	 * Try to localize the start flag inside the internal sync buffer.
	 * @param start position from which the search is started, in case of success it returns the index of the starting byte of the start flag, otherwise the
	 *               last position scanned.
	 * @return true if the flag was located, false otherwise.
	 */ 
	bool  findStartFlag(DWORD& start) const;
	
	/**
	 * Try to localize the end flag inside the internal sync buffer.
	 * @param stop this argument indicates the  last byte of the buffer, where the end flag is expected to be.
	 * @return 0 if the sync buffer does not contain the flag or -1 if the bytes of the given position do not correspond to the end flag. A zero in case of success
	 */ 
	int findEndFlag(const DWORD& stop);
	
	/**
	 * This method can be called to get the status message length
	 * @param msgLen the status message length
	 * @return 0 if the sync buffer does not contain length indication, -1 if the length is not correct or differs from the expected one.
	 *         A positive in case of success
	 */
	int findStatusMessageLenght(DWORD& msgLen);
	
	/**
	 * This method prepares a message that contains a mode command. 
	 * @param azMode mode to be commanded to the azimuth axis
	 * @param elMode mode to be commanded to the elevation axis
	 * @param azP1 first parameter for the azimuth mode
	 * @param azP2 second parameter for the azimuth mode
	 * @param elP1 first parameter for the azimuth mode
	 * @param elP2 second parameter for the azimuth mode
	 * @param outBuff buffer that stores the message to the ACU. It must be freed. 
	 * @param command return the basic information about the commands that can be used to check the answer from the status socket. It must be freed by caller.
	 * @param commNumber number of elements of the command array
	 * @return the length in bytes of the message, if zero an error occurred and the <i>outBuffer</i> argument could contain impredictable results and need not to be freed	   
	 */
	WORD modeCommand(const TModes& azMode,const TModes& elMode,const double& azP1,const double& azP2,const double& elP1,const double& elP2,BYTE *& outBuff,TCommand *& command,WORD& commNumber) const;

	/**
	 * This method prepares a message that contains a mode command.
	 * @param outBuff buffer that stores the message to the ACU. It must be freed.
	 * @param command return the basic information about the commands that can be used to check the answer from the status socket. It must be freed by caller.
	 * @param commNumber number of elements of the command array
	 * @return the length in bytes of the message, if zero an error occurred and the <i>outBuffer</i> argument could contain impredictable results and need not to be freed
	 */
	WORD modeCommand(BYTE *& outBuff,TCommand *& command,WORD& commNumber) const;
	
	/**
	 * This method fills a parameter command.
	 * @param parameterID parameter identifier
	 * @param subsystem this is the identifier of the subsystem the parameter is going to take effect
	 * @param p1 first parameter value
	 * @param p2 second parameter value;
	 * @param command the basic information about the commands that can be used to check the answer from the status socket.
	 * @return the number fo bytes of the message
	 */
	WORD parameterCommand(const TParameters& parameterID,const TSubsystems& subSystem,const double& p1,const double& p2,BYTE * outBuff,TCommand* command);
	
	/**
	 * This method is used internally to add to the message all auxiliary data required to complete the any command message.
	 * @param msg message to packed
	 * @param msgLen length in bytes of the message to be packed
	 * @param commands number of commands that the message contains
	 * @param outBuff buffer that stores the message to the ACU, it must be freed by the caller
	 * @return the length in bytes of the final message, if zero an error occurred and the <i>outBuffer</i> argument could contain impredictable results and need not to be freed
	 */
	WORD packMessage(BYTE *msg,const WORD& msgLen,const TUINT32& commands,BYTE *& outBuff) const;
	
	/**
	 * Copy a value inside a buffer
	 */
	template <class DATATYPE> void copyData(BYTE *dst,const DATATYPE& data,WORD& position) const {
		*((DATATYPE *)(dst+position))=data;
		position+=sizeof(DATATYPE);
	}
	
	/**
	 * return the current time as number of milliseconds of the day.
	 */
	TUINT32 getMillisOfTheDay() const;
};


#endif
