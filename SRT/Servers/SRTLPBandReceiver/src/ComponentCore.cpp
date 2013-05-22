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
    // Members initialization
    m_LBandStartFreq.length(m_configuration.getIFs());
    m_PBandStartFreq.length(m_configuration.getIFs());
    m_LBandPolarization.length(m_configuration.getIFs());
    m_PBandPolarization.length(m_configuration.getIFs());

    for (WORD i=0; i<m_configuration.getIFs(); i++) {
        m_LBandStartFreq[i] = m_configuration.getLBandIFMin()[i];
        m_LBandPolarization[i] = (long)m_configuration.getLBandPolarizations()[i];
    }

    for (WORD i=0; i<m_configuration.getIFs(); i++) {
        m_PBandStartFreq[i] = m_configuration.getPBandIFMin()[i];
        m_PBandPolarization[i] = (long)m_configuration.getPBandPolarizations()[i];
    }

    m_setupMode="";
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
        lo[i] = m_configuration.getLBandRFMin()[i] - m_configuration.getLBandIFMin()[i];
    }
}


void CComponentCore::getPBandLO(ACS::doubleSeq& lo)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    lo.length(m_configuration.getIFs());
    for (WORD i=0; i<m_configuration.getIFs(); i++) {
        lo[i] = m_configuration.getPBandRFMin()[i] - m_configuration.getPBandIFMin()[i];
    }
}


void CComponentCore::getLBandBandwidth(ACS::doubleSeq& bw)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    bw.length(m_configuration.getIFs());
    for (WORD i=0; i<m_configuration.getIFs(); i++) {
        bw[i] = m_configuration.getLBandRFMax()[i] - m_configuration.getLBandRFMin()[i];
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
        sf[i] = m_LBandStartFreq[i];
    }
}


void CComponentCore::getPBandStartFrequency(ACS::doubleSeq& sf)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    sf.length(m_configuration.getIFs());
    for (WORD i=0; i<m_configuration.getIFs(); i++) {
        sf[i] = m_PBandStartFreq[i];
    }
}


void CComponentCore::getLBandPolarization(ACS::longSeq& pol)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    pol.length(m_configuration.getIFs());
    for (WORD i=0; i<m_configuration.getIFs(); i++) {
        pol[i] = m_LBandPolarization[i];
    }
}


void CComponentCore::getPBandPolarization(ACS::longSeq& pol)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    pol.length(m_configuration.getIFs());
    for (WORD i=0; i<m_configuration.getIFs(); i++) {
        pol[i] = m_PBandPolarization[i];
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


void CComponentCore::activate() throw (
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
    // Call the f of the derived class (setMode is pure virtual in ComponentCore).
    setMode((const char *)m_configuration.getSetupMode()); 
    guard.release();
    lnaOn(); // Throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl)
}

void CComponentCore::deactivate() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // no guard needed.
    lnaOff(); // throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl)
}

void CComponentCore::calOn() throw (
        ReceiversErrors::NoRemoteControlErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl
        )
{
    baci::ThreadSyncGuard guard(&m_mutex);
    if (m_setupMode=="") {
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
    if (m_setupMode=="") {
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

    double startFreq, bandwidth = 1.0;
    for (unsigned i=0;i<stdLen;i++) {
        startFreq = (feeds[i] == 0) ? m_LBandStartFreq[ifs[i]] : m_PBandStartFreq[ifs[i]];
        bandwidth = (feeds[i] == 0) ? m_configuration.getLBandIFBandwidth()[ifs[i]] : m_configuration.getPBandIFBandwidth()[ifs[i]];
        polarization = (feeds[i] == 0) ? m_configuration.getLBandPolarizations()[ifs[i]] : m_configuration.getPBandPolarizations()[ifs[i]];
        (feeds[i] == 0) ? getLBandLO(lo) : getPBandLO(lo);
        tableLeftFreq = NULL;
        tableLeftMark = NULL;
        tableRightFreq = NULL;
        tableRightMark = NULL;

        // Get the calibration mark tables
        sizeL = m_configuration.getLeftMarkTable(tableLeftFreq, tableLeftMark, feeds[i]);
        sizeR = m_configuration.getRightMarkTable(tableRightFreq, tableRightMark, feeds[i]);
        
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

    startFreq = (feed == 0) ? m_LBandStartFreq[ifNumber] : m_PBandStartFreq[ifNumber];
    bandwidth = (feed == 0) ? m_configuration.getLBandIFBandwidth()[ifNumber] :  m_configuration.getPBandIFBandwidth()[ifNumber];
    (feed == 0) ? getLBandLO(lo) : getPBandLO(lo);

    // take the real observed bandwidth....the correlation between detector device and the band provided by the receiver
    if (!IRA::CIRATools::skyFrequency(freq, bw, startFreq, bandwidth, realFreq, realBw)) {
        realFreq = startFreq;
        realBw = 0.0;
    }
    
    centralFreq = realFreq + lo[ifNumber] + realBw/2.0;
    ACS_LOG(LM_FULL_INFO, "CComponentCore::getTaper()", (LM_DEBUG,"CENTRAL_FREQUENCY %lf", centralFreq));
    waveLen = LIGHTSPEED / (centralFreq * 1000000);
    ACS_LOG(LM_FULL_INFO, "CComponentCore::getTaper()", (LM_DEBUG, "WAVELENGTH %lf", waveLen));
    size = m_configuration.getTaperTable(freqVec, taperVec, feed);
    taper = linearFit(freqVec, taperVec, size, centralFreq);
    ACS_LOG(LM_FULL_INFO, "CComponentCore::getTaper()", (LM_DEBUG,"TAPER %lf", taper));
    if (freqVec) delete [] freqVec;
    if (taperVec) delete [] taperVec;

    return taper;
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
    if (!answer) setStatusBit(LOCAL);
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
