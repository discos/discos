#ifndef ACUMACROS_H
#define ACUMACROS_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: ACUInterface.h,v 1.4 2010-09-24 15:38:53 a.orlati Exp $													  */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                 when            What                                             */
/* Andrea Orlati(aorlati@ira.inaf.it)  30/08/2005      Creation                                         */
/* Andrea Orlati(aorlati@ira.inaf.it)  10/11/2006      Added TServoSystemStatus enumeration             */
/* Andrea Orlati(aorlati@ira.inaf.it)	14/11/2006		 Added the static getSection method               */

#include <IRA>

using namespace IRA;

/**
 * This class implements the interface toward the ACU. It supplies some methods that allow to comunicate to the 
 * ACU using the right protocol. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
 */
class CACUInterface {
public:
	/** This is used to describe the actual modes for each of the ACU axe */
	enum TAxeModes {
		STANDBY=0,	      /*!< Axe is in standby */
		STOP=1,			  /*!< Axe break is on  */
		PRESET=2,		  /*!< Axe is in preset mode  */
		PROGRAMTRACK=3,   /*!< Axe is in program track mode */
		RATE=4,		      /*!< Axe is in rate mode */
		SECTORSCAN=5,     /*!< Axe is in sector scan mode */
		STOW=6,           /*!< Axe is in survival position */
		BORESIGHT=7,      /*!< Axe is in boresight mode */
		STARTRACK=9,      /*!< Axe is in star track mode */
		POSITIONTRACK=11, /*!< Axe is in position track mode */
		UNSTOW=14,        /*!< Axe is extracting the stow pins */ 
		UNKNOWN=20		  /*!< Axe is in unknown mode */ 
	};
	/** This is used to describe the actual servo status for both the ACU axes */
	enum TServoStatus {
		EMERGENCYLIMIT=1U,
		OPERLIMITCCW=2U,
		OPERLIMITCW=4U,
		PRELIMITCCW=8U,
		PRELIMITCW=16U,
		STOWPOSITION=32U,
		STOWPININSERTED=64U,
		STOWPINRETRACTED=128U,
		SERVOFAILURE=256U,
		BRAKEFAILURE=512U,
		ENCODERFAILURE=1024U,
		MOTIONFAILURE=2048U,
		MOTOROVERSPEED=4096U,
		AUXILIARYMODE=8192U,
		AXISDISABLED=16384U,
		COMPUTERDISABLED=32768U
	};	
	/** This lists the bitwise flags (meaningful) that compose the servo system status word */
	enum TServoSystemStatus {
		DOORINTERLOCK=1U,
		SAFE=2U,
		EMERGENCY=64U,
		TRANSFERERROR=512U,
		TIMEERROR=1024U,
		RFINHIBIT=2048U,
		REMOTE=32768U
	};
	/** This is used to describe the sections of the antenna travel range */
	enum TAntennaSection {
		CW=0,				/*!< clock wise section */
		CCW=1,			   /*!< counter clock wise section */  
		NEUTRAL=2   /*!< used only when commanding new positions, it indicates no preference for the chosen section */
	};
	/**
	 * This method is used to retrive a more human readable message from an error code returned by the ACU.
	 * @param Err error code that explains the origin of a problem
	 * @return the explanation of the error code.
	*/
	static CString messageFromError(BYTE Err);
	
	/** 
	 * This method convert from a TAxeModes to a human readable character string.
	 * @param mode axes mode taht should be translated
	 * @return the string tha conatains the description of the mode
 	*/
	static CString messageFromMode(TAxeModes mode);
	
	/**
	 * Prepare a buffer that must be sent to the ACU in order to obtain a new monitor data buffer.
	 * @param Buff pointer to the byte buffer that will contain the telegram
	 * @return the length of the buffer
	*/
	static WORD getMonitorBuffer(BYTE *Buff);
	
	/**
	 * Prepare a buffer that must be sent to the ACU in order to start a rate tracking.
	 * @param Buff pointer to the byte buffer that will contain the telegram
	 * @param azVel This is azimuth velocity (10^-6 degrees) (-0.8 to 0.8 degrees/sec)	 
	 * @param elVel This is elevation velocity (10^-6 degrees) (-0.5 to 0.5 degrees/sec)	
	 * @return the length of the buffer
 	*/
	static WORD getRateBuffer(BYTE *Buff,long azVel,long elVel);
	
	/**
	 * Prepare a buffer that must be sent to the ACU in order to preset a new position.
	 * @param Buff pointer to the byte buffer that will contain the telegram
	 * @param Azim Azimuth coordinate (32 bit signed integer) (10^-6 degrees) (-70 to 455 degrees)
	 * @param Elev Elevation coordinate (32 bit signed integer) (10^-6 degrees) (3 to 120 degrees)	 
	 * @return the length of the buffer
	*/	
	static WORD getPresetBuffer(BYTE *Buff,long Azim,long Elev);
	
	/**
	 * Prepare a buffer that must be sent to the ACU in order to clear any servo failures.
	 * @param Buff pointer to the byte buffer that will contain the telegram	 
	 * @return the length of the buffer	 
	*/		
	static WORD getFailureResetBuffer(BYTE* Buff);
	
	/**
	 * Prepare a buffer that must be sent to the ACU in order to set actual time.
	 * @param Buff pointer to the byte buffer that will contain the telegram	 
	 * @param dayTime 0...86400000 milliseconds of the current day
	 * @param yearDay 1..366 days of the current year
	 * @param year current year, from 2000 to 2100 
	 * @return the length of the buffer	 
	*/		
	static WORD getTimeTransferBuffer(BYTE* Buff,int dayTime,short int yearDay,short int year);
	
	/**
	 * Prepare a buffer that must be sent in order to transfer a new position and velocity spot of a
	 * trajectory to the ACU.
	 * @param Azim Azimuth coordinate (32 bit signed integer) (10^-6 degrees) (-70 to 455 degrees)
	 * @param Elev Elevation coordinate (32 bit signed integer) (10^-6 degrees) (3 to 120 degrees)	 
	 * @param azVel This is the current target azimuth velocity (10^-6 degrees) (-0.8 to 0.8 degrees/sec)	 
	 * @param elVel This is the current target elevation velocity (10^-6 degrees) (-0.5 to 0.5 degrees/sec)	 
	 * @param Fit If this parameter is true the velocity parameters will be ignored and the velocities are calculated by the ACU
	 * @param Buff pointer to the byte buffer that will contain the telegram
	 * @return the length of the buffer
	*/	
	static WORD getPositionTrackBuffer(BYTE *Buff,long Azim,long Elev,long azVel,long elVel,bool Fit=false);
	
	/**
	 * Prepare a buffer that must be sent in order to transfer the time stamp and the required position of a particular spot of a
	 * trajectory.
	 * @param Azim Azimuth coordinate (32 bit signed integer) (10^-6 degrees) (-70 to 455 degrees)
	 * @param Elev Elevation coordinate (32 bit signed integer) (10^-6 degrees) (3 to 120 degrees)	 
	 * @param azVel This is the current target azimuth velocity (10^-6 degrees) (-0.8 to 0.8 degrees/sec)	 
	 * @param elVel This is the current target elevation velocity (10^-6 degrees) (-0.5 to 0.5 degrees/sec)	 
	 * @param dayTime Time of the day (32 bit signed integer) (10^-3) (0 to 86400000 milliseconds)
	 * @param yearDay Day of year (16 bit signed integer) (0 to 366 days)
	 * @param Fit If this parameter is true the velocity parameters will be ignored and the velocities are calculated by the ACU
	 * @param Clear If this flag is true then the stack tha contains the sets of values is cleared 
	 * @param Buff pointer to the byte buffer that will contain the telegram
	 * @return the length of the buffer	 
	*/
	static WORD getProgramTrackBuffer(BYTE *Buff,long Azim,long Elev,long azVel,long elVel,long dayTime,short yearDay,bool Fit=false,bool Clear=false);
		
	/**
	 * Prepare a buffer that must be sent in order to activate a particular mode for the antenna.
	 * @param Az Mode that should have been activated for the azimuth axe 
	 * @param El Mode that should have been activated for the elevation axe 
	 * @param DrivePower THis flag can be used to command the remote power on after a power break or emergency off
	 * @param Buff pointer to the byte buffer that will contain the telegram
	 * @return the length of the buffer
	*/	
	static WORD getModeSelectionBuffer(BYTE *Buff,TAxeModes Az,TAxeModes El,bool DrivePower=false);
	
	/**
	 * This function vets the ACU answer buffer and check if the answer is ACK or NAK. in the latter case it also returns the
	 * reason for refusing the command.
	 * @param Buff pointer to the byte buffer that contains the ACU answer.
	 * @param Err Reference to the code that eventually provides the explanation of the NAK.
	 * @return true if the answer was ACK, false otherwise.
	*/
	static bool isAck(const BYTE *Buff,BYTE &Err);		
	
	/** 
	 * This function converts from an unsigned 8 bit integer to a <i>TAxeModes</i> object; 
	 * if the mode doesn't not exist it will retunn UNKNOW 
	*/
	static TAxeModes getMode(BYTE mode);
	
	/**
	 * This function returns the antenna presnt section given the control word
	*/
	static TAntennaSection getSection(WORD controlWord);
			
};

#endif
