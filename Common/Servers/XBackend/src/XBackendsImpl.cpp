// $Id: XBackendsImpl.cpp,v 1.39 2010/08/26 13:29:34 bliliana Exp $

#include "XBackendsImpl.h"
#include <Definitions.h>
#include <ComponentErrors.h>
#include <LogFilter.h>
#include <baciDevIO.h>
#include <baciROstring.h>
#include "DevIOAttenuation.h"
#include "DevIOBandWidth.h"
#include "DevIOTime.h"
#include "DevIOFrequency.h"
#include "DevIOPolarization.h"
#include "DevIOInputsNumber.h"
#include "DevIOSampleRate.h"
#include "DevIOBins.h"
#include "DevIOFeed.h"
#include "DevIOIntegration.h"
#include "DevIOStatus.h"
#include "DevIOBusy.h"
#include "DevIOMode8bit.h"
#include "DevIOTsys.h"
#include "DevIOSectionsNumber.h"
#include "DevIOInputSection.h"
#include "Timer.h"

static char *rcsId="@(#) $Id: XBackendsImpl.cpp,v 1.39 2010/08/26 13:29:34 bliliana Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

using namespace SimpleParser;
_IRA_LOGFILTER_DECLARE;

XBackendsImpl::XBackendsImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) :
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
	m_psectionsNumber(this),
	m_pinputSection(this),
	m_pintegration(this),
	m_pstatus(this),
	m_pfeed(this),
	m_ptsys(this),
	m_pbusy(this),
	m_pmode8bit(this),
	m_pcontrolLoop(NULL)	
{
	AUTO_TRACE("XBackendsImpl::XBackendsImpl");	
	ACS_LOG(LM_FULL_INFO,"XBackendsImpl::XBackendsImpl()",(LM_INFO,"XBackendsImpl::COMPSTATE_Constructor"));
	m_fullName="alma/"+CompName;
	m_initialized=false;	
	m_GroupSpectrometer=NULL;
	m_commandLine=NULL;
	commandL=NULL;
	groupS=NULL;
	m_parser=NULL;
	m_pcontrolLoop=NULL;
}

XBackendsImpl::~XBackendsImpl() 
{
	AUTO_TRACE("XBackendsImp::~XBackendsImpl()");
	// if the initialization failed...clear everything as well. 
	// otherwise it is called by the cleanUp procedure.
	if (!m_initialized) deleteAll();
}

void XBackendsImpl::initialize() 
	throw (ACSErr::ACSbaseExImpl)
{
		
	AUTO_TRACE("XBackendsImpl::initialize()");
	ACS_LOG(LM_FULL_INFO,"XBackendsImpl::initialize()",(LM_INFO,"XBackendsImpl::COMPSTATE_INITIALIZING"));
	m_configuration.init(getContainerServices());
	ACS_LOG(LM_FULL_INFO,"XBackendsImpl::initialize()",(LM_INFO,"CONFIGURATION_OK"));
	IPAddress=m_configuration.getAddress();	
	if (IPAddress=="") {
		_THROW_EXCPT(ComponentErrors::SocketErrorExImpl,"XBackendsImpl::initialize()");
	}
	else {
		ACS_DEBUG_PARAM("XBackendsImpl::initialize()","IP Address: %s",(const char*)IPAddress); 
	}
	Port=m_configuration.getPort();
	if (Port==0) {
		Port=1234;
	}
	else {
		ACS_DEBUG_PARAM("XBackendsImpl::initialize()","Port: %lu",Port); 
	}
    Mode8Bit=m_configuration.getMode8Bit();
	try {
#ifdef DOPPIO 
		groupS=new GroupSpectrometer(IPAddress,Port,true);
#else 
	groupS=new GroupSpectrometer(IPAddress,Port,false);
#endif 
		m_GroupSpectrometer=new CSecureArea<GroupSpectrometer>(groupS);
		commandL=new CCommandLine(groupS);
		m_commandLine=new CSecureArea<CCommandLine>(commandL);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"XBackendsImpl::initialize()");
		throw dummy;
	}	
	ACS_LOG(LM_FULL_INFO,"XBackendsImpl::initialize()",(LM_INFO,"CONFIGURATION_OK"));

	ACS_LOG(LM_FULL_INFO,"XBackendsImpl::initialize()",(LM_INFO,"PROPERTY_CREATION"));
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
		m_psectionsNumber=new ROlong(getContainerServices()->getName()+":sectionsNumber",getComponent(),
				new DevIOSectionsNumber(m_commandLine),true); 
		m_pinputsNumber=new ROlong(getContainerServices()->getName()+":inputsNumber",getComponent(),
				new DevIOInputsNumber(m_commandLine),true); 
		m_pbins=new ROlongSeq(getContainerServices()->getName()+":bins",getComponent(),
				new DevIOBins(m_commandLine),true);
		m_pfeed=new ROlongSeq(getContainerServices()->getName()+":feed",getComponent(),
				new DevIOFeed(m_commandLine),true);
		m_ptsys=new ROdoubleSeq(getContainerServices()->getName()+":systemTemperature",
				getComponent(),new DevIOTsys(m_commandLine),true);
		m_pintegration=new ROlong(getContainerServices()->getName()+":integration",getComponent(),
				new DevIOIntegration(m_commandLine),false);
		m_pstatus=new ROpattern(getContainerServices()->getName()+":status",getComponent(),
				new DevIOStatus(m_commandLine),true);	
		m_pbusy=new ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>
		  (getContainerServices()->getName()+":busy",getComponent(), new DevIOBusy(m_commandLine),true);		
		m_pmode8bit=new ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>
		  (getContainerServices()->getName()+":mode8bit",getComponent(), new DevIOMode8bit(m_commandLine),true);		
		m_pinputSection=new ROlongSeq(getContainerServices()->getName()+":inputSection",getComponent(),
				new DevIOInputSection(m_commandLine),true);	
		m_parser=new SimpleParser::CParser<CCommandLine>(commandL,10);  
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"XBackendsImpl::initialize()");
		throw dummy;
	}
	// command parser configuration	
	//m_parser->add("setIntegration",new function1<CCommandLine,non_constant,void_type,I<long_type> >(commandL,&CCommandLine::setIntegration),1 );
	m_parser->add("integration",new function1<CCommandLine,non_constant,void_type,I<long_type> >(commandL,&CCommandLine::setIntegration),1 );
	m_parser->add("setSection",new function7<CCommandLine,non_constant,void_type,I<long_type>,I<double_type>,I<double_type>,I<long_type>,I<long_type>,I<double_type>,I<long_type> >
					(commandL,&CCommandLine::setSection),7 );
	m_parser->add("setAttenuation", new function2<CCommandLine,non_constant,void_type,I<long_type>,I<double_type> >(commandL,&CCommandLine::setAttenuation),2 );
	m_parser->add("enable",new function1<CCommandLine,non_constant,void_type,I<longSeq_type> >(commandL,&CCommandLine::setEnabled),1 );
	m_parser->add("getIntegration",new function1<CCommandLine,constant,void_type,O<long_type> >(commandL,&CCommandLine::getIntegration),0 );
	m_parser->add("getFrequency",new function1<CCommandLine,constant,void_type,O<doubleSeq_type> >(commandL,&CCommandLine::getFrequency),0 );
	m_parser->add("getSampleRate",new function1<CCommandLine,constant,void_type,O<doubleSeq_type> >(commandL,&CCommandLine::getSampleRate),0 );
	m_parser->add("getBins",new function1<CCommandLine,constant,void_type,O<longSeq_type> >(commandL,&CCommandLine::getBins),0 );
	m_parser->add("getPolarization",new function1<CCommandLine,constant,void_type,O<longSeq_type> >(commandL,&CCommandLine::getPolarization),0 );
	m_parser->add("getBandWidth",new function1<CCommandLine,constant,void_type,O<doubleSeq_type> >(commandL,&CCommandLine::getBandWidth),0 );
	m_parser->add("getAttenuation",new function1<CCommandLine,constant,void_type,O<doubleSeq_type> >(commandL,&CCommandLine::getAttenuation),0 );
	m_parser->add("getTime",new function1<CCommandLine,constant,void_type,O<time_type> >(commandL,&CCommandLine::getTime),0 );
	m_parser->add("initialize",new function1<CCommandLine,non_constant,void_type,I<string_type> >(commandL,&CCommandLine::setup),1 );
	
	m_initialized=true;
	ACS_LOG(LM_FULL_INFO,"XBackendsImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZED"));
}

void XBackendsImpl::execute() 
	throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("XBackendsImpl::execute()");		
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	XarcosThread::TSenderParameter threadPar;
	ACS::Time timeSleep=ACS::ThreadBase::defaultSleepTime;
	ACS::Time time=ACS::ThreadBase::defaultResponseTime;
	ACS_DEBUG_PARAM("XBackendsImpl::execute()","THREAD_STARTING","THREAD_STARTING");
	ACS_LOG(LM_FULL_INFO,"XBackendsImpl::execute()",(LM_INFO,"XBackendsImpl::CONTROL_THREAD_STARTING"));	
	try {
		if(m_pcontrolLoop==NULL) {
			threadPar.sender=this;
			threadPar.commandLine=m_commandLine;
			threadPar.command=commandL;
			threadPar.group=groupS;
			threadPar.groupSpectrometer=m_GroupSpectrometer;
			XarcosThread::TSenderParameter *temp=&threadPar;
			m_pcontrolLoop=getContainerServices()->getThreadManager()->create<XarcosThread,XarcosThread::TSenderParameter*>("XBackendsControl",temp,time,timeSleep);
//			m_pcontrolLoop->setSleepTime(timeSleep*10);
//			m_pcontrolLoop->setResponseTime(time*10);
			m_pcontrolLoop->resume();			
			line->setControlThread(m_pcontrolLoop);//Passiamo la gestione del thread di controllo a CommandLine

		}
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"XBackendsImpl::execute()");
	}
	ACS_LOG(LM_FULL_INFO,"XBackendsImpl::execute()",(LM_INFO,"XBackendsImpl::CONTROL_THREAD_STARTED"));	
	try {
		//sets the property defaults....some of them cannot be changed any more (hardware dependent) 
		m_pbackendName->getDevIO()->write(getComponent()->getName(),time);	
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,impl,ex,"XBackendsImpl::execute()");
		throw impl;
	}
//	_IRA_LOGFILTER_ACTIVATE(m_configuration.getRepetitionCacheTime(),m_configuration.getRepetitionExpireTime());
	//ACS_LOG(LM_FULL_INFO,"XBackendsImpl::execute()",(LM_INFO,"SOCKET_CONNECTING"));

    if (Mode8Bit == 0)
        line->setMode8BitParameter(false);
    if (Mode8Bit == 1)
        line->setMode8BitParameter(true);

	try {
		line->InitConf(&m_configuration);  	// this could throw an ACS exception.....
	}	
	catch (ComponentErrors::ValidationErrorExImpl& ex) {
		_THROW_EXCPT(ComponentErrors::ValidationErrorExImpl,"XBackendsImpl::execute()");
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"XBackendsImpl::execute()");
	}

	
	//Waits a bit so that everything can settle down
	IRA::CIRATools::Wait(0,200000);
	try{
		line->setDefaultConfiguration();//Configuro Specifiche nell'HW // this could throw ACS exceptions....
	}
	catch (XBackendsErrors::ErrorConfigurationExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"XBackendsImpl::enableChannels()");
		impl.setReason("error initialization");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}			
	ACS_LOG(LM_FULL_INFO,"XBackendsImpl::execute()",(LM_INFO,"HARWARE_INITIALIZATION..OK"));

	try {
		startPropertiesMonitoring();
		}
		catch (acsthreadErrType::CanNotStartThreadExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"XBackendsImpl::execute");
			throw __dummy;
		}
		catch (ACSErrTypeCommon::NullPointerExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"XBackendsImpl::execute");
			throw __dummy;		
		}
#ifdef DOPPIO
		ACS_LOG(LM_FULL_INFO,"XBackendsImpl::execute()",(LM_INFO,"XBackendsImpl::execute_double_mode"));
#else 
		ACS_LOG(LM_FULL_INFO,"XBackendsImpl::execute()",(LM_INFO,"XBackendsImpl::execute_single_mode"));	
#endif 
#ifdef BKD_DEBUG 
		ACS_LOG(LM_FULL_INFO,"XBackendsImpl::execute()",(LM_INFO,"XBackendsImpl::execute_debug_mode"));
#else 
		ACS_LOG(LM_FULL_INFO,"XBackendsImpl::execute()",(LM_INFO,"XBackendsImpl::execute_nodebug_mode"));
#endif 
	    ACS_LOG(LM_FULL_INFO,"XBackendsImpl::execute()",(LM_INFO,"XBackendsImpl::COMPSTATE_OPERATIONAL"));

}

void XBackendsImpl::deleteAll()
{
	IRA::CError err;
	AUTO_TRACE("XBackendsImpl::deleteAll()");
//	if (commandL) {
//		delete commandL;
//		commandL=NULL;
//	}	
//	if (groupS) {
//		delete groupS;
//		groupS=NULL;
//	}
	if (m_GroupSpectrometer) {
		delete m_GroupSpectrometer;
		m_GroupSpectrometer=NULL;
	}
	if (m_commandLine) {
		delete m_commandLine;
		m_commandLine=NULL;
	}		
	if (m_parser) {
		delete m_parser;
		m_parser=NULL;
	}
	if (m_pcontrolLoop!=NULL) {
		getContainerServices()->getThreadManager()->destroy(m_pcontrolLoop);
		m_pcontrolLoop=NULL;
	}

	//need to be closed before the thread that makes use of it
	ACS_LOG(LM_FULL_INFO,"XBackendsImpl::deleteAll()",(LM_INFO,"DATA_LINE_CLOSED"));
	_IRA_LOGFILTER_FLUSH;
	ACS_LOG(LM_FULL_INFO,"XBackendsImpl::deleteAll()",(LM_INFO,"LOGS_FLUSHED"));
}

void XBackendsImpl::cleanUp()
{
	AUTO_TRACE("XBackendsImpl::cleanUp()");
	stopPropertiesMonitoring();
	if (m_initialized) deleteAll();
	CharacteristicComponentImpl::cleanUp();	
}

void XBackendsImpl::aboutToAbort()
{
	AUTO_TRACE("XBackendsImpl::aboutToAbort()");
	if (m_initialized) deleteAll();
	m_initialized=false;
}

void XBackendsImpl::sendHeader() 
	throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("XBackendsImpl::sendHeader()");
//	Backends::TMainHeader header;
//	Backends::TSectionHeader chHeader[MAX_INPUT_NUMBER];
	THeaderRecord buffer;
	DWORD tpi[MAX_INPUT_NUMBER];
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
    /*
	try {
		line->startDataAcquisition();	
	}	
	catch (XBackendsErrors::NoSettingExImpl& ex) {
		_ADD_BACKTRACE(BackendsErrors::TXErrorExImpl,impl,ex,"XBackendsImpl::sendHeader()");
		impl.setDetails("Not setting HW");
		impl.log(LM_DEBUG);
		throw impl.getBackendsErrorsEx();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::sendHeader()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
    */
	ACS_DEBUG("XBackendsImpl::sendHeader()","Initialization");
	line->fillMainHeader(buffer.header);
	line->fillChannelHeader(buffer.chHeader,MAX_INPUT_NUMBER);
#ifndef BKD_DEBUG
	try {
		ACS_LOG(LM_FULL_INFO,"XBackendsImpl::sendHeader()",(LM_INFO,"HEADERS_SENT_OK"));
		getSender()->startSend(FLOW_NUMBER,(const char*)&buffer,
				sizeof(Backends::TMainHeader)+buffer.header.sections*sizeof(Backends::TSectionHeader));
	}
	catch (AVStartSendErrorExImpl& ex) {
		_ADD_BACKTRACE(BackendsErrors::TXErrorExImpl,impl,ex,"XBackendsImpl::sendHeader()");
		impl.setDetails("main header could not be sent");
		impl.log(LM_DEBUG);
		throw impl.getBackendsErrorsEx();
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"XBackendsImpl::sendHeader()");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
	}
#else
	printf("Chnls: %d beams: %d integration: %d sampleSize: %d\n",buffer.header.sections,buffer.header.beams,
			buffer.header.integration,buffer.header.sampleSize);
	for(int h=0;h<buffer.header.sections;h++) {
		printf("id: %d bins: %d pol: %d bw: %lf freq: %lf att L: %lf att R: %lf sr: %lf ifd: %d feed: %d\n",
				buffer.chHeader[h].id,
				buffer.chHeader[h].bins,
				buffer.chHeader[h].polarization,
				buffer.chHeader[h].bandWidth,
				buffer.chHeader[h].frequency,
				buffer.chHeader[h].attenuation[0],
				buffer.chHeader[h].attenuation[1],
				buffer.chHeader[h].sampleRate,
				buffer.chHeader[h].IF[0],
				buffer.chHeader[h].feed);
	}
#endif
	ACS_LOG(LM_FULL_INFO,"XBackendsImpl::sendHeader()",(LM_INFO,"HEADERS_SENT"));
	// inform the sender thread about the configuration......
	ACS_DEBUG_PARAM("XBackendsImpl::sendHeader()"," buffer.chHeader %lf ",buffer.chHeader);
	m_pcontrolLoop->saveDataHeader(&(buffer.header),(buffer.chHeader));	
#ifndef BKD_DEBUG
	// measure the zero tpi
	try {
		line->getZeroTPI(tpi);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::sendHeader()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
#else
	for(int i=0;i<MAX_INPUT_NUMBER;tpi[i]=0,i++);
#endif
	// now comunicate the reading to the sender thread.....
	m_pcontrolLoop->saveZero(tpi);
	// start the job for the backend.....
	try {
		line->startDataAcquisition();	
	}	
	catch (XBackendsErrors::NoSettingExImpl& ex) {
		_ADD_BACKTRACE(BackendsErrors::TXErrorExImpl,impl,ex,"XBackendsImpl::sendHeader()");
		impl.setDetails("Not setting HW");
		impl.log(LM_DEBUG);
		throw impl.getBackendsErrorsEx();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::sendHeader()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
}

void XBackendsImpl::terminate() 
	throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("XBackendsImpl::terminate()");
    //printf("XBackendsImpl::terminate()\n");
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::terminate()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
}

void XBackendsImpl::sendData(ACS::Time startTime) 
	throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("XBackendsImpl::sendData()");
    //printf("sendData()\n");
//	TIMEVALUE now;
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->resumeDataAcquisition(startTime); 
	}
	catch (XBackendsErrors::NoSettingExImpl& ex) {
		_ADD_BACKTRACE(BackendsErrors::TXErrorExImpl,impl,ex,"XBackendsImpl::sendData()");
		impl.setDetails("Not setting HW");
		impl.log(LM_DEBUG);
		throw impl.getBackendsErrorsEx();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::sendData()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
    //line.Release();
    //IRA::CIRATools::Wait(100000);
}

void XBackendsImpl::sendStop() 
	throw (CORBA::SystemException, BackendsErrors::BackendsErrorsEx,ComponentErrors::ComponentErrorsEx)
{	
    //printf("XBackendsImpl::sendStop()\n");
	AUTO_TRACE("XBackendsImpl::sendStop()");	
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
        //printf ("sendStop::before line->suspendDataAcquisition()\n");
		line->suspendDataAcquisition(); 
	}	
	catch (XBackendsErrors::NoSettingExImpl& ex) {
		_ADD_BACKTRACE(BackendsErrors::TXErrorExImpl,impl,ex,"XBackendsImpl::sendStop()");
		impl.setDetails("Not setting HW");
		impl.log(LM_DEBUG);
		throw impl.getBackendsErrorsEx();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::sendStop()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}	
#ifndef BKD_DEBUG
    //line.Release();
    bool active = true;
    DWORD backendStatus;
    while (active == true) {
        //line->checkBackend();
        line->getBackendStatus(backendStatus);
        if ((backendStatus & (1 << 2))) { // ACTIVE!!
            active = true;
            //printf ("active = %d\n", active);
        }
        else {
            active = false;
            //printf ("active = %d\n", active);
        }
        IRA::CIRATools::Wait(1,0);// TEST!!!!
    }
        
    //IRA::CIRATools::Wait(12,0);// TEST!!!!
    
	try {
		getSender()->stopSend(FLOW_NUMBER);
        //printf ("sendStop::getSender()->stopSend(FLOW_NUMBER)\n");
	}
	catch (AVStopSendErrorExImpl& ex) {
		_ADD_BACKTRACE(BackendsErrors::TXErrorExImpl,impl,ex,"XBackendsImpl::sendStop()");
		impl.setDetails("stop message could not be sent");
        _IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		//throw impl.getBackendsErrorsEx();
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"XBackendsImpl::sendStop()");
        _IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		//throw impl.getComponentErrorsEx();
	}
#endif
}

void XBackendsImpl::setSection(CORBA::Long input,CORBA::Double freq,CORBA::Double bw,
		CORBA::Long feed,CORBA::Long pol,CORBA::Double sr,CORBA::Long bins)
	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("XBackendsImpl::setSection()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {		
		line->setConfiguration(input,freq-ANALOG_FREQUENCY,bw,feed,pol,sr,bins);//Ricezione Specificha Nuova
		line->setAttenuation(input,-1);
		line->Init();//Configurazione nell'HW 
		line->getConfiguration();
	}	
	catch (XBackendsErrors::DisableChInExImpl& ex) {
		_ADD_BACKTRACE(BackendsErrors::TXErrorExImpl,impl,ex,"XBackendsImpl::setSection()");
		impl.setDetails("Not valide Channel input");
		impl.log(LM_DEBUG);
		throw impl.getBackendsErrorsEx();
	}
	catch (XBackendsErrors::NoSettingExImpl& ex) {
		_ADD_BACKTRACE(BackendsErrors::TXErrorExImpl,impl,ex,"XBackendsImpl::setSection()");
		impl.setDetails("Not setting HW");
		impl.log(LM_DEBUG);
		throw impl.getBackendsErrorsEx();
	}	
	catch (XBackendsErrors::ErrorConfigurationExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"XBackendsImpl::setSection()");
		impl.setReason("error setting new configuration ");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::setSection()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}
	if (line->m_XarcosC == true)
		line->setFeedC();		
}

ACS::doubleSeq *XBackendsImpl::getTpi ()
	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	ACS_DEBUG("XBackendsImpl::getTpi()"," ");
	AUTO_TRACE("XBackendsImpl::getTpi()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	ACS::doubleSeq_var tpiA=new ACS::doubleSeq;

	try {
		line->getTpZero(tpiA,false);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::getTpi()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}			
	return tpiA._retn();
}

ACS::doubleSeq * XBackendsImpl::getZero () 
	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("XBackendsImpl::getZero()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	ACS::doubleSeq_var tpiA=new ACS::doubleSeq;

	try {
		line->getTpZero(tpiA,true);
		
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::getZero()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}		
	return tpiA._retn();
}

void XBackendsImpl::enableChannels(const ACS::longSeq& enable) 
	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("XBackendsImpl::enableChannels()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->setEnabled(enable);
	}
	catch (XBackendsErrors::NoImplementedExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"XBackendsImpl::enableChannels()");
		impl.setReason("Command is not implemented");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::enableChannels()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}			
}

void XBackendsImpl::setTime()
	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("XBackendsImpl::setTime()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		;//line->setTime();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::setTime()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}		
}


void XBackendsImpl::initialize(const char * configuration) 
	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("XBackendsImpl::initialize()");
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::initialize()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}				
}

void XBackendsImpl::setIntegration(CORBA::Long Integration) 
	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("XBackendsImpl::setIntegration()");
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::setIntegration()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}		
}

CORBA::Boolean XBackendsImpl::command(const char *configCommand, CORBA::String_out answer) 
	throw (CORBA::SystemException)
{
	AUTO_TRACE("XBackendsImpl::command()");
	IRA::CString out;
	//IRA::CString in("");
	bool res;
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	//in=IRA::CString(configCommand);
	try {
		m_parser->run(configCommand,out);
		res = true;
	}
	catch (ParserErrors::ParserErrorsExImpl &ex) {
		/*_ADD_BACKTRACE(ManagementErrors::CommandLineErrorExImpl,impl,ex,"XBackendsImpl::command()");
		impl.setCommand(configCommand);
		impl.setErrorMessage((const char *)out);
		impl.log(LM_DEBUG);
		throw impl.getCommandLineErrorEx();*/
		res = false;
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		ex.log(LM_ERROR); // the errors resulting from the execution are logged here as stated in the documentation of CommandInterpreter interface, while the parser errors are never logged.
		res=false;
	}
	answer=CORBA::string_dup((const char *)out);
	return res;
	//return CORBA::string_dup((const char *)out);
}


void XBackendsImpl::setAttenuation(CORBA::Long input,CORBA::Double att) 
	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("XBackendsImpl::setAttenutation()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->setAttenuation(input,att);
	}	
	catch (XBackendsErrors::ErrorConfigurationExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"XBackendsImpl::setAttenutation()");
		impl.setReason("error setting new attenuation ");
		impl.log(LM_DEBUG);
		throw impl.getComponentErrorsEx();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::setAttenutation()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}		
}

CORBA::Long XBackendsImpl::getInputs(ACS::doubleSeq_out freq,ACS::doubleSeq_out bandWidth,
		ACS::longSeq_out feed,ACS::longSeq_out ifNumber/*Receivers::TPolarizationSeq_out polarization*/) 
	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("XBackendsImpl::getInputs()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	freq=new ACS::doubleSeq;
	bandWidth=new ACS::doubleSeq;
	feed=new ACS::longSeq;
	//polarization=new Receivers::TPolarizationSeq;
    ifNumber=new ACS::longSeq;
	return line->getInputsConfiguration(*freq,*bandWidth,*feed,*ifNumber/**polarization*/);
}

void XBackendsImpl::setInputsNumber(CORBA::Long inputsNumber) 
	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("XBackendsImpl::setSection()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->setInputsNumber(inputsNumber);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::setSection()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}		
}

void XBackendsImpl::setSectionsNumber(CORBA::Long sectionsNumber) 
	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("XBackendsImpl::setSection()");
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->setSectionsNumber(sectionsNumber);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::setSection()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}		
}

void XBackendsImpl::setKelvinCountsRatio(const ACS::doubleSeq& ratio, const ACS::doubleSeq& tsys) 
	throw (CORBA::SystemException)
{
	AUTO_TRACE("XBackendsImpl::setKelvinCountsRatio()");	
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	line->saveTsys(tsys);	
	if (m_pcontrolLoop) {
		m_pcontrolLoop->setKelvinCountsRatio(ratio);
		ACS_LOG(LM_FULL_INFO,"XBackendsImpl::setKelvinCountsRatio()",(LM_INFO,"KELVIN_COUNTS_CONVERSION_FACTOR_SET"));
	}
}

void XBackendsImpl::visualData() 
	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{	
	AUTO_TRACE("XBackendsImpl::VisualData()");

	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->VisualizzaDato();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::VisualData()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}		
	ACS_DEBUG("XBackendsImpl::VisualData()"," ");	
	
}

void XBackendsImpl::visualSpecific()
	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{

	AUTO_TRACE("XBackendsImpl::VisualSpecific()");

	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->VisualizzaSpecific();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::VisualSpecific()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}

	ACS_DEBUG("XBackendsImpl::VisualSpecific()"," ");	
}

void XBackendsImpl::visualACS()
	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{

	AUTO_TRACE("XBackendsImpl::VisualSpecific()");

	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->VisualizzaACS();
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::VisualSpecific()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}

	ACS_DEBUG("XBackendsImpl::VisualSpecific()"," ");	
}

void XBackendsImpl::setMode8bit(CORBA::Boolean mode)
 	throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
 {

	AUTO_TRACE("XBackendsImpl::setMode8bit()");

	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->setMode8bit(mode);
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
		_EXCPT(ComponentErrors::UnexpectedExImpl,dummy,"XBackendsImpl::VisualSpecific()");
		dummy.log(LM_DEBUG);
		throw dummy.getComponentErrorsEx();
	}

	ACS_DEBUG("XBackendsImpl::setMode8bit()"," ");	 
 }

void XBackendsImpl::setXarcosConf(Backends::TXArcosConf conf) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,BackendsErrors::BackendsErrorsEx)
{
	AUTO_TRACE("XBackendsImpl::setXarcosConf()");
	/*CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	try {
		line->InitConf(&m_configuration);  	// this could throw an ACS exception.....
	}	
	catch (ComponentErrors::ValidationErrorExImpl& ex) {
		_THROW_EXCPT(ComponentErrors::ValidationErrorExImpl,"XBackendsImpl::execute()");
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"XBackendsImpl::execute()");
	}	
	IRA::CIRATools::Wait(0,100000);
	*/
	CSecAreaResourceWrapper<CCommandLine> line=m_commandLine->Get();
	switch (conf) {
		case (Backends::XArcos_K77): // XK77, ALL FEED
			setMode8bit(false);
            setSectionsNumber(7);
			IRA::CIRATools::Wait(0,100000);
			setSection(0,145,62.5,0,2,125,-1);
			IRA::CIRATools::Wait(0,100000);
			setSection(1,145,62.5,1,2,125,-1);
			IRA::CIRATools::Wait(0,100000);
			setSection(2,145,62.5,2,2,125,-1);
			IRA::CIRATools::Wait(0,100000);
			setSection(3,145,62.5,3,2,125,-1);
			IRA::CIRATools::Wait(0,100000);
			setSection(4,145,62.5,4,2,125,-1);
			IRA::CIRATools::Wait(0,100000);
			setSection(5,145,62.5,5,2,125,-1);
			IRA::CIRATools::Wait(0,100000);
			setSection(6,145,62.5,6,2,125,-1);
			line->m_XarcosC=false;
			break;		
		case (Backends::XArcos_K01): // XK01, CENTRAL FEED & FEED 1
			setMode8bit(true);
			setSectionsNumber(4);
			IRA::CIRATools::Wait(0,100000);
			setSection(0,145,62.5,1,2,125,-1);
			IRA::CIRATools::Wait(0,100000);
			setSection(1,174.296875,3.90625,1,2,7.8125,-1);
			IRA::CIRATools::Wait(0,100000);
			setSection(2,145,62.5,2,2,125,-1);
			IRA::CIRATools::Wait(0,100000);
			setSection(3,174.296875,3.90625,2,2,7.8125,-1);
			line->m_XarcosC=false;
			break;
		case (Backends::XArcos_K04): // XK04, CENTRAL FEED & FEED 4
			setMode8bit(true);
			setSectionsNumber(4);
			IRA::CIRATools::Wait(0,100000);
			setSection(0,145,62.5,1,2,125,-1);
			IRA::CIRATools::Wait(0,100000);
			setSection(1,174.296875,3.90625,1,2,7.8125,-1);
			IRA::CIRATools::Wait(0,100000);
			setSection(2,145,62.5,3,2,125,-1);
			IRA::CIRATools::Wait(0,100000);
			setSection(3,174.296875,3.90625,3,2,7.8125,-1);
			IRA::CIRATools::Wait(0,100000);
			line->m_XarcosC=false;
			break;
		case (Backends::XArcos_K00): // XK00, CENTRAL FEED
			setMode8bit(true);
			setSectionsNumber(4);
			IRA::CIRATools::Wait(0,100000);
			setSection(0,145,62.5,1,2,125,-1);
			IRA::CIRATools::Wait(0,100000);
			setSection(1,172.34375,7.8125,1,2,15.625,-1);
			IRA::CIRATools::Wait(0,100000);
			setSection(2,175.2734375,1.953125,1,2,3.90625,-1);
			IRA::CIRATools::Wait(0,100000);
			setSection(3,176.005859375,0.48828125,1,2,0.9765625,-1);
			IRA::CIRATools::Wait(0,100000);
            line->m_XarcosC=false;
			break;
		case (Backends::XArcos_C00): // XC00
			setMode8bit(true);
			setSectionsNumber(4);
			IRA::CIRATools::Wait(0,100000);
			setSection(0,145,62.5,1,2,125,-1);
			IRA::CIRATools::Wait(0,100000);
			setSection(1,172.34375,7.8125,1,2,15.625,-1);
			IRA::CIRATools::Wait(0,100000);
			setSection(2,175.2734375,1.953125,1,2,3.90625,-1);
			IRA::CIRATools::Wait(0,100000);
			setSection(3,176.005859375,0.48828125,1,2,0.9765625,-1);
			IRA::CIRATools::Wait(0,100000);
		    line->setFeedC();
            line->m_XarcosC=true;
			break;
	}
}

_PROPERTY_REFERENCE_CPP(XBackendsImpl,ACS::ROuLongLong,m_ptime,time);
_PROPERTY_REFERENCE_CPP(XBackendsImpl,ACS::ROstring,m_pbackendName,backendName);
_PROPERTY_REFERENCE_CPP(XBackendsImpl,ACS::ROdoubleSeq,m_pbandWidth,bandWidth);
_PROPERTY_REFERENCE_CPP(XBackendsImpl,ACS::ROdoubleSeq,m_ptsys,systemTemperature);
_PROPERTY_REFERENCE_CPP(XBackendsImpl,ACS::ROdoubleSeq,m_pfrequency,frequency);
_PROPERTY_REFERENCE_CPP(XBackendsImpl,ACS::ROdoubleSeq,m_psampleRate,sampleRate);
_PROPERTY_REFERENCE_CPP(XBackendsImpl,ACS::ROdoubleSeq,m_pattenuation,attenuation);
_PROPERTY_REFERENCE_CPP(XBackendsImpl,ACS::ROlongSeq,m_ppolarization,polarization);
_PROPERTY_REFERENCE_CPP(XBackendsImpl,ACS::ROlongSeq,m_pbins,bins);
_PROPERTY_REFERENCE_CPP(XBackendsImpl,ACS::ROlongSeq,m_pfeed,feed);
_PROPERTY_REFERENCE_CPP(XBackendsImpl,ACS::ROlong,m_pinputsNumber,inputsNumber);
_PROPERTY_REFERENCE_CPP(XBackendsImpl,ACS::ROlong,m_psectionsNumber,sectionsNumber);
_PROPERTY_REFERENCE_CPP(XBackendsImpl,ACS::ROlong,m_pintegration,integration);
_PROPERTY_REFERENCE_CPP(XBackendsImpl,ACS::ROpattern,m_pstatus,status);
_PROPERTY_REFERENCE_CPP(XBackendsImpl,Management::ROTBoolean,m_pbusy,busy);
_PROPERTY_REFERENCE_CPP(XBackendsImpl,Management::ROTBoolean,m_pmode8bit,mode8bit);
_PROPERTY_REFERENCE_CPP(XBackendsImpl,ACS::ROlongSeq,m_pinputSection,inputSection);

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(XBackendsImpl)

