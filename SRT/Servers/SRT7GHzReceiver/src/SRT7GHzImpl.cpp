#include "SRT7GHzImpl.h"

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
		/*m_plocalOscillator=new ROdoubleSeq(getContainerServices()->getName()+":LO",getComponent(),new DevIOLO(m_core),true);
		m_pactualSetup=new ROstring(getContainerServices()->getName()+":actualSetup",getComponent(),new DevIORecvCode(m_core),true);
		m_pstatus=new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus> (getContainerServices()->getName()+":status",getComponent(),new DevIOStatus(m_core),true);
		m_pfeeds=new ROlong(getContainerServices()->getName()+":feeds",getComponent(),new DevIOFeeds(m_core),true);
		m_pIFs=new ROlong(getContainerServices()->getName()+":IFs",getComponent(),new DevIOIFs(m_core),true);
		m_ppolarization=new ROlongSeq(getContainerServices()->getName()+":polarization",getComponent(),new DevIOPolarization(m_core),true);
		
		m_pinitialFrequency=new ROdoubleSeq(getContainerServices()->getName()+":initialFrequency",getComponent(),new DevIOInitialFrequency(m_core),true);
		m_pbandWidth=new ROdoubleSeq(getContainerServices()->getName()+":bandWidth",getComponent(),new DevIOBandWidth(m_core),true);*/

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
	m_core.execute(); //throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl)
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
	CharacteristicComponentImpl::cleanUp();	
}

void SRT7GHzImpl::aboutToAbort()
{
	AUTO_TRACE("SRT7GHzImpl::aboutToAbort()");
	m_core.cleanup();
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

void SRT7GHzImpl::setMode(const char * mode) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ConfigurationErrorEx)
{
	try {
		//m_core->setMode(mode);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
	catch (ManagementErrors::ConfigurationErrorExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getConfigurationErrorEx();		
	}
}

ACS::doubleSeq *SRT7GHzImpl::getCalibrationMark(const ACS::doubleSeq& freqs, const ACS::doubleSeq& bandwidths, const ACS::longSeq& feeds,const ACS::longSeq& ifs) throw (CORBA::SystemException,
	ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	ACS::doubleSeq_var result=new ACS::doubleSeq;
	try {
		//m_core->getCalibrationMark(result,freqs,bandwidths,feeds,ifs);
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
		//res=m_core->getFeeds(tempX,tempY,tempPower);
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
		//res=(CORBA::Double)m_core->getTaper(freq,bandWidth,feed,ifNumber,wL);
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
