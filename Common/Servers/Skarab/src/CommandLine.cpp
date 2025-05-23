// $Id: CommandLine.cpp,v 1.3 2011-05-12 14:14:31 c.migoni Exp $

#include <LogFilter.h>
#include "CommandLine.h"
/****************************************************************************************************************************/
#include <CustomLoggerUtils.h>
/****************************************************************************************************************************/
#define _CHECK_ERRORS(ROUTINE) \
    else if (res==FAIL) { \
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error); \
        dummy.setCode(m_Error.getErrorCode()); \
        dummy.setDescription((const char*)m_Error.getDescription()); \
        m_Error.Reset(); \
        _THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,ROUTINE); \
    } \
    else if (res==WOULDBLOCK) { \
        _THROW_EXCPT(ComponentErrors::TimeoutExImpl,ROUTINE); \
    } \
    else if (res==DECODE_ERROR) { \
        _THROW_EXCPT(BackendsErrors::MalformedAnswerExImpl,ROUTINE); \
    } \
    else { \
        _THROW_EXCPT(BackendsErrors::ConnectionExImpl,ROUTINE); \
    }

#define DECODE_ERROR -100

_IRA_LOGFILTER_IMPORT;

CCommandLine::CCommandLine(ContainerServices *service): CSocket(),
    m_services(service)
{
    AUTO_TRACE("CCommandLine::CCommandLine()");
    m_bTimedout=false;
    m_reiniting=false;
    ACS::Time init=0;
    m_lastUpdate.value(init);
    m_Error.Reset();
    m_configuration=NULL;
    m_backendStatus=0;
    setStatus(NOTCNTD);
    m_setTpiIntegration=true;
    m_stationSRT=false;
    m_stationMED=false;
    m_sectionsNumber=0;
    m_inputsNumber=0;
    m_stokes=false;
    m_TsysRange_freq=0.0;
    m_TsysRange_bw=0.0;
    m_SKARAB_1=false;
    m_SKARAB_1S=false;
    m_SKARAB_2=false;
    m_SKARAB_2S=false;
    m_SKARAB_3=false;
    m_SKARAB_3S=false;
    m_SKARAB_4=false;
    m_SKARAB_4S=false;
    m_SKARAB_5=false;
    m_SKARAB_5S=false;
    m_SKARAB_7=false;
    m_SKARAB_11=false;
    m_SKARAB_11S=false;
    m_SKARAB_12=false;
    m_SKARAB_12S=false;

    m_SkarabInitialized=false;

    m_kkg77=false;
}

CCommandLine::~CCommandLine()
{
    AUTO_TRACE("CCommandLine::~CCommandLine()");
    m_Error.Reset();
    // if the backend is transferring data...make a try to inform the backend before closing the connection
    /*if (getIsBusy()) {
        WORD len;
        char sBuff[SENDBUFFERSIZE];
        len=CProtocol::stopAcquisition(sBuff); // get the buffer
        sendBuffer(sBuff,len);
    }*/
    Close(m_Error);
}

void CCommandLine::Init(CConfiguration *config) throw (ComponentErrors::SocketErrorExImpl,
        ComponentErrors::ValidationErrorExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,BackendsErrors::NakExImpl,ComponentErrors::CDBAccessExImpl)
{
    int res;
    //WORD len;
    //char sBuff[SENDBUFFERSIZE];
    char rBuff[RECBUFFERSIZE];

    AUTO_TRACE("CCommandLine::Init()");
    m_configuration=config;
    /*if (!initializeConfiguration(m_configuration->getConfiguration())) { // throw (ComponentErrors::CDBAccessExImpl)
        IRA::CString msg;
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandLine::Init()");
        msg.Format("the requested configuration %s is not known",(const char *)m_configuration->getConfiguration());
        impl.setReason((const char *)msg);
        throw impl;
    }*/
    // this will create the socket in blocking mode.....
    if (Create(m_Error,STREAM)==FAIL) {
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
        dummy.setCode(m_Error.getErrorCode());
        dummy.setDescription((const char*)m_Error.getDescription());
        m_Error.Reset();
        _THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::Init()");
    }
    // the first time, perform a blocking connection....
    if (Connect(m_Error,m_configuration->getAddress(),m_configuration->getPort())==FAIL) {
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
        dummy.setCode(m_Error.getErrorCode());
        dummy.setDescription((const char*)m_Error.getDescription());
        m_Error.Reset();
        _THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::Init()");
    }
    else {
        setStatus(CNTD);
    }
    // set socket send buffer!!!!
    int Val=SENDBUFFERSIZE;
    if (setSockOption(m_Error,SO_SNDBUF,&Val,sizeof(int))==FAIL) {
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
        dummy.setCode(m_Error.getErrorCode());
        dummy.setDescription((const char*)m_Error.getDescription());
        m_Error.Reset();
        _THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::Init()");
    }
    // set socket in non-blocking mode.
    if (setSockMode(m_Error,NONBLOCKING)==FAIL) {
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
        dummy.setCode(m_Error.getErrorCode());
        dummy.setDescription((const char*)m_Error.getDescription());
        m_Error.Reset();
        _THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::Init()");
    }

    /* After the first connection, the server send the protocol version string */
    res=receiveBuffer(rBuff,RECBUFFERSIZE);

    m_totalPower = Backends::TotalPower::_nil();
    try {
        m_totalPower = m_services->getComponent<Backends::TotalPower>("BACKENDS/TotalPower");
    }
    catch (maciErrType::CannotGetComponentExImpl& ex) {
        _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCommadLine::Init()");
        Impl.setComponentName("BACKENDS/TotalPower");
        throw Impl;
    }
    m_receiversBoss = Receivers::ReceiversBoss::_nil();
    try {
        m_receiversBoss = m_services->getComponent<Receivers::ReceiversBoss>("RECEIVERS/Boss");
    }
    catch (maciErrType::CannotGetComponentExImpl& ex) {
        _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCommadLine::Init()");
        Impl.setComponentName("RECEIVERS/Boss");
        throw Impl;
    }
    /*m_ifDistributor = Receivers::GenericIFDistributor::_nil();
    try {
        m_ifDistributor = m_services->getComponent<Receivers::GenericIFDistributor>("RECEIVERS/SRTIFDistributor");
    }
    catch (maciErrType::CannotGetComponentExImpl& ex) {
        _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCommadLine::Init()");
        Impl.setComponentName("RECEIVERS/SRTIFDistributor");
        throw Impl;
    }*/

    char *Station;
    IRA::CString cStation;
    Station = getenv ("STATION");
    cStation = IRA::CString (Station);
    if((cStation.Compare("SRT")==0)) {
        m_stationSRT = true;
        strcpy (superVisorName, GAVINO);
    }
    if((cStation.Compare("Medicina")==0)) {
        m_stationMED = true;
        strcpy (superVisorName, PALMIRO);
    }
    m_Scheduler = Management::Scheduler::_nil ();
    try {
        m_Scheduler = m_services->getComponent < Management::Scheduler > (superVisorName);
    }
    catch (maciErrType::CannotGetComponentExImpl & ex)
    {
        _ADD_BACKTRACE (ComponentErrors::CouldntGetComponentExImpl, Impl, ex, "CommandLine::Init()");
        Impl.setComponentName (superVisorName);
        throw Impl;
    }
}

void CCommandLine::stopDataAcquisition() throw (BackendsErrors::ConnectionExImpl,BackendsErrors::NakExImpl,
        ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,ComponentErrors::NotAllowedExImpl,BackendsErrors::BackendFailExImpl)
{
    AUTO_TRACE("CCommandLine::stopDataAcquisition()");
    /*****************************************************************/
    // since both suspend and stop data acquisition are mapped into Command::stop message to the backend
    // It happens (@ the end of a scan) that the backend receives two Command::stop messages. Even if this is not
    // an issue for the backend, this leads to an unwanted behviour of the control software. The thrown exception, infact,
    // leads to skip the first subscan of the next scan. Temporarly workround if to catch the exception here. A debug messages is sent.
    /*****************************************************************/
    try {
        Message reply = sendBackendCommand(Command::stop());
    }
    catch (...) {
        ACS_LOG(LM_FULL_INFO,"CCommandLine::stopDataAcquisition()",(LM_DEBUG,"backend error while issuing a stop ascquisition"));
    }
    ACS_LOG(LM_FULL_INFO,"CCommandLine::stopDataAcquisition()",(LM_NOTICE,"TRANSFER_JOB_STOPPED"));
    clearStatusField(CCommandLine::BUSY); // unsets the component status to busy
    clearStatusField(CCommandLine::SUSPEND); // unsets the component status to transfer job suspended......
}

void CCommandLine::endSchedule() throw (BackendsErrors::ConnectionExImpl,BackendsErrors::NakExImpl,
        ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,ComponentErrors::NotAllowedExImpl,BackendsErrors::BackendFailExImpl)
{
    AUTO_TRACE("CCommandLine::endSchedule()");
    /*****************************************************************/
    // since both suspend and stop data acquisition are mapped into Command::stop message to the backend
    // It happens (@ the end of a scan) that the backend receives two Command::stop messages. Even if this is not
    // an issue for the backend, this leads to an unwanted behviour of the control software. The thrown exception, infact,
    // leads to skip the first subscan of the next scan. Temporarly workround if to catch the exception here. A debug messages is sent.
    /*****************************************************************/
    try {
        Message reply = sendBackendCommand(Command::endSchedule());
    }
    catch (...) {
        ACS_LOG(LM_FULL_INFO,"CCommandLine::endSchedule()",(LM_DEBUG,"endSchedule"));
    }
    ACS_LOG(LM_FULL_INFO,"CCommandLine::endSchedule()",(LM_NOTICE,"endSchedule"));
    clearStatusField(CCommandLine::BUSY); // unsets the component status to busy
    clearStatusField(CCommandLine::SUSPEND); // unsets the component status to transfer job suspended......
}

void CCommandLine::backendPark() throw (BackendsErrors::ConnectionExImpl,BackendsErrors::NakExImpl,
        ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,ComponentErrors::NotAllowedExImpl,BackendsErrors::BackendFailExImpl)
{
    AUTO_TRACE("CCommandLine::backendPark()");
    try {
        Message reply = sendBackendCommand(Command::backendPark());
    }
    catch (...) {
        ACS_LOG(LM_FULL_INFO,"CCommandLine::backendPark()",(LM_DEBUG,"backendPark"));
    }
    ACS_LOG(LM_FULL_INFO,"CCommandLine::backendPark()",(LM_NOTICE,"backendPark"));
}

void CCommandLine::stopDataAcquisitionForced() throw (BackendsErrors::ConnectionExImpl,BackendsErrors::NakExImpl,
        ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,ComponentErrors::NotAllowedExImpl)
{
}


void CCommandLine::startDataAcquisition() throw (BackendsErrors::BackendBusyExImpl,BackendsErrors::ConnectionExImpl,
        BackendsErrors::NakExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl)
{
    AUTO_TRACE("CCommandLine::startDataAcquisition()");
    /* if (getIsBusy()) {
        _EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::startDataAcquisition()");
        throw impl;
    }*/
    //Message reply = sendBackendCommand(Command::start());
    ACS_LOG(LM_FULL_INFO,"CCommandLine::startDataAcquisition()",(LM_NOTICE,"ACQUISTION_STARTED"));
    setStatusField(CCommandLine::BUSY); // sets the component status to busy
    setStatusField(CCommandLine::SUSPEND); // sets the component status to transfer job suspended......
}

ACS::Time CCommandLine::resumeDataAcquisition(const ACS::Time& startT) throw (BackendsErrors::ConnectionExImpl,ComponentErrors::NotAllowedExImpl,
        BackendsErrors::NakExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::BackendFailExImpl)
{
    TIMEVALUE now;
    TIMEVALUE epoch;
    long waitSec,waitMicro;
    DDWORD diff;
    ACS::Time expectedTime;
    AUTO_TRACE("CCommandLine::resumeDataAcquisition()");
    /*if ( !(m_backendStatus & (1 << SUSPEND)) || !getIsBusy()) {
        _EXCPT(ComponentErrors::NotAllowedExImpl,impl,"CCommandLine::resumeDataAcquisition()");
        impl.setReason("transfer job cannot be resumed in present configuration");
        throw impl;
    }*/
    // check that the backend latency in preparing the data transfer is respected......
    if (startT==0) { // immediate start
        IRA::CIRATools::getTime(epoch); //...so take the present time
    }
    else {
        epoch.value(startT);
        IRA::CIRATools::getTime(now);
        if (startT<=now.value().value) {
            ACS_LOG(LM_FULL_INFO,"TotalPowerImpl::sendData()",(LM_WARNING,"START_TIME_ALREADY_ELAPSED"));
        }
    }
    diff=IRA::CIRATools::timeDifference(epoch,m_acquisitionStartEpoch);
    if (diff<m_configuration->getDataLatency()) { //wait until the latency time is elapsed......
        double waitTime;
        waitMicro=m_configuration->getDataLatency()-diff;
        waitTime=((double)waitMicro/1000000.0);
        waitSec=(long)waitTime;
        waitMicro%=1000000;
        ACS_LOG(LM_FULL_INFO,"CCommandLine::resumeDataAcquisition()",(LM_WARNING,"BACKEND_LATENCY_NOT_MATCHED,WAITING %3.2lf usec",waitTime));
        IRA::CIRATools::Wait(waitSec,waitMicro);
        expectedTime=m_acquisitionStartEpoch.value().value+m_configuration->getDataLatency()*10;
    }
    else {
        expectedTime=epoch.value().value;
    }
    Message request = Command::start();
    Message reply = sendBackendCommand(request);
    if(reply.is_success_reply()) {
        ACS_LOG(LM_FULL_INFO,"CCommandLine::resumeDataAcquisition()",(LM_NOTICE,"TRANSFER_JOB_RESUMED"));
        clearStatusField(SUSPEND);
    }
    return expectedTime;
}

void CCommandLine::suspendDataAcquisition() throw (BackendsErrors::ConnectionExImpl,ComponentErrors::NotAllowedExImpl,
            BackendsErrors::NakExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::BackendFailExImpl)
{
    AUTO_TRACE("CCommandLine::suspendDataAcquisition()");
    /*if ((m_backendStatus & (1 << SUSPEND)) || !getIsBusy()) { //not suspended....running
        _EXCPT(ComponentErrors::NotAllowedExImpl,impl,"CCommandLine::suspendDataAcquisition()");
        impl.setReason("transfer job cannot be suspended in present configuration");
        throw impl;
    }*/
    Message request = Command::stop();
    Message reply = sendBackendCommand(request);
    if(reply.is_success_reply()) {
        ACS_LOG(LM_FULL_INFO,"CCommandLine::suspendDataAcquisition()",(LM_NOTICE,"TRANSFER_JOB_SUSPENDED"));
        setStatusField(SUSPEND);
    }
}

void CCommandLine::setAttenuation(const long&inputId, const double& attenuation) throw (BackendsErrors::BackendBusyExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,BackendsErrors::NakExImpl,
        ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl)
{
    AUTO_TRACE("CCommandLine::setAttenuation()");
    double newAtt;

    if ((inputId < 0 ) || (inputId >= MAX_SECTION_NUMBER)) {
        /*//if (inputId>=m_sectionsNumber) {
        if (inputId>m_inputsNumber) { // TBC !!!
            _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandLine::setAttenuation()");
            impl.setReason("the input identifier is out of range");
            throw impl;
        }
    }
    else {*/
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandLine::setAttenuation()");
        impl.setReason("the input identifier is out of range");
        throw impl;
    }
    if (attenuation>=0) { // // the user ask for a new value
        if  (attenuation<MIN_ATTENUATION) {
            _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setAttenuation()");
            impl.setValueName("attenuation");
            impl.setValueLimit(MIN_ATTENUATION);
            throw impl;
        }
        else if (attenuation>MAX_ATTENUATION) {
            _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setAttenuation()");
            impl.setValueName("attenuation");
            impl.setValueLimit(MAX_ATTENUATION);
            throw impl;
        }
        m_attenuation[inputId]=attenuation;
        try {
            m_totalPower->setAttenuation(inputId, attenuation);
            ACS_LOG(LM_FULL_INFO,"CCommandLine::setAttenuation()",(LM_NOTICE,"TOTALPOWER_ATTENUATION INPUT=%ld,ATTENUATION=%lf",inputId,attenuation));
        }
        catch (...) {
            _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCommandLine::setAttenuation()");
            impl.log(LM_ERROR);
        }
    }
    else {
        newAtt=m_attenuation[inputId];
    }

}

void CCommandLine::setTsysRange(const double& freq, const double& bw) throw            (BackendsErrors::BackendBusyExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,BackendsErrors::NakExImpl,
        ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl)
{
    AUTO_TRACE("CCommandLine::setTsysRange()");
    m_TsysRange_freq = freq;
    m_TsysRange_bw = bw;
    ACS_LOG(LM_FULL_INFO,"CCommandLine::setTsysRange()",(LM_NOTICE,"TSYS RANGE FREQUENCY = %lf, BANDWIDTH = %lf",m_TsysRange_freq,m_TsysRange_bw));

}

void CCommandLine::setConfiguration(const long& inputId,const double& freq,const double& bw,const long& feed,const long& pol, const double& sr,const long& bins) throw (
        ComponentErrors::ValidationErrorExImpl,BackendsErrors::ConnectionExImpl,BackendsErrors::NakExImpl,
        ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,ComponentErrors::ValueOutofRangeExImpl,
        BackendsErrors::BackendBusyExImpl)
{
    AUTO_TRACE("CCommandLine::setConfiguration()");
    double newBW,newSR,newFreq;
    long newBins, newFeed, newPol;
    double filter;
    double ddcoffset;
    ACS::doubleSeq_var lo_var;
    ACS::ROdoubleSeq_var loRef;
    ACS::doubleSeq lo;
    ACS::ROlong_var IFNumberRO;
    long IFNumber;
    ACSErr::Completion_var completion;

    if (m_SkarabInitialized == true) {

/*    if (getIsBusy()) {
        _EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setConfiguration()");
        throw impl;
    }*/
    if (inputId>=0) {  //check the section id is in valid ranges
        //if (inputId>=m_sectionsNumber) {
        if (inputId>m_sectionsNumber) {
            _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandLine::setConfiguration()");
            impl.setReason("the section identifier is out of range");
            throw impl;
        }
    }
    else {
        _EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandLine::setConfiguration()");
        impl.setReason("the section identifier is out of range");
        throw impl;
    }
    if (bw>=0) { // the user ask for a new value
        if (bw<MIN_BAND_WIDTH)  {
            _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setConfiguration()");
            impl.setValueName("bandWidth");
            impl.setValueLimit(MIN_BAND_WIDTH);
            throw impl;
        }
        else if (bw>MAX_BAND_WIDTH) {
            _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setConfiguration()");
            impl.setValueName("bandWidth");
            impl.setValueLimit(MAX_BAND_WIDTH);
            throw impl;
        }
        newBW=bw;
    }
    else { // else keep the present value
        newBW=m_bandWidth[inputId];
    }
    if (sr>=0) {// the user ask for a new value
        if ((sr > MAX_SAMPLE_RATE) || (sr != 2*newBW)) {
            _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setConfiguration()");
            impl.setValueName("sampleRate");
            impl.setValueLimit(MAX_SAMPLE_RATE);
            throw impl;
        }
        newSR=sr;
    }
    else {
        newSR=m_sampleRate[inputId];
    }

    if (freq >= 0) { // the user ask for a new value
        if (freq >= MIN_FREQUENCY && freq <= MAX_FREQUENCY) {
            newFreq = freq;
        }
        else {
            _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setConfiguration()");
            impl.setValueName("freq");
            throw impl;
        }
    }
    else
        newFreq = m_frequency[inputId];

    if (feed >= 0) { // the user ask for a new value
        if (feed != 0) { // BUT for the moment is it possible to use ONLY feed 0
            _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setConfiguration()");
            impl.setValueName("feed");
            throw impl;
        }
        newFeed = feed;
    }
    else
        newFeed = m_feedNumber[inputId];

    if (pol >= 0) { // the user ask for a new value
        if ((pol == 0) || (pol == 1)) { // LCP or RCP
            newPol = pol;
        }
        if (pol == 2) { // FULL STOKES
            newPol = pol;
        }
        if (pol >= 3) {
            _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setConfiguration()");
                impl.setValueName("pol");
                throw impl;
        }
        newPol = pol;
    }
    else
        newPol = m_polarization[inputId];


    if (bins>=0) { // the user ask for a new value
        if (bins <  MIN_BINS || bins >  MAX_BINS) {
            _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setConfiguration()");
            impl.setValueName("bins");
            /*if (bins != MIN_BINS)
                impl.setValue(MIN_BINS);
            if (bins != MAX_BINS)
                impl.setValue(MAX_BINS);*/
            throw impl;
        }
        newBins=bins;
    }
    else
        newBins = m_bins[inputId];

    if (m_stationSRT == true) {
        try {
            Message request = Command::setSection(inputId, newFreq, newBW, newFeed, newPol, newSR, newBins);
            Message reply = sendBackendCommand(request);
            if (reply.is_success_reply()) {
                for (int j=0;j<m_sectionsNumber;j++)
                    m_sampleRate[j]=newSR; //the given sample rate is taken also for all the others
                m_commonSampleRate=newSR;
                if (m_stokes==true) {
                    m_frequency[2*inputId]=newFreq;
                    m_frequency[2*inputId+1]=newFreq;
                    m_bandWidth[2*inputId]=newBW;
                    m_bandWidth[2*inputId+1]=newBW;
                }
                else {
                    m_frequency[inputId]=newFreq;
                    m_bandWidth[inputId]=newBW;
                }
                m_feedNumber[inputId]=newFeed;
                m_bins[inputId]=newBins;
                m_polarization[inputId]=newPol;
                IRA::CString temp;
                if (m_polarization[inputId]==Backends::BKND_LCP)
                    temp="LCP";
                else if (m_polarization[inputId]==Backends::BKND_RCP)
                    temp="RCP";
                else
                    temp="FULL_STOKES";
                ACS_LOG(LM_FULL_INFO,"CCommandLine::setConfiguration()",(LM_NOTICE,"SECTION_CONFIGURED %ld,FREQ=%lf,BW=%lf,FEED=%ld,POL=%s,SR=%lf,BINS=%ld",inputId,m_frequency[inputId],newBW,m_feedNumber[inputId],
                    (const char *)temp,newSR,m_bins[inputId]));
            }
        }
        catch (...) {
            ACS_LOG(LM_FULL_INFO,"CCommandLine::setSection()",(LM_NOTICE,"BACKEND_SKARAB_SET_SECTION ERROR"));
        }
    }
    if (m_stationMED == true) {
        try {
            Message request = Command::setSection(inputId, newFreq, newBW, newFeed, newPol, newSR, newBins);
            Message reply = sendBackendCommand(request);
            if (reply.is_success_reply()) {
                for (int j=0;j<m_sectionsNumber;j++)
                    m_sampleRate[j]=newSR; //the given sample rate is taken also for all the others
                m_commonSampleRate=newSR;
                if (m_stokes==true) {
                    m_frequency[2*inputId]=newFreq;
                    m_frequency[2*inputId+1]=newFreq;
                    m_bandWidth[2*inputId]=newBW;
                    m_bandWidth[2*inputId+1]=newBW;
                }
                else {
                    m_frequency[inputId]=newFreq;
                    m_bandWidth[inputId]=newBW;
                }
                m_feedNumber[inputId]=newFeed;
                m_bins[inputId]=newBins;
                m_polarization[inputId]=newPol;
                IRA::CString temp;
                if (m_polarization[inputId]==Backends::BKND_LCP)
                    temp="LCP";
                else if (m_polarization[inputId]==Backends::BKND_RCP)
                    temp="RCP";
                else
                    temp="FULL_STOKES";
                ACS_LOG(LM_FULL_INFO,"CCommandLine::setConfiguration()",(LM_NOTICE,"SECTION_CONFIGURED %ld,FREQ=%lf,BW=%lf,FEED=%ld,POL=%s,SR=%lf,BINS=%ld",inputId,m_frequency[inputId],newBW,m_feedNumber[inputId],
                (const char *)temp,newSR,m_bins[inputId]));
            }
        }
        catch (...) {
            ACS_LOG(LM_FULL_INFO,"CCommandLine::setSection()",(LM_NOTICE,"BACKEND_SKARAB_SET_SECTION ERROR"));
        }
    }
    //get DDC offset
    /* try {
        Message reply = sendBackendCommand(Command::getDDCoffset());
        if(reply.is_success_reply()) {
            ddcoffset=(double)reply.get_argument<double>(0);
            ACS_LOG(LM_FULL_INFO,"CCommandLine::setSection()",(LM_NOTICE,"DDC OFFSET = %f", ddcoffset));
        }
    }
    catch (...) {
        ACS_LOG(LM_FULL_INFO,"CCommandLine::setSection()",(LM_NOTICE,"BACKEND_SKARAB_GET-DDC-OFFSET ERROR"));
    }
    if (ddcoffset != 0.0) {
        loRef = m_receiversBoss->LO ();
        lo_var = loRef->get_sync (completion.out ());
        IFNumberRO=m_receiversBoss->IFs();
        IFNumber=IFNumberRO->get_sync(completion.out()); // number of output IFs of the receiver
        lo.length(IFNumber);
        for (int j=0; j<IFNumber; j++) {
            lo[j] = lo_var[j] + ddcoffset;
        }
        BYTE inBuffer[256];
        sprintf(inBuffer,"setLO=%f\n",lo[0]);
        bool ans = false;
        char *ret_val;
        IRA::CString out;
        try {
            ans = m_Scheduler->command ((const char *) inBuffer, ret_val);
            out = IRA::CString (ret_val);
        }
        catch (CORBA::SystemException & ex)
        {
            _EXCPT (ComponentErrors::CORBAProblemExImpl, impl,"CExternalClientsSocketServer::cmdToScheduler()");
            impl.setName (ex._name ());
            impl.setMinor (ex.minor ());
            impl.log (LM_ERROR);
            ret_val = "CORBA Error";
            out = IRA::CString (ret_val);
        }
        catch (...)
        {
            _EXCPT (ComponentErrors::UnexpectedExImpl, impl,"CExternalClientsSocketServer::cmdToScheduler()");
            impl.log (LM_ERROR);
            ret_val = "Unexpected Error";
            out = IRA::CString (ret_val);
        }
    } */
    }
}

void CCommandLine::getZeroTPI(DWORD *tpi) throw (ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
        ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,BackendsErrors::MalformedAnswerExImpl,
        BackendsErrors::BackendBusyExImpl)
{
    /*AUTO_TRACE("CCommandLine::getZeroTPI()");
    int res;
    WORD len;
    char sBuff[SENDBUFFERSIZE];
    char rBuff[RECBUFFERSIZE];
    long integration;
    bool busy=getIsBusy();
    long waitTime=0;
    if (busy) {
        _EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::getZeroTPI()");
        throw impl;
    }
    if (!checkConnection()) {
        _THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getZeroTPI()");
    }
    integration=(long)round(1.0/ (m_commonSampleRate*1000.0));
    //res=getConfiguration(); // refresh the m_currentSampleRate..........
    if (res>0) { // load OK
        // do nothing
    }
    _CHECK_ERRORS("CommandLine::getZeroTPI()");
    //if the requested integration is not equal to the current sample rate (given is milliseconds as a sample period)
    // or the integration time must be forced...and the backend is not busy. Then set the correct sample rate....
    if ((integration!=m_currentSampleRate) || (m_setTpiIntegration)) {
        len=CProtocol::setIntegrationTime(sBuff,integration); // get the buffer
        if ((res=sendBuffer(sBuff,len))==SUCCESS) {
            res=receiveBuffer(rBuff,RECBUFFERSIZE);
        }
        if (res>0) { // operation was ok.
            if (!CProtocol::isAck(rBuff)) {
                    _THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::getZeroTPI()");
            }
        }
        else if (res==FAIL) {
            _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
            dummy.setCode(m_Error.getErrorCode());
            dummy.setDescription((const char*)m_Error.getDescription());
            m_Error.Reset();
            _THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::getZeroTPI()");
        }
        else if (res==WOULDBLOCK) {
            _THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::getZeroTPI()");
        }
        else {
            _THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getZeroTPI()");
        }
        m_setTpiIntegration=false;
        waitTime=1000000+integration*2000; // //wait time in microseconds...we should wait 2 seconds plus twice the integration time (which is given in milliseconds).
    }
    //  connect the backend to the 50ohm
    len=CProtocol::setZeroInput(sBuff,true); // get the buffer
    if ((res=sendBuffer(sBuff,len))==SUCCESS) {
        res=receiveBuffer(rBuff,RECBUFFERSIZE);
    }
    if (res>0) { // operation was ok.
        if (!CProtocol::isAck(rBuff)) {
            _THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::getZeroTPI()");
        }
    }
    else if (res==FAIL) {
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
        dummy.setCode(m_Error.getErrorCode());
        dummy.setDescription((const char*)m_Error.getDescription());
        m_Error.Reset();
        _THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::getZeroTPI()");
    }
    else if (res==WOULDBLOCK) {
        _THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::getZeroTPI()");
    }
    else {
        _THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getZeroTPI()");
    }
    waitTime+=integration*2000;
    if (waitTime>0) IRA::CIRATools::Wait(waitTime);
    // now read the total power
    len=CProtocol::getSample(sBuff); // get the buffer
    if ((res=sendBuffer(sBuff,len))==SUCCESS) {
        res=receiveBuffer(rBuff,RECBUFFERSIZE);
    }
    if (res>0) { // operation was ok.
        if (!CProtocol::decodeData(rBuff,tpi,m_configuration->getBoardsNumber(),m_sectionsNumber,m_boards)) {
            _THROW_EXCPT(BackendsErrors::MalformedAnswerExImpl,"CCommandLine::getZeroTPI()");
        }
        for (int j=0;j<m_sectionsNumber;j++) {
            m_tpiZero[j]=(double)tpi[j]/(double)integration;
        }
    }
    else if (res==FAIL) {
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
        dummy.setCode(m_Error.getErrorCode());
        dummy.setDescription((const char*)m_Error.getDescription());
        m_Error.Reset();
        _THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::getZeroTPI()");
    }
    else if (res==WOULDBLOCK) {
        _THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::getZeroTPI()");
    }
    else {
        _THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getZeroTPI()");
    }
    // now return to the default attenuation level
    len=CProtocol::setZeroInput(sBuff,false); // get the buffer
    if ((res=sendBuffer(sBuff,len))==SUCCESS) {
        res=receiveBuffer(rBuff,RECBUFFERSIZE);
    }
    if (res>0) { // operation was ok.
        if (!CProtocol::isAck(rBuff)) {
            _THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::getZeroTPI()");
        }
    }
    else if (res==FAIL) {
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
        dummy.setCode(m_Error.getErrorCode());
        dummy.setDescription((const char*)m_Error.getDescription());
        m_Error.Reset();
        _THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::getZeroTPI()");
    }
    else if (res==WOULDBLOCK) {
        _THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::getZeroTPI()");
    }
    else {
        _THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getZeroTPI()");
    }*/
}

void CCommandLine::getTpi(ACS::doubleSeq& tpi) throw (ComponentErrors::TimeoutExImpl,
        BackendsErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,
        BackendsErrors::MalformedAnswerExImpl,BackendsErrors::BackendBusyExImpl)
{
    getSample(tpi,false);
}

void CCommandLine::getZero(ACS::doubleSeq& tpi) throw (ComponentErrors::TimeoutExImpl,
        BackendsErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,
        BackendsErrors::MalformedAnswerExImpl,BackendsErrors::BackendBusyExImpl)
{
    getSample(tpi,true);
}

void CCommandLine::getSample(ACS::doubleSeq& tpi,bool zero) throw (ComponentErrors::TimeoutExImpl,
        BackendsErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,
        BackendsErrors::MalformedAnswerExImpl,BackendsErrors::BackendBusyExImpl)
{
    AUTO_TRACE("CCommandLine::getSample()");
    try {
        Message reply = sendBackendCommand(Command::getTpi(m_TsysRange_freq,m_TsysRange_bw));
        if(reply.is_success_reply()){
            tpi.length(m_inputsNumber/*m_sectionsNumber*/);
            for (int j=0;j<m_inputsNumber/*m_sectionsNumber*/;j++) {
                tpi[j]=(double)reply.get_argument<double>(j);
                //if (zero) m_tpiZero[j]=tpi[j]/(double)m_integration; // in case of tpiZero we store it......
                if (zero)
                    m_tpiZero[j]=tpi[j]=0.0; // in case of tpiZero we store it......
            }
        }
    }
    catch (...) {
        ACS_LOG(LM_FULL_INFO,"CCommandLine::getTpi()",(LM_NOTICE,"BACKEND_SKARAB_GET-TPI ERROR"));
    }
}

void CCommandLine::getRms(ACS::doubleSeq& rms) throw (ComponentErrors::TimeoutExImpl,
        BackendsErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,
        BackendsErrors::MalformedAnswerExImpl,BackendsErrors::BackendBusyExImpl)
{
    AUTO_TRACE("CCommandLine::getRms()");
    try {
        Message reply = sendBackendCommand(Command::getRms());
        if(reply.is_success_reply()){
            rms.length(m_inputsNumber/*m_sectionsNumber*/);
            for (int j=0;j<m_inputsNumber/*m_sectionsNumber*/;j++) {
                rms[j]=(double)reply.get_argument<double>(j);
            }
        }
    }
    catch (...) {
        ACS_LOG(LM_FULL_INFO,"CCommandLine::getRms()",(LM_NOTICE,"BACKEND_SKARAB_GET-RMS ERROR"));
    }
}

void CCommandLine::setDefaultConfiguration(const IRA::CString & config) throw (ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
        ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ReplyNotValidExImpl,BackendsErrors::BackendFailExImpl)
{
    AUTO_TRACE("CCommandLine::setDefaultConfiguration()");
    if (config.Compare("SKARAB_1")==0 || config.Compare("SKARAB_2")==0 || config.Compare("SKARAB_3")==0 || config.Compare("SKARAB_4")==0 || config.Compare("SKARAB_5")==0 || config.Compare("SKARAB_7")==0
            || config.Compare("SKARAB_11")==0 || config.Compare("SKARAB_12")==0 ) {
        m_inputsNumber=m_sectionsNumber;
    }
    if (config.Compare("SKARAB_1S")==0 || config.Compare("SKARAB_2S")==0 || config.Compare("SKARAB_3S")==0 || config.Compare("SKARAB_4S")==0 || config.Compare("SKARAB_5S")==0
            || config.Compare("SKARAB_11S")==0 || config.Compare("SKARAB_12S")==0 ) {
        m_inputsNumber=m_sectionsNumber;
        m_sectionsNumber=m_sectionsNumber/2;
        m_stokes=true;
    }
}

void CCommandLine::setTargetFileName(const char *conf)
{
    m_targetFileName = (const char*)conf;
}

void CCommandLine::sendTargetFileName() throw (BackendsErrors::BackendBusyExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl)
{
    AUTO_TRACE("CCommandLine::sendTargetFileName()");
    int res;
    WORD len;
    char sBuff[SENDBUFFERSIZE];
    char rBuff[RECBUFFERSIZE];
    // I do not check for backend busy because this is a call done at the initialization and never repeated
    if (!checkConnection()) {
        _THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::sendTargetFileName()");
    }
    strcpy (sBuff,"?set-filename,");
    strcat (sBuff,(const char*)m_targetFileName);
    strcat (sBuff,"\r\n");
    len = strlen (sBuff);

    if ((res=sendBuffer(sBuff,len))==SUCCESS) {
        res=receiveBuffer(rBuff,RECBUFFERSIZE);
        //printf("set-filename = %s\n",rBuff);
    }
    if (res>0) { // operation was ok.
        //if (!CProtocol::setConfiguration(rBuff)) {
        //    _THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::setTargetFileName()");
        //}
    }
    else if (res==FAIL) {
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
        dummy.setCode(m_Error.getErrorCode());
        dummy.setDescription((const char*)m_Error.getDescription());
        m_Error.Reset();
        _THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::sendTargetFileName()");
    }
    else if (res==WOULDBLOCK) {
        _THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::sendTargetFileName()");
    }
    else {
        _THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::sendTargetFileName()");
    }
    ACS_LOG(LM_FULL_INFO,"CCommandLine::sendTargetFileName()",(LM_NOTICE,"targetFileName SENT"));
}

void CCommandLine::setup(const char *conf) throw (BackendsErrors::BackendBusyExImpl,BackendsErrors::ConfigurationErrorExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
        ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,ComponentErrors::CDBAccessExImpl,BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ReplyNotValidExImpl,BackendsErrors::BackendFailExImpl)
{
    AUTO_TRACE("CCommandLine::setup()");

    int inputs;

    /*    if (getIsBusy()) {
        _EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setup()");
        throw impl;
    }*/
    ACSErr::Completion_var completion;
    m_receiverRef = m_receiversBoss->actualSetup ();
    m_receiver = m_receiverRef->get_sync (completion.out ());
    m_recstr = IRA::CString (m_receiver);

    if (!initializeConfiguration(conf)) {
        _EXCPT(BackendsErrors::ConfigurationErrorExImpl,impl,"CCommandLine::setup()");
        throw impl;
    }
    try {
        Message request = Command::setConfiguration(string((const char*)conf));
        Message reply = sendBackendCommand(request);
        if(reply.is_success_reply()) {
            setDefaultConfiguration(conf);
            if (m_recstr.Compare("KKG")==0) {
                inputs=14;
            }
            if (m_recstr.Compare("CCB")==0) {
                inputs=2;
            }
            if (m_recstr.Compare("CCG")==0) {
                inputs=2;
            }
            m_filter=2350.0;
            for (int i=0;i<inputs;i++) {
            //for (int i=0;i<m_inputsNumber;i++) {
            //for (int i=0;i<2;i++) {
                m_totalPower->setSection(i,-1, m_filter, -1, -1, -1, -1);
                ACS_LOG(LM_FULL_INFO,"CCommandLine::setup()",(LM_NOTICE,"TOTALPOWER_FILTER_CONFIGURED %d,FILTER=%lf",i,m_filter));
            }
        }
        ACS_LOG(LM_FULL_INFO,"CCommandLine::setup()",(LM_NOTICE,"BACKEND_SKARAB_INITIALIZED, CONFIGURATION: %s",conf));
        m_SkarabInitialized = true;
    }
    catch (...) {
        ACS_LOG(LM_FULL_INFO,"CCommandLine::setup()",(LM_NOTICE,"BACKEND_SKARAB_INITIALIZATION ERROR, CONFIGURATION: %s",conf));
    }
}

void CCommandLine::checkTime() throw (BackendsErrors::ConnectionExImpl,BackendsErrors::MalformedAnswerExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl)
{
    if (!checkConnection()) {
        _THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::checkTime()");
    }
    /*Message request = Command::time();
    Message reply = sendBackendCommand(request);
    string _time = reply.get_argument<string>(0);
    if(reply.is_success_reply())
        strcpy(time, _time.c_str());
    return _time.length();*/

    /*char sBuff[SENDBUFFERSIZE];
    char rBuff[RECBUFFERSIZE];
    WORD len;
    int res;
    ///the check of th ebusy status is not done because this time chek must be performed also during sampling.
    if (!checkConnection()) {
        _THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::checkTime()");
    }
    len=CProtocol::checkBackendTime(sBuff); // get the buffer
    if ((res=sendBuffer(sBuff,len))==SUCCESS) {
        res=receiveBuffer(rBuff,RECBUFFERSIZE);
    }
    if (res>0) { // operation was ok.
        bool sync;
        if (!CProtocol::checkBackendTime(rBuff,m_configuration->getTimeTollerance(),sync)) {
            _THROW_EXCPT(BackendsErrors::MalformedAnswerExImpl,"CCommandLine::checkTime()");
        } else if (!sync) {
            _IRA_LOGFILTER_LOG(LM_WARNING,"CCommandLine::checkTime()","BACKEND_CLOCK_NOT_SYNCHRONIZED");
            setStatusField(TIME_SYNC);
        }
        else {
            clearStatusField(TIME_SYNC);
        }
    }
    else if (res==FAIL) {
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
        dummy.setCode(m_Error.getErrorCode());
        dummy.setDescription((const char*)m_Error.getDescription());
        m_Error.Reset();
        _THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::checkTime()");
    }
    else if (res==WOULDBLOCK) {
        _THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::checkTime()");
    }
    else {
        _THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::checkTime()");
    }*/
}

void CCommandLine::setTime()  throw (ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
        ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,BackendsErrors::MalformedAnswerExImpl,
        BackendsErrors::BackendBusyExImpl)
{
    AUTO_TRACE("CCommandLine::setTime()");
    //int res;
    //WORD len;
    //char sBuff[SENDBUFFERSIZE];
    //char rBuff[RECBUFFERSIZE];
/*    if (getIsBusy()) {
        _EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setTime()");
        throw impl;
    }*/
    /*len=CProtocol::setBackendTime(sBuff); // get the buffer
    if ((res=sendBuffer(sBuff,len))==SUCCESS) {
        res=receiveBuffer(rBuff,RECBUFFERSIZE);
    }
    if (res>0) { // operation was ok.
        bool sync;
        if (!CProtocol::decodeBackendTime(rBuff,sync)) {
            _THROW_EXCPT(BackendsErrors::MalformedAnswerExImpl,"CCommandLine::setTime()");
            setStatusField(TIME_SYNC);
        } else if (!sync) {
            ACS_LOG(LM_FULL_INFO,"CCommandLine::setTime()",(LM_WARNING,"CLOCK_NOT_SYNCHRONIZED"));
            setStatusField(TIME_SYNC);
        }
        else {
            clearStatusField(TIME_SYNC);
            ACS_LOG(LM_FULL_INFO,"CCommandLine::setTime()",(LM_NOTICE,"TIME_IS_SYNCHRONIZED"));
        }
    }
    else if (res==FAIL) {
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
        dummy.setCode(m_Error.getErrorCode());
        dummy.setDescription((const char*)m_Error.getDescription());
        m_Error.Reset();
        _THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::setTime()");
    }
    else if (res==WOULDBLOCK) {
        _THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::setTime()");
    }
    else {
        _THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::setTime()");
    }
    */
}

void CCommandLine::activateCalSwitching(const long& interleave) throw (BackendsErrors::BackendBusyExImpl,ComponentErrors::NotAllowedExImpl)
{
    AUTO_TRACE("CCommandLine::activateCalSwitching()");
    /*int res;
    WORD len;
    char sBuff[SENDBUFFERSIZE];
    char rBuff[RECBUFFERSIZE];*/
    if (getIsBusy()) {
        _EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setTime()");
        throw impl;
    }
    /*if (!checkConnection()) {
        _THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::setTime()");
    }*/
    Message request = Command::calOn(interleave);
    Message reply = sendBackendCommand(request);
    if(reply.is_success_reply()){
        // TBD
    }
    if (!m_calSwitchingEnabled) {
        _EXCPT(ComponentErrors::NotAllowedExImpl,impl,"CCommandLine::activateCalSwitching()");
        throw impl;
    }
    if (interleave>=0) {
        m_calPeriod=interleave;
        if (m_calPeriod>0) {
            ACS_LOG(LM_FULL_INFO,"CCommandLine::activateCalSwitching()",(LM_NOTICE,"CALIBRATION_DIODE_SWITCHING_ON"));
        }
        else {
            ACS_LOG(LM_FULL_INFO,"CCommandLine::activateCalSwitching()",(LM_NOTICE,"CALIBRATION_DIODE_SWITCHING_OFF"));
        }
    }
}

void CCommandLine::setEnabled(const ACS::longSeq& en) throw (BackendsErrors::BackendBusyExImpl, BackendsErrors::ConfigurationErrorExImpl, ComponentErrors::ValueOutofRangeExImpl)
{
    int bound;
    int i;
    long m_en[4];
//    bool kkg77 = false;
//    IRA::CString recstr;
//    ACS::ROstring_var receiverRef;
//    CORBA::String_var receiver;
//    ACSErr::Completion_var completion;

    /* if (getIsBusy()) {
        _EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setEnabled()");
        throw impl;
    }*/
    if (m_SkarabInitialized == true) {
        if ((long)en.length()!=2/*m_sectionsNumber*/) {
        /*errore;*/
        }
        /*else {
            bound=en.length();
        }
        for (int i=0;i<bound;i++) {
            if (en[i]>0) m_enabled[i]=true;
            else if (en[i]==0) m_enabled[i]=false;
        }*/
//        receiverRef = m_receiversBoss->actualSetup ();
//        receiver = receiverRef->get_sync (completion.out ());
//        recstr = IRA::CString (receiver);
        if ((m_recstr.Compare("KKG")==0) && (en[0]==7 && en[1]==7)) {
            m_en[0]=m_en[2]=7;
            m_kkg77=true;
        }
        if (m_kkg77 == false) {
            if (m_recstr.Compare("KKG")==0) {
                if ((en[0] < 0) || (en[0] > 6)) {
                    _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setEnabled()");
                    impl.setValueName("feed");
                    throw impl;
                }
                if ((en[1] < 0) || (en[1] > 6)) {
                    _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setEnabled()");
                    impl.setValueName("feed");
                    throw impl;
                }
            }
            if ((m_recstr.Compare("CCB")==0) || (m_recstr.Compare("CCG")==0)) {
                if (en[0] != 0) {
                    _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setEnabled()");
                    impl.setValueName("feed");
                    throw impl;
                }
                if (en[1] != 0) {
                    _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setEnabled()");
                    impl.setValueName("feed");
                    throw impl;
                }
            }
            if (en[0] == en[1]) { // central feed
                m_inputsNumber = 2;
                if (m_stokes == false)
                    m_sectionsNumber = 2;
                if (m_stokes == true)
                    m_sectionsNumber = 1;
                m_beams = 1; // TBC !!!
            }
            else { // nodding
                m_inputsNumber = 4;
                if (m_stokes == false)
                    m_sectionsNumber = 4;
                if (m_stokes == true)
                    m_sectionsNumber = 2;
                m_beams = 2; // TBC !!!
            }
            m_en[0] = en[0];
            m_en[1] = en[0];
            m_en[2] = en[1];
            m_en[3] = en[1];
            for (i=0;i<m_inputsNumber;i++) {
                m_feedNumber[i] = m_en[i];
            }
        }
        Message request = Command::setEnable(m_en[0],m_en[2]);
        Message reply = sendBackendCommand(request);
        if (reply.is_success_reply()) {
            // TBD
            ACS_LOG(LM_FULL_INFO,"CCommandLine::setEnabled()",(LM_NOTICE,"FEEDS enabled"));
        }
    }
    else {
        ACS_LOG(LM_FULL_INFO,"CCommandLine::setEnabled()",(LM_NOTICE,"BCK SKARAB NOT INITIALIZED"));
        _EXCPT(BackendsErrors::ConfigurationErrorExImpl,impl,"CCommandLine::setEnabled()");
        throw impl;
    }
}

void CCommandLine::setIntegration(const long& integration)  throw (BackendsErrors::BackendBusyExImpl, ComponentErrors::ValueOutofRangeExImpl)
{
    AUTO_TRACE("CCommandLine::setIntegration()");
    /*if (getIsBusy()) {
        _EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setIntegration()");
        throw impl;
    }*/
        m_integration = integration;
    if (m_integration>=MIN_INTEGRATION && m_integration <= MAX_INTEGRATION) {
        Message request = Command::setIntegration(integration);
        Message reply = sendBackendCommand(request);
        if (reply.is_success_reply()) {
            // TBD
            ACS_LOG(LM_FULL_INFO,"CCommandLine::setIntegration()",(LM_NOTICE,"INTEGRATION is now %ld (millisec)",m_integration));
        }
    }
    else {
        _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setIntegration()");
        impl.setValueName("integration");
        throw impl;
    }
}

void CCommandLine::setShift(const long& shift)  throw (BackendsErrors::BackendBusyExImpl)
{
    AUTO_TRACE("CCommandLine::setShift()");
    Message request = Command::setShift(shift);
    Message reply = sendBackendCommand(request);
    if (reply.is_success_reply()) {
        // TBD
        ACS_LOG(LM_FULL_INFO,"CCommandLine::setShift()",(LM_NOTICE,"SHIFT is now %ld",shift));
    }
}

void CCommandLine::getAttenuation(ACS::doubleSeq& att) throw (ComponentErrors::SocketErrorExImpl,
        ComponentErrors::TimeoutExImpl,BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ConnectionExImpl)
{
    AUTO_TRACE("CCommandLine::getAttenuation()");
    att.length(m_inputsNumber);
    for (int i=0;i<m_inputsNumber;i++) {
        att[i]=m_attenuation[i];
    }
}

void CCommandLine::getAttenuationAttr(ACS::doubleSeq& att) throw (ComponentErrors::SocketErrorExImpl,
        ComponentErrors::TimeoutExImpl,BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ConnectionExImpl)
{
    AUTO_TRACE("CCommandLine::getAttenuation()");
    att.length(m_sectionsNumber);
    for (int i=0;i<m_sectionsNumber;i++) {
        att[i]=m_attenuation[i];
    }
}

void CCommandLine::getFrequency(ACS::doubleSeq& freq) const
{
    freq.length(m_inputsNumber);
    for (int i=0;i<m_inputsNumber;i++) {
        freq[i]=m_frequency[i];
    }
}

void CCommandLine::getFrequencyAttr(ACS::doubleSeq& freq) const
{
    freq.length(m_sectionsNumber);
    for (int i=0;i<m_sectionsNumber;i++) {
        freq[i]=m_frequency[i];
    }
}

void CCommandLine::getBackendStatus(DWORD& status)
{
    AUTO_TRACE("CCommandLine::getBackendStatus()");
    try {
        Message request = Command::status();
        Message reply = sendBackendCommand(request);
        if(reply.is_success_reply()) {
            string _status = reply.get_argument<string>(2);
            //strcpy(status, _status.c_str());
            status=atof(_status.c_str());
            if (status == 0)
                clearStatusField(BUSY);
            if (status == 1)
                setStatusField(BUSY);

        }
    }
    catch (...) {
         ACS_LOG(LM_FULL_INFO,"CCommandLine::getBackendStatus()",(LM_NOTICE,"BACKEND_SKARAB_GET_STATUS_ERROR"));
    }
    //return _status.length();
    return status;
}

void CCommandLine::getSampleRate(ACS::doubleSeq& sr) const
{
    sr.length(m_sectionsNumber);
    for (int i=0;i<m_sectionsNumber;i++) {
        sr[i]=m_sampleRate[i];
    }
}

void CCommandLine::getTsys(ACS::doubleSeq& tsys) const
{
    /*tsys.length(m_sectionsNumber);
    for (int i=0;i<m_sectionsNumber;i++) {
        tsys[i]=m_tsys[i];
    }*/
    tsys.length(m_inputsNumber);
    for (int i=0;i<m_inputsNumber;i++) {
        tsys[i]=m_tsys[i];
    }
}

void CCommandLine::getKCRatio(ACS::doubleSeq& ratio) const
{
    ratio.length(m_sectionsNumber);
    for (int i=0;i<m_sectionsNumber;i++) {
        ratio[i]=m_KCratio[i];
    }
}

void CCommandLine::getKelvinCountsRatio(ACS::doubleSeq& kcr) const
{
    kcr.length(m_inputsNumber);
    for (int i=0;i<m_inputsNumber;i++) {
        kcr[i]=m_KCratio[i];
    }
}

void CCommandLine::getBins(ACS::longSeq& bins) const
{
    bins.length(m_sectionsNumber);
    for (int i=0;i<m_sectionsNumber;i++) {
        bins[i]=m_bins[i];
    }
}

void CCommandLine::getPolarization(ACS::longSeq& pol) const
{
    pol.length(m_sectionsNumber);
    for (int i=0;i<m_sectionsNumber;i++) {
        if (m_stokes==true)
            pol[i]=2;
        else
            pol[i]=(long)m_polarization[i];
    }
}

void CCommandLine::getFeed(ACS::longSeq& feed) const
{
    feed.length(m_inputsNumber);
    for (int i=0;i<m_inputsNumber;i++) {
        feed[i]=m_feedNumber[i];
    }
}

void CCommandLine::getFeedAttr(ACS::longSeq& feed) const
{
    feed.length(m_sectionsNumber);
    for (int i=0;i<m_sectionsNumber;i++) {
        if (m_stokes == true)
            feed[i]=m_feedNumber[2*i];
        else
            feed[i]=m_feedNumber[i];
    }
}

void CCommandLine::getIFs(ACS::longSeq& ifs) const
{
    ifs.length(m_inputsNumber);
    for (int i=0;i<m_inputsNumber;i++) {
        ifs[i]=m_ifNumber[i];
    }
}

void CCommandLine::getInputSection(ACS::longSeq& inpSection) const
{
    inpSection.length(m_inputsNumber);
    for (int i=0;i<m_inputsNumber;i++) {
        inpSection[i]=m_inputSection[i];
    }
}

void CCommandLine::getInputSectionAttr(ACS::longSeq& inpSection) const
{
    long index=0;

    if (m_stokes==true)
        index = m_sectionsNumber;
    else
        index = m_inputsNumber;

    inpSection.length(index);
    for (int i=0;i<index;i++) {
        inpSection[i]=m_inputSection[i];
    }
}

void CCommandLine::getBandWidth(ACS::doubleSeq& bw) throw (ComponentErrors::SocketErrorExImpl,
        ComponentErrors::TimeoutExImpl,BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ConnectionExImpl)
{
    AUTO_TRACE("CCommandLine::getBandWidth()");
    bw.length(m_inputsNumber);
    for (int i=0;i<m_inputsNumber;i++) {
        bw[i]=m_bandWidth[i];
    }
}

void CCommandLine::getBandWidthAttr(ACS::doubleSeq& bw) const
{
    AUTO_TRACE("CCommandLine::getBandWidthAttr()");

    bw.length(m_sectionsNumber);
    for (int i=0;i<m_sectionsNumber;i++) {
        bw[i]=m_bandWidth[i];
    }
}
void CCommandLine::getTime(ACS::Time& tt) throw (ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,
        BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ConnectionExImpl)
{
    AUTO_TRACE("CCommandLine::getTime()");
    TIMEDIFFERENCE bkndTime;
    TIMEVALUE tm;
    long long seconds;
    long second,minute,hour,day;
    try {
        Message request = Command::time();
        Message reply = sendBackendCommand(request);
        if(reply.is_success_reply()) {
            seconds=reply.get_argument<long long>(0);
            day=seconds/86400;
            seconds%=86400;
            hour=seconds/3600;
            seconds%=3600;
            minute=seconds/60;
            second=seconds % 60;
            bkndTime.day(day);
            bkndTime.hour(hour);
            bkndTime.minute(minute);
            bkndTime.second(second);
            tm.reset();
            tm.normalize(true);
            tm.year(1970);
            tm.month(1);
            tm.day(1);
            tm+=bkndTime.value();
            tt=tm.value().value;
        }
    }
    catch (...) {
         ACS_LOG(LM_FULL_INFO,"CCommandLine::getTime()",(LM_NOTICE,"BACKEND_SKARAB_GET_TIME_ERROR"));
    }
}

void CCommandLine::fillMainHeader(Backends::TMainHeader& bkd)
{
    bkd.sections=m_sectionsNumber;
    bkd.beams=m_beams;
    //resultingSampleRate(m_integration,m_commonSampleRate,intTime);
    //bkd.integration=intTime;
    bkd.integration=m_integration;
    bkd.sampleSize=m_sampleSize;
    bkd.noData=true;
}

void CCommandLine::fillChannelHeader(Backends::TSectionHeader *chHr,const long& size)
{
    long index=0;
    for (int i=0;i<m_sectionsNumber;i++) {
    //for (int i=0;i<m_inputsNumber;i++) {
                //printf("i = %d\n", i);
        //if (m_enabled[i]) {
        //    if (index<size) {
                chHr[index].id=i;
                chHr[index].bins=m_bins[i];
                chHr[index].bandWidth=m_bandWidth[i];
                //m_frequency[i]=m_frequency[i]-m_bandWidth[i]/2.; // TBC !!
                chHr[index].frequency=m_frequency[i];
                chHr[index].attenuation[0]=m_attenuation[i];
                chHr[index].attenuation[1]=m_attenuation[i];
                chHr[index].sampleRate=m_sampleRate[i];
                if (m_stokes==true) {
                    chHr[index].inputs=2;
                    chHr[index].feed=m_feedNumber[2*i];
                    chHr[index].polarization=m_polarization[2*i];
                    chHr[index].IF[0]=0;
                    chHr[index].IF[1]=1;
                }
                else {
                    chHr[index].inputs=1;
                    chHr[index].feed=m_feedNumber[i];
                    chHr[index].polarization=m_polarization[i];
                    chHr[index].IF[0]=m_ifNumber[i];
                }
                //printf("feed = %d\n", chHr[index].feed);
                //printf("pol = %d\n", m_polarization[i]);
                index++;
        //    }
        //}
    }
}

void CCommandLine::saveTsys(const ACS::doubleSeq& tsys,const ACS::doubleSeq& ratio)
{
    /*if (tsys.length()==(unsigned)m_sectionsNumber) {
        for (int i=0;i<m_sectionsNumber;i++) {
            m_tsys[i]=tsys[i];
        }
    }
    if (ratio.length()==(unsigned)m_sectionsNumber) {
        for (int i=0;i<m_sectionsNumber;i++) {
            m_KCratio[i]=ratio[i];
        }
        ACS_LOG(LM_FULL_INFO,"CCommandLine::saveTsys()",(LM_NOTICE,"KELVIN_COUNTS_CONVERSION_FACTOR_SET"));
    }*/
    if (tsys.length()==(unsigned)m_inputsNumber) {
        for (int i=0;i<m_inputsNumber;i++) {
            m_tsys[i]=tsys[i];
        }
        ACS_LOG(LM_FULL_INFO,"CCommandLine::saveTsys()",(LM_NOTICE,"TSYS_SET"));
    }
    if (ratio.length()==(unsigned)m_inputsNumber) {
        for (int i=0;i<m_inputsNumber;i++) {
            m_KCratio[i]=ratio[i];
        }
        ACS_LOG(LM_FULL_INFO,"CCommandLine::saveTsys()",(LM_NOTICE,"KELVIN_COUNTS_CONVERSION_FACTOR_SET"));
    }
}

bool CCommandLine::resultingSampleRate(const long& integration,const double& sr,long& result)
{
    bool res=false;
    long accumulations;
    double temp;
    temp=(double)integration*1000.0*sr;
    accumulations=(long)round(temp);
    if (accumulations<1) accumulations=1;
    if (((temp-accumulations)>0.0) || ((temp-accumulations)<0.0)) {
        res=true;
    }
    result=(long)round((accumulations/(sr*1000)));
    return res;
}

// Protected Methods

void CCommandLine::onConnect(int ErrorCode)
{
    CError Tmp;
    if (getStatus()==CNTDING) { // if the line is connecting.......
        if (ErrorCode==0) {
            if (EventSelect(Tmp,E_CONNECT,false)==SUCCESS) {
                setStatus(CNTD);
                CSocket::setStatus(IRA::CSocket::READY); // force the socket status to be ready.....
                try {
                    //stopDataAcquisitionForced(); // ask the backend to exit the data transfering mode......
                }
                catch (ACSErr::ACSbaseExImpl& ex) {

                }
                clearStatusField(BUSY);
                m_reiniting=true; // ask to the datasocket to accept connection again
                ACS_LOG(LM_FULL_INFO,"CCommandLine::onConnect()",(LM_NOTICE,"SOCKET_RECONNECTED"));
            }
        }
        else {
            setStatus(NOTCNTD);
            ACS_DEBUG_PARAM("CCommandLine::onConnect()","Reconnection failed, exit code is %d",ErrorCode);
        }
    }
}

void CCommandLine::onTimeout(WORD EventMask)
{
    if ((EventMask&E_CONNECT)==E_CONNECT) {
        ACS_DEBUG("CCommandLine::onTimeout()","Reconnection timed-out, keep trying....");
    }
}

// private methods

IRA::CSocket::OperationResult CCommandLine::sendBuffer(char *Msg,WORD Len)
{
    int NWrite;
    int BytesSent;
    BytesSent=0;
    while (BytesSent<Len) {
        if ((NWrite=Send(m_Error,(const void *)(Msg+BytesSent),Len-BytesSent))<0) {
            if (NWrite==WOULDBLOCK) {
                setStatus(NOTCNTD);
                _IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommandLine::sendBuffer()","SOCKET_DISCONNECTED - remote side shutdown");
                return WOULDBLOCK;
            }
            else {
                setStatus(NOTCNTD);
                CString app;
                app.Format("SOCKET_DISCONNECTED - %s",(const char *)m_Error.getFullDescription());
                _IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommandLine::sendBuffer()",(const char*)app);
                return FAIL;
            }
        }
        else { // success
            BytesSent+=NWrite;
        }
    }
    if (BytesSent==Len) {
        return SUCCESS;
   }
    else {
        _SET_ERROR(m_Error,CError::SocketType,CError::SendError,"CCommandLine::SendBuffer()");
        return FAIL;
    }
}

int CCommandLine::receiveBuffer(char *Msg,WORD Len)
{
    char inCh;
    int res;
    int nRead=0;
    TIMEVALUE Now;
    TIMEVALUE Start;
    CIRATools::getTime(Start);
    while(true) {
        res=Receive(m_Error,&inCh,1);
        if (res==WOULDBLOCK) {
            CIRATools::getTime(Now);
            if (CIRATools::timeDifference(Start,Now)>m_configuration->getCommandLineTimeout()) {
                m_bTimedout=true;
                return WOULDBLOCK;
            }
            else {
                CIRATools::Wait(0,20000);
                continue;
            }
        }
        else if (res==FAIL) {
            setStatus(NOTCNTD);
            CString app;
            app.Format("SOCKET_DISCONNECTED - %s",(const char *)m_Error.getFullDescription());
            _IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommandLine::receiveBuffer()",(const char*)app);
            return res;
        }
        else if (res==0) {
            setStatus(NOTCNTD);
            _IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommandLine::receiveBuffer()","SOCKET_DISCONNECTED - remote side shutdown");
            return res;
        }
        else {
            if (inCh!=PROT_TERMINATOR_CH) {
                Msg[nRead]=inCh;
                nRead++;
            }
            else {
                Msg[nRead]=0;
                return nRead;
            }
        }
    }
}

int CCommandLine::sendCommand(char *inBuff,const WORD& inLen,char *outBuff)
{
    OperationResult Res;
    if ((Res=sendBuffer(inBuff,inLen))==SUCCESS) {
        int bytes=receiveBuffer(outBuff,RECBUFFERSIZE);
        return bytes;
    }
    else {  // send fails....m_Error already set by sendBuffer
        return Res;
    }
}

Message
CCommandLine::sendBackendCommand(Message request)
{
    if (!checkConnection()) {
        _THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::sendBackendCommand()");
    }
    char sBuff[SENDBUFFERSIZE];
    char rBuff[RECBUFFERSIZE];
        strcpy (sBuff,request.toString(true).c_str());
    int len = strlen (sBuff);
        int res = 0;
    if ((res=sendBuffer(sBuff,len))==SUCCESS) {
        res=receiveBuffer(rBuff,RECBUFFERSIZE);
    }
    if (res>0) { // operation was ok.
            clearStatusField(CCommandLine::BUSY);
            setStatusField(SUSPEND);
    }
    else if (res==FAIL) {
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
        dummy.setCode(m_Error.getErrorCode());
        dummy.setDescription((const char*)m_Error.getDescription());
        m_Error.Reset();
        _THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::sendBackendCommand()");
    }
    else if (res==WOULDBLOCK) {
        _THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::sendBackendCommand()");
    }
    else {
        _THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::sendBackendCommand()");
    }
        Message reply(string(rBuff, res), true);
        try {
            reply.validate();
        }
    catch (BackendProtocolError& bpe) {
        //TODO: somehow the reply is not valid
            //we cannot proceed we need to throw an exception
        _THROW_EXCPT(BackendsErrors::MalformedAnswerExImpl,"sendBackendCommand()");
        }
        if(!(reply.is_valid_reply_for(request)))
        {
            //TODO: this answer is not valid for this request, throw exception
        _THROW_EXCPT(BackendsErrors::ReplyNotValidExImpl,"sendBackendCommand()");
        }
        if(!(reply.is_success_reply()))
        {
            //TODO: the reply has a FAIL o INVALID code, this must be
            //managed
        _EXCPT(BackendsErrors::BackendFailExImpl,impl,"sendBackendCommand()");
        //impl.setReason("transfer job cannot be resumed in present configuration");
        impl.setReason(reply.get_argument<string>(0).c_str());
        throw impl;
        }
    return reply;
}

//int CCommandLine::getConfiguration(char* configuration)
char* CCommandLine::getConfiguration()
{
    Message request = Command::getConfiguration();
    try {
        Message reply = sendBackendCommand(request);
        if(reply.is_success_reply())
        {
            return CORBA::string_dup(reply.get_argument<string>(0).c_str());
        }
    }
    catch (...) {
    }

    return CORBA::string_dup("");
}

char* CCommandLine::getCommProtVersion()
{
    Message request = Command::version();
    try
    {
        Message reply = sendBackendCommand(request);
        if(reply.is_success_reply())
        {
            return CORBA::string_dup(reply.get_argument<string>(0).c_str());
        }
    }
    catch(...) {
    }

    return CORBA::string_dup("");
}

bool CCommandLine::checkConnection()
{
    CError Tmp;
    char sBuffer[RECBUFFERSIZE];
    int rBytes;
    if (m_bTimedout) { // if a previous time out happend....try if the buffer has still to be received
        rBytes=receiveBuffer(sBuffer,RECBUFFERSIZE);
        if (rBytes==WOULDBLOCK) { // if the operation would have block again.....declare the disconnection
            setStatus(NOTCNTD);    // another timeout! something has happend
            _IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommandLine::checkConnection()","SOCKET_DISCONNECTED - timeout expired");
        }
        else if (rBytes==FAIL) {
            // Nothing to do, this error will be handled below....
        }
        else if (rBytes==0) {
            // Nothing to do, this error will be handled below....
        }
        else {
            m_bTimedout=false; // timeout recovered
        }
    }
    if (getStatus()==CNTD)  {
        return true;
    }
    else if (getStatus()==CNTDING) {
        return false;
    }
    else {  // socket is not connected....
        // try to close the socket, if it is already closed : never known....
        Close(Tmp);
        Tmp.Reset();
        m_bTimedout=false;
        // this will create the socket in blocking mode.....
        if (Create(Tmp,STREAM)==SUCCESS) {
            // Put the socket in non-blocking mode, registers the onconnect event....and allow some seconds to complete!
            if (EventSelect(Tmp,E_CONNECT,true,m_configuration->getConnectTimeout())==SUCCESS) {
                OperationResult Res;
                ACS_DEBUG("CCommandLine::checkConnection()","Trying to reconnect");
                Res=Connect(Tmp,m_configuration->getAddress(),m_configuration->getPort());
                if (Res==WOULDBLOCK) {
                    setStatus(CNTDING);
                }
                else if (Res==SUCCESS) {
                    setStatus(CNTD);
                    try {
                        //stopDataAcquisitionForced(); // ask the backend to exit the data transfering mode......
                    }
                    catch (ACSErr::ACSbaseExImpl& ex) {

                    }
                    clearStatusField(BUSY);
                    m_reiniting=true;
                    ACS_LOG(LM_FULL_INFO,"CCommandLine::checkConnection()",(LM_NOTICE,"SOCKET_RECONNECTED")); //we do not want to filter this info so no logfilter.....
                    return true;
                }
            }
        }
        return false;
    }
}

bool CCommandLine::initializeConfiguration(const IRA::CString & config) throw (ComponentErrors::CDBAccessExImpl)
{
    int i;
    CConfiguration::TBackendSetup setup;
    if (m_configuration->getSetupFromID(config,setup)) { // throw (ComponentErrors::CDBAccessExImpl)
        m_sectionsNumber=setup.sections;
        for (WORD k=0;k<MAX_BOARDS_NUMBER;k++) {
            //m_defaultInput[k]=setup.inputPort[k];
            m_sections[k]=-1;
        }
        m_defaultInputSize=setup.inputPorts; // this should be 1 or the number of installed boards
        m_beams=setup.beams;
        m_calSwitchingEnabled=setup.calSwitchEnabled;
        for (i=0;i<m_sectionsNumber;i++) {
            m_boards[i]=setup.section_boards[i];
            m_sections[m_boards[i]]=i;
            //m_input[i]=m_defaultInput[m_boards[i]];
            m_polarization[i]=setup.polarizations[i];
            //printf ("m_polarization = %d\n", m_polarization[i]);
            m_ifNumber[i]=setup.ifs[i];
            m_feedNumber[i]=setup.feed[i];
            m_inputSection[i]=i; // input 0 belongs to section 0 and so on.....
            m_attenuation[i]=setup.attenuation;
            m_bandWidth[i]=setup.bandWidth;
            m_bins[i]=setup.bins;
        }
    }
    else {
        return false;
    }

    // Common configurations.......
    m_integration=DEFAULT_INTEGRATION;  // integration if by default zero...that means the 1/samplerate is the real integration time
    m_currentSampleRate=m_integration;  // this is given in milliseconds as sample period
    m_sampleSize=SAMPLESIZE;
    m_commonSampleRate=DEFAULT_SAMPLE_RATE;
    m_calPeriod=DEFAULT_DIODE_SWITCH_PERIOD;
    for (i=0;i<m_sectionsNumber;i++) {
        //m_sampleRate[i]=DEFAULT_SAMPLE_RATE;
        m_sampleRate[i]=2*m_bandWidth[i];
        if (m_bandWidth[i] == 1400.0)
            m_frequency[i] = 0.0;
        else
            m_frequency[i]=STARTFREQUENCY-m_bandWidth[i]/2.;
        //m_bins[i]=BINSNUMBER;
        m_enabled[i]=true;
        m_tsys[i]=0.0;
        m_KCratio[i]=1.0;
        m_tpiZero[i]=0.0;
    }
    return true;
}

void CCommandLine::setStatus(TLineStatus sta)
{
    m_status=sta;
    if (m_status!=CNTD) {
        setStatusField(CMDLINERROR);
    }
    else {
        clearStatusField(CMDLINERROR);
    }
}
