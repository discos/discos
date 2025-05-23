// $Id: SardaraImpl.cpp,v 1.1 2011-03-14 14:15:07 a.orlati Exp $

#include "SardaraImpl.h"
#include <Definitions.h>
#include <ComponentErrors.h>
#include <LogFilter.h>
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

static char *rcsId="@(#) $Id: SardaraImpl.cpp,v 1.1 2011-03-14 14:15:07 a.orlati Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

using namespace SimpleParser;

_IRA_LOGFILTER_DECLARE;

SardaraImpl::SardaraImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) :
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
	AUTO_TRACE("SardaraImpl::SardaraImpl");
	m_initialized=false;
	m_senderThread=NULL;
	m_controlThread=NULL;
}

SardaraImpl::~SardaraImpl() 
{
	AUTO_TRACE("SardaraImpl::~SardaraImpl()");
	// if the initialization failed...clear everything as well. 
	// otherwise it is called by the cleanUp procedure.
	if (!m_initialized) deleteAll();
}

void SardaraImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	CError Err;
	CSenderThread::TSenderParameter threadPar;
	CCommandLine* line;
	//IRA::CString hostName,hostAddress;
	AUTO_TRACE("SardaraImpl::initialize()");
	ACS_LOG(LM_FULL_INFO,"SardaraImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZING"));
	ACS_LOG(LM_FULL_INFO,"SardaraImpl::initialize()",(LM_INFO,"READING_CONFIGURATION"));
	// could throw an ComponentErrors::CDBAccessExImpl exception
	m_configuration.init(getContainerServices());
	ACS_LOG(LM_FULL_INFO,"SardaraImpl::initialize()",(LM_INFO,"CONFIGURATION_OK"));
	ACS_LOG(LM_FULL_INFO,"SardaraImpl::initialize()",(LM_INFO,"INITIALIIZING_COMMUNICATION_LINES"));
	_IRA_LOGFILTER_ACTIVATE(m_configuration.getRepetitionCacheTime(),m_configuration.getRepetitionExpireTime());
	try {
		line=new CCommandLine(getContainerServices());
		m_commandLine=new CSecureArea<CCommandLine>(line);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"SardaraImpl::initialize()");
		throw dummy;
	}
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
		m_parser=new CParser<CCommandLine>(line,10); 
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"SardaraImpl::initialize()");
		throw dummy;
	}
	// command parser configuration
	m_parser->add("integration",new function1<CCommandLine,non_constant,void_type,I<long_type> >(line,&CCommandLine::setIntegration),1);
	m_parser->add("calSwitch",new function1<CCommandLine,non_constant,void_type,I<long_type> >(line,&CCommandLine::activateCalSwitching),1 );
	m_parser->add("setSection", new function7<CCommandLine,non_constant,void_type,I<long_type>,I<double_type>,I<double_type>,I<long_type>,I<enum_type<PolarizationToString> >,I<double_type>,I<long_type> >(line,&CCommandLine::setConfiguration),7 );
	m_parser->add("setAttenuation", new function2<CCommandLine,non_constant,void_type,I<long_type>,I<double_type> >(line,&CCommandLine::setAttenuation),2 );
	m_parser->add("enable",new function1<CCommandLine,non_constant,void_type,I<longSeq_type> >(line,&CCommandLine::setEnabled),1 );
	m_parser->add("getIntegration",new function1<CCommandLine,constant,void_type,O<long_type> >(line,&CCommandLine::getIntegration),0 );
	m_parser->add("getFrequency",new function1<CCommandLine,constant,void_type,O<doubleSeq_type> >(line,&CCommandLine::getFrequency),0 );
	m_parser->add("getSampleRate",new function1<CCommandLine,constant,void_type,O<doubleSeq_type> >(line,&CCommandLine::getSampleRate),0 );
	m_parser->add("getBins",new function1<CCommandLine,constant,void_type,O<longSeq_type> >(line,&CCommandLine::getBins),0 );
	m_parser->add("getPolarization",new function1<CCommandLine,constant,void_type,O<longSeq_type> >(line,&CCommandLine::getPolarization),0 );
	m_parser->add("getBandWidth",new function1<CCommandLine,non_constant,void_type,O<doubleSeq_type> >(line,&CCommandLine::getBandWidth),0 );
	m_parser->add("getAttenuation",new function1<CCommandLine,non_constant,void_type,O<doubleSeq_type> >(line,&CCommandLine::getAttenuation),0 );
	m_parser->add("getTpi",new function1<CCommandLine,non_constant,void_type,O<doubleSeq_type> >(line,&CCommandLine::getTpi),0 );
	m_parser->add("getZero",new function1<CCommandLine,non_constant,void_type,O<doubleSeq_type> >(line,&CCommandLine::getZero),0 );
	m_parser->add("getTime",new function1<CCommandLine,non_constant,void_type,O<time_type> >(line,&CCommandLine::getTime),0 );
	m_parser->add("initialize",new function1<CCommandLine,non_constant,void_type,I<string_type> >(line,&CCommandLine::setup),1 );
	m_parser->add("getRms",new function1<CCommandLine,non_constant,void_type,O<doubleSeq_type> >(line,&CCommandLine::getRms),0 );
	m_parser->add("setTsysRange", new function2<CCommandLine,non_constant,void_type,I<double_type>,I<double_type> >(line,&CCommandLine::setTsysRange),2 );
	m_parser->add("backendPark", new function0<CCommandLine,non_constant, void_type>(line,&CCommandLine::backendPark),0 );
		
	threadPar.sender=this;
	threadPar.command=m_commandLine;
	threadPar.configuration=&m_configuration;
	//threadPar.dataLine=&m_dataLine;
	try {
		CSenderThread::TSenderParameter *temp=&threadPar;
		m_senderThread=getContainerServices()->getThreadManager()->create<CSenderThread,CSenderThread::TSenderParameter*> (
				"DATAPUSHER",temp,m_configuration.getSenderResponseTime()*10,m_configuration.getSenderSleepTime()*10);
		m_controlThread=getContainerServices()->getThreadManager()->create<CControlThread,CSecureArea<CCommandLine> * > (
				"SUPERVISOR",m_commandLine,m_configuration.getControlResponseTime()*10,m_configuration.getControlSleepTime()*10);
	}
	catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"SardaraImpl::initialize()");
		throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"SardaraImpl::initialize()");
	}
	m_initialized=true;
	ACS_LOG(LM_FULL_INFO,"SardaraImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZED"));
}

void SardaraImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("SardaraImpl::execute()");
	ACS::Time time;
	IRA::CError error;
	ACS_LOG(LM_FULL_INFO,"SardaraImpl::execute()",(LM_INFO,"BACKEND_INITIAL_CONFIGURATION"));
	try {
		//sets the property defaults....some of them cannot be changed any more (hardware dependent) 
		m_pbackendName->getDevIO()->write(getComponent()->getName(),time);
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,impl,ex,"SardaraImpl::execute()");
		throw impl;
	}	
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	ACS_LOG(LM_FULL_INFO,"SardaraImpl::execute()",(LM_INFO,"SOCKET_CONNECTING"));
	try {
		ACS_LOG(LM_FULL_INFO,"SardaraImpl::execute()",(LM_INFO,"HARDWARE_INITIALIZATION"));
		line->Init(&m_configuration);  	// this could throw an ACS exception.....
		//line->setTime(); // this could throw ACS exceptions....
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		deleteAll();
		throw ex;
	}
	//resume the threads 
	//m_senderThread->resume();
	//m_controlThread->resume();
	try {
		startPropertiesMonitoring();
	}
	catch (acsthreadErrType::CanNotStartThreadExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"SardaraImpl::execute");
		deleteAll();
		throw __dummy;
	}
	catch (ACSErrTypeCommon::NullPointerExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"SardaraImpl::execute");
		deleteAll();
		throw __dummy;		
	}
	ACS_LOG(LM_FULL_INFO,"SardaraImpl::execute()",(LM_INFO,"COMPSTATE_OPERATIONAL"));
}

void SardaraImpl::deleteAll()
{
	IRA::CError err;
	if (m_initialized) {
		if (m_senderThread!=NULL) {
			m_senderThread->suspend();
		}
		if (m_controlThread!=NULL) {
			m_controlThread->suspend();
		}	
		if (m_commandLine) {
			delete m_commandLine;
			m_commandLine=NULL;
		}
		if (m_senderThread!=NULL) {
			getContainerServices()->getThreadManager()->destroy(m_senderThread);
			m_senderThread=NULL;
		}
		if (m_controlThread!=NULL) {		
			getContainerServices()->getThreadManager()->destroy(m_controlThread);
			m_senderThread=NULL;
		}
		if (m_parser) {
			delete m_parser;
			m_parser=NULL;
		}	
		// need to be closed before the thread that makes use of it
		// m_dataLine.Close(err);
		// ACS_LOG(LM_FULL_INFO,"SardaraImpl::deleteAll()",(LM_INFO,"DATA_LINE_CLOSED"));
		_IRA_LOGFILTER_FLUSH;
		_IRA_LOGFILTER_DESTROY;
		ACS_LOG(LM_FULL_INFO,"SardaraImpl::deleteAll()",(LM_INFO,"LOGS_FLUSHED"));
		m_initialized=false;
	}
}

void SardaraImpl::cleanUp()
{
	AUTO_TRACE("SardaraImpl::cleanUp()");
	stopPropertiesMonitoring();
	deleteAll();
	CharacteristicComponentImpl::cleanUp();	
}

void SardaraImpl::aboutToAbort()
{
	AUTO_TRACE("SardaraImpl::aboutToAbort()");
	deleteAll();
}

void SardaraImpl::sendHeader() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx, 
		ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SardaraImpl::sendHeader()");
	Backends::TMainHeader header;
	Backends::TSectionHeader chHeader[MAX_SECTION_NUMBER];
	//DWORD tpi[MAX_SECTION_NUMBER];
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	line->fillMainHeader(header);
	line->fillChannelHeader(chHeader,MAX_SECTION_NUMBER);
	/*try {
		line->setTime();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::sendHeader()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
    */

    char buffer[sizeof(Backends::TMainHeader)+header.sections*sizeof(Backends::TSectionHeader)];

    memcpy(buffer,(void *)&header,sizeof(Backends::TMainHeader));
    memcpy(buffer+sizeof(Backends::TMainHeader),(void *)chHeader,header.sections*sizeof(Backends::TSectionHeader));

	#ifndef BKD_DEBUG
	try {
		getSender()->startSend(FLOW_NUMBER,(const char*)buffer,
				sizeof(Backends::TMainHeader)+header.sections*sizeof(Backends::TSectionHeader));
	}
	catch (AVStartSendErrorExImpl& ex) {
		_ADD_BACKTRACE(BackendsErrors::TXErrorExImpl,impl,ex,"SardaraImpl::sendHeader()");
		impl.setDetails("main header could not be sent");
		impl.log(LM_DEBUG);
		throw impl.getBackendsErrorsEx();
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SardaraImpl::sendHeader()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
	#else
	/*printf("Sects: %d beams: %d integration: %d sampleSize: %d\n",header.sections,header.beams,
			header.integration,header.sampleSize);
	for(int h=0;h<header.sections;h++) {
		printf("id: %d bins: %d pol: %d bw: %lf freq: %lf att L: %lf att R: %lf sr: %lf feed: %d\n",
				chHeader[h].id,
				chHeader[h].bins,
				chHeader[h].polarization,
				chHeader[h].bandWidth,
				chHeader[h].frequency,
				chHeader[h].attenuation[0],
				chHeader[h].attenuation[1],
				chHeader[h].sampleRate,
				chHeader[h].feed);
	}*/
	#endif
	ACS_LOG(LM_FULL_INFO,"SardaraImpl::sendHeader()",(LM_INFO,"HEADERS_SENT"));
	// inform the sender thread about the configuration......
	m_senderThread->saveDataHeader(&header,chHeader);
	// measure the zero tpi
	#ifndef BKD_DEBUG
	/*try {
		//line->getZeroTPI(tpi);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::sendHeader()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}*/
	#else
	//for(int i=0;i<MAX_INPUT_NUMBER;tpi[i]=0,i++);
	#endif
	// now comunicate the reading to the sender thread.....
	//m_senderThread->saveZero(tpi);
	// start the job for the backend.....
	try {
		line->startDataAcquisition();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::sendHeader()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
}

void SardaraImpl::terminate() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,
		ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SardaraImpl::terminate()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->stopDataAcquisition();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::terminate()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
}

void SardaraImpl::sendData(ACS::Time startTime) throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,
		ComponentErrors::ComponentErrorsEx)
{

	AUTO_TRACE("SardaraImpl::sendData()");

	TIMEVALUE now;
	ACS::Time expectedStartTime;
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		expectedStartTime=line->resumeDataAcquisition(startTime);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::sendData()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
	// if the resume succeeds....than set the start time accordingly.
	//I explicitly release the mutex before accessing the sender thread because it also make use of the command line...just to make sure to avoid deadlock
	line.Release();
	m_senderThread->saveStartTime(expectedStartTime);
	//m_senderThread->resumeTransfer();
}

void SardaraImpl::sendStop() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,
		ComponentErrors::ComponentErrorsEx)
{
	
	AUTO_TRACE("SardaraImpl::sendStop()");

	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->suspendDataAcquisition(); 
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::sendStop()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
	//I explicity release the mutex before accessing the sender thread because it also make use of the command line...just to make sure to avoid deadlock
	//line.Release();
	//m_senderThread->suspendTransfer();

	try {
		getSender()->stopSend(FLOW_NUMBER);
	}
	catch (AVStopSendErrorExImpl& ex) {
		_ADD_BACKTRACE(BackendsErrors::TXErrorExImpl,impl,ex,"SardaraImpl::sendStop()");
		impl.setDetails("stop message could not be sent");
		throw impl.getBackendsErrorsEx();
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"SardaraImpl::sendStop()");
		throw impl.getComponentErrorsEx();
	}

    try {
		line->sendTargetFileName(); 
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::sendStop()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
}

void SardaraImpl::setSection(CORBA::Long input,CORBA::Double freq,CORBA::Double bw,CORBA::Long feed,CORBA::Long pol,CORBA::Double sr,CORBA::Long bins) throw (
				CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("SardaraImpl::setSection()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->setConfiguration(input,freq,bw,feed,pol,sr,bins);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::setSection()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}		
}

void SardaraImpl::setTargetFileName (const char * fileName) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
		BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("SardaraImpl::setTargetFileName()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();

	line->setTargetFileName(fileName);
}

ACS::doubleSeq *SardaraImpl::getRms() throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("SardaraImpl::getRms()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	ACS::doubleSeq_var rms=new ACS::doubleSeq;
	try {
		line->getRms(rms);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::getRms()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}			
	return rms._retn();
}

ACS::doubleSeq *SardaraImpl::getTpi() throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("SardaraImpl::getTpi()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	ACS::doubleSeq_var tpi=new ACS::doubleSeq;
	try {
		line->getSample(tpi,false);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::getTpi()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}			
	return tpi._retn();
}

ACS::doubleSeq * SardaraImpl::getZero () throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("SardaraImpl::getZero()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	ACS::doubleSeq_var tpi=new ACS::doubleSeq;
    //TODO: how many sections????
    //tpi->length(2);
	try {
		line->getSample(tpi,true);
        //tpi[0]=0.0;
        //tpi[1]=0.0;
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::getZero()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}		
	return tpi._retn();
}

void SardaraImpl::setKelvinCountsRatio(const ACS::doubleSeq& ratio, const ACS::doubleSeq& tsys) throw (CORBA::SystemException)
{
	AUTO_TRACE("SardaraImpl::setKelvinCountsRatio()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	line->saveTsys(tsys,ratio);
	//if (m_senderThread) {
		//m_senderThread->setKelvinCountsRatio(ratio);
	//ACS_LOG(LM_FULL_INFO,"SardaraImpl::setKelvinCountsRatio()",(LM_INFO,"KELVIN_COUNTS_CONVERSION_FACTOR_SET"));
	//}
}

void SardaraImpl::enableChannels(const ACS::longSeq& enable) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
		BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("SardaraImpl::enableChannels()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->setEnabled(enable);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::enableChannels()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}			
}

void SardaraImpl::setTime() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
		BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("SardaraImpl::setTime()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->setTime();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::setTime()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}		
}

void SardaraImpl::setAttenuation(CORBA::Long input,CORBA::Double att) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("SardaraImpl::setAttenutation()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->setAttenuation(input,att);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::setAttenutation()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
}

void SardaraImpl::setTsysRange(CORBA::Double freq,CORBA::Double bw) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("SardaraImpl::setTsysRange()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->setTsysRange(freq,bw);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::setTsysRange()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
}

CORBA::Long SardaraImpl::getInputs(ACS::doubleSeq_out freq,ACS::doubleSeq_out bandWidth,ACS::longSeq_out feed,ACS::longSeq_out ifNumber) throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	long inputs;
	ACS::longSeq pol;
	AUTO_TRACE("SardaraImpl::getInputs()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	//no need to take care of freeing allocated memory in case of exception because there are no except........
	freq=new ACS::doubleSeq;
	bandWidth=new ACS::doubleSeq;
	feed=new ACS::longSeq;
	ifNumber=new ACS::longSeq;
	line->getFeed(*feed);
	line->getFrequency(*freq);
	line->getInputsNumber(inputs);
	line->getBandWidth(*bandWidth);
	line->getIFs(*ifNumber);
	return inputs;
}


void SardaraImpl::activateNoiseCalibrationSwitching(CORBA::Long interleave) throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("SardaraImpl::activateNoiseCalibrationSwitching()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->activateCalSwitching(interleave); // NOT YET AVAILABLE
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::activateNoiseCalibrationSwitching()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
}

void SardaraImpl::initialize(const char * configuration) throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("SardaraImpl::initialize()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->setup(configuration);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::initialize()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}				
}

void SardaraImpl::setIntegration(CORBA::Long Integration) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
		BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("SardaraImpl::setIntegration()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->setIntegration(Integration);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::setIntegration()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}		
}

char* SardaraImpl::getConfiguration() throw (CORBA::SystemException)
{
	AUTO_TRACE("SardaraImpl::getIntegration()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		return line->getConfiguration();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::getConfiguration()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}	
}

char* SardaraImpl::getCommProtVersion() throw (CORBA::SystemException)
{
	AUTO_TRACE("SardaraImpl::getCommProtVersion()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		return line->getCommProtVersion();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"SardaraImpl::getCommProtVersion()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}	
}

CORBA::Boolean SardaraImpl::command(const char *cmd,CORBA::String_out answer) throw (CORBA::SystemException)
{
	AUTO_TRACE("SardaraImpl::command()");
	IRA::CString out;
	bool res;
	// this is a cheat....it forces the acquisition of the mutex before the parser. It works only because the parser has no async behaviour  configured
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
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

_PROPERTY_REFERENCE_CPP(SardaraImpl,ACS::ROuLongLong,m_ptime,time);
_PROPERTY_REFERENCE_CPP(SardaraImpl,ACS::ROstring,m_pbackendName,backendName);
_PROPERTY_REFERENCE_CPP(SardaraImpl,ACS::ROdoubleSeq,m_pbandWidth,bandWidth);
_PROPERTY_REFERENCE_CPP(SardaraImpl,ACS::ROdoubleSeq,m_pfrequency,frequency);
_PROPERTY_REFERENCE_CPP(SardaraImpl,ACS::ROdoubleSeq,m_psampleRate,sampleRate);
_PROPERTY_REFERENCE_CPP(SardaraImpl,ACS::ROdoubleSeq,m_pattenuation,attenuation);
_PROPERTY_REFERENCE_CPP(SardaraImpl,ACS::ROlongSeq,m_ppolarization,polarization);
_PROPERTY_REFERENCE_CPP(SardaraImpl,ACS::ROlongSeq,m_pbins,bins);
_PROPERTY_REFERENCE_CPP(SardaraImpl,ACS::ROlong,m_pinputsNumber,inputsNumber);
_PROPERTY_REFERENCE_CPP(SardaraImpl,ACS::ROlong,m_pintegration,integration);
_PROPERTY_REFERENCE_CPP(SardaraImpl,ACS::ROpattern,m_pstatus,status);
_PROPERTY_REFERENCE_CPP(SardaraImpl,Management::ROTBoolean,m_pbusy,busy);
_PROPERTY_REFERENCE_CPP(SardaraImpl,ACS::ROlongSeq,m_pfeed,feed);
_PROPERTY_REFERENCE_CPP(SardaraImpl,ACS::ROdoubleSeq,m_ptsys,systemTemperature);
_PROPERTY_REFERENCE_CPP(SardaraImpl,ACS::ROlong,m_psectionsNumber,sectionsNumber);
_PROPERTY_REFERENCE_CPP(SardaraImpl,ACS::ROlongSeq,m_pinputSection,inputSection);

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SardaraImpl)
