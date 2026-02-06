#include "Configuration.h"

template <class T>
CConfiguration<T>::CConfiguration()
{
   m_markVector = NULL;
   m_markVectorLen = 0;
   m_markTable=NULL;
   m_mode="";
   m_loTable_K=m_loTable_Q=m_loTable_WL=m_loTable_WH=m_loTable_2IF=NULL;
   m_loVector_K=m_loVector_Q=m_loVector_WL=m_loVector_WH=m_loVector_2IF=NULL;
   m_loVectorLen_K=m_loVectorLen_Q=m_loVectorLen_WL=m_loVectorLen_WH=m_loVectorLen_2IF=0;
   m_BandPolarizations = NULL;
   m_feedsTable = NULL;
   m_feedVector = NULL;
   m_taperTable = NULL;
   m_taperVector = NULL;
   m_taperVectorLen = 0;
   m_BandRFMin = NULL;
   m_BandRFMax = NULL;
   m_BandIF1Min = NULL;
   m_BandIF1Max = NULL;
   m_BandIF2Min = NULL;
   m_BandIF2Max = NULL;
	m_IFStartFreq = NULL;
	m_IFBandWidth = NULL;
	m_LO1Injection=m_LO2Injection=NULL;    
   m_DefaultLO1 = m_DefaultLO2= m_currentLOValue= m_currentLO1Value = m_currentLO2Value = m_LOMin = m_LOMax = m_LO1Min = m_LO1Max = m_LO2Min = m_LO2Max = NULL;
}

template <class T>
CConfiguration<T>::~CConfiguration()
{
    if (m_markTable) {
        delete m_markTable;
    }
    if (m_loTable_K) {
        delete m_loTable_K;
    }
    if (m_loTable_Q) {
        delete m_loTable_Q;
    }
    if (m_loTable_WL) {
        delete m_loTable_WL;
    }
    if (m_loTable_WH) {
        delete m_loTable_WH;
    }
    if (m_loTable_2IF) {
        delete m_loTable_2IF;
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
    if (m_loVector_K) {
        delete [] m_loVector_K;
    }
    if (m_loVector_Q) {
        delete [] m_loVector_Q;
    }
    if (m_loVector_WL) {
        delete [] m_loVector_WL;
    }
    if (m_loVector_WH) {
        delete [] m_loVector_WH;
    }
    if (m_loVector_2IF) {
        delete [] m_loVector_2IF;
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
    if (m_BandIF1Min) {
        delete [] m_BandIF1Min;
    }
    if (m_BandIF1Max) {
        delete [] m_BandIF1Max;
    }

    if (m_BandIF2Min) {
        delete [] m_BandIF2Min;
    }
    if (m_BandIF2Max) {
        delete [] m_BandIF2Max;
    }    
    
    if (m_IFStartFreq) {
        delete [] m_IFStartFreq;
    }
    if (m_IFBandWidth) {
        delete [] m_IFBandWidth;
    }    
    if (m_currentLOValue) {
        delete [] m_currentLOValue;
    }
    if (m_currentLO1Value) {
        delete [] m_currentLO1Value;
    }
    if (m_currentLO2Value) {
        delete [] m_currentLO2Value;
    }    
    if (m_DefaultLO1) {
        delete [] m_DefaultLO1;
    }
    if (m_DefaultLO2) {
        delete [] m_DefaultLO2;
    }
    if (m_LO1Injection) {
        delete [] m_LO1Injection;
    }
    if (m_LO2Injection) {
        delete [] m_LO2Injection;
    }
    if (m_LOMin) {
        delete [] m_LOMin;
    }
    if (m_LOMax) {
        delete [] m_LOMax;
    }
    if (m_LO1Min) {
        delete [] m_LO2Min;
    }
    if (m_LO1Max) {
        delete [] m_LO2Max;
    }        
    if (m_LO2Min) {
        delete [] m_LO2Min;
    }
    if (m_LO2Max) {
        delete [] m_LO2Max;
    }
}

/*
   @throw (
   ComponentErrors::CDBAccessExImpl
   ComponentErrors::MemoryAllocationExImpl
   ComponentErrors::CDBAccessExImpl)
*/
template <class T>
WORD CConfiguration<T>::openSynthTable(IRA::CDBTable * &table, const IRA::CString &path, TLOValue * &loV)
{
    IRA::CError ErrEvent;
    IRA::CString field;
    WORD len;
    try {
        table=new IRA::CDBTable(m_services,"SynthesizerEntry",path);
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::openSynthTable()");
        throw dummy;
    }

    ErrEvent.Reset();
    if (!table->addField(ErrEvent,"Frequency",IRA::CDataField::DOUBLE)) {
        field="Frequency";
    }
    else if (!table->addField(ErrEvent,"OutputPower",IRA::CDataField::DOUBLE)) {
        field="OutputPower";
    }
    if (!ErrEvent.isNoError()) {
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,ErrEvent);
        dummy.setFieldName((const char *)field);
        throw dummy;
    }

    if (!table->openTable(ErrEvent))   {
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, ErrEvent);
        throw dummy;
    }
    table->First();
    len=table->recordCount();

    try {
        loV=new TLOValue[len];
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::openSynthTable()");
        throw dummy;
    }
    ACS_LOG(LM_FULL_INFO,"CConfiguration::openSynthTable()",(LM_DEBUG,"SYNTH_VALUE_ENTRY_NUMBER: %d",len));
    for (WORD i=0;i<len;i++) {
        loV[i].frequency=(*table)["Frequency"]->asDouble();
        loV[i].outputPower=(*table)["OutputPower"]->asDouble();
        ACS_LOG(LM_FULL_INFO,"CConfiguration::openSynthTable()",(LM_DEBUG,"SYNTH_VALUE_ENTRY: %lf %lf",loV[i].frequency,loV[i].outputPower));
        table->Next();
    }
    table->closeTable();
    delete table;
    table=NULL;
    return len;
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
    IRA::CString buffer;
    WORD len;
    // read component configuration
    _GET_STRING_ATTRIBUTE(m_services,"DewarIPAddress","Dewar IP address:",m_dewarIPAddress,comp_name);
    _GET_STRING_ATTRIBUTE(m_services,"LNAIPAddress", "LNA IP address:",m_LNAIPAddress,comp_name);
    _GET_STRING_ATTRIBUTE(m_services,"LocalOscillator_K_Instance","Local oscillator for K band:",m_localOscillator_K_Instance,comp_name);
    _GET_STRING_ATTRIBUTE(m_services,"LocalOscillator_Q_Instance","Local oscillator for Q band:",m_localOscillator_Q_Instance,comp_name);
    _GET_STRING_ATTRIBUTE(m_services,"LocalOscillator_W1_Instance","Local oscillator for W low band:",m_localOscillator_W1_Instance,comp_name);
    _GET_STRING_ATTRIBUTE(m_services,"LocalOscillator_W2_Instance","Local oscillator for W high band:",m_localOscillator_W2_Instance,comp_name);
    _GET_STRING_ATTRIBUTE(m_services,"LocalOscillator_LO2_Instance","Local oscillator for the second conversion:",m_localOscillator_2_Instance,comp_name);
    _GET_DWORD_ATTRIBUTE(m_services,"DewarPort","Dewar port:",m_dewarPort,comp_name);
    _GET_DWORD_ATTRIBUTE(m_services,"LNAPort","LNA port:",m_LNAPort,comp_name);
    _GET_DWORD_ATTRIBUTE(m_services,"WatchDogResponseTime","Response time of watch dog thread (uSec):",m_watchDogResponseTime,comp_name);
    _GET_DWORD_ATTRIBUTE(m_services,"WatchDogSleepTime","Sleep time of the watch dog thread (uSec):",m_watchDogSleepTime,comp_name);
    _GET_DWORD_ATTRIBUTE(m_services,"LNASamplingTime","Time needed to collect LNA information from control boards (uSec):",m_LNASamplingTime,comp_name);
    _GET_DWORD_ATTRIBUTE(m_services,"RepetitionCacheTime","Log repetition filter, caching time (uSec):",m_repetitionCacheTime,comp_name);
    _GET_DWORD_ATTRIBUTE(m_services,"RepetitionExpireTime","Log repetition filter, expire time (uSec):",m_repetitionExpireTime,comp_name);
    _GET_STRING_ATTRIBUTE(m_services,"DefaultMode","Default operating mode:",m_defaultMode,comp_name);
    _GET_STRING_ATTRIBUTE(m_services,"BypassSwitchesPattern","Pattern assumed by POS1 readings when in LNA bypass configuration.",buffer,comp_name);
    m_bypassSwitchesPattern = std::bitset<4>((const char*)buffer);

    const IRA::CString DEFAULTMODE_PATH = CONFIG_PATH"/Modes/" + m_defaultMode;
    m_2IFConversionEnabled=(m_localOscillator_2_Instance!="");

    // now read the receiver configuration
    _GET_STRING_ATTRIBUTE(m_services,"Mode","mode name:", mode, DEFAULTMODE_PATH);
    _GET_DWORD_ATTRIBUTE(m_services,"Feeds","Number of feeds:", m_feeds, DEFAULTMODE_PATH);
    _GET_DWORD_ATTRIBUTE(m_services,"IFs","Number of IFs for each feed:", m_IFs, DEFAULTMODE_PATH);

    try {
        m_BandPolarizations = new Receivers::TPolarization[m_IFs*m_feeds];
        m_BandRFMin = new double[m_IFs*m_feeds];
        m_BandRFMax = new double[m_IFs*m_feeds];
        m_BandIF1Min = new double[m_IFs*m_feeds];
        m_BandIF1Max = new double[m_IFs*m_feeds];
        m_BandIF2Min = new double[m_IFs*m_feeds];
        m_BandIF2Max = new double[m_IFs*m_feeds];
        m_IFStartFreq = new double[m_IFs*m_feeds];
        m_IFBandWidth = new double[m_IFs*m_feeds];
        m_DefaultLO1 = new double[m_IFs*m_feeds];
        m_currentLOValue = new double[m_IFs*m_feeds];
        m_currentLO1Value = new double[m_IFs*m_feeds];
        m_currentLO2Value = new double[m_IFs*m_feeds];
        m_DefaultLO2 = new double[m_IFs*m_feeds];
        m_LO1Injection= new long[m_IFs*m_feeds];
        m_LO2Injection= new long[m_IFs*m_feeds];
        m_LOMin = new double[m_IFs*m_feeds];
        m_LOMax = new double[m_IFs*m_feeds];
        m_LO1Min = new double[m_IFs*m_feeds];
        m_LO1Max = new double[m_IFs*m_feeds];        
        m_LO2Min = new double[m_IFs*m_feeds];
        m_LO2Max = new double[m_IFs*m_feeds];
        
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
        field="Feed";
    }
    else if (!m_markTable->addField(error,"Polarization",IRA::CDataField::STRING)) {
        field="Polarization";
    }
    else if (!m_markTable->addField(error,"Coefficients",IRA::CDataField::STRING)) {
        field="Coefficients";
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
    if(len != m_feeds * 2) { // Two channels per feed
        _EXCPT(ComponentErrors::CDBAccessExImpl, dummy, "CConfiguration::init()");
        dummy.setFieldName("MarkCoefficients table size");
        throw dummy;
    }
    try {
        m_markVector = new TMarkValue[len];
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "CConfiguration::init()");
        throw dummy;
    }
    for (WORD i=0;i<len;i++) {
        m_markVector[i].feed=(*m_markTable)["Feed"]->asLongLong();
        m_markVector[i].polarization=(*m_markTable)["Polarization"]->asString()=="LEFT"?Receivers::RCV_LCP:Receivers::RCV_RCP;

        std::vector<std::string> marks_str = split(std::string((*m_markTable)["Coefficients"]->asString()), std::string(","));

        // Vector of coefficients (double)
        for(std::vector<std::string>::iterator iter = marks_str.begin(); iter != marks_str.end(); iter++)
            (m_markVector[i].coefficients).push_back(str2double(*iter));
        m_markTable->Next();
    }
    m_markVectorLen = len;
    m_markTable->closeTable();
    delete m_markTable;
    m_markTable = NULL;

    // The synthesizer

    m_loVectorLen_K=openSynthTable(m_loTable_K,LOTABLE_K_PATH,m_loVector_K);
    m_loVectorLen_Q=openSynthTable(m_loTable_Q,LOTABLE_Q_PATH,m_loVector_Q);
    m_loVectorLen_WL=openSynthTable(m_loTable_WL,LOTABLE_WL_PATH,m_loVector_WL);
    m_loVectorLen_WH=openSynthTable(m_loTable_WH,LOTABLE_WH_PATH,m_loVector_WH);
    m_loVectorLen_2IF=openSynthTable(m_loTable_2IF,LOTABLE_2IF_PATH,m_loVector_2IF);


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
    if (!m_feedsTable->openTable(error)) {
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

/*template <class T>
void CConfiguration<T>::setCurrentLOValue(const ACS::doubleSeq& lo)
{
    for (WORD k=0;k<MIN(lo.length(),getArrayLen());k++) {
        if (lo[k]>=0.0) {
            m_currentLOValue[k]=lo[k];
            updateBandWith(k);
        }
    }
}*/

template <class T>
bool CConfiguration<T>::checkCurrentLOValue(const ACS::doubleSeq& lo,std::vector<double>& ol1,double& ol2)
{
	long pos=0;
	double dol1,dol2;
	while ((lo[pos]==-1) && (pos<lo.length())) { // find the first provided value
		pos++;
	}
	if (pos>=lo.length()) return false;
	ol1.resize(m_IFs*m_feeds);
	if (computeCurrentLOValue(lo[pos],pos,dol1,dol2,false)) {//this is used to compute the first OL2, then it will be fixed
		ol2=dol2; // save the ol2 value
		for (long i=0;i<lo.length();i++) {
			// if target ol is -1, don't perform the computation, keep the current value
			if (lo[i]==-1) ol1[getArrayIndex(i)]=m_currentLO1Value[getArrayIndex(i)]; 
			else if (computeCurrentLOValue(lo[i],i,dol1,ol2,true)) { //in this case dol2 is kept fixed for all iterations
				ol1[getArrayIndex(i)]=dol1;  //save the ol1 values
			}
			else {
				return false;
			}
		}
		return true;
	}
	else return false;
}

template <class T>
bool CConfiguration<T>::computeCurrentLOValue(const double& val,const long& pos,double& ol1, double &ol2, bool fixedOl2)
{
    if ((pos>=0) && (pos<getFeeds())) {
       	if (m_2IFConversionEnabled) {
       		bool res;
       		res=compute_OL_distribution(
       	  	m_LO1Min[getArrayIndex(pos)],
       	 	m_LO1Max[getArrayIndex(pos)],
       	  	m_LO2Min[getArrayIndex(pos)],
       	  	m_LO2Max[getArrayIndex(pos)],
       	  	val,ol1,ol2,
       	  	m_LO1Injection[getArrayIndex(pos)],m_LO2Injection[getArrayIndex(pos)],
       	  	fixedOl2);
       	  	return res;       	  	
        }
        else if ((val>=m_LO1Min[getArrayIndex(pos)]) && (val<=m_LO1Max[getArrayIndex(pos)])) {
        	ol1=val;
        	ol2=0;
			return true;        
        }
        else return false;
    }
    else return false;
}

template <class T>
double CConfiguration<T>::getResultingLO(const double& ol1,const double& ol2,const long& pos)
{
	if (m_2IFConversionEnabled) {
		if (m_LO1Injection[getArrayIndex(pos)]==1) { //low injection
			return ol1+ol2;
		}
		else { //high injection
			return fabs(ol1-ol2);
		}
	}
	else { // 2IF not enabled
		return ol1;
	}
}

template <class T>
bool CConfiguration<T>::setCurrentLOValue(const double& ol1,const double& ol2,const long& pos)
{
	if ((pos>=0) && (pos<getFeeds())) {
		if (m_2IFConversionEnabled) {
			m_currentLOValue[getArrayIndex(pos)]=getResultingLO(ol1,ol2,pos);
    		m_currentLOValue[getArrayIndex(pos)+1]=getResultingLO(ol1,ol2,pos);
    		m_currentLO1Value[getArrayIndex(pos)]=ol1;
    		m_currentLO1Value[getArrayIndex(pos)+1]=ol1;
    		m_currentLO2Value[getArrayIndex(pos)]=ol2;
    		m_currentLO2Value[getArrayIndex(pos)+1]=ol2;
		}
		else {
			m_currentLOValue[getArrayIndex(pos)]=getResultingLO(ol1,ol2,pos);
    		m_currentLOValue[getArrayIndex(pos)+1]=getResultingLO(ol1,ol2,pos); 
    		m_currentLO1Value[getArrayIndex(pos)]=ol1;
    		m_currentLO1Value[getArrayIndex(pos)+1]=ol1;			
			m_currentLO2Value[getArrayIndex(pos)]=0;
    		m_currentLO2Value[getArrayIndex(pos)+1]=0;
		}
		return (updateIFLimits(getArrayIndex(pos)) && updateIFLimits(getArrayIndex(pos)+1));
    }
    return false;
}

template <class T>
bool CConfiguration<T>::updateIFLimits(const WORD &i)
{
	//long side;
	//double IFMin,IFMax,IFStart,IFBandwidth;
	IRA::DoubleConversionResult res;
	if (m_2IFConversionEnabled) { 
		res=IRA::CIRATools::calculateDualConversion(
		  m_currentLO1Value[i],m_currentLO2Value[i],
        m_BandIF2Min[i],m_BandIF2Max[i],
        m_BandIF1Min[i],m_BandIF1Max[i],
		  m_LO1Injection[i], m_LO2Injection[i]);
		if (!res.valid) {
			return false;  
		}
		else {
			ACS_LOG(LM_FULL_INFO,"CConfiguration::updateIFLimits()",(LM_DEBUG,"RF limits are : %lf %lf",res.rf_min_freq,res.rf_max_freq));
			ACS_LOG(LM_FULL_INFO,"CConfiguration::updateIFLimits()",(LM_DEBUG,"IF limits are : %lf %lf",res.if2_min_freq,res.if2_max_freq));
			ACS_LOG(LM_FULL_INFO,"CConfiguration::updateIFLimits()",(LM_DEBUG,"IF1 limits are : %lf %lf",res.if1_min_generated,res.if1_max_generated));
			m_IFStartFreq[i]=res.if2_min_freq;
    		m_IFBandWidth[i]=res.if2_bandwidth;
		}                            
	}
	else { 
		double out_IF_min, out_IF_max;                     
		if (!IRA::CIRATools::calculateIFlimits(m_currentLOValue[i], 
                             m_BandRFMin[i],m_BandRFMax[i], 
                             m_LO1Injection[i],
                             out_IF_min,out_IF_max,
                             m_IFStartFreq[i],m_IFBandWidth[i])) {
      	return false;
		}
		else {
			ACS_LOG(LM_FULL_INFO,"CConfiguration::updateIFLimits()",(LM_DEBUG,"IF limits are : %lf %lf",m_IFStartFreq[i],m_IFBandWidth[i]));
		}		
	} 
   return true;
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
    double maxRF,minRF;

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

    _GET_STRING_ATTRIBUTE(m_services,"IF1Min", "IF1 lower limit (MHz):",value,MODE_PATH);
    start=0;
    for (WORD k=0; k<m_IFs*m_feeds; k++) {
        if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("IF1Min");
            throw dummy;
        }
        m_BandIF1Min[k] = token.ToDouble();
    }

    _GET_STRING_ATTRIBUTE(m_services,"IF1Max","IF1 upper limit (MHz):",value,MODE_PATH);
    start = 0;
    for (WORD k=0;k<m_IFs*m_feeds;k++) {
        if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,error);
            dummy.setFieldName("IF1Max");
            throw dummy;
        }
        m_BandIF1Max[k]=token.ToDouble();
    }
    
    _GET_STRING_ATTRIBUTE(m_services,"IF2Min", "IF2 lower limit (MHz):",value,MODE_PATH);
    start=0;
    for (WORD k=0; k<m_IFs*m_feeds; k++) {
        if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("IF1Min");
            throw dummy;
        }
        m_BandIF2Min[k] = token.ToDouble();
    }

    _GET_STRING_ATTRIBUTE(m_services,"IF2Max","IF2 upper limit (MHz):",value,MODE_PATH);
    start = 0;
    for (WORD k=0;k<m_IFs*m_feeds;k++) {
        if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,error);
            dummy.setFieldName("IF1Max");
            throw dummy;
        }
        m_BandIF2Max[k]=token.ToDouble();
    }

    _GET_STRING_ATTRIBUTE(m_services,"DefaultLO1","Default LO1 Value (MHz):",value,MODE_PATH);
    start=0;
    for (WORD k=0; k<m_IFs*m_feeds; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("DefaultLO1");
            throw dummy;
        }
        m_DefaultLO1[k]=token.ToDouble();
    }
    
    _GET_STRING_ATTRIBUTE(m_services,"LO1Injection","LO1 side injection:",value,MODE_PATH);
    start=0;
    for (WORD k=0;k<m_IFs*m_feeds;k++) {
        if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("LO1Injection");
            throw dummy;
        }
        token.MakeUpper();
        if (token == "LOW") {
            m_LO1Injection[k]=1;
        }
        else if (token == "HIGH") {
            m_LO1Injection[k]=-1;
        }
        else {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,error);
            dummy.setFieldName("LO1Injection");
            throw dummy;
        }
    }

    _GET_STRING_ATTRIBUTE(m_services,"DefaultLO2", "LO2 Value (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0;k<m_IFs*m_feeds;k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("DefaultLO2");
            throw dummy;
        }
        m_DefaultLO2[k] = token.ToDouble();
    }
    
    _GET_STRING_ATTRIBUTE(m_services,"LO2Injection","LO1 side injection:",value,MODE_PATH);
    start=0;
    for (WORD k=0;k<m_IFs*m_feeds;k++) {
        if (!IRA::CIRATools::getNextToken(value,start,' ',token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("LO2Injection");
            throw dummy;
        }
        token.MakeUpper();
        if (token == "LOW") {
            m_LO2Injection[k]=1;
        }
        else if (token == "HIGH") {
            m_LO2Injection[k]=-1;
        }
        else {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,error);
            dummy.setFieldName("LO2Injection");
            throw dummy;
        }
    }

    _GET_STRING_ATTRIBUTE(m_services,"LO1Min", "Minimum LO1 Value (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs*m_feeds; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("LOMin");
            throw dummy;
        }
        m_LO1Min[k] = token.ToDouble();
        m_LOMin[k] = m_LO1Min[k]; 
    }

    _GET_STRING_ATTRIBUTE(m_services,"LO1Max","Maximum LO1 Value (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs*m_feeds; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("LOMax");
            throw dummy;
        }
        m_LO1Max[k] = token.ToDouble();
        m_LOMax[k] = m_LO1Max[k]; 
    }
    _GET_STRING_ATTRIBUTE(m_services,"LO2Min", "Minimum LO2 Value (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs*m_feeds; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("LO2Min");
            throw dummy;
        }
        m_LO2Min[k] = token.ToDouble();
    }
    _GET_STRING_ATTRIBUTE(m_services,"LO2Max","Maximum LO2 Value (MHz):", value, MODE_PATH);
    start = 0;
    for (WORD k=0; k<m_IFs*m_feeds; k++) {
        if (!IRA::CIRATools::getNextToken(value, start, ' ', token)) {
            _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
            dummy.setFieldName("LO2Max");
            throw dummy;
        }
        m_LO2Max[k] = token.ToDouble();
    } 
    if (m_2IFConversionEnabled) {
    	for (WORD k=0; k<m_IFs*m_feeds; k++) {
    		IRA::DoubleConversionAnalysis res;
    		res=IRA::CIRATools::analyzeDualConversion(m_BandRFMin[k],m_BandRFMax[k],
                                     m_LO1Min[k],m_LO1Max[k],
                                     m_BandIF1Min[k],m_BandIF1Max[k],
                                     m_LO2Min[k],m_LO2Max[k],
                                     m_BandIF2Min[k],m_BandIF2Max[k],
                                     m_LO1Injection[k], m_LO2Injection[k]);
         if (res.valid_configuration) {
        		m_LOMax[k]=res.max_OL_eq;
         	m_LOMin[k]=res.min_OL_eq;
         	minRF=res.min_RF_converted;
         	maxRF=res.max_RF_converted;
         	m_BandRFMin[k]=minRF;
         	m_BandRFMax[k]=maxRF;
         	ACS_LOG(LM_FULL_INFO,"CConfiguration::updateIFLimits()",(LM_DEBUG,"LO limits are : %lf %lf",m_LOMin[k],m_LOMax[k]));
         	ACS_LOG(LM_FULL_INFO,"CConfiguration::updateIFLimits()",(LM_DEBUG,"RF limits are : %lf %lf",minRF,maxRF));
         }
         else {
            _EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"CConfiguration::setMode()");
            dummy.setFieldName("RF configuration sanity check");
            throw dummy;
			}	                              
    	}	
    }
    else  {
    	for (WORD k=0; k<m_IFs*m_feeds; k++) {
	 		m_LOMax[k]=m_LO1Max[k];
      	m_LOMin[k]=m_LO1Max[k];
      }
    }
    m_mode = cmdMode;
}

template <class T>
DWORD CConfiguration<T>::getSynthesizerTable_2IF(double * &freq,double *&power) const
{
    freq= new double [m_loVectorLen_2IF];
    power=new double [m_loVectorLen_2IF];
    for (DWORD j=0;j<m_loVectorLen_2IF;j++) {
        freq[j]=m_loVector_2IF[j].frequency;
        power[j]=m_loVector_2IF[j].outputPower;
    }
    return m_loVectorLen_2IF;
}


template <class T>
DWORD CConfiguration<T>::getSynthesizerTable_K(double * &freq,double *&power) const
{
    freq= new double [m_loVectorLen_K];
    power=new double [m_loVectorLen_K];
    for (DWORD j=0;j<m_loVectorLen_K;j++) {
        freq[j]=m_loVector_K[j].frequency;
        power[j]=m_loVector_K[j].outputPower;
    }
    return m_loVectorLen_K;
}

template <class T>
DWORD CConfiguration<T>::getSynthesizerTable_Q(double * &freq,double *&power) const
{
    freq= new double [m_loVectorLen_Q];
    power=new double [m_loVectorLen_Q];
    for (DWORD j=0;j<m_loVectorLen_Q;j++) {
        freq[j]=m_loVector_Q[j].frequency;
        power[j]=m_loVector_Q[j].outputPower;
    }
    return m_loVectorLen_Q;
}

template <class T>
DWORD CConfiguration<T>::getSynthesizerTable_WL(double * &freq,double *&power) const
{
    freq= new double [m_loVectorLen_WL];
    power=new double [m_loVectorLen_WL];
    for (DWORD j=0;j<m_loVectorLen_WL;j++) {
        freq[j]=m_loVector_WL[j].frequency;
        power[j]=m_loVector_WL[j].outputPower;
    }
    return m_loVectorLen_WL;
}

template <class T>
DWORD CConfiguration<T>::getSynthesizerTable_WH(double * &freq,double *&power) const
{
    freq= new double [m_loVectorLen_WH];
    power=new double [m_loVectorLen_WH];
    for (DWORD j=0;j<m_loVectorLen_WH;j++) {
        freq[j]=m_loVector_WH[j].frequency;
        power[j]=m_loVector_WH[j].outputPower;
    }
    return m_loVectorLen_WH;
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

template <class T>
bool CConfiguration<T>::compute_OL_distribution(double a, double b, double c, double d, double OL, 
                             double& out_OL1, double& inout_OL2, 
                             long LO1Injection, long LO2Injection, 
                             bool OL2Fixed)
{
    
    // ---------------------------------------------------------
    // 1. Validazione base degli argomenti
    // ---------------------------------------------------------
    // I range min non possono essere maggiori dei max
    if (a > b || c > d) return false; 
    // L'iniezione deve essere 1 (Low) o -1 (High)
    if (std::abs(LO1Injection) != 1 || std::abs(LO2Injection) != 1) return false;

    double calc_OL1 = 0.0;
    double calc_OL2 = 0.0;

    // ---------------------------------------------------------
    // 2. Caso A: OL2 è FISSATO (OL2Fixed == true)
    // ---------------------------------------------------------
    if (OL2Fixed) {
        calc_OL2 = inout_OL2;
        // Verifica che il valore fisso sia nei limiti hardware di OL2
        if (calc_OL2 < c || calc_OL2 > d) return false;
        // Calcolo di OL1 in base alla regola dell'iniezione
        if (LO1Injection == 1) {
            // Low Side: OL = LO1 + LO2  --> LO1 = OL - LO2
            calc_OL1 = OL - calc_OL2;
        } else {
            // High Side: OL = LO1 - LO2 --> LO1 = OL + LO2
            calc_OL1 = OL + calc_OL2;
        }
        // Verifica che il risultato sia nei limiti hardware di OL1
        if (calc_OL1 < a || calc_OL1 > b) return false;
        out_OL1 = calc_OL1;
        return true;
    }

    // ---------------------------------------------------------
    // 3. Caso B: OL2 è VARIABILE (OL2Fixed == false)
    // ---------------------------------------------------------
    else {
        // Dobbiamo trovare un valore di OL2 che soddisfi sia i propri limiti [c,d]
        // sia i limiti imposti indirettamente da OL1 [a,b].
        
        double math_min_OL2, math_max_OL2;

        if (LO1Injection == 1) {
            // Low Side: OL = LO1 + LO2 --> LO2 = OL - LO1
            // Il range matematico di OL2 è determinato dagli estremi di OL1
            // Min LO2 si ha con Max LO1 (b)
            // Max LO2 si ha con Min LO1 (a)
            math_min_OL2 = OL - b;
            math_max_OL2 = OL - a;
        } else {
            // High Side: OL = LO1 - LO2 --> LO2 = LO1 - OL
            // Min LO2 si ha con Min LO1 (a)
            // Max LO2 si ha con Max LO1 (b)
            math_min_OL2 = a - OL;
            math_max_OL2 = b - OL;
        }

        // Ora calcoliamo l'intersezione tra il range fisico [c, d] 
        // e il range matematico calcolato [math_min, math_max]
        double valid_min = MAX(c, math_min_OL2);
        double valid_max = MIN(d, math_max_OL2);

        // Se il minimo supera il massimo, non esiste intersezione valida
        if (valid_min > valid_max) return false;

        // Strategia di scelta: Prendiamo il punto centrale del range valido
        // per massimizzare i margini di tolleranza, 
        calc_OL2 = (valid_min + valid_max) / 2.0;

        // Ricalcoliamo OL1 basandoci sul valore scelto di OL2
        if (LO1Injection == 1) {
            calc_OL1 = OL - calc_OL2;
        } else {
            calc_OL1 = OL + calc_OL2;
        }

        // Assegnazione parametri di uscita
        inout_OL2 = calc_OL2;
        out_OL1 = calc_OL1;
        
        return true;
    }
}


