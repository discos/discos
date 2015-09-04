#ifndef _COMMANDLINE_H_
#define _COMMANDLINE_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: CommandLine.h,v 1.23 2010/07/09 13:30:23 bliliana Exp $											*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                             		   			when            What                                */
/* Liliana Branciforti(bliliana@arcetri.astro.it) 04/08/2009 		Creation								*/

#include <IRA>
#include <ComponentErrors.h>
#include <BackendsErrors.h>
#include <XBackendsErrors.h>
#include "Configuration.h"
#include "Common.h"
#include <GroupSpectrometer.h>
#include <XBackendsS.h>
#include "XarcosThread.h"

/**
 * This class is inherited from the IRA::CSocket class. It takes charge of setting the configuration to thebackend. 
 *  if the remote side risconnects or a problem affects the comunication line, this class try to reconnect to the backend
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CCommandLine
{
friend class CSenderThread;
public:
	enum TLineStatus {
		NOTCNTD,   	        /*!< Socket is not connected, no operation available  */
		CNTDING,              /*!< Connection is in progress, no operation available */
		CNTD  		           /*!< Socket is connected and ready, line is ready */
	};
	enum TInputs {
		PRIMARY,
		BWG,
		GREGORIAN,
		OHM50
	};
	/**
	 * Constructor.
	*/
	CCommandLine(GroupSpectrometer *groupS);
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
	*/
	void InitConf(CConfiguration *config) throw (ComponentErrors::ValidationErrorExImpl);

	/**
	 * Call this function to get the inputNumber 
	 */
	inline void getInputsNumber(long &in) const { in=m_inputsNumber; }
	
	/**
	 * Call this function to get the sectionsNumber 
	 */
	inline void getSectionsNumber(long &in) const { in=m_sectionsNumber; }
	
	/**
	 * Call this function to get the milliseconds of integration time
	 */
	inline void getIntegration(long &integr) const { integr=m_integration; } 
	
	/**
	 * Call this function in order to get the start frequency value for each input. 
	 */
	void getFrequency(ACS::doubleSeq& freq) const;
	
	/**
	 * Call this function in order to get the sample rate for each input. 
	 */	
	void getSampleRate(ACS::doubleSeq& sr) const;

    /**
	 * Call this function in order to get the IF connected to each input.
	 */	
	void getIFs(ACS::longSeq& ifs) const;
	
	/**
	 * Call this function in order to know which section an input is attached to.
	 */
	void getInputSection(ACS::longSeq& inpSection) const;
	
	/**
	 * Call this function in order to get the number of bins for each input. 
	 */	
	void getBins(ACS::longSeq& bins) const;
	/**
	 * Call this function in order to get the number of bins for each input. 
	 */	
	void getFeed(ACS::longSeq& feed) const;	
    void setFeedZero();
	/**
	 * Call this function in order to get the polarization for each input. 
	 */	
	void getPolarization(ACS::longSeq& pol) const;
	
	/**
	 * Call this function in order to get the component status back
	 */
	void getBackendStatus(DWORD& status);	
	
	/**
	 * Call this function to obtain the exact configuration of each input of the backend in one shot 
	 */ 
	long getInputsConfiguration(ACS::doubleSeq& freq,ACS::doubleSeq& bandWidth,ACS::longSeq& feed,ACS::longSeq& ifs/*Receivers::TPolarizationSeq& polarization*/);

	/**
	 * Call this function in order to get the last measure of the system temperature.
	 */
	void getTsys(ACS::doubleSeq& tsys) const;
	
	void getSample(ACS::doubleSeq& tpi,bool zero) throw (ComponentErrors::TimeoutExImpl,
			BackendsErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,
			BackendsErrors::MalformedAnswerExImpl,BackendsErrors::BackendBusyExImpl);

	/**
	 * Call this function to know if the backend is busy or not. 
	 * This function is remapped on the BUSY field of the backed status
	 */
	bool getIsBusy() const { return m_busy; }
	
	/**
	 * Call this function to set busy the backend . 
	 */
	void SetBusy() {m_busy=true; }
	/**
	 * Call this function to reset busy the backend . 
	 */
	void ClearBusy() {m_busy=false; }
	
	/**
	 * Call this function to know if the backend HW Type is in used. 
	 */
	bool getModo8bit()const { return m_mode8bit;}
	
	/**
	 * Call this function in order to get the attenuation values for each input.
	 * @param att this is a sequence of double values that correspond to the attenuation for each analog input.
	 */
	void getAttenuation(ACS::doubleSeq& att) const;
	
	/**
	 * Call this function in order to get the current band width of each input.
	 * @param bw this is a sequence of double values that correspond to the bandwidth of each analog input.
	 */
	void getBandWidth(ACS::doubleSeq& bw) const;
	
	/**
	 * Call this function in order to get the time clock reported by the backend. 
	 * @param tt backend time, as numebr of 100 ns from 1582-10-15 00:00:00
	 */
	void getTime(ACS::Time& tt) const;
	
	/**
	 * This function must be called in order to change the integration time
	 * @throw  BackendsErrors::BackendBusyExImpl
	 * @param integration new integration time in milliseconds. A negative value has no effect.
	 */
	void setIntegration(const long& integration) throw (BackendsErrors::BackendBusyExImpl);
	
	/**
	 * This function must be called in order to change the Number of Section time
	 * @throw  BackendsErrors::BackendBusyExImpl
	 * @throw ComponentErrors::ValidationErrorExImpl
	 * @param sectionsNumber new Number of Section. A negative value has no effect.
	 */
	void setSectionsNumber(const long& sectionsNumber) 
		throw (BackendsErrors::BackendBusyExImpl,ComponentErrors::ValidationErrorExImpl);

	/**
	 * This function must be called in order to change the Number of Section time
	 * @throw  BackendsErrors::BackendBusyExImpl 
	 * @throw ComponentErrors::ValidationErrorExImpl
	 * @param inputsNumber new Number of Section. A negative value has no effect.
	 */
	void setInputsNumber(const long& inputsNumber)
		throw (BackendsErrors::BackendBusyExImpl,ComponentErrors::ValidationErrorExImpl);	
	
	/** 
	 * This methos is not implemented
	 * * @throw XBackendsErrors::NoImplementedExImpl
	 */ 
	void setEnabled(const ACS::longSeq& en) 
		throw (XBackendsErrors::NoImplementedExImpl);
	
	/**
	 * This function will upload the default configuration into the backend. 
	 * This method should be called at the component startup in order prevent the inconsistent 
	 * status of the backend when it not configured yet. 
	 * @throw XBackendsErrors::ErrorConfigurationExImpl
	 */
	void setDefaultConfiguration() 
		throw (XBackendsErrors::ErrorConfigurationExImpl);
	
	/**
	 * This function can be called in order to load an initial setup for the backend. Some parameter are fixed and cannot be changed during normal
	 * operation. In order for this call to suceed the backend must be ready and not busy with other operation. This operation will also reset all configuration
	 * to the defaults.
	 * @param BackendsErrors::BackendBusyExImpl
	 * @param BackendsErrors::ConfigurationErrorExImpl
	 * @param conf identifier of the configuration to be loaded
	 */ 
	void setup(const char *conf)  throw (BackendsErrors::BackendBusyExImpl,BackendsErrors::ConfigurationErrorExImpl);
	
	/**
	 * Called to configure the attenuation level for each input of the backend.
	 *  @throw BackendsErrors::BackendBusyExImpl
	 *  @throw ComponentErrors::ValidationErrorExImpl
	 *  @throw ComponentErrors::ValueOutofRangeExImpl
	 *	@throw BackendsErrors::ConnectionExImpl
	 *	@throw XBackendsErrors::ErrorConfigurationExImpl
	 * 	@param secId identifier of the section.
	 * 	@param attenuation new input level. It must lay between 0 and 50. 
	 * 		A negative will keep the previous value;
	 */
	void setAttenuation(const long&secId, const double& attenuation) 
		throw (ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::ValidationErrorExImpl,
			BackendsErrors::ConnectionExImpl,BackendsErrors::BackendBusyExImpl,XBackendsErrors::ErrorConfigurationExImpl);
 
	/**
	 * This function can be used to set up an input of the backend. The input is identified by its numeric value. If a configuration
	 * parameter is negative the current value is not changed. Since the backend hardware does not support different sample rates
	 * the last commanded sample rate is considered also for all other inputs.
	 * @throw BackendsErrors::ConnectionExImp
	 * @throw ComponentErrors::ValueOutOfRangeExImpl
	 * @throw ComponentErrors::ValidationErrorExImpl
	 * @throw BackendErrors::BackendBusyExImpl
	 * @throw XBackendsErrors::ErrorConfigurationExImpl 
	 * @param secId identifier of the section. If negative all the inputs are configured in one shot, otherwise 
	 *    it must be between 0 and the number of inputs.
	 * @param freq initial frequency of the input filter. 
	 * @param bw bandwidth of the filter in MHz. If all the inputs are involved cannot be negative (keep presetn value).
	 * @param sr  it can only be DEFAULT_SAMPLERATE
	 * @param feed indeicates the feeb  number the input is connected to. 
	 * @param pol polarization of the input, this is fixed by hardware and could not be changed so it is ignored.
	 * @param bins number of bins of the input, it can only be DEFAULT_BINS.
	 */	
	void setConfiguration(const long& secId,const double& freq,const double& bw,const long& fd,const long& pol, const double& sr,const long& bins) 
		throw (ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::ValidationErrorExImpl,
				BackendsErrors::ConnectionExImpl,BackendsErrors::BackendBusyExImpl,
				XBackendsErrors::ErrorConfigurationExImpl,XBackendsErrors::DisableChInExImpl);

    void setSection(const long& input,const double& freq,const double& bw,const long& feed,const long& pol, const double& sr,const long& bins);

	/**
	 * This function will start an acquisition job. The job will be created suspended and requires an explicit
	 * resum in order to begin the data flow. The backend will just connect to a specific socket.
	 * @throw BackendsErrors::ConnectionExImp
	 * @throw BackendErrors::BackendBusyExImpl
	 * @throw XBackendsErrors::NoSettingExImpl
	 */
	void startDataAcquisition() 
		throw (BackendsErrors::BackendBusyExImpl,BackendsErrors::ConnectionExImpl,
			XBackendsErrors::NoSettingExImpl);
	
	/**
	 * This function will stop the current acquisition job. 
	 * @throw BackendsErrors::ConnectionExImp
	 * @throw ComponentErrors::NotAllowedExImpl
	 * @throw XBackendsErrors::NoSettingExImpl
	 */	
	void stopDataAcquisition() 
		throw (BackendsErrors::ConnectionExImpl,ComponentErrors::NotAllowedExImpl,
			XBackendsErrors::NoSettingExImpl);
	
	/**
	 * This function will resume an acquisition job. This function will also check if the backend latency time is respected.
	 * if the time between the invocation of this method and <i>startDataAcquisition</i> is lesser than the latency the execution
	 * is suspended for the required time.  If the component is not suspended an exception is thrown.
	 * @throw BackendsErrors::ConnectionExImp
	 * @throw ComponentErrors::NotAllowedExImpl
	 * @throw XBackendsErrors::NoSettingExImpl
	 * @param startT epoch at which the acquisition is supposed to start
	 */
	void resumeDataAcquisition(const ACS::Time& startT) 
		throw (BackendsErrors::ConnectionExImpl,ComponentErrors::NotAllowedExImpl,
			XBackendsErrors::NoSettingExImpl);
	
	/**
	 * This function will suspend the data acquisition job.
	 * @throw BackendsErrors::ConnectionExImp
	 * @throw ComponentErrors::NotAllowedExImpl
	 * @throw XBackendsErrors::NoSettingExImpl
	 */
	void suspendDataAcquisition() 
		throw (BackendsErrors::ConnectionExImpl,ComponentErrors::NotAllowedExImpl,
			XBackendsErrors::NoSettingExImpl);
	
	/**
	 * Called to get a single measure of the total power for each in channel. The measure is done by the slow mode of the backend
	 * @throw ComponentErrors::SocketErrorExImpl
	 *		 @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::ConnectionExImp
	 * @throw BackendsErrors::NakExImpl
	 * @throw BackendsErrors::MalformedAnswerExImpl
	 * @throw BackendsErrors::BackendBusyExImpl
	 * @param integration integration time given in milliseconds
	 * @param tpi this is a sequence (allocated by the caller) of at least N positions, where N is the number of channels. Each element
	 *               of the vector cointains the total power of the corresponding channel.
	 * @param  zero if true the returned measure refers to a backend configuration in which the signal is coming from the 50 Ohm input. In that
	 *                 case the answer is delayed by an ammount of time that allows the signal to settled down.
	 * @param wait if true two time the integration time is waited before forwarding the request to the baceknd. Ignored if <i>zero</i> is set.
	 */ 
	void getSample(const long& integration,ACS::doubleSeq& tpi,bool zero,bool wait) throw (ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
	ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,BackendsErrors::MalformedAnswerExImpl,BackendsErrors::BackendBusyExImpl);
	
	/**
	 * Called to get the TPI measure when the attenuators are all in. Differently from <i>getSample()</i> it will return the counts returned
	 * by the backend for the configured sample rate.
	 * @throw ComponentErrors::SocketErrorExImpl
	 *		 @arg \c ComponentErrors::IRALibraryResource
	 * @throw ComponentErrors::TimeoutExImpl
	 * @throw BackendsErrors::ConnectionExImp
	 * @throw BackendsErrors::NakExImpl
	 * @throw BackendsErrors::MalformedAnswerExImpl
	 * @throw BackendsErrors::BackendBusyExImpl
	 * @param tpi array that contains for each element the tpi value for any input channel.
	 */
	void getTpZero(ACS::doubleSeq& tpiA, bool zero) throw (ComponentErrors::TimeoutExImpl,
			BackendsErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,
			BackendsErrors::MalformedAnswerExImpl,BackendsErrors::BackendBusyExImpl);

	/**
	 * Called by the component to fill the <i>Backends::TMainHeader</i> with the proper informations.
	 * @param bkd the stucture that contains the required information.
	 */
	void fillMainHeader(Backends::TMainHeader& bkd);
	
	/**
	 * Called to fill the sequence of the <i>Backends:channelHeader</i> with the proper information. Only the enabled channel
	 * are reported.
	 * @param chHr pointer to a (caller allocated) vector that will containes the information channel by channel
	 * @param size size of the input vector, used to avoid to exceed the vector limits
	 */
	void fillChannelHeader(Backends::TSectionHeader *chHr,const long& size);
	
	void setControlThread(XarcosThread * controlLoop);
	/**
	 * @throw  BackendsErrors::ConnectionExImpl
	 * @throw  BackendsErrors::BackendBusyExImpl
	 */
	void VisualizzaDato() throw (BackendsErrors::ConnectionExImpl,BackendsErrors::BackendBusyExImpl);
	
	/**
	 * @throw  BackendsErrors::ConnectionExImpl
	 * @throw  BackendsErrors::BackendBusyExImpl
	 */
	void VisualizzaSpecific() 
		throw (BackendsErrors::ConnectionExImpl,BackendsErrors::BackendBusyExImpl);
	/**
	 * @throw  BackendsErrors::ConnectionExImpl
	 * @throw  BackendsErrors::BackendBusyExImpl
	 */
	void VisualizzaACS() 
		throw (BackendsErrors::ConnectionExImpl,BackendsErrors::BackendBusyExImpl);
	/**
	 * @throw  BackendsErrors::ConnectionExImpl
	 * @throw  BackendsErrors::BackendBusyExImpl
	 * @throw  XBackendsErrors::NoSettingExImpl
	 */
	void Init()
		throw (BackendsErrors::ConnectionExImpl,BackendsErrors::ConnectionExImpl,XBackendsErrors::NoSettingExImpl);
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
	 * Called by the implementation of the interface to infor the component of the last tsys measurement.
	 * @param tsys suquence of doubles with the tsys for each input. 
	 */
	void saveTsys(const ACS::doubleSeq& tsys);
	
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
	
	/**
	 * reports the status of BUSY ACS attribute
	 */
	bool m_busy;
	
	XarcosThread * m_pcontrolLoop;

private:	
	/**
	 * List the fields of the backend status 
	 */   
	enum TstatusFields {
		HW=0,             /*!< backend time is connect  */
		SETTING=1,                  /*!< backend is setting */
		ACTIVE=2,				 /*!< backend is at work */
		ABORT=3,              /*!< backend is abort */
		DATAREADY=4,      	 /*!< backend is stop and data are ready */ 
		OVERFLOWDATA=5     		 /*!< flag data are overflow */
	};
	
	/** Connection status */
	TLineStatus m_Linestatus;
	/**This flag indicates if the socket has timedout */
	bool m_bTimedout;
	/** It contains error information */	
	CError m_Error;
	
	/** Component configuration data */
	CConfiguration *m_configuration;
	GroupSpectrometer *m_GroupSpectrometer;
	CSecureArea<GroupSpectrometer> *m_pLink;
	
	/**
	 * This mark the stat time fo the acquisition, used to check if the
	 * real transfer respect the backend initialization latency
	 */
	TIMEVALUE m_acquisitionStartEpoch;
	/** 
	 * attenuator value for each backend input
	*/
	double m_attenuation[MAX_INPUT_NUMBER];
	/**
	 *  bandwidth value for each backend input
	 */
	double m_bandWidth[MAX_SECTION_NUMBER];
	double m_bandWidthInput[MAX_SECTION_NUMBER];
	/**
	 * frequency value for each backend input
	 */
	double m_frequency[MAX_SECTION_NUMBER];
	double m_frequencyInput[MAX_SECTION_NUMBER];
	/**
	 * the sample rate associated to each input
	 */
	double m_sampleRate[MAX_SECTION_NUMBER];
	/**
	 * the polarization of each backend input 
	 */
	Backends::TPolarization m_polarization[MAX_SECTION_NUMBER];
	/**
	 * the number of bins that each input will deliver
	 */
	long m_bins[MAX_SECTION_NUMBER];
	/**
	 * Indicates if the input is enabled or not. A disabled input will not produce output data
	 */
	bool m_enabled[MAX_SECTION_NUMBER];
	/**
	 * indicates the feed  number the input is connected to. Configuration dependant
	 */
	long m_feedNumber[MAX_SECTION_NUMBER];
	long m_feedNumberInput[MAX_SECTION_NUMBER];
	/**
	 * indicates which section the input belongs to.
	 */
	long m_inputSection[MAX_INPUT_NUMBER];	
	
	/**
	 * Indicates if the adc is enabled or not. 
	 * A disabled input will not produce output data
	 * The values depend on the variable modo8bit
	 */
	bool m_adc[MAX_ADC_NUMBER];
	
	/**
	 * Indicates the mappinh hardware feed to ChIn
	 */
	long m_ChIn[MAX_ADC_NUMBER];
	long searchChIn(long feed);
	long searchFeed(long ChIn);
	/**
	 * This is a pattern variable that stoers the status of the component
	 */
	DWORD m_backendStatus;
	/**
	 * millisecond of integration
	 */
	long m_integration;
	/**
	 * reports for the number of inputs
	 */
	long m_inputsNumber;
	/**
	 * reports the number of sections
	*/
	long m_sectionsNumber;
	/**
	 * reports the number of feeds (in the current configuration) that the backend deal with
	 */
	long m_beams;
	/**
	 * reports the size of the samples in the current configuration
	 */
	long m_sampleSize;	
	/**
	 * It stores the last available tsys measurment 
	 */
	double m_tsys[MAX_INPUT_NUMBER];	

    /**
	 * Indicates the if number (relative to the feed) the input is attached to.
	 */
	long m_ifNumber[MAX_INPUT_NUMBER];

	/**
	 * indicates the bit number  
	 */
	bool m_mode8bit;
	/**
	 * input type for the initial configuration
	 */
	TInputs m_defaultInput;
//
//	/**
//	 * Represents the period of the cal diode switching, expressed as multiple of the sample period.
//	 * 0 means the switching is disable, 1 means that the cal diode is switched every sample period
//	 */
//	long m_calPeriod;
	/**
	 * Name of the host that run the component
	*/
	IRA::CString m_hostName;
	/**
	 * IP address of the host that run the component
	*/	
	IRA::CString m_hostAddress;

	/**
	 * It stores the last measure of zero TPI
	 */
	double m_tpiZero[MAX_INPUT_NUMBER];

	bool initializeConfiguration(const IRA::CString & config);
	
	/**
	 * Check if the connection is still up and ready. If a previuos call timed out it will check the status of the connection.
	 * Insted if the connection is off it will try a reconnect. In both case the return value is false.
	 * @return true if the connection is ok
	 */ 
	bool checkConnection();
	DWORD checkBackend();
	/** 
	 * Sets the antenna status.
	 * @param sta the new antenna status.
	*/
	void setStatus(TLineStatus sta);

    bool start;

	/**
	 * @return the current line status
	*/
	inline const TLineStatus& getStatus() const { return m_Linestatus; }
	
	/**
	 * This function will set the status bit corresponding to the given field 
	 */
	inline void setStatusField(TstatusFields field) { m_backendStatus |= 1 << field; }

	/**
	 * This function will unset (cleared) the status bit corresponding to the given field 
	 */
	inline void clearStatusField(TstatusFields field) { m_backendStatus &= ~(1 << field); }

public:
	/**
	 * This function is used to keep update the properties. At every request it checks if the last update is enough recent (update time not expired), 
	 * if not it takes charge of collecting new data. 
	 * @return a number greater than zero if the operation is succesfully, else FAIL in case of error, WOULDBLOCK if the timeout expires, 
	 *              0 if the remoteside disconnected or -100 is the answer of the backend is malformed
	*/
	int getConfiguration();
	void setMode8bit(bool mode)
		throw (BackendsErrors::BackendBusyExImpl,BackendsErrors::ConnectionExImpl);
	double gainToAttenuation(double gain){ return -20*log10(gain/MAX_GAIN); }
	double attenuationToGain(double attenuation){ return MAX_GAIN*exp10(-attenuation/20); }
    	void setMode8BitParameter(bool mode) {m_mode8bit=mode;}

	bool m_XarcosC;
	bool m_XarcosK00;
};

#endif /*_COMMANDLINE_H_*/

