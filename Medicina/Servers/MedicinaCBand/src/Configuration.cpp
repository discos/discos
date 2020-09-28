// $Id: Configuration.cpp,v 1.5 2011-06-01 18:24:44 a.orlati Exp $

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

/**@todo Bring this path inside ReceiverConf? */
#define CONFIG_PATH "DataBlock/MedicinaCBand"
#define LOTABLE_PATH CONFIG_PATH"/Synthesizer"
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
	/* conf files*/
	CConfiguration::ConfigurationFiles l_ccc_normal;	
	CConfiguration::ConfigurationFiles l_ccc_narrow;
	CConfiguration::ConfigurationFiles l_chc_normal;
	CConfiguration::ConfigurationFiles l_chc_narrow;
	m_conf_file['CCCNormalSetup']= l_ccc_normal;
	m_conf_file['CCCNarrowSetup']= l_ccc_narrow;
	m_conf_file['CHCNormalSetup']= l_chc_normal;
	m_conf_file['CHCNarrowSetup']= l_chc_narrow;
	/* conf params */
	m_conf_param['CCCNormalSetup']= SetupParams();
	m_conf_param['CCCNarrowSetup']= SetupParams();
	m_conf_param['CHCNormalSetup']= SetupParams();
	m_conf_param['CHCNarrowSetup']= SetupParams();
}

CConfiguration::~CConfiguration()
{	
	if (m_loTable) {delete m_loTable;}
	if (m_feedsTable) {delete m_feedsTable;}
	if (m_taperTable) {delete m_taperTable;}
	if (m_markVector) {delete [] m_markVector;}
	if (m_loVector) {delete [] m_loVector;}
	if (m_taperVector) {delete [] m_taperVector;}	
	if (m_feedVector) {delete [] m_feedVector;}	
}

void CConfiguration::init(maci::ContainerServices *Services) 
				throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl)
{
	/**
	 *  Basically we look for:
	 *  * Component configuration data  at Medicina\Configuration\alma\RECEIVERS\MedicinaCBand\MedicinaCBand.xml
	 *  * Working mode configuration data:
	 * 		* CHC, C High freq receiver, NormalMode at Medicina\Configuration\alma\DataBlock\MedicinaCBand\NormalModeSetup\CHC\NormalModeSetup.xml
	 * 		* CCC, C Low freq receiver, NormalMode at Medicina\Configuration\alma\DataBlock\MedicinaCBand\NormalModeSetup\CCC\NormalModeSetup.xml
	 * 		* CHC, C High freq receiver, NarrowBw. at Medicina\Configuration\alma\DataBlock\MedicinaCBand\NarrowBandwidthSetup\CHC\NarrowBandwidthSetup.xml
	 * 		* CCC, C Low freq receiver, NarrowBw. at Medicina\Configuration\alma\DataBlock\MedicinaCBand\NarrowBandwidthSetup\CCC\NarrowBandwidthSetup.xml
	 *  Basic config attributes are set following given receiver operation mode:
	 *   * CHC Normal
	 *   * CCC Normal
	 *   * CHC NarrowBw.
	 *   * CCC NarrowBw.
	 * 
	 */	
	/* read component configuration */
	/**@todo review LO 2nd stage component readings and setup */	
	_GET_STRING_ATTRIBUTE("DewarIPAddress","Dewar IP address:",m_dewarIPAddress,"");
	_GET_STRING_ATTRIBUTE("LNAIPAddress","LNA IP address:",m_LNAIPAddress,"");
	_GET_STRING_ATTRIBUTE("LocalOscillatorInstance1st","Local oscillator instance:",m_localOscillatorInstance1st,"");
	_GET_STRING_ATTRIBUTE("LocalOscillatorInstance2nd","Local oscillator instance:",m_localOscillatorInstance2nd,"");
	_GET_DWORD_ATTRIBUTE("DewarPort","Dewar port:",m_dewarPort,"");
	_GET_DWORD_ATTRIBUTE("LNAPort","LNA port:",m_LNAPort,"");
	_GET_DWORD_ATTRIBUTE("WatchDogResponseTime","Response time of watch dog thread (uSec):",m_watchDogResponseTime,"");
	_GET_DWORD_ATTRIBUTE("WatchDogSleepTime","Sleep time of the watch dog thread (uSec):",m_watchDogSleepTime,"");
	_GET_DWORD_ATTRIBUTE("LNASamplingTime","Time needed to collect LNA information from control boards (uSec):",m_LNASamplingTime,"");
	_GET_DWORD_ATTRIBUTE("RepetitionCacheTime","Log repetition filter, caching time (uSec):",m_repetitionCacheTime,"");
	_GET_DWORD_ATTRIBUTE("RepetitionExpireTime","Log repetition filter, expire time (uSec):",m_repetitionExpireTime,"");	
	/* Setup conf files (datablock) */ 
	_GET_STRING_ATTRIBUTE("CCCNormalSetup","CCC normal setup:", m_conf_file['CCCNormalSetup'].m_setup, "");
	_GET_STRING_ATTRIBUTE("CCCNarrowSetup","CCC narrow setup:", m_conf_file['CCCNarrowSetup'].m_setup, "");
	_GET_STRING_ATTRIBUTE("CHCNormalSetup","CHC Normal setup:", m_conf_file['CHCNormalSetup'].m_setup, "");
	_GET_STRING_ATTRIBUTE("CHCNarrowSetup","CHC narrow setup:", m_conf_file['CHCNarrowSetup'].m_setup, "");		
	/* Noise mark conf files (datablock) */
	_GET_STRING_ATTRIBUTE("CCCNormalNoiseMark","CCC normal setup:", m_conf_file['CCCNormalSetup'].m_noise_mark, "");
	_GET_STRING_ATTRIBUTE("CCCNarrowNoiseMark","CCC narrow setup:", m_conf_file['CCCNarrowSetup'].m_noise_mark, "");
	_GET_STRING_ATTRIBUTE("CHCNormalNoiseMark","CHC Normal setup:", m_conf_file['CHCNormalSetup'].m_noise_mark, "");
	_GET_STRING_ATTRIBUTE("CHCNarrowNoiseMark","CHC narrow setup:", m_conf_file['CHCNarrowSetup'].m_noise_mark, "");		

	/* now read the setup related configurations <name - con file path> , filling associated parameters struct */	
	std::map<IRA::CString, IRA::CString>::const_iterator l_it;
	for (l_it= m_conf_file.begin(); it != m_conf_file.end(); it++ ){		
		/* setup read */
		IRA::CString l_setup_conf_path= (l_it->second).m_setup;
		readConfigurationSetup(l_it->first, l_setup_conf_path, m_conf_param[l_it->first]);
		/* noise mark read */
		IRA::CString l_noise_mark_conf_path= (l_it->second).m_noise_mark;
		readConfigurationSetup(l_it->first, l_noise_mark_conf_path, m_conf_param[l_it->first]);
	}		
	/* Feeds */
	readFeeds();

	/* Taper */
	readTaper();


	/** @todo how does it cope with LO ?  */
	/** @todo add 2nd stage controller .. */
	/* SYNTHs */
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

/* *** PRIVATE *** */

void CConfiguration::readConfiguration(const IRA::CString & p_conf_path,
									CConfiguration::SetupParams & p_params_out)
									throw (ComponentErrors::CDBAccessExImpl)
{	
	_GET_DWORD_ATTRIBUTE("Feeds","Number of feeds:", p_params_out.m_feeds, p_conf_path) ;
	_GET_DWORD_ATTRIBUTE("IFs","Number of IFs per feed:",p_params_out.m_IFs, p_conf_path);
	/* Read polarization token, string splitted by ' ' 
	 * Reading L R .., converting string to Receivers tokens RCV_LCP ...
	 */
	IRA::CError l_error;
	IRA::CString l_field, l_value, l_token;
	WORD l_len;
	int l_start=0;
	_GET_STRING_ATTRIBUTE("Polarization","IF polarization:", l_value, p_conf_path);
	l_start=0;
	for (WORD k=0; k< p_params_out.m_IFs; k++) {
		if (!IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("Polarization");
			throw dummy;
		}
		token.MakeUpper();
		if (l_token=="L") {
			m_polarizations.push_bach(Receivers::RCV_LCP);
		}
		else if (l_token=="R") {
			m_polarizations.push_back(Receivers::RCV_RCP);
		}
		else {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("Polarization");
			throw dummy;
		}
	}
	/* Set RF lower limit for every feed */
	_GET_STRING_ATTRIBUTE("RFMin","RF lower limit (MHz):", l_value, p_conf_path);
	l_start=0;
	for (WORD k=0; k < p_params_out.m_IFs; k++) {
		if (!IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("RFMin");
			throw dummy;
		}
		p_params_out.m_RFMin.push_back(token.ToDouble());
	}
	/* Set RF upper limit for every feed */
	_GET_STRING_ATTRIBUTE("RFMax","RF upper limit (MHz):", l_value, p_conf_path);
	l_start=0;
	for (WORD k=0; k < p_params_out.m_IFs; k++) {
		if (!IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("RFMax");
			throw dummy;
		}
		p_params_out.m_RFMax.push_back(token.ToDouble());
	}
	/* Set IF start freq. for every feed */
	_GET_STRING_ATTRIBUTE("IFMin","IF start frequency (MHz):", l_value, p_conf_path);
	l_start=0;
	for (WORD k=0; k < p_params_out.m_IFs; k++) {
		if (!IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("IFMin");
			throw dummy;
		}
		p_params_out.m_IFMin.push_back(token.ToDouble());
	}
	/* Set IF bw for every feed */
	_GET_STRING_ATTRIBUTE("IFBandwidth","IF bandwidth (MHz):", l_value, p_conf_path);
	l_start=0;
	for (WORD k=0; k < p_params_out.m_IFs; k++) {
		if (!IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("IFBandwidth");
			throw dummy;
		}
		p_params_out.m_IFBandwidth.push_back(token.ToDouble());
	}
	/* Set default LO value for every feed */
	_GET_STRING_ATTRIBUTE("DefaultLO","Default local oscillator (MHz):", l_value, p_conf_path);
	l_start=0;
	for (WORD k=0; k < p_params_out.m_IFs; k++) {
		if (!IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("DefaultLO");
			throw dummy;
		}
		p_params_out.m_defaultLO.push_back(token.ToDouble());
	}
	/** @todo Check LO stage control */
	/* Set Fixed LO 2nd stage? value for every feed */
	_GET_STRING_ATTRIBUTE("FixedLO2","Second fixed local oscillator value (MHz):", l_value, p_conf_path);
	l_start=0;
	for (WORD k=0; k < p_params_out.m_IFs; k++) {
		if (!IRA::CIRATools::getNextToken( l_value, l_start, ' ', l_token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("FixedLO2");
			throw dummy;
		}
		p_params_out.m_fixedLO2.push_back(token.ToDouble());
	}
	/** @todo Check LO stage control */
	/* Set  LO min (stage?) value for every feed */
	_GET_STRING_ATTRIBUTE("LOMin","Local oscillator minimum allowed value (MHz):", l_value, p_conf_path);
	l_start=0;
	for (WORD k=0; k < p_params_out.m_IFs; k++) {
		if (!IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("LOMin");
			throw dummy;
		}
		p_params_out.m_LOMin.push_back(token.ToDouble());
	}
	/** @todo Check LO stage control */
	/* Set  LO max (stage?) value for every feed */
	_GET_STRING_ATTRIBUTE("LOMax","Local oscillator maximum allowed value (MHz):", l_value, p_conf_path);
	l_start=0;
	for (WORD k=0; k < p_params_out.m_IFs; k++) {
		if (!IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("LOMax");
			throw dummy;
		}
		p_params_out.m_LOMax.push_back(token.ToDouble());
	}
}

void CConfiguration::readNoiseMarkPoly(const IRA::CString & p_conf_path,
									CConfiguration::SetupParams & p_params_out)
									throw (ComponentErrors::CDBAccessExImpl)
{
	IRA::CError l_error;
	IRA::CString l_field, l_value, l_token;	
 	/* Collecting LCP "C0 C1 .."" coefficients from appropriate configuration file */ 
	 int l_start=0;
	_GET_STRING_ATTRIBUTE("LCoeffs","LCP noise mark poly coefficients :", l_value, p_conf_path);	
	while(!IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)){		
		p_params_out.m_noise_mark_lcp_coeffs.push_back(token.ToDouble());
	}	
	/* Collecting RCP "C0 C1 .."" coefficients from appropriate configuration file */ 
	int l_start=0;
	_GET_STRING_ATTRIBUTE("RCoeffs","RCP noise mark poly coefficients :", l_value, p_conf_path);	
	while(!IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)){		
		p_params_out.m_noise_mark_rcp_coeffs.push_back(token.ToDouble());
	}
}

void CConfiguration::readFeeds()
			 	throw (ComponentErrors::CDBAccessExImpl, ComponentErrors::MemoryAllocationExImpl)
{
	/* Local feed table */
	IRA::CError l_error;
	IRA::CString l_field;	
	WORD l_len;
	try {
		m_feedsTable=new IRA::CDBTable(Services,"Feed",FEEDTABLE_PATH);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::init()");
		throw dummy;
	}
	error.Reset();
	if (!m_feedsTable->addField(l_error,"feedCode",IRA::CDataField::LONGLONG)) {
		l_field="feedCode";
 	}
	else if (!m_feedsTable->addField(l_error,"xOffset",IRA::CDataField::DOUBLE)) {
		l_field="xOffset";
 	}
	else if (!m_feedsTable->addField(l_error,"yOffset",IRA::CDataField::DOUBLE)) {
		l_field="yOffset";
 	}
	else if (!m_feedsTable->addField(l_error,"relativePower",IRA::CDataField::DOUBLE)) {
		l_field="relativePower";
 	}
	if (!error.isNoError()) {
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,l_error);
		dummy.setFieldName((const char *)l_field);
		throw dummy;
	}
	if (!m_feedsTable->openTable(l_error))	{
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
		throw dummy;
	}
	m_feedsTable->First();
	if (m_feeds!=m_feedsTable->recordCount()) {
		_EXCPT(ComponentErrors::CDBAccessExImpl, dummy, "CConfiguration::init()");
		dummy.setFieldName("feed table size");
		throw dummy;
	}
	l_len=m_feeds;
	try {
		m_feedVector=new TFeedValue[l_len];
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
		ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_DEBUG,"FEED_VALUE_ENTRY: %d %lf %lf %lf",
														m_feedVector[i].code,
														m_feedVector[i].xOffset,
														m_feedVector[i].yOffset,
														m_feedVector[i].relativePower));
		m_feedsTable->Next();
	}
	m_feedsTable->closeTable();
	delete m_feedsTable;
	m_feedsTable=NULL;
}

void readTaper() 
	throw (ComponentErrors::CDBAccessExImpl, ComponentErrors::MemoryAllocationExImpl)
{
	IRA::CError l_error;
	IRA::CString l_field;	
	WORD l_len;
	try {
		m_taperTable=new IRA::CDBTable(Services,"TaperEntry",TAPERTABLE_PATH);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::init()");
		throw dummy;
	}
	error.Reset();
	if (!m_taperTable->addField(l_error,"Frequency",IRA::CDataField::DOUBLE)) {
		l_field="Frequency";
 	}
	else if (!m_taperTable->addField(l_error,"Taper",IRA::CDataField::DOUBLE)) {
		l_field="OutputPower";
 	}
	if (!error.isNoError()) {
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,l_error);
		dummy.setFieldName((const char *)l_field);
		throw dummy;
	}
	if (!m_taperTable->openTable(error))	{
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
		throw dummy;
	}
	m_taperTable->First();
	l_len=m_taperTable->recordCount();
	try {
		m_taperVector=new TTaperValue[l_len];
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::init()");
		throw dummy;
	}
	ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_DEBUG,"TAPER_ENTRY_NUMBER: %d",l_len));
	for (WORD i=0;i<l_len;i++) {
		m_taperVector[i].frequency=(*m_taperTable)["Frequency"]->asDouble();
		m_taperVector[i].taper=(*m_taperTable)["Taper"]->asDouble();
		ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_DEBUG,"SYNTH_VALUE_ENTRY: %lf %lf", 
														m_taperVector[i].frequency,
														m_taperVector[i].taper));
		m_taperTable->Next();
	}
	m_taperVectorLen=l_len;
	m_taperTable->closeTable();
	delete m_taperTable;
	m_taperTable=NULL;
}