// $Id: NoiseGeneratorImpl.cpp,v 1.1 2011-03-14 15:16:22 a.orlati Exp $

#include "NoiseGeneratorImpl.h"
#include <Definitions.h>
#include <ComponentErrors.h>
#include "Common.h"
#include "DevIOAttenuation.h"
#include "DevIOBandWidth.h"
#include "DevIOTime.h"
#include "DevIOFrequency.h"
#include "DevIOPolarization.h"
#include "DevIOInputsNumber.h"
#include "DevIOSampleRate.h"
#include "DevIOBins.h"
#include "DevIOIntegration.h"
#include "DevIOStatus.h"
#include "DevIOBusy.h"
#include "DevIOFeed.h"
#include "DevIOTsys.h"
#include "DevIOInputSection.h"
#include "DevIOSectionsNumber.h"
#include "SenderThread.h"

static char *rcsId="@(#) $Id: NoiseGeneratorImpl.cpp,v 1.1 2011-03-14 15:16:22 a.orlati Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

using namespace SimpleParser;

NoiseGeneratorImpl::NoiseGeneratorImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) :
	BulkDataSenderDefaultImpl(CompName,containerServices),
	m_ptime(this),
	m_pbackendName(this),
	m_pbandWidth(this),
	m_pfrequency(this),
	m_psampleRate(this),
	m_pattenuation(this),
	m_ppolarization(this),
	m_pbins(this),
	m_pinputsNumber(this),
	m_pintegration(this),
	m_pstatus(this),
	m_pbusy(this),
	m_pfeed(this),
	m_ptsys(this),
	m_psectionsNumber(this),
	m_pinputSection(this)
{
	AUTO_TRACE("NoiseGeneratorImpl::NoiseGeneratorImpl");
	m_initialized=false;
	m_senderThread=NULL;
}

NoiseGeneratorImpl::~NoiseGeneratorImpl() 
{
	AUTO_TRACE("NoiseGeneratorImpl::~NoiseGeneratorImpl()");
	// if the initialization failed...clear everything as well. 
	// otherwise it is called by the cleanUp procedure.
	if (!m_initialized) deleteAll();
}

void NoiseGeneratorImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	CError Err;
	CSenderThread::TSenderParameter threadPar;
	AUTO_TRACE("NoiseGeneratorImpl::initialize()");
	ACS_LOG(LM_FULL_INFO,"NoiseGeneratorImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZING"));
	ACS_LOG(LM_FULL_INFO,"NoiseGeneratorImpl::initialize()",(LM_INFO,"READING_CONFIGURATION"));
	// could throw an ComponentErrors::CDBAccessExImpl exception
	m_configuration.init(getContainerServices());
	ACS_LOG(LM_FULL_INFO,"NoiseGeneratorImpl::initialize()",(LM_INFO,"CONFIGURATION_OK"));
	try {
		m_commandLine=new CCommandLine();
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"NoiseGeneratorImpl::initialize()");
		throw dummy;
	}
	ACS_LOG(LM_FULL_INFO,"NoiseGeneratorImpl::initialize()",(LM_INFO,"PROPERTY_CREATION"));
	try {	
		m_ptime=new ROuLongLong(getContainerServices()->getName()+":time",getComponent(),
				new DevIOTime(m_commandLine),true);
		m_pbackendName=new ROstring(getContainerServices()->getName()+":backendName",getComponent());
		m_pbandWidth=new ROdoubleSeq(getContainerServices()->getName()+":bandWidth",getComponent(),
				new DevIOBandWidth(m_commandLine),true);
		m_pfrequency=new ROdoubleSeq(getContainerServices()->getName()+":frequency",getComponent(),
				new DevIOFrequency(m_commandLine),true);
		m_psampleRate=new ROdoubleSeq(getContainerServices()->getName()+":sampleRate",getComponent(),
				new DevIOSampleRate(m_commandLine),true);
		m_pattenuation=new ROdoubleSeq(getContainerServices()->getName()+":attenuation",getComponent(),
				new DevIOAttenuation(m_commandLine),true);
		m_ppolarization=new ROlongSeq(getContainerServices()->getName()+":polarization",getComponent(),
				new DevIOPolarization(m_commandLine),true);
		m_pinputsNumber=new ROlong(getContainerServices()->getName()+":inputsNumber",getComponent(),
				new DevIOInputsNumber(m_commandLine),true); 
		m_pbins=new ROlongSeq(getContainerServices()->getName()+":bins",getComponent(),
				new DevIOBins(m_commandLine),true);
		m_pintegration=new ROlong(getContainerServices()->getName()+":integration",getComponent(),
				new DevIOIntegration(m_commandLine),false);
		m_pstatus=new ROpattern(getContainerServices()->getName()+":status",getComponent(),
				new DevIOStatus(m_commandLine),true);	
		m_pbusy=new ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>
		  (getContainerServices()->getName()+":busy",getComponent(), new DevIOBusy(m_commandLine),true);
		m_pfeed=new ROlongSeq(getContainerServices()->getName()+":feed",getComponent(),new DevIOFeed(m_commandLine),true);
		m_ptsys=new ROdoubleSeq(getContainerServices()->getName()+":systemTemperature",getComponent(),new DevIOTsys(m_commandLine),true);
		m_psectionsNumber=new ROlong(getContainerServices()->getName()+":sectionsNumber",getComponent(),
				new DevIOSectionsNumber(m_commandLine),true);
		m_pinputSection=new ROlongSeq(getContainerServices()->getName()+":inputSection",getComponent(),new DevIOInputSection(m_commandLine),true);
		m_parser=new CParser<CCommandLine>(m_commandLine,10); 
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"NoiseGeneratorImpl::initialize()");
		throw dummy;
	}
	// command parser configuration
	m_parser->add<1>("integration",new function1<CCommandLine,non_constant,void_type,I<long_type> >(m_commandLine,&CCommandLine::setIntegration) );
	m_parser->add<7>("setSection",new function7<CCommandLine,non_constant,void_type,I<long_type>,I<double_type>,I<double_type>,I<long_type>,I<enum_type<PolarizationToString> >,I<double_type>,I<long_type> >
			(m_commandLine,&CCommandLine::setConfiguration) );
	m_parser->add<2>("setAttenuation", new function2<CCommandLine,non_constant,void_type,I<long_type>,I<double_type> >(m_commandLine,&CCommandLine::setAttenuation) );
	m_parser->add<1>("enable",new function1<CCommandLine,non_constant,void_type,I<longSeq_type> >(m_commandLine,&CCommandLine::setEnabled) );
	m_parser->add<1>("getIntegration",new function1<CCommandLine,non_constant,void_type,O<long_type> >(m_commandLine,&CCommandLine::getIntegration) );
	m_parser->add<1>("getFrequency",new function1<CCommandLine,non_constant,void_type,O<doubleSeq_type> >(m_commandLine,&CCommandLine::getFrequency) );
	m_parser->add<1>("getSampleRate",new function1<CCommandLine,non_constant,void_type,O<doubleSeq_type> >(m_commandLine,&CCommandLine::getSampleRate) );
	m_parser->add<1>("getBins",new function1<CCommandLine,non_constant,void_type,O<longSeq_type> >(m_commandLine,&CCommandLine::getBins) );
	m_parser->add<1>("getPolarization",new function1<CCommandLine,non_constant,void_type,O<longSeq_type> >(m_commandLine,&CCommandLine::getPolarization) );
	m_parser->add<1>("getBandWidth",new function1<CCommandLine,non_constant,void_type,O<doubleSeq_type> >(m_commandLine,&CCommandLine::getBandWidth) );
	m_parser->add<1>("getAttenuation",new function1<CCommandLine,non_constant,void_type,O<doubleSeq_type> >(m_commandLine,&CCommandLine::getAttenuation) );
	m_parser->add<1>("getTime",new function1<CCommandLine,constant,void_type,O<time_type> >(m_commandLine,&CCommandLine::getTime) );
		
	threadPar.sender=this;
	threadPar.command=m_commandLine;
	threadPar.configuration=&m_configuration;
	try {
		CSenderThread::TSenderParameter *temp=&threadPar;
		m_senderThread=getContainerServices()->getThreadManager()->create<CSenderThread,CSenderThread::TSenderParameter*> (
				"DATAPUSHER",temp,m_configuration.getSenderResponseTime()*10,m_configuration.getSenderSleepTime()*10);
	}
	catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"NoiseGeneratorImpl::initialize()");
		throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"NoiseGeneratorImpl::initialize()");
	}
	m_initialized=true;
	ACS_LOG(LM_FULL_INFO,"NoiseGeneratorImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZED"));
}

void NoiseGeneratorImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("NoiseGeneratorImpl::execute()");
	ACS::Time time;
	ACS_LOG(LM_FULL_INFO,"NoiseGeneratorImpl::execute()",(LM_INFO,"BACKEND_INITIAL_CONFIGURATION"));
	try {
		//sets the property defaults....some of them cannot be changed any more (hardware dependent) 
		m_pbackendName->getDevIO()->write(getComponent()->getName(),time);
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,impl,ex,"NoiseGeneratorImpl::execute()");
		throw impl;
	}
	try {
		ACS_LOG(LM_FULL_INFO,"NoiseGeneratorImpl::execute()",(LM_INFO,"HARDWARE_INITIALIZATION"));
		m_commandLine->Init(&m_configuration);  	// this could throw an ACS exception.....
		m_commandLine->setTime(); // this could throw ACS exceptions....
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		deleteAll();
		throw ex;
	}
	//resume the threads 
	m_senderThread->resume();
	try {
		startPropertiesMonitoring();
	}
	catch (acsthreadErrType::CanNotStartThreadExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"NoiseGeneratorImpl::execute");
		deleteAll();
		throw __dummy;
	}
	catch (ACSErrTypeCommon::NullPointerExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"NoiseGeneratorImpl::execute");
		deleteAll();
		throw __dummy;		
	}
	ACS_LOG(LM_FULL_INFO,"NoiseGeneratorImpl::execute()",(LM_INFO,"COMPSTATE_OPERATIONAL"));
}

void NoiseGeneratorImpl::deleteAll()
{
	IRA::CError err;
	if (m_initialized) {
		if (m_senderThread!=NULL) {
			m_senderThread->suspend();
		}
		if (m_commandLine) {
			delete m_commandLine;
			m_commandLine=NULL;
		}
		if (m_senderThread!=NULL) {
			getContainerServices()->getThreadManager()->destroy(m_senderThread);
			m_senderThread=NULL;
		}
		if (m_parser) {
			delete m_parser;
			m_parser=NULL;
		}
		if (m_commandLine) {
			delete m_commandLine;
			m_commandLine=NULL;
		}
		m_initialized=false;
	}
}

void NoiseGeneratorImpl::cleanUp()
{
	AUTO_TRACE("NoiseGeneratorImpl::cleanUp()");
	stopPropertiesMonitoring();
	deleteAll();
	CharacteristicComponentImpl::cleanUp();	
}

void NoiseGeneratorImpl::aboutToAbort()
{
	AUTO_TRACE("NoiseGeneratorImpl::aboutToAbort()");
	deleteAll();
}

void NoiseGeneratorImpl::sendHeader() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NoiseGeneratorImpl::sendHeader()");
	//Backends::TMainHeader header;
	//Backends::TChannelHeader chHeader[MAX_INPUT_NUMBER];
	THeaderRecord buffer;
	ACS::doubleSeq tpi;
	try {
		m_commandLine->startDataAcquisition();
	}
	catch (BackendsErrors::BackendsErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getBackendsErrorsEx();		
	}
	m_commandLine->fillMainHeader(buffer.header);
	m_commandLine->fillChannelHeader(buffer.chHeader,MAX_SECTION_NUMBER);
	m_commandLine->setTime();
	try {
		getSender()->startSend(FLOW_NUMBER,(const char*)&buffer,
				sizeof(Backends::TMainHeader)+buffer.header.sections*sizeof(Backends::TSectionHeader));
		//getSender()->startSend(FLOW_NUMBER,(const char*)&header,sizeof(header));
	}
	catch (AVStartSendErrorExImpl& ex) {
		_ADD_BACKTRACE(BackendsErrors::TXErrorExImpl,impl,ex,"NoiseGeneratorImpl::sendHeader()");
		impl.setDetails("main header could not be sent");
		impl.log(LM_DEBUG);
		throw impl.getBackendsErrorsEx();
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"NoiseGeneratorImpl::sendHeader()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
	ACS_LOG(LM_FULL_INFO,"NoiseGeneratorImpl::sendHeader()",(LM_INFO,"HEADERS_SENT"));
	// inform the sender thread about the configuration......
	m_senderThread->saveDataHeader(&(buffer.header),(buffer.chHeader));
}

void NoiseGeneratorImpl::terminate() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,
		ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NoiseGeneratorImpl::terminate()");
	try {
		m_commandLine->stopDataAcquisition();
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void NoiseGeneratorImpl::sendData(ACS::Time startTime) throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,
		ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("NoiseGeneratorImpl::sendData()");
	TIMEVALUE now;
	try {
		m_commandLine->resumeDataAcquisition(startTime); 
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
	// if the resume succeeds....than set the start time accordingly.
	if (startTime!=0) {
		IRA::CIRATools::getTime(now);
		if (startTime<=now.value().value) {
			ACS_LOG(LM_FULL_INFO,"NoiseGeneratorImpl::sendData()",(LM_WARNING,"START_TIME_ALREADY_ELAPSED"));
		}
	}
	m_senderThread->saveStartTime(startTime);
	m_senderThread->resumeTransfer();
}

void NoiseGeneratorImpl::sendStop() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,
		ComponentErrors::ComponentErrorsEx)
{	
	AUTO_TRACE("NoiseGeneratorImpl::sendStop()");
	try {
		m_commandLine->suspendDataAcquisition(); 
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
	m_senderThread->suspendTransfer();
}


void NoiseGeneratorImpl::setSection(CORBA::Long input,CORBA::Double freq,CORBA::Double bw,CORBA::Long feed,CORBA::Long pol,CORBA::Double sr,CORBA::Long bins) throw (
				CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("NoiseGeneratorImpl::setSection()");
	try {
		m_commandLine->setConfiguration(input,freq,bw,feed,pol,sr,bins);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
	catch (BackendsErrors::BackendsErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getBackendsErrorsEx();		
	}	
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"NoiseGeneratorImpl::setSection()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}		
}

ACS::doubleSeq *NoiseGeneratorImpl::getTpi () throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("NoiseGeneratorImpl::getTpi()");
	ACS::doubleSeq_var tpi=new ACS::doubleSeq;
	try {
		m_commandLine->getSample(tpi,false);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
	catch (BackendsErrors::BackendsErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getBackendsErrorsEx();		
	}	
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"NoiseGeneratorImpl::getTpi()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}			
	return tpi._retn();
}

ACS::doubleSeq * NoiseGeneratorImpl::getZero () throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("NoiseGeneratorImpl::getZero()");
	ACS::doubleSeq_var tpi=new ACS::doubleSeq;
	try {
		m_commandLine->getSample(tpi,true);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
	catch (BackendsErrors::BackendsErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getBackendsErrorsEx();		
	}	
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"NoiseGeneratorImpl::getZero()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}		
	return tpi._retn();
}

void NoiseGeneratorImpl::setKelvinCountsRatio(const ACS::doubleSeq& ratio, const ACS::doubleSeq& tsys) throw (CORBA::SystemException)
{
	AUTO_TRACE("NoiseGeneratorImpl::setKelvinCountsRatio()");
	m_commandLine->saveTsys(tsys);
	if (m_senderThread) {
		m_senderThread->setKelvinCountsRatio(ratio);
		ACS_LOG(LM_FULL_INFO,"NoiseGeneratorImpl::setKelvinCountsRatio()",(LM_INFO,"KELVIN_COUNTS_CONVERSION_FACTOR_SET"));
	}
}

void NoiseGeneratorImpl::enableChannels(const ACS::longSeq& enable) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
		BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("NoiseGeneratorImpl::enableChannels()");
	try {
		m_commandLine->setEnabled(enable);
	}
	catch (BackendsErrors::BackendsErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getBackendsErrorsEx();		
	}	
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"NoiseGeneratorImpl::enableChannels()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}			
}

void NoiseGeneratorImpl::setTime() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
		BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("NoiseGeneratorImpl::setTime()");
	m_commandLine->setTime();
}

void NoiseGeneratorImpl::setAttenuation(CORBA::Long input,CORBA::Double att) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("NoiseGeneratorImpl::setAttenutation()");
	m_commandLine->setAttenuation(input,att);
}

CORBA::Long NoiseGeneratorImpl::getInputs(ACS::doubleSeq_out freq,ACS::doubleSeq_out bandWidth,ACS::longSeq_out feed,ACS::longSeq_out ifNumber) throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("NoiseGeneratorImpl::getInputs()");
	freq=new ACS::doubleSeq;
	bandWidth=new ACS::doubleSeq;
	feed=new ACS::longSeq;
	ifNumber=new ACS::longSeq;; //no need to take care of freeing allocated memory in case of exception because there are no except........
	return m_commandLine->getInputsConfiguration(*freq,*bandWidth,*feed,*ifNumber);	
}

void NoiseGeneratorImpl::setIntegration(CORBA::Long Integration) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
		BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("NoiseGeneratorImpl::setIntegration()");
	m_commandLine->setIntegration(Integration);
}

char * NoiseGeneratorImpl::command(const char *configCommand)  throw (CORBA::SystemException,ManagementErrors::CommandLineErrorEx)
{
	AUTO_TRACE("NoiseGeneratorImpl::command()");
	IRA::CString out;
	IRA::CString in;
	in=IRA::CString(configCommand);
	try {
		m_parser->run(in,out);
	}
	catch (ParserErrors::ParserErrorsExImpl &ex) {
		_ADD_BACKTRACE(ManagementErrors::CommandLineErrorExImpl,impl,ex,"NoiseGeneratorImpl::command()");
		impl.setCommand(configCommand);
		impl.setErrorMessage((const char *)out);
		impl.log(LM_DEBUG);
		throw impl.getCommandLineErrorEx();
	}
	return CORBA::string_dup((const char *)out);	
}

_PROPERTY_REFERENCE_CPP(NoiseGeneratorImpl,ACS::ROuLongLong,m_ptime,time);
_PROPERTY_REFERENCE_CPP(NoiseGeneratorImpl,ACS::ROstring,m_pbackendName,backendName);
_PROPERTY_REFERENCE_CPP(NoiseGeneratorImpl,ACS::ROdoubleSeq,m_pbandWidth,bandWidth);
_PROPERTY_REFERENCE_CPP(NoiseGeneratorImpl,ACS::ROdoubleSeq,m_pfrequency,frequency);
_PROPERTY_REFERENCE_CPP(NoiseGeneratorImpl,ACS::ROdoubleSeq,m_psampleRate,sampleRate);
_PROPERTY_REFERENCE_CPP(NoiseGeneratorImpl,ACS::ROdoubleSeq,m_pattenuation,attenuation);
_PROPERTY_REFERENCE_CPP(NoiseGeneratorImpl,ACS::ROlongSeq,m_ppolarization,polarization);
_PROPERTY_REFERENCE_CPP(NoiseGeneratorImpl,ACS::ROlongSeq,m_pbins,bins);
_PROPERTY_REFERENCE_CPP(NoiseGeneratorImpl,ACS::ROlong,m_pinputsNumber,inputsNumber);
_PROPERTY_REFERENCE_CPP(NoiseGeneratorImpl,ACS::ROlong,m_pintegration,integration);
_PROPERTY_REFERENCE_CPP(NoiseGeneratorImpl,ACS::ROpattern,m_pstatus,status);
_PROPERTY_REFERENCE_CPP(NoiseGeneratorImpl,Management::ROTBoolean,m_pbusy,busy);
_PROPERTY_REFERENCE_CPP(NoiseGeneratorImpl,ACS::ROlongSeq,m_pfeed,feed);
_PROPERTY_REFERENCE_CPP(NoiseGeneratorImpl,ACS::ROdoubleSeq,m_ptsys,systemTemperature);
_PROPERTY_REFERENCE_CPP(NoiseGeneratorImpl,ACS::ROlong,m_psectionsNumber,sectionsNumber);
_PROPERTY_REFERENCE_CPP(NoiseGeneratorImpl,ACS::ROlongSeq,m_pinputSection,inputSection);

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(NoiseGeneratorImpl)
