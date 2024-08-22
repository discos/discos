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
#define LOTABLE_PATH CONFIG_PATH"/Synthesizer"
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
	 * @return the instance of the local oscillator component  that the receiver will use to drive the set its LO
	 */
	//inline const IRA::CString& getLocalOscillatorInstance() const { return m_localOscillatorInstance; }

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
	void setCurrentLOValue(const ACS::doubleSeq& lo);
	
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
	IRA::CDBTable *m_loTable;
	IRA::CDBTable *m_taperTable;
	IRA::CDBTable *m_feedsTable;

	TMarkValue *m_markVector;
	TLOValue * m_loVector;
	DWORD m_loVectorLen;
	DWORD m_markVectorLen;
	TTaperValue * m_taperVector;
	DWORD m_taperVectorLen;
	TFeedValue * m_feedVector; // length given by m_feeds
	
	void updateBandWith(const long& pos);
};

template <class T>
CConfiguration<T>::CConfiguration()
{
    m_markVector = NULL;
    m_markVectorLen = 0;
    m_markTable=NULL;
    m_mode="";
    m_loTable=NULL;
    m_loVector=NULL;
    m_loVectorLen=0;
    m_BandPolarizations = NULL;
    m_feedsTable = NULL;
    m_feedVector = NULL;
    m_taperTable = NULL;
    m_taperVector = NULL;
    m_taperVectorLen = 0;
    m_BandRFMin = NULL;
    m_BandRFMax = NULL;
    m_BandIFMin = NULL;
    m_BandIFBandwidth = NULL;
    m_DefaultLO = m_FixedLO2= m_currentLOValue= m_LOMin = m_LOMax = NULL; 
}

template <class T>
CConfiguration<T>::~CConfiguration()
{
    if (m_markTable) {
        delete m_markTable;
    }
    if (m_loTable) {
        delete m_loTable;
    }
    if (m_feedsTable) {
        delete m_feedsTable;
    }
    if (m_taperTable) {
        delete m_taperTable;
    }
    if (m_markVector) {
        delete [] m_markVector;
    }
    if (m_loVector) {
        delete [] m_loVector;
    }
    if (m_taperVector) {
        delete [] m_taperVector;
    }
    if (m_BandPolarizations) {
        delete [] m_BandPolarizations;
    }
    if (m_feedVector) {
        delete [] m_feedVector;
    }
    if (m_BandRFMin) {
        delete [] m_BandRFMin;
    }
    if (m_BandRFMax) {
        delete [] m_BandRFMax;
    }
    if (m_BandIFMin) {
        delete [] m_BandIFMin;
    }
    if (m_BandIFBandwidth) {
        delete [] m_BandIFBandwidth;
    }
    if (m_currentLOValue) {
        delete [] m_currentLOValue;
    }
    if (m_DefaultLO) {
        delete [] m_DefaultLO;
    }
    if (m_FixedLO2) {
        delete [] m_FixedLO2;
    }
    if (m_LOMin) {
        delete [] m_LOMin;
    }
    if (m_LOMax) {
        delete [] m_LOMax;
    }
}


/*
   throw (
   ComponentErrors::CDBAccessExImpl,
   ComponentErrors::MemoryAllocationExImpl, 
	ReceiversErrors::ModeErrorExImpl)
*/
template <class T>
void CConfiguration<T>::init(T *Services,IRA::CString comp_name) 
{
    m_services = Services;
    IRA::CError error;
    IRA::CString field;
    IRA::CString mode;
    WORD len;
    // read component configuration
    _GET_STRING_ATTRIBUTE(m_services,"DewarIPAddress","Dewar IP address:",m_dewarIPAddress,comp_name);
    _GET_STRING_ATTRIBUTE(m_services,"LNAIPAddress", "LNA IP address:",m_LNAIPAddress,comp_name);
	 _GET_STRING_ATTRIBUTE(m_services,"LocalOscillator_K_Instance","Local oscillator for K band:",m_localOscillator_K_Instance,comp_name);
	 _GET_STRING_ATTRIBUTE(m_services,"LocalOscillator_Q_Instance","Local oscillator for Q band:",m_localOscillator_Q_Instance,comp_name);
	 _GET_STRING_ATTRIBUTE(m_services,"LocalOscillator_W1_Instance","Local oscillator for W1 band:",m_localOscillator_W1_Instance,comp_name);
	 _GET_STRING_ATTRIBUTE(m_services,"LocalOscillator_W2_Instance","Local oscillator for W2 band:",m_localOscillator_W2_Instance,comp_name);
	 _GET_DWORD_ATTRIBUTE(m_services,"DewarPort","Dewar port:",m_dewarPort,comp_name);
    _GET_DWORD_ATTRIBUTE(m_services,"LNAPort","LNA port:",m_LNAPort,comp_name);
    _GET_DWORD_ATTRIBUTE(m_services,"WatchDogResponseTime","Response time of watch dog thread (uSec):",m_watchDogResponseTime,comp_name);
    _GET_DWORD_ATTRIBUTE(m_services,"WatchDogSleepTime","Sleep time of the watch dog thread (uSec):",m_watchDogSleepTime,comp_name);
    _GET_DWORD_ATTRIBUTE(m_services,"LNASamplingTime","Time needed to collect LNA information from control boards (uSec):",m_LNASamplingTime,comp_name);
    _GET_DWORD_ATTRIBUTE(m_services,"RepetitionCacheTime","Log repetition filter, caching time (uSec):",m_repetitionCacheTime,comp_name);
    _GET_DWORD_ATTRIBUTE(m_services,"RepetitionExpireTime","Log repetition filter, expire time (uSec):",m_repetitionExpireTime,comp_name);
    _GET_STRING_ATTRIBUTE(m_services,"DefaultMode","Default operating mode:",m_defaultMode,comp_name);

    const IRA::CString DEFAULTMODE_PATH = CONFIG_PATH"/Modes/" + m_defaultMode;
    
    // now read the receiver configuration
    _GET_STRING_ATTRIBUTE(m_services,"Mode","mode name:", mode, DEFAULTMODE_PATH);
    _GET_DWORD_ATTRIBUTE(m_services,"Feeds","Number of feeds:", m_feeds, DEFAULTMODE_PATH);
    _GET_DWORD_ATTRIBUTE(m_services,"IFs","Number of IFs for each feed:", m_IFs, DEFAULTMODE_PATH);

    try {
        m_BandPolarizations = new Receivers::TPolarization[m_IFs*m_feeds];
        m_BandRFMin = new double[m_IFs*m_feeds];
        m_BandRFMax = new double[m_IFs*m_feeds];
        m_BandIFMin = new double[m_IFs*m_feeds];
        m_BandIFBandwidth = new double[m_IFs*m_feeds];
        m_DefaultLO = new double[m_IFs*m_feeds];
        m_currentLOValue = new double[m_IFs*m_feeds];
        m_FixedLO2 = new double[m_IFs*m_feeds];
        m_LOMin = new double[m_IFs*m_feeds];
        m_LOMax = new double[m_IFs*m_feeds];      
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "CConfiguration::init()");
        throw dummy;
    }

    // Set the default operating mode
    setMode(mode);

    // The noise mark
    try {
        m_markTable = new IRA::CDBTable(Services,"MarkEntry",MARKTABLE_PATH);
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::init()");
        throw dummy;
    }
    error.Reset();
    if (!m_markTable->addField(error,"Feed",IRA::CDataField::LONGLONG)) {
        field = "Feed";
    }
    else if (!m_markTable->addField(error, "Polarization", IRA::CDataField::STRING)) {
        field="Polarization";
    }
    else if (!m_markTable->addField(error,"SkyFrequency",IRA::CDataField::DOUBLE)) {
        field="SkyFrequency";
    }
    else if (!m_markTable->addField(error,"NoiseMark",IRA::CDataField::DOUBLE)) {
        field="NoiseMark";
    }
    if (!error.isNoError()) {
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,error);
        dummy.setFieldName((const char *)field);
        throw dummy;
    }
    if (!m_markTable->openTable(error)) {
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,error);
        throw dummy;
    }

    m_markTable->First();
    len = m_markTable->recordCount();
    try {
        m_markVector = new TMarkValue[len];
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "CConfiguration::init()");
        throw dummy;
    }
    for (WORD i=0;i<len;i++) {
        m_markVector[i].skyFrequency=(*m_markTable)["SkyFrequency"]->asDouble();
        m_markVector[i].feed = (*m_markTable)["Feed"]->asLongLong();
        m_markVector[i].markValue=(*m_markTable)["NoiseMark"]->asDouble();
        m_markVector[i].polarization=(*m_markTable)["Polarization"]->asString()=="LEFT"?Receivers::RCV_LCP:Receivers::RCV_RCP;
        ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",
                (LM_DEBUG,"MARK_VALUE_ENTRY: %d %lf %lf",m_markVector[i].polarization,m_markVector[i].skyFrequency,
        m_markVector[i].markValue));
        m_markTable->Next();
    }
    
    m_markVectorLen = len;
    m_markTable->closeTable();
    delete m_markTable;
    m_markTable = NULL;
    
    // The synthesizer
    try {
        m_loTable=new IRA::CDBTable(Services,"SynthesizerEntry",LOTABLE_PATH);
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::init()");
        throw dummy;
    }
    error.Reset();
    if (!m_loTable->addField(error,"Frequency",IRA::CDataField::DOUBLE)) {
        field="Frequency";
    }
    else if (!m_loTable->addField(error,"OutputPower",IRA::CDataField::DOUBLE)) {
        field="OutputPower";
    }
    if (!error.isNoError()) {
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,error);
        dummy.setFieldName((const char *)field);
        throw dummy;
    }
    if (!m_loTable->openTable(error))   {
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
        throw dummy;
    }
    m_loTable->First();
    len=m_loTable->recordCount();
    try {
        m_loVector=new TLOValue[len];
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::init()");
        throw dummy;
    }
    ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_DEBUG,"SYNTH_VALUE_ENTRY_NUMBER: %d",len));
    for (WORD i=0;i<len;i++) {
        m_loVector[i].frequency=(*m_loTable)["Frequency"]->asDouble();
        m_loVector[i].outputPower=(*m_loTable)["OutputPower"]->asDouble();
        ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_DEBUG,"SYNTH_VALUE_ENTRY: %lf %lf",m_loVector[i].frequency,m_loVector[i].outputPower));
        m_loTable->Next();
    }
    m_loVectorLen=len;
    m_loTable->closeTable();
    delete m_loTable;
    m_loTable=NULL;


    // The feeds
	try {
		m_feedsTable=new IRA::CDBTable(Services,"Feed", FEEDTABLE_PATH);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::init()");
		throw dummy;
	}
	error.Reset();
	if (!m_feedsTable->addField(error,"feedCode",IRA::CDataField::LONGLONG)) {
		field="feedCode";
 	}
	else if (!m_feedsTable->addField(error,"xOffset",IRA::CDataField::DOUBLE)) {
		field="xOffset";
 	}
	else if (!m_feedsTable->addField(error,"yOffset",IRA::CDataField::DOUBLE)) {
		field="yOffset";
 	}
	else if (!m_feedsTable->addField(error,"relativePower",IRA::CDataField::DOUBLE)) {
		field="relativePower";
 	}
	if (!error.isNoError()) {
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,error);
		dummy.setFieldName((const char *)field);
		throw dummy;
	}
	if (!m_feedsTable->openTable(error))	{
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
		throw dummy;
	}
	len=m_feeds;
	try {
		m_feedVector=new TFeedValue[len];
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::init()");
		throw dummy;
	}
	for (WORD i=0;i<len;i++) {
		m_feedVector[i].xOffset=(*m_feedsTable)["xOffset"]->asDouble();
		m_feedVector[i].yOffset=(*m_feedsTable)["yOffset"]->asDouble();
		m_feedVector[i].relativePower=(*m_feedsTable)["relativePower"]->asDouble();
		m_feedVector[i].code=(WORD)(*m_feedsTable)["feedCode"]->asLongLong();
		ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_DEBUG,"FEED_VALUE_ENTRY: %d %lf %lf %lf",m_feedVector[i].code,m_feedVector[i].xOffset,m_feedVector[i].yOffset,m_feedVector[i].relativePower));
		m_feedsTable->Next();
	}
	m_feedsTable->closeTable();
	delete m_feedsTable;
	m_feedsTable=NULL;

    //The taper.....
    try {
        m_taperTable = new IRA::CDBTable(Services, "TaperEntry", TAPERTABLE_PATH);
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "CConfiguration::init()");
        throw dummy;
    }
    error.Reset();
    if (!m_taperTable->addField(error, "Feed", IRA::CDataField::LONGLONG)) {
        field = "Feed";
    }
    else if (!m_taperTable->addField(error, "Frequency", IRA::CDataField::DOUBLE)) {
        field = "Frequency";
    }
    else if (!m_taperTable->addField(error, "Taper", IRA::CDataField::DOUBLE)) {
        field = "OutputPower";
    }
    if (!error.isNoError()) {
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
        dummy.setFieldName((const char *)field);
        throw dummy;
    }
    if (!m_taperTable->openTable(error))    {
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
        throw dummy;
    }
    m_taperTable->First();
    len = m_taperTable->recordCount();
    try {
        m_taperVector = new TTaperValue[len];
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "CConfiguration::init()");
        throw dummy;
    }
    ACS_LOG(LM_FULL_INFO, "CConfiguration::init()", (LM_DEBUG, "TAPER_ENTRY_NUMBER: %d", len));
    for (WORD i=0; i<len; i++) {
        m_taperVector[i].feed = (*m_taperTable)["Feed"]->asLongLong();
        m_taperVector[i].frequency = (*m_taperTable)["Frequency"]->asDouble();
        m_taperVector[i].taper = (*m_taperTable)["Taper"]->asDouble();
        ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",
        	(LM_DEBUG,"SYNTH_VALUE_ENTRY: %lld %lf %lf",m_taperVector[i].feed,m_taperVector[i].frequency,m_taperVector[i].taper));
        m_taperTable->Next();
    }
    m_taperVectorLen = len;
    m_taperTable->closeTable();
    delete m_taperTable;
    m_taperTable = NULL;
}

template <class T>
void CConfiguration<T>::setCurrentLOValue(const ACS::doubleSeq& lo)
{
	for (WORD k=0;k<MIN(lo.length(),getArrayLen());k++) {
		if (lo[k]>=0.0) {
			m_currentLOValue[k]=lo[k];
			updateBandWith(k);
		}
	}	
}

template <class T>
void CConfiguration<T>::setCurrentLOValue(const double& val,const long& pos)
{
	if ((pos>=0) && (pos<=getArrayLen())) {
		if (val>=0.0) {
			m_currentLOValue[pos]=val;
			updateBandWith(pos);
		}
	}
}

template <class T>
void CConfiguration<T>::updateBandWith(const long& pos)
{
	m_BandIFBandwidth[pos]=m_BandRFMax[pos]-(m_BandIFMin[pos]+m_currentLOValue[pos]);
}

/*
* throw (ComponentErrors::CDBAccessExImpl, ReceiversErrors::ModeErrorExImpl) 
*/
template <class T>
void CConfiguration<T>::setMode(const char * mode) 
{
    IRA::CString value, token;
    IRA::CError error;
    IRA::CString cmdMode(mode);

	 if (cmdMode==m_mode) return;
    if(!m_mode.IsEmpty()) {
    	  if (cmdMode=="*") return;
    }    
    
    CString MODE_PATH((std::string(CONFIG_PATH) + std::string("/Modes/") + std::string(cmdMode)).c_str());
    _GET_DWORD_ATTRIBUTE(m_services,"Feeds","Number of feeds:",m_feeds,MODE_PATH);
    _GET_DWORD_ATTRIBUTE(m_services,"IFs","Number of feeds:",m_IFs,MODE_PATH);
    _GET_STRING_ATTRIBUTE(m_services,"ActivateBypass","Set LNA bypass mode:",value,MODE_PATH); 
    value.MakeUpper();
    if (value=="TRUE") m_lnaBypass=true;
    else m_lnaBypass=false;											  
    

    _GET_STRING_ATTRIBUTE(m_services,"IFBandPolarizations","IF polarizations:",value,MODE_PATH); 											  
    int start=0;
    for (WORD k=0;k<m_IFs*m_feeds;k++) {
        if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("IFBandPolarizations");
            throw dummy;
        }
        token.MakeUpper();
        if (token == "L") {
            m_BandPolarizations[k]=Receivers::RCV_LCP; 
        }
        else if (token == "R") {
            m_BandPolarizations[k]=Receivers::RCV_RCP;
        }
        else {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,error);
            dummy.setFieldName("IFBandPolarizations");
            throw dummy;
        }
    }

    _GET_STRING_ATTRIBUTE(m_services,"RFMin", "RF lower limit (MHz):",value,MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs*m_feeds; k++) {
        if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("RFMin");
            throw dummy;
        }
        m_BandRFMin[k] = token.ToDouble();
    }

    _GET_STRING_ATTRIBUTE(m_services,"RFMax", "RF upper limit (MHz):",value,MODE_PATH);
    start = 0;
    for (WORD k=0;k<m_IFs*m_feeds;k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("RFMax");
            throw dummy;
        }
        m_BandRFMax[k] = token.ToDouble();
    }
    
    _GET_STRING_ATTRIBUTE(m_services,"IFMin", "IF lower limit (MHz):",value,MODE_PATH);
    start=0;
    for (WORD k=0; k<m_IFs*m_feeds; k++) {
        if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("IFMin");
            throw dummy;
        }
        m_BandIFMin[k] = token.ToDouble();
    }

    _GET_STRING_ATTRIBUTE(m_services,"IFMax","IF upper limit (MHz):",value,MODE_PATH);
    start = 0;
    for (WORD k=0;k<m_IFs*m_feeds;k++) {
        if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,error);
            dummy.setFieldName("RFMax");
            throw dummy;
        }
        m_BandIFBandwidth[k]=token.ToDouble()-m_BandIFMin[k];
    }
    
    _GET_STRING_ATTRIBUTE(m_services,"DefaultLO","Default LO Value (MHz):",value,MODE_PATH);
    start=0;
    for (WORD k=0; k<m_IFs*m_feeds; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("DefaultLO");
            throw dummy;
        }
        m_DefaultLO[k]=token.ToDouble();
    }

    _GET_STRING_ATTRIBUTE(m_services,"FixedLO2", "LO2 Value (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0;k<m_IFs*m_feeds;k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("FixedLO2");
            throw dummy;
        }
        m_FixedLO2[k] = token.ToDouble();
    }

    _GET_STRING_ATTRIBUTE(m_services,"LOMin", "Minimum LO Value (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs*m_feeds; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("LOMin");
            throw dummy;
        }
        m_LOMin[k] = token.ToDouble();
    }

    _GET_STRING_ATTRIBUTE(m_services,"LOMax","Maximum LO Value (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs*m_feeds; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("LOMax");
            throw dummy;
        }
        m_LOMax[k] = token.ToDouble();
    }
    m_mode = cmdMode;
}

template <class T>
DWORD CConfiguration<T>::getSynthesizerTable(double * &freq,double *&power) const
{
    freq= new double [m_loVectorLen];
    power=new double [m_loVectorLen];
    for (DWORD j=0;j<m_loVectorLen;j++) {
        freq[j]=m_loVector[j].frequency;
        power[j]=m_loVector[j].outputPower;
    }
    return m_loVectorLen;
}

template <class T>
DWORD CConfiguration<T>::getTaperTable(double * &freq,double *&taper, short feed) const
{
   freq = new double [m_taperVectorLen];
   taper = new double [m_taperVectorLen];
	DWORD count=0;
   for (DWORD j=0; j<m_taperVectorLen; j++) {
		if (m_taperVector[j].feed==feed) {
      	freq[count] = m_taperVector[j].frequency;
         taper[count] = m_taperVector[j].taper;
			count++;
    	}
	}
   return count;
}

template <class T>
DWORD CConfiguration<T>::getLeftMarkTable(ACS::doubleSeq& freq,ACS::doubleSeq& markValue,short feed) const
{
	freq.length(m_markVectorLen);
	markValue.length(m_markVectorLen);
	DWORD count=0;
	for (DWORD j=0;j<m_markVectorLen;j++) {
		if (m_markVector[j].polarization==Receivers::RCV_LCP && m_markVector[j].feed==feed) {
			freq[count]=m_markVector[j].skyFrequency;
			markValue[count]=m_markVector[j].markValue;
			count++;
		}
	}
	return count;
}

template <class T>
DWORD CConfiguration<T>::getRightMarkTable(ACS::doubleSeq& freq,ACS::doubleSeq& markValue,short feed) const
{
	freq.length(m_markVectorLen);
	markValue.length(m_markVectorLen);
	DWORD count=0;
	for (DWORD j=0;j<m_markVectorLen;j++) {
		if (m_markVector[j].polarization==Receivers::RCV_RCP && m_markVector[j].feed==feed) {
			freq[count]=m_markVector[j].skyFrequency;
			markValue[count]=m_markVector[j].markValue;
			count++;
		}
	}
	return count;
}

template <class T>
DWORD CConfiguration<T>::getFeedInfo(
        WORD *& code,
        double *& xOffset,
        double *& yOffset,
        double *& relativePower
        ) const
{
    code = new WORD[m_feeds];
    xOffset = new double [m_feeds];
    yOffset = new double [m_feeds];
    relativePower = new double [m_feeds];
    for (DWORD j=0; j<m_feeds; j++) {
        code[j] = m_feedVector[j].code;
        xOffset[j] = m_feedVector[j].xOffset;
        yOffset[j] = m_feedVector[j].yOffset;
        relativePower[j] = m_feedVector[j].relativePower;
    }
    return m_feeds;
}

#endif
