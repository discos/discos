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

// Dummy value for a board connection error
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

	typedef struct {
		double skyFrequency;
		double markValue;
		Receivers::TPolarization polarization;
	} TMarkValue;

	typedef struct {
		double frequency;
		double outputPower;
	} TLOValue;

	typedef struct {
		double frequency;
		double taper;
	} TTaperValue;

	typedef struct {
		WORD code;
		double xOffset;
		double yOffset;
		double relativePower;
	} TFeedValue;

	typedef struct {
		double temperature;
        ACS::Time timestamp;
	} BoardValue;

	/**
	 * @brief Setup parmaters collector
	 */
	struct SetupParams{
		DWORD m_IFs;
		DWORD m_feeds;
		std::vector<double> m_RFMin;
		std::vector<double> m_RFMax;
		std::vector<double> m_IFMin;
		std::vector<double> m_IFBandwidth;		
		std::vector<Receivers::TPolarization> m_polarizations;		
		std::vector<double> m_defaultLO;
		std::vector<double> m_fixedLO2;
		std::vector<double> m_LOMin;
		std::vector<double> m_LOMax;
	};

	/**
	 * Default constructor
	 */
	CConfiguration();

	/**
	 * Destructor
	 */
	~CConfiguration();

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

	/**
	 * @return the instance of the local oscillator component name to drive 1st stage LO
	 */
	inline const IRA::CString& getLocalOscillatorInstance1st() const { return m_localOscillatorInstance1st; }

	/**
	 * @return the instance of the local oscillator component name to drive 2nd stage LO
	 */
	inline const IRA::CString& getLocalOscillatorInstance2nd() const { return m_localOscillatorInstance2nd; }

	/**
	 * Allows to get the table of mark values relative to left polarization
	 * @param freq vector containing the frequency value of the mark table. It must be freed by caller.
	 * @param markValue vector of the value of the calibration diode. It must be freed by caller.
	 * @param len used to return the length of the mark values array
	 * @return the size of the output vectors
	 */
	DWORD getLeftMarkTable(double *& freq,double *& markValuel) const;

	/**
	 * Allows to get the table of mark values relative to left polarization
	 * @param freq vector containing the frequency value of the mark table. It must be freed by caller.
	 * @param markValue vector of the value of the calibration diode. It must be freed by caller.
	 * @param len used to return the length of the mark values array
	 * @return the size of the output vectors
	 */
	DWORD getRightMarkTable(double *& freq,double *& markValue) const;

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
	 * @return the number of feeds
	 */
	inline const  DWORD& getFeeds() const { return m_feeds; }

	/**
	 * @param code feed identification codes. It must be freed by caller.
	 * @param xOffset displacement of the feed with respect to the central one along x axis. It must be freed by caller.
	  *@param yOffset displacement of the feed with respect to the central one along y axis. It must be freed by caller.
	  *@param relativePower expected percentage of variation of gain with respect to the central one. It must be freed by caller.
	 * @return the size of the output vectors
	 */
	DWORD getFeedInfo(WORD *& code,double *& xOffset,double *& yOffset,double *& relativePower) const;

	/**
	 * @return mnemonic of the working mode of the receiver
	 */
	inline const IRA::CString& getSetupMode() const { return m_mode; }

	/**
	 * @return the lower limit of the RF coming from the receiver (MHz)
	 */
	inline double const * const  getRFMin() const { return m_RFMin; }

	/**
	 * @return the upper  limit of the RF coming from the receiver (MHz)
	 */
	inline double const * const  getRFMax() const { return m_RFMax; }

	/**
	 * @return the start frequency of the IF coming from the receiver (MHz)
	 */
	inline double const * const  getIFMin() const { return m_IFMin; }

	/**
	 * @return the bandwidth of the IF coming from the receiver (MHz)
	 */
	inline double const * const  getIFBandwidth() const{ return  m_IFBandwidth; }

	/**
	 * @return the number of IF chains for each feed
	 */
	inline const  DWORD& getIFs() const { return m_IFs; }

	/**
	 * @return the pointer to the polarizations array, the first element correspond to the first IF and so on.....
	 */
	inline Receivers::TPolarization const * const getPolarizations() const { return m_polarizations; }

	/**
	 * @return the number of feeds
	 */
	inline const  DWORD& getFeeds() const { return m_feeds; }

	/**
	 * @return the default frequency for the synthesizer  (MHz)
	 */
	inline double const * const  getDefaultLO()  const { return m_defaultLO; }

	/**
	 * @return the value of the fixed synthesizer used for the second conversion (MHz)
	 */
	inline double const * const  getFixedLO2() const { return m_fixedLO2; }

	/**
	 * @return lower limit for the synthesizer tuning (MHz)
	 */
	inline double const * const  getLOMin() const { return  m_LOMin; }

	/**
	 * @return upper limit for  the  synthesizer tuning (MHz)
	 */
	inline double const * const  getLOMax() const { return  m_LOMax; }

	/**
    * This member function is used to configure component by reading the configuration parameter from the CDB.
	 * This must be the first call before using any other function of this class.
	 * @throw ComponentErrors::CDBAccess
	 * @param Services pointer to the container services object
	*/
	void init(maci::ContainerServices *Services)  throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl);
	
private:
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

	std::map<IRA::CString, IRA::CString> m_conf_files;	/**< Map known allowed configurations - configuration files */ 
	std::map<IRA::CString, SetupParams> m_conf_param; /**< Map configuration name - parameters */ 

	IRA::CDBTable *m_markTable;
	IRA::CDBTable *m_loTable;
	IRA::CDBTable *m_taperTable;
	IRA::CDBTable *m_feedsTable;

	TMarkValue *m_markVector;
	DWORD m_markVectorLen;
	TLOValue * m_loVector;
	DWORD m_loVectorLen;
	TTaperValue * m_taperVector;
	DWORD m_taperVectorLen;
	TFeedValue * m_feedVector; // length given by m_feeds
};


#endif
