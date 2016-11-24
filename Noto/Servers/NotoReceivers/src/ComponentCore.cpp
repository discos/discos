#include "ComponentCore.h"
#include <LogFilter.h>
#include "Tone.h"

#define IS_LO_AVAIL m_configuration.getLocalOscillatorInstance()!=""

//_IRA_LOGFILTER_IMPORT;

// speed of light in meters per second
#define LIGHTSPEED 299792458.0

CComponentCore::CComponentCore()
{
}

CComponentCore::~CComponentCore()
{
}

void CComponentCore::initialize(maci::ContainerServices* services) throw (ComponentErrors::CDBAccessExImpl)
{
	m_configuration.init(services);  //throw (ComponentErrors::CDBAccessExImpl);
    m_services=services;
    m_localOscillatorDevice=Receivers::LocalOscillator::_nil();
    m_localOscillatorFault=false;
    m_statusWord=0;
    m_startFreq.length(2);
    m_bandwidth.length(2);
    m_polarization.length(2);
    for (WORD i=0;i<2;i++) {
        m_startFreq[i]=50.0;
        m_bandwidth[i]=100.0;
        m_polarization[i]=(long)Receivers::RCV_LCP;
        m_localOscillatorValue=0.0;
    }
    m_setupMode="";
    m_setup="";
    m_totalPower_proxy.setContainerServices(m_services);
    m_totalPower_proxy.setComponentName((const char *)m_configuration.getFocusSelctorInterface());
}

void CComponentCore::cleanup()
{
}

void CComponentCore::getLO(ACS::doubleSeq& lo)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    lo.length(m_configuration.getIFs());
    for (WORD i=0;i<m_configuration.getIFs();i++) {
        lo[i]=m_localOscillatorValue;
    }
}

void CComponentCore::getBandwidth(ACS::doubleSeq& bw)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    bw.length(m_configuration.getIFs());
    for (WORD i=0;i<m_configuration.getIFs();i++) {
        bw[i]=m_bandwidth[i];
    }
}

void CComponentCore::getStartFrequency(ACS::doubleSeq& sf)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    sf.length(m_configuration.getIFs());
    for (WORD i=0;i<m_configuration.getIFs();i++) {
        sf[i]=m_startFreq[i];
    }
}

void CComponentCore::getPolarization(ACS::longSeq& pol)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    pol.length(m_configuration.getIFs());
    for (WORD i=0;i<m_configuration.getIFs();i++) {
        pol[i]=m_polarization[i];
    }
}

const IRA::CString& CComponentCore::getSetupMode()
{
    baci::ThreadSyncGuard guard(&m_mutex);
    return m_setupMode;
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

void CComponentCore::activate(const char *setup) throw (ReceiversErrors::ModeErrorExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,
        ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::LocalOscillatorErrorExImpl,ReceiversErrors::NoRemoteControlErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl,ReceiversErrors::ConfigurationExImpl,ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    if (setup!=m_setup) { //change receiver only if required
    	loadConf(setup); //throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl,ComponentErrors::SocketErrorExImpl)
        m_setup=setup;
        CUSTOM_LOG(LM_FULL_INFO,"CComponentCore::activate()",(LM_NOTICE,"New receiver is now active: %s",setup));
        //set the default mode
        setMode((const char *)m_configuration.getSetupMode()); // Throw ......
    }
}

void CComponentCore::deactivate() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
}

void CComponentCore::setMode(const char * mode) throw (ReceiversErrors::ModeErrorExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,
        ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::LocalOscillatorErrorExImpl,
        ReceiversErrors::ConfigurationExImpl)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    IRA::CString cmdMode(mode);
    cmdMode.MakeUpper();
    if (m_setup=="") {
        _EXCPT(ReceiversErrors::ConfigurationExImpl,impl,"CComponentErrors::setMode()");
        throw impl;
    }
    if (cmdMode!=m_configuration.getSetupMode()) { // in this case i have just one allowed mode...so no need to do many checks and settings
        _EXCPT(ReceiversErrors::ModeErrorExImpl,impl,"CComponentErrors::setMode()");
        throw impl;
    }
    ACS::doubleSeq lo;
    lo.length(m_configuration.getIFs());
    for (WORD i=0;i<m_configuration.getIFs();i++) {
        lo[i]=m_configuration.getDefaultLO()[i];
    }
    setLO(lo); // throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::LocalOscillatorErrorExImpl)
    m_setupMode=mode;
    CUSTOM_LOG(LM_FULL_INFO,"CComponentCore::setMode()",(LM_NOTICE,"New receiver mode: %s",mode));
}

void CComponentCore::calOn() throw (ReceiversErrors::FocusSelectorErrorExImpl,ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
    try{
    	m_totalPower_proxy->calOn();
    }
    catch (ComponentErrors::ComponentErrorsEx& ex) {
    	_ADD_BACKTRACE(ReceiversErrors::FocusSelectorErrorExImpl,impl,ex,"CComponentCore::calOn()");
    	setStatusBit(NOISEMARKERROR);
    	throw impl;
    }
    catch (BackendsErrors::BackendsErrorsEx& ex) {
    	_ADD_BACKTRACE(ReceiversErrors::FocusSelectorErrorExImpl,impl,ex,"CComponentCore::calOn()");
    	setStatusBit(NOISEMARKERROR);
    	throw impl;
    }
    catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
    	_ADD_BACKTRACE(ReceiversErrors::FocusSelectorErrorExImpl,impl,ex,"CComponentCore::calOn()");
    	setStatusBit(NOISEMARKERROR);
    	throw impl;
    }
    catch(...) {
        _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CComponentCore::calOn()");
    	setStatusBit(NOISEMARKERROR);
    }
	CUSTOM_LOG(LM_FULL_INFO,"CRecvBossCore::calOn()",(LM_NOTICE,"Noise diode turned on"));
    clearStatusBit(NOISEMARKERROR);
    setStatusBit(NOISEMARK);
}

void CComponentCore::calOff() throw (ReceiversErrors::FocusSelectorErrorExImpl,ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
    try{
    	m_totalPower_proxy->calOff();
    }
    catch (ComponentErrors::ComponentErrorsEx& ex) {
    	_ADD_BACKTRACE(ReceiversErrors::FocusSelectorErrorExImpl,impl,ex,"CComponentCore::calOff()");
    	setStatusBit(NOISEMARKERROR);
    	throw impl;
    }
    catch (BackendsErrors::BackendsErrorsEx& ex) {
    	_ADD_BACKTRACE(ReceiversErrors::FocusSelectorErrorExImpl,impl,ex,"CComponentCore::calOff()");
    	setStatusBit(NOISEMARKERROR);
    	throw impl;
    }
    catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
    	_ADD_BACKTRACE(ReceiversErrors::FocusSelectorErrorExImpl,impl,ex,"CComponentCore::calOff()");
    	setStatusBit(NOISEMARKERROR);
    	throw impl;
    }
    catch(...) {
        _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CComponentCore::calOff()");
    	setStatusBit(NOISEMARKERROR);
    }
	CUSTOM_LOG(LM_FULL_INFO,"CRecvBossCore::calOff()",(LM_NOTICE,"Noise diode turned off"));
    clearStatusBit(NOISEMARKERROR);
    clearStatusBit(NOISEMARK);
}

void CComponentCore::antennaUnitOff() throw (ReceiversErrors::AntennaUnitErrorExImpl)
{
	CTone tone(m_configuration.getHPIBIPAddress(),m_configuration.getHPIBPort(),2000000);
	try {
		tone.init();
	}
	catch (ComponentErrors::IRALibraryResourceExImpl& ex) {
		_ADD_BACKTRACE(ReceiversErrors::AntennaUnitErrorExImpl,impl,ex,"CComponentCore::antennaUnitOff()");
		setStatusBit(AUERROR);
		throw impl;
	}
	try {
		tone.turnOff();
	}
	catch (ComponentErrors::IRALibraryResourceExImpl& ex) {
		_ADD_BACKTRACE(ReceiversErrors::AntennaUnitErrorExImpl,impl,ex,"CComponentCore::antennaUnitOff()");
		setStatusBit(AUERROR);
		throw impl;
	}
	clearStatusBit(AUERROR);
	clearStatusBit(ANTENNAUNIT);
}

void CComponentCore::antennaUnitOn() throw (ReceiversErrors::AntennaUnitErrorExImpl)
{
	CTone tone(m_configuration.getHPIBIPAddress(),m_configuration.getHPIBPort(),2000000);
	try {
		tone.init();
	}
	catch (ComponentErrors::IRALibraryResourceExImpl& ex) {
		_ADD_BACKTRACE(ReceiversErrors::AntennaUnitErrorExImpl,impl,ex,"CComponentCore::antennaUnitOn()");
		setStatusBit(AUERROR);
		throw impl;
	}
	try {
		tone.turnOff();
	}
	catch (ComponentErrors::IRALibraryResourceExImpl& ex) {
		_ADD_BACKTRACE(ReceiversErrors::AntennaUnitErrorExImpl,impl,ex,"CComponentCore::antennaUnitOn()");
		setStatusBit(AUERROR);
		throw impl;
	}
	clearStatusBit(AUERROR);
	clearStatusBit(ANTENNAUNIT);
}

void CComponentCore::setLO(const ACS::doubleSeq& lo) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,
        ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::LocalOscillatorErrorExImpl)
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
    // in case -1 is given we keep the current value...so nothing to do
    if (lo[0]==-1) {
        ACS_LOG(LM_FULL_INFO,"CComponentCore::setLO()",(LM_NOTICE,"KEEP_CURRENT_LOCAL_OSCILLATOR %lf",m_localOscillatorValue));
        return;
    }
    // now check if the requested value match the limits
    if (lo[0]<m_configuration.getLOMin()[0]) {
        _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::setLO");
        impl.setValueName("local oscillator lower limit");
        impl.setValueLimit(m_configuration.getLOMin()[0]);
        throw impl;
    }
    else if (lo[0]>m_configuration.getLOMax()[0]) {
        _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::setLO");
        impl.setValueName("local oscillator upper limit");
        impl.setValueLimit(m_configuration.getLOMax()[0]);
        throw impl;
    }
    //computes the synthesizer settings
    trueValue=lo[0]+m_configuration.getFixedLO2()[0];
    size=m_configuration.getSynthesizerTable(freq,power);
    amp=round(linearFit(freq,power,size,trueValue));
    if (power) delete [] power;
    if (freq) delete [] freq;
    ACS_LOG(LM_FULL_INFO,"CComponentCore::setLO()",(LM_DEBUG,"SYNTHESIZER_VALUES %lf %lf",trueValue,amp));
    // make sure the synthesizer component is available
    loadLocalOscillator(); // throw (ComponentErrors::CouldntGetComponentExImpl)
    try {
    	if (IS_LO_AVAIL) m_localOscillatorDevice->set(amp,trueValue);
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
    // now that the local oscillator has been properly set...let's do some easy computations
    m_localOscillatorValue=lo[0];
    for (WORD i=0;i<m_configuration.getIFs();i++) {
        m_bandwidth[i]=m_configuration.getRFMax()[i]-(m_startFreq[i]+m_localOscillatorValue);
        // the if bandwidth could never be larger than the max IF bandwidth:
        if (m_bandwidth[i]>m_configuration.getIFBandwidth()[i]) m_bandwidth[i]=m_configuration.getIFBandwidth()[i];
    }
    ACS_LOG(LM_FULL_INFO,"CComponentCore::setLO()",(LM_NOTICE,"LOCAL_OSCILLATOR %lf",m_localOscillatorValue));
}

void CComponentCore::getCalibrationMark(ACS::doubleSeq& result,ACS::doubleSeq& resFreq,ACS::doubleSeq& resBw,const ACS::doubleSeq& freqs,const ACS::doubleSeq& bandwidths,const ACS::longSeq& feeds,
            const ACS::longSeq& ifs,double &scaleFactor) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl)
{
    double realFreq,realBw;
    double *tableLeftFreq=NULL;
    double *tableLeftMark=NULL;
    double *tableRightFreq=NULL;
    double *tableRightMark=NULL;
    DWORD sizeL=0;
    DWORD sizeR=0;
    baci::ThreadSyncGuard guard(&m_mutex);
    if (m_setupMode=="") {
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
    for (unsigned i=0;i<stdLen;i++) {
        if ((feeds[i]>=(long)m_configuration.getFeeds()) || (feeds[i]<0)) {
            _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::getCalibrationMark()");
            impl.setValueName("feed identifier");
            throw impl;
        }
    }
    result.length(stdLen);
    resFreq.length(stdLen);
    resBw.length(stdLen);
    // first get the calibration mark tables
    sizeL=m_configuration.getLeftMarkTable(tableLeftFreq,tableLeftMark);
    sizeR=m_configuration.getRightMarkTable(tableRightFreq,tableRightMark);
    for (unsigned i=0;i<stdLen;i++) {
        // now computes the mark for each input band....considering the present mode and configuration of the receiver.
        if (!IRA::CIRATools::skyFrequency(freqs[i],bandwidths[i],m_startFreq[ifs[i]],m_bandwidth[ifs[i]],realFreq,realBw)) {
                realFreq=m_startFreq[ifs[i]];
                realBw=0.0;
        }
        ACS_LOG(LM_FULL_INFO,"CComponentCore::getCalibrationMark()",(LM_DEBUG,"SUB_BAND %lf %lf",realFreq,realBw));
        realFreq+=m_localOscillatorValue;
        resFreq[i]=realFreq;
        resBw[i]=realBw;
        realFreq+=realBw/2.0;
        ACS_LOG(LM_FULL_INFO,"CComponentCore::getCalibrationMark()",(LM_DEBUG,"REFERENCE_FREQUENCY %lf",realFreq));
        if (m_polarization[ifs[i]]==(long)Receivers::RCV_LCP) {
            result[i]=linearFit(tableLeftFreq,tableLeftMark,sizeL,realFreq);
            ACS_LOG(LM_FULL_INFO,"CComponentCore::getCalibrationMark()",(LM_DEBUG,"LEFT_MARK_VALUE %lf",result[i]));
        }
        else { //RCV_RCP
            result[i]=linearFit(tableRightFreq,tableRightMark,sizeR,realFreq);
            ACS_LOG(LM_FULL_INFO,"CComponentCore::getCalibrationMark()",(LM_DEBUG,"RIGHT_MARK_VALUE %lf",result[i]));
        }
    }
    scaleFactor=1.0;
    if (tableLeftFreq) delete [] tableLeftFreq;
    if (tableLeftMark) delete [] tableLeftMark;
    if (tableRightFreq) delete [] tableRightFreq;
    if (tableRightMark) delete [] tableRightMark;
}

void CComponentCore::getIFOutput(
        const ACS::longSeq& feeds,
        const ACS::longSeq& ifs,
        ACS::doubleSeq& freqs,
        ACS::doubleSeq& bw,
        ACS::longSeq& pols,
        ACS::doubleSeq& LO
        ) throw (ComponentErrors::ValidationErrorExImpl, ComponentErrors::ValueOutofRangeExImpl)
{

    if (m_setupMode=="") {
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
        freqs[i] = m_startFreq[ifs[i]];
        bw[i] = m_bandwidth[ifs[i]];
        pols[i] = m_polarization[ifs[i]];
        LO[i] = m_localOscillatorValue;
    }
}

double CComponentCore::getTaper(const double& freq,const double& bw,const long& feed,const long& ifNumber,double& waveLen) throw (
        ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl)
{
    double centralFreq;
    double taper;
    double realFreq,realBw;
    double *freqVec=NULL;
    double *taperVec=NULL;
    DWORD size;

    baci::ThreadSyncGuard guard(&m_mutex);
    if (m_setupMode=="") {
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
    // take the real observed bandwidth....the correlation between detector device and the band provided by the receiver
    if (!IRA::CIRATools::skyFrequency(freq,bw,m_startFreq[ifNumber],m_bandwidth[ifNumber],realFreq,realBw)) {
        realFreq=m_startFreq[ifNumber];
        realBw=0.0;
    }
    centralFreq=realFreq+m_localOscillatorValue+realBw/2.0;
    ACS_LOG(LM_FULL_INFO,"CComponentCore::getTaper()",(LM_DEBUG,"CENTRAL_FREQUENCY %lf",centralFreq));
    waveLen=LIGHTSPEED/(centralFreq*1000000);
    ACS_LOG(LM_FULL_INFO,"CComponentCore::getTaper()",(LM_DEBUG,"WAVELENGTH %lf",waveLen));
    size=m_configuration.getTaperTable(freqVec,taperVec);
    taper=linearFit(freqVec,taperVec,size,centralFreq);
    ACS_LOG(LM_FULL_INFO,"CComponentCore::getTaper()",(LM_DEBUG,"TAPER %lf",taper));
    if (freqVec) delete [] freqVec;
    if (taperVec) delete [] taperVec;
    return taper;
}

long CComponentCore::getFeeds(ACS::doubleSeq& X,ACS::doubleSeq& Y,ACS::doubleSeq& power)
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

void CComponentCore::checkLocalOscillator()
{
    baci::ThreadSyncGuard guard(&m_mutex);
    if (m_setupMode=="") { // if the receiver is not configured the check makes no sense
        return;
    }
    // make sure the synthesizer component is available
    loadLocalOscillator(); // throw (ComponentErrors::CouldntGetComponentExImpl)
    ACSErr::Completion_var comp;
    ACS::ROlong_var isLockedRef;
    CORBA::Long isLocked;
    try {
        if (IS_LO_AVAIL) isLockedRef=m_localOscillatorDevice->isLocked();
    }
    catch (CORBA::SystemException& ex) {
        m_localOscillatorFault=true;
        setStatusBit(LOERROR);
        return;
    }
    if (IS_LO_AVAIL) {
    	isLocked=isLockedRef->get_sync(comp.out());
    	ACSErr::CompletionImpl complImpl(comp);
    	if (!complImpl.isErrorFree()) {
    		setStatusBit(LOERROR);
    	    return;
    	}
    }
    else {
    	isLocked=true;
    }
    clearStatusBit(LOERROR);
    if (!isLocked) setStatusBit(UNLOCKED);
    else clearStatusBit(UNLOCKED);
}

void CComponentCore::updateComponent()
{
	// @TODO: implement the logging filter and start raise the warnings related to the checks below
    baci::ThreadSyncGuard guard(&m_mutex);
    m_componentStatus=Management::MNG_OK;
    if (checkStatusBit(UNLOCKED)) {
        setComponentStatus(Management::MNG_FAILURE);
        //_IRA_LOGFILTER_LOG(LM_CRITICAL,"CComponentCore::updateComponent()","RECEVER_NOT_REMOTELY_CONTROLLABLE");
    }
    if (checkStatusBit(NOISEMARKERROR)) {
        setComponentStatus(Management::MNG_FAILURE);
        //_IRA_LOGFILTER_LOG(LM_CRITICAL,"CComponentCore::updateComponent()","NOISE_MARK_ERROR");
    }
    if (checkStatusBit(AUERROR)) {
        setComponentStatus(Management::MNG_FAILURE);
        //_IRA_LOGFILTER_LOG(LM_CRITICAL,"CComponentCore::updateComponent()","RECEIVER_CONNECTION_ERROR");
    }
    if (checkStatusBit(LOERROR)) {
        setComponentStatus(Management::MNG_FAILURE);
        //_IRA_LOGFILTER_LOG(LM_CRITICAL,"CComponentCore::updateComponent()","LOCAL_OSCILLATOR_NOT_LOCKED");
    }
}

void CComponentCore::loadConf(const IRA::CString& conf) throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl,ComponentErrors::SocketErrorExImpl)
{
	m_configuration.loadConf(m_services,conf); // throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl)
    //members initialization
    m_startFreq.length(m_configuration.getIFs());
    m_bandwidth.length(m_configuration.getIFs());
    m_polarization.length(m_configuration.getIFs());
    for (WORD i=0;i<m_configuration.getIFs();i++) {
        m_startFreq[i]=m_configuration.getIFMin()[i];
        m_bandwidth[i]=m_configuration.getIFBandwidth()[i];
        m_polarization[i]=(long)m_configuration.getPolarizations()[i];
        //m_localOscillatorValue=m_configuration.getDefaultLO()[i];
    }
    m_setupMode="";
}

void CComponentCore::loadLocalOscillator() throw (ComponentErrors::CouldntGetComponentExImpl)
{
	if (!(IS_LO_AVAIL)) return;
    if ((!CORBA::is_nil(m_localOscillatorDevice)) && (m_localOscillatorFault)) { // if reference was already taken, but an error was found....dispose the reference
        try {
            m_services->releaseComponent((const char*)m_localOscillatorDevice->name());
        }
        catch (...) { //dispose silently...if an error...no matter
        }
        m_localOscillatorDevice=Receivers::LocalOscillator::_nil();
    }
    if (CORBA::is_nil(m_localOscillatorDevice)) {  //only if it has not been retrieved yet
        try {
            m_localOscillatorDevice=m_services->getComponent<Receivers::LocalOscillator>((const char*)m_configuration.getLocalOscillatorInstance());
            ACS_LOG(LM_FULL_INFO,"CCore::loadLocalOscillator()",(LM_INFO,"LOCAL_OSCILLATOR_OBTAINED"));
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
	if (!(IS_LO_AVAIL)) return;
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

double CComponentCore::linearFit(double *X,double *Y,const WORD& size,double x)
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
    else return 0.0; //this will never happen if size!=0
}
