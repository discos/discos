// $Id: RoachImpl.cpp,v 1.1 2011-03-14 14:15:07 a.orlati Exp $

#include "RoachImpl.h"
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

static char *rcsId="@(#) $Id: RoachImpl.cpp,v 1.1 2011-03-14 14:15:07 a.orlati Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

using namespace SimpleParser;

_IRA_LOGFILTER_DECLARE;

RoachImpl::RoachImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) :
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
	AUTO_TRACE("RoachImpl::RoachImpl");
	m_initialized=false;
	m_senderThread=NULL;
	m_controlThread=NULL;
}

RoachImpl::~RoachImpl() 
{
	AUTO_TRACE("RoachImpl::~RoachImpl()");
	// if the initialization failed...clear everything as well. 
	// otherwise it is called by the cleanUp procedure.
	if (!m_initialized) deleteAll();
}

void RoachImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	CError Err;
	CSenderThread::TSenderParameter threadPar;
	CCommandLine* line;
	//IRA::CString hostName,hostAddress;
	AUTO_TRACE("RoachImpl::initialize()");
	ACS_LOG(LM_FULL_INFO,"RoachImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZING"));
	ACS_LOG(LM_FULL_INFO,"RoachImpl::initialize()",(LM_INFO,"READING_CONFIGURATION"));
	// could throw an ComponentErrors::CDBAccessExImpl exception
	m_configuration.init(getContainerServices());
	ACS_LOG(LM_FULL_INFO,"RoachImpl::initialize()",(LM_INFO,"CONFIGURATION_OK"));
	ACS_LOG(LM_FULL_INFO,"RoachImpl::initialize()",(LM_INFO,"INITIALIIZING_COMMUNICATION_LINES"));
	_IRA_LOGFILTER_ACTIVATE(m_configuration.getRepetitionCacheTime(),m_configuration.getRepetitionExpireTime());
	try {
		line=new CCommandLine();
		m_commandLine=new CSecureArea<CCommandLine>(line);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"RoachImpl::initialize()");
		throw dummy;
	}
	/* hostAddress=m_configuration.getDataIPAddress();
	if (hostAddress=="") {
		if ((hostName=IRA::CSocket::getHostName())=="") {
			_THROW_EXCPT(ComponentErrors::SocketErrorExImpl,"RoachImpl::initialize()");
		}
		else {
			ACS_DEBUG_PARAM("RoachImpl::initialize()","Name of the host: %s",(const char *)hostName);
		}
		if ((hostAddress=IRA::CSocket::getAddrbyHostName(hostName))=="") {
			_THROW_EXCPT(ComponentErrors::SocketErrorExImpl,"RoachImpl::initialize()");
		}
		else {
			ACS_DEBUG_PARAM("RoachImpl::initialize()","Address of the host: %s",(const char *)hostAddress);
		}
	}
	else {
		ACS_DEBUG_PARAM("RoachImpl::initialize()","Address of the host: %s",(const char *)hostAddress);
	}
	if (m_dataLine.Create(Err,IRA::CSocket::STREAM,m_configuration.getDataPort(),&hostAddress)!=IRA::CSocket::SUCCESS) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,Err);
		dummy.setCode(Err.getErrorCode());
		dummy.setDescription((const char*)Err.getDescription());
		throw dummy;
	}
	// sets the socket in blocking mode
	if (m_dataLine.setSockMode(Err,IRA::CSocket::NONBLOCKING)!=IRA::CSocket::SUCCESS) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,Err);
		dummy.setCode(Err.getErrorCode());
		dummy.setDescription((const char*)Err.getDescription());		
		throw dummy;
	}
	// set up the listening queue to 1
	if (m_dataLine.Listen(Err,1)!=IRA::CSocket::SUCCESS) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,Err);
		dummy.setCode(Err.getErrorCode());
		dummy.setDescription((const char*)Err.getDescription());
		throw dummy;
	}
	ACS_LOG(LM_FULL_INFO,"RoachImpl::initialize()",(LM_INFO,"COMMUNICATION_LINE_DONE"));
	ACS_LOG(LM_FULL_INFO,"RoachImpl::initialize()",(LM_INFO,"PROPERTY_CREATION"));
	*/
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
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"RoachImpl::initialize()");
		throw dummy;
	}
	// command parser configuration
	m_parser->add("integration",new function1<CCommandLine,non_constant,void_type,I<long_type> >(line,&CCommandLine::setIntegration),1);
	m_parser->add("calSwitch",new function1<CCommandLine,non_constant,void_type,I<long_type> >(line,&CCommandLine::activateCalSwitching),1 );
	m_parser->add("setSection",
			new function7<CCommandLine,non_constant,void_type,I<long_type>,I<double_type>,I<double_type>,I<long_type>,I<enum_type<PolarizationToString> >,I<double_type>,I<long_type> >
			(line,&CCommandLine::setConfiguration),7 );
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
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"RoachImpl::initialize()");
		throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"RoachImpl::initialize()");
	}
	m_initialized=true;
	ACS_LOG(LM_FULL_INFO,"RoachImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZED"));
}

void RoachImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("RoachImpl::execute()");
	ACS::Time time;
	IRA::CError error;
	ACS_LOG(LM_FULL_INFO,"RoachImpl::execute()",(LM_INFO,"BACKEND_INITIAL_CONFIGURATION"));
	try {
		//sets the property defaults....some of them cannot be changed any more (hardware dependent) 
		m_pbackendName->getDevIO()->write(getComponent()->getName(),time);
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,impl,ex,"RoachImpl::execute()");
		throw impl;
	}	
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	ACS_LOG(LM_FULL_INFO,"RoachImpl::execute()",(LM_INFO,"SOCKET_CONNECTING"));
	try {
		ACS_LOG(LM_FULL_INFO,"RoachImpl::execute()",(LM_INFO,"HARDWARE_INITIALIZATION"));
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
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"RoachImpl::execute");
		deleteAll();
		throw __dummy;
	}
	catch (ACSErrTypeCommon::NullPointerExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"RoachImpl::execute");
		deleteAll();
		throw __dummy;		
	}
	ACS_LOG(LM_FULL_INFO,"RoachImpl::execute()",(LM_INFO,"COMPSTATE_OPERATIONAL"));
}

void RoachImpl::deleteAll()
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
		// ACS_LOG(LM_FULL_INFO,"RoachImpl::deleteAll()",(LM_INFO,"DATA_LINE_CLOSED"));
		_IRA_LOGFILTER_FLUSH;
		_IRA_LOGFILTER_DESTROY;
		ACS_LOG(LM_FULL_INFO,"RoachImpl::deleteAll()",(LM_INFO,"LOGS_FLUSHED"));
		m_initialized=false;
	}
}

void RoachImpl::cleanUp()
{
	AUTO_TRACE("RoachImpl::cleanUp()");
	stopPropertiesMonitoring();
	deleteAll();
	CharacteristicComponentImpl::cleanUp();	
}

void RoachImpl::aboutToAbort()
{
	AUTO_TRACE("RoachImpl::aboutToAbort()");
	deleteAll();
}

void RoachImpl::sendHeader() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx, 
		ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("RoachImpl::sendHeader()");
	//Backends::TMainHeader header;
	//Backends::TChannelHeader chHeader[MAX_INPUT_NUMBER];
	THeaderRecord buffer;
	//DWORD tpi[MAX_SECTION_NUMBER];
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	line->fillMainHeader(buffer.header);
	line->fillChannelHeader(buffer.chHeader,MAX_SECTION_NUMBER);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::sendHeader()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
    */
	#ifndef BKD_DEBUG
	try {
		getSender()->startSend(FLOW_NUMBER,(const char*)&buffer,
				sizeof(Backends::TMainHeader)+buffer.header.sections*sizeof(Backends::TSectionHeader));
	}
	catch (AVStartSendErrorExImpl& ex) {
		_ADD_BACKTRACE(BackendsErrors::TXErrorExImpl,impl,ex,"RoachImpl::sendHeader()");
		impl.setDetails("main header could not be sent");
		impl.log(LM_DEBUG);
		throw impl.getBackendsErrorsEx();
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"RoachImpl::sendHeader()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
	#else
	printf("Sects: %d beams: %d integration: %d sampleSize: %d\n",buffer.header.sections,buffer.header.beams,
			buffer.header.integration,buffer.header.sampleSize);
	for(int h=0;h<buffer.header.sections;h++) {
		printf("id: %d bins: %d pol: %d bw: %lf freq: %lf att L: %lf att R: %lf sr: %lf feed: %d\n",
				buffer.chHeader[h].id,
				buffer.chHeader[h].bins,
				buffer.chHeader[h].polarization,
				buffer.chHeader[h].bandWidth,
				buffer.chHeader[h].frequency,
				buffer.chHeader[h].attenuation[0],
				buffer.chHeader[h].attenuation[1],
				buffer.chHeader[h].sampleRate,
				buffer.chHeader[h].feed);
	}
	#endif
	ACS_LOG(LM_FULL_INFO,"RoachImpl::sendHeader()",(LM_INFO,"HEADERS_SENT"));
	// inform the sender thread about the configuration......
	m_senderThread->saveDataHeader(&(buffer.header),(buffer.chHeader));
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::sendHeader()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}*/
	#else
	for(int i=0;i<MAX_INPUT_NUMBER;tpi[i]=0,i++);
	#endif
	// now comunicate the reading to the sender thread.....
	//m_senderThread->saveZero(tpi);
	// start the job for the backend.....
	/*try {
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::sendHeader()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}*/
}

void RoachImpl::terminate() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,
		ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("RoachImpl::terminate()");
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::terminate()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
}

void RoachImpl::sendData(ACS::Time startTime) throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,
		ComponentErrors::ComponentErrorsEx)
{

	AUTO_TRACE("RoachImpl::sendData()");

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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::sendData()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
	// if the resume succeeds....than set the start time accordingly.
	//I explicitly release the mutex before accessing the sender thread because it also make use of the command line...just to make sure to avoid deadlock
	line.Release();
	m_senderThread->saveStartTime(expectedStartTime);
	//m_senderThread->resumeTransfer();
}

void RoachImpl::sendStop() throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,
		ComponentErrors::ComponentErrorsEx)
{
	
	AUTO_TRACE("RoachImpl::sendStop()");

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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::sendStop()");
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
		_ADD_BACKTRACE(BackendsErrors::TXErrorExImpl,impl,ex,"RoachImpl::sendStop()");
		impl.setDetails("stop message could not be sent");
		throw impl.getBackendsErrorsEx();
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"RoachImpl::sendStop()");
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::sendStop()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
}

/*void RoachImpl::setAllSections(CORBA::Double freq,CORBA::Double bw,CORBA::Long feed,Backends::TPolarization pol,CORBA::Double sr,CORBA::Long bins) throw (
				CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("RoachImpl::setAllSections()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->setConfiguration(-1,freq,bw,sr,att,pol,bins);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::setAllSections()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}		
}*/


void RoachImpl::setSection(CORBA::Long input,CORBA::Double freq,CORBA::Double bw,CORBA::Long feed,CORBA::Long pol,CORBA::Double sr,CORBA::Long bins) throw (
				CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("RoachImpl::setSection()");
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::setSection()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}		
}

void RoachImpl::setTargetFileName (const char * fileName) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
		BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("RoachImpl::setTargetFileName()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();

	line->setTargetFileName(fileName);

    /*try {
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::setSection()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}*/

}

ACS::doubleSeq *RoachImpl::getTpi() throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("RoachImpl::getTpi()");
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::getTpi()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}			
	return tpi._retn();
}

ACS::doubleSeq * RoachImpl::getZero () throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("RoachImpl::getZero()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	ACS::doubleSeq_var tpi=new ACS::doubleSeq;
	try {
		line->getSample(tpi,true);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::getZero()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}		
	return tpi._retn();
}

void RoachImpl::setKelvinCountsRatio(const ACS::doubleSeq& ratio, const ACS::doubleSeq& tsys) throw (CORBA::SystemException)
{
	AUTO_TRACE("RoachImpl::setKelvinCountsRatio()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	line->saveTsys(tsys,ratio);
	//if (m_senderThread) {
		//m_senderThread->setKelvinCountsRatio(ratio);
	//ACS_LOG(LM_FULL_INFO,"RoachImpl::setKelvinCountsRatio()",(LM_INFO,"KELVIN_COUNTS_CONVERSION_FACTOR_SET"));
	//}
}

void RoachImpl::enableChannels(const ACS::longSeq& enable) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
		BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("RoachImpl::enableChannels()");
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::enableChannels()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}			
}

void RoachImpl::setTime() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
		BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("RoachImpl::setTime()");
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::setTime()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}		
}

void RoachImpl::setAttenuation(CORBA::Long input,CORBA::Double att) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("RoachImpl::setAttenutation()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	/*try {
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::setAttenutation()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}*/		
}

CORBA::Long RoachImpl::getInputs(ACS::doubleSeq_out freq,ACS::doubleSeq_out bandWidth,ACS::longSeq_out feed,ACS::longSeq_out ifNumber) throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	long inputs;
	ACS::longSeq pol;
	AUTO_TRACE("RoachImpl::getInputs()");
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


void RoachImpl::activateNoiseCalibrationSwitching(CORBA::Long interleave) throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("RoachImpl::activateNoiseCalibrationSwitching()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	/*try {
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::activateNoiseCalibrationSwitching()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}*/			
}

void RoachImpl::initialize(const char * configuration) throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("RoachImpl::initialize()");
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::initialize()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}				
}

void RoachImpl::setIntegration(CORBA::Long Integration) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,
		BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("RoachImpl::setIntegration()");
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::setIntegration()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}		
}

void RoachImpl::getConfiguration (CORBA::String_out configuration) throw (CORBA::SystemException)
{
	AUTO_TRACE("RoachImpl::getIntegration()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->getConfiguration(configuration);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::getConfiguration()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}	
}

void RoachImpl::getCommProtVersion (CORBA::String_out version) throw (CORBA::SystemException)
{
	AUTO_TRACE("RoachImpl::getCommProtVersion()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->getCommProtVersion(version);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"RoachImpl::getCommProtVersion()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}	
}

CORBA::Boolean RoachImpl::command(const char *cmd,CORBA::String_out answer) throw (CORBA::SystemException)
{
	AUTO_TRACE("RoachImpl::command()");
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

_PROPERTY_REFERENCE_CPP(RoachImpl,ACS::ROuLongLong,m_ptime,time);
_PROPERTY_REFERENCE_CPP(RoachImpl,ACS::ROstring,m_pbackendName,backendName);
_PROPERTY_REFERENCE_CPP(RoachImpl,ACS::ROdoubleSeq,m_pbandWidth,bandWidth);
_PROPERTY_REFERENCE_CPP(RoachImpl,ACS::ROdoubleSeq,m_pfrequency,frequency);
_PROPERTY_REFERENCE_CPP(RoachImpl,ACS::ROdoubleSeq,m_psampleRate,sampleRate);
_PROPERTY_REFERENCE_CPP(RoachImpl,ACS::ROdoubleSeq,m_pattenuation,attenuation);
_PROPERTY_REFERENCE_CPP(RoachImpl,ACS::ROlongSeq,m_ppolarization,polarization);
_PROPERTY_REFERENCE_CPP(RoachImpl,ACS::ROlongSeq,m_pbins,bins);
_PROPERTY_REFERENCE_CPP(RoachImpl,ACS::ROlong,m_pinputsNumber,inputsNumber);
_PROPERTY_REFERENCE_CPP(RoachImpl,ACS::ROlong,m_pintegration,integration);
_PROPERTY_REFERENCE_CPP(RoachImpl,ACS::ROpattern,m_pstatus,status);
_PROPERTY_REFERENCE_CPP(RoachImpl,Management::ROTBoolean,m_pbusy,busy);
_PROPERTY_REFERENCE_CPP(RoachImpl,ACS::ROlongSeq,m_pfeed,feed);
_PROPERTY_REFERENCE_CPP(RoachImpl,ACS::ROdoubleSeq,m_ptsys,systemTemperature);
_PROPERTY_REFERENCE_CPP(RoachImpl,ACS::ROlong,m_psectionsNumber,sectionsNumber);
_PROPERTY_REFERENCE_CPP(RoachImpl,ACS::ROlongSeq,m_pinputSection,inputSection);

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(RoachImpl)
