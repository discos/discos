#include "SRTQBandReceiverImpl.h"
#include "DevIOBandWidth.h"
#include "DevIOInitialFrequency.h"
#include "DevIOLocalOscillator.h"
#include "DevIOPolarization.h"
#include "DevIOMode.h"
#include "DevIOVacuum.h"
#include "DevIOCryoTemperatureCoolHead.h"
#include "DevIOCryoTemperatureCoolHeadWindow.h"
#include "DevIOCryoTemperatureLNA.h"
#include "DevIOCryoTemperatureLNAWindow.h"
#include "DevIOEnvTemperature.h"
#include "DevIOLNAControls.h"
#include "DevIOStatus.h"
#include "DevIOComponentStatus.h"
#include <LogFilter.h>

#include <iostream>

_IRA_LOGFILTER_IMPORT;

SRTQBandReceiverImpl::SRTQBandReceiverImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) :
    CharacteristicComponentImpl(CompName,containerServices),
    m_plocalOscillator(this),
    m_pfeeds(this),
    m_pIFs(this),
    m_pinitialFrequency(this),
    m_pbandWidth(this),
    m_ppolarization(this),
    m_pstatus(this),
    m_pvacuum(this),
    m_pvdL1(this),
    m_pvdR1(this),
    m_pidL1(this),
    m_pidR1(this),
    m_pvgL1(this),
    m_pvgR1(this),
    m_pcryoTemperatureCoolHead(this),
    m_pcryoTemperatureCoolHeadWindow(this),
    m_pcryoTemperatureLNA(this),
    m_pcryoTemperatureLNAWindow(this),
    m_penvironmentTemperature(this),
    m_pmode(this),
    m_preceiverStatus(this),
    m_preceiverName(this)
{   
    AUTO_TRACE("SRTQBandReceiverImpl::SRTQBandReceiverImpl()");
}


SRTQBandReceiverImpl::~SRTQBandReceiverImpl()
{
    AUTO_TRACE("SRTQBandReceiverImpl::~SRTQBandReceiverImpl()");
}


void SRTQBandReceiverImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
    AUTO_TRACE("SRTQBandReceiverImpl::initialize()");
    ACS_LOG(LM_FULL_INFO, "SRTQBandReceiverImpl::initialize()", (LM_INFO, "SRTQBandReceiverImpl::COMPSTATE_INITIALIZING"));
    m_core.initialize(getContainerServices());
    m_monitor = NULL;
    ACS_LOG(LM_FULL_INFO, "SRTQBandReceiverImpl::initialize()", (LM_INFO, "COMPSTATE_INITIALIZED"));
}


void SRTQBandReceiverImpl::execute() throw (ACSErr::ACSbaseExImpl, ComponentErrors::MemoryAllocationExImpl)
{
    AUTO_TRACE("SRTQBandReceiverImpl::execute()");
    ACS::Time timestamp;
    const CConfiguration *config = m_core.execute(); 

    ACS_LOG(LM_FULL_INFO, "SRTQBandReceiverImpl::execute()",(LM_INFO, "ACTIVATING_LOG_REPETITION_FILTER"));
    _IRA_LOGFILTER_ACTIVATE(config->getRepetitionCacheTime(), config->getRepetitionExpireTime());

    try {
        m_preceiverName = new ROstring(getContainerServices()->getName() + ":receiverName", getComponent());
        m_pIFs = new baci::ROlong(getContainerServices()->getName() + ":IFs", getComponent());
        m_pfeeds = new baci::ROlong(getContainerServices()->getName() + ":feeds", getComponent());
        m_pvacuum = new baci::ROdouble(getContainerServices()->getName() + ":vacuum", getComponent(), new DevIOVacuum(&m_core), true);
        m_plocalOscillator = new baci::ROdoubleSeq(getContainerServices()->getName() + ":LO", getComponent(), \
                new DevIOLocalOscillator(&m_core),true);
        m_ppolarization = new baci::ROlongSeq(getContainerServices()->getName() + ":polarization", getComponent(), \
                new DevIOPolarization(&m_core), true);
        m_pmode = new baci::ROstring(getContainerServices()->getName() + ":mode", getComponent(), new DevIOMode(&m_core), true);
        m_pinitialFrequency = new baci::ROdoubleSeq(getContainerServices()->getName() + ":initialFrequency", getComponent(), \
                new DevIOInitialFrequency(&m_core), true);
        m_pbandWidth = new baci::ROdoubleSeq(getContainerServices()->getName() + ":bandWidth", getComponent(), \
                new DevIOBandWidth(&m_core), true);

        // Drain Voltage
        m_pvdL1=new baci::ROdoubleSeq(getContainerServices()->getName() + ":vdL1", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::DRAIN_VOLTAGE, 0, 1), true);
        m_pvdR1=new baci::ROdoubleSeq(getContainerServices()->getName() + ":vdR1", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::DRAIN_VOLTAGE, 1, 1), true);

        // Drain Current
        m_pidL1=new baci::ROdoubleSeq(getContainerServices()->getName() + ":idL1", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::DRAIN_CURRENT, 0, 1), true);
        m_pidR1=new baci::ROdoubleSeq(getContainerServices()->getName() + ":idR1", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::DRAIN_CURRENT, 1, 1), true);

        // Gate Voltage
        m_pvgL1=new baci::ROdoubleSeq(getContainerServices()->getName() + ":vgL1", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::GATE_VOLTAGE, 0, 1), true);
        m_pvgR1=new baci::ROdoubleSeq(getContainerServices()->getName() + ":vgR1", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::GATE_VOLTAGE, 1, 1), true);

        m_pcryoTemperatureCoolHead = new baci::ROdouble(getContainerServices()->getName() + ":cryoTemperatureCoolHead", \
                getComponent(), new DevIOCryoTemperatureCoolHead(&m_core),true);
        m_pcryoTemperatureCoolHeadWindow = new baci::ROdouble(getContainerServices()->getName() + ":cryoTemperatureCoolHeadWindow", \
                getComponent(), new DevIOCryoTemperatureCoolHeadWin(&m_core), true);
        m_pcryoTemperatureLNA = new baci::ROdouble(getContainerServices()->getName() + ":cryoTemperatureLNA", getComponent(), \
                new DevIOCryoTemperatureLNA(&m_core), true);
        m_pcryoTemperatureLNAWindow = new baci::ROdouble(getContainerServices()->getName() + ":cryoTemperatureLNAWindow", \
                getComponent(), new DevIOCryoTemperatureLNAWin(&m_core), true);
        m_penvironmentTemperature = new baci::ROdouble(getContainerServices()->getName() + ":environmentTemperature", \
                getComponent(), new DevIOEnvTemperature(&m_core), true);
        m_pstatus = new baci::ROpattern(getContainerServices()->getName()+":status",getComponent(), \
                new DevIOStatus(&m_core), true);
        m_preceiverStatus = new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus), POA_Management::ROTSystemStatus>
                 (getContainerServices()->getName() + ":receiverStatus", getComponent(), new DevIOComponentStatus(&m_core), true);
    }
    catch (std::bad_alloc& ex) {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "SRTQBandReceiverImpl::initialize()");
        throw dummy;
    }

    // Write some fixed values
    m_preceiverName->getDevIO()->write(getComponent()->getName(), timestamp);
    m_pfeeds->getDevIO()->write(m_core.getFeeds(), timestamp);
    m_pIFs->getDevIO()->write(m_core.getIFs(), timestamp);
    m_core.setVacuumDefault(m_pvacuum->default_value());

    SRTQBandCore *temp = &m_core;
    try {
         m_monitor = getContainerServices()->getThreadManager()->create<CMonitorThread, SRTQBandCore*> (
                 "WHATCHDOQBANDMF", temp, config->getWarchDogResponseTime(), config->getWatchDogSleepTime());
    }
    catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
        _ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl, _dummy, ex, "SRTQBandReceiverImpl::execute()");
        throw _dummy;
    }
    catch (...) {
        _THROW_EXCPT(ComponentErrors::UnexpectedExImpl, "SRTQBandReceiverImpl::execute()");
    }
    m_monitor->setLNASamplingTime(config->getLNASamplingTime());
    m_monitor->resume();
    ACS_LOG(LM_FULL_INFO, "SRTQBandReceiverImpl::execute()", (LM_INFO, "WATCH_DOG_SPAWNED"));
    try {
        startPropertiesMonitoring();
    }
    catch (acsthreadErrType::CanNotStartThreadExImpl& E) {
        _ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl, __dummy, E, "SRTQBandReceiverImpl::execute()");
        throw __dummy;
    }
    catch (ACSErrTypeCommon::NullPointerExImpl& E) {
        _ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl, __dummy, E, "SRTQBandReceiverImpl::execute()");
        throw __dummy;      
    }
    ACS_LOG(LM_FULL_INFO, "SRTQBandReceiverImpl::execute()", (LM_INFO, "COMPSTATE_OPERATIONAL"));
}


void SRTQBandReceiverImpl::cleanUp()
{
    AUTO_TRACE("SRTQBandReceiverImpl::cleanUp()");
    stopPropertiesMonitoring();
    if (m_monitor != NULL) {
        m_monitor->suspend();
        getContainerServices()->getThreadManager()->destroy(m_monitor);
        m_monitor = NULL;
    }
    m_core.cleanup();
    _IRA_LOGFILTER_FLUSH;
    _IRA_LOGFILTER_DESTROY;
    CharacteristicComponentImpl::cleanUp(); 
}


void SRTQBandReceiverImpl::aboutToAbort()
{
    AUTO_TRACE("SRTQBandReceiverImpl::aboutToAbort()");
    stopPropertiesMonitoring();
    if (m_monitor != NULL) {
        getContainerServices()->getThreadManager()->destroy(m_monitor);
    }
    m_core.cleanup();
    CharacteristicComponentImpl::aboutToAbort(); 
}


void SRTQBandReceiverImpl::activate(const char * setup_mode) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx, ReceiversErrors::ReceiversErrorsEx)
{
    try {
        m_core.activate();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
    catch (ReceiversErrors::ReceiversErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getReceiversErrorsEx();
    }
    catch (...) {
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "SRTQBandReceiverImpl::activate()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}

void SRTQBandReceiverImpl::deactivate() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
    try {
        m_core.deactivate();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
    catch (ReceiversErrors::ReceiversErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getReceiversErrorsEx();
    }
    catch (...) {
        _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRTQBandReceiverImpl::deactivate()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}

void SRTQBandReceiverImpl::calOn() throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
{   
    try {
        m_core.calOn();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();        
    }
    catch (ReceiversErrors::ReceiversErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getReceiversErrorsEx();
    }
    catch (...) {
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "SRTQBandReceiverImpl::calOn()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}


void SRTQBandReceiverImpl::calOff() throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
{
    try {
        m_core.calOff();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();        
    }
    catch (ReceiversErrors::ReceiversErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getReceiversErrorsEx();
    }
    catch (...) {
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "SRTQBandReceiverImpl::calOff()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}


void SRTQBandReceiverImpl::externalCalOn() throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
{   
    try {
        m_core.externalCalOn();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();        
    }
    catch (ReceiversErrors::ReceiversErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getReceiversErrorsEx();
    }
    catch (...) {
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "SRTQBandReceiverImpl::externalCalOn()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}


void SRTQBandReceiverImpl::externalCalOff() throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
{   
    try {
        m_core.externalCalOff();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();        
    }
    catch (ReceiversErrors::ReceiversErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getReceiversErrorsEx();
    }
    catch (...) {
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "SRTQBandReceiverImpl::externalCalOff()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}


void SRTQBandReceiverImpl::setLO(const ACS::doubleSeq& lo) throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
{
    try {
        m_core.setLO(lo);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();        
    }
    catch (ReceiversErrors::ReceiversErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getReceiversErrorsEx();
    }
    catch (...) {
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "SRTQBandReceiverImpl::setLO()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}


void SRTQBandReceiverImpl::setMode(const char * mode) throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
{
    try {
        m_core.setMode(mode);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
    catch (ReceiversErrors::ReceiversErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getReceiversErrorsEx();
    }
    catch (...) {
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "SRTQBandReceiverImpl::setMode()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}


ACS::doubleSeq * SRTQBandReceiverImpl::getCalibrationMark(
        const ACS::doubleSeq& freqs, 
        const ACS::doubleSeq& bandwidths, 
        const ACS::longSeq& feeds,
        const ACS::longSeq& ifs,
        ACS::doubleSeq_out skyFreq,
        ACS::doubleSeq_out skyBw,
        CORBA::Boolean_out onoff,
        CORBA::Double_out scaleFactor
        ) throw (
            CORBA::SystemException,
            ComponentErrors::ComponentErrorsEx,
            ReceiversErrors::ReceiversErrorsEx
        )
{
    ACS::doubleSeq_var result = new ACS::doubleSeq;
    ACS::doubleSeq_var resFreq = new ACS::doubleSeq;
    ACS::doubleSeq_var resBw = new ACS::doubleSeq;
    try {
        m_core.getCalibrationMark(result.inout(), resFreq.inout(), resBw.inout(), freqs, bandwidths, feeds,ifs,onoff,scaleFactor);
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();        
    }
    catch (ReceiversErrors::ReceiversErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getReceiversErrorsEx();
    }
    catch (...) {
        _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRTKBandReceiverImpl::getCalibrationMark()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
    skyFreq = resFreq._retn();
    skyBw = resBw._retn();
    return result._retn();
}


void SRTQBandReceiverImpl::getIFOutput(
            const ACS::longSeq& feeds,
            const ACS::longSeq& ifs,
            ACS::doubleSeq_out freqs,
            ACS::doubleSeq_out bw,
            ACS::longSeq_out pols,
            ACS::doubleSeq_out LO
    ) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, ReceiversErrors::ReceiversErrorsEx)
{
    ACS_SHORT_LOG((LM_INFO, "SRTQBandReceiverImpl::getIFOutput()"));
    ACS::doubleSeq_var freqs_res = new ACS::doubleSeq;
    ACS::doubleSeq_var bw_res = new ACS::doubleSeq;
    ACS::longSeq_var pols_res = new ACS::longSeq;
    ACS::doubleSeq_var LO_res = new ACS::doubleSeq;

    try {
        m_core.getIFOutput(feeds, ifs, freqs_res.inout(), bw_res.inout(), pols_res.inout(), LO_res.inout());
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();        
    }
    catch (ReceiversErrors::ReceiversErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getReceiversErrorsEx();
    }
    catch (...) {
        _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRTKBandReceiverImpl::getIFOutput()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }

    freqs = freqs_res._retn();
    bw = bw_res._retn();
    pols = pols_res._retn();
    LO = LO_res._retn();
}


CORBA::Long SRTQBandReceiverImpl::getFeeds(
        ACS::doubleSeq_out X,
        ACS::doubleSeq_out Y,
        ACS::doubleSeq_out power
        ) throw (
            CORBA::SystemException, 
            ComponentErrors::ComponentErrorsEx,
            ReceiversErrors::ReceiversErrorsEx
          )
{
    ACS::doubleSeq_var tempX = new ACS::doubleSeq;
    ACS::doubleSeq_var tempY = new ACS::doubleSeq;
    ACS::doubleSeq_var tempPower = new ACS::doubleSeq;
    long res;
    try {
        res = m_core.getFeeds(tempX.inout(), tempY.inout(), tempPower.inout());
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();        
    }
    catch (ReceiversErrors::ReceiversErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getReceiversErrorsEx();
    }
    catch (...) {
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "SRTQBandReceiverImpl::getFeeds()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
    X = tempX._retn();
    Y = tempY._retn();
    power = tempPower._retn();
    return res;
}


CORBA::Double SRTQBandReceiverImpl::getTaper(
        CORBA::Double freq,
        CORBA::Double bandWidth,
        CORBA::Long feed,
        CORBA::Long ifNumber,
        CORBA::Double_out waveLen
        ) throw (
            CORBA::SystemException,
            ComponentErrors::ComponentErrorsEx,
            ReceiversErrors::ReceiversErrorsEx
          )
{
    CORBA::Double res;
    double wL;
    try {
        res = (CORBA::Double)m_core.getTaper(freq, bandWidth, feed, ifNumber, wL);
        waveLen = wL;
        return res;
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();        
    }
    catch (ReceiversErrors::ReceiversErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getReceiversErrorsEx();
    }
    catch (...) {
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "SRTQBandReceiverImpl::getTaper()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }   
}


void SRTQBandReceiverImpl::turnLNAsOn() throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
{
    try {
        m_core.lnaOn();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
    catch (ReceiversErrors::ReceiversErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getReceiversErrorsEx();
    }
    catch (...) {
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "SRTQBandReceiverImpl::turnLNAsOn()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}


void SRTQBandReceiverImpl::turnLNAsOff() throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
{
    try {
        m_core.lnaOff();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
    catch (ReceiversErrors::ReceiversErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getReceiversErrorsEx();
    }
    catch (...) {
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "SRTQBandReceiverImpl::turnLNAsOff()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}

void SRTQBandReceiverImpl::turnVacuumSensorOn() throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
{
    try {
        m_core.vacuumSensorOn();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
    catch (ReceiversErrors::ReceiversErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getReceiversErrorsEx();
    }
    catch (...) {
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "SRTQBandReceiverImpl::turnVacuumSensorOn()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}


void SRTQBandReceiverImpl::turnVacuumSensorOff() throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
{
    try {
        m_core.vacuumSensorOff();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
    catch (ReceiversErrors::ReceiversErrorsExImpl& ex) {
        ex.log(LM_DEBUG);
        throw ex.getReceiversErrorsEx();
    }
    catch (...) {
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "SRTQBandReceiverImpl::turnVacuumSensorOff()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}

void SRTQBandReceiverImpl::turnAntennaUnitOn() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
    //has it to be implemented?
    ACS_LOG(LM_FULL_INFO,"SRTQBandReceiverImpl::turnAntennaUnitOn()",(LM_NOTICE,"QBAND_ANTENNA_UNIT_ON"));
}

void SRTQBandReceiverImpl::turnAntennaUnitOff() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
    //has it to be implemented?
    ACS_LOG(LM_FULL_INFO,"SRTQBandReceiverImpl::turnAntennaUnitOff()",(LM_NOTICE,"QBAND_ANTENNA_UNIT_OFF"));
}


_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROdoubleSeq, m_plocalOscillator, LO);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROpattern, m_pstatus, status);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROlongSeq, m_ppolarization, polarization);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROlong, m_pfeeds, feeds);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROlong, m_pIFs, IFs);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROdoubleSeq, m_pbandWidth, bandWidth);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROdoubleSeq, m_pinitialFrequency, initialFrequency);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROdouble, m_pvacuum, vacuum);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROdoubleSeq, m_pvdL1, vdL1);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROdoubleSeq, m_pvdR1, vdR1);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROdoubleSeq, m_pidL1, idL1);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROdoubleSeq, m_pidR1, idR1);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROdoubleSeq, m_pvgL1, vgL1);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROdoubleSeq, m_pvgR1, vgR1);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROdouble, m_pcryoTemperatureCoolHead, cryoTemperatureCoolHead);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROdouble, m_pcryoTemperatureCoolHeadWindow, cryoTemperatureCoolHeadWindow);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROdouble, m_pcryoTemperatureLNA, cryoTemperatureLNA);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROdouble, m_pcryoTemperatureLNAWindow, cryoTemperatureLNAWindow);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROdouble, m_penvironmentTemperature, environmentTemperature);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROstring, m_pmode, mode);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, Management::ROTSystemStatus, m_preceiverStatus, receiverStatus);
_PROPERTY_REFERENCE_CPP(SRTQBandReceiverImpl, ACS::ROstring, m_preceiverName, receiverName);

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SRTQBandReceiverImpl)
