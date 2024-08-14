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
	inline const WORD& getDewarPort() const { return m_dewarPort; }

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
	DWORD getLeftMarkTable(double *& freq,double *& markValuel, short feed) const;

	/**
	 * Allows to get the table of mark values relative to left polarization
	 * @param freq vector containing the frequency value of the mark table. It must be freed by caller.
	 * @param markValue vector of the value of the calibration diode. It must be freed by caller.
	 * @param len used to return the length of the mark values array
	 * @return the size of the output vectors
	 */
	DWORD getRightMarkTable(double *& freq,double *& markValue, short feed) const;

	/**
	 * @return the instance of the local oscillator component  that the receiver will use to drive the set its LO
	 */
	inline const IRA::CString& getLocalOscillatorInstance() const { return m_localOscillatorInstance; }

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
	 * @return the default frequency for the synthesizer  (MHz)
	 */
	inline double const * const  getDefaultLO()  const { return m_DefaultLO; }

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
	

 /*   std::vector<double> getLBandRFMaxFromMode(IRA::CString cmdMode) throw (
            ComponentErrors::CDBAccessExImpl,
            ComponentErrors::MemoryAllocationExImpl, 
            ReceiversErrors::ModeErrorExImpl);
 

    std::vector<double> getLBandRFMinFromMode(IRA::CString cmdMode) throw (
            ComponentErrors::CDBAccessExImpl,
            ComponentErrors::MemoryAllocationExImpl, 
            ReceiversErrors::ModeErrorExImpl);*/

    
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
	IRA::CString m_localOscillatorInstance;
   T* m_services;
	IRA::CString m_mode;
	IRA::CString m_defaultMode;
	double *m_BandRFMin;
	double *m_BandRFMax;
	double *m_BandIFMin;
	double *m_BandIFBandwidth;
   double *m_DefaultLO;
   double *m_FixedLO2;
   double *m_LOMin;
   double *m_LOMax;
   //double *m_LowpassFilterMin;
   //double *m_LowpassFilterMax;
	DWORD m_IFs;
	//DWORD m_LBandFilterID;
	//DWORD m_PBandFilterID;
	Receivers::TPolarization *m_BandPolarizations;
	//Receivers::TPolarization *m_PBandPolarizations;
   //IRA::CString m_LBandPolarization;
   //IRA::CString m_PBandPolarization;
	DWORD m_feeds;
	//double *m_LBandLO;
	//double *m_PBandLO;

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
};

template <class T>
CConfiguration<T>::CConfiguration()
{
    m_markVector = NULL;
    m_markVectorLen = 0;
    m_markTable=NULL;
    
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
    //m_LBandLO = m_PBandLO = NULL;
    m_BandIFBandwidth = NULL;
    m_DefaultLO = m_FixedLO2= m_LOMin = m_LOMax = NULL; 
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
    if (m_BandIFBandwidth) {
        delete [] m_BandIFBandwidth;
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
    WORD len;
    // read component configuration
    _GET_STRING_ATTRIBUTE(m_services,"DewarIPAddress","Dewar IP address:",m_dewarIPAddress,comp_name);
    _GET_STRING_ATTRIBUTE(m_services,"LNAIPAddress", "LNA IP address:",m_LNAIPAddress,comp_name);
	 _GET_STRING_ATTRIBUTE(m_services,"LocalOscillatorInstance","Local oscillator instance:",m_localOscillatorInstance,comp_name);
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
    _GET_STRING_ATTRIBUTE(m_services,"Mode","mode name:", m_mode, DEFAULTMODE_PATH);
    _GET_DWORD_ATTRIBUTE(m_services,"Feeds","Number of feeds:", m_feeds, DEFAULTMODE_PATH);
    _GET_DWORD_ATTRIBUTE(m_services,"IFs","Number of IFs for each feed:", m_IFs, DEFAULTMODE_PATH);

	/*double *m_BandRFMin;
	double *m_BandRFMax;
	double *m_BandIFMin;
	double *m_BandIFBandwidth;
   double *m_DefaultLO;
   double *m_FixedLO2;
   double *m_LOMin;
   double *m_LOMax;*/


    try {
        m_BandPolarizations = new Receivers::TPolarization[m_IFs*m_feeds];

        /*m_BandRFMin = new double[m_IFs];

        m_BandRFMax = new double[m_IFs];

        m_PBandIFMin = new double[m_IFs];

        m_BandLO = new double[m_IFs];

        m_BandIFBandwidth = new double[m_IFs];

        m_DefaultLO = new double[m_IFs];
        m_FixedLO2 = new double[m_IFs];
        m_LOMin = new double[m_IFs];
        m_LOMax = new double[m_IFs];*/

    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "CConfiguration::init()");
        throw dummy;
    }

    // Set the default operating mode
    setMode(m_mode);

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

/*
* throw (ComponentErrors::CDBAccessExImpl, ReceiversErrors::ModeErrorExImpl) 
*/
template <class T>
void CConfiguration<T>::setMode(const char * mode) 
{
    IRA::CString value, token;
    IRA::CError error;
    IRA::CString cmdMode(mode);

    if(!m_mode.IsEmpty()){
        if(cmdMode.GetLength() != m_mode.GetLength()) {
            _EXCPT(ReceiversErrors::ModeErrorExImpl, impl, 
                    "CConfiguration::setMode(): Wrong number of characters in the commanded mode.");
            ACS_LOG(LM_FULL_INFO, "CConfiguration::init()", (LM_DEBUG, "Wrong number of characters in " + m_mode));
            throw impl; 
        }
    }

    if(cmdMode.Find('*') != -1) {
        for (WORD k=0; k<m_mode.GetLength(); k++) {
            if(cmdMode[k] == '*') {
                cmdMode.SetAt(k, m_mode[k]); // Do not change this configuration
            }
        }
    }
    
    CString MODE_PATH((std::string(CONFIG_PATH) + std::string("/Modes/") + std::string(cmdMode)).c_str());
    _GET_DWORD_ATTRIBUTE(m_services,"Feeds","Number of feeds:",m_feeds,MODE_PATH);


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

   /* _GET_STRING_ATTRIBUTE("LBandRFMin", "L band RF lower limit (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("LBandRFMin");
            throw dummy;
        }
        m_LBandRFMin[k] = token.ToDouble();
    }

    _GET_STRING_ATTRIBUTE("LBandRFMax", "L band RF upper limit (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("LBandRFMax");
            throw dummy;
        }
        m_LBandRFMax[k] = token.ToDouble();
    }

    for (WORD k=0; k<m_IFs; k++)
        m_LBandIFBandwidth[k] = m_LBandRFMax[k] - m_LBandRFMin[k];
    
    _GET_STRING_ATTRIBUTE("PBandRFMin", "P band RF lower limit (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("PBandRFMin");
            throw dummy;
        }
        m_PBandRFMin[k] = token.ToDouble();
    }

    _GET_STRING_ATTRIBUTE("PBandRFMax", "P band RF upper limit (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("PBandRFMax");
            throw dummy;
        }
        m_PBandRFMax[k] = token.ToDouble();
    }

    for (WORD k=0; k<m_IFs; k++)
        m_PBandIFBandwidth[k] = m_PBandRFMax[k] - m_PBandRFMin[k];

    _GET_STRING_ATTRIBUTE("LBandIFMin","L band IF start frequency (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("LBandIFMin");
            throw dummy;
        }
        m_LBandIFMin[k] = token.ToDouble();
    }

    _GET_STRING_ATTRIBUTE("PBandIFMin","P band IF start frequency (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("PBandIFMin");
            throw dummy;
        }
        m_PBandIFMin[k] = token.ToDouble();
    }

    _GET_STRING_ATTRIBUTE("DefaultLO", "Default LO Value (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("DefaultLO");
            throw dummy;
        }
        m_DefaultLO[k] = token.ToDouble();
    }


    _GET_STRING_ATTRIBUTE("FixedLO2", "LO2 Value (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("FixedLO2");
            throw dummy;
        }
        m_FixedLO2[k] = token.ToDouble();
    }


    _GET_STRING_ATTRIBUTE("LOMin", "Minumum LO Value (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("LOMin");
            throw dummy;
        }
        m_LOMin[k] = token.ToDouble();
    }


    _GET_STRING_ATTRIBUTE("LOMax", "Maximum LO Value (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("LOMax");
            throw dummy;
        }
        m_LOMax[k] = token.ToDouble();
    }

    _GET_STRING_ATTRIBUTE("LowpassFilterMin", "Minimum lowpass filter value (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("LowpassFilterMin");
            throw dummy;
        }
        m_LowpassFilterMin[k] = token.ToDouble();
    }

    _GET_STRING_ATTRIBUTE("LowpassFilterMax", "Maximum lowpass filter value (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("LowpassFilterMax");
            throw dummy;
        }
        m_LowpassFilterMax[k] = token.ToDouble();
    }


    // TODO: commented during the LO integration (27 oct 2015)
    // for (WORD k=0; k<m_IFs; k++)
    //     m_LBandLO[k] = m_LBandRFMin[k] - m_LBandIFMin[k];

    // for (WORD k=0; k<m_IFs; k++)
    //     m_PBandLO[k] = m_PBandRFMin[k] - m_PBandIFMin[k];


    _GET_DWORD_ATTRIBUTE("LBandFilterID", "L band filter ID:", m_LBandFilterID, MODE_PATH);
    _GET_DWORD_ATTRIBUTE("PBandFilterID", "P band filter ID:", m_PBandFilterID, MODE_PATH);

    // for (WORD k=0; k<cmdMode.GetLength(); k++)
    //     m_mode.SetAt(k, cmdMode[k]); */
    m_mode = cmdMode;
}


/*std::vector<double> CConfiguration::getLBandRFMinFromMode(IRA::CString cmdMode) throw (
        ComponentErrors::CDBAccessExImpl,
        ComponentErrors::MemoryAllocationExImpl, 
        ReceiversErrors::ModeErrorExImpl)
{
    CString MODE_PATH((std::string(CONFIG_PATH) + std::string("/Modes/") + std::string(cmdMode)).c_str());

    std::vector<double> rfMin;
    IRA::CString value, token;

    maci::ContainerServices *Services = m_services;
    _GET_STRING_ATTRIBUTE("LBandRFMin", "L band RF lower limit (MHz):", value, MODE_PATH);
    int start = 0;
    IRA::CError error;
    for (WORD k=0; k<m_IFs; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("LBandRFMin");
            throw dummy;
        }
        rfMin.push_back(token.ToDouble());
    }
    return rfMin;
}*/


/*std::vector<double> CConfiguration::getLBandRFMaxFromMode(IRA::CString cmdMode) throw (
        ComponentErrors::CDBAccessExImpl,
        ComponentErrors::MemoryAllocationExImpl, 
        ReceiversErrors::ModeErrorExImpl)
{
    CString MODE_PATH((std::string(CONFIG_PATH) + std::string("/Modes/") + std::string(cmdMode)).c_str());

    std::vector<double> rfMax;
    IRA::CString value, token;

    maci::ContainerServices *Services = m_services;
    _GET_STRING_ATTRIBUTE("LBandRFMax", "L band RF upper limit (MHz):", value, MODE_PATH);
    int start = 0;
    IRA::CError error;
    for (WORD k=0; k<m_IFs; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("LBandRFMax");
            throw dummy;
        }
        rfMax.push_back(token.ToDouble());
    }
    return rfMax;
}*/

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
DWORD CConfiguration<T>::getLeftMarkTable(double *& freq, double *& markValue, short feed) const
{
	freq = new double [m_markVectorLen];
	markValue = new double [m_markVectorLen];
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
DWORD CConfiguration<T>::getRightMarkTable(double *& freq,double *& markValue, short feed) const
{
	freq= new double [m_markVectorLen];
	markValue=new double [m_markVectorLen];
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
