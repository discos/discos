#ifndef CCONFIGURATION_H
#define CCONFIGURATION_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 02/08/2011     Creation                                         */


#include <IRA>
#include <maciContainerServices.h>
#include <ComponentErrors.h>
#include <ReceiversDefinitionsC.h>
#include <map>
#include <vector>
#include "Commons.h"
#include "ReceiverConfHandler.h"

// Temperature dummy value to use in case of Connection Error
#define CEDUMMY 100000


/**
 * This class implements the component configuration. The data inside this class are initialized at the startup from the
 * configuration database and then are used (read) inside the component.
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,v
 * @author <a href=mailto:matteo.debiaggi@inaf.it>Matteo De Biaggi</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
  */
class CConfiguration {
public:

	/**
	 * Default constructor
	 */
	CConfiguration();

	/**
	 * Destructor
	 */
	~CConfiguration();

	/* *** COMPONENT *** */
	/**
    *  @brief This member function is used to configure component by reading the configuration parameter from the CDB.
	 * @details This must be the first call before using any other function of this class.
	 * 			This call applies CCC_Normal configuration as default
	 * @throw ComponentErrors::CDBAccess
	 * @param Services pointer to the container services object
	*/
	void init(maci::ContainerServices *Services)  throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl);

	/* *** NOT CONFIGURATION RELATED PARAMS *** */

	/**
	 * @return the IP address of the board that controls the dewar
	 */
	inline const IRA::CString& getDewarIPAddress() const { return m_dewarIPAddress; }

	/**
	 * @return the IP port of the board that controls the dewar
	 */
	inline const WORD& getDewarPort() const { return m_dewarPort; }

	/**
	 * @return the IP address of the board that controls the receiver LNA
	 */
	inline const IRA::CString& getLNAIPAddress() const { return m_LNAIPAddress; }

	/**
	 * @return the IP port of the board that controls the receiver LNA
	 */
	inline const WORD& getLNAPort() const { return m_LNAPort; }

	/**
	 * @return the time allowed to the watch dog thread to complete an iteration and respond to the thread manager (microseconds)
	 */
	inline const DDWORD& getWarchDogResponseTime() const { return m_watchDogResponseTime; }

	/**
	 * @return the time between two execution of the watch dog thread (microseconds)
	 */
	inline const DDWORD& getWatchDogSleepTime() const { return m_watchDogSleepTime; }

	/**
	 * @return the time required by the board to sample the values of LNA (microseconds)
	 */
	inline const DDWORD& getLNASamplingTime() const { return m_LNASamplingTime; }

	/**
	 * @return the time the repetition log guard will cache new log messages before sending to the central logger (microseconds)
	 */
	inline const DDWORD& getRepetitionCacheTime() const { return m_repetitionCacheTime; }

	/**
	 * @return the time of expiration of a log sent to the repetition log guard (microseconds)
	 */
	inline const DDWORD& getRepetitionExpireTime() const { return m_repetitionExpireTime; }

	/* *** LO INSTANCES *** */

	/**
	 * @return the instance of the local oscillator component name to drive 1st stage LO
	 */
	 IRA::CString getLocalOscillatorInstance1st() const { return m_localOscillatorInstance1st; }

	/**
	 * @return the instance of the local oscillator component name to drive 2nd stage LO
	 */
	IRA::CString getLocalOscillatorInstance2nd() const { return m_localOscillatorInstance2nd; }

	/* **** TABLES **** */

	/**
	 * @brief Get the Left pol. Noise Mark Coeffs c array
	 * 
	 * @param[out] p_out_coeffs Coeff c array
	 * @return Coeff Array len.
	 */
	DWORD getLeftMarkCoeffs( double *& p_out_coeffs) ;

	/**
	 * @brief Get the Right pol. noise Mark Coeffs c array
	 * 
	 * @param[out] p_out_coeffs Coeff c array	 
	 * @return Coeff Array len.
	 */
	DWORD getRightMarkCoeffs(double *& p_out_coeffs) ;

	/**
	 * @brief Get mark temp at given freq ( left )
	 * @details Calculation depends on receiver configuration
	 * 
	 * @param freq Input frequency
	 * @return double Cal mark value
	 */
	double getLeftMarkTemp(double freq);

	/**
	 * @brief Get mark temp at given freq ( right )
	 * @details Calculation depends on receiver configuration
	 * 
	 * @param freq Input frequency
	 * @return double Cal mark value
	 */
	double getRightMarkTemp(double freq);

	/**
	 * @param freq vector with the synthesizer frequencies. It must be freed by caller.
	 * @param power corresponding powers for the frequencies vector. It must be freed by caller.
	 * @return the size of the output vectors
	 */
	DWORD getSynthesizerTable(double *& freq,double *& power) const;

	/**
	 * @param freq vector with the frequencies. It must be freed by caller.
	 * @param taper corresponding taper  for the frequencies vector. It must be freed by caller.
	 * @return the size of the output vectors
	 */
	DWORD getTaperTable(double * &freq,double *&taper) const;

	/**
	 * @param code feed identification codes. It must be freed by caller.
	 * @param xOffset displacement of the feed with respect to the central one along x axis. It must be freed by caller.
	  *@param yOffset displacement of the feed with respect to the central one along y axis. It must be freed by caller.
	  *@param relativePower expected percentage of variation of gain with respect to the central one. It must be freed by caller.
	 * @return the size of the output vectors
	 */
	DWORD getFeedInfo(WORD *& code,double *& xOffset,double *& yOffset,double *& relativePower) const;

	/* *** SINGLE SETUP ENTRIES  *** */

	/**
	 * @return mnemonic of the working mode of the receiver
	 */
	inline const IRA::CString& getSetupMode()  { return m_conf_hnd.getCurrentSetup().m_name; }	

	/**
	* @brief Getter whole current setup
	*/
	ReceiverConfHandler::ConfigurationSetup getCurrentSetup(){ return m_conf_hnd.getCurrentSetup(); }

	/**
	 * @return the lower limit of the RF coming from the receiver (MHz)
	 */
	inline double const * const  getRFMin()  { return &m_conf_hnd.getCurrentSetup().m_RFMin[0]; }

	/**
	 * @return the upper  limit of the RF coming from the receiver (MHz)
	 */
	inline double const * const  getRFMax()  { 
			return &m_conf_hnd.getCurrentSetup().m_RFMax[0];
			 }

	/**
	 * @return the start frequency of the IF coming from the receiver (MHz)
	 */
	inline double const * const  getIFMin()  { 
			return &m_conf_hnd.getCurrentSetup().m_IFMin[0];
			 }

	/**
	 * @return the bandwidth of the IF coming from the receiver (MHz)
	 */
	inline double const * const  getIFBandwidth() { 
		return  &m_conf_hnd.getCurrentSetup().m_IFBandwidth[0];
		 }

	/**
	 * @return the number of IF chains for each feed
	 */
	inline const  DWORD getIFs()  { return m_conf_hnd.getCurrentSetup().m_IFs; }

	/**
	 * @return the pointer to the polarizations array, the first element correspond to the first IF and so on.....
	 */
	inline Receivers::TPolarization const * const getPolarizations()  { return &m_conf_hnd.getCurrentSetup().m_polarizations[0]; }

	/**
	 * @return the number of feeds
	 */
	inline const  DWORD getFeeds()  { return m_conf_hnd.getCurrentSetup().m_feeds; }

	/**
	 * @return the default frequency for the synthesizer  (MHz)
	 */
	inline double const * const  getDefaultLO()   { return &m_conf_hnd.getCurrentSetup().m_defaultLO[0]; }

	/**
	 * @return the value of the fixed synthesizer used for the second conversion (MHz)
	 */
	inline double const * const  getFixedLO2()  { return &m_conf_hnd.getCurrentSetup().m_fixedLO2[0]; }

	/**
	 * @return lower limit for the synthesizer tuning (MHz)
	 */
	inline double const * const  getLOMin()  { return  &m_conf_hnd.getCurrentSetup().m_LOMin[0]; }

	/**
	 * @return upper limit for  the  synthesizer tuning (MHz)
	 */
	inline double const * const  getLOMax()  { return  &m_conf_hnd.getCurrentSetup().m_LOMax[0]; }

private:

	/**
	 * @brief Read setup configuration from file
	 * @param[in] p_conf_path configuration file path
	 * @param[out] p_params_out Read conf params	 
	 */
	void readConfigurationSetup(const IRA::CString & p_conf_path, ReceiverConfHandler::ConfigurationSetup & p_params_out )  throw (ComponentErrors::CDBAccessExImpl);

	/**
	 * @brief Read noise mark polynomial coefficient from file
	 * @details NoiseMark DataBlock carries polynomial calcultaion coefficients
	 * 	E.g.:  NMtemp= C0 + C1*Fin	 
	 * @param[in] p_conf_path file path
	 * @param[out] p_params_out Read conf params	 
	 */
	void readNoiseMarkPoly(const IRA::CString & p_conf_path, ReceiverConfHandler::ConfigurationSetup & p_params_out) throw (ComponentErrors::CDBAccessExImpl);

	/**
	 * @brief Feed coordinates readings
	 */
	void readFeeds() throw (ComponentErrors::CDBAccessExImpl, ComponentErrors::MemoryAllocationExImpl);	

	/**
	 * @brief Taper values readings
	 */
	void readTaper() throw (ComponentErrors::CDBAccessExImpl, ComponentErrors::MemoryAllocationExImpl);

	/**
	 * @brief Read freq-power table for each synth
	 * @details Getting 2 differents data table for 1st and 2nd stage mixer
	 */
	void readSynths()throw (ComponentErrors::CDBAccessExImpl, ComponentErrors::MemoryAllocationExImpl);

	/**
	 * @brief Single synth table reading
	 * @param[out] p_synt_table Data output container
	 * @param[in] p_table_file Input configuration file
	 */
	void readSyntTable(std::vector<TLOValue> & p_synt_table, IRA::CString p_table_file) throw (ComponentErrors::CDBAccessExImpl, ComponentErrors::MemoryAllocationExImpl);

public: 

	ReceiverConfHandler m_conf_hnd;	/**< Receiver Configuration  helper */	

private:

	maci::ContainerServices * m_service;
	
	IRA::CString m_dewarIPAddress;
	WORD m_dewarPort;
	IRA::CString m_LNAIPAddress;
	WORD m_LNAPort;
	DDWORD m_watchDogResponseTime;
	DDWORD m_watchDogSleepTime;
	DDWORD m_LNASamplingTime;
	DDWORD m_repetitionCacheTime;
	DDWORD m_repetitionExpireTime;

	IRA::CString m_localOscillatorInstance1st; /**< 1st stage mixer component instance name */
	IRA::CString m_localOscillatorInstance2nd; /**< 2nd stage mixer component instance name */

	std::vector<TLOValue> m_synt_table_1st;	/**< Freq - power synt table 1st stage LO*/
	std::vector<TLOValue> m_synt_table_2nd;	/**< Freq - power synt table 2nd stage LO*/
	std::vector<TMarkValue> m_mark_values; /**< @todo Mark value cosa do fuori?? */
	std::vector<TTaperValue>  m_taper_table; /**< Freq - value taper */	
	std::vector<TFeedValue>  m_feeds_table; /**< feeds data table */	
};

#endif
