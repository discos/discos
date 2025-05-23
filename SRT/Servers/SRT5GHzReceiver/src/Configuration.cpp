
#include "Configuration.h"

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

#define CONFIG_PATH "DataBlock/SRT5GHzReceiver"
#define LOTABLE_PATH CONFIG_PATH"/Synthesizer"
#define MARKTABLE_PATH CONFIG_PATH"/NoiseMark"
#define NORMALMODE_PATH CONFIG_PATH"/NormalModeSetup"
#define FEEDTABLE_PATH CONFIG_PATH"/Feeds"
#define TAPERTABLE_PATH CONFIG_PATH"/Taper"


CConfiguration::CConfiguration()
{
	m_markTable=m_loTable=NULL;
	m_markVector=NULL;
	m_markVectorLen=0;
	m_loTable=NULL;
	m_loVector=NULL;
	m_loVectorLen=0;
	m_polarizations=NULL;
	m_feedsTable=NULL;
	m_feedVector=NULL;
	m_taperTable=NULL;
	m_taperVector=NULL;
	m_taperVectorLen=0;
	m_RFMin=m_RFMax=m_IFMin=m_IFBandwidth=m_defaultLO=m_fixedLO2=m_LOMin=m_LOMax=NULL;
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
	if (m_polarizations) {
		delete [] m_polarizations;
	}
	if (m_feedVector) {
		delete [] m_feedVector;
	}
	if (m_RFMin) {
		delete [] m_RFMin;
	}
	if (m_RFMax) {
		delete [] m_RFMax;
	}
	if (m_IFMin) {
		delete [] m_IFMin;
	}
	if (m_IFBandwidth) {
		delete [] m_IFBandwidth;
	}
	if (m_defaultLO) {
		delete [] m_defaultLO;
	}
	if (m_fixedLO2) {
		delete [] m_fixedLO2;
	}
	if (m_LOMin) {
		delete [] m_LOMin;
	}
	if (m_LOMax) {
		delete [] m_LOMax;
	}
}
// throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl)
void CConfiguration::init(maci::ContainerServices *Services)
{
	IRA::CError error;
	IRA::CString field,value,token;
	WORD len;
	int start=0;
	// read component configuration
	_GET_STRING_ATTRIBUTE("DewarIPAddress","Dewar IP address:",m_dewarIPAddress,"");
	_GET_STRING_ATTRIBUTE("LNAIPAddress","LNA IP address:",m_LNAIPAddress,"");
	_GET_STRING_ATTRIBUTE("LocalOscillatorInstance","Local oscillator instance:",m_localOscillatorInstance,"");
	_GET_DWORD_ATTRIBUTE("DewarPort","Dewar port:",m_dewarPort,"");
	_GET_DWORD_ATTRIBUTE("LNAPort","LNA port:",m_LNAPort,"");
	_GET_DWORD_ATTRIBUTE("WatchDogResponseTime","Response time of watch dog thread (uSec):",m_watchDogResponseTime,"");
	_GET_DWORD_ATTRIBUTE("WatchDogSleepTime","Sleep time of the watch dog thread (uSec):",m_watchDogSleepTime,"");
	_GET_DWORD_ATTRIBUTE("LNASamplingTime","Time needed to collect LNA information from control boards (uSec):",m_LNASamplingTime,"");
	_GET_DWORD_ATTRIBUTE("RepetitionCacheTime","Log repetition filter, caching time (uSec):",m_repetitionCacheTime,"");
	_GET_DWORD_ATTRIBUTE("RepetitionExpireTime","Log repetition filter, expire time (uSec):",m_repetitionExpireTime,"");
	_GET_STRING_ATTRIBUTE("LocalOscillatorInstance","Local oscillator instance:",m_localOscillatorInstance,"");
	// now read the receiver configuration
	_GET_STRING_ATTRIBUTE("Mode","mode name:",m_mode,NORMALMODE_PATH);
	_GET_DWORD_ATTRIBUTE("Feeds","Number of feeds:",m_feeds,NORMALMODE_PATH);
	_GET_DWORD_ATTRIBUTE("IFs","Number of IFs per feed:",m_IFs,NORMALMODE_PATH);
	try {
		m_polarizations=new Receivers::TPolarization[m_IFs];
		m_RFMin=new double[m_IFs];
		m_RFMax=new double[m_IFs];
		m_IFMin=new double[m_IFs];
		m_IFBandwidth=new double[m_IFs];
		m_defaultLO=new double[m_IFs];
		m_fixedLO2=new double[m_IFs];
		m_LOMin=new double[m_IFs];
		m_LOMax=new double[m_IFs];
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::init()");
		throw dummy;
	}
	_GET_STRING_ATTRIBUTE("Polarization","IF polarization:",value,NORMALMODE_PATH);
	start=0;
	for (WORD k=0;k<m_IFs;k++) {
		if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
			dummy.setFieldName("Polarization");
			throw dummy;
		}
		token.MakeUpper();
		if (token=="L") {
			m_polarizations[k]=Receivers::RCV_LCP;
		}
		else if (token=="R") {
			m_polarizations[k]=Receivers::RCV_RCP;
		}
		else {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
			dummy.setFieldName("Polarization");
			throw dummy;
		}
	}
	_GET_STRING_ATTRIBUTE("RFMin","RF lower limit (MHz):",value,NORMALMODE_PATH);
	start=0;
	for (WORD k=0;k<m_IFs;k++) {
		if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
			dummy.setFieldName("RFMin");
			throw dummy;
		}
		m_RFMin[k]=token.ToDouble();
	}
	_GET_STRING_ATTRIBUTE("RFMax","RF upper limit (MHz):",value,NORMALMODE_PATH);
	start=0;
	for (WORD k=0;k<m_IFs;k++) {
		if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
			dummy.setFieldName("RFMax");
			throw dummy;
		}
		m_RFMax[k]=token.ToDouble();
	}
	_GET_STRING_ATTRIBUTE("IFMin","IF start frequency (MHz):",value,NORMALMODE_PATH);
	start=0;
	for (WORD k=0;k<m_IFs;k++) {
		if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
			dummy.setFieldName("IFMin");
			throw dummy;
		}
		m_IFMin[k]=token.ToDouble();
	}
	_GET_STRING_ATTRIBUTE("IFBandwidth","IF bandwidth (MHz):",value,NORMALMODE_PATH);
	start=0;
	for (WORD k=0;k<m_IFs;k++) {
		if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
			dummy.setFieldName("IFBandwidth");
			throw dummy;
		}
		m_IFBandwidth[k]=token.ToDouble();
	}
	_GET_STRING_ATTRIBUTE("DefaultLO","Default local oscillator (MHz):",value,NORMALMODE_PATH);
	start=0;
	for (WORD k=0;k<m_IFs;k++) {
		if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
			dummy.setFieldName("DefaultLO");
			throw dummy;
		}
		m_defaultLO[k]=token.ToDouble();
	}
	_GET_STRING_ATTRIBUTE("FixedLO2","Second fixed local oscillator value (MHz):",value,NORMALMODE_PATH);
	start=0;
	for (WORD k=0;k<m_IFs;k++) {
		if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
			dummy.setFieldName("FixedLO2");
			throw dummy;
		}
		m_fixedLO2[k]=token.ToDouble();
	}
	_GET_STRING_ATTRIBUTE("LOMin","Local oscillator minimum allowed value (MHz):",value,NORMALMODE_PATH);
	start=0;
	for (WORD k=0;k<m_IFs;k++) {
		if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
			dummy.setFieldName("LOMin");
			throw dummy;
		}
		m_LOMin[k]=token.ToDouble();
	}
	_GET_STRING_ATTRIBUTE("LOMax","Local oscillator maximum allowed value (MHz):",value,NORMALMODE_PATH);
	start=0;
	for (WORD k=0;k<m_IFs;k++) {
		if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
			dummy.setFieldName("LOMax");
			throw dummy;
		}
		m_LOMax[k]=token.ToDouble();
	}
	// The noise mark
	try {
		m_markTable=new IRA::CDBTable(Services,"MarkEntry",MARKTABLE_PATH);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::init()");
		throw dummy;
	}
	error.Reset();
	if (!m_markTable->addField(error,"Polarization",IRA::CDataField::STRING)) {
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
	if (!m_markTable->openTable(error))	{
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
		throw dummy;
	}
	m_markTable->First();
	len=m_markTable->recordCount();
	try {
		m_markVector=new TMarkValue[len];
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::init()");
		throw dummy;
	}
	ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_DEBUG,"MARK_VALUE_ENTRY_NUMBER: %d",len));
	for (WORD i=0;i<len;i++) {
		m_markVector[i].skyFrequency=(*m_markTable)["SkyFrequency"]->asDouble();
		m_markVector[i].markValue=(*m_markTable)["NoiseMark"]->asDouble();
		m_markVector[i].polarization=(*m_markTable)["Polarization"]->asString()=="LEFT"?Receivers::RCV_LCP:Receivers::RCV_RCP;
		ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_DEBUG,"MARK_VALUE_ENTRY: %d %lf %lf",m_markVector[i].polarization,m_markVector[i].skyFrequency,
				m_markVector[i].markValue));
		m_markTable->Next();
	}
	m_markVectorLen=len;
	m_markTable->closeTable();
	delete m_markTable;
	m_markTable=NULL;
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
	if (!m_loTable->openTable(error))	{
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
		m_feedsTable=new IRA::CDBTable(Services,"Feed",FEEDTABLE_PATH);
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
	m_feedsTable->First();
	if (m_feeds!=m_feedsTable->recordCount()) {
		_EXCPT(ComponentErrors::CDBAccessExImpl, dummy, "CConfiguration::init()");
		dummy.setFieldName("feed table size");
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
		m_taperTable=new IRA::CDBTable(Services,"TaperEntry",TAPERTABLE_PATH);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::init()");
		throw dummy;
	}
	error.Reset();
	if (!m_taperTable->addField(error,"Frequency",IRA::CDataField::DOUBLE)) {
		field="Frequency";
 	}
	else if (!m_taperTable->addField(error,"Taper",IRA::CDataField::DOUBLE)) {
		field="OutputPower";
 	}
	if (!error.isNoError()) {
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,error);
		dummy.setFieldName((const char *)field);
		throw dummy;
	}
	if (!m_taperTable->openTable(error))	{
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
		throw dummy;
	}
	m_taperTable->First();
	len=m_taperTable->recordCount();
	try {
		m_taperVector=new TTaperValue[len];
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::init()");
		throw dummy;
	}
	ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_DEBUG,"TAPER_ENTRY_NUMBER: %d",len));
	for (WORD i=0;i<len;i++) {
		m_taperVector[i].frequency=(*m_taperTable)["Frequency"]->asDouble();
		m_taperVector[i].taper=(*m_taperTable)["Taper"]->asDouble();
		ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_DEBUG,"SYNTH_VALUE_ENTRY: %lf %lf",m_taperVector[i].frequency,m_taperVector[i].taper));
		m_taperTable->Next();
	}
	m_taperVectorLen=len;
	m_taperTable->closeTable();
	delete m_taperTable;
	m_taperTable=NULL;
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

DWORD CConfiguration::getTaperTable(double * &freq,double *&taper) const
{
	freq= new double [m_taperVectorLen];
	taper=new double [m_taperVectorLen];
	for (DWORD j=0;j<m_taperVectorLen;j++) {
		freq[j]=m_taperVector[j].frequency;
		taper[j]=m_taperVector[j].taper;
	}
	return m_taperVectorLen;
}

DWORD CConfiguration::getLeftMarkTable(double *& freq,double *& markValue) const
{
	freq= new double [m_markVectorLen];
	markValue=new double [m_markVectorLen];
	DWORD count=0;
	for (DWORD j=0;j<m_markVectorLen;j++) {
		if (m_markVector[j].polarization==Receivers::RCV_LCP) {
			freq[count]=m_markVector[j].skyFrequency;
			markValue[count]=m_markVector[j].markValue;
			count++;
		}
	}
	return count;
}

DWORD CConfiguration::getRightMarkTable(double *& freq,double *& markValue) const
{
	freq= new double [m_markVectorLen];
	markValue=new double [m_markVectorLen];
	DWORD count=0;
	for (DWORD j=0;j<m_markVectorLen;j++) {
		if (m_markVector[j].polarization==Receivers::RCV_RCP) {
			freq[count]=m_markVector[j].skyFrequency;
			markValue[count]=m_markVector[j].markValue;
			count++;
		}
	}
	return count;
}

DWORD CConfiguration::getFeedInfo(WORD *& code,double *& xOffset,double *& yOffset,double *& relativePower) const
{
	code=new WORD[m_feeds];
	xOffset=new double [m_feeds];
	yOffset=new double [m_feeds];
	relativePower=new double [m_feeds];
	for (DWORD j=0;j<m_feeds;j++) {
		code[j]=m_feedVector[j].code;
		xOffset[j]=m_feedVector[j].xOffset;
		yOffset[j]=m_feedVector[j].yOffset;
		relativePower[j]=m_feedVector[j].relativePower;
	}
	return m_feeds;
}

