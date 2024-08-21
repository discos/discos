

#include <Cplusplus11Helper.h>
#define _CPLUSPLUS11_PORTING_

#include "KQWBandReceiverImpl.h"
/*

#include "DevIOLNAControls.h"

*/
#include "DevIOFeeds.h"
#include "DevIOVacuum.h"
#include "DevIOLocalOscillator.h"
#include "DevIOPolarization.h"
#include "DevIOInitialFrequency.h"
#include "DevIOBandWidth.h"
#include "DevIOMode.h"
#include "DevIOCryoTemperatureCoolHead.h"
#include "DevIOCryoTemperatureCoolHeadWindow.h"
#include "DevIOCryoTemperatureLNA.h"
#include "DevIOCryoTemperatureLNAWindow.h"
#include "DevIOEnvTemperature.h"
#include "DevIOStatus.h"
#include "DevIOComponentStatus.h"

#include <LogFilter.h>


#include <iostream>

_IRA_LOGFILTER_DECLARE;

KQWBandReceiverImpl::KQWBandReceiverImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) :
    CharacteristicComponentImpl(CompName,containerServices),
    m_plocalOscillator(this),
    m_pfeeds(this),
    m_pIFs(this),
    m_pinitialFrequency(this),
    m_pbandWidth(this),
    m_ppolarization(this),
    m_pstatus(this),
    m_pvacuum(this),
    m_pvdKL(this),
    m_pvdKR(this),
    m_pvdQL(this),
    m_pvdQR(this),
    m_pvdWL(this),
    m_pvdWR(this),
    m_pidKL(this),
    m_pidKR(this),
    m_pidQL(this),
    m_pidQR(this),
    m_pidWL(this),
    m_pidWR(this),
    m_pvgKL(this),
    m_pvgKR(this),
    m_pvgQL(this),
    m_pvgQR(this),
    m_pvgWL(this),
    m_pvgWR(this),
    m_pcryoTemperatureCoolHead(this),
    m_pcryoTemperatureCoolHeadWindow(this),
    m_pcryoTemperatureLNA(this),
    m_pcryoTemperatureLNAWindow(this),
    m_penvironmentTemperature(this),
    m_pmode(this),
    m_preceiverStatus(this),
    m_preceiverName(this)
{   
    AUTO_TRACE("KQWBandReceiverImpl::KQWBandReceiverImpl()");
}


KQWBandReceiverImpl::~KQWBandReceiverImpl()
{
    AUTO_TRACE("KQWBandReceiverImpl::~KQWBandReceiverImpl()");
}

// throw ACSErr::ACSbaseExImpl
void KQWBandReceiverImpl::initialize()
{
    AUTO_TRACE("KQWBandReceiverImpl::initialize()");
    ACS_LOG(
            LM_FULL_INFO, 
            "KQWBandReceiverImpl::initialize()", 
            (LM_INFO, "KQWBandReceiverImpl::COMPSTATE_INITIALIZING")
    );
    m_core.initialize(getContainerServices());
    m_monitor = NULL;
    ACS_LOG(LM_FULL_INFO, "KQWBandReceiverImpl::initialize()", (LM_INFO, "COMPSTATE_INITIALIZED"));
}

//throw ACSErr::ACSbaseExImpl, ComponentErrors::MemoryAllocationExImpl
void KQWBandReceiverImpl::execute() 
{
    AUTO_TRACE("KQWBandReceiverImpl::execute()");
    ACS::Time timestamp;
    const CConfiguration<maci::ContainerServices> *config = m_core.execute(); 

    ACS_LOG(LM_FULL_INFO, "KQWBandReceiverImpl::execute()",(LM_INFO, "ACTIVATING_LOG_REPETITION_FILTER"));
    _IRA_LOGFILTER_ACTIVATE(config->getRepetitionCacheTime(), config->getRepetitionExpireTime());

    try {
        m_preceiverName=new ROstring(getContainerServices()->getName()+":receiverName",getComponent());
        m_pIFs=new baci::ROlong(getContainerServices()->getName()+":IFs",getComponent());
        m_pfeeds=new baci::ROlong(getContainerServices()->getName()+":feeds",getComponent(),new DevIOFeeds(&m_core),true);
        m_pvacuum=new baci::ROdouble(getContainerServices()->getName()+":vacuum",getComponent(),new DevIOVacuum(&m_core),true);
        m_plocalOscillator=new baci::ROdoubleSeq(getContainerServices()->getName()+":LO",getComponent(),new DevIOLocalOscillator(&m_core),true);
        m_ppolarization=new baci::ROlongSeq(getContainerServices()->getName()+":polarization",getComponent(),new DevIOPolarization(&m_core),true);
        m_pmode=new baci::ROstring(getContainerServices()->getName()+":mode",getComponent(),new DevIOMode(&m_core),true);
        m_pinitialFrequency=new baci::ROdoubleSeq(getContainerServices()->getName() + ":initialFrequency", getComponent(),new DevIOInitialFrequency(&m_core),true);
        m_pbandWidth=new baci::ROdoubleSeq(getContainerServices()->getName()+":bandWidth",getComponent(),new DevIOBandWidth(&m_core),true);

        // Drain Voltage
       // DevIOLNAControls *p=new DevIOLNAControls(&m_core, IRA::ReceiverControl::DRAIN_VOLTAGE, 0, 1);
        //m_pvdKL=new baci::ROdouble(getContainerServices()->getName() + ":vdKL",getComponent(),p,true);
        /*m_pvdKR=new baci::ROdouble(getContainerServices()->getName() + ":vdKR", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::DRAIN_VOLTAGE, 1, 1), true);
        m_pvdQL=new baci::ROdouble(getContainerServices()->getName() + ":vdQL", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::DRAIN_VOLTAGE, 0, 2), true);
        m_pvdQR=new baci::ROdouble(getContainerServices()->getName() + ":vdQR", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::DRAIN_VOLTAGE, 1, 2), true);
        m_pvdWL=new baci::ROdouble(getContainerServices()->getName() + ":vdWL", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::DRAIN_VOLTAGE, 0, 3), true);
        m_pvdWR=new baci::ROdouble(getContainerServices()->getName() + ":vdWR", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::DRAIN_VOLTAGE, 1, 3), true);

        // Drain Current
        m_pidKL=new baci::ROdouble(getContainerServices()->getName() + ":idKL", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::DRAIN_CURRENT, 0, 1), true);
        m_pidKR=new baci::ROdouble(getContainerServices()->getName() + ":idKR", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::DRAIN_CURRENT, 1, 1), true);
        m_pidQL=new baci::ROdouble(getContainerServices()->getName() + ":idQL", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::DRAIN_CURRENT, 0, 2), true);
        m_pidQR=new baci::ROdouble(getContainerServices()->getName() + ":idQR", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::DRAIN_CURRENT, 1, 2), true);
        m_pidWL=new baci::ROdouble(getContainerServices()->getName() + ":idWL", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::DRAIN_CURRENT, 0, 3), true);
        m_pidWR=new baci::ROdouble(getContainerServices()->getName() + ":idWR", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::DRAIN_CURRENT, 1, 3), true);

        // Gate Voltage
        m_pvgKL=new baci::ROdouble(getContainerServices()->getName() + ":vgKL", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::GATE_VOLTAGE, 0, 1), true);
        m_pvgKR=new baci::ROdouble(getContainerServices()->getName() + ":vgKR", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::GATE_VOLTAGE, 1, 1), true);
        m_pvgQL=new baci::ROdouble(getContainerServices()->getName() + ":vgQL", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::GATE_VOLTAGE, 0, 2), true);
        m_pvgQR=new baci::ROdouble(getContainerServices()->getName() + ":vgQR", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::GATE_VOLTAGE, 1, 2), true);
        m_pvgWL=new baci::ROdouble(getContainerServices()->getName() + ":vgWL", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::GATE_VOLTAGE, 0, 3), true);
        m_pvgWR=new baci::ROdouble(getContainerServices()->getName() + ":vgWR", getComponent(),
                new DevIOLNAControls(&m_core, IRA::ReceiverControl::GATE_VOLTAGE, 1, 3), true);*/

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
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, dummy, "KQWBandReceiverImpl::initialize()");
        throw dummy;
    }

    // Write some fixed values
    m_preceiverName->getDevIO()->write(getComponent()->getName(), timestamp);
    m_pIFs->getDevIO()->write(m_core.getIFs(),timestamp);
    m_core.setVacuumDefault(m_pvacuum->default_value());

    CComponentCore *temp = &m_core;
    try {
         m_monitor = getContainerServices()->getThreadManager()->create<CMonitorThread, CComponentCore*> (
                 "WHATCHDOKBANDMF", temp, config->getWarchDogResponseTime(), config->getWatchDogSleepTime());
    }
    catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
        _ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl, _dummy, ex, "KQWBandReceiverImpl::execute()");
        throw _dummy;
    }
    catch (...) {
        _THROW_EXCPT(ComponentErrors::UnexpectedExImpl, "KQWBandReceiverImpl::execute()");
    }
    m_monitor->setLNASamplingTime(config->getLNASamplingTime());
    m_monitor->resume();
    ACS_LOG(LM_FULL_INFO, "KQWBandReceiverImpl::execute()", (LM_INFO, "WATCH_DOG_SPAWNED"));
    try {
        startPropertiesMonitoring();
    }
    catch (acsthreadErrType::CanNotStartThreadExImpl& E) {
        _ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl, __dummy, E, "KQWBandReceiverImpl::execute()");
        throw __dummy;
    }
    catch (ACSErrTypeCommon::NullPointerExImpl& E) {
        _ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl, __dummy, E, "KQWBandReceiverImpl::execute()");
        throw __dummy;      
    }
    ACS_LOG(LM_FULL_INFO, "KQWBandReceiverImpl::execute()", (LM_INFO, "COMPSTATE_OPERATIONAL"));
}


void KQWBandReceiverImpl::cleanUp()
{
    AUTO_TRACE("KQWBandReceiverImpl::cleanUp()");
    stopPropertiesMonitoring();
    if (m_monitor != NULL) {
        m_monitor->suspend();
        m_monitor->terminate();
        getContainerServices()->getThreadManager()->destroy(m_monitor);
        m_monitor = NULL;
    }
    m_core.cleanup();
    _IRA_LOGFILTER_FLUSH;
    _IRA_LOGFILTER_DESTROY;
    CharacteristicComponentImpl::cleanUp(); 
}


void KQWBandReceiverImpl::aboutToAbort()
{
    AUTO_TRACE("KQWBandReceiverImpl::aboutToAbort()");
    stopPropertiesMonitoring();
    if (m_monitor != NULL) {
        m_monitor->suspend();
        m_monitor->terminate();
        getContainerServices()->getThreadManager()->destroy(m_monitor);
        m_monitor = NULL;
    }
    m_core.cleanup();
    CharacteristicComponentImpl::aboutToAbort(); 
}

/*
 throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
*/
void KQWBandReceiverImpl::activate(const char * setup_mode)
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
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "KQWBandReceiverImpl::activate()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}
/*
 throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
*/
void KQWBandReceiverImpl::deactivate()
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
        _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"KQWBandReceiverImpl::deactivate()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}

/*
 throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
*/
void KQWBandReceiverImpl::calOn()
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
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "KQWBandReceiverImpl::calOn()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}

/*
 throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
*/
void KQWBandReceiverImpl::calOff()
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
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "KQWBandReceiverImpl::calOff()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}

/*
 throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
*/
void KQWBandReceiverImpl::externalCalOn()
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
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "KQWBandReceiverImpl::externalCalOn()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}

/*
 throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
*/
void KQWBandReceiverImpl::externalCalOff()
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
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "KQWBandReceiverImpl::externalCalOff()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}

/*
 throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
*/
void KQWBandReceiverImpl::setLO(const ACS::doubleSeq& lo)
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
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "KQWBandReceiverImpl::setLO()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}

/*
 throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
*/
void KQWBandReceiverImpl::setMode(const char * mode)
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
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "KQWBandReceiverImpl::setMode()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}

/*
 throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
*/
ACS::doubleSeq * KQWBandReceiverImpl::getCalibrationMark(
        const ACS::doubleSeq& freqs, 
        const ACS::doubleSeq& bandwidths, 
        const ACS::longSeq& feeds,
        const ACS::longSeq& ifs,
        ACS::doubleSeq_out skyFreq,
        ACS::doubleSeq_out skyBw,
        CORBA::Boolean_out onoff,
        CORBA::Double_out scaleFactor
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
        _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"KQWBandReceiverImpl::getCalibrationMark()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
    skyFreq = resFreq._retn();
    skyBw = resBw._retn();
    return result._retn();
}

/*
 throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
*/
void KQWBandReceiverImpl::getIFOutput(
            const ACS::longSeq& feeds,
            const ACS::longSeq& ifs,
            ACS::doubleSeq_out freqs,
            ACS::doubleSeq_out bw,
            ACS::longSeq_out pols,
            ACS::doubleSeq_out LO
    )
{
    ACS_SHORT_LOG((LM_INFO, "KQWBandReceiverImpl::getIFOutput()"));
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
        _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"KQWBandReceiverImpl::getIFOutput()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }

    freqs = freqs_res._retn();
    bw = bw_res._retn();
    pols = pols_res._retn();
    LO = LO_res._retn();
}
            
/*
 throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
*/
CORBA::Long KQWBandReceiverImpl::getFeeds(
        ACS::doubleSeq_out X,
        ACS::doubleSeq_out Y,
        ACS::doubleSeq_out power
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
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "KQWBandReceiverImpl::getFeeds()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
    X = tempX._retn();
    Y = tempY._retn();
    power = tempPower._retn();
    return res;
}

/*
 throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
*/
CORBA::Double KQWBandReceiverImpl::getTaper(
        CORBA::Double freq,
        CORBA::Double bandWidth,
        CORBA::Long feed,
        CORBA::Long ifNumber,
        CORBA::Double_out waveLen
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
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "KQWBandReceiverImpl::getTaper()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }   
}


/*
 throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
*/
void KQWBandReceiverImpl::turnLNAsOn()
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
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "KQWBandReceiverImpl::turnLNAsOn()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}

/*
 throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
*/
void KQWBandReceiverImpl::turnLNAsOff()
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
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "KQWBandReceiverImpl::turnLNAsOff()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}

/*
 throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
*/
void KQWBandReceiverImpl::turnVacuumSensorOn()
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
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "KQWBandReceiverImpl::turnVacuumSensorOn()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}

/*
throw (
        CORBA::SystemException,
        ComponentErrors::ComponentErrorsEx,
        ReceiversErrors::ReceiversErrorsEx
        )
*/
void KQWBandReceiverImpl::turnVacuumSensorOff() 
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
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "KQWBandReceiverImpl::turnVacuumSensorOff()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}

// throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
void KQWBandReceiverImpl::turnAntennaUnitOn() 
{
    //has it to be implemented?
    ACS_LOG(LM_FULL_INFO,"KQWBandReceiverImpl::turnAntennaUnitOn()",(LM_NOTICE,"KBAND_ANTENNA_UNIT_ON"));
}

// throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
void KQWBandReceiverImpl::turnAntennaUnitOff()
{
    //has it to be implemented?
    ACS_LOG(LM_FULL_INFO,"KQWBandReceiverImpl::turnAntennaUnitOff()",(LM_NOTICE,"KBAND_ANTENNA_UNIT_OFF"));
}

_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdoubleSeq, m_plocalOscillator, LO);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROpattern, m_pstatus, status);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROlongSeq, m_ppolarization, polarization);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROlong, m_pfeeds, feeds);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROlong, m_pIFs, IFs);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdoubleSeq, m_pbandWidth, bandWidth);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdoubleSeq, m_pinitialFrequency, initialFrequency);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pvacuum, vacuum);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pvdKL, vdKL);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pvdKR, vdKR);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pvdQL, vdQL);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pvdQR, vdQR);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pvdWL, vdWL);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pvdWR, vdWR);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pidKL, idKL);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pidKR, idKR);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pidQL, idQL);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pidQR, idQR);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pidWL, idWL);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pidWR, idWR);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pvgKL, vgKL);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pvgKR, vgKR);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pvgQL, vgQL);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pvgQR, vgQR);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pvgWL, vgWL);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pvgWR, vgWR);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pcryoTemperatureCoolHead, cryoTemperatureCoolHead);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pcryoTemperatureCoolHeadWindow, cryoTemperatureCoolHeadWindow);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pcryoTemperatureLNA, cryoTemperatureLNA);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_pcryoTemperatureLNAWindow, cryoTemperatureLNAWindow);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROdouble, m_penvironmentTemperature, environmentTemperature);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROstring, m_pmode, mode);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, Management::ROTSystemStatus, m_preceiverStatus, receiverStatus);
_PROPERTY_REFERENCE_CPP(KQWBandReceiverImpl, ACS::ROstring, m_preceiverName, receiverName);

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(KQWBandReceiverImpl)
