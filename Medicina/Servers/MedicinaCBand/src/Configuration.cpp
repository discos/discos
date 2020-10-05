// $Id: Configuration.cpp,v 1.5 2011-06-01 18:24:44 a.orlati Exp $

#include "Configuration.h"
#include "Defs.h"

using namespace IRA;

/* *** MACROs *** */

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


/* *** PUBLIC CTORS *** */

CConfiguration::CConfiguration()
{	
	m_loTable=NULL;	
	m_feedsTable=NULL;
	m_taperTable=NULL;	
}

CConfiguration::~CConfiguration()
{	
	if (m_loTable) {delete m_loTable;}		
	if (m_taperTable) {delete m_taperTable;}		
	if (m_feedsTable) {delete m_feedsTable;}		
}

/* *** COMPONENT *** */

void CConfiguration::init(maci::ContainerServices *Services) 
				throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl)
{
	/**
	 *  Basically we look for:
	 *  * Component configuration data at Medicina\Configuration\alma\RECEIVERS\MedicinaCBand\MedicinaCBand.xml
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
	 */	
	/* read component configuration */	
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
	/* now read the setup for every available conf <name - con file path> , filling associated parameters struct */	
	const std::vector<ConfigurationName> l_available_confs= m_conf_hnd.getAvailableConfs();
	std::vector<ConfigurationName>::const_iterator l_conf_it;
	for (l_conf_it= m_conf_file.begin(); l_conf_it != m_conf_file.end(); l_conf_it++ ){		
		/* Getting configuration file path */
		ReceiverConfHandler::ConfigurationSetup l_setup;
		ReceiverConfHandler::ConfigurationAccess l_access;		
		IRA::CError l_error;		
		bool l_found= m_conf_hnd.getConfigurationAccess(*l_conf_it, l_access);
		if(!l_found){
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("Configuration");
			throw dummy;
		}
		/* Reading Setup main infos */
		l_setup.m_name= m_conf_hnd.m_conf_name[*l_conf_it];
		readConfigurationSetup(l_access.m_conf_file_path, l_setup );		
		/* noise mark read */		
		readNoiseMarkPoly(l_access.m_noisemark_file_path, l_setup);
		/* set setup conf back to configuration handler */
		l_found= m_conf_hnd.setSetup(*l_conf_it, l_setup);
		if(!l_found){
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("Configuration");
			throw dummy;
		}
		 ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_NOTICE, "CONFIGURATION PROCESSED %s", l_setup.m_name));
	}		
	/* Applying default conf */
	m_conf_hnd.setConfiguration(ReceiverConfHandler::CCC_Normal);
	/* Feeds */
	readFeeds();
	/* Taper */
	readTaper();
}


/* *** TABLES *** */

DWORD CConfiguration::getLeftMarkCoeffs(double *& p_out_coeffs) const
{		
	if (p_out_coeffs)
		delete p_out_coeffs;
	WORD l_coeff_vect_len= p_params_out.m_noise_mark_lcp_coeffs.size();	
	p_out_coeffs= new double[l_coeff_vect_len];
	for(int i=0; i< l_coeff_vect_len; i++){
		p_out_coeffs[i]= p_params_out.m_noise_mark_lcp_coeffs[i];
	}
	return l_coeff_vect_len
}

DWORD CConfiguration::getRightMarkCoeffs(double *& p_out_coeffs) const
{
	if (p_out_coeffs)
		delete p_out_coeffs;
	WORD l_coeff_vect_len= p_params_out.m_noise_mark_rcp_coeffs.size();	
	p_out_coeffs= new double[l_coeff_vect_len];
	for(int i=0; i< l_coeff_vect_len; i++){
		p_out_coeffs[i]= p_params_out.m_noise_mark_rcp_coeffs[i];
	}
	return l_coeff_vect_len
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

void CConfiguration::readConfigurationSetup(const IRA::CString & p_conf_path,
									ReceiverConfHandler::ConfigurationSetup & p_params_out)
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
									ReceiverConfHandler::ConfigurationSetup & p_params_out)
									throw (ComponentErrors::CDBAccessExImpl)
{
	/* Nois mark is bound to receiver configuration */
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
	for (WORD i=0; i < m_feeds; i++) {
		TFeedValue l_feed_value;
		l_feed_value.xOffset=(*m_feedsTable)["xOffset"]->asDouble();
		l_feed_value.yOffset=(*m_feedsTable)["yOffset"]->asDouble();
		l_feed_value.relativePower=(*m_feedsTable)["relativePower"]->asDouble();
		l_feed_value.code=(WORD)(*m_feedsTable)["feedCode"]->asLongLong();		
		ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_DEBUG,"FEED_VALUE_ENTRY: %d %lf %lf %lf",
														l_feed_value.code,
														l_feed_value.xOffset,
														l_feed_value.yOffset,
														l_feed_value.relativePower));
		m_feeds_table.push_back(l_feed_value);														
		m_feedsTable->Next();
	}
	m_feedsTable->closeTable();
	delete m_feedsTable;
	m_feedsTable=NULL;
}

void CConfiguration::readTaper() 
	throw (ComponentErrors::CDBAccessExImpl, ComponentErrors::MemoryAllocationExImpl)
{
	IRA::CError l_error;
	IRA::CString l_field;		
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
	ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_DEBUG,"TAPER_ENTRY_NUMBER: %d", m_taperTable->recordCount()));
	for (WORD i=0; i < m_taperTable->recordCount(); i++) {
		TTaperValue l_taper_value;
		l_taper_value.frequency= (*m_taperTable)["Frequency"]->asDouble();		
		l_taper_value.taper=(*m_taperTable)["Taper"]->asDouble();
		ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_DEBUG,"SYNTH_VALUE_ENTRY: %lf %lf", 
														l_taper_value.frequency,
														l_taper_value.taper));
		m_taper_table.push_back(l_taper_value);													
		m_taperTable->Next();
	}
	m_taperTable->closeTable();
	delete m_taperTable;
	m_taperTable=NULL;
}

void CConfiguration::readSynths(){
	/* SYNTHs */
	readSyntTable(m_synt_table_1st, SYNTH_1ST_TABLE_PATH);	
	readSyntTable(m_synt_table_2st, SYNTH_2ND_TABLE_PATH);
}

void CConfiguration::readSyntTable(std::vector<TLOValue> & p_synt_table, IRA::CString p_table_file) 
							throw (ComponentErrors::CDBAccessExImpl, ComponentErrors::MemoryAllocationExImpl)
{
	/* file table reading */
	try {
		m_loTable=new IRA::CDBTable(Services,"SynthesizerEntry",p_table_file);
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
	/* Cove table data to class member container */
	m_loTable->First();
	ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_DEBUG,"SYNTH_VALUE_ENTRY_NUMBER: %d", m_loTable->recordCount()));
	for (WORD i=0; i < m_loTable->recordCount(); i++) {
		TLOValue l_tlo_value;
		l_tlo_value.frequency= (*m_loTable)["Frequency"]->asDouble();
		l_tlo_value.outputPower= (*m_loTable)["OutputPower"]->asDouble();		
		m_synt_table_1st.push_back(l_tlo_value);
		ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_DEBUG,"SYNTH_VALUE_ENTRY: %lf %lf", l_tlo_value.frequency, l_tlo_value.outputPower));
		m_loTable->Next();
	}	
	m_loTable->closeTable();
	delete m_loTable;
	m_loTable=NULL;
}