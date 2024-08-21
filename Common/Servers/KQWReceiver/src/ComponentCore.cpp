#include "ComponentCore.h"
#include <LogFilter.h>
#include "utils.h"

_IRA_LOGFILTER_IMPORT;

// speed of light in meters per second
#define LIGHTSPEED 299792458.0

CComponentCore::CComponentCore() {}

CComponentCore::~CComponentCore() {}

void CComponentCore::initialize(maci::ContainerServices* services)
{
    m_services = services;
    m_control = NULL;
    m_cryoCoolHead.temperature = 0.0;
    m_cryoCoolHeadWin.temperature = 0.0;
    m_cryoLNA.temperature = 0.0;
    m_cryoLNAWin.temperature = 0.0;
    m_envTemperature.temperature = 20.0;
    m_vacuum = 0.0;
    m_fetValues.VDL = 0.0;
    m_fetValues. IDL = 0.0;
    m_fetValues.VGL = 0.0;
    m_fetValues.VDR = 0.0;
    m_fetValues.IDR = 0.0;
    m_fetValues.VGR = 0.0;
    m_statusWord = 0;
    m_ioMarkError = false;
    m_calDiode=false;
}

/*throw (
        ComponentErrors::CDBAccessExImpl,
        ComponentErrors::MemoryAllocationExImpl,
        ComponentErrors::SocketErrorExImpl,
        ReceiversErrors::ModeErrorExImpl
        )
*/
CConfiguration<maci::ContainerServices> const * const  CComponentCore::execute() 
{
    m_configuration.init(m_services);  // Throw (ComponentErrors::CDBAccessExImpl);
    try {
    	m_control=new IRA::ReceiverControl(
                (const char *)m_configuration.getDewarIPAddress(),
                m_configuration.getDewarPort(),
                (const char *)m_configuration.getLNAIPAddress(),
                m_configuration.getLNAPort(),
                m_configuration.getLNASamplingTime(),
                m_configuration.getFeeds());
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CComponentCore::execute()");
        throw dummy;
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CComponentCore::execute()");
        throw dummy;
    }
    return &m_configuration;
}


void CComponentCore::cleanup()
{
    // make sure no one is using the object
    baci::ThreadSyncGuard guard(&m_mutex);
    if (m_control) {
        m_control->closeConnection();
        delete m_control;
    }
}

const IRA::CString& CComponentCore::getActualMode()
{
    baci::ThreadSyncGuard guard(&m_mutex);
    return m_configuration.getActualMode();
}


const DWORD& CComponentCore::getIFs()
{
    baci::ThreadSyncGuard guard(&m_mutex);
    return m_configuration.getIFs();
}


const Management::TSystemStatus& CComponentCore::getComponentStatus()
{
    baci::ThreadSyncGuard guard(&m_mutex);
    return m_componentStatus;
}


const DWORD& CComponentCore::getFeeds()
{
    baci::ThreadSyncGuard guard(&m_mutex);
    return m_configuration.getFeeds();
}

/*
*throw (
        ReceiversErrors::ModeErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ComponentErrors::ValueOutofRangeExImpl,
        ComponentErrors::CouldntGetComponentExImpl,
        ComponentErrors::CORBAProblemExImpl,
        ReceiversErrors::LocalOscillatorErrorExImpl,
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
        )
*/
void CComponentCore::activate() 
{
	baci::ThreadSyncGuard guard(&m_mutex);
   setMode((const char *)m_configuration.getDefaultMode()); // Throw ......
   guard.release();
   lnaOn(); // throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl)
   externalCalOff();
   bool answer;
   try {
   	answer=m_control->isRemoteOn();
   }
   catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::activate()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
   }
   if (answer) {
   	_IRA_LOGFILTER_LOG(LM_NOTICE, "CComponentCore::activate()", "RECEIVER_COMMUNICATION_MODE_REMOTE");
   	clearStatusBit(LOCAL);
   }
   else {
   	_IRA_LOGFILTER_LOG(LM_NOTICE, "CComponentCore::activate()", "RECEIVER_COMMUNICATION_MODE_LOCAL");
   	setStatusBit(LOCAL);
   }	
}

/*
* throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl)
*/
void CComponentCore::externalCalOn() 
{
    baci::ThreadSyncGuard guard(&m_mutex);
    if (m_configuration.getActualMode()=="") {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::externalCalOn()");
        impl.setReason("receiver not configured yet");
        throw impl;
    }
    guard.release();
    try {
        m_control->setExtCalibrationOn();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::externalCalOn()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    try {
        m_control->isExtCalibrationOn() ? setStatusBit(EXTNOISEMARK) : clearStatusBit(EXTNOISEMARK);
        clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
    }    
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::externalCalOn()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }

}

/*
throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
        )
*/
void CComponentCore::externalCalOff() 
{
    baci::ThreadSyncGuard guard(&m_mutex);
    if (m_configuration.getActualMode()=="") {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::externalCalOff()");
        impl.setReason("receiver not configured yet");
        throw impl;
    }
    guard.release();
    try {
        m_control->setExtCalibrationOff();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::externalCalOff()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    try {
        m_control->isExtCalibrationOn() ? setStatusBit(EXTNOISEMARK) : clearStatusBit(EXTNOISEMARK);
        clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::externalCalOff()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
}


//throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl)
void CComponentCore::deactivate() 
{
	ACS_LOG(LM_FULL_INFO,"CComponentCore::setLO()",(LM_DEBUG,"Nothing is really required to deactivate the receiver"));
}

/*
throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
        )
*/
void CComponentCore::calOn()
{
    baci::ThreadSyncGuard guard(&m_mutex);
    if (m_configuration.getActualMode()=="") {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::calOn()");
        impl.setReason("receiver not configured yet");
        throw impl;
    }
    try {
        m_control->setCalibrationOn();
		  m_calDiode=true;    
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::calOn()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    try {
        m_control->isCalibrationOn() ? setStatusBit(NOISEMARK) : clearStatusBit(NOISEMARK);
        clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
    }    
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::calOn()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
}

/*
* throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
        )
*/
void CComponentCore::calOff() 
{
    baci::ThreadSyncGuard guard(&m_mutex);
    if (m_configuration.getActualMode()=="") {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::calOff()");
        impl.setReason("receiver not configured yet");
        throw impl;
    }
    try {
        m_control->setCalibrationOff();
        m_calDiode=false;
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::calOff()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    try {
        m_control->isCalibrationOn() ? setStatusBit(NOISEMARK) : clearStatusBit(NOISEMARK);
        clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::calOff()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
}

/*
throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
        )*/
void CComponentCore::vacuumSensorOff() 
{
    if (checkStatusBit(LOCAL)) {
        _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::vacuumSensorOff()");
        throw impl;
    }
    try {
        m_control->setVacuumSensorOff();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::vacuumSensorOff()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
}

/*
throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
        )
*/
void CComponentCore::vacuumSensorOn() 
{
    if (checkStatusBit(LOCAL)) {
        _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::vacuumSensorOn()");
        throw impl;
    }
    try {
        m_control->setVacuumSensorOn();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::vacuumSensorOn()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
}

/*
 throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
        )
*/
void CComponentCore::lnaOff()
{
    if (checkStatusBit(LOCAL)) {
    	_EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::lnaOff()");
    	throw impl;
    }
    try {
        m_control-> turnRightLNAsOff();
        m_control-> turnLeftLNAsOff();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::lnaOff()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
}

/*
 throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
        )
*/
void CComponentCore::lnaOn()
{
    if (checkStatusBit(LOCAL)) {
    	_EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::lnaOn()");
    	throw impl;
    }
    try {
        m_control-> turnRightLNAsOn();
        m_control-> turnLeftLNAsOn();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::lnaOn()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
}

/*
throw (
        ComponentErrors::ValidationErrorExImpl,
        ComponentErrors::ValueOutofRangeExImpl,
        ComponentErrors::CouldntGetComponentExImpl,
        ComponentErrors::CORBAProblemExImpl,
        ReceiversErrors::LocalOscillatorErrorExImpl
        )
*/
void CComponentCore::setLO(const ACS::doubleSeq& lo) 
{
	
    double trueValue,amp;
    double *freq=NULL;
    double *power=NULL;
    DWORD size;
    baci::ThreadSyncGuard guard(&m_mutex);
    if (lo.length()==0) {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::setLO");
        impl.setReason("at least one value must be provided");
        throw impl;
    }
	 if (lo.length()>m_configuration.getArrayLen()) {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::setLO");
        impl.setReason("too many values provided");
        throw impl;
    }
    for (WORD k=0;k<lo.length();k++) {
	 	// now check if the requested value match the limits
    	if (lo[k]<m_configuration.getLOMin()[k]) {
      	_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::setLO");
      	impl.setValueName("local oscillator lower limit");
      	impl.setValueLimit(m_configuration.getLOMin()[k]);
      	throw impl;
    	}
    	else if (lo[k]>m_configuration.getLOMax()[k]) {
      	_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::setLO");
      	impl.setValueName("local oscillator upper limit");
      	impl.setValueLimit(m_configuration.getLOMax()[k]);
      	throw impl;
    	}
    	else if (lo[k]==-1) {
      	ACS_LOG(LM_FULL_INFO,"CComponentCore::setLO()",(LM_NOTICE,"KEEP_CURRENT_LOCAL_OSCILLATOR %lf",
      	m_configuration.getCurrentLOValue()[k]));
    	}
    	else {
			//computes the synthesizer settings
    		trueValue=lo[k]+m_configuration.getFixedLO2()[k];
    		size=m_configuration.getSynthesizerTable(freq,power);
    		amp=round(linearFit(freq,power,size,trueValue));
    		if (power) delete [] power;
    		if (freq) delete [] freq;
    		ACS_LOG(LM_FULL_INFO,"CComponentCore::setLO()",(LM_DEBUG,"SYNTHESIZER_VALUES %lf %lf",trueValue,amp));
    		// make sure the synthesizer component is available
    		// Da verificare........************************************************* gestione di più OL da fare
    		//loadLocalOscillator(); // throw (ComponentErrors::CouldntGetComponentExImpl)
   		try {
        		//m_localOscillatorDevice->set(amp,trueValue);
    		}
    		catch (CORBA::SystemException& ex) {
        		m_localOscillatorFault=true;
        		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CComponentCore::setLO()");
        		impl.setName(ex._name());
        		impl.setMinor(ex.minor());
        		throw impl;
    		}
    		catch (ReceiversErrors::ReceiversErrorsEx& ex) { 
        		_ADD_BACKTRACE(ReceiversErrors::LocalOscillatorErrorExImpl,impl,ex,"CComponentCore::setLO()");
        		throw impl;
    		}
    		m_configuration.setCurrentLOValue(lo[k],k);
    		ACS_LOG(LM_FULL_INFO,"CComponentCore::setLO()",(LM_NOTICE,"LOCAL_OSCILLATOR %lf",lo[k]));			
    	}
    }
}

//throw (ComponentErrors::ValidationErrorExImpl, ComponentErrors::ValueOutofRangeExImpl)
void CComponentCore::getCalibrationMark(
        ACS::doubleSeq& result,
        ACS::doubleSeq& resFreq,
        ACS::doubleSeq& resBw,
        const ACS::doubleSeq& freqs,
        const ACS::doubleSeq& bandwidths,
        const ACS::longSeq& feeds, 
        const ACS::longSeq& ifs,
        bool& onoff,
        double& scale
        )
{
    double realFreq,realBw, startF, BW, lo;
    ACS::doubleSeq tableLeftFreq,tableLeftMark,tableRightFreq,tableRightMark;
    DWORD sizeL=0;
    DWORD sizeR=0;
	 Receivers::TPolarization polarization;

    baci::ThreadSyncGuard guard(&m_mutex);
    if (m_configuration.getActualMode()=="") {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::getCalibrationMark()");
        impl.setReason("receiver not configured yet");
        throw impl;
    }

    //let's do some checks about input data
    unsigned stdLen=freqs.length();
    if ((stdLen!=bandwidths.length()) || (stdLen!=feeds.length()) || (stdLen!=ifs.length())) {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::getCalibrationMark()");
        impl.setReason("sub-bands definition is not consistent");
        throw impl;
    }
    for (unsigned i=0;i<stdLen;i++) {
        if ((ifs[i]>=(long)m_configuration.getIFs()) || (ifs[i]<0)) {
            _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::getCalibrationMark()");
            impl.setValueName("IF identifier");
            throw impl;
        }
    }
    if(getFeeds() > 1) { 
        for (unsigned i=0;i<stdLen;i++) {
            if ((feeds[i]>=(long)m_configuration.getFeeds()) || (feeds[i]<0)) {
                _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::getCalibrationMark()");
                impl.setValueName("feed identifier");
                throw impl;
            }
        }
    }
    
    result.length(stdLen);
    resFreq.length(stdLen);
    resBw.length(stdLen);
    for (unsigned i=0;i<stdLen;i++) {
    	      // first get the calibration mark tables
    	  sizeL=m_configuration.getLeftMarkTable(tableLeftFreq,tableLeftMark,feeds[i]);
    	  sizeR=m_configuration.getRightMarkTable(tableRightFreq,tableRightMark,feeds[i]);
        // now computes the mark for each input band....considering the present mode and configuration of the receiver.
        startF=m_configuration.getBandIFMin()[m_configuration.getArrayIndex(feeds[i],ifs[i])];
        BW=m_configuration.getBandIFBandwidth()[m_configuration.getArrayIndex(feeds[i],ifs[i])];
        lo=m_configuration.getCurrentLOValue()[m_configuration.getArrayIndex(feeds[i],ifs[i])];
        polarization=m_configuration.getBandPolarizations()[m_configuration.getArrayIndex(feeds[i],ifs[i])];
        
        if (!IRA::CIRATools::skyFrequency(freqs[i],bandwidths[i],startF,BW,realFreq,realBw)) {
        		realFreq=startF;
        		realBw=0.0;
        }
        ACS_LOG(LM_FULL_INFO,"CComponentCore::getCalibrationMark()",(LM_DEBUG,"SUB_BAND %lf %lf",realFreq,realBw));
        realFreq+=lo;
        resFreq[i]=realFreq;
        resBw[i]=realBw;
        realFreq+=realBw/2.0;
        ACS_LOG(LM_FULL_INFO,"CComponentCore::getCalibrationMark()",(LM_DEBUG,"REFERENCE_FREQUENCY %lf",realFreq));
        if (polarization==Receivers::RCV_LCP) {
            result[i]=linearFit(tableLeftFreq,tableLeftMark,sizeL,realFreq);
            ACS_LOG(LM_FULL_INFO,"CComponentCore::getCalibrationMark()",(LM_DEBUG,"LEFT_MARK_VALUE %lf",result[i]));
        }
        else { //RCV_RCP
            result[i]=linearFit(tableRightFreq,tableRightMark,sizeR,realFreq);
            ACS_LOG(LM_FULL_INFO,"CComponentCore::getCalibrationMark()",(LM_DEBUG,"RIGHT_MARK_VALUE %lf",result[i]));
        }
    }
    scale=1.0;
    onoff=m_calDiode;
}    

//throw (ComponentErrors::ValidationErrorExImpl, ComponentErrors::ValueOutofRangeExImpl)
void CComponentCore::getIFOutput(
        const ACS::longSeq& feeds,
        const ACS::longSeq& ifs,
        ACS::doubleSeq& freqs,
        ACS::doubleSeq& bw,
        ACS::longSeq& pols, 
        ACS::doubleSeq& LO
        ) 
{
	 long index;
    if (m_configuration.getActualMode()=="") {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::getIFOutput()");
        impl.setReason("receiver not configured yet");
        throw impl;
    }
    // let's do some checks about input data
    unsigned stdLen=feeds.length();
    if ((stdLen!=ifs.length())) {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::getIFOutput()");
        impl.setReason("sub-bands definition is not consistent");
        throw impl;
    }
    for (unsigned i=0;i<stdLen;i++) {
        if ((ifs[i]>=(long)m_configuration.getIFs()) || (ifs[i]<0)) {
            _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::getIFOutputMark()");
            impl.setValueName("IF identifier");
            throw impl;
        }
    }
    for (unsigned i=0;i<stdLen;i++) {
        if ((feeds[i]>=(long)m_configuration.getFeeds()) || (feeds[i]<0)) {
            _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::getIFOutput()");
            impl.setValueName("feed identifier");
            throw impl;
        }
    }
        
    freqs.length(stdLen);
    bw.length(stdLen);
    pols.length(stdLen);
    LO.length(stdLen);
    for (unsigned i=0;i<stdLen;i++) {
    	index=m_configuration.getArrayIndex(feeds[i],ifs[i]);
	 	  freqs[i]=m_configuration.getBandIFMin()[index];
        bw[i]=m_configuration.getBandIFBandwidth()[index];
        LO[i]=m_configuration.getCurrentLOValue()[index];
        pols[i]=m_configuration.getBandPolarizations()[index];    	
    }
}

//throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl)
double CComponentCore::getTaper(
        const double& freq,
        const double& bw,
        const long& feed,
        const long& ifNumber,
        double& waveLen
        ) 
{
    double taper;
    double centralFreq, startFreq, bandwidth, lo;
    double realFreq, realBw;
    double *freqVec = NULL;
    double *taperVec = NULL;
    DWORD size;

    baci::ThreadSyncGuard guard(&m_mutex);
    if (m_configuration.getActualMode()=="") {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::getTaper()");
        impl.setReason("receiver not configured yet");
        throw impl;
    }
    if ((ifNumber>=(long)m_configuration.getIFs()) || (ifNumber<0)) {
        _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::getTaper()");
        impl.setValueName("IF identifier");
        throw impl;
    }
    if ((feed>=(long)m_configuration.getFeeds()) || (feed<0)) {
        _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::getTaper()");
        impl.setValueName("feed identifier");
        throw impl;
    }
	 bandwidth=m_configuration.getBandIFBandwidth()[m_configuration.getArrayIndex(feed,ifNumber)];
	 startFreq=m_configuration.getBandIFMin()[m_configuration.getArrayIndex(feed,ifNumber)];
	 lo=m_configuration.getCurrentLOValue()[m_configuration.getArrayIndex(feed,ifNumber)];
    // take the real observed bandwidth....the correlation between detector device and the band provided by the receiver
    if (!IRA::CIRATools::skyFrequency(freq,bw,startFreq,bandwidth,realFreq,realBw)) {
        realFreq=startFreq;
        realBw=0.0;
    }
    centralFreq=realFreq+lo+realBw/2.0;
    ACS_LOG(LM_FULL_INFO,"CComponentCore::getTaper()",(LM_DEBUG,"CENTRAL_FREQUENCY %lf",centralFreq));
    waveLen=LIGHTSPEED/(centralFreq*1000000);
    ACS_LOG(LM_FULL_INFO,"CComponentCore::getTaper()",(LM_DEBUG,"WAVELENGTH %lf",waveLen));
    size=m_configuration.getTaperTable(freqVec,taperVec,feed);
    taper=linearFit(freqVec,taperVec,size,centralFreq);
    ACS_LOG(LM_FULL_INFO,"CComponentCore::getTaper()",(LM_DEBUG,"TAPER %lf",taper));
    if (freqVec) delete [] freqVec;
    if (taperVec) delete [] taperVec;
    return taper;
}

/*
 * ComponentErrors::ValidationErrorExImpl
 * ComponentErrors::ValueOutofRangeExImpl
 * ComponentErrors::CouldntGetComponentExImpl
 * ComponentErrors::CORBAProblemExImpl
 * ReceiversErrors::LocalOscillatorErrorExImpl
 * ComponentErrors::CDBAccessExImpl
 * ReceiversErrors::ModeErrorExImpl
*/
void CComponentCore::setMode(const char * mode) {
    baci::ThreadSyncGuard guard(&m_mutex);
    IRA::CString cmdMode(mode);
	 m_configuration.setMode(mode); //* throw (ComponentErrors::CDBAccessExImpl, ReceiversErrors::ModeErrorExImpl)
	 
	 ACS::doubleSeq lo;
    lo.length(m_configuration.getArrayLen());
    for (WORD i=0;i<m_configuration.getArrayLen();i++) {
        lo[i]=m_configuration.getDefaultLO()[i];
    }
    // the set the default LO for the default LO for the selected mode.....
	 setLO(lo); // throw ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,
	            //       ComponentErrors::CouldntGetComponentExImpl, ComponentErrors::CORBAProblemExImpl,
	            //       ReceiversErrors::LocalOscillatorErrorExImpl)
    m_calDiode=false;
    ACS_LOG(LM_FULL_INFO,"CComponentCore::setMode()",(LM_NOTICE,"RECEIVER_MODE %s",mode));	

}

long CComponentCore::getFeeds(ACS::doubleSeq& X, ACS::doubleSeq& Y, ACS::doubleSeq& power)
{
    DWORD size;
    WORD *code;
    double *xOffset;
    double *yOffset;
    double *rPower;
    baci::ThreadSyncGuard guard(&m_mutex);
    size=m_configuration.getFeedInfo(code,xOffset,yOffset,rPower);
    X.length(size);
    Y.length(size);
    power.length(size);
    for (DWORD j=0;j<size;j++) {
        X[j]=xOffset[j];
        Y[j]=yOffset[j];
        power[j]=rPower[j];
    }
    if (xOffset) delete [] xOffset;
    if (yOffset) delete [] yOffset;
    if (rPower) delete [] rPower;
    return size;
}


void CComponentCore::updateComponent()
{
    baci::ThreadSyncGuard guard(&m_mutex);
    m_componentStatus=Management::MNG_OK;
    // if (checkStatusBit(LOCAL)) {
    //     setComponentStatus(Management::MNG_FAILURE);
    //     _IRA_LOGFILTER_LOG(LM_CRITICAL,"CComponentCore::updateComponent()","RECEIVER_NOT_REMOTELY_CONTROLLABLE");
    // }
    if (checkStatusBit(VACUUMPUMPFAULT)) {
        setComponentStatus(Management::MNG_WARNING);
        _IRA_LOGFILTER_LOG(LM_WARNING,"CComponentCore::updateComponent()","VACUUM_PUMP_FAILURE");
    }
    if (checkStatusBit(NOISEMARKERROR)) {
        setComponentStatus(Management::MNG_FAILURE);
        _IRA_LOGFILTER_LOG(LM_CRITICAL,"CComponentCore::updateComponent()","NOISE_MARK_ERROR");
    }
    if (checkStatusBit(CONNECTIONERROR)) {
        setComponentStatus(Management::MNG_FAILURE);
        _IRA_LOGFILTER_LOG(LM_CRITICAL,"CComponentCore::updateComponent()","RECEIVER_CONNECTION_ERROR");
    }
    if (checkStatusBit(UNLOCKED)) {
        setComponentStatus(Management::MNG_FAILURE);
        _IRA_LOGFILTER_LOG(LM_CRITICAL,"CComponentCore::updateComponent()","LOCAL_OSCILLATOR_NOT_LOCKED");
    }
}

//throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
void CComponentCore::updateVacuum()
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    bool vacuumSensor;
    try {
        vacuumSensor=m_control->isVacuumSensorOn();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateVacuum()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    if (vacuumSensor) {
        try {
            m_vacuum=m_control->vacuum(CComponentCore::voltage2mbar);
        }
        catch (IRA::ReceiverControlEx& ex) {
            _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateVacuum()");
            impl.setDetails(ex.what().c_str());
            setStatusBit(CONNECTIONERROR);
            throw impl;
        }
    }
    else {
        m_vacuum=m_vacuumDefault;
    }
    if (!vacuumSensor) setStatusBit(VACUUMSENSOR);
    else clearStatusBit(VACUUMSENSOR);
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

// throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
void CComponentCore::updateVacuumPump() 
{
    bool answer;
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        answer=m_control->isVacuumPumpOn();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateVacuumPump()");
        impl.setDetails(ex.what().c_str());
        baci::ThreadSyncGuard guard(&m_mutex);
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    if (!answer) setStatusBit(VACUUMPUMPSTATUS);
    else clearStatusBit(VACUUMPUMPSTATUS);
    try {
        m_control->hasVacuumPumpFault() ? setStatusBit(VACUUMPUMPFAULT): clearStatusBit(VACUUMPUMPFAULT);
        clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateVacuumPump()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
}

// throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
void CComponentCore::updateNoiseMark()
{
    bool answer;
    baci::ThreadSyncGuard guard(&m_mutex);
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        answer=m_control->isCalibrationOn();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateNoiseMark()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    if(answer!=checkStatusBit(NOISEMARK)) {
        if(m_ioMarkError) {
            setStatusBit(NOISEMARKERROR);
        }
        else {
            m_ioMarkError = true;
        }
    }
    else {
        clearStatusBit(NOISEMARKERROR);
        m_ioMarkError = false;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

// throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
void CComponentCore::updateVacuumValve() 
{
    bool answer;
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        answer=m_control->isVacuumValveOn();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateVacuumValve()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    if (!answer) setStatusBit(VACUUMVALVEOPEN);
    else clearStatusBit(VACUUMVALVEOPEN);
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

// throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
void CComponentCore::updateIsRemote() 
{
    bool answer;
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        answer=m_control->isRemoteOn();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateIsRemote()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }

    if (checkStatusBit(LOCAL) && answer) {
        _IRA_LOGFILTER_LOG(
            LM_NOTICE,
            "CComponentCore::updateIsRemote()",
            "RECEIVER_SWITCHED_FROM_LOCAL_TO_REMOTE"
        );
    }
    else if (!checkStatusBit(LOCAL) && !answer) {
        _IRA_LOGFILTER_LOG(
            LM_NOTICE,
            "CComponentCore::updateIsRemote()",
            "RECEIVER_SWITCHED_FROM_REMOTE_TO_LOCAL"
        );
    }

    if (!answer) {
        setStatusBit(LOCAL);
    }
    else clearStatusBit(LOCAL);
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

//throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
void CComponentCore::updateCoolHead()
{
    bool answer;
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        answer=m_control->isCoolHeadOn();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateCoolHead() - isCoolHeadOn()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    if (!answer) {
        setStatusBit(COOLHEADON);
        try {
            answer=m_control->isCoolHeadSetOn();
        }
        catch (IRA::ReceiverControlEx& ex) {
            _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateCoolHead() - isCoolHeadSetOn()");
            impl.setDetails(ex.what().c_str());
            setStatusBit(CONNECTIONERROR);
            throw impl;
        }
        if(!answer)
            setStatusBit(COMPRESSORFAULT);
        else
            clearStatusBit(COMPRESSORFAULT);
    }
    else 
        clearStatusBit(COOLHEADON);

    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

//throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
void CComponentCore::updateCryoCoolHead()
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_cryoCoolHead.temperature = m_control->cryoTemperature(0,CComponentCore::voltage2Kelvin);
        m_cryoCoolHead.timestamp = getTimeStamp();
    }
    catch (IRA::ReceiverControlEx& ex) {
        m_cryoCoolHead.temperature = CEDUMMY;
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateCryoCoolHead()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

// throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
void CComponentCore::updateCryoCoolHeadWin()
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_cryoCoolHeadWin.temperature = m_control->cryoTemperature(1,CComponentCore::voltage2Kelvin);
        m_cryoCoolHeadWin.timestamp = getTimeStamp();
    }
    catch (IRA::ReceiverControlEx& ex) {
        m_cryoCoolHeadWin.temperature = CEDUMMY;
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateCryoCoolHeadWin()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

// throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
void CComponentCore::updateCryoLNA()
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_cryoLNA.temperature = m_control->cryoTemperature(3,CComponentCore::voltage2Kelvin);
        m_cryoLNA.timestamp = getTimeStamp();
    }
    catch (IRA::ReceiverControlEx& ex) {
        m_cryoLNA.temperature = CEDUMMY;
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateCryoLNA()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

// throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
void CComponentCore::updateCryoLNAWin()
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_cryoLNAWin.temperature = m_control->cryoTemperature(4,CComponentCore::voltage2Kelvin);
        m_cryoLNAWin.timestamp = getTimeStamp();
    }
    catch (IRA::ReceiverControlEx& ex) {
        m_cryoLNAWin.temperature = CEDUMMY;
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateCryoLNAWin()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

// throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
void CComponentCore::updateVertexTemperature()
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_envTemperature.temperature = m_control->vertexTemperature(CComponentCore::voltage2Celsius);
        m_envTemperature.timestamp = getTimeStamp();
    }
    catch (IRA::ReceiverControlEx& ex) {
        m_envTemperature.temperature = CEDUMMY;
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateVertexTemperature()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
}

//throw (ComponentErrors::CouldntGetComponentExImpl)
void CComponentCore::loadLocalOscillator()
{
    // If reference was already taken, but an error was found....dispose the reference
    if ((!CORBA::is_nil(m_localOscillatorDevice)) && (m_localOscillatorFault)) { 
        try {
            m_services->releaseComponent((const char*)m_localOscillatorDevice->name());
        }
        catch (...) { // Dispose silently...if an error...no matter
        }
        m_localOscillatorDevice=Receivers::LocalOscillator::_nil();
    }
    if (CORBA::is_nil(m_localOscillatorDevice)) {  // Only if it has not been retrieved yet
        try {
            m_localOscillatorDevice=m_services->getComponent<Receivers::LocalOscillator>(
                    (const char*)m_configuration.getLocalOscillatorInstance()
            );
            ACS_LOG(LM_FULL_INFO,"CCore::loadAntennaBoss()",(LM_INFO,"LOCAL_OSCILLATOR_OBTAINED"))
            m_localOscillatorFault=false;
        }
        catch (maciErrType::CannotGetComponentExImpl& ex) {
            _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CComponentCore::loadLocalOscillator()");
            Impl.setComponentName((const char*)m_configuration.getLocalOscillatorInstance());
            m_localOscillatorDevice=Receivers::LocalOscillator::_nil();
            throw Impl;
        }
        catch (maciErrType::NoPermissionExImpl& ex) {
            _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CComponentCore::loadLocalOscillator()");
            Impl.setComponentName((const char*)m_configuration.getLocalOscillatorInstance());
            m_localOscillatorDevice=Receivers::LocalOscillator::_nil();
            throw Impl;
        }
        catch (maciErrType::NoDefaultComponentExImpl& ex) {
            _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CComponentCore::loadLocalOscillator()");
            Impl.setComponentName((const char*)m_configuration.getLocalOscillatorInstance());
            m_localOscillatorDevice=Receivers::LocalOscillator::_nil();
            throw Impl;
        }
    }
}

void CComponentCore::unloadLocalOscillator()
{
    if (!CORBA::is_nil(m_localOscillatorDevice)) {
        try {
            m_services->releaseComponent((const char*)m_localOscillatorDevice->name());
        }
        catch (maciErrType::CannotReleaseComponentExImpl& ex) {
            _ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CComponentCore::unloadLocalOscillator()");
            Impl.setComponentName((const char *)m_configuration.getLocalOscillatorInstance());
            Impl.log(LM_WARNING);
        }
        catch (...) {
            _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CComponentCore::unloadLocalOscillator()");
            impl.log(LM_WARNING);
        }
        m_localOscillatorDevice=Receivers::LocalOscillator::_nil();
    }
}

void CComponentCore::getBandwidth(ACS::doubleSeq& bw)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	const double *p;
	p=m_configuration.getBandIFBandwidth();
	bw.length(m_configuration.getArrayLen());
	for (long i=0;i<m_configuration.getFeeds();i++) {
		for (long j=0;i<m_configuration.getIFs();j++) {
			bw[i]=p[m_configuration.getArrayIndex(i,j)];
		}
	}
}

void CComponentCore::getInitialFreq(ACS::doubleSeq& f)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	const double *p;
	p=m_configuration.getBandIFMin();
	f.length(m_configuration.getArrayLen());
	for (long i=0;i<m_configuration.getFeeds();i++) {
		for (long j=0;i<m_configuration.getIFs();j++) {
			f[i]=p[m_configuration.getArrayIndex(i,j)];
		}
	}
}

void CComponentCore::getLocalOscillator(ACS::doubleSeq& lo)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	const double *p;
	p=m_configuration.getCurrentLOValue();
	lo.length(m_configuration.getArrayLen());
	for (long i=0;i<m_configuration.getFeeds();i++) {
		for (long j=0;i<m_configuration.getIFs();j++) {
			lo[i]=p[m_configuration.getArrayIndex(i,j)];
		}
	}	
}

void CComponentCore::getPolarizations(ACS::longSeq& pol)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	const Receivers::TPolarization *p;
	p=m_configuration.getBandPolarizations();
	pol.length(m_configuration.getArrayLen());
	for (long i=0;i<m_configuration.getFeeds();i++) {
		for (long j=0;i<m_configuration.getIFs();j++) {
			pol[i]=(long)p[m_configuration.getArrayIndex(i,j)];
		}
	}	
}

double CComponentCore::linearFit(ACS::doubleSeq& X,ACS::doubleSeq& Y, const WORD& size, double x) const
{
	double *Xv, *Yv;
	double res;
	Xv=new double[size];
	Yv=new double[size];
	for (WORD i=0;i<size;i++) {
		Xv[i]=X[i];
		Yv[i]=Y[i];
	}	 
	res=linearFit(Xv,Yv,size,x);
	delete [] Xv;
	delete [] Yv;
	return res;
}

double CComponentCore::linearFit(double *X, double *Y, const WORD& size, double x) const
{
    int low=-1,high=-1;
    for (WORD j=0;j<size;j++) {
        if (x==X[j]) {
            return Y[j];
        }
        else if (x<X[j]) {
            if (high==-1) high=j;
            else if (X[j]<X[high]) high=j;
        }
        else if (x>X[j]) { // X value is lower
            if (low==-1) low=j;
            else if (X[j]>X[low]) low=j;
        }
    }
    if ((high!=-1) && (low!=-1)) {
        double slope=X[low]-X[high];
        return ((x-X[high])/slope)*Y[low]-((x-X[low])/slope)*Y[high];
    }
    else if (high==-1) {
        return Y[low];
    }
    else if (low==-1) {
        return Y[high];
    }
    else return 0.0; // This will never happen if size!=0
}
