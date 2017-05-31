// $Id: Configuration.cpp,v 1.5 2011-06-01 18:24:44 a.orlati Exp $

#include "Configuration.h"
#include <string>

using namespace IRA;

#define _GET_DOUBLE_ATTRIBUTE(ATTRIB,DESCR,FIELD,NAME) { \
    double tmpd; \
    if (!CIRATools::getDBValue(Services,ATTRIB,tmpd,"alma/",NAME)) { \
        _EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"CConfiguration::Init()"); \
        dummy.setFieldName(ATTRIB); \
        throw dummy; \
    } \
    else { \
        FIELD=tmpd; \
        ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %lf",tmpd); \
    } \
}

#define _GET_DWORD_ATTRIBUTE(ATTRIB,DESCR,FIELD,NAME) { \
    DWORD tmpw; \
    if (!CIRATools::getDBValue(Services,ATTRIB,tmpw,"alma/",NAME)) { \
        _EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"CConfiguration::Init()"); \
        dummy.setFieldName(ATTRIB); \
        throw dummy; \
    } \
    else { \
        FIELD=tmpw; \
        ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %u",tmpw); \
    } \
}

#define _GET_STRING_ATTRIBUTE(ATTRIB,DESCR,FIELD,NAME) { \
    CString tmps; \
    if (!CIRATools::getDBValue(Services,ATTRIB,tmps,"alma/",NAME)) { \
        _EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"::CConfiguration::Init()"); \
        dummy.setFieldName(ATTRIB); \
        throw dummy; \
    } \
    else { \
        FIELD=tmps; \
        ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %s",(const char*)tmps); \
    } \
}

#define CONFIG_PATH "DataBlock/SRTLPBandReceiver"
#define LOTABLE_PATH CONFIG_PATH"/Synthesizer"
#define MARKTABLE_PATH CONFIG_PATH"/NoiseMark"
#define FEEDTABLE_PATH CONFIG_PATH"/Feeds"
#define TAPERTABLE_PATH CONFIG_PATH"/Taper"


CConfiguration::CConfiguration()
{
    m_markVector = NULL;
    m_markVectorLen = 0;
    m_loTable=NULL;
    m_loVector=NULL;
    m_loVectorLen=0;
    m_LBandPolarizations = NULL;
    m_PBandPolarizations = NULL;
    m_feedsTable = NULL;
    m_feedVector = NULL;
    m_taperTable = NULL;
    m_taperVector = NULL;
    m_taperVectorLen = 0;
    m_LBandRFMin = m_PBandRFMin = NULL;
    m_LBandRFMax = m_PBandRFMax = NULL;
    m_LBandIFMin = m_PBandIFMin = NULL;
    m_LBandLO = m_PBandLO = NULL;
    m_LBandIFBandwidth = m_PBandIFBandwidth = NULL;
    m_DefaultLO = m_FixedLO2= m_LOMin = m_LOMax = m_LowpassFilterMin = m_LowpassFilterMax = NULL; 
}

CConfiguration::~CConfiguration()
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
    if (m_LBandPolarizations) {
        delete [] m_LBandPolarizations;
    }
    if (m_PBandPolarizations) {
        delete [] m_PBandPolarizations;
    }
    if (m_feedVector) {
        delete [] m_feedVector;
    }
    if (m_LBandRFMin) {
        delete [] m_LBandRFMin;
    }
    if (m_PBandRFMin) {
        delete [] m_PBandRFMin;
    }
    if (m_LBandRFMax) {
        delete [] m_LBandRFMax;
    }
    if (m_PBandRFMax) {
        delete [] m_PBandRFMax;
    }
    if (m_LBandIFMin) {
        delete [] m_LBandIFMin;
    }
    if (m_PBandIFMin) {
        delete [] m_PBandIFMin;
    }
    if (m_LBandIFBandwidth) {
        delete [] m_LBandIFBandwidth;
    }
    if (m_PBandIFBandwidth) {
        delete [] m_PBandIFBandwidth;
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
    if (m_LowpassFilterMin) {
        delete [] m_LowpassFilterMin;
    }
    if (m_LowpassFilterMax) {
        delete [] m_LowpassFilterMax;
    }
}

void CConfiguration::init(maci::ContainerServices *Services) throw (
        ComponentErrors::CDBAccessExImpl,
        ComponentErrors::MemoryAllocationExImpl, 
        ReceiversErrors::ModeErrorExImpl
        )
{
    m_services = Services;
    IRA::CError error;
    IRA::CString field;
    WORD len;
    // read component configuration
    _GET_STRING_ATTRIBUTE("DewarIPAddress", "Dewar IP address:", m_dewarIPAddress,"");
    _GET_STRING_ATTRIBUTE("LNAIPAddress", "LNA IP address:", m_LNAIPAddress,"");
	_GET_STRING_ATTRIBUTE("LocalOscillatorInstance","Local oscillator instance:",m_localOscillatorInstance,"");
    _GET_STRING_ATTRIBUTE("SwitchIPAddress", "Switch board IP address:", m_switchIPAddress,"");
    _GET_DWORD_ATTRIBUTE("DewarPort", "Dewar port:", m_dewarPort,"");
    _GET_DWORD_ATTRIBUTE("LNAPort", "LNA port:", m_LNAPort,"");
    _GET_DWORD_ATTRIBUTE("SwitchPort", "Switch board port:", m_switchPort,"");
    _GET_DWORD_ATTRIBUTE("WatchDogResponseTime", "Response time of watch dog thread (uSec):", m_watchDogResponseTime,"");
    _GET_DWORD_ATTRIBUTE("WatchDogSleepTime", "Sleep time of the watch dog thread (uSec):", m_watchDogSleepTime,"");
    _GET_DWORD_ATTRIBUTE("LNASamplingTime", "Time needed to collect LNA information from control boards (uSec):", m_LNASamplingTime,"");
    _GET_DWORD_ATTRIBUTE("RepetitionCacheTime", "Log repetition filter, caching time (uSec):", m_repetitionCacheTime,"");
    _GET_DWORD_ATTRIBUTE("RepetitionExpireTime", "Log repetition filter, expire time (uSec):", m_repetitionExpireTime,"");
    _GET_STRING_ATTRIBUTE("DefaultMode", "Default operating mode:", m_defaultMode,"");

    const IRA::CString DEFAULTMODE_PATH = CONFIG_PATH"/Modes/" + m_defaultMode;
    // now read the receiver configuration

    _GET_STRING_ATTRIBUTE("Mode","mode name:", m_mode, DEFAULTMODE_PATH);
    _GET_DWORD_ATTRIBUTE("Feeds","Number of feeds:", m_feeds, DEFAULTMODE_PATH);
    _GET_DWORD_ATTRIBUTE("IFs","Number of IFs for each feed:", m_IFs, DEFAULTMODE_PATH);
    _GET_DWORD_ATTRIBUTE("LBandFilterID","L band filter ID:", m_LBandFilterID, DEFAULTMODE_PATH);
    _GET_DWORD_ATTRIBUTE("PBandFilterID","P band filter ID:", m_PBandFilterID, DEFAULTMODE_PATH);

    try {
        m_LBandPolarizations = new Receivers::TPolarization[m_IFs];
        m_PBandPolarizations = new Receivers::TPolarization[m_IFs];

        m_LBandRFMin = new double[m_IFs];
        m_PBandRFMin = new double[m_IFs];

        m_LBandRFMax = new double[m_IFs];
        m_PBandRFMax = new double[m_IFs];

        m_LBandIFMin = new double[m_IFs];
        m_PBandIFMin = new double[m_IFs];

        m_LBandLO = new double[m_IFs];
        m_PBandLO = new double[m_IFs];

        m_LBandIFBandwidth = new double[m_IFs];
        m_PBandIFBandwidth = new double[m_IFs];

        m_DefaultLO = new double[m_IFs];
        m_FixedLO2 = new double[m_IFs];
        m_LOMin = new double[m_IFs];
        m_LOMax = new double[m_IFs];
        m_LowpassFilterMin = new double[m_IFs];
        m_LowpassFilterMax = new double[m_IFs];
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "CConfiguration::init()");
        throw dummy;
    }

    // Set the default operating mode
    setMode(m_mode); 

    // The noise mark
    try {
        m_markTable = new IRA::CDBTable(Services, "MarkEntry", MARKTABLE_PATH);
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy, "CConfiguration::init()");
        throw dummy;
    }
    error.Reset();
    if (!m_markTable->addField(error, "Feed", IRA::CDataField::LONGLONG)) {
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
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
        dummy.setFieldName((const char *)field);
        throw dummy;
    }
    if (!m_markTable->openTable(error)) {
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
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
        ACS_LOG(LM_FULL_INFO, 
                "CConfiguration::init()",
                (
                     LM_DEBUG,
                     "SYNTH_VALUE_ENTRY: %lld %lf %lf",
                     m_taperVector[i].feed,
                     m_taperVector[i].frequency,
                     m_taperVector[i].taper
                 )
        );
        m_taperTable->Next();
    }
    m_taperVectorLen = len;
    m_taperTable->closeTable();
    delete m_taperTable;
    m_taperTable = NULL;

}

void CConfiguration::setMode(const char * mode) throw (
        ComponentErrors::CDBAccessExImpl, 
        ReceiversErrors::ModeErrorExImpl
        ) 
{
    IRA::CString value, token;
    IRA::CError error;
    IRA::CString cmdMode(mode);
    cmdMode.MakeUpper();

    if(!m_mode.IsEmpty()){
        if(cmdMode.GetLength() != m_mode.GetLength()) {
            _EXCPT(ReceiversErrors::ModeErrorExImpl, impl, 
                    "CConfiguration::setMode(): Wrong number of characters in the commanded mode."
            );
            ACS_LOG(LM_FULL_INFO, "CConfiguration::init()", (LM_DEBUG, "Wrong number of characters in " + m_mode));

            throw impl; 
        }
    }

    if(cmdMode.Find('*') != -1) {
        for (WORD k=0; k<m_mode.GetLength(); k++) {
            if(cmdMode[k] == '*') {
                cmdMode.SetAt(k, m_mode[k]); // Don not change this configuration
            }
        }
    }

    maci::ContainerServices* Services = m_services;
    CString MODE_PATH((std::string(CONFIG_PATH) + std::string("/Modes/") + std::string(cmdMode)).c_str());
    _GET_DWORD_ATTRIBUTE("Feeds","Number of feeds:", m_feeds, MODE_PATH);

    _GET_STRING_ATTRIBUTE("LBandPolarization", "LBand IF polarization:", value, MODE_PATH);
    int start = 0;
    for (WORD k=0; k<m_IFs; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("LBandPolarization");
            throw dummy;
        }
        token.MakeUpper();
        if (token == "L") {
            m_LBandPolarizations[k] = Receivers::RCV_LCP;
            m_LBandPolarization = "C"; 
        }
        else if (token == "R") {
            m_LBandPolarizations[k] = Receivers::RCV_RCP;
            m_LBandPolarization = "C"; 
        }
        else if (token == "V") {
            m_LBandPolarizations[k] = Receivers::RCV_VLP;
            m_LBandPolarization = "L"; 
        }
        else if (token == "H") {
            m_LBandPolarizations[k] = Receivers::RCV_HLP;
            m_LBandPolarization = "L"; 
        }
        else {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("LBandPolarization");
            throw dummy;
        }
    }

    _GET_STRING_ATTRIBUTE("PBandPolarization", "PBand IF polarization:", value, MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("PBandPolarization");
            throw dummy;
        }
        token.MakeUpper();
        if (token == "L") {
            m_PBandPolarizations[k] = Receivers::RCV_LCP;
            m_PBandPolarization = "C"; 
        }
        else if (token == "R") {
            m_PBandPolarizations[k] = Receivers::RCV_RCP;
            m_PBandPolarization = "C"; 
        }
        else if (token == "V") {
            m_PBandPolarizations[k] = Receivers::RCV_VLP;
            m_PBandPolarization = "L"; 
        }
        else if (token == "H") {
            m_PBandPolarizations[k] = Receivers::RCV_HLP;
            m_PBandPolarization = "L"; 
        }
        else {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("PBandPolarization");
            throw dummy;
        }
    }

    _GET_STRING_ATTRIBUTE("LBandRFMin", "L band RF lower limit (MHz):", value, MODE_PATH);
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
    //     m_mode.SetAt(k, cmdMode[k]); 
    m_mode = cmdMode;
}


std::vector<double> CConfiguration::getLBandRFMinFromMode(IRA::CString cmdMode) throw (
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
}


std::vector<double> CConfiguration::getLBandRFMaxFromMode(IRA::CString cmdMode) throw (
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
}


DWORD CConfiguration::getSynthesizerTable(double * &freq,double *&power) const
{
    freq= new double [m_loVectorLen];
    power=new double [m_loVectorLen];
    for (DWORD j=0;j<m_loVectorLen;j++) {
        freq[j]=m_loVector[j].frequency;
        power[j]=m_loVector[j].outputPower;
    }
    return m_loVectorLen;
}


DWORD CConfiguration::getTaperTable(double * &freq,double *&taper, short feed) const
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
    return m_taperVectorLen;
}

DWORD CConfiguration::getLeftMarkTable(double *& freq, double *& markValue, short feed) const
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

DWORD CConfiguration::getRightMarkTable(double *& freq,double *& markValue, short feed) const
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



DWORD CConfiguration::getFeedInfo(
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
