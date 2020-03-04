#include "MFKBandBaseCore.h"
#include <LogFilter.h>
#include "MFKBandBaseUtils.h"

_IRA_LOGFILTER_DECLARE;

// speed of light in meters per second
#define LIGHTSPEED 299792458.0

CComponentCore::CComponentCore()
{
	voltage2mbar=voltage2Kelvin=voltage2Celsius=currentConverter=voltageConverter=NULL;
}

CComponentCore::~CComponentCore()
{
}

void CComponentCore::initialize(maci::ContainerServices* services)
{
    m_services=services;
    m_control=NULL;
    m_localOscillatorDevice=Receivers::LocalOscillator::_nil();
    m_localOscillatorFault=false;
    m_cryoCoolHeadWin.temperature = 0.0;
    m_cryoLNA.temperature = 0.0;
    m_cryoLNAWin.temperature = 0.0;
    m_cryoCoolHead.temperature = 0.0;
    m_envTemperature.temperature = 20.0;
    m_vacuum=0.0;
    m_fetValues.VDL=m_fetValues.IDL=m_fetValues.VGL=m_fetValues.VDR=m_fetValues.IDR=m_fetValues.VGR=0.0;
    m_statusWord=0;
    m_ioMarkError = false;
    m_calDiode=false;
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
        m_control=new IRA::ReceiverControl(
                (const char *)m_configuration.getDewarIPAddress(),
                m_configuration.getDewarPort(),
                (const char *)m_configuration.getLNAIPAddress(),
                m_configuration.getLNAPort(),
                m_configuration.getLNASamplingTime(),
                m_configuration.getFeeds()
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
    m_startFreq.length(m_configuration.getIFs());
    m_bandwidth.length(m_configuration.getIFs());
    m_polarization.length(m_configuration.getIFs());
    for (WORD i=0;i<m_configuration.getIFs();i++) {
        m_startFreq[i]=m_configuration.getIFMin()[i];
        m_bandwidth[i]=m_configuration.getIFBandwidth()[i];
        m_polarization[i]=m_configuration.getPolarizations()[i];
    }
    m_localOscillatorValue=0.0;
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


void CComponentCore::getLO(ACS::doubleSeq& lo)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    lo.length(m_configuration.getIFs() * m_configuration.getFeeds());
    for (WORD i=0;i<lo.length();i++)
       lo[i]=m_localOscillatorValue;
}


void CComponentCore::getBandwidth(ACS::doubleSeq& bw)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    bw.length(m_configuration.getIFs() * m_configuration.getFeeds());
    for (WORD i=0;i<bw.length();i++) {
        bw[i]= m_bandwidth[i % m_configuration.getIFs()];
    }
}


void CComponentCore::getStartFrequency(ACS::doubleSeq& sf)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    sf.length(m_configuration.getIFs() * m_configuration.getFeeds());
    for (WORD i=0;i<sf.length();i++) {
        sf[i]=m_startFreq[i % m_configuration.getIFs()];
    }
}


void CComponentCore::getPolarization(ACS::longSeq& pol)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    pol.length(m_configuration.getIFs() * m_configuration.getFeeds());
    for (WORD i=0;i<pol.length();i++) {
        pol[i]=m_polarization[i % m_configuration.getIFs()];
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
    m_calDiode=false; 
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
    // if (checkStatusBit(LOCAL)) {
    //     _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::externalCalOn()");
    //     throw impl;
    // }
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
    // if (checkStatusBit(LOCAL)) {
    //     _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::externalCalOff()");
    //     throw impl;
    // }
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
	m_setupMode="";
	/*************************************************************************************/
	/**************************************************************************************/	
	//lnaOff(); // throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl)
	/**************************************************************************************/
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
    // guard.release();
    // if (checkStatusBit(LOCAL)) {
    //     _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::calOn()");
    //     throw impl;
    // }
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
    // guard.release();
    // if (checkStatusBit(LOCAL)) {
    //     _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::calOff()");
    //     throw impl;
    // }
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
    // if (checkStatusBit(LOCAL)) {
    //     _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::lnaOff()");
    //     throw impl;
    // }
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
    // if (checkStatusBit(LOCAL)) {
    //     _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::lnaOn()");
    //     throw impl;
    // }
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
        ACS_LOG(LM_FULL_INFO,"CComponentCore::setLO()",(LM_NOTICE,"KEEP_CURRENT_LOCAL_OSCILLATOR %lf",m_localOscillatorValue));
        return;
    }
    // Now check if the requested value match the limits
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
    loadLocalOscillator(); // throw (ComponentErrors::CouldntGetComponentExImpl)
    if (CORBA::is_nil(m_localOscillatorDevice)) {
	 	ACS_LOG(LM_FULL_INFO,"CComponentCore::setLO()",(LM_ERROR,"Local Oscillator not available"));    	
    }
    // Computes the synthesizer settings
    trueValue=lo[0]+m_configuration.getFixedLO2()[0];
    size=m_configuration.getSynthesizerTable(freq,power);
    amp=round(linearFit(freq,power,size,trueValue));
    if (power) delete [] power;
    if (freq) delete [] freq;
    ACS_LOG(LM_FULL_INFO,"CComponentCore::setLO()",(LM_DEBUG,"SYNTHESIZER_VALUES %lf %lf",trueValue,amp));
   
    try {
	 	if (!CORBA::is_nil(m_localOscillatorDevice)) {
      	m_localOscillatorDevice->set(amp,trueValue);
     	}
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


void CComponentCore::getCalibrationMark(
        ACS::doubleSeq& result,
        ACS::doubleSeq& resFreq,
        ACS::doubleSeq& resBw,
        const ACS::doubleSeq& freqs,
        const ACS::doubleSeq& bandwidths,
        const ACS::longSeq& feeds, 
        const ACS::longSeq& ifs,
        bool &onoff,
        double& scale
        ) throw (ComponentErrors::ValidationErrorExImpl, ComponentErrors::ValueOutofRangeExImpl)
{
    double realFreq,realBw;
    double *tableLeftFreq=NULL;
    double *tableLeftMark=NULL;
    double *tableRightFreq=NULL;
    double *tableRightMark=NULL;
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

    // DWORD sizeL=0;
    // DWORD sizeR=0;
    // // first get the calibration mark tables
    // sizeL=m_configuration.getLeftMarkTable(tableLeftFreq,tableLeftMark); // To insert in the CDB
    // sizeR=m_configuration.getRightMarkTable(tableRightFreq,tableRightMark); // To insert in the CDB


    
    /*
    double LeftMarkCoeff[7][4] = { {0.2912,-21.21,506.97,-3955.5}, {0.558,-40.436,961.81,-7472.1}, {0.8963,-63.274,1469.2,-11170.0}, 
                                 {0.5176,-37.47,889.81,-6895.8}, {0.662,-47.86,1136.2,-8809.9}, {0.3535,-25.63,609.53,-4727.4}, {0.2725,-19.926,478.05,-3737.9}  };
    double RightMarkCoeff[7][4] = { {0.2141,-15.771,379.96,-2976.8}, {0.523,-37.88,899.74,-6974.4}, {0.8572,-60.324,1396.3,-10581.0}, 
                                 {0.5173,-37.339,884.31,-6838.1}, {0.7267,-52.213,1231.4,-9486.9}, {0.3955,-28.468,672.77,-5192.0}, {0.201,-15.761,398.52,-3237.7} };
    */

    vector< vector<double> > leftMarkCoeffs;
    vector< vector<double> > rightMarkCoeffs;
    const CConfiguration::TMarkValue *const markVector = m_configuration.getMarkVector();
    if(markVector == NULL) {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::getCalibrationMark()");
        impl.setReason("The markVector pointer is NULL");
        throw impl;
    }
    for(DWORD i=0; i<m_configuration.getMarkVectorLen(); i++) {
        if(markVector[i].polarization==Receivers::RCV_LCP)
            leftMarkCoeffs.push_back(markVector[i].coefficients);
        else if(markVector[i].polarization==Receivers::RCV_RCP)
            rightMarkCoeffs.push_back(markVector[i].coefficients);
        else {
            _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::getCalibrationMark()");
            impl.setReason("Polarization unknown");
            throw impl;
        }
    }
    if(leftMarkCoeffs.size() != rightMarkCoeffs.size() || leftMarkCoeffs.size() != m_configuration.getFeeds()) {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::getCalibrationMark()");
        impl.setReason("The mark coefficients length is inconsistent");
        throw impl;
    }

    double f1,f2;
    double integral;
    double mark=0;
    for (unsigned i=0;i<stdLen;i++) {
        if (m_polarization[ifs[i]]==(long)Receivers::RCV_LCP) {
            // take the real observed bandwidth....the correlation between detector device and the band provided by the receiver
            if (!IRA::CIRATools::skyFrequency(freqs[i],bandwidths[i],m_startFreq[ifs[i]],m_bandwidth[ifs[i]],realFreq,realBw)) {
                realFreq=m_startFreq[ifs[i]];
                realBw=0.0;
            }
            // realFreq+=m_LO[ifs[i]]; Questo era nel Boss. Quindi devo fare un: (?)
            // ACS::doubleSeq lo;
            // getLO(lo);
            // realFreq += lo[ifs[i]];
            realFreq+=m_localOscillatorValue; // Questo e' quanto c'era nel mio component. Devo fare come sopra per allinearlo al RecvBossCore?
            // resFreq[i]=realFreq;
            // resBw[i]=realBw;
            // realFreq+=realBw/2.0;
            f1=realFreq;
            f2=f1+realBw;
            f1/=1000.0; f2/=1000.0; //frequencies in giga Hertz
            integral=(leftMarkCoeffs[feeds[i]][0]/4)*(f2*f2*f2*f2-f1*f1*f1*f1)+(leftMarkCoeffs[feeds[i]][1]/3) * \
                     (f2*f2*f2-f1*f1*f1)+(leftMarkCoeffs[feeds[i]][2]/2)*(f2*f2-f1*f1)+leftMarkCoeffs[feeds[i]][3]*(f2-f1);               
            mark=integral/(f2-f1);              
        }
        else if (m_polarization[ifs[i]]==(long)Receivers::RCV_RCP) {
            // take the real observed bandwidth....the correlation between detectro device and the band provided by the receiver
            if (!IRA::CIRATools::skyFrequency(freqs[i],bandwidths[i],m_startFreq[ifs[i]],m_bandwidth[ifs[i]],realFreq,realBw)) {
                realFreq=m_startFreq[ifs[i]];
                realBw=0.0;
            }
            // realFreq+=m_LO[ifs[i]]; // STESSO DISCORSO DI SOPRA?
            realFreq+=m_localOscillatorValue; // Questo e' quanto c'era nel mio component. Devo fare come sopra per allinearlo al RecvBossCore?
            f1= realFreq;
            f2=f1+realBw;
            f1/=1000.0; f2/=1000.0; //frequencies in giga Hertz
            integral=(rightMarkCoeffs[feeds[i]][0]/4)*(f2*f2*f2*f2-f1*f1*f1*f1)+(rightMarkCoeffs[feeds[i]][1]/3) * \
                     (f2*f2*f2-f1*f1*f1)+(rightMarkCoeffs[feeds[i]][2]/2)*(f2*f2-f1*f1)+rightMarkCoeffs[feeds[i]][3]*(f2-f1);             
            mark=integral/(f2-f1);
        }
        result[i]=mark;
        resFreq[i]=realFreq;
        resBw[i]=realBw;
    }
    scale=1.0;
    onoff=m_calDiode;
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


double CComponentCore::getTaper(
        const double& freq,
        const double& bw,
        const long& feed,
        const long& ifNumber,
        double& waveLen
        ) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl)
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

double CComponentCore::getFetValue(const IRA::ReceiverControl::FetValue& control, const DWORD& ifs)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    if (ifs>=m_configuration.getIFs()) {
        return 0.0;
    }
    else if (m_polarization[ifs]==(long)Receivers::RCV_LCP) {
        if (control==IRA::ReceiverControl::DRAIN_VOLTAGE) return m_fetValues.VDL;
        else if (control==IRA::ReceiverControl::DRAIN_CURRENT) return m_fetValues.IDL;
        else return m_fetValues.VGL;
    }
    else {
        if (control==IRA::ReceiverControl::DRAIN_VOLTAGE) return m_fetValues.VDR;
        else if (control==IRA::ReceiverControl::DRAIN_CURRENT) return m_fetValues.IDR;
        else return m_fetValues.VGR;
    }
}


void CComponentCore::checkLocalOscillator() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::CouldntGetAttributeExImpl)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    if (m_setupMode=="") { // if the receiver is not configured the check makes no sense
        return;
    }
    // make sure the synthesizer component is available
    loadLocalOscillator(); // throw (ComponentErrors::CouldntGetComponentExImpl)
    if (CORBA::is_nil(m_localOscillatorDevice)) {
    	return;
    }
    ACSErr::Completion_var comp;
    ACS::ROlong_var isLockedRef;
    CORBA::Long isLocked;
    try {
        isLockedRef=m_localOscillatorDevice->isLocked();
    }
    catch (CORBA::SystemException& ex) {
        m_localOscillatorFault=true;
        _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CComponentCore::checkLocalOscillator()");
        impl.setName(ex._name());
        impl.setMinor(ex.minor());
        throw impl;
    }
    isLocked=isLockedRef->get_sync(comp.out()); 
    ACSErr::CompletionImpl complImpl(comp);
    if (!complImpl.isErrorFree()) {
        _ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,complImpl,"CComponentCore::checkLocalOscillator()");
        impl.setAttributeName("isLocked");
        impl.setComponentName((const char *)m_configuration.getLocalOscillatorInstance());
        throw impl;
    }
    if (!isLocked) setStatusBit(UNLOCKED);
    else clearStatusBit(UNLOCKED);
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
            m_vacuum=m_control->vacuum(voltage2mbar);
            //m_vacuum=voltage2mbar(m_control->vacuum(NULL));
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
    //*********************************************************************************************/
    // EXTNOISEMARK is missing
    //*********************************************************************************************/
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

    if (checkStatusBit(LOCAL) && answer) {
        _IRA_LOGFILTER_LOG(
            LM_INFO,
            "CComponentCore::updateIsRemote()",
            "RECEIVER_SWITCHED_FROM_LOCAL_TO_REMOTE"
        );
    }
    else if (!checkStatusBit(LOCAL) && !answer) {
        _IRA_LOGFILTER_LOG(
            LM_INFO,
            "CComponentCore::updateIsRemote()",
            "RECEIVER_SWITCHED_FROM_REMOTE_TO_LOCAL"
        );
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
        m_cryoCoolHead.temperature = m_control->cryoTemperature(0,voltage2Kelvin);
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


void CComponentCore::updateCryoCoolHeadWin() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_cryoCoolHeadWin.temperature = m_control->cryoTemperature(1,voltage2Kelvin);
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


void CComponentCore::updateCryoLNA() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_cryoLNA.temperature = m_control->cryoTemperature(3,voltage2Kelvin);
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


void CComponentCore::updateCryoLNAWin() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_cryoLNAWin.temperature = m_control->cryoTemperature(4,voltage2Kelvin);
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


void CComponentCore::updateVertexTemperature() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // Not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_envTemperature.temperature = m_control->vertexTemperature(voltage2Celsius);
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


void CComponentCore::loadLocalOscillator() throw (ComponentErrors::CouldntGetComponentExImpl)
{    
    if (m_configuration.getLocalOscillatorInstance()=="") {
    	return;
    }
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
        		if (m_configuration.getLocalOscillatorInstance()!="") {
            	m_localOscillatorDevice=m_services->getComponent<Receivers::LocalOscillator>(
                    (const char*)m_configuration.getLocalOscillatorInstance());
            	ACS_LOG(LM_FULL_INFO,"CCore::loadAntennaBoss()",(LM_INFO,"LOCAL_OSCILLATOR_OBTAINED"))
            	m_localOscillatorFault=false;
            }
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
