// $Id: Configuration.cpp,v 1.5 2011-06-01 18:24:44 a.orlati Exp $

#include "Configuration.h"
#include <math.h>
#include "Defs.h"

using namespace IRA;

/* *** MACROs *** */

#define _GET_DOUBLE_ATTRIBUTE(ATTRIB,DESCR,FIELD,NAME) { \
	double tmpd; \
	if (!CIRATools::getDBValue(m_service,ATTRIB,tmpd,"alma/",NAME)) { \
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
	if (!CIRATools::getDBValue(m_service,ATTRIB,tmpw,"alma/",NAME)) { \
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
	if (!CIRATools::getDBValue(m_service,ATTRIB,tmps,"alma/",NAME)) { \
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
	MED_TRACE();
}

CConfiguration::~CConfiguration()
{	
	
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
	 MED_TRACE_MSG(" IN ");
	 m_service= Services;
	/* read component configuration */	
	_GET_STRING_ATTRIBUTE("DewarIPAddress","Dewar IP address:",m_dewarIPAddress,"");	
	_GET_DWORD_ATTRIBUTE("DewarPort","Dewar port:",m_dewarPort,"");
	_GET_STRING_ATTRIBUTE("LNAIPAddress","LNA IP address:",m_LNAIPAddress,"");
	_GET_DWORD_ATTRIBUTE("LNAPort","LNA port:",m_LNAPort,"");
	_GET_STRING_ATTRIBUTE("LocalOscillatorInstance1st","Local oscillator instance:",m_localOscillatorInstance1st,"");
	_GET_STRING_ATTRIBUTE("LocalOscillatorInstance2nd","Local oscillator instance:",m_localOscillatorInstance2nd,"");
	_GET_DWORD_ATTRIBUTE("WatchDogResponseTime","Response time of watch dog thread (uSec):",m_watchDogResponseTime,"");
	_GET_DWORD_ATTRIBUTE("WatchDogSleepTime","Sleep time of the watch dog thread (uSec):",m_watchDogSleepTime,"");
	_GET_DWORD_ATTRIBUTE("LNASamplingTime","Time needed to collect LNA information from control boards (uSec):",m_LNASamplingTime,"");
	_GET_DWORD_ATTRIBUTE("RepetitionCacheTime","Log repetition filter, caching time (uSec):",m_repetitionCacheTime,"");
	_GET_DWORD_ATTRIBUTE("RepetitionExpireTime","Log repetition filter, expire time (uSec):",m_repetitionExpireTime,"");	
	/** TEST */
	fprintf(stderr, "---- CONF ----" );
	fprintf(stderr, "LO 1st %s \n", (const char*)m_localOscillatorInstance1st);
	fprintf(stderr, "LO 1st %s \n", (const char*)m_localOscillatorInstance2nd);
	fprintf(stderr, "LNA address %S \n", (const char*)m_LNAIPAddress);
	fprintf(stderr, "LNA port %u \n", m_LNAPort);
	fprintf(stderr, "Dewar address %S \n", (const char*)m_dewarIPAddress);
	fprintf(stderr, "Dewar port %u \n", m_dewarPort);
	fprintf(stderr, "---- * ----" );
	/* END TEST */	
	/* Get available configurations, access every conf file, filling associated parameters struct */	
	const std::vector<ReceiverConfHandler::ConfigurationName> l_available_confs= m_conf_hnd.getAvailableConfs();
	std::vector<ReceiverConfHandler::ConfigurationName>::const_iterator l_conf_it;
	for (l_conf_it= l_available_confs.begin(); l_conf_it != l_available_confs.end(); l_conf_it++ ){		
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
		readConfigurationSetup(l_access.m_conf_file_path, l_setup );		
		/* noise mark read */		
		readNoiseMarkPoly(l_access.m_noisemark_file_path, l_setup);
		/* Set setup conf back to configuration handler */
		l_found= m_conf_hnd.setConfigurationSetup(*l_conf_it, l_setup);
		if(!l_found){
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("Configuration");
			throw dummy;
		}
		ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_NOTICE, "CONFIGURATION PROCESSED"));
	}		
	/* Applying default conf */
	m_conf_hnd.setConfiguration(ReceiverConfHandler::CCC_Normal);
	/* Feeds */
	readFeeds();
	/* Taper */
	readTaper();	
	MED_TRACE_MSG(" OUT ");
}

/* *** TABLES *** */

DWORD CConfiguration::getLeftMarkCoeffs(double *& p_out_coeffs) 
{		
	if (p_out_coeffs)
		delete p_out_coeffs;
	ReceiverConfHandler::ConfigurationSetup l_conf_setup= m_conf_hnd.getCurrentSetup();
	WORD l_coeff_vect_len= l_conf_setup.m_noise_mark_lcp_coeffs.size();	
	p_out_coeffs= new double[l_coeff_vect_len];	
	for(int i=0; i< l_coeff_vect_len; i++){
		p_out_coeffs[i]= l_conf_setup.m_noise_mark_lcp_coeffs[i];		
	}
	return l_coeff_vect_len;
}

DWORD CConfiguration::getRightMarkCoeffs(double *& p_out_coeffs) 
{
	if (p_out_coeffs)
		delete p_out_coeffs;
	ReceiverConfHandler::ConfigurationSetup l_conf_setup= m_conf_hnd.getCurrentSetup();
	WORD l_coeff_vect_len= l_conf_setup.m_noise_mark_rcp_coeffs.size();	
	p_out_coeffs= new double[l_coeff_vect_len];	
	for(int i=0; i< l_coeff_vect_len; i++){
		p_out_coeffs[i]= l_conf_setup.m_noise_mark_rcp_coeffs[i];		
	}
	return l_coeff_vect_len;
}

double CConfiguration::getLeftMarkTemp(double freq)
{
	double * l_coeffs= NULL;
	DWORD l_coeffs_len;
	double l_ret= 0.0;
	l_coeffs_len= getLeftMarkCoeffs(l_coeffs);				
	for(DWORD i=0; i< l_coeffs_len; i++){
		l_ret+= l_coeffs[i] *  pow(freq, i);
	}
	return l_ret;
}

double CConfiguration::getRightMarkTemp(double freq)
{
	double * l_coeffs= NULL;
	DWORD l_coeffs_len;
	double l_ret= 0.0;
	l_coeffs_len= getRightMarkCoeffs(l_coeffs);				
	for(DWORD i=0; i< l_coeffs_len; i++){
		l_ret+= l_coeffs[i] *  pow(freq, i);
	}
	return l_ret;
}

DWORD CConfiguration::getSynthesizerTable(double *& freq,double *& power) const
{
	/* returning only one table values, we assume similar steps between two stages */
	WORD l_synt_len= m_synt_table_1st.size();
	freq= new double [l_synt_len];
	power=new double [l_synt_len];
	for (DWORD j=0;j < l_synt_len; j++){
		freq[j]= m_synt_table_1st[j].frequency;
		power[j]= m_synt_table_1st[j].outputPower;
	}
	return l_synt_len;
}

DWORD CConfiguration::getTaperTable(double * &freq,double *&taper) const
{
	WORD l_taper_len= m_taper_table.size();
	freq= new double [l_taper_len];
	taper=new double [l_taper_len];
	for (WORD j=0; j < l_taper_len; j++) {
		freq[j]=m_taper_table[j].frequency;
		taper[j]=m_taper_table[j].taper;
	}
	return l_taper_len;
}

DWORD CConfiguration::getFeedInfo(WORD *& code,double *& xOffset,double *& yOffset,double *& relativePower) const
{
	WORD l_feeds_len= m_feeds_table.size();
	code=new WORD[l_feeds_len];
	xOffset=new double [l_feeds_len];
	yOffset=new double [l_feeds_len];
	relativePower=new double [l_feeds_len];
	for (DWORD j= 0; j < l_feeds_len; j++) {
		code[j]=m_feeds_table[j].code;
		xOffset[j]=m_feeds_table[j].xOffset;
		yOffset[j]=m_feeds_table[j].yOffset;
		relativePower[j]=m_feeds_table[j].relativePower;
	}
	return l_feeds_len;
}

/* *** PRIVATE *** */

void CConfiguration::readConfigurationSetup(const IRA::CString & p_conf_path,
									ReceiverConfHandler::ConfigurationSetup & p_conf_setup)
									throw (ComponentErrors::CDBAccessExImpl)
{	
	MED_TRACE_MSG("IN");	
	const char * l_app= p_conf_path;
	MED_TRACE_MSG(l_app);
	_GET_DWORD_ATTRIBUTE("Feeds","Number of feeds:", p_conf_setup.m_feeds, p_conf_path) ;
	_GET_DWORD_ATTRIBUTE("IFs","Number of IFs per feed:",p_conf_setup.m_IFs, p_conf_path);
	/* Read polarization token, string splitted by ' ' 
	 * Reading L R .., converting string to Receivers tokens RCV_LCP ...
	 */
	IRA::CError l_error;
	IRA::CString l_field, l_value, l_token;
	int l_start=0;
	_GET_STRING_ATTRIBUTE("Polarization","IF polarization:", l_value, p_conf_path);
	l_start=0;
	for (WORD k=0; k< p_conf_setup.m_IFs; k++) {
		if (!IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("Polarization");
			throw dummy;
		}
		l_token.MakeUpper();
		if (l_token=="L") {
			p_conf_setup.m_polarizations.push_back(Receivers::RCV_LCP);
		}
		else if (l_token=="R") {
			p_conf_setup.m_polarizations.push_back(Receivers::RCV_RCP);
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
	for (WORD k=0; k < p_conf_setup.m_IFs; k++) {
		if (!IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("RFMin");
			throw dummy;
		}
		p_conf_setup.m_RFMin.push_back(l_token.ToDouble());
	}
	/* Set RF upper limit for every feed */
	_GET_STRING_ATTRIBUTE("RFMax","RF upper limit (MHz):", l_value, p_conf_path);
	l_start=0;
	for (WORD k=0; k < p_conf_setup.m_IFs; k++) {
		if (!IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("RFMax");
			throw dummy;
		}
		p_conf_setup.m_RFMax.push_back(l_token.ToDouble());
	}
	/* Set IF start freq. for every feed */
	_GET_STRING_ATTRIBUTE("IFMin","IF start frequency (MHz):", l_value, p_conf_path);
	l_start=0;
	for (WORD k=0; k < p_conf_setup.m_IFs; k++) {
		if (!IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("IFMin");
			throw dummy;
		}
		p_conf_setup.m_IFMin.push_back(l_token.ToDouble());
	}
	/* Set IF bw for every feed */
	_GET_STRING_ATTRIBUTE("IFBandwidth","IF bandwidth (MHz):", l_value, p_conf_path);
	l_start=0;
	for (WORD k=0; k < p_conf_setup.m_IFs; k++) {
		if (!IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("IFBandwidth");
			throw dummy;
		}
		p_conf_setup.m_IFBandwidth.push_back(l_token.ToDouble());
	}
	/* Set default LO value for every feed */
	_GET_STRING_ATTRIBUTE("DefaultLO","Default local oscillator (MHz):", l_value, p_conf_path);
	l_start=0;
	for (WORD k=0; k < p_conf_setup.m_IFs; k++) {
		if (!IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("DefaultLO");
			throw dummy;
		}
		p_conf_setup.m_defaultLO.push_back(l_token.ToDouble());
	}
	/** @todo Check LO stage control */
	/* Set Fixed LO 2nd stage? value for every feed */
	_GET_STRING_ATTRIBUTE("FixedLO2","Second fixed local oscillator value (MHz):", l_value, p_conf_path);
	l_start=0;
	for (WORD k=0; k < p_conf_setup.m_IFs; k++) {
		if (!IRA::CIRATools::getNextToken( l_value, l_start, ' ', l_token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("FixedLO2");
			throw dummy;
		}
		p_conf_setup.m_fixedLO2.push_back(l_token.ToDouble());
	}
	/** @todo Check LO stage control */
	/* Set  LO min (stage?) value for every feed */
	_GET_STRING_ATTRIBUTE("LOMin","Local oscillator minimum allowed value (MHz):", l_value, p_conf_path);
	l_start=0;
	for (WORD k=0; k < p_conf_setup.m_IFs; k++) {
		if (!IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("LOMin");
			throw dummy;
		}
		p_conf_setup.m_LOMin.push_back(l_token.ToDouble());
	}
	/** @todo Check LO stage control */
	/* Set  LO max (stage?) value for every feed */
	_GET_STRING_ATTRIBUTE("LOMax","Local oscillator maximum allowed value (MHz):", l_value, p_conf_path);
	l_start=0;
	for (WORD k=0; k < p_conf_setup.m_IFs; k++) {
		if (!IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)) {
			_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
			dummy.setFieldName("LOMax");
			throw dummy;
		}
		p_conf_setup.m_LOMax.push_back(l_token.ToDouble());
	}
	MED_TRACE_MSG("OUT");
}

void CConfiguration::readNoiseMarkPoly(const IRA::CString & p_conf_path,
									ReceiverConfHandler::ConfigurationSetup & p_conf_setup)
									throw (ComponentErrors::CDBAccessExImpl)
{
	MED_TRACE_MSG("IN");
	/* Nois mark is bound to receiver configuration */
	IRA::CError l_error;
	IRA::CString l_field, l_value, l_token;	
 	/* Collecting LCP "C0 C1 .."" coefficients from appropriate configuration file */ 
	int l_start=0;
	_GET_STRING_ATTRIBUTE("LCoeffs","LCP noise mark poly coefficients :", l_value, p_conf_path);	
	while(IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)){		
		p_conf_setup.m_noise_mark_lcp_coeffs.push_back(l_token.ToDouble());
	}	
	/* wo token convert string to value */
	if (!l_start)
		p_conf_setup.m_noise_mark_lcp_coeffs.push_back(l_value.ToDouble());
	/* Collecting RCP "C0 C1 .."" coefficients from appropriate configuration file */ 
	/* wo token convert string to value */
	if (!l_start)
		p_conf_setup.m_noise_mark_lcp_coeffs.push_back(l_value.ToDouble());
	l_start=0;
	_GET_STRING_ATTRIBUTE("RCoeffs","RCP noise mark poly coefficients :", l_value, p_conf_path);	
	while(IRA::CIRATools::getNextToken(l_value, l_start, ' ', l_token)){		
		p_conf_setup.m_noise_mark_rcp_coeffs.push_back(l_token.ToDouble());
	}
	MED_TRACE_MSG("OUT");
}

void CConfiguration::readFeeds()
			 	throw (ComponentErrors::CDBAccessExImpl, ComponentErrors::MemoryAllocationExImpl)
{
	MED_TRACE_MSG("IN");
	/* Local feed table */
	IRA::CDBTable *m_feedsTable;	/**< Helper reading xml feeds table */
	IRA::CError l_error;
	IRA::CString l_field;

	ReceiverConfHandler::ConfigurationAccess l_access= m_conf_hnd.getCurrentAccess();

	try {
		m_feedsTable=new IRA::CDBTable(m_service,"Feed", l_access.m_feeds_file_path);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::init()");
		throw dummy;
	}
	l_error.Reset();
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
	if (!l_error.isNoError()) {
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,l_error);
		dummy.setFieldName((const char *)l_field);
		throw dummy;
	}
	if (!m_feedsTable->openTable(l_error))	{
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
		throw dummy;
	}
	/** @todo prende i dati delal conf di default ? */
	ReceiverConfHandler::ConfigurationSetup l_default_setup;
	m_conf_hnd.getConfigurationSetup(ReceiverConfHandler::CCC_Normal, l_default_setup);
	WORD l_default_conf_feed_len= l_default_setup.m_feeds;
	m_feedsTable->First();	
	if (l_default_conf_feed_len!= m_feedsTable->recordCount()) {
		_EXCPT(ComponentErrors::CDBAccessExImpl, dummy, "CConfiguration::init()");
		dummy.setFieldName("feed table size");
		throw dummy;
	}			
	for (WORD i=0; i < l_default_conf_feed_len; i++) {
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
	MED_TRACE_MSG("OUT");
}

void CConfiguration::readTaper() 
	throw (ComponentErrors::CDBAccessExImpl, ComponentErrors::MemoryAllocationExImpl)
{
	MED_TRACE_MSG("IN");
	IRA::CDBTable * l_taper_table;
	IRA::CError l_error;
	IRA::CString l_field;
	
	ReceiverConfHandler::ConfigurationAccess l_access= m_conf_hnd.getCurrentAccess();		

	try {
		l_taper_table=new IRA::CDBTable(m_service, "TaperEntry", l_access.m_taper_file_path);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::init()");
		throw dummy;
	}
	l_error.Reset();
	if (!l_taper_table->addField(l_error,"Frequency",IRA::CDataField::DOUBLE)) {
		l_field="Frequency";
 	}
	else if (!l_taper_table->addField(l_error,"Taper",IRA::CDataField::DOUBLE)) {
		l_field="OutputPower";
 	}
	if (!l_error.isNoError()) {
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,l_error);
		dummy.setFieldName((const char *)l_field);
		throw dummy;
	}
	if (!l_taper_table->openTable(l_error))	{
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
		throw dummy;
	}
	l_taper_table->First();
	ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_DEBUG,"TAPER_ENTRY_NUMBER: %d", l_taper_table->recordCount()));
	for (WORD i=0; i < l_taper_table->recordCount(); i++) {
		TTaperValue l_taper_value;
		l_taper_value.frequency= (*l_taper_table)["Frequency"]->asDouble();		
		l_taper_value.taper=(*l_taper_table)["Taper"]->asDouble();
		ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_DEBUG,"SYNTH_VALUE_ENTRY: %lf %lf", 
														l_taper_value.frequency,
														l_taper_value.taper));
		m_taper_table.push_back(l_taper_value);													
		l_taper_table->Next();
	}
	l_taper_table->closeTable();
	delete l_taper_table;
	l_taper_table=NULL;
	MED_TRACE_MSG("OUT");
}

void CConfiguration::readSynths()
			throw (ComponentErrors::CDBAccessExImpl,
					 ComponentErrors::MemoryAllocationExImpl)
{
	MED_TRACE_MSG("IN");
	/* SYNTHs */
	ReceiverConfHandler::ConfigurationAccess l_access= m_conf_hnd.getCurrentAccess();
	readSyntTable(m_synt_table_1st, l_access.m_synth_1_file_path);	
	readSyntTable(m_synt_table_2nd, l_access.m_synth_2_file_path);
	MED_TRACE_MSG("OUT");
}

void CConfiguration::readSyntTable(std::vector<TLOValue> & p_synt_table, IRA::CString p_table_file) 
							throw (ComponentErrors::CDBAccessExImpl, ComponentErrors::MemoryAllocationExImpl)
{
	MED_TRACE_MSG("IN");
	/* file table reading */
	IRA::CDBTable * m_loTable;
	IRA::CError l_error;
	IRA::CString l_field;	
	try {
		m_loTable=new IRA::CDBTable(m_service,"SynthesizerEntry",p_table_file);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::init()");
		throw dummy;
	}
	l_error.Reset();
	if (!m_loTable->addField(l_error,"Frequency",IRA::CDataField::DOUBLE)) {
		l_field="Frequency";
 	}
	else if (!m_loTable->addField(l_error,"OutputPower",IRA::CDataField::DOUBLE)) {
		l_field="OutputPower";
 	}
	if (!l_error.isNoError()) {
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,l_error);
		dummy.setFieldName((const char *)l_field);
		throw dummy;
	}
	if (!m_loTable->openTable(l_error))	{
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, l_error);
		throw dummy;
	}
	/* Cove table data to class member container */
	m_loTable->First();
	ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_DEBUG,"SYNTH_VALUE_ENTRY_NUMBER: %d", m_loTable->recordCount()));
	for (WORD i=0; i < m_loTable->recordCount(); i++) {
		TLOValue l_tlo_value;
		l_tlo_value.frequency= (*m_loTable)["Frequency"]->asDouble();
		l_tlo_value.outputPower= (*m_loTable)["OutputPower"]->asDouble();		
		p_synt_table.push_back(l_tlo_value);
		ACS_LOG(LM_FULL_INFO,"CConfiguration::init()",(LM_DEBUG,"SYNTH_VALUE_ENTRY: %lf %lf", l_tlo_value.frequency, l_tlo_value.outputPower));
		m_loTable->Next();
	}	
	m_loTable->closeTable();
	delete m_loTable;
	m_loTable=NULL;
	MED_TRACE_MSG("OUT");
}