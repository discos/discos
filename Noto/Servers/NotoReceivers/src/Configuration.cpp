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

#define CONFIG_PATH "DataBlock/"

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
	m_loMultiplier=NULL;
	m_RFMin=m_RFMax=m_IFMin=m_IFBandwidth=m_defaultLO=m_fixedLO2=m_LOMin=m_LOMax=NULL;
}

CConfiguration::~CConfiguration()
{
	freeAll();
}

void CConfiguration::init(maci::ContainerServices *Services) throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl)
{
	// read component configuration
	_GET_STRING_ATTRIBUTE("HPIBIPAddress","HPIB device IP address:",m_HPIBIPAddress,"");
	_GET_DWORD_ATTRIBUTE("HPIBPort","HPIB device port:",m_HPIBPort,"");
	_GET_STRING_ATTRIBUTE("LocalOscillatorInstance","Local oscillator instance:",m_localOscillatorInstance,"");
	_GET_STRING_ATTRIBUTE("LocalOscillatorEnabledReceivers","Receivers with a Local oscillator:",m_LocalOscillatorEnabledReceivers,"");
	_GET_STRING_ATTRIBUTE("FocusSelectorInterface","Focus Selector instance:",m_fsInterface,"");
	m_IFs=2;
	m_feeds=1;
	m_mode="";
}

void CConfiguration::loadConf(maci::ContainerServices *Services,const IRA::CString& conf) throw (
		ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl)
{
	IRA::CError error;
	IRA::CString field,value,token;
	WORD len;
	int start=0;
	IRA::CString normalMode_Path;
	IRA::CString markTable_Path;
	IRA::CString loTable_Path;
	IRA::CString feedTable_Path;
	IRA::CString taperTable_Path;

	normalMode_Path=IRA::CString(CONFIG_PATH)+conf+IRA::CString("/NormalModeSetup");
	markTable_Path=CString(CONFIG_PATH)+conf+IRA::CString("/NoiseMark");
	loTable_Path=CString(CONFIG_PATH)+conf+IRA::CString("/Synthesizer");
	feedTable_Path=CString(CONFIG_PATH)+conf+IRA::CString("/Feeds");
	taperTable_Path=CString(CONFIG_PATH)+conf+IRA::CString("/Taper");

	// now read the receiver configuration
	_GET_STRING_ATTRIBUTE("Mode","mode name:",m_mode,normalMode_Path);
	_GET_DWORD_ATTRIBUTE("Feeds","Number of feeds:",m_feeds,normalMode_Path);
	_GET_DWORD_ATTRIBUTE("IFs","Number of IFs per feed:",m_IFs,normalMode_Path);
	freeAll();
	try {
		m_polarizations=new Receivers::TPolarization[m_IFs];
		m_RFMin=new double[m_IFs];
		m_RFMax=new double[m_IFs];
		m_IFMin=new double[m_IFs];
		m_IFBandwidth=new double[m_IFs];
		m_defaultLO=new double[m_IFs];
		m_loMultiplier=new long[m_IFs];
		m_fixedLO2=new double[m_IFs];
		m_LOMin=new double[m_IFs];
		m_LOMax=new double[m_IFs];
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::loadConf()");
		throw dummy;
	}
	_GET_STRING_ATTRIBUTE("Polarization","IF polarization:",value,normalMode_Path);
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
	_GET_STRING_ATTRIBUTE("RFMin","RF lower limit (MHz):",value,normalMode_Path);
	start=0;
	for (WORD k=0;k<m_IFs;k++) {
		if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
			dummy.setFieldName("RFMin");
			throw dummy;
		}
		m_RFMin[k]=token.ToDouble();
	}
	_GET_STRING_ATTRIBUTE("RFMax","RF upper limit (MHz):",value,normalMode_Path);
	start=0;
	for (WORD k=0;k<m_IFs;k++) {
		if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
			dummy.setFieldName("RFMax");
			throw dummy;
		}
		m_RFMax[k]=token.ToDouble();
	}
	_GET_STRING_ATTRIBUTE("IFMin","IF start frequency (MHz):",value,normalMode_Path);
	start=0;
	for (WORD k=0;k<m_IFs;k++) {
		if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
			dummy.setFieldName("IFMin");
			throw dummy;
		}
		m_IFMin[k]=token.ToDouble();
	}
	_GET_STRING_ATTRIBUTE("IFBandwidth","IF bandwidth (MHz):",value,normalMode_Path);
	start=0;
	for (WORD k=0;k<m_IFs;k++) {
		if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
			dummy.setFieldName("IFBandwidth");
			throw dummy;
		}
		m_IFBandwidth[k]=token.ToDouble();
	}
	_GET_STRING_ATTRIBUTE("DefaultLO","Default local oscillator (MHz):",value,normalMode_Path);
	start=0;
	for (WORD k=0;k<m_IFs;k++) {
		if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
			dummy.setFieldName("DefaultLO");
			throw dummy;
		}
		m_defaultLO[k]=token.ToDouble();
	}
	_GET_STRING_ATTRIBUTE("LOMultiplier","Local oscillator multiplier:",value,normalMode_Path);
	start=0;
	for (WORD k=0;k<m_IFs;k++) {
		if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
			dummy.setFieldName("LOMultiplier");
			throw dummy;
		}
		m_loMultiplier[k]=token.ToLong();
	}
	_GET_STRING_ATTRIBUTE("FixedLO2","Second fixed local oscillator value (MHz):",value,normalMode_Path);
	start=0;
	for (WORD k=0;k<m_IFs;k++) {
		if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
			dummy.setFieldName("FixedLO2");
			throw dummy;
		}
		m_fixedLO2[k]=token.ToDouble();
	}
	_GET_STRING_ATTRIBUTE("LOMin","Local oscillator minimum allowed value (MHz):",value,normalMode_Path);
	start=0;
	for (WORD k=0;k<m_IFs;k++) {
		if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
			dummy.setFieldName("LOMin");
			throw dummy;
		}
		m_LOMin[k]=token.ToDouble();
	}
	_GET_STRING_ATTRIBUTE("LOMax","Local oscillator maximum allowed value (MHz):",value,normalMode_Path);
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
		m_markTable=new IRA::CDBTable(Services,"MarkEntry",markTable_Path);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::loadConf()");
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
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::loadConf()");
		throw dummy;
	}
	ACS_LOG(LM_FULL_INFO,"CConfiguration::loadConf()",(LM_DEBUG,"MARK_VALUE_ENTRY_NUMBER: %d",len));
	for (WORD i=0;i<len;i++) {
		m_markVector[i].skyFrequency=(*m_markTable)["SkyFrequency"]->asDouble();
		m_markVector[i].markValue=(*m_markTable)["NoiseMark"]->asDouble();
		m_markVector[i].polarization=(*m_markTable)["Polarization"]->asString()=="LEFT"?Receivers::RCV_LCP:Receivers::RCV_RCP;
		ACS_LOG(LM_FULL_INFO,"CConfiguration::loadConf()",(LM_DEBUG,"MARK_VALUE_ENTRY: %d %lf %lf",m_markVector[i].polarization,m_markVector[i].skyFrequency,
				m_markVector[i].markValue));
		m_markTable->Next();
	}
	m_markVectorLen=len;
	m_markTable->closeTable();
	delete m_markTable;
	m_markTable=NULL;
	// The synthesizer
	try {
		m_loTable=new IRA::CDBTable(Services,"SynthesizerEntry",loTable_Path);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::loadConf()");
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
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::loadConf()");
		throw dummy;
	}
	ACS_LOG(LM_FULL_INFO,"CConfiguration::loadConf()",(LM_DEBUG,"SYNTH_VALUE_ENTRY_NUMBER: %d",len));
	for (WORD i=0;i<len;i++) {
		m_loVector[i].frequency=(*m_loTable)["Frequency"]->asDouble();
		m_loVector[i].outputPower=(*m_loTable)["OutputPower"]->asDouble();
		ACS_LOG(LM_FULL_INFO,"CConfiguration::loadConf()",(LM_DEBUG,"SYNTH_VALUE_ENTRY: %lf %lf",m_loVector[i].frequency,m_loVector[i].outputPower));
		m_loTable->Next();
	}
	m_loVectorLen=len;
	m_loTable->closeTable();
	delete m_loTable;
	m_loTable=NULL;
	// The feeds
	try {
		m_feedsTable=new IRA::CDBTable(Services,"Feed",feedTable_Path);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::loadConf()");
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
		_EXCPT(ComponentErrors::CDBAccessExImpl, dummy, "CConfiguration::loadConf()");
		dummy.setFieldName("feed table size");
		throw dummy;
	}
	len=m_feeds;
	try {
		m_feedVector=new TFeedValue[len];
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::loadConf()");
		throw dummy;
	}
	for (WORD i=0;i<len;i++) {
		m_feedVector[i].xOffset=(*m_feedsTable)["xOffset"]->asDouble();
		m_feedVector[i].yOffset=(*m_feedsTable)["yOffset"]->asDouble();
		m_feedVector[i].relativePower=(*m_feedsTable)["relativePower"]->asDouble();
		m_feedVector[i].code=(WORD)(*m_feedsTable)["feedCode"]->asLongLong();
		ACS_LOG(LM_FULL_INFO,"CConfiguration::loadConf()",(LM_DEBUG,"FEED_VALUE_ENTRY: %d %lf %lf %lf",m_feedVector[i].code,m_feedVector[i].xOffset,m_feedVector[i].yOffset,m_feedVector[i].relativePower));
		m_feedsTable->Next();
	}
	m_feedsTable->closeTable();
	delete m_feedsTable;
	m_feedsTable=NULL;
	//The taper.....
	try {
		m_taperTable=new IRA::CDBTable(Services,"TaperEntry",taperTable_Path);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::loadConf()");
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
	if (!m_taperTable->openTable(error)) {
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
		throw dummy;
	}
	m_taperTable->First();
	len=m_taperTable->recordCount();
	try {
		m_taperVector=new TTaperValue[len];
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::loadConf()");
		throw dummy;
	}
	ACS_LOG(LM_FULL_INFO,"CConfiguration::loadConf()",(LM_DEBUG,"TAPER_ENTRY_NUMBER: %d",len));
	for (WORD i=0;i<len;i++) {
		m_taperVector[i].frequency=(*m_taperTable)["Frequency"]->asDouble();
		m_taperVector[i].taper=(*m_taperTable)["Taper"]->asDouble();
		ACS_LOG(LM_FULL_INFO,"CConfiguration::loadConf()",(LM_DEBUG,"SYNTH_VALUE_ENTRY: %lf %lf",m_taperVector[i].frequency,m_taperVector[i].taper));
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

void CConfiguration::freeAll()
{
	if (m_markTable) {
		delete m_markTable;
		m_markTable=NULL;
	}
	if (m_loTable) {
		delete m_loTable;
		m_loTable=NULL;
	}
	if (m_feedsTable) {
		delete m_feedsTable;
		m_feedsTable=NULL;
	}
	if (m_taperTable) {
		delete m_taperTable;
		m_taperTable=NULL;
	}
	if (m_markVector) {
		delete [] m_markVector;
		m_markVector=NULL;
	}
	if (m_loVector) {
		delete [] m_loVector;
		m_loVector=NULL;
	}
	if (m_taperVector) {
		delete [] m_taperVector;
		m_taperVector=NULL;
	}
	if (m_polarizations) {
		delete [] m_polarizations;
		m_polarizations=NULL;
	}
	if (m_feedVector) {
		delete [] m_feedVector;
		m_feedVector=NULL;
	}
	if (m_RFMin) {
		delete [] m_RFMin;
		m_RFMin=NULL;
	}
	if (m_RFMax) {
		delete [] m_RFMax;
		m_RFMax=NULL;
	}
	if (m_IFMin) {
		delete [] m_IFMin;
		m_IFMin=NULL;
	}
	if (m_IFBandwidth) {
		delete [] m_IFBandwidth;
		m_IFBandwidth=NULL;
	}
	if (m_defaultLO) {
		delete [] m_defaultLO;
		m_defaultLO=NULL;
	}
	if (m_loMultiplier) {
		delete [] m_loMultiplier;
		m_loMultiplier=NULL;
	}
	if (m_fixedLO2) {
		delete [] m_fixedLO2;
		m_fixedLO2=NULL;
	}
	if (m_LOMin) {
		delete [] m_LOMin;
		m_LOMin=NULL;
	}
	if (m_LOMax) {
		delete [] m_LOMax;
		m_LOMax=NULL;
	}
}


