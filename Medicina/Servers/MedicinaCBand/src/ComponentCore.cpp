#include "ComponentCore.h"
#include "Commons.h"
#include "ReceiverConfHandler.h"
#include <LogFilter.h>

_IRA_LOGFILTER_IMPORT;

// speed of light in meters per second
#define LIGHTSPEED 299792458.0

CComponentCore::CComponentCore():
	m_mixer(MixerOperator(m_configuration))
{
}

CComponentCore::~CComponentCore()
{
}

/* *** COMPONENT *** */

void CComponentCore::initialize(maci::ContainerServices* services)
{
	MED_TRACE();
    m_services=services;
    m_mixer.setServices(services);
    m_control=NULL;

    m_environmentTemperature.temperature = 20.0;
    
    m_vacuum=0.0;
    m_calDiode=false;
    m_fetValues.VDL=m_fetValues.IDL=m_fetValues.VGL=m_fetValues.VDR=m_fetValues.IDR=m_fetValues.VGR=0.0;
    
    m_statusWord=0;
    m_ioMarkError = false;
}

CConfiguration const * const  CComponentCore::execute() throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl,ComponentErrors::SocketErrorExImpl)
{
	MED_TRACE_MSG(" IN ");
    /*
     * This Call sets default Recevier Configuration at CCC_Normal 
     * User has to call for Activate() and setupMode() to be more specific about receiver conf
     */
    m_configuration.init(m_services);  //throw (ComponentErrors::CDBAccessExImpl);    
    try {
    	MED_TRACE_MSG(" Receiver new ");    	  
        m_control=new ReceiverControlCBand(
                (const char *)m_configuration.getDewarIPAddress(),
                m_configuration.getDewarPort(),
                (const char *)m_configuration.getLNAIPAddress(),
                m_configuration.getLNAPort(),
                m_configuration.getLNASamplingTime(),
                m_configuration.getFeeds()
        );
		MED_TRACE_MSG(" Receiver new done");
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CComponentCore::execute()");
        throw dummy;
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CComponentCore::execute()");
        throw dummy;
    }
    MED_TRACE_MSG(" OUT ");
    return &m_configuration;
}

void CComponentCore::cleanup()
{
    //make sure no one is using the object
    baci::ThreadSyncGuard guard(&m_mutex);
    if (m_control) {
        m_control->closeConnection();
        delete m_control;
    }
}

void CComponentCore::activate(const char *mode) throw (ReceiversErrors::ModeErrorExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,
        ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::LocalOscillatorErrorExImpl,ReceiversErrors::NoRemoteControlErrorExImpl,
        ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
	MED_TRACE_MSG(" IN ");
    /* activate mode */
    baci::ThreadSyncGuard guard(&m_mutex);
    bool l_res= m_configuration.m_conf_hnd.setConfiguration(mode);
    if (! l_res){
    		MED_TRACE_MSG(" EXC set mode");
        _EXCPT(ReceiversErrors::ModeErrorExImpl,impl,"CComponentErrors::setMode()");
        throw impl;
    }    
    guard.release();
	 MED_TRACE_MSG(" ASK IFs ");
    /* For every feed ( L R for med C) populate core member  
     * arrays eg bandwidth polarization.. */
    ReceiverConfHandler::ConfigurationSetup l_setup= m_configuration.getCurrentSetup();	   
    m_startFreq.length(l_setup.m_IFs);
    m_bandwidth.length(l_setup.m_IFs);
    m_polarization.length(l_setup.m_IFs);
    
    MED_TRACE_FMT("IFMin len %d\n",l_setup.m_IFMin.size());
	 MED_TRACE_FMT("IFBandwidth len %d\n",l_setup.m_IFBandwidth.size());
	 MED_TRACE_FMT("Polar. len %d\n",l_setup.m_polarizations.size());
    MED_TRACE_FMT("Default LO len %d\n",l_setup.m_defaultLO.size());

    try{
    for (WORD i=0; i < l_setup.m_IFs; i++) {
    	  MED_TRACE_MSG(" SET PARAMS IF ");
        m_startFreq[i]=l_setup.m_IFMin[i];
        m_bandwidth[i]=l_setup.m_IFBandwidth[i];
        m_polarization[i]=(long)l_setup.m_polarizations[i];
        m_localOscillatorValue[i]=l_setup.m_defaultLO[i];
    }
    }catch(...){
    	MED_TRACE_MSG(" EXC SET IFS ");
    }
    // Basic operations
    lnaOn(); // throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl)
    externalCalOff();
    // Remote control check
    bool answer;
    try {
    		MED_TRACE_MSG(" CHECK LOCAL REMOTE ");
        answer=m_control->isRemoteOn();
    }
    catch (IRA::ReceiverControlEx& ex) {
			MED_TRACE_MSG(" LOCAL REMOTE EXC");
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
    MED_TRACE_MSG(" OUT ");
}


void CComponentCore::deactivate() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // no guard needed.
    lnaOff(); // throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl)
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


const Management::TSystemStatus& CComponentCore::getComponentStatus()
{
    baci::ThreadSyncGuard guard(&m_mutex);
    return m_componentStatus;
}

/* *** SETUP *** */

void CComponentCore::setReceiverHigh()throw  (ReceiversErrors::ModeErrorExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,
            ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::LocalOscillatorErrorExImpl)
{
     baci::ThreadSyncGuard guard(&m_mutex);
    try {
        m_control->setReceiverHigh();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::setReceiverHigh()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    setStatusBit(C_HIGH);    
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

void CComponentCore::setReceiverLow()throw  (ReceiversErrors::ModeErrorExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,
            ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::LocalOscillatorErrorExImpl)
{
     baci::ThreadSyncGuard guard(&m_mutex);
    try {
        m_control->setReceiverLow();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::setReceiverHigh()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    setStatusBit(C_LOW);    
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

void CComponentCore::setMode(const char * mode) throw (ReceiversErrors::ModeErrorExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,
        ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::LocalOscillatorErrorExImpl)
{
	 MED_TRACE_MSG(" IN ");
    baci::ThreadSyncGuard guard(&m_mutex);
    IRA::CString cmdMode(mode);
    cmdMode.MakeUpper();        
    bool l_res= m_configuration.m_conf_hnd.setMode(mode);
    if (!l_res) {
			MED_TRACE_MSG(" EXCP ");
        _EXCPT(ReceiversErrors::ModeErrorExImpl,impl,"CComponentErrors::setMode()");
        throw impl;
    }    	 
	 ReceiverConfHandler::ConfigurationSetup l_setup= m_configuration.getCurrentSetup();	  	 	 
	 /**/
	 try{
	 	MED_TRACE_FMT("Len IFMin %d", l_setup.m_IFBandwidth.size());	 	 	
	    for (int i=0; i < l_setup.m_IFs ;i++) {
	    	 #ifdef MED_DEBUG_LVL
	    	  fprintf(stderr,"IFMin %f\n",l_setup.m_IFMin[i]);
	    	  fprintf(stderr,"IFBandwidth %f\n",l_setup.m_IFBandwidth[i]);			  
			  fprintf(stderr,"IFBandwidth %d\n",l_setup.m_polarizations[i]);
			 #endif   	     
   	     m_startFreq[i]= l_setup.m_IFMin[i];			  
      	  m_bandwidth[i]= l_setup.m_IFBandwidth[i];			  
      	  m_polarization[i]= (long)l_setup.m_polarizations[i];        
    	}
    }catch(...){
    	MED_TRACE_MSG(" EXC 1 ");	
    }
    // the set the default LO for the default LO for the selected mode.....
    ACS::doubleSeq lo;
    MED_TRACE_MSG(" LO ");
    lo.length(l_setup.m_IFs);
    try{
    	for (WORD i=0; i < l_setup.m_IFs; i++) {
    		MED_TRACE_MSG(" LO DEFAULT ");
        	lo[i]= l_setup.m_defaultLO[i];
    	}
  	 }catch(...){
    	MED_TRACE_MSG(" EXC 2 ");	
    }
    setLO(lo); // throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::LocalOscillatorErrorExImpl)    
    m_calDiode=false;
    ACS_LOG(LM_FULL_INFO,"CComponentCore::setMode()",(LM_NOTICE,"RECEIVER_MODE %s",mode));
	MED_TRACE_MSG(" OUT ");
}

const IRA::CString CComponentCore::getSetupMode(){
    baci::ThreadSyncGuard guard(&m_mutex);
    return m_configuration.m_conf_hnd.getActualConfStr();
}

/* *** LO *** */

void CComponentCore::setLO(const ACS::doubleSeq& lo)
         throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,
        ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::LocalOscillatorErrorExImpl)
{    
	MED_TRACE_MSG(" IN ");	
    baci::ThreadSyncGuard guard(&m_mutex);
    m_mixer.setValue(lo);
    ReceiverConfHandler::ConfigurationSetup l_setup= m_configuration.getCurrentSetup();	      
    double l_lo_value= m_mixer.getValue();    
    for (WORD i=0; i < l_setup.m_IFs; i++) {
        m_bandwidth[i]= l_setup.m_RFMax[i]-( m_startFreq[i] + l_lo_value );
        // the if bandwidth could never be larger than the max IF bandwidth:
        if (m_bandwidth[i] > l_setup.m_IFBandwidth[i])
            m_bandwidth[i]= l_setup.m_IFBandwidth[i];
    }
    ACS_LOG(LM_FULL_INFO,"CComponentCore::setLO()",(LM_NOTICE,"LOCAL_OSCILLATOR %lf",l_lo_value));
	MED_TRACE_MSG(" OUT ");
}

void CComponentCore::getLO(ACS::doubleSeq& lo)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    lo.length(m_configuration.getIFs());
    for (WORD i=0; i<m_configuration.getIFs(); i++) {
        lo[i]= m_mixer.getValue();
    }
}

void CComponentCore::checkLocalOscillator() throw (ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntGetAttributeExImpl)
{
    baci::ThreadSyncGuard guard(&m_mutex);    
    /* Load mixer if it's not ready */
    if (!m_mixer.isLoaded())
        m_mixer.loadComponents();
    if (!m_mixer.isLocked()) setStatusBit(UNLOCKED);
    else clearStatusBit(UNLOCKED);   
}

void CComponentCore::loadLocalOscillator() throw (ComponentErrors::CouldntGetComponentExImpl)
{
    m_mixer.loadComponents();   
}

void CComponentCore::unloadLocalOscillator()
{
    m_mixer.releaseComponents();   
}

/* *** IFS *** */

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

const DWORD CComponentCore::getIFs()
{
    baci::ThreadSyncGuard guard(&m_mutex);
    return m_configuration.getIFs();
}

const DWORD CComponentCore::getFeeds()
{
    baci::ThreadSyncGuard guard(&m_mutex);
    return m_configuration.getFeeds();
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

/* *** CAL *** */

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

void CComponentCore::calOn() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ComponentErrors::ValidationErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    /*
    if (m_setupMode=="") {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::calOn()");
        impl.setReason("receiver not configured yet");
        throw impl;
    }*/
    // guard.release();
    // if (checkStatusBit(LOCAL)) {
    //     _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::calOn()");
    //     throw impl;
    // }
    try {
        m_control->setCalibrationOn();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::calOn()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    setStatusBit(NOISEMARK);
    m_calDiode=true;
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

void CComponentCore::calOff() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ComponentErrors::ValidationErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    baci::ThreadSyncGuard guard(&m_mutex);
    /*
    if (m_setupMode=="") {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::calOff()");
        impl.setReason("receiver not configured yet");
        throw impl;
    }
    */
    // guard.release();
    // if (checkStatusBit(LOCAL)) {
    //     _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::calOff()");
    //     throw impl;
    // }
    try {
        m_control->setCalibrationOff();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::calOff()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(NOISEMARK);
    m_calDiode=false;
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}


void CComponentCore::getCalibrationMark(ACS::doubleSeq& result,
                                ACS::doubleSeq& resFreq,
                                ACS::doubleSeq& resBw,
                                const ACS::doubleSeq& freqs,
                                const ACS::doubleSeq& bandwidths,
                                const ACS::longSeq& feeds,
                                const ACS::longSeq& ifs,
                                bool& onoff,double &scaleFactor) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl)
{
    double realFreq,realBw;            
    baci::ThreadSyncGuard guard(&m_mutex);
    /* Checking start frequnency input seq length*/
    unsigned stdLen=freqs.length();
    if ((stdLen!=bandwidths.length()) || (stdLen!=feeds.length()) || (stdLen!=ifs.length())) {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::getCalibrationMark()");
        impl.setReason("sub-bands definition is not consistent");
        throw impl;
    }
    /* For every start frequency checks IF input seq */
    for (unsigned i=0;i<stdLen;i++) {
        if ((ifs[i]>=(long)m_configuration.getIFs()) || (ifs[i]<0)) {
            _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::getCalibrationMark()");
            impl.setValueName("IF identifier");
            throw impl;
        }
    }
    /* For every start frequency checks Feeds input seq */
    for (unsigned i=0;i<stdLen;i++) {
        if ((feeds[i]>=(long)m_configuration.getFeeds()) || (feeds[i]<0)) {
            _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::getCalibrationMark()");
            impl.setValueName("feed identifier");
            throw impl;
        }
    }
    /* Setting output data containers lenght */
    result.length(stdLen);
    resFreq.length(stdLen);
    resBw.length(stdLen);
    /* Ask for polynomial coeffs from configuration layer */    
    //sizeL= m_configuration.getLeftMarkCoeffs(tableLeftMark);
    //sizeR= m_configuration.getRightMarkCoeffs(tableRightMark);
    /**@todo rivedere questa parte 
     * in pratica per ogni entry del set di input 
     * freq, bw, feeds, ifs 
     * devo calcolare la relativa cal mark temp tramite
     * la formula polinomiale
     * e ritornare il dato attraverso
     * result, resFreq, resBw ?
    */
    for (unsigned i=0; i < stdLen; i++) {
        /* Get sky frequency for each input freq */
        if (!IRA::CIRATools::skyFrequency(freqs[i],bandwidths[i],m_startFreq[ifs[i]],m_bandwidth[ifs[i]],realFreq,realBw)) {
                realFreq=m_startFreq[ifs[i]];
                realBw=0.0;
        }
        ACS_LOG(LM_FULL_INFO,"CComponentCore::getCalibrationMark()",(LM_DEBUG,"SUB_BAND %lf %lf",realFreq,realBw));
        realFreq+=m_mixer.getValue();
        resFreq[i]=realFreq;
        resBw[i]=realBw;
        realFreq+=realBw/2.0;
        ACS_LOG(LM_FULL_INFO,"CComponentCore::getCalibrationMark()",(LM_DEBUG,"REFERENCE_FREQUENCY %lf",realFreq));
        if (m_polarization[ifs[i]]== (long)Receivers::RCV_LCP) {
            result[i]= m_configuration.getLeftMarkTemp(realFreq);            
            ACS_LOG(LM_FULL_INFO,"CComponentCore::getCalibrationMark()",(LM_DEBUG,"LEFT_MARK_VALUE %lf",result[i]));
        }
        else { //RCV_RCP            
            result[i]= m_configuration.getRightMarkTemp(realFreq);
            ACS_LOG(LM_FULL_INFO,"CComponentCore::getCalibrationMark()",(LM_DEBUG,"RIGHT_MARK_VALUE %lf",result[i]));
        }
    }
    scaleFactor=1.0;
    onoff=m_calDiode;    
}


void CComponentCore::updateNoiseMark() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    bool answer;
    baci::ThreadSyncGuard guard(&m_mutex);
    // not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
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
    /**********************************************************************************************/
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

/* *** LNA *** */

void CComponentCore::lnaOff() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl)
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
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

void CComponentCore::lnaOn() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl)
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
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
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

    /*
    if (m_setupMode=="") {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::getIFOutput()");
        impl.setReason("receiver not configured yet");
        throw impl;
    }
    */
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
        LO[i] = m_mixer.getValue();
    }
}

void CComponentCore::updateLNAControls() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_fetValues= m_control->fetValues(0,1, Helpers::currentConverter, Helpers::voltageConverter);
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateCryoLNAWin()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

/* *** TAPER *** */

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
    /*
    if (m_setupMode=="") {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::getTaper()");
        impl.setReason("receiver not configured yet");
        throw impl;
    }*/
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
    centralFreq=realFreq+m_mixer.getValue()+realBw/2.0;
    ACS_LOG(LM_FULL_INFO,"CComponentCore::getTaper()",(LM_DEBUG,"CENTRAL_FREQUENCY %lf",centralFreq));
    waveLen=LIGHTSPEED/(centralFreq*1000000);
    ACS_LOG(LM_FULL_INFO,"CComponentCore::getTaper()",(LM_DEBUG,"WAVELENGTH %lf",waveLen));
    size=m_configuration.getTaperTable(freqVec,taperVec);
    taper=Helpers::linearFit(freqVec,taperVec,size,centralFreq);
    ACS_LOG(LM_FULL_INFO,"CComponentCore::getTaper()",(LM_DEBUG,"TAPER %lf",taper));
    if (freqVec) delete [] freqVec;
    if (taperVec) delete [] taperVec;
    return taper;
}

/* *** FET *** */

double CComponentCore::getFetValue(const IRA::ReceiverControl::FetValue& control,const DWORD& ifs)
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
    else { //RCV_RCP
        if (control==IRA::ReceiverControl::DRAIN_VOLTAGE) return m_fetValues.VDR;
        else if (control==IRA::ReceiverControl::DRAIN_CURRENT) return m_fetValues.IDR;
        else return m_fetValues.VGR;
    }
}

/* *** VACUUM AND TEMP *** */

void CComponentCore::updateVacuum() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
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
            m_vacuum=m_control->vacuum(Helpers::voltage2mbar);
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

void CComponentCore::vacuumPumpOn() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
	if (checkStatusBit(LOCAL)) {
        _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::vacuumPumpOn()");
        throw impl;
    }
    try {
        m_control->setVacuumPumpOn();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::vacuumPumpOn()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

void CComponentCore::vacuumPumpOff() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
	if (checkStatusBit(LOCAL)) {
        _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::vacuumPumpOff()");
        throw impl;
    }
    try {
        m_control->setVacuumPumpOff();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::vacuumPumpOff()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

void CComponentCore::updateVacuumPump() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    bool answer;
    // not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
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
    //**********************************************************************************/
    // VACUUM PUMP FAULT MISSING (VACUUMPUMPFAULT)
    //************************************************************************************
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

void CComponentCore::openVacuumValve(const  char * p_mode, double p_delay) throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
	/**@todo il delay..*/
	if (checkStatusBit(LOCAL)) {
        _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::openVacuumValve()");
        throw impl;
    }
    try {
        m_control->setVacuumValveOn();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::openVacuumValve()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

void CComponentCore::closeVacuumValve() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
	/**@todo il delay..*/
	if (checkStatusBit(LOCAL)) {
        _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::closeVacuumValve()");
        throw impl;
    }
    try {
        m_control->setVacuumValveOn();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::closeVacuumValve()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

void CComponentCore::updateVacuumValve() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    bool answer;
    // not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
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

void CComponentCore::vacuumSensorOn() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl)
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
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

void CComponentCore::vacuumSensorOff() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl)
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
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

/* ** COLD SENSOR ** */

void CComponentCore::coldHeadOn() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
	if (checkStatusBit(LOCAL)) {
        _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::vacuumSensorOff()");
        throw impl;
    }
    try {
    	/* @todo */
        //m_control->set cold head on();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::vacuumSensorOff()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

void CComponentCore::coldHeadOff() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
	if (checkStatusBit(LOCAL)) {
        _EXCPT(ReceiversErrors::NoRemoteControlErrorExImpl,impl,"CComponentCore::vacuumSensorOff()");
        throw impl;
    }
    try {
    	/* @todo */
        //m_control->set cold head off();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::vacuumSensorOff()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

void CComponentCore::updateColdHead()  throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    bool answer;
    // not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        answer=m_control->isCoolHeadOn();
    }
    catch (IRA::ReceiverControlEx& ex) {
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateCoolHead()->isCoolHeadOn()");
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

void CComponentCore::updateEnvironmentTemperature() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_environmentTemperature.temperature = m_control->vertexTemperature(Helpers::voltage2Celsius);
        m_environmentTemperature.timestamp = getTimeStamp();
    }
    catch (IRA::ReceiverControlEx& ex) {
        m_environmentTemperature.temperature = CEDUMMY;
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateEnvironmentTemperature()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

void CComponentCore::updateShieldTemperature() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_shieldTemperature.temperature = m_control->cryoTemperature(0,Helpers::voltage2Kelvin);
        m_shieldTemperature.timestamp = getTimeStamp();
    }
    catch (IRA::ReceiverControlEx& ex) {
        m_environmentTemperature.temperature = CEDUMMY;
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateShieldTemperature()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

void CComponentCore::updateLnaTemperature() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    // not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
    try {
        m_lnaTemperature.temperature = m_control->cryoTemperature(1,Helpers::voltage2Kelvin);
        m_lnaTemperature.timestamp = getTimeStamp();
    }
    catch (IRA::ReceiverControlEx& ex) {
        m_environmentTemperature.temperature = CEDUMMY;
        _EXCPT(ReceiversErrors::ReceiverControlBoardErrorExImpl,impl,"CComponentCore::updateLnaTemperature()");
        impl.setDetails(ex.what().c_str());
        setStatusBit(CONNECTIONERROR);
        throw impl;
    }
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

/* *** LOCAL REMOTE CONTROL*** */

void CComponentCore::updateIsRemote() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl)
{
    bool answer;
    // not under the mutex protection because the m_control object is thread safe (at the micro controller board stage)
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

    if (!answer) setStatusBit(LOCAL);
    else clearStatusBit(LOCAL);
    clearStatusBit(CONNECTIONERROR); // the communication was ok so clear the CONNECTIONERROR bit
}

