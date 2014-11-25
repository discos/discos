#include "ReceiversBossImpl.h"
#include <ComponentErrors.h>
#include <ManagementErrors.h>
#include <LogFilter.h>
#include "DevIOLO.h"
#include "DevIORecvCode.h"
#include "DevIOStatus.h"
#include "DevIOFeeds.h"
#include "DevIOIFs.h"
#include "DevIOPolarization.h"
#include "DevIOBandWidth.h"
#include "DevIOInitialFrequency.h"
#include "DevIOMode.h"
#include "DevIODerotatorPosition.h"
#include <ReceiversModule.h>

static char *rcsId="@(#) $Id: ReceiversBossImpl.cpp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

using namespace SimpleParser;
using namespace baci;

_SP_WILDCARD_CLASS(DerotatorConfigurations_WildCard,"UNDEFINED");
_SP_WILDCARD_CLASS(RewindModes_WildCard,"UNDEFINED");

class DerotatorConfigurations_converter
{
public:
	Receivers::TDerotatorConfigurations strToVal(const char * str) throw (ParserErrors::BadTypeFormatExImpl) {
		Receivers::TDerotatorConfigurations mode;
		if (!Receivers::Definitions::map(str,mode)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"UpdateModes_converter::strToVal()");
			ex.setExpectedType("Update Mode");
			throw ex;
		}
		return mode;
	}
	char *valToStr(const Receivers::TDerotatorConfigurations& val) {
		IRA::CString mode(Receivers::Definitions::map(val));
		char *c=new char[mode.GetLength()+1];
		strcpy(c,(const char*)mode);
		return c;
	}
};

class RewindModes_converter
{
public:
	Receivers::TRewindModes strToVal(const char * str) throw (ParserErrors::BadTypeFormatExImpl) {
		Receivers::TRewindModes mode;
		if (!Receivers::Definitions::map(str,mode)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"RewindModes_converter::strToVal()");
			ex.setExpectedType("Rewind Mode");
			throw ex;
		}
		return mode;
	}
	char *valToStr(const Receivers::TRewindModes& val) {
		IRA::CString mode(Receivers::Definitions::map(val));
		char *c=new char[mode.GetLength()+1];
		strcpy(c,(const char*)mode);
		return c;
	}
};


_IRA_LOGFILTER_DECLARE;

#define WATCHING_THREAD_NAME "RECEIVERS_WATCHDOG"

ReceiversBossImpl::ReceiversBossImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) : 
	CharacteristicComponentImpl(CompName,containerServices),
	m_plocalOscillator(this),
	m_pactualSetup(this),
	m_pfeeds(this),
	m_pIFs(this),
	m_pinitialFrequency(this),
	m_pbandWidth(this),
	m_ppolarization(this),
	m_pstatus(this),
	m_pmode(this),
	m_pderotatorPosition(this)
{	
	AUTO_TRACE("ReceiversBossImpl::ReceiversBossImpl()");
	m_core=NULL;
}

ReceiversBossImpl::~ReceiversBossImpl()
{
	AUTO_TRACE("ReceiversBossImpl::~ReceiversBossImpl()");
}

void ReceiversBossImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("ReceiversBossImpl::initialize()");
	ACS_LOG(LM_FULL_INFO,"ReceiversBossImpl::initialize()",(LM_INFO,"ReceiversBossImpl::COMPSTATE_INITIALIZING"));
	m_config.init(getContainerServices()); // throw exceptions
	try {
		m_core=new CRecvBossCore();
		m_plocalOscillator=new ROdoubleSeq(getContainerServices()->getName()+":LO",getComponent(),new DevIOLO(m_core),true);
		m_pactualSetup=new ROstring(getContainerServices()->getName()+":actualSetup",getComponent(),new DevIORecvCode(m_core),true);
		m_pstatus=new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus> (getContainerServices()->getName()+":status",getComponent(),new DevIOStatus(m_core),true);
		m_pfeeds=new ROlong(getContainerServices()->getName()+":feeds",getComponent(),new DevIOFeeds(m_core),true);
		m_pIFs=new ROlong(getContainerServices()->getName()+":IFs",getComponent(),new DevIOIFs(m_core),true);
		m_ppolarization=new ROlongSeq(getContainerServices()->getName()+":polarization",getComponent(),new DevIOPolarization(m_core),true);
		m_pinitialFrequency=new ROdoubleSeq(getContainerServices()->getName()+":initialFrequency",getComponent(),new DevIOInitialFrequency(m_core),true);
		m_pbandWidth=new ROdoubleSeq(getContainerServices()->getName()+":bandWidth",getComponent(),new DevIOBandWidth(m_core),true);
		m_pmode=new ROstring(getContainerServices()->getName()+":mode",getComponent(),new DevIOMode(m_core),true);
		m_pderotatorPosition=new ROdouble(getContainerServices()->getName()+":derotatorPosition",getComponent(),
		  new DevIODerotatorPosition(m_core),true);
		// create the parser for command line execution
		m_parser= new SimpleParser::CParser<CRecvBossCore>(m_core,10);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"ReceiversBossImpl::initialize()");
		throw dummy;
	}
	m_core->initialize(getContainerServices(),&m_config,this);
	m_parser->add("receiversPark",new function0<CRecvBossCore,non_constant,void_type >(m_core,&CRecvBossCore::park),0);
	m_parser->add("receiversSetup",new function1<CRecvBossCore,non_constant,void_type,I<string_type> >(m_core,&CRecvBossCore::setupReceiver),1);
	m_parser->add("receiversMode",new function1<CRecvBossCore,non_constant,void_type,I<string_type> >(m_core,&CRecvBossCore::setMode),1);
	m_parser->add("calOn",new function0<CRecvBossCore,non_constant,void_type >(m_core,&CRecvBossCore::calOn),0);
	m_parser->add("calOff",new function0<CRecvBossCore,non_constant,void_type >(m_core,&CRecvBossCore::calOff),0);
	m_parser->add("setLO",new function1<CRecvBossCore,non_constant,void_type,I<doubleSeq_type> >(m_core,&CRecvBossCore::setLO),1);
	m_parser->add("antennaUnitOn",new function0<CRecvBossCore,non_constant,void_type >(m_core,&CRecvBossCore::AUOn),0);
	m_parser->add("antennaUnitOff",new function0<CRecvBossCore,non_constant,void_type >(m_core,&CRecvBossCore::AUOff),0);
	m_parser->add("derotatorMode",new function2<CRecvBossCore,non_constant,void_type,
			I<enum_type<DerotatorConfigurations_converter,Receivers::TDerotatorConfigurations,DerotatorConfigurations_WildCard> >,
			I<enum_type<RewindModes_converter,Receivers::TRewindModes,RewindModes_WildCard> > >(m_core,&CRecvBossCore::derotatorMode),2);
	m_parser->add("derotatorPark",new function0<CRecvBossCore,non_constant,void_type >(m_core,&CRecvBossCore::derotatorPark),0);
	m_parser->add("derotatorPosition",new function1<CRecvBossCore,non_constant,void_type, I<angle_type<SimpleParser::deg> > >(m_core,&CRecvBossCore::setDerotatorPosition),1);

	ACS_LOG(LM_FULL_INFO,"ReceiversBossImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZED"));
}

void ReceiversBossImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("ReceiversBossImpl::execute()");
	_IRA_LOGFILTER_ACTIVATE(m_config.repetitionCacheTime(),m_config.expireCacheTime());
	ACS_LOG(LM_FULL_INFO,"ReceiversBossImpl::cleanUp()",(LM_INFO,"LOG_FILTER_ACTIVATED"));
	m_core->execute(); //could throw exceptions
	try {
		m_watcher=getContainerServices()->getThreadManager()->create<CWatchingThread,CRecvBossCore *>(WATCHING_THREAD_NAME,m_core);
	}
	catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"ReceiversBossImpl::execute()");
		throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"ReceiversBossImpl::execute()");
	}
	m_watcher->resume();
	ACS_LOG(LM_FULL_INFO,"ReceiversBossImpl::cleanUp()",(LM_INFO,"THREAD_SPAWNED"));
	/*m_schedExecuter->initialize(m_services,m_dut1,m_site); // throw (ComponentErrors::TimerErrorExImpl)
	ACS::TimeInterval sleepTime=m_config->getScheduleExecutorSleepTime()*10;
	m_schedExecuter->setSleepTime(sleepTime);*/

	try {
		startPropertiesMonitoring();
	}
	catch (acsthreadErrType::CanNotStartThreadExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"ReceiversBossImpl::execute()");
		throw __dummy;
	}
	catch (ACSErrTypeCommon::NullPointerExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"ReceiversBossImpl::execute()");
		throw __dummy;		
	}
	ACS_LOG(LM_FULL_INFO,"ReceiversBossImpl::execute()",(LM_INFO,"COMPSTATE_OPERATIONAL"));
}

void ReceiversBossImpl::cleanUp()
{
	AUTO_TRACE("ReceiversBossImpl::cleanUp()");
	stopPropertiesMonitoring();
	if (m_parser) delete m_parser;
	if (m_core) {
		m_core->cleanUp();
		delete m_core;
	}
	if (m_watcher!=NULL) {
		m_watcher->suspend();
		getContainerServices()->getThreadManager()->destroy(m_watcher);
	}
	ACS_LOG(LM_FULL_INFO,"ReceiversBossImpl::cleanUp()",(LM_INFO,"THREAD_DESTROYED"));
	_IRA_LOGFILTER_FLUSH;
	ACS_LOG(LM_FULL_INFO,"ReceiversBossImpl::cleanUp()",(LM_INFO,"LOG_FILTER_FLUSHED"));
	_IRA_LOGFILTER_DESTROY;
	CharacteristicComponentImpl::cleanUp();	
}

void ReceiversBossImpl::aboutToAbort()
{
	AUTO_TRACE("ReceiversBossImpl::aboutToAbort()");
	if (m_parser) delete m_parser;
	if (m_core) {
		m_core->cleanUp();
		delete m_core;
	}
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
}

void ReceiversBossImpl::calOn() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{	
	try {
		m_core->calOn();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"ReceiversBossImpl::calOn()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

void ReceiversBossImpl::calOff() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	try {
		m_core->calOff();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"ReceiversBossImpl::calOff()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

void ReceiversBossImpl::setLO(const ACS::doubleSeq& lo) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	try {
		m_core->setLO(lo);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"ReceiversBossImpl::setLO()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

void ReceiversBossImpl::setup(const char * code) throw (CORBA::SystemException,ManagementErrors::ConfigurationErrorEx)
{
	try {
		m_core->setupReceiver(code);
	}
	catch (ManagementErrors::ConfigurationErrorExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getConfigurationErrorEx();
	}
}

void ReceiversBossImpl::setMode(const char * mode) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	try {
		m_core->setMode(mode); 
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"ReceiversBossImpl::setMode()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

void ReceiversBossImpl::park() throw (CORBA::SystemException,ManagementErrors::ParkingErrorEx)
{
	try {
		m_core->park();
	}
	catch (ManagementErrors::ParkingErrorExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getParkingErrorEx();
	}
}

ACS::doubleSeq *ReceiversBossImpl::getCalibrationMark(const ACS::doubleSeq& freqs, const ACS::doubleSeq& bandwidths, const ACS::longSeq& feeds,const ACS::longSeq& ifs,
		ACS::doubleSeq_out skyFreq,ACS::doubleSeq_out skyBw,CORBA::Double_out scaleFactor) throw (CORBA::SystemException,	ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	ACS::doubleSeq_var result=new ACS::doubleSeq;
	ACS::doubleSeq_var resFreq=new ACS::doubleSeq;
	ACS::doubleSeq_var resBw=new ACS::doubleSeq;
	try {
		m_core->getCalibrationMark(result,resFreq,resBw,freqs,bandwidths,feeds,ifs,scaleFactor);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"ReceiversBossImpl::getCalibrationMark()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
	skyFreq=resFreq._retn();
	skyBw=resBw._retn();
	return result._retn();
}

void ReceiversBossImpl::getIFOutput (const ACS::longSeq & feeds, const ACS::longSeq & ifs,ACS::doubleSeq_out freqs,ACS::doubleSeq_out bw,ACS::longSeq_out pols,ACS::doubleSeq_out LO) throw (
		CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	ACS::doubleSeq_var resFreq=new ACS::doubleSeq;
	ACS::doubleSeq_var resBw=new ACS::doubleSeq;
	ACS::longSeq_var resPols=new ACS::longSeq;
	ACS::doubleSeq_var resLO=new ACS::doubleSeq;
	try {
		m_core->getIFOutput(feeds,ifs,resFreq,resBw,resPols,resLO);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"ReceiversBossImpl::getIFOutput()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
	freqs=resFreq._retn();
	bw=resBw._retn();
	pols=resPols._retn();
	LO=resLO._retn();
}

CORBA::Long ReceiversBossImpl::getFeeds(ACS::doubleSeq_out X,ACS::doubleSeq_out Y,ACS::doubleSeq_out power) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
		ReceiversErrors::ReceiversErrorsEx)
{
	ACS::doubleSeq_var tempX=new ACS::doubleSeq;
	ACS::doubleSeq_var tempY=new ACS::doubleSeq;
	ACS::doubleSeq_var tempPower=new ACS::doubleSeq;
	long res;
	try {
		res=m_core->getFeeds(tempX,tempY,tempPower);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"ReceiversBossImpl::getFeeds()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
	X=tempX._retn();
	Y=tempY._retn();
	power=tempPower._retn();
	return res;
}

CORBA::Double ReceiversBossImpl::getTaper(CORBA::Double freq,CORBA::Double bandWidth,CORBA::Long feed,CORBA::Long ifNumber,CORBA::Double_out waveLen) throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	CORBA::Double res;
	double wL;
	try {
		res=(CORBA::Double)m_core->getTaper(freq,bandWidth,feed,ifNumber,wL);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"ReceiversBossImpl::getTaper()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}	
}

CORBA::Boolean ReceiversBossImpl::command(const char *cmd,CORBA::String_out answer) throw (CORBA::SystemException)
{
	IRA::CString out;
	bool res;
	try {
		m_parser->run(cmd,out);
		res=true;
	}
	catch (ParserErrors::ParserErrorsExImpl &ex) {
		res=false;
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		ex.log(LM_ERROR); // the errors resulting from the execution are logged here as stated in the documentation of CommandInterpreter interface, while the parser errors are never logged.
		res=false;
	}
	answer=CORBA::string_dup((const char *)out);
	return res;
}

void ReceiversBossImpl::turnAntennaUnitOn() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	try {
		m_core->AUOn();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"ReceiversBossImpl::turnAntennaUnitOn()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

void ReceiversBossImpl::turnAntennaUnitOff() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	try {
		m_core->AUOff();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"ReceiversBossImpl::turnAntennaUnitOff()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

void ReceiversBossImpl::startScan(ACS::Time& startUT,const Receivers::TReceiversParameters& param,
		const Antenna::TRunTimeParameters& antennaInfo) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	try{
		m_core->startScan(startUT,param,antennaInfo);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"ReceiversBossImpl::startScan()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

CORBA::Boolean ReceiversBossImpl::checkScan(ACS::Time startUt,const Receivers::TReceiversParameters& param,const Antenna::TRunTimeParameters& antennaInfo,
  Receivers::TRunTimeParameters_out runTime) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	// At the moment no need to perform checks or something
	// also no need to compute startUT
	if (startUt!=0) {
		runTime.startEpoch=startUt;
	}
	else {
		TIMEVALUE now;
		IRA::CIRATools::getTime(now);
		runTime.startEpoch=now.value().value+2000000; // take now, plus 0.2 sec
	}
	runTime.onTheFly=false;
	return true;
}

void ReceiversBossImpl::setDerotatorPosition(CORBA::Double position) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
  ReceiversErrors::ReceiversErrorsEx)
{
	try {
		m_core->setDerotatorPosition(position);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"ReceiversBossImpl::setDerotatorPosition()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

CORBA::Double ReceiversBossImpl::getDerotatorPositionFromHistory(ACS::Time epoch) throw (CORBA::SystemException,
  ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	try {
		return (CORBA::Double)m_core->getDerotatorPosition(epoch);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"ReceiversBossImpl::getDerotatorPosition()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

void ReceiversBossImpl::derotatorRewind(CORBA::Long steps) throw (
		CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	try {
		//m_core->derotatorMode(mode,rewind);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"ReceiversBossImpl::derotatorRewind()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}


void ReceiversBossImpl::derotatorMode(Receivers::TDerotatorConfigurations mode,Receivers::TRewindModes rewind) throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	try {
		m_core->derotatorMode(mode,rewind);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"ReceiversBossImpl::derotatorSetup()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

void ReceiversBossImpl::derotatorPark() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	try {
		m_core->derotatorPark();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"ReceiversBossImpl::derotatorPark()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}

void ReceiversBossImpl::getDewarParameter(Receivers::TDerotatorConfigurations_out mod,CORBA::Double_out pos) throw (
  CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx)
{
	try {
		m_core->getDewarParameter(mod,pos);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"ReceiversBossImpl::getDewarParameter()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
}


_PROPERTY_REFERENCE_CPP(ReceiversBossImpl,ACS::ROdoubleSeq,m_plocalOscillator,LO);
_PROPERTY_REFERENCE_CPP(ReceiversBossImpl,ACS::ROstring,m_pactualSetup,actualSetup);
_PROPERTY_REFERENCE_CPP(ReceiversBossImpl,Management::ROTSystemStatus,m_pstatus,status);
_PROPERTY_REFERENCE_CPP(ReceiversBossImpl,ACS::ROlongSeq,m_ppolarization,polarization);
_PROPERTY_REFERENCE_CPP(ReceiversBossImpl,ACS::ROlong,m_pfeeds,feeds);
_PROPERTY_REFERENCE_CPP(ReceiversBossImpl,ACS::ROlong,m_pIFs,IFs);
_PROPERTY_REFERENCE_CPP(ReceiversBossImpl,ACS::ROdoubleSeq,m_pbandWidth,bandWidth);
_PROPERTY_REFERENCE_CPP(ReceiversBossImpl,ACS::ROdoubleSeq,m_pinitialFrequency,initialFrequency);
_PROPERTY_REFERENCE_CPP(ReceiversBossImpl,ACS::ROstring,m_pmode,mode);
_PROPERTY_REFERENCE_CPP(ReceiversBossImpl,ACS::ROdouble,m_pderotatorPosition,derotatorPosition);


/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(ReceiversBossImpl)
