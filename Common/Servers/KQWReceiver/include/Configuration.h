#ifndef _BASECCONFIGURATION_H_
#define _BASECCONFIGURATION_H_

/*******************************************************************************\
 IRA Istituto di Radioastronomia                                 
 This code is under GNU General Public License (GPL).          
                                                              
 Author: Andrea Orlati (andrea.orlati@inaf.it)
\*******************************************************************************/


#include <IRA>
#include <ComponentErrors.h>
#include <ReceiversErrors.h>
#include <ReceiversDefinitionsC.h>
#include <string>
#include "utils.h"

#define _GET_DOUBLE_ATTRIBUTE(SERV,ATTRIB,DESCR,FIELD,NAME) { \
    double tmpd; \
    if (!CIRATools::getDBValue(SERV,ATTRIB,tmpd,"alma/",NAME)) { \
        _EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"CConfiguration::Init()"); \
        dummy.setFieldName(ATTRIB); \
        throw dummy; \
    } \
    else { \
        FIELD=tmpd; \
        ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %lf",tmpd); \
    } \
}

#define _GET_DWORD_ATTRIBUTE(SERV,ATTRIB,DESCR,FIELD,NAME) { \
    if (!CIRATools::getDBValue(SERV,ATTRIB,FIELD,"alma/",NAME)) { \
        _EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"CConfiguration::Init()"); \
        dummy.setFieldName(ATTRIB); \
        throw dummy; \
    } \
    else { \
        ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %u",FIELD); \
    } \
}

#define _GET_STRING_ATTRIBUTE(SERV,ATTRIB,DESCR,FIELD,NAME) { \
    if (!CIRATools::getDBValue(SERV,ATTRIB,FIELD,"alma/",NAME)) { \
        _EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"::CConfiguration::Init()"); \
        dummy.setFieldName(ATTRIB); \
        throw dummy; \
    } \
    else { \
        ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %s",(const char*)FIELD); \
    } \
}

#define CONFIG_PATH "DataBlock/KQWReceiver"
#define LOTABLE_K_PATH CONFIG_PATH"/Synthesizer_K"
#define LOTABLE_Q_PATH CONFIG_PATH"/Synthesizer_Q"
#define LOTABLE_WL_PATH CONFIG_PATH"/Synthesizer_WLOW"
#define LOTABLE_WH_PATH CONFIG_PATH"/Synthesizer_WHIGH"
#define MARKTABLE_PATH CONFIG_PATH"/NoiseMark"
#define FEEDTABLE_PATH CONFIG_PATH"/Feeds"
#define TAPERTABLE_PATH CONFIG_PATH"/Taper"

// Dummy value for a board connection error
#define CEDUMMY 100000

using namespace IRA;

/**
 * This class implements the component configuration. The data inside this class are initialized at the startup from the
 * configuration database and then are used (read) inside the component.
 * @author <a href=mailto:andrea.orlati@inaf.it>Andrea Orlati</a>, Istituto di Radioastronomia, Italia
 * <br> 
  */
template <class T>  
class CConfiguration {
public:

	typedef struct {
		double skyFrequency;
		long long feed; 
		Receivers::TPolarization polarization;
		double markValue;
	} TMarkValue;

	typedef struct {
		double frequency;
		double outputPower;
	} TLOValue;

	typedef struct {
		long long feed; 
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
	
	typedef enum {
		KBAND=0,
		QBAND=2,
		WLBAND=4,
		WHBAND=6
	} TBandId;
	

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
	inline const DWORD& getDewarPort() const { return m_dewarPort; }

	/**
	 * @return the IP address of the board that controls the receiver LNA
	 */
	inline const IRA::CString& getLNAIPAddress() const { return m_LNAIPAddress; }

	/**
	 * @return the IP port of the board that controls the receiver LNA
	 */
	inline const DWORD& getLNAPort() const { return m_LNAPort; }

	/**
	 * @return the time allowed to the watch dog thread to complete an iteration and 
     * respond to the thread manager (microseconds)
	 */
	inline const DWORD& getWarchDogResponseTime() const { return m_watchDogResponseTime; }

	/**
	 * @return the time between two execution of the watch dog thread (microseconds)
	 */
	inline const DWORD& getWatchDogSleepTime() const { return m_watchDogSleepTime; }

	/**
	 * @return the time required by the board to sample the values of LNA (microseconds)
	 */
	inline const DWORD& getLNASamplingTime() const { return m_LNASamplingTime; }

	/**
	 * @return the time the repetition log guard will cache new log messages before sending 
     * to the central logger (microseconds)
	 */
	inline const DWORD& getRepetitionCacheTime() const { return m_repetitionCacheTime; }

	/**
	 * @return the time of expiration of a log sent to the repetition log guard (microseconds)
	 */
	inline const DWORD& getRepetitionExpireTime() const { return m_repetitionExpireTime; }

	/**
	 * Allows to get the table of mark values relative to left polarization
	 * @param freq vector containing the frequency value of the mark table. It must be freed by caller.
	 * @param markValue vector of the value of the calibration diode. It must be freed by caller.
	 * @param len used to return the length of the mark values array
	 * @return the size of the output vectors
	 */
	DWORD getLeftMarkTable(ACS::doubleSeq& freq,ACS::doubleSeq& markValue,short feed) const;

	/**
	 * Allows to get the table of mark values relative to left polarization
	 * @param freq vector containing the frequency value of the mark table. It must be freed by caller.
	 * @param markValue vector of the value of the calibration diode. It must be freed by caller.
	 * @param len used to return the length of the mark values array
	 * @return the size of the output vectors
	 */
	DWORD getRightMarkTable(ACS::doubleSeq& freq,ACS::doubleSeq& markValue,short feed) const;

	/**
	 * @return the instance of the K-band local oscillator component  that the receiver will use to drive the set its LO
	 */
	inline const IRA::CString& getLocalOscillatorInstance_K() const { return m_localOscillator_K_Instance; }
	
	/**
	 * @return the instance of the Q-band local oscillator component  that the receiver will use to drive the set its LO
	 */
	inline const IRA::CString& getLocalOscillatorInstance_Q() const { return m_localOscillator_Q_Instance; }
	
	/**
	 * @return the instance of the WL-band local oscillator component  that the receiver will use to drive the set its LO
	*/
	inline const IRA::CString& getLocalOscillatorInstance_WL() const { return m_localOscillator_W1_Instance; }
	
	/**
	 * @return the instance of the WH-band local oscillator component  that the receiver will use to drive the set its LO
	*/
	inline const IRA::CString& getLocalOscillatorInstance_WH() const { return m_localOscillator_W2_Instance; }

	/**
	 * Return the synth table for the K band
	 * @param freq vector with the synthesizer frequencies. It must be freed by caller.
	 * @param power corresponding powers for the frequencies vector. It must be freed by caller.
	 * @return the size of the output vectors
	 */
	DWORD getSynthesizerTable_K(double * &freq,double *&power) const;
	
		/**
	 * Return the synth table for the Q band
	 * @param freq vector with the synthesizer frequencies. It must be freed by caller.
	 * @param power corresponding powers for the frequencies vector. It must be freed by caller.
	 * @return the size of the output vectors
	 */
	DWORD getSynthesizerTable_Q(double * &freq,double *&power) const;
	
		/**
	 * Return the synth table for the WL band
	 * @param freq vector with the synthesizer frequencies. It must be freed by caller.
	 * @param power corresponding powers for the frequencies vector. It must be freed by caller.
	 * @return the size of the output vectors
	 */
	DWORD getSynthesizerTable_WL(double * &freq,double *&power) const;
	
		/**
	 * Return the synth table for the WH band
	 * @param freq vector with the synthesizer frequencies. It must be freed by caller.
	 * @param power corresponding powers for the frequencies vector. It must be freed by caller.
	 * @return the size of the output vectors
	 */
	DWORD getSynthesizerTable_WH(double * &freq,double *&power) const;

	/**
	 * @param freq vector with the frequencies. It must be freed by caller.
	 * @param taper corresponding taper  for the frequencies vector. It must be freed by caller.
	 * @return the size of the output vectors
	 */
	DWORD getTaperTable(double * &freq,double *&taper,short feed) const;

	/**
	* @param code feed identification codes. It must be freed by caller.
	* @param xOffset displacement of the feed with respect to the central one along x axis. 
	* It must be freed by caller.
	* @param yOffset displacement of the feed with respect to the central one along y axis. 
	* It must be freed by caller.
	* @param relativePower expected percentage of variation of gain with respect to the central one. 
	* It must be freed by caller.
	* @return the size of the output vectors
	*/
	DWORD getFeedInfo(WORD *& code,double *& xOffset,double *& yOffset,double *& relativePower) const;

	/**
	* @return mnemonic of the working mode of the receiver
	*/
	inline const IRA::CString& getActualMode() const { return m_mode; }

	/**
	 * @return mnemonic of the default mode of the receiver
	*/
	inline const IRA::CString& getDefaultMode() const { return m_defaultMode; }

    /**
     * @return the markVector
     */
	inline const TMarkValue * getMarkVector() const { return m_markVector; }

    inline const DWORD getMarkVectorLen() const { return m_markVectorLen; }

	 /*
	 * @throw (ComponentErrors::CDBAccessExImpl, ReceiversErrors::ModeErrorExImpl)
	 */
    void setMode(const char * mode); 
    
    /** 
     * @return the index of the dedicated arrays where the relative information are located 
    */ 
    inline const DWORD getArrayIndex(const long& feed, const long& ifs) const { return feed*2+ifs; }
    
    inline const DWORD getArrayIndex(const long& feed) const { return feed*2; }

    /** 
     * @return the length of the dedicated arrays where the relative information are located 
    */ 
    inline const DWORD getArrayLen() const { return m_feeds*m_IFs; }
    

	/**
	 * @return the lower limit of the RF coming from the K,Q,W1 and W2 band feed (MHz)
	 */
	inline double const * const  getBandRFMin() const { return m_BandRFMin; }

	/**
	 * @return the upper limit of the RF coming from the K,Q,W1 and W2 band feed (MHz)
	 */
	inline double const * const  getBandRFMax() const { return m_BandRFMax; }

	/**
	 * @return the start frequency of the IF coming from the K,Q,W1 and W2 band feed (MHz)
	 */
	inline double const * const  getBandIFMin() const { return m_BandIFMin; }
	
	/**
	 * @return the bandwidth of the IF coming from the K,Q,W1 and W2 band feed (MHz)
	 */
	inline double const * const  getBandIFBandwidth() const{ return  m_BandIFBandwidth; }

	/**
	 * @return the number of IF chains for each feed
	 */
	inline const  DWORD& getIFs() const { return m_IFs; }

	/**
	 * @return the pointer to the polarizations array; the first element corresponds to the first IF and so on.
	 */
	inline Receivers::TPolarization const * const getBandPolarizations() const { return m_BandPolarizations; }

	/**
	 * @return the number of feeds
	 */
	inline const  DWORD& getFeeds() const { return m_feeds; }

	/**
	 * @return the default frequency for the local oscillators (MHz)
	 */
	inline double const * const  getDefaultLO()  const { return m_DefaultLO; }
	
	/**
	 * @return the current frequency for local Oscillator (MHz)
	 */
	inline double const * const  getCurrentLOValue()  const { return m_currentLOValue; }
	
	/**
	 * @return the current frequency for local Oscillator (MHz)
	 */
	//void setCurrentLOValue(const ACS::doubleSeq& lo);
	
	void setCurrentLOValue(const double& val,const long& pos);

	/**
	 * @return the value of the fixed synthesizer used for the second conversion (MHz)
	 */
	inline double const * const  getFixedLO2() const { return m_FixedLO2; }

	/**
	 * @return lower limit for the synthesizer tuning (MHz)
	 */
	inline double const * const  getLOMin() const { return  m_LOMin; }

	/**
	 * @return upper limit for  the  synthesizer tuning (MHz)
	 */
	inline double const * const  getLOMax() const { return  m_LOMax; }
	
	/*
	 * @return if the lan bypass mode has to be commanded to the receiver or not
	 */
	inline bool getLNABypass() const { return m_lnaBypass; }

	/**
    * This member function is used to configure component by reading the configuration parameter from the CDB.
	 * This must be the first call before using any other function of this class.
	 * @throw ComponentErrors::CDBAccessExImpl,
    * @throw ComponentErrors::MemoryAllocationExImpl,
    * @thorw ReceiversErrors::ModeErrorExImpl
	 * @param Services pointer to the container services object or to simpleClient
	 * @param comp_name name of the component 
	*/
	void init(T *Services,IRA::CString comp_name=""); 
	

private:
	IRA::CString m_dewarIPAddress;
	DWORD m_dewarPort;
	IRA::CString m_LNAIPAddress;
	DWORD m_LNAPort;
	DWORD m_watchDogResponseTime;
	DWORD m_watchDogSleepTime;
	DWORD m_LNASamplingTime;
	DWORD m_repetitionCacheTime;
	DWORD m_repetitionExpireTime;
	IRA::CString m_localOscillator_K_Instance;
	IRA::CString m_localOscillator_Q_Instance;
	IRA::CString m_localOscillator_W1_Instance;
	IRA::CString m_localOscillator_W2_Instance;
	
   T* m_services;
	IRA::CString m_mode;
	IRA::CString m_defaultMode;
	double *m_BandRFMin;
	double *m_BandRFMax;
	double *m_BandIFMin;
	double *m_BandIFBandwidth;
   double *m_DefaultLO;
   double *m_FixedLO2;
   double *m_currentLOValue;
   double *m_LOMin;
   double *m_LOMax;
	DWORD m_IFs;
	bool m_lnaBypass;
	Receivers::TPolarization *m_BandPolarizations;
	DWORD m_feeds;


	IRA::CDBTable *m_markTable;
	IRA::CDBTable *m_loTable_K;
	IRA::CDBTable *m_loTable_Q;
	IRA::CDBTable *m_loTable_WL;
	IRA::CDBTable *m_loTable_WH;
	
	IRA::CDBTable *m_taperTable;
	IRA::CDBTable *m_feedsTable;

	TMarkValue *m_markVector;
	TLOValue * m_loVector_K;
	TLOValue * m_loVector_Q;
	TLOValue * m_loVector_WL;
	TLOValue * m_loVector_WH;
	DWORD m_loVectorLen_K;
	DWORD m_loVectorLen_Q;
	DWORD m_loVectorLen_WL;
	DWORD m_loVectorLen_WH;
	DWORD m_markVectorLen;
	TTaperValue * m_taperVector;
	DWORD m_taperVectorLen;
	TFeedValue * m_feedVector; // length given by m_feeds
	
	void updateBandWith();
	WORD openSynthTable(IRA::CDBTable * &table, const IRA::CString &path, TLOValue * &loV);
};


#include "Configuration.tpp"

#endif
