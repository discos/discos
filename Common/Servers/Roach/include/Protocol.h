#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: Protocol.h,v 1.2 2011-05-12 14:14:31 a.orlati Exp $																								*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)     13/11/2008      Creation                                         */

#include <IRA>

#define PROT_TERMINATOR_CH '\n'
#define PROT_TERMINATOR "\n"
#define PROT_SEPARATOR_CH ' '
#define PROT_SEPARATOR " "
#define PROT_SEPARATOR_COMMA ','

#define PROT_GET_CONF "?"
#define PROT_SET_CONF_BROAD "I"
#define PROT_SET_CONF "A"
#define PROT_SET_TIME "T"
#define PROT_START_ACQ "X"
#define PROT_CHECK_TIME "E"
#define PROT_AUTO_GAIN "G"
#define PROT_STOP_ACQ "stop"
#define PROT_RESUME_ACQ "resume"
#define PROT_SUSPEND_ACQ "pause"
#define PROT_SET_ZERO "Z"
#define PROT_SET_SAMPLE_RATE "S"
#define PROT_GET_SAMPLE "R"

#define PROT_ACK "ack"

//#define PROT_PACKET_LENGTH 64
//#define PROT_TOTAL_DEVICES 14

using namespace IRA;

class CProtocol {
public:
	/**
	 * This enum enlists the pssoble configurations that can be done to set the source of  the backend input signal.
	 */
	enum TInputs {
		PRIMARY,
		BWG,
		GREGORIAN,
		OHM50
	};
	
	/** 
	 * Prepare a buffer to be sent to the backend in order to request the backend configuration
	 * @param sBuff pointer to the buffer that contains the message. the buffer must be allocated by the caller.
	 * @return the length of the message
	 */ 
	static WORD askBackendConfiguration(char *sBuff);
	
	/**
	 * Prepare a buffer to be sent to the backend in order to configure all the inputs in one shot.
	 * @param sBuff pointer to the buffer that contains the message. the buffer must be allocated by the caller.
	 * @param input allows to set up the input type
	 * @param att allow to indicate the attenuation level
	 * @param bw allows to set up the bandwidth
	 * @return the length of the message
	 */
	static WORD setConfiguration_broadcast(char *sBuff,const TInputs& input,const double &att,const double& bw);
	
	/**
	 * Prepare a buffer to be sent to the backend in order to configure one input at a time.
	 * @param sBuff pointer to the buffer that contains the message. the buffer must be allocated by the caller.
	 * @param id numerical identifier of the section
	 * @param input allows to set up the input type
	 * @param att allow to indicate the attenuation level
	 * @param bw allows to set up the bandwidth
	 * @param boards gives the mapping of section over boards, if null the mapping is flat: section 0 on board 0 and so on....
	 * @return the length of the message
	 */
	static WORD setConfiguration(char *sBuff,long id,const TInputs& input,const double &att,const double& bw,long *boards=NULL);
	
	/**
	 * Prepare a buffer to be sent to the backend in order to sync  the time. The time is taken from the clock of the host machine.
	 * Since the message to the backend must arrive between 300th and 700th milliseconds of each second, if the current time
	 * does not lie in that window, this method could suspend execution for the required amount of time.
	 * @param sBuff pointer to the buffer that contains the message. the buffer must be allocated by the caller.
	 * @return the length of the message
	 */
	static WORD setBackendTime(char *sBuff);
	
	/**
	 * Prepare a buffer to be sent to the backend in order th check the time synchronization. 
	 * @param sBuff pointer to the buffer that contains the message. the buffer must be allocated by the caller.
	 * @return the length of the message
	 */
	static WORD checkBackendTime(char *sBuff);
	
	/**
	 * Prepare a buffer to be sent to the backend in order to start the data acquisition.
	 * @param sBuff pointer to the buffer that contains the message. the buffer must be allocated by the caller.
	 * @param sampleRate this is the rate used by the backend to sample the total power
	 * @param cal this parameter gives the number of normal sample to have a sample with the noise call on.
	 * @param port port number of the socket that will wait for the data
	 * @param address TCP/IP address of the machine that waits for the data.
	 * @return the length of the message
	 */
	static WORD startAcquisition(char *sBuff,const double& sampleRate,const long& cal,const WORD& port,const IRA::CString& address);
	
	/**
	 * Prepare a buffer to be sent to the backend in order to resume a data acquisition
	 * @param sBuff pointer to the buffer that contains the message. the buffer must be allocated by the caller.
	 * @return the length of the message
	 */
	static WORD resumeAcquisition(char *sBuff);
	
	/**
	 * Prepare a buffer to be sent to the backend in order to suspend the data acquisition
	 * @param sBuff pointer to the buffer that contains the message. the buffer must be allocated by the caller.
	 * @return the length of the message
	 */
	static WORD suspendAcquisition(char *sBuff);
	
	/**
	 * Prepare a buffer to be sent to the backend in order to stop the data acquisition.
	 * @param sBuff pointer to the buffer that contains the message. the buffer must be allocated by the caller.
	 * @return the length of the message
	*/
	static WORD stopAcquisition(char *sBuff);
	
	/** 
	 * Prepare a buffer to be sent to the backend in order to command the zero RF input or restore the normal input.
	 * @param sBuff pointer to the buffer that contains the message. the buffer must be allocated by the caller.
	 * @param activate if true the mode zero input is activated, otherwise false will go back to the normal input mode 
	 * @return the length of the message
	*/
	static WORD setZeroInput(char *sBuff,bool activate);
	
	/**
	 * Prepare a buffer to be sent to the backend in order to issue the AGC functionality. This functionality will equalize all the inputs level
	 * @param sBuff pointer to the buffer that contains the message. the buffer must be allocated by the caller.
	 * @param channels number of channels that will be passed through the functionality
	 * @param level given in counts, is the point where the channels level is brought to.
	 */
	static WORD AutoGainControl(char *sBuff,const WORD& channels,const WORD& level);
	
	/**
	 * Prepare a buffer to be sent to the backend in order to set the sample rate that is used for the slow mode. The slow mode
	 * can be used to get a single tpi measure, so this value has nothing to to with the sampleRate set by the <i>startAcquisition()</i>
	 * procedure. Also in that case the sample rate can be interpreted as an integration time.
	 * @param sBuff pointer to the buffer that contains the message. the buffer must be allocated by the caller.
	 * @param integrationTime the integration time in milliseconds.
	 * @return the length of the message
	 */ 
	static WORD setIntegrationTime(char *sBuff,const long& integrationTime);
	
	/**
	 * Prepare a buffer ti be sent to the backend in order to get from the backend a tpi measure using the slow mode. The sample
	 * rate used is set by the call to <i>setSampleRate</i>.
	 * @param sBuff pointer to the buffer that contains the message. the buffer must be allocated by the caller.
	 */
	static WORD getSample(char *sBuff);
	
	/**
	 * Decodes the answer of the backend when asked about its configuration. The vectors must already allocated.
	 * @param rBuff buffer that contains the backend answer
	 * @param inputsNumber this is to inform the function about how many section are expected (size of output arrays)
	 * @param boardNumber number of installed boards, this dimensions the size of the answer from the backend
	 * @param att this vector reports the attenuation values for each of the backend inputs (db)
	 * @param bw this vector reports the band width for each of the backend inputs (Mhz)
	 * @param in this vector reports about which analog source the input is connected to.
	 * @param tm reports the current time of the backend FPGA.
	 * @param currentSR reports the sample rate currently used in the backend (milliseconds)
	  *@param boards gives the mapping of section over boards, if null the mapping is flat: section 0 on board 0 and so on....
	 * @return true if the answer is correct and could be parsed 
	 */
	static bool decodeBackendConfiguration(const char *rBuff,const long& sectionNumber,const DWORD& boardsNumber,double *att,double *bw,TInputs *in,TIMEVALUE& tm,long& currentSR,long * boards);
	
	/**
	 * Decodes the answer of the backend after it has been commanded a new time. 
	 * @param rBuff buffer that contains the backend answer
	 * @param res this parameter can be used to check if the backend has properly synchronized its clock. True in that case.
	 * @return true if the answer is correct and could be parsed.
	 */
	static bool decodeBackendTime(const char *rBuff,bool& res);

	static bool status(char *rBuff,double &timestamp, char *statusCode, bool &acquiring);

	static bool setConfiguration(const char *rBuff);
	
	/**
	 * Decodes the answer of the backend after it has been commanded a check time operations. 
	 * @param rBuff buffer that contains the backend answer
	 * @param res this parameter can be used to check if the backend is properly synchronized. True in that case.
	 * @return true if the aswer is correct and could be parsed regularly.
	 */	
	static bool checkBackendTime(const char * rBuff,const long threshold,bool& res);
	
	/**
	 * Check if the backend answer is an acknowledge or not
	 * @param rBuff backend answer
	 * @return true if the backend acknowledges
	 */
	static bool isAck(const char *rBuff);
	
	/**
	 * This method is used to decode the data contained in the backend data packet. 
	 * @param buff pointer to the buffer that contains the data
	 * @param sampleRate current value of the sample rate, used by the  backend to sample the data
	 * @param prevStatus stores the status of the previous sample
	 * @param prevCounter stores the numeral counter of the previous sample
	 * @param tm date and time mark of the data (returned)
	 * @param counter sample counter 
	 * @param flag this is a bit pattern that reports some information about the data
	 * @param data this will contain the real data can be found. The size is given b <i>sectionNumber</i>
	 *@param boards gives the mapping of section over boards, if null the mapping is flat: section 0 on board 0 and so on....
	 */
	static void decodeData(BYTE *buff,const double& sampleRate,const WORD& prevStatus,const WORD& prevCounter,
			TIMEVALUE& tm,WORD& counter,WORD& flag,DWORD *data,const long& sectionNumber,long *boards=NULL);
	
	/**
	 * This method is used to decode the data coming from the backend when asked for slow mode acquisition
	 * @param buff pointer to the buffer that contains the data
	 * @param data this will point to an array of elements (must be allocated by the caller) that contains the total power measure of
	 *                each section the size is given by the <i>sectionNumber</i> argument
	 * @param boardsNumber the number of installed boards
	 * @param sectionNumber number of supported section, the size of the data array
	 * @param boards map a section into a board (its size is <i>sectionNumber</i>), if null the mapping is flat: section 0 on board 0 and so on....
	 * @return true if the backend answer could be decoded, false otherwise.
	 */
	static bool decodeData(const char* rBuff,DWORD *data,const DWORD& boardsNumber,const long& sectionNumber,long *boards=NULL);
	
	/**
	 * This function takes in input the backend flag (given together with the data and check if the data packet is taken when the
	 * cal diode is off
	 * @param flag backend flag, bit pattern
	 * @return true if the data refers to normal measurement
	 */
	static bool isTpi(const WORD& flag);

	/**
	 * This function takes in input the backend flag (given together with the data and check if the data packet is taken when the
	 * cal diode is on
	 * @param flag backend flag, bit pattern
	 * @return true if the data refers to calibration diode on measurement
	 */
	static bool isCal(const WORD& flag);
	
	/**
	 * This function checks if the current backend sample is the beginning of a new stream by comparing the previous sample status
	 * and counter to the current sample counter.
	 * @param previousStatus stores the status of the previous sample
	 * @param previousCounter stores the numeral counter of the previous sample
	 * @param currentCounter stores the current sample counter
	 * @return true if the sample is the first of a new stream
	 */
	static bool isNewStream(const WORD& previousStatus,const WORD& previousCounter,const WORD& currentCounter);
		
private:
	
	/** 
	 * Swap the 32 bits word coming from the backend into a standard little endian word.
	 * @param pointer to the 32 bits word
	 */
	static void swap(DWORD* word);

	
	/**
	 * This method is used to convert the time as given by the backend FPGA(string) into a full ACS timestamp.
	 * @param str string that containes the time of the backend FPGA (given as number of seconds from 01/10/1970)
	 * @param tm decoded time corresponding to the one given with first parameter
	 * @return false in case of errors
	 */  
	static bool decodeFPGATime(const IRA::CString& str,TIMEVALUE& tm);
	
	/**
	 * This method is used to convert the time as given by the backend FPGA into a full ACS timestamp.
	 * @param clock number of seconds from 01/10/1970
	 * @param sampleRate the sampling rate of the backend, each sample is spaced (in time) by the inverse of that number
	 * @param counter sample counter inside each second of acquisition, one indicates the first sample
	 * @param tm decoded time corresponding to the one given with first parameter
	 * @return false in case of errors
	*/  	
	static bool decodeFPGATime(const DWORD& clock,const double& sampleRate,const WORD& counter,TIMEVALUE& tm);
	
	/**
	 * This method can be used to translate the backend answer regarding the attenuation level to its corresponding
	 * double value (db)
	 * @param str string that contains the attenuation level
	 * @param val corresponding attenuation level
	 * @return false if the translation could not be performed
	 */
	static bool decodeAttenuationLevel(const IRA::CString& str,double& val);
	
	 /**
	 * This method is used to encode  an attenuation level, given in db, into the backend representation
	 * @param att double value that represent the attenuation
	 * @return a string that contains the backend representation of the given attenuation
	 */
	static IRA::CString encodeAttenuationLevel(const double& att);
	
	/**
	 * This method can be used to translate the backend answer regarding the current input source  to its corresponding
	 * symbolic value.
	 * @param str string that contains the backend answerl
	 * @param vall corresponding input 
	 * @return false if the translation could not be performed
	 */
	static bool decodeInput(const IRA::CString& str,TInputs& val);
	
	/**
	 * This method is used to encode an inputs type into the backend representation
	 * @param in input code to be translated
	 * @return a string that contains the backend representation of the given input
	 */
	static IRA::CString encodeInput(const TInputs& in);
	
	/**
	 * This method can be used to translate the backend answer regarding the configured bandWidth  to its corresponding
	 * double value (MHz)
	 * @param str string that contains the band width information
	 * @param val corresponding attenuation level
	 * @return false if the translation could not be performed
	 */	
	static bool decodeBandWidth(const IRA::CString& str,double& val);
	
	/**
	 * This method is used to encode bandwidth value in MHz  into the backend representation
	 * @param in bw double value to be translated
	 * @return a string that contains the backend representation of the given badn width
	 */
	static IRA::CString encodeBandWidth(const double& bw);
	
	/**
	 * This method returns the ACS timestamp relative to the backend reference time (01/01/1970)
	 * @param tm the first day of January 1970 as ACS timestamp.
	 */
	static void getReferenceTime(TIMEVALUE& tm);
	
};


#endif /*_PROTOCOL_H_*/
