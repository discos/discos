#ifndef _COMMANDLINE_H_
#define _COMMANDLINE_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: CommandLine.h,v 1.1 2011-03-14 15:16:22 a.orlati Exp $																								*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)     10/06/2010      Creation                                         */

#include <IRA>
#include <ComponentErrors.h>
#include <BackendsErrors.h>
#include <BackendsDefinitionsC.h>
#include <ReceiversDefinitionsC.h>
#include "Configuration.h"
#include "Common.h"

/**
 * This class takes charge of setting the configuration. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CCommandLine
{
friend class CSenderThread;
public:;
	/**
	 * Constructor.
	*/
	CCommandLine();
	/**
	 * Destructor
	*/
	virtual ~CCommandLine();
	
	/**
	 * This must be the first call before using any other function of this class.
	 * @param config pointer to the component configuration data structure
	 * @throw ComponentErrors::ValidationErrorExImpl
	*/
	void Init(CConfiguration *config) throw (ComponentErrors::ValidationErrorExImpl);
	
	/**
	 * Call this function to get the number of virtual inputs of the backend
	 */
	void getInputsNumber(long &in) { baci::ThreadSyncGuard guard(&m_mutex); in=m_inputsNumber; }
	
	/**
	 * Call this function to get the number of sections of the backend
	 */
	void getSectionsNumber(long &in) { baci::ThreadSyncGuard guard(&m_mutex); in=m_sectionsNumber; }
	
	/**
	 * Call this function to get the milliseconds of integration time
	 */
	void getIntegration(long &integr) { baci::ThreadSyncGuard guard(&m_mutex); integr=m_integration; }
	
	/**
	 * Call this function in order to get the start frequency value for each section. 
	 */
	void getFrequency(ACS::doubleSeq& freq);
	
	/**
	 * Call this function in order to get the sample rate for each section. 
	 */	
	void getSampleRate(ACS::doubleSeq& sr);
	
	/**
	 * Call this function in order to get the number of bins for each section. 
	 */	
	void getBins(ACS::longSeq& bins);
	
	/**
	 * Call this function in order to get the polarization for each section. 
	 */	
	void getPolarization(ACS::longSeq& pol);
	
	/**
	 * Call this function in order to get the component status back
	 */
	void getBackendStatus(DWORD& status);
	
	/**
	 * Call this function in order to get the feed connected to each section.
	 */
	void getFeed(ACS::longSeq& feed);
	
	/**
	 * Call this function in order to know which section an input is attached to.
	 */
	void getInputSection(ACS::longSeq& inpSection);
	
	/**
	 * Call this function to obtain the exact configuration of each input of the backend in one shot 
	 */ 
	long getInputsConfiguration(ACS::doubleSeq& freq,ACS::doubleSeq& bandWidth,ACS::longSeq& feed,ACS::longSeq& ifs);
	
	/**
	 * Call this function in order to get the last measure of the system temperature.
	 */
	void getTsys(ACS::doubleSeq& tsys);
	
	/**
	 * Call this function to know if the baceknd is busy or not. This function is remapped on the BUSY field of the backed status
	 */
	bool getIsBusy() const { return (m_backendStatus & (1 << BUSY)); }

	/**
	 * Call this function in order to get the attenuation values for each input.
	 * @param att this is a sequence of double values that correspond to the attenuation for each analog input.
	*/
	void getAttenuation(ACS::doubleSeq& att);
	
	/**
	 * Call this function in order to get the current band width of each input.
	 * @param bw this is a sequence of double values that correspond to the bandwidth of each section
	*/
	void getBandWidth(ACS::doubleSeq& bw);
	
	/**
	 * Call this function in order to get the time clock reported by the backend. 
	 * @param tt backend time, as numebr of 100 ns from 1582-10-15 00:00:00
	 */
	void getTime(ACS::Time& tt) const;
	
	/**
	 * It does nothing special.
	 */
	void setTime();
			
	/**
	 * This function must be called in order to change the integration time
	 * @throw  (BackendsErrors::BackendBusyExImpl
	 * @param integration new integration time in milliseconds. A negative value has no effect.
	 */
	void setIntegration(const long& integration) throw (BackendsErrors::BackendBusyExImpl);
	
	/**
	 * This methos will changes the current value of the <i>m_enabled</i> array.
	 * @throw BackendsErrors::BackendBusyExImpl
	 * @param en new values sequence for the <i>m_enabled</i> elements. A value grater than zero correspond to a true,
	 *                a zero match to a false, while a negative will keep the things unchanged.
	 */ 
	void setEnabled(const ACS::longSeq& en) throw (BackendsErrors::BackendBusyExImpl);
	
	/**
	 * Called to configure the attenuation level for each input of the backend.
	 *  @throw BackendsErrors::BackendBusyExImpl
	 *  @throw ComponentErrors::ValidationErrorExImpl
	 * @param identifier of the input.
	 * @param attenuation new input level. A negative will keep the previous value;
	 */
	void setAttenuation(const long&inputId, const double& attenuation) throw (BackendsErrors::BackendBusyExImpl,ComponentErrors::ValidationErrorExImpl);

	/**
	 * This function can be used to set up an input of the backend. The input is identified by its numeric value. If a configuration
	 * parameter is negative the current value is not changed. 
	 * @throw ComponentErrors::ValidationErrorExImpl
	 * @throw BackendErrors::BackendBusyExImpl
	 * @param sectId identifier of the section. it must be between 0 and the number of sectoins.
	 * @param freq initial frequency of the input filter. 
	 * @param bw bandwidth of the filter in MHz. .
	 * @param feed feed identifier.
	 * @param pol polarization of the input.
	 * @param sr new sample rate. 
	 * @param bins number of bins of the section.
	 */
	void setConfiguration(const long& sectId,const double& freq,const double& bw,const long& feed,const long& pol, const double& sr,const long& bins) throw (
			ComponentErrors::ValidationErrorExImpl,BackendsErrors::BackendBusyExImpl);
	
	/**
	 * This function will start an acquisition job. The job will be created suspended and requires an explicit
	 * resume in order to begin the data flow. 
	 * @throw BackendErrors::BackendBusyExImpl
	 */
	void startDataAcquisition() throw (BackendsErrors::BackendBusyExImpl);

	/**
	 * This function will stop the current acquisition job. 
	 * @throw ComponentErrors::NotAllowedExImpl
	 */	
	void stopDataAcquisition() throw (ComponentErrors::NotAllowedExImpl);
	
	/**
	 * This function will resume an acquisition job. 
	 * @throw ComponentErrors::NotAllowedExImpl
	 * @param startT epoch at which the acquisition is supposed to start
	 */
	void resumeDataAcquisition(const ACS::Time& startT) throw (ComponentErrors::NotAllowedExImpl);
	
	/**
	 * This function will suspend the data acquisition job.
	 * @throw ComponentErrors::NotAllowedExImpl
	 */
	void suspendDataAcquisition() throw (ComponentErrors::NotAllowedExImpl);
	
	/**
	 * Called to get a single measure of the total power for each in channel. The measure is done by the slow mode of the backend
	 * @param tpi this is a sequence (allocated by the caller) of at least N positions, where N is the number of inputs. Each element
	 *               of the sequence cointains the total power of the corresponding inputs.
	 * @param  zero if true the returned measure refers to a backend configuration in which the signal is coming from the 50 Ohm input.
	 */
	void getSample(ACS::doubleSeq& tpi,bool zero) const;
	
	/**
	 * Called by the component to fill the <i>Backends::TMainHeader</i> with the proper informations.
	 * @param bkd the stucture that contains the required information.
	 */
	void fillMainHeader(Backends::TMainHeader& bkd);
	
	/**
	 * Called to fill the sequence of the <i>Backends:channelHeader</i> with the proper information. Only the enabled channel
	 * are reported.
	 * @param chHr pointer to a (caller allocated) vector that will containes the information section by sectionl
	 * @param size size of the input vector, used to avoid to exceed the vector limits
	 */
	void fillChannelHeader(Backends::TSectionHeader *chHr,const long& size);
	
	/**
	 * Called by the implementation of the interface to infor the component of the last tsys measurement.
	 * @param tsys suquence of doubles with the tsys for each input. 
	 */
	void saveTsys(const ACS::doubleSeq& tsys);
	
private:
	/**
	 * List the fields of the backend status 
	 */
	enum TstatusFields {
		BUSY=0,                        /*!< backend is busy(transfer job initiated) and cannot accept other commands */
	};
	/** Component configuration data */
	CConfiguration *m_configuration;
	/**
	 * Thread sync mutex
	 */
	BACIMutex m_mutex;
	
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
	/**
	 * frequency value for each backend input
	 */
	double m_frequency[MAX_SECTION_NUMBER];
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

	/**
	 * indicates which section the input belongs to.
	 */
	long m_inputSection[MAX_INPUT_NUMBER];
	/**
	 * This is a pattern variable that soters the status of the component
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
	long m_feeds;
	/**
	 * reports the size of the samples in the current configuration
	 */
	long m_sampleSize;	
	/**
	 * It stores the last available tsys measurment 
	 */
	double m_tsys[MAX_INPUT_NUMBER];
	
	/**
	 * This method is called to set up the component to the default configuration. 
	 * @return false if the configuration fails
	 */ 
	bool initializeConfiguration();
			
	/**
	 * This function will set the status bit corresponding to the given field 
	 */
	inline void setStatusField(TstatusFields field) { m_backendStatus |= 1 << field; }

	/**
	 * This function will unset (cleared) the status bit corresponding to the given field 
	 */
	inline void clearStatusField(TstatusFields field) { m_backendStatus &= ~(1 << field); }
	
};

#endif /*_COMMANDLINE_H_*/
