#include "SRT5GHzImpl.h"
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


_IRA_LOGFILTER_DECLARE;

SRT5GHzImpl::SRT5GHzImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) :
	CharacteristicComponentImpl(CompName,containerServices),
	m_plocalOscillator(this),
	m_pfeeds(this),
	m_pIFs(this),
	m_pinitialFrequency(this),
	m_pbandWidth(this),
	m_ppolarization(this),
	m_pstatus(this),
	m_pvacuum(this),
	m_pVd_1(this),
	m_pVd_2(this),
	m_pId_1(this),
	m_pId_2(this),
	m_pVg_1(this),
	m_pVg_2(this),
	m_pcryoTemperatureCoolHead(this),
	m_pcryoTemperatureCoolHeadWindow(this),
	m_pcryoTemperatureLNA(this),
	m_pcryoTemperatureLNAWindow(this),
	m_penvironmentTemperature(this),
	m_pmode(this),
	m_preceiverStatus(this)
{	
	AUTO_TRACE("SRT5GHzImpl::SRT5GHzImpl()");
}

SRT5GHzImpl::~SRT5GHzImpl()
{
	AUTO_TRACE("SRT5GHzImpl::~SRT5GHzImpl()");
}

//throw (ACSErr::ACSbaseExImpl) 
void SRT5GHzImpl::initialize()
{
	AUTO_TRACE("SRT5GHzImpl::initialize()");
	ACS_LOG(LM_FULL_INFO,"SRT5GHzImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZING"));
	m_core.initialize(getContainerServices());
	m_monitor=NULL;
	ACS_LOG(LM_FULL_INFO,"SRT5GHzImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZED"));
}

//throw (ACSErr::ACSbaseExImpl)
void SRT5GHzImpl::execute()
{
	AUTO_TRACE("SRT5GHzImpl::execute()");
	ACS::Time timestamp;
	const CConfiguration *config=m_core.execute(); //throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl,ComponentErrors::SocketErrorExImpl)

	ACS_LOG(LM_FULL_INFO,"SRT5GHzImpl::execute()",(LM_INFO,"ACTIVATING_LOG_REPETITION_FILTER"));
	_IRA_LOGFILTER_ACTIVATE(config->getRepetitionCacheTime(),config->getRepetitionExpireTime());

	try {
		m_plocalOscillator=new baci::ROdoubleSeq(getContainerServices()->getName()+":LO",getComponent(),new DevIOLocalOscillator(&m_core),true);
		m_ppolarization=new baci::ROlongSeq(getContainerServices()->getName()+":polarization",getComponent(),new DevIOPolarization(&m_core),true);
		m_pmode=new baci::ROstring(getContainerServices()->getName()+":mode",getComponent(),new DevIOMode(&m_core),true);
		m_pinitialFrequency=new baci::ROdoubleSeq(getContainerServices()->getName()+":initialFrequency",getComponent(),new DevIOInitialFrequency(&m_core),true);
		m_pbandWidth=new baci::ROdoubleSeq(getContainerServices()->getName()+":bandWidth",getComponent(),new DevIOBandWidth(&m_core),true);
		m_pIFs=new baci::ROlong(getContainerServices()->getName()+":IFs",getComponent());
		m_pfeeds=new baci::ROlong(getContainerServices()->getName()+":feeds",getComponent());
		m_pvacuum=new baci::ROdouble(getContainerServices()->getName()+":vacuum",getComponent(),new DevIOVacuum(&m_core),true);
		m_pVd_1=new baci::ROdouble(getContainerServices()->getName()+":Vd_1",getComponent(),
				new DevIOLNAControls(&m_core,IRA::ReceiverControl::DRAIN_VOLTAGE,0),true);
		m_pVd_2=new baci::ROdouble(getContainerServices()->getName()+":Vd_2",getComponent(),
				new DevIOLNAControls(&m_core,IRA::ReceiverControl::DRAIN_VOLTAGE,1),true);
		m_pId_1=new baci::ROdouble(getContainerServices()->getName()+":Id_1",getComponent(),
				new DevIOLNAControls(&m_core,IRA::ReceiverControl::DRAIN_CURRENT,0),true);
		m_pId_2=new baci::ROdouble(getContainerServices()->getName()+":Id_2",getComponent(),
				new DevIOLNAControls(&m_core,IRA::ReceiverControl::DRAIN_CURRENT,1),true);
		m_pVg_1=new baci::ROdouble(getContainerServices()->getName()+":Vg_1",getComponent(),
				new DevIOLNAControls(&m_core,IRA::ReceiverControl::GATE_VOLTAGE,0),true);
		m_pVg_2=new baci::ROdouble(getContainerServices()->getName()+":Vg_2",getComponent(),
				new DevIOLNAControls(&m_core,IRA::ReceiverControl::GATE_VOLTAGE,1),true);
		m_pcryoTemperatureCoolHead=new baci::ROdouble(getContainerServices()->getName()+":cryoTemperatureCoolHead",getComponent(),
				new DevIOCryoTemperatureCoolHead(&m_core),true);
		m_pcryoTemperatureCoolHeadWindow=new baci::ROdouble(getContainerServices()->getName()+":cryoTemperatureCoolHeadWindow",getComponent(),
				new DevIOCryoTemperatureCoolHeadWin(&m_core),true);
		m_pcryoTemperatureLNA=new baci::ROdouble(getContainerServices()->getName()+":cryoTemperatureLNA",getComponent(),
				new DevIOCryoTemperatureLNA(&m_core),true);
		m_pcryoTemperatureLNAWindow=new baci::ROdouble(getContainerServices()->getName()+":cryoTemperatureLNAWindow",getComponent(),
				new DevIOCryoTemperatureLNAWin(&m_core),true);
		m_penvironmentTemperature=new baci::ROdouble(getContainerServices()->getName()+":environmentTemperature",getComponent());
				// new DevIOEnvTemperature(&m_core),true); // Is there a sensor?
		m_pstatus=new baci::ROpattern(getContainerServices()->getName()+":status",getComponent(),
				new DevIOStatus(&m_core),true);
		m_preceiverStatus=new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus>
				  (getContainerServices()->getName()+":receiverStatus",getComponent(),new DevIOComponentStatus(&m_core),true);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"SRT5GHzImpl::initialize()");
		throw dummy;
	}

	// write some fixed values
	m_pfeeds->getDevIO()->write(m_core.getFeeds(),timestamp);
	m_pIFs->getDevIO()->write(m_core.getIFs(),timestamp);
	m_core.setVacuumDefault(m_pvacuum->default_value());

	CComponentCore *temp=&m_core;
	try {
		m_monitor=getContainerServices()->getThreadManager()->create<CMonitorThread,CComponentCore*> (
				"WHATCHDOG5GHZ",temp,config->getWarchDogResponseTime()*10,config->getWatchDogSleepTime()*10);
	}
	catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"SRT5GHzImpl::execute()");
		throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"SRT5GHzImpl::execute()");
	}
	m_monitor->setLNASamplingTime(config->getLNASamplingTime());
	m_monitor->resume();
	ACS_LOG(LM_FULL_INFO,"SRT5GHzImpl::execute()",(LM_INFO,"WATCH_DOG_SPAWNED"));
	try {
		startPropertiesMonitoring();
	}
	catch (acsthreadErrType::CanNotStartThreadExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"SRT5GHzImpl::execute()");
		throw __dummy;
	}
	catch (ACSErrTypeCommon::NullPointerExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"SRT5GHzImpl::execute()");
		throw __dummy;		
	}
	ACS_LOG(LM_FULL_INFO,"SRT5GHzImpl::execute()",(LM_INFO,"COMPSTATE_OPERATIONAL"));
}

void SRT5GHzImpl::cleanUp()
{
	AUTO_TRACE("SRT5GHzImpl::cleanUp()");
	stopPropertiesMonitoring();
	if (m_monitor!=NULL) {
		m_monitor->suspend();
		getContainerServices()->getThreadManager()->destroy(m_monitor);
		m_monitor=NULL;
	}
	m_core.cleanup();
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
	CharacteristicComponentImpl::cleanUp();	
}

void SRT5GHzImpl::aboutToAbort()
{
	AUTO_TRACE("SRT5GHzImpl::aboutToAbort()");
	if (m_monitor!=NULL) {
		getContainerServices()->getThreadManager()->destroy(m_monitor);
	}
	m_core.cleanup();
}

//throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
void SRT5GHzImpl::activate(const char * setup_mode)
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT5GHzImpl::activate()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

// throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
void SRT5GHzImpl::deactivate()
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT5GHzImpl::deactivate()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

// throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
void SRT5GHzImpl::calOn()
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT5GHzImpl::calOn()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

//throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
void SRT5GHzImpl::calOff() 
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT5GHzImpl::calOff()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

// throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
void SRT5GHzImpl::externalCalOn()
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
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "SRT5GHzImpl::externalCalOn()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}

//throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
void SRT5GHzImpl::externalCalOff()
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
        _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "SRT5GHzImpl::externalCalOff()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
}
// throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
void SRT5GHzImpl::setLO(const ACS::doubleSeq& lo)
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT5GHzImpl::setLO()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

// throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
void SRT5GHzImpl::setMode(const char * mode)
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT5GHzImpl::setMode()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

//throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
ACS::doubleSeq *SRT5GHzImpl::getCalibrationMark(const ACS::doubleSeq& freqs, const ACS::doubleSeq& bandwidths, const ACS::longSeq& feeds,const ACS::longSeq& ifs,
		ACS::doubleSeq_out skyFreq,ACS::doubleSeq_out skyBw,CORBA::Boolean_out onoff,CORBA::Double_out scaleFactor)
{
	ACS::doubleSeq_var result=new ACS::doubleSeq;
	ACS::doubleSeq_var resFreq=new ACS::doubleSeq;
	ACS::doubleSeq_var resBw=new ACS::doubleSeq;
	try {
		m_core.getCalibrationMark(result.inout(),resFreq.inout(),resBw.inout(),freqs,bandwidths,feeds,ifs,onoff,scaleFactor);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT5GHzImpl::getCalibrationMark()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
	skyFreq=resFreq._retn();
	skyBw=resBw._retn();
	return result._retn();
}

// throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
CORBA::Long SRT5GHzImpl::getFeeds(ACS::doubleSeq_out X,ACS::doubleSeq_out Y,ACS::doubleSeq_out power)
{
	ACS::doubleSeq_var tempX=new ACS::doubleSeq;
	ACS::doubleSeq_var tempY=new ACS::doubleSeq;
	ACS::doubleSeq_var tempPower=new ACS::doubleSeq;
	long res;
	try {
		res=m_core.getFeeds(tempX.inout(),tempY.inout(),tempPower.inout());
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT5GHzImpl::getFeeds()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
	X=tempX._retn();
	Y=tempY._retn();
	power=tempPower._retn();
	return res;
}

// throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
void SRT5GHzImpl::getIFOutput(
		const ACS::longSeq& feeds,
        const ACS::longSeq& ifs,
        ACS::doubleSeq_out freqs,
        ACS::doubleSeq_out bw,
        ACS::longSeq_out pols,
        ACS::doubleSeq_out LO
)
{
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
        _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT5GHzImpl::getIFOutput()");
        impl.log(LM_DEBUG);
        throw impl.getComponentErrorsEx();
    }
    freqs = freqs_res._retn();
    bw = bw_res._retn();
    pols = pols_res._retn();
    LO = LO_res._retn();
}

//throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
CORBA::Double SRT5GHzImpl::getTaper(CORBA::Double freq,CORBA::Double bandWidth,CORBA::Long feed,CORBA::Long ifNumber,CORBA::Double_out waveLen)
{
	CORBA::Double res;
	double wL;
	try {
		res=(CORBA::Double)m_core.getTaper(freq,bandWidth,feed,ifNumber,wL);
		waveLen=wL;
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT5GHzImpl::getTaper()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}	
}

//throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
void SRT5GHzImpl::turnLNAsOn()
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT5GHzImpl::turnLNAsOn()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

//throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
void SRT5GHzImpl::turnLNAsOff()
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT5GHzImpl::turnLNAsOff()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

//throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
void SRT5GHzImpl::turnVacuumSensorOn()
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT5GHzImpl::turnVacuumSensorOn()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

//throw  (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
void SRT5GHzImpl::turnVacuumSensorOff()
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT5GHzImpl::turnVacuumSensorOff()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

//throw  (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
void SRT5GHzImpl::turnAntennaUnitOn()
{
	//has it to be implemented?
	ACS_LOG(LM_FULL_INFO,"SRT5GHzImpl::turnAntennaUnitOn()",(LM_WARNING,"Antenna Unit not supported"));
}

//throw  (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
void SRT5GHzImpl::turnAntennaUnitOff()
{
	//has it to be implemented?
	ACS_LOG(LM_FULL_INFO,"SRT5GHzImpl::turnAntennaUnitOff()",(LM_NOTICE,"Antenna Unit not supported"));
}

_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROdoubleSeq,m_plocalOscillator,LO);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROpattern,m_pstatus,status);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROlongSeq,m_ppolarization,polarization);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROlong,m_pfeeds,feeds);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROlong,m_pIFs,IFs);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROdoubleSeq,m_pbandWidth,bandWidth);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROdoubleSeq,m_pinitialFrequency,initialFrequency);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROdouble,m_pvacuum,vacuum);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROdouble,m_pVd_1,Vd_1);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROdouble,m_pVd_2,Vd_2);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROdouble,m_pId_1,Id_1);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROdouble,m_pId_2,Id_2);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROdouble,m_pVg_1,Vg_1);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROdouble,m_pVg_2,Vg_2);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROdouble,m_pcryoTemperatureCoolHead,cryoTemperatureCoolHead);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROdouble,m_pcryoTemperatureCoolHeadWindow,cryoTemperatureCoolHeadWindow);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROdouble,m_pcryoTemperatureLNA,cryoTemperatureLNA);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROdouble,m_pcryoTemperatureLNAWindow,cryoTemperatureLNAWindow);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROdouble,m_penvironmentTemperature,environmentTemperature);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,ACS::ROstring,m_pmode,mode);
_PROPERTY_REFERENCE_CPP(SRT5GHzImpl,Management::ROTSystemStatus,m_preceiverStatus,receiverStatus);


/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SRT5GHzImpl)
