#include "SRT7GHzImpl.h"
#include "DevIOBandWidth.h"
#include "DevIOInitialFrequency.h"
#include "DevIOLocalOscillator.h"
#include "DevIOPolarization.h"
#include "DevIOMode.h"
#include <LogFilter.h>


_IRA_LOGFILTER_DECLARE;

SRT7GHzImpl::SRT7GHzImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) :
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
	m_pmode(this)
{	
	AUTO_TRACE("SRT7GHzImpl::SRT7GHzImpl()");
}

SRT7GHzImpl::~SRT7GHzImpl()
{
	AUTO_TRACE("SRT7GHzImpl::~SRT7GHzImpl()");
}

void SRT7GHzImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("SRT7GHzImpl::initialize()");
	ACS_LOG(LM_FULL_INFO,"SRT7GHzImpl::initialize()",(LM_INFO,"SRT7GHzImpl::COMPSTATE_INITIALIZING"));
	m_core.initialize(getContainerServices());
	try {
		m_plocalOscillator=new baci::ROdoubleSeq(getContainerServices()->getName()+":LO",getComponent(),new DevIOLocalOscillator(&m_core),true);
		m_ppolarization=new baci::ROlongSeq(getContainerServices()->getName()+":polarization",getComponent(),new DevIOPolarization(&m_core),true);
		m_pmode=new baci::ROstring(getContainerServices()->getName()+":mode",getComponent(),new DevIOMode(&m_core),true);
		m_pinitialFrequency=new baci::ROdoubleSeq(getContainerServices()->getName()+":initialFrequency",getComponent(),new DevIOInitialFrequency(&m_core),true);
		m_pbandWidth=new baci::ROdoubleSeq(getContainerServices()->getName()+":bandWidth",getComponent(),new DevIOBandWidth(&m_core),true);
		m_pIFs=new baci::ROlong(getContainerServices()->getName()+":IFs",getComponent());
		m_pfeeds=new baci::ROlong(getContainerServices()->getName()+":feeds",getComponent());
		m_pvacuum=new baci::ROdouble(getContainerServices()->getName()+":vacuum",getComponent());
		m_pVd_1=new baci::ROdouble(getContainerServices()->getName()+":Vd_1",getComponent());
		m_pVd_2=new baci::ROdouble(getContainerServices()->getName()+":Vd_2",getComponent());
		m_pId_1=new baci::ROdouble(getContainerServices()->getName()+":Id_1",getComponent());
		m_pId_2=new baci::ROdouble(getContainerServices()->getName()+":Id_2",getComponent());
		m_pVg_1=new baci::ROdouble(getContainerServices()->getName()+":Vg_1",getComponent());
		m_pVg_2=new baci::ROdouble(getContainerServices()->getName()+":Vg_2",getComponent());
		m_pcryoTemperatureCoolHead=new baci::ROdouble(getContainerServices()->getName()+":cryoTemperatureCoolHead",getComponent());
		m_pcryoTemperatureCoolHeadWindow=new baci::ROdouble(getContainerServices()->getName()+":cryoTemperatureCoolHeadWindow",getComponent());
		m_pcryoTemperatureLNA=new baci::ROdouble(getContainerServices()->getName()+":cryoTemperatureLNA",getComponent());
		m_pcryoTemperatureLNAWindow=new baci::ROdouble(getContainerServices()->getName()+":cryoTemperatureLNAWindow",getComponent());
		m_penvironmentTemperature=new baci::ROdouble(getContainerServices()->getName()+":environmentTemperature",getComponent());
		m_pstatus=new baci::ROpattern(getContainerServices()->getName()+":status",getComponent());
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"SRT7GHzImpl::initialize()");
		throw dummy;
	}
	ACS_LOG(LM_FULL_INFO,"SRT7GHzImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZED"));
}

void SRT7GHzImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("SRT7GHzImpl::execute()");
	ACS::Time timestamp;
	const CConfiguration *config=m_core.execute(); //throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl)

	ACS_LOG(LM_FULL_INFO,"SRT7GHzImpl::execute()",(LM_INFO,"ACTIVATING_LOG_REPETITION_FILTER"));
	_IRA_LOGFILTER_ACTIVATE(config->getRepetitionCacheTime(),config->getRepetitionExpireTime());

	// write some fixed values
	m_pfeeds->getDevIO()->write(m_core.getFeeds(),timestamp);
	m_pIFs->getDevIO()->write(m_core.getIFs(),timestamp);

	try {
		startPropertiesMonitoring();
	}
	catch (acsthreadErrType::CanNotStartThreadExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"SRT7GHzImpl::execute()");
		throw __dummy;
	}
	catch (ACSErrTypeCommon::NullPointerExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"SRT7GHzImpl::execute()");
		throw __dummy;		
	}
	ACS_LOG(LM_FULL_INFO,"SRT7GHzImpl::execute()",(LM_INFO,"COMPSTATE_OPERATIONAL"));
}

void SRT7GHzImpl::cleanUp()
{
	AUTO_TRACE("SRT7GHzImpl::cleanUp()");
	stopPropertiesMonitoring();
	m_core.cleanup();
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
	CharacteristicComponentImpl::cleanUp();	
}

void SRT7GHzImpl::aboutToAbort()
{
	AUTO_TRACE("SRT7GHzImpl::aboutToAbort()");
	m_core.cleanup();
}

void SRT7GHzImpl::activate() throw (ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT7GHzImpl::activate()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

void SRT7GHzImpl::calOn() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{	
	try {
		//m_core->calOn();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT7GHzImpl::calOn()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

void SRT7GHzImpl::calOff() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	try {
		//m_core->calOff();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT7GHzImpl::calOff()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

void SRT7GHzImpl::setLO(const ACS::doubleSeq& lo) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT7GHzImpl::setLO()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

void SRT7GHzImpl::setMode(const char * mode) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT7GHzImpl::setMode()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

ACS::doubleSeq *SRT7GHzImpl::getCalibrationMark(const ACS::doubleSeq& freqs, const ACS::doubleSeq& bandwidths, const ACS::longSeq& feeds,const ACS::longSeq& ifs) throw (CORBA::SystemException,
	ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	ACS::doubleSeq_var result=new ACS::doubleSeq;
	try {
		m_core.getCalibrationMark(result.inout(),freqs,bandwidths,feeds,ifs);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT7GHzImpl::getCalibrationMark()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
	return result._retn();
}

CORBA::Long SRT7GHzImpl::getFeeds(ACS::doubleSeq_out X,ACS::doubleSeq_out Y,ACS::doubleSeq_out power) throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT7GHzImpl::getFeeds()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
	X=tempX._retn();
	Y=tempY._retn();
	power=tempPower._retn();
	return res;
}

CORBA::Double SRT7GHzImpl::getTaper(CORBA::Double freq,CORBA::Double bandWidth,CORBA::Long feed,CORBA::Long ifNumber,CORBA::Double_out waveLen) throw (
		CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT7GHzImpl::getTaper()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}	
}

void SRT7GHzImpl::turnLNAsOn() throw (ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	try {
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT7GHzImpl::turnLNAsOn()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

void SRT7GHzImpl::turnLNAsOff() throw (ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	try {
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SRT7GHzImpl::turnLNAsOff()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROdoubleSeq,m_plocalOscillator,LO);
_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROpattern,m_pstatus,status);
_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROlongSeq,m_ppolarization,polarization);
_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROlong,m_pfeeds,feeds);
_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROlong,m_pIFs,IFs);
_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROdoubleSeq,m_pbandWidth,bandWidth);
_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROdoubleSeq,m_pinitialFrequency,initialFrequency);
_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROdouble,m_pvacuum,vacuum);
_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROdouble,m_pVd_1,Vd_1);
_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROdouble,m_pVd_2,Vd_2);
_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROdouble,m_pId_1,Id_1);
_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROdouble,m_pId_2,Id_2);
_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROdouble,m_pVg_1,Vg_1);
_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROdouble,m_pVg_2,Vg_2);
_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROdouble,m_pcryoTemperatureCoolHead,cryoTemperatureCoolHead);
_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROdouble,m_pcryoTemperatureCoolHeadWindow,cryoTemperatureCoolHeadWindow);
_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROdouble,m_pcryoTemperatureLNA,cryoTemperatureLNA);
_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROdouble,m_pcryoTemperatureLNAWindow,cryoTemperatureLNAWindow);
_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROdouble,m_penvironmentTemperature,environmentTemperature);
_PROPERTY_REFERENCE_CPP(SRT7GHzImpl,ACS::ROstring,m_pmode,mode);


/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SRT7GHzImpl)
