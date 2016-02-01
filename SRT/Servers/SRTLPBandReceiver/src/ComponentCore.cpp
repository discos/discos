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
    m_cryoCoolHead = 0.0;
    m_cryoCoolHeadWin = 0.0;
    m_cryoLNA = 0.0;
    m_cryoLNAWin = 0.0;
    m_vacuum = 0.0;
    m_fetValues.VDL = 0.0;
    m_fetValues. IDL = 0.0;
    m_fetValues.VGL = 0.0;
    m_fetValues.VDR = 0.0;
    m_fetValues.IDR = 0.0;
    m_fetValues.VGR = 0.0;
    m_statusWord = 0;
    m_setupMode = "";
}

CConfiguration const * const  CComponentCore::execute() throw (
        ComponentErrors::CDBAccessExImpl,
        ComponentErrors::MemoryAllocationExImpl,
        ComponentErrors::SocketErrorExImpl,
        ReceiversErrors::ModeErrorExImpl
        )
{
    m_configuration.init(m_services);  // Throw (ComponentErrors::CDBAccessExImpl);
    try {
        m_control = new IRA::ReceiverControl(
                (const char *)m_configuration.getDewarIPAddress(),
                m_configuration.getDewarPort(),
                (const char *)m_configuration.getLNAIPAddress(),
                m_configuration.getLNAPort(),
                m_configuration.getLNASamplingTime(),
                m_configuration.getFeeds(),
                (const char *)m_configuration.getSwitchIPAddress(),
                m_configuration.getSwitchPort(),
                0x7C,
                0x01,
                0x7C,
                0x7D,
                0x7C,
                0x01
        );
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CComponentCore::execute()");
        throw dummy;
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CComponentCore::execute()");
        throw dummy;
    }
    
    m_localOscillatorValue = m_configuration.getDefaultLO()[0];
    m_actualMode="";
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


void CComponentCore::getLBandLO(ACS::doubleSeq& lo)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    lo.length(m_configuration.getIFs());
    for (WORD i=0; i<m_configuration.getIFs(); i++) {
        lo[i] = m_localOscillatorValue;
    }
}


void CComponentCore::getPBandLO(ACS::doubleSeq& lo)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    lo.length(m_configuration.getIFs());
    for (WORD i=0; i<m_configuration.getIFs(); i++) {
        lo[i] = 0;
    }
}


void CComponentCore::getLBandBandwidth(ACS::doubleSeq& bw)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    bw.length(m_configuration.getIFs());
    double if_max, if_min, filter_max;
    for (WORD i=0; i<m_configuration.getIFs(); i++) {
        if_min = m_configuration.getLBandRFMin()[i] - m_localOscillatorValue;
        if_max = m_configuration.getLBandRFMax()[i] - m_localOscillatorValue;
        filter_max = m_configuration.getLowpassFilterMax()[0];
        if_max = (if_max < filter_max) ? if_max : filter_max;
        bw[i] = (if_max - if_min) > 0 ? (if_max - if_min) : 0;
    }
}


void CComponentCore::getPBandBandwidth(ACS::doubleSeq& bw)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    bw.length(m_configuration.getIFs());
    for (WORD i=0; i<m_configuration.getIFs(); i++) {
        bw[i] = m_configuration.getPBandRFMax()[i] - m_configuration.getPBandRFMin()[i];
    }
}


void CComponentCore::getLBandStartFrequency(ACS::doubleSeq& sf)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    sf.length(m_configuration.getIFs());
    for (WORD i=0; i<m_configuration.getIFs(); i++) {
        sf[i] = m_configuration.getLBandIFMin()[i] - m_localOscillatorValue;
    }
}


void CComponentCore::getPBandStartFrequency(ACS::doubleSeq& sf)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    sf.length(m_configuration.getIFs());
    for (WORD i=0; i<m_configuration.getIFs(); i++) {
        sf[i] = m_configuration.getPBandIFMin()[i];
    }
}


void CComponentCore::getLBandPolarization(ACS::longSeq& pol)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    pol.length(m_configuration.getIFs());
    for (WORD i=0; i<m_configuration.getIFs(); i++) {
        pol[i] = m_configuration.getLBandPolarizations()[i];
    }
}


void CComponentCore::getPBandPolarization(ACS::longSeq& pol)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    pol.length(m_configuration.getIFs());
    for (WORD i=0; i<m_configuration.getIFs(); i++) {
        pol[i] = m_configuration.getPBandPolarizations()[i];
    }
}


const IRA::CString& CComponentCore::getActualMode()
{
    baci::ThreadSyncGuard guard(&m_mutex);
    return m_actualMode;
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


void CComponentCore::activate(const char * setup_mode) throw (
        ReceiversErrors::ModeErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ComponentErrors::ValueOutofRangeExImpl,
        ComponentErrors::CouldntGetComponentExImpl,
        ComponentErrors::CORBAProblemExImpl,
        ReceiversErrors::LocalOscillatorErrorExImpl,
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
        )
{
    baci::ThreadSyncGuard guard(&m_mutex);
    setSetupMode(setup_mode); // It calls the setMode()

    ACS::doubleSeq_var lo = new ACS::doubleSeq;
    lo->length(m_configuration.getIFs());
    for (WORD k=0; k<m_configuration.getIFs(); k++) {
        lo[k] = m_configuration.getDefaultLO()[k];
    }
    setLO(lo);

    guard.release();
    lnaOn(); // Throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl)
    externalCalOff();
}


void CComponentCore::externalCalOn() throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    if (m_setupMode=="") {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::externalCalOn()");
        impl.setReason("receiver not configured yet");
        throw impl;
    }
    guard.release();
    if (checkStatusBit(LOCAL)) {
        _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::externalCalOn()");
        throw impl;
    }
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


void CComponentCore::externalCalOff() throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
        )
{
    baci::ThreadSyncGuard guard(&m_mutex);
    if (m_setupMode=="") {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::externalCalOff()");
        impl.setReason("receiver not configured yet");
        throw impl;
    }
    guard.release();
    if (checkStatusBit(LOCAL)) {
        _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::externalCalOff()");
        throw impl;
    }
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



void CComponentCore::deactivate() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // no guard needed.
    m_localOscillatorValue = m_configuration.getDefaultLO()[0];
    lnaOff(); // throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl)
}

void CComponentCore::calOn() throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
        )
{
    baci::ThreadSyncGuard guard(&m_mutex);
    if (m_actualMode=="") {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::calOn()");
        impl.setReason("receiver not configured yet");
        throw impl;
    }
    guard.release();
    if (checkStatusBit(LOCAL)) {
        _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::calOn()");
        throw impl;
    }
    try {
        m_control->setCalibrationOn();
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


void CComponentCore::calOff() throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
        )
{
    baci::ThreadSyncGuard guard(&m_mutex);
    if (m_actualMode=="") {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::calOff()");
        impl.setReason("receiver not configured yet");
        throw impl;
    }
    guard.release();
    if (checkStatusBit(LOCAL)) {
        _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::calOff()");
        throw impl;
    }
    try {
        m_control->setCalibrationOff();
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


void CComponentCore::vacuumSensorOff() throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
        )
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


void CComponentCore::vacuumSensorOn() throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
        )
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


void CComponentCore::lnaOff() throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
        )
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


void CComponentCore::lnaOn() throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
        )
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


void CComponentCore::setLO(const ACS::doubleSeq& lo) throw (
        ComponentErrors::ValidationErrorExImpl,
        ComponentErrors::ValueOutofRangeExImpl,
        ComponentErrors::CouldntGetComponentExImpl,
        ComponentErrors::CORBAProblemExImpl,
        ReceiversErrors::LocalOscillatorErrorExImpl
        )
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
    // In case -1 is given we keep the current value...so nothing to do
    if (lo[0]==-1) {
        ACS_LOG(
           LM_FULL_INFO,
           "CComponentCore::setLO()",
           (LM_NOTICE, "KEEP_CURRENT_LOCAL_OSCILLATOR %lf", m_localOscillatorValue)
        );
        return;
    }
    // Now check if the requested value match the limits
    if (lo[0] < m_configuration.getLOMin()[0]) {
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
    else if (lo[0] >= getRFMin() && lo[0] <= getRFMax()) {
        _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::setLO");
        impl.setValueName("LO frequency value not allowed. It is within the band and "\
                          "might generate strong aliasing.\nSet LO to a different value.");
        throw impl;
    }
    else if ((getRFMin() - lo[0]) >= m_configuration.getLowpassFilterMax()[0]) {
        _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::setLO");
        impl.setValueName("LO frequency value not allowed. The IF bandwidth " \
                          "is outside the low pass filter.");
        throw impl;
    }
    
    // Computes the synthesizer settings
    trueValue=lo[0]+m_configuration.getFixedLO2()[0];
    size=m_configuration.getSynthesizerTable(freq,power);
    amp=round(linearFit(freq,power,size,trueValue));
    if (power) delete [] power;
    if (freq) delete [] freq;
    ACS_LOG(LM_FULL_INFO,"CComponentCore::setLO()",(LM_DEBUG,"SYNTHESIZER_VALUES %lf %lf",trueValue,amp));
    /***************************************************************************/
    /****   COMMENT OUT when the local oscillator component will be available  */
    /***************************************************************************/
    // make sure the synthesizer component is available
    loadLocalOscillator(); // throw (ComponentErrors::CouldntGetComponentExImpl)
    try {
        m_localOscillatorDevice->set(amp,trueValue);
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
    // for (WORD i=0;i<m_configuration.getIFs();i++) {
    //     m_bandwidth[i]=m_configuration.getRFMax()[i]-(m_startFreq[i]+m_localOscillatorValue);
    //     // the if bandwidth could never be larger than the max IF bandwidth:
    //     if (m_bandwidth[i]>m_configuration.getIFBandwidth()[i]) m_bandwidth[i]=m_configuration.getIFBandwidth()[i];
    // }
    ACS_LOG(LM_FULL_INFO,"CComponentCore::setLO()",(LM_NOTICE,"LOCAL_OSCILLATOR %lf",m_localOscillatorValue));
}


void CComponentCore::getCalibrationMark(
        ACS::doubleSeq& result,
        ACS::doubleSeq& resFreq,
        ACS::doubleSeq& resBw,
        const ACS::doubleSeq& freqs,
        const ACS::doubleSeq& bandwidths,
        const ACS::longSeq& feeds, 
        const ACS::longSeq& ifs,
        double& scale
        ) throw (ComponentErrors::ValidationErrorExImpl, ComponentErrors::ValueOutofRangeExImpl)
{
    double realFreq,realBw;
    double *tableLeftFreq=NULL;
    double *tableLeftMark=NULL;
    double *tableRightFreq=NULL;
    double *tableRightMark=NULL;
    DWORD sizeL=0;
    DWORD sizeR=0;
    ACS::doubleSeq lo;
	Receivers::TPolarization polarization;

    baci::ThreadSyncGuard guard(&m_mutex);
    if (m_actualMode=="") {
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

    if(getFeeds() > 1) { // Dual feed
        for (unsigned i=0;i<stdLen;i++) {
            if ((feeds[i]>=(long)m_configuration.getFeeds()) || (feeds[i]<0)) {
                _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::getCalibrationMark()");
                impl.setValueName("feed identifier");
                throw impl;
            }
        }
    }

    IRA::CString actualMode(getActualMode());
    ACS::longSeq feeds_idx; // Backend indexes
    ACS::doubleSeq lbandValue, pbandValue;
    result.length(stdLen);
    resFreq.length(stdLen);
    resBw.length(stdLen);
    feeds_idx.length(stdLen);
    lbandValue.length(getIFs());
    pbandValue.length(getIFs());
    getLBandBandwidth(lbandValue);
    getPBandBandwidth(pbandValue);

    double startFreq, bandwidth = 1.0;
    ACS::doubleSeq ifMin;
    for (unsigned i=0;i<stdLen;i++) {
        if(actualMode.Right() == "X") { // P band conf
            getPBandStartFrequency(ifMin);
            startFreq = ifMin[i];
            bandwidth = m_configuration.getPBandIFBandwidth()[ifs[i]];
            polarization = m_configuration.getPBandPolarizations()[ifs[i]];
            getPBandLO(lo);
            for(size_t j=0; j<stdLen; j++) // Backend indexes
                feeds_idx[j] = 0; 
        }
        else if(actualMode.Left() == "X") { // L band conf
            getLBandStartFrequency(ifMin);
            startFreq = ifMin[i];
            bandwidth = m_configuration.getLBandIFBandwidth()[ifs[i]];
            polarization = m_configuration.getLBandPolarizations()[ifs[i]];
            getLBandLO(lo);
            for(size_t j=0; j<stdLen; j++) // Backend indexes
                feeds_idx[j] = 1; 
        }
        else { // Dual band conf: backend indexes
            (feeds[i] == 0) ? getPBandStartFrequency(ifMin) : getLBandStartFrequency(ifMin);
            startFreq = ifMin[i];
            bandwidth = (feeds[i] == 0) ? m_configuration.getPBandIFBandwidth()[ifs[i]] : 
                m_configuration.getLBandIFBandwidth()[ifs[i]];
            polarization = (feeds[i] == 0) ? m_configuration.getPBandPolarizations()[ifs[i]] \
                           : m_configuration.getLBandPolarizations()[ifs[i]];
            (feeds[i] == 0) ? getPBandLO(lo) : getLBandLO(lo);
            feeds_idx[i] = feeds[i];
        }

        tableLeftFreq = NULL;
        tableLeftMark = NULL;
        tableRightFreq = NULL;
        tableRightMark = NULL;

        // Get the calibration mark tables
        sizeL = m_configuration.getLeftMarkTable(tableLeftFreq, tableLeftMark, feeds_idx[i]);
        sizeR = m_configuration.getRightMarkTable(tableRightFreq, tableRightMark, feeds_idx[i]);

        // Now computes the mark for each input band, considering the present mode and configuration of the receiver.
        if (!IRA::CIRATools::skyFrequency(
                freqs[i],
                bandwidths[i],
                startFreq,
                bandwidth,
                realFreq,
                realBw
                )
        ) 
        {
            realFreq = startFreq;
            realBw = 0.0;
        }

        ACS_LOG(LM_FULL_INFO,"CComponentCore::getCalibrationMark()",(LM_DEBUG,"SUB_BAND %lf %lf", realFreq, realBw));
        realFreq += lo[i];
        resFreq[i] = realFreq;
        resBw[i] = realBw;
        realFreq += realBw / 2.0;
        ACS_LOG(LM_FULL_INFO,"CComponentCore::getCalibrationMark()",(LM_DEBUG,"REFERENCE_FREQUENCY %lf",realFreq));

        if (polarization==Receivers::RCV_LCP || polarization==Receivers::RCV_HLP) {
            result[i]=linearFit(tableLeftFreq, tableLeftMark, sizeL, realFreq);
            ACS_LOG(LM_FULL_INFO,"CComponentCore::getCalibrationMark()",(LM_DEBUG,"LEFT_MARK_VALUE %lf",result[i]));
        }
        else { // RCV_RCP
            result[i]=linearFit(tableRightFreq,tableRightMark,sizeR,realFreq);
            ACS_LOG(LM_FULL_INFO,"CComponentCore::getCalibrationMark()",(LM_DEBUG,"RIGHT_MARK_VALUE %lf",result[i]));
        }
    }

    if (tableLeftFreq) delete [] tableLeftFreq;
    if (tableLeftMark) delete [] tableLeftMark;
    if (tableRightFreq) delete [] tableRightFreq;
    if (tableRightMark) delete [] tableRightMark;
    scale = 1.0;

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

    IRA::CString actualMode(getActualMode());

    ACS::doubleSeq lo;
    for (unsigned i=0;i<stdLen;i++) {
        if(actualMode.Right() == "X") { // P band conf
            freqs[i] = m_configuration.getPBandIFMin()[ifs[i]];
            bw[i] = m_configuration.getPBandIFBandwidth()[ifs[i]];
            pols[i] = (long)m_configuration.getPBandPolarizations()[ifs[i]];
            getPBandLO(lo);
            LO[i] = lo[ifs[i]];
        }
        else if(actualMode.Left() == "X") { // L band conf
            freqs[i] = m_configuration.getLBandIFMin()[ifs[i]];
            bw[i] = m_configuration.getLBandIFBandwidth()[ifs[i]];
            pols[i] = (long)m_configuration.getLBandPolarizations()[ifs[i]];
            getLBandLO(lo);
            LO[i] = lo[ifs[i]];
        }
        else { // Dual band conf: backend indexes
            freqs[i] = (feeds[i] == 0) ? m_configuration.getPBandIFMin()[ifs[i]] : 
                m_configuration.getLBandIFMin()[ifs[i]];
            bw[i] = (feeds[i] == 0) ? m_configuration.getPBandIFBandwidth()[ifs[i]] : 
                m_configuration.getLBandIFBandwidth()[ifs[i]];
            pols[i] = (feeds[i] == 0) ? (long)m_configuration.getPBandPolarizations()[ifs[i]] : 
                (long)m_configuration.getLBandPolarizations()[ifs[i]];
            (feeds[i] == 0) ? getPBandLO(lo) : getLBandLO(lo);
            LO[i] = lo[ifs[i]];
        }
    }
}


double CComponentCore::getTaper(
        const double& freq,
        const double& bw,
        const long& feed,
        const long& ifNumber,
        double& waveLen
        ) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl)
{
    double taper;
    double centralFreq, startFreq, bandwidth;
    double realFreq, realBw;
    double *freqVec = NULL;
    double *taperVec = NULL;
    DWORD size;
    ACS::doubleSeq lo;
    short feed_idx; // Backend index

    baci::ThreadSyncGuard guard(&m_mutex);
    if (m_actualMode=="") {
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

    IRA::CString actualMode(getActualMode());

    if(actualMode.Right() == "X") { // P band conf
        startFreq = m_configuration.getPBandIFMin()[ifNumber];
        bandwidth = m_configuration.getPBandIFBandwidth()[ifNumber];
        getPBandLO(lo);
        feed_idx = 0;
    }
    else if(actualMode.Left() == "X") { // L band conf
        startFreq = m_configuration.getLBandIFMin()[ifNumber];
        bandwidth = m_configuration.getLBandIFBandwidth()[ifNumber];
        getLBandLO(lo);
        feed_idx = 1;
    }
    else { // Dual band conf: backend indexes
        startFreq = (feed == 0) ? m_configuration.getPBandIFMin()[ifNumber] :
            m_configuration.getLBandIFMin()[ifNumber];
        bandwidth = (feed == 0) ? m_configuration.getPBandIFBandwidth()[ifNumber] :  
            m_configuration.getLBandIFBandwidth()[ifNumber];
        (feed == 0) ? getPBandLO(lo) : getLBandLO(lo);
        feed_idx = feed;
    }

    // take the real observed bandwidth....the correlation between detector device and the band provided by the receiver
    if (!IRA::CIRATools::skyFrequency(freq, bw, startFreq, bandwidth, realFreq, realBw)) {
        realFreq = startFreq;
        realBw = 0.0;
    }
    
    centralFreq = realFreq + lo[ifNumber] + realBw/2.0;
    ACS_LOG(LM_FULL_INFO, "CComponentCore::getTaper()", (LM_DEBUG,"CENTRAL_FREQUENCY %lf", centralFreq));
    waveLen = LIGHTSPEED / (centralFreq * 1000000);
    ACS_LOG(LM_FULL_INFO, "CComponentCore::getTaper()", (LM_DEBUG, "WAVELENGTH %lf", waveLen));
    size = m_configuration.getTaperTable(freqVec, taperVec, feed_idx);
    taper = linearFit(freqVec, taperVec, size, centralFreq);
    ACS_LOG(LM_FULL_INFO, "CComponentCore::getTaper()", (LM_DEBUG,"TAPER %lf", taper));
    if (freqVec) delete [] freqVec;
    if (taperVec) delete [] taperVec;

    return taper;
}


void CComponentCore::setSetupMode(const char * setup_mode) throw (ReceiversErrors::ModeErrorExImpl)
{
    IRA::CString setupMode(setup_mode);
    setupMode.MakeUpper();
    if(setupMode == "PLP" || setupMode == "PPP" || setupMode == "LLP")
        m_setupMode = setupMode;
    else {
        _THROW_EXCPT(ReceiversErrors::ModeErrorExImpl, "ComponentCore::setSetupMode()");
    }

    // Call the derived class method (setMode is pure virtual in ComponentCore).
    setMode((const char *)getTargetMode()); 
    ACS_LOG(LM_FULL_INFO,"CComponentCore::setSetupMode()",(LM_NOTICE,"SETUP_MODE %s", (const char *)getTargetMode()));
}


const IRA::CString CComponentCore::getTargetMode()
{
    IRA::CString setupMode = getSetupMode().IsEmpty() ? "PLP" : getSetupMode();
    IRA::CString defaultMode = m_configuration.getDefaultMode();
    IRA::CString feed0 = setupMode[0];
    IRA::CString feed1 = setupMode[1];

    if(feed0 == feed1) { // Single feed
        if(feed0 == "L")
            return "XX" + defaultMode.Mid(2, 2);
        else
            return defaultMode.Mid(0, 2) + "XX";
    }
    else // Dual Feed
        return defaultMode;
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
    if (checkStatusBit(LOCAL)) {
        setComponentStatus(Management::MNG_FAILURE);
        _IRA_LOGFILTER_LOG(LM_CRITICAL,"CComponentCore::updateComponent()","RECEVER_NOT_REMOTELY_CONTROLLABLE");
    }
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

void CComponentCore::updateVacuum() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
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


void CComponentCore::updateVacuumPump() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
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


void CComponentCore::updateNoiseMark() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    bool answer;
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
    if (answer!=checkStatusBit(NOISEMARK)) 
        setStatusBit(NOISEMARKERROR);
    else 
        clearStatusBit(NOISEMARKERROR);
    try {
        m_control->isCalibrationOn() ? setStatusBit(NOISEMARK) : clearStatusBit(NOISEMARK);
        m_control->isExtCalibrationOn() ? setStatusBit(EXTNOISEMARK) : clearStatusBit(EXTNOISEMARK);
    }   
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateNoiseMark()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}


void CComponentCore::updateVacuumValve() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
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


void CComponentCore::updateIsRemote() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
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
    if (!answer) {
        setStatusBit(LOCAL);
    }
    else clearStatusBit(LOCAL);
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}


void CComponentCore::updateCoolHead()  throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
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


void CComponentCore::updateCryoCoolHead() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_cryoCoolHead=m_control->cryoTemperature(0,CComponentCore::voltage2Kelvin);
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateCryoCoolHead()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}


void CComponentCore::updateCryoCoolHeadWin() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_cryoCoolHeadWin=m_control->cryoTemperature(1,CComponentCore::voltage2Kelvin);
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateCryoCoolHeadWin()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}


void CComponentCore::updateCryoLNA() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_cryoLNA=m_control->cryoTemperature(3,CComponentCore::voltage2Kelvin);
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateCryoLNA()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}


void CComponentCore::updateCryoLNAWin() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_cryoLNAWin=m_control->cryoTemperature(4,CComponentCore::voltage2Kelvin);
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateCryoLNAWin()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}


void CComponentCore::updateVertexTemperature() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_envTemperature = m_control->vertexTemperature(CComponentCore::voltage2Celsius);
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateVertexTemperature()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // The communication was ok so clear the CONNECTIONERROR bit
}


void CComponentCore::loadLocalOscillator() throw (ComponentErrors::CouldntGetComponentExImpl)
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


double CComponentCore::getRFMax() {
    baci::ThreadSyncGuard guard(&m_mutex);
    double rfmax = 0.0;

    IRA::CString actualMode(getActualMode());
    if(actualMode.Right() == "X") { // P band
        rfmax = m_configuration.getPBandRFMax()[0];
    }
    else if(actualMode.Left() == "X") { // L band
        rfmax = m_configuration.getLBandRFMax()[0];
    }
    else { // Dual feed
        rfmax = m_configuration.getLBandRFMax()[0];
    }

    return rfmax;
}


double CComponentCore::getRFMin() {
    baci::ThreadSyncGuard guard(&m_mutex);
    double rfmin = 0.0;

    IRA::CString actualMode(getActualMode());
    if(actualMode.Right() == "X") { // P band
        rfmin = m_configuration.getPBandRFMin()[0];
    }
    else if(actualMode.Left() == "X") { // L band
        rfmin = m_configuration.getLBandRFMin()[0];
    }
    else { // Dual feed
        rfmin = m_configuration.getLBandRFMin()[0];
    }

    return rfmin;
}


double CComponentCore::linearFit(double *X, double *Y, const WORD& size, double x)
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
