#ifndef _COMMANDLINE_H_
#define _COMMANDLINE_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: CommandLine.h,v 1.1 2011-03-14 14:15:07 a.orlati Exp $																								*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)     01/10/2008      Creation                                         */

#include <IRA>
#include <ComponentErrors.h>
#include <BackendsErrors.h>
#include <RoachS.h>
#include <TotalPowerS.h>
#include <string>
#include <sstream>
#include <DiscosBackendProtocol>
#include "Common.h"
//#include "Protocol.h"
#include "Configuration.h"

using namespace maci;
using namespace DiscosBackend;
using namespace std;

/**
 * This class is inherited from the IRA::CSocket class. It takes charge of setting the configuration to the backend.
 *  if the remote side disconnects or a problem affects the communication line, this class try to reconnect to the backend
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CCommandLine: public CSocket
{
friend class CSenderThread;
public:
	enum TLineStatus {
		NOTCNTD,   	        /*!< Socket is not connected, no operation available  */
		CNTDING,              /*!< Connection is in progress, no operation available */
		CNTD  		           /*!< Socket is connected and ready, line is ready */
	};
	/**
	 * Constructor.
	*/
	CCommandLine(ContainerServices *service);
	/**
	 * Destructor
	*/
	virtual ~CCommandLine();
	
	/**
     * This member function is used to enstablish and configure the communication channel. 
     * The first connection is performed in blocking mode(if it fails the component fails to load), the the socket is
     * trasformed in non-blocking mode.
	 * This must be the first call before using any other function of this class.
	 * @param config pointer to the component configuration data structure
	 * @throw ComponentErrors::SocketError
	 * 		  @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::ValidationErrorExImpl
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::ConnectionExImpl
	 * @throw BackendsErrors::NakExImpl
	*/
	void Init(CConfiguration *config) throw (ComponentErrors::SocketErrorExImpl,
			ComponentErrors::ValidationErrorExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,BackendsErrors::NakExImpl,
			ComponentErrors::CDBAccessExImpl);
	
	/**
	 * Call this function to get the number of inputs
	 * In this implementation the inputs number is the same of the number of section (SIMPLE_SECTION)
	 */
	//inline void getInputsNumber(long &in) const { in=m_sectionsNumber; }
	inline void getInputsNumber(long &in) const { in=m_rfInputs; }
		
	/**
	 * Call this function to get the milliseconds of integration time
	 */
	void getIntegration(long &integr) const  { integr=m_integration;
		/*resultingSampleRate(m_integration,m_commonSampleRate,integr)*/;
	}
	
	/**
	 * Call this function in order to get the start frequency value for each section. 
	 */
	void getFrequency(ACS::doubleSeq& freq) const;
	
	/**
	 * Call this function in order to get the sample rate for each section. 
	 */	
	void getSampleRate(ACS::doubleSeq& sr) const;
	
	/**
	 * Call this function in order to get the number of bins for each section. 
	 */	
	void getBins(ACS::longSeq& bins) const;
	
	/**
	 * Call this function in order to get the polarization for each section. 
	 */	
	void getPolarization(ACS::longSeq& pol) const;
	
	/**
	 * Call this function in order to get the component status back
	 */
	void getBackendStatus(DWORD& status);
	
	/**
	 * Call this function in order to get the feed connected to each section.
	 */
	void getFeed(ACS::longSeq& feed) const;

	/**
	 * Call this function in order to get the IF connected to each input.
	 */	
	void getIFs(ACS::longSeq& ifs) const;
	
	/**
	 * Call this function in order to know which section an input is attached to.
	 */
	void getInputSection(ACS::longSeq& inpSection) const;
	
	/**
	 * Call this function in order to get the last measure of the system temperature.
	 */
	void getTsys(ACS::doubleSeq& tsys) const;
	
	/**
	 * Call this function in order to get the last measure of the Kelvin/Counts ratio.
	 */
	void getKCRatio(ACS::doubleSeq& ratio) const;
		
	/**
	 * Call this function to know if the baceknd is busy or not. This function is remapped on the BUSY field of the backed status
	 */
	bool getIsBusy() const { return (m_backendStatus & (1 << BUSY)); }

	/**
	 * Call this function in order to get the attenuation values for each input.
	 * @param att this is a sequence of double values that correspond to the attenuation for each analog input.
	 * @throw ComponentErrors::SocketErrorExImpl
	 *		 @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::MalformedAnswerExImpl
	 * @throw BackendsErrors::ConnectionExImp
	 */
	void getAttenuation(ACS::doubleSeq& att) throw (ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,
			BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ConnectionExImpl);
	
	/**
	 * Call this function in order to get the current band width of each input.
	 * @param bw this is a sequence of double values that correspond to the bandwidth of each analog input.
	 * @throw ComponentErrors::SocketErrorExImpl
	 *		 @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::MalformedAnswerExImpl
	 * @throw BackendsErrors::ConnectionExImp
	 */
	void getBandWidth(ACS::doubleSeq& bw) throw (ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,
			BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ConnectionExImpl);
	
	/**
	 * Call this function in order to get the time clock reported by the backend. 
	 * @param tt backend time, as numebr of 100 ns from 1582-10-15 00:00:00
	 * @throw ComponentErrors::SocketErrorExImpl
	 *		 @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::MalformedAnswerExImpl
	 * @throw BackendsErrors::ConnectionExImp
	 */
	void getTime(ACS::Time& tt) throw (ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,
			BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ConnectionExImpl);
	
	/**
	 * Call this function in order to set the time of the backend FPGA. The time will be set according to the local computer
	 * time.
	 @throw ComponentErrors::SocketErrorExImpl
	 *		 @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::ConnectionExImp
	 * @throw BackendsErrors::NakExImpl
	 * @throw BackendsErrors::MalformedAnswerExImpl
	 * @throw BackendsErrors::BackendBusyExImpl
	 */
	void setTime()  throw (ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
			ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,BackendsErrors::MalformedAnswerExImpl,
			BackendsErrors::BackendBusyExImpl);
	
	/**
	 * This function is called by the control thread in order to check the beackend clock matches with the host computer clock. In the two times do not
	 * match a log file in sent and the TIME_SYNC flag is set.
	 * @throw ComponentErrors::SocketErrorExImpl
	 *		 @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::ConnectionExImp
	 * @throw BackendsErrors::MalformedAnswerExImpl
	*/
	void checkTime() throw (BackendsErrors::ConnectionExImpl,BackendsErrors::MalformedAnswerExImpl,
			ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);
	
	/**
	 * This method starts the calibration diode switching.
	 * @throw BackendsErrors::BackendBusyExImpl
	 * @thorw ,ComponentErrors::NotAllowedExImpl
	 * @param interleave this will control the switcing mode. An integer positive(N) will command the backend to produce N samples
	 *               when the calibration diode is turned off and then one with the calibration diode turned on. A zero disable the switching
	 *               (default), whilst a negative will not change the current configuration   
	 */
	void activateCalSwitching(const long& interleave) throw (BackendsErrors::BackendBusyExImpl,ComponentErrors::NotAllowedExImpl);
	
	/**
	 * This function must be called in order to change the integration time
	 * @throw  (BackendsErrors::BackendBusyExImpl
	 * @param integration new integration time in milliseconds. A negative value has no effect.
	 */
	void setIntegration(const long& integration) throw (BackendsErrors::BackendBusyExImpl, ComponentErrors::ValueOutofRangeExImpl);
	
	/**
	 * This methos will changes the current value of the <i>m_enabled</i> array.
	 * @throw BackendsErrors::BackendBusyExImpl
	 * @param en new values sequence for the <i>m_enabled</i> elements. A value grater than zero correspond to a true,
	 *                a zero match to a false, while a negative will keep the things unchanged.
	 */ 
	void setEnabled(const ACS::longSeq& en) throw (BackendsErrors::BackendBusyExImpl);
	
	/**
	 * This function can be called in order to load an initial setup for the backend. Some parameter are fixed and cannot be changed during normal
	 * operation. In order for this call to suceed the backend must be ready and not busy with other operation. This operation will also reset all configuration
	 * to the defaults.
	 * @throw BackendsErrors::BackendBusyExImpl
	 * @throw BackendsErrors::ConfigurationErrorExImpl
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::ConnectionExImpl
	 * @throw ComponentErrors::SocketErrorExImpl
	 * @throw BackendsErrors::NakExImpl
	 * @param conf identifier of the configuration to be loaded
	 */ 
	void setup(const char *conf) throw (BackendsErrors::BackendBusyExImpl,BackendsErrors::ConfigurationErrorExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl, ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,ComponentErrors::CDBAccessExImpl,BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ReplyNotValidExImpl,BackendsErrors::BackendFailExImpl);
	
	void setTargetFileName(const char *conf);
	
    void sendTargetFileName() throw (BackendsErrors::BackendBusyExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl, ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl);

	/**
	 * This function can be used to set up an input of the backend. The input is identified by its numeric value. If a configuration
	 * parameter is negative the current value is not changed. Since the backend hardware does not support different sample rates
	 * the last commanded sample rate is considered also for all other inputs.
	 * @throw ComponentErrors::SocketErrorExImpl
	 *		 @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::ConnectionExImp
	 * @throw BackendsErrors::NakExImpl
	 * @throw ComponentErrors::ValueOutOfRangeExImpl
	 * @throw ComponentErrors::ValidationErrorExImpl
	 * @throw BackendErrors::BackendBusyExImpl
	 * @param inputId identifier of the input. If negative all the inputs are configured in one shot, otherwise 
	 *    it must be between 0 and the number of inputs.
	 * @param freq initial frequency of the input filter. This parameter is ignored. It is expected a negative, otherwise a warning message is logged.
	 * @param bw bandwidth of the filter in MHz. .
	 * @param feed feed identifier, this parameter is ignored. It is expected a negative, otherwise a warning message is logged.
	 * @param pol polarization of the input, this is fixed by hardware and could not be changed so it is ignored. It is expected a negative, otherwise a warning message is logged.
	 * @param sr new sample rate, the maximum allowed value is 0.001 MHz. 
	 * @param bins number of bins of the input, it can only be 1, so it is ignored. It is expected a negative, otherwise a warning message is logged.
	 */
	void setConfiguration(const long& inputId,const double& freq,const double& bw,const long& feed,const long& pol, const double& sr,const long& bins) throw (
			ComponentErrors::ValidationErrorExImpl,BackendsErrors::ConnectionExImpl,BackendsErrors::NakExImpl,
			ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,ComponentErrors::ValueOutofRangeExImpl,
			BackendsErrors::BackendBusyExImpl);
	
	/**
	 * This function will start an acquisition job. The job will be created suspended and requires an explicit
	 * resum in order to begin the data flow. The backend will just connect to a specific socket.
	 * @throw ComponentErrors::SocketErrorExImpl
	 *		 @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::ConnectionExImp
	 * @throw BackendsErrors::NakExImpl
	 * @throw BackendErrors::BackendBusyExImpl
	 */
	void startDataAcquisition() throw (BackendsErrors::BackendBusyExImpl,BackendsErrors::ConnectionExImpl,
			BackendsErrors::NakExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl);

	/**
	 * This function will stop the current acquisition job. 
	 * @throw ComponentErrors::SocketErrorExImpl
	 *		 @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::ConnectionExImp
	 * @throw BackendsErrors::NakExImpl
	 * @throw ComponentErrors::NotAllowedExImpl
	 */	
	void stopDataAcquisition() throw (BackendsErrors::ConnectionExImpl,BackendsErrors::NakExImpl,
			ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,ComponentErrors::NotAllowedExImpl,BackendsErrors::BackendFailExImpl);
	
	/**
	 * This function will stop the current acquisition job without caring about the component or the backend status
	 * @throw ComponentErrors::SocketErrorExImpl
	 *		 @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::ConnectionExImp
	 * @throw BackendsErrors::NakExImpl
	 * @throw ComponentErrors::NotAllowedExImpl
	 */		
	void stopDataAcquisitionForced() throw (BackendsErrors::ConnectionExImpl,BackendsErrors::NakExImpl,
			ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,ComponentErrors::NotAllowedExImpl);
	
	/**
	 * This function will resume an acquisition job. This function will also check if the backend latency time is respected.
	 * if the time between the invocation of this method and <i>startDataAcquisition</i> is lesser than the latency the execution
	 * is suspended for the required time.  If the component is not suspended an exception is thrown.
	 * @throw ComponentErrors::SocketErrorExImpl
	 *		 @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::ConnectionExImp
	 * @throw BackendsErrors::NakExImpl
	 * @throw ComponentErrors::NotAllowedExImpl
	 * @param startT epoch at which the acquisition is supposed to start
	 * @return the epoch of the real expected start time.
	 */
	ACS::Time resumeDataAcquisition(const ACS::Time& startT) throw (BackendsErrors::ConnectionExImpl,ComponentErrors::NotAllowedExImpl,
		BackendsErrors::NakExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::BackendFailExImpl);
	
	/**
	 * This function will suspend the data acquisition job.
	 * @throw ComponentErrors::SocketErrorExImpl
	 *		 @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::ConnectionExImpl
	 * @throw BackendsErrors::NakExImpl
	 * @throw ComponentErrors::NotAllowedExImpl
	 */
	void suspendDataAcquisition() throw (BackendsErrors::ConnectionExImpl,ComponentErrors::NotAllowedExImpl,
			BackendsErrors::NakExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::BackendFailExImpl);
	
	/**
	 * Called to get a single measure of the total power for each in channel. The measure is done by the slow mode of the backend
	 * @throw ComponentErrors::SocketErrorExImpl
	 *		 @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::ConnectionExImp
	 * @throw BackendsErrors::NakExImpl
	 * @throw BackendsErrors::MalformedAnswerExImpl
	 * @throw BackendsErrors::BackendBusyExImpl
	 * @param tpi this is a sequence (allocated by the caller) of at least N positions, where N is the number of channels. Each element
	 *               of the vector cointains the total power of the corresponding channel.
	 * @param  zero if true the returned measure refers to a backend configuration in which the signal is coming from the 50 Ohm input. In that
	 *                 case the answer is delayed by an ammount of time that allows the signal to settled down.
	 */ 
	void getSample(ACS::doubleSeq& tpi,bool zero) throw (ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
	ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,BackendsErrors::MalformedAnswerExImpl,BackendsErrors::BackendBusyExImpl);
	
	/**
	 * This a wrapper function of the <i>getSample()</i> method. In this case the sample correspond th the total power measurment in each channel.
	 * @throw ComponentErrors::SocketErrorExImpl
	 *		 @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::ConnectionExImp
	 * @throw BackendsErrors::NakExImpl
	 * @throw BackendsErrors::MalformedAnswerExImpl
	 * @throw BackendsErrors::BackendBusyExImpl
	 * @param tpi this is a sequence (allocated by the caller) of at least N positions, where N is the number of channels. Each element
	 *               of the vector cointains the total power of the corresponding channel. 
	 */ 
	void getTpi(ACS::doubleSeq& tpi) throw (ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
			ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,BackendsErrors::MalformedAnswerExImpl,BackendsErrors::BackendBusyExImpl);

	
	/**
	 * This a wrapper function of the <i>getSample()</i> method. In this case the sample correspond the power measurment in each channel with full attenuation.
	 * @throw ComponentErrors::SocketErrorExImpl
	 *		 @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::ConnectionExImp
	 * @throw BackendsErrors::NakExImpl
	 * @throw BackendsErrors::MalformedAnswerExImpl
	 * @throw BackendsErrors::BackendBusyExImpl
	 * @param tpi this is a sequence (allocated by the caller) of at least N positions, where N is the number of channels. Each element
	 *               of the vector cointains the total power of the corresponding channel. 
	 */ 	
	void getZero(ACS::doubleSeq& tpi) throw (ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
			ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,BackendsErrors::MalformedAnswerExImpl,BackendsErrors::BackendBusyExImpl);

	
	/**
	 * Called to get the TPI measure when the attenuators are all in. Differently from <i>getSample()</i> it will return the counts returned
	 * by the backend for the configured sample rate. The measure also is not normalized for the integration time.
	 * @throw ComponentErrors::SocketErrorExImpl
	 *		 @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::ConnectionExImp
	 * @throw BackendsErrors::NakExImpl
	 * @throw BackendsErrors::MalformedAnswerExImpl
	 * @throw BackendsErrors::BackendBusyExImpl
	 * @param tpi array that contains for each element the tpi value for any input channel.
	 */
	void getZeroTPI(DWORD *tpi) throw (ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
			ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,BackendsErrors::MalformedAnswerExImpl,
			BackendsErrors::BackendBusyExImpl);
	
	/**
	 * Called to configure the attenuation level for each input of the backend.
	 *  @throw BackendsErrors::BackendBusyExImpl
	 *  @throw ComponentErrors::ValidationErrorExImpl
	 *  @throw ComponentErrors::ValueOutofRangeExImpl
	 *  @throw BackendsErrors::NakExImpl,
	 *	@throw ComponentErrors::SocketErrorExImpl
	 *		 @arg \c ComponentErrors::IRALibraryResource
	 *	@throw ComponentErrors::TimeoutExImpl
	 *	@throw BackendsErrors::ConnectionExImpl
	 * @param identifier of the input.
	 * @param attenuation new input level. It must lay between 0 and 15. A negative will keep the previous value;
	 */
	void setAttenuation(const long&inputId, const double& attenuation)  throw (BackendsErrors::BackendBusyExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,BackendsErrors::NakExImpl,
			ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl);
	
	/**
	 * Called by the component to fill the <i>Backends::TMainHeader</i> with the proper informations.
	 * @param bkd the stucture that contains the required information.
	 */
	void fillMainHeader(Backends::TMainHeader& bkd);
	
	/**
	 * Called to fill the sequence of the <i>Backends:channelHeader</i> with the proper information. Only the enabled channel
	 * are reported.
	 * @param chHr pointer to a (caller allocated) vector that will contains the information section by section
	 * @param size size of the input vector, used to avoid to exceed the vector limits
	 */
	void fillChannelHeader(Backends::TSectionHeader *chHr,const long& size);
	
	/**
	 * Called by the implementation of the interface to inform the component of the last tsys measurement and the Kelvin/counts ratio.
	 * @param tsys sequence of doubles with the tsys for each input.
	 * @param ration ratio between Kelvin and counts as determined during the last tsys measurement
	 */
	void saveTsys(const ACS::doubleSeq& tsys,const ACS::doubleSeq& ratio);
	
	/**
	 * return the array  that contains the mapping of sections over boards.
	 */
	long *boardsMapping() { return m_boards; }

	long sectionNumber() { return m_sectionsNumber; }

	/**
	 * Computes the resulting integration time considering the sample rate and the integration time.
	 * @param integration current integration time (msec)
	 * @param sr current sample rate (MHz)
	 * @param result integration time resulting from the two parameters
	 * @return true if the resulting integration has to be rounded to match the two input parameters
	 */
	static bool resultingSampleRate(const long& integration,const double& sr,long& result);

	//int getConfiguration(char* configuration);
	void getConfiguration(char* configuration);

	int getCommProtVersion(CORBA::String_out version);

    IRA::CString m_targetFileName;

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

    ContainerServices* m_services;
    Backends::TotalPower_var m_totalPower;

	/**
	 * List the fields of the backend status 
	 */
	enum TstatusFields {
		TIME_SYNC=0,             /*!< backend time not sync  */
		BUSY=1,                        /*!< backend is busy(transfer job initiated) and cannot accept other commands */
		SUSPEND=2,				  /*!< backend data flow is suspended */
		SAMPLING=3,              /*!< backend is recording */
		CMDLINERROR=4,      /*!< error in the command line */ 
		DATALINERROR=5     /*!< error in the data line */
	};
	/** Connection status */
	TLineStatus m_status;
	/**This flag indicates if the socket has timedout */
	bool m_bTimedout;
	bool m_reiniting;
	/** It contains error information */	
	CError m_Error;
	/** Component configuration data */
	CConfiguration *m_configuration;
	/** This is the epoch of the last update from the backend */
	TIMEVALUE m_lastUpdate;
	/**
	 * This mark the stat time fo the acquisition, used to check if the
	 * real transfer respect the backend initialization latency
	 */
	TIMEVALUE m_acquisitionStartEpoch;
	/** 
	 * attenuator value for each backend input
	*/
	double m_attenuation[MAX_SECTION_NUMBER];
	/**
	 *  bandwidth value for each backend input
	 */
	double m_bandWidth[MAX_SECTION_NUMBER];
	/**
	 * frequency value for each backend input
	 */
	double m_frequency[MAX_SECTION_NUMBER];
	/**
	 * the sample rate associated to each input
	 */
	double m_sampleRate[MAX_SECTION_NUMBER];
	/**
	 * The sample rate currently configured in the backend, given as sample period in milliseconds
	 */
	long m_currentSampleRate;
	/**
	 * Since the backend is not capable to have different sample rate this is used for common
	 */
	double m_commonSampleRate;
	/**
	 * allows to link a section to its proper board number
	 */
	long m_boards[MAX_SECTION_NUMBER];

	/**
	 * Allows to link a board number to its section
	 */
	long m_sections[MAX_BOARDS_NUMBER];
	/**
	 * the polarization of each backend input 
	 */
	Backends::TPolarization m_polarization[MAX_SECTION_NUMBER];
	/**
	 * the number of bins that each input will deliver
	 */
	long m_bins[MAX_SECTION_NUMBER];
	/**
	 * input type for each backend section
	 */
	//CProtocol::TInputs m_input[MAX_SECTION_NUMBER];
	/**
	 * Indicates if the input is enabled or not. A disabled input will not produce output data
	 */
	bool m_enabled[MAX_SECTION_NUMBER];
	/**
	 * indicates the feed  number the input is connected to. Configuration dependent
	 */
	long m_feedNumber[MAX_SECTION_NUMBER];
	/**
	 * Indicates the if number (relative to the feed) the input is attached to.
	 */
	long m_ifNumber[MAX_SECTION_NUMBER];
	/**
	 * indicates which section the input belongs to.
	 */
	long m_inputSection[MAX_SECTION_NUMBER];
	/**
	 * input type for the initial configuration
	 */
	//CProtocol::TInputs m_defaultInput[MAX_BOARDS_NUMBER];
	WORD m_defaultInputSize;
	/**
	 * That's the clock of the backend. Is has the precision of one second
	 */
	TIMEVALUE m_backendTime;
	/**
	 * This is a pattern variable that soters the status of the component
	 */
	DWORD m_backendStatus;
	/**
	 * millisecond of integration
	 */
	long m_integration;
	/**
	 * reports for the number of inputs, for this implementation it happens that number of inputs is equal to the number of sections.
	 */
	long m_sectionsNumber;
	
	/**
	 * reports the number of beams (in the current configuration) that the backend deal with
	 */
	long m_beams;
	
	/**
	 * if true indicates that the connected receiver supports the fast calibration mark switching. It can only be changed
	 * by selecting a configuration (@sa <i>initializeConfiguration()</i>
	 */
	bool m_calSwitchingEnabled;
	
	/**
	 * reports the size of the samples in the current configuration
	 */
	long m_sampleSize;
	/**
	 * Represents the period of the cal diode switching, expressed as multiple of the sample period.
	 * 0 means the switching is disable, 1 means that the cal diode is switched every sample period
	 */
	long m_calPeriod;
	/**
	 * The configuration of the integration time when a tpi measure is done is required the first time this measure is done and every time the integration time changes. It is also required
	 * when the backend has started and completed an acquisition job. This flag keep track of when the initialization must be done (true).
	 */
	bool m_setTpiIntegration;
	/**
	 * Name of the host that run the component
	*/
	IRA::CString m_hostName;
	/**
	 * IP address of the host that run the component
	*/	
	IRA::CString m_hostAddress;
	
	/**
	 * It stores the last available tsys measurement
	 */
	double m_tsys[MAX_SECTION_NUMBER];
	
	/**
	 * It stores the last available ration Kelvin/counts
	 */
	double m_KCratio[MAX_SECTION_NUMBER];
	
	/**
	 * It stores the last available total power measurement
	 */
	//double m_tpi[MAX_SECTION_NUMBER];
	
	/**
	 * It stores the last measure of zero TPI
	 */
	double m_tpiZero[MAX_SECTION_NUMBER];

    long m_rfInputs;

    double m_filter;
	
	/**
	 * Pointer to the configuration table, every record of the table stores a possible backend setup.
	 */
	IRA::CDBTable * m_configTable;
	
	/**
	 * This function will upload the default configuration into the backend. This method should be called at the component startup
	 * in order prevent the inconsistent status of the backend when it not configured yet. The deafult configuration is BWG, 8db
	 * and 2000Mhz bandwidth. The integration time is also loaded.
	 * @throw ComponentErrors::SocketErrorExImpl
	 *		 @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::ConnectionExImp
	 * @throw BackendsErrors::NakExImpl
	 */
	void setDefaultConfiguration(const IRA::CString & config) throw (ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
			ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ReplyNotValidExImpl,BackendsErrors::BackendFailExImpl);
	
	/**
	 * This method is called to set up the component in order to get the desired configuration. Defaults  and
	 * hard coded values are set according to the configuration.
	 * @param config name of the configuration to be loaded
	 * @return false if the configuration name is not coded or known
	 */ 
	bool initializeConfiguration(const IRA::CString & config) throw (ComponentErrors::CDBAccessExImpl);
	
	/**
	 * Check if the connection is still up and ready. If a previuos call timed out it will check the status of the connection.
	 * Insted if the connection is off it will try a reconnect. In both case the return value is false.
	 * @return true if the connection is ok
	 */ 
	bool checkConnection();
	
	/** 
	 * Sets the antenna status.
	 * @param sta the new antenna status.
	*/
	void setStatus(TLineStatus sta);

	/**
	 * @return the current line status
	*/
	inline const TLineStatus& getStatus() const { return m_status; }

	/**
    * This member function is called to send a command to the backend
	 * @param Msg ponter to the byte buffer that contains the message
	 * @param Len length of the buffer
	 * @return SUCCESS if the buffer was sent correctly, WOULDBLOCK if the send would block, FAIL and the <i>m_Error</i> member is set accordingly.
	*/
	OperationResult sendBuffer(char *Msg,WORD Len);
	
	/**
    * This member function is called to receive a buffer. if it realizes that the remote side disconnected it sets the member <i>m_Status</i> 
	 * to NOTCNTD.
	 * @param Msg ponter to the byte buffer that will contain the answer
	 * @param Len length of the buffer, that means the exact number of bytes that can be read
	 * @return the number of bytes read, 0 if the connection fell down, FAIL in case of error and the <i>m_Error</i> member is set accordingly, 
	 *         WOULDBLOCK if the timeout expires
	*/
	int receiveBuffer(char *Msg,WORD Len);
	
	/**
	 * This private member function is used to send a command to the connected device. It waits for the answer.
	 * @param Buff pointer to the buffer that contains the message to send to the device, on exit it will cointains the aswer.
	 * @param inLen lenght of the input buffer.
	 * @param outBuff pointer to the buffer that contains the message from the other side as answer to the sent message.
	 * @return a positive number if the command was correctly received by the other side, else -1 in case of error, -2 if the timeout expires
	*/
	int sendCommand(char *inBuff,const WORD& inLen,char *outBuff);

    /**
     * Sends a message to the backend and return the message reply.
     * @throws BackendProtocolError
    */
    Message sendBackendCommand(Message request);
	
	/**
	 * This function is used to keep update the properties. At every request it checks if the last update is enough recent (update time not expired), 
	 * if not it takes charge of collecting new data. 
	 * @return a number greater than zero if the operation is succesfully, else FAIL in case of error, WOULDBLOCK if the timeout expires, 
	 *              0 if the remoteside disconnected or -100 is the answer of the backend is malformed
	*/
	//int getConfiguration(char* configuration);
	
	/**
	 * This function will set the status bit corresponding to the given field 
	 */
	inline void setStatusField(TstatusFields field) { m_backendStatus |= 1 << field; }

	/**
	 * This function will unset (cleared) the status bit corresponding to the given field 
	 */
	inline void clearStatusField(TstatusFields field) { m_backendStatus &= ~(1 << field); }

    bool m_RK00;
    bool m_RC00;
    bool m_RL00;
    bool m_RP00;
    bool m_RK00S;
    bool m_RC00S;
    bool m_RL00S;
    bool m_RP00S;
	
};

#endif /*_COMMANDLINE_H_*/
