#include <new>
#include <baciDB.h>
#include <ManagmentDefinitionsS.h>
#include "ExternalClientsImpl.h"
#include "DevIOStatus.h"

#define GET_PROPERTY_REFERENCE(TYPE,PROPERTY,PROPERTYNAME) TYPE##_ptr ExternalClientsImpl::PROPERTYNAME() throw (CORBA::SystemException) \
{ \
	if (PROPERTY==0) return TYPE::_nil(); \
	TYPE##_var tmp=TYPE::_narrow(PROPERTY->getCORBAReference()); \
	return tmp._retn(); \
}
	
using namespace ComponentErrors;

static char *rcsId="@(#) $Id: ExternalClientsImpl.cpp,v 1.4 2010-10-04 11:39:04 c.migoni Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

ExternalClientsImpl::ExternalClientsImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) : 
	CharacteristicComponentImpl(CompName,containerServices),
	m_pStatus(this),
  	m_ExternalClientsSocketServer(NULL)
{	
	AUTO_TRACE("ExternalClientsImpl::ExternalClientsImpl()");
    m_pExternalClientsThread = NULL;
}

ExternalClientsImpl::~ExternalClientsImpl()
{
	AUTO_TRACE("ExternalClientsImpl::~ExternalClientsImpl()");
}

void ExternalClientsImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("ExternalClientsImpl::initialize()");
	ACS_LOG(LM_FULL_INFO,"ExternalClientsImpl::initialize()",(LM_INFO,"ExternalClients::READING_CONFIGURATION"));
    try {
        m_CompConfiguration.Init(getContainerServices());
    }
    catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,_dummy,E,"SchedulerImpl::initialize()");
		throw _dummy;
	}
	ACS_LOG(LM_FULL_INFO,"ExternalClients::initialize()",(LM_INFO,"ExternalClients::CONFIGURATION_DONE"));	

    ACS_LOG(LM_FULL_INFO,"ExternalClientsImpl::initialize()",(LM_INFO,"ExternalClients::SOCKETSERVER_CREATING"));
	CExternalClientsSocketServer *ExternalClientsSocketServer = NULL;
	ACS_LOG(LM_FULL_INFO,"ExternalClientsImpl::initialize()",(LM_INFO,"ExternalClients::COMPSTATE_INITIALIZING"));
	try {
		ExternalClientsSocketServer = (CExternalClientsSocketServer*)new CExternalClientsSocketServer(getContainerServices());
		m_ExternalClientsSocketServer = new CSecureArea<CExternalClientsSocketServer>(ExternalClientsSocketServer);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(MemoryAllocationExImpl,dummy,"ExternalClientsImpl::initialize()");
		throw dummy;
	}
    ACS_LOG(LM_FULL_INFO,"ExternalClientsImpl::initialize()",(LM_INFO,"ExternalClients::SOCKETSERVER_CREATED"));

    ExternalClientsSocketServer->initialize(&m_CompConfiguration);
    m_ControlThreadPeriod = m_CompConfiguration.controlThreadPeriod();

    ACS_LOG(LM_FULL_INFO,"ExternalClientsImpl::initialize()",(LM_INFO,"ExternalClients::CONTROL_THREAD_CREATING"));
    try {
		if(m_pExternalClientsThread == NULL) {
			m_pExternalClientsThread = getContainerServices()->getThreadManager()->create<CExternalClientsThread,CSecureArea<CExternalClientsSocketServer> *>("ExternalClientsThread",m_ExternalClientsSocketServer);
		}
	}
    catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"ExternalClients::execute()");
		throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"ExternalClientsImpl::execute()");
	}

	try {		
		m_pStatus=new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus>(getContainerServices()->getName()+":status",getComponent(),
		  new DevIOStatus(ExternalClientsSocketServer),true);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(MemoryAllocationExImpl,dummy,"ExternalClientsImpl::initialize()");
		throw dummy;
	}
	ACS_LOG(LM_FULL_INFO,"ExternalClientsImpl::initialize()",(LM_INFO,"ExternalClients::COMPSTATE_INITIALIZED"));
}

void ExternalClientsImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("ExternalClientsImpl::execute()");
	CSecAreaResourceWrapper<CExternalClientsSocketServer> SocketServer = m_ExternalClientsSocketServer->Get();
    try {
        SocketServer->execute();
    }
    catch  (ACSErr::ACSbaseExImpl& E) {
        _ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,_dummy,E,"Refraction::execute()");
		throw _dummy;
	}

	ACS_LOG(LM_FULL_INFO,"ExternalClientsImpl::execute()",(LM_INFO,"ExternalClients::CONTROL_THREAD_STARTING"));	
	try {
		if(m_pExternalClientsThread != NULL) {
			m_pExternalClientsThread->setSleepTime(m_ControlThreadPeriod);
			m_pExternalClientsThread->resume();
	    }
	}
    catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
	    _ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"ExternalClients::execute()");
	    throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"ExternalClientsImpl::execute()");
	}
	ACS_LOG(LM_FULL_INFO,"ExternalClientsImpl::execute()",(LM_INFO,"ExternalClients::CONTROL_THREAD_STARTED"));

	ACS_LOG(LM_FULL_INFO,"ExternalClientsImpl::execute()",(LM_INFO,"ExternalClients::COMPSTATE_OPERATIONAL"));
}

void ExternalClientsImpl::cleanUp()
{
    system("$INTROOT/bin/closeServer &");
    ACE_OS::sleep(3);
    system("killall $INTROOT/bin/closeServer");
	AUTO_TRACE("ExternalClientsImpl::cleanUp()");
    CSecAreaResourceWrapper<CExternalClientsSocketServer> SocketServer = m_ExternalClientsSocketServer->Get();
    SocketServer->byebye();
	if (m_pExternalClientsThread != NULL)
        m_pExternalClientsThread->suspend();
    getContainerServices()->getThreadManager()->destroy(m_pExternalClientsThread);
    ACS_LOG(LM_FULL_INFO,"ExternalClientsImpl::cleanUp()",(LM_INFO,"ExternalClients::CONTROL_THREAD_TERMINATED"));
    SocketServer->cleanUp();
    ACS_LOG(LM_FULL_INFO,"ExternalClientsImpl::cleanUp()",(LM_INFO,"ExternalClients::SOCKET_SERVER_TERMINATED"));

	if (m_ExternalClientsSocketServer) {
		delete m_ExternalClientsSocketServer;
		m_ExternalClientsSocketServer = NULL;
	}
	ACS_LOG(LM_FULL_INFO,"ExternalClientsImpl::cleanUp()",(LM_INFO,"ExternalClients::SOCKETSERVER_CLOSED"));

	CharacteristicComponentImpl::cleanUp();	
}

void ExternalClientsImpl::aboutToAbort()
{
	AUTO_TRACE("ExternalClientsImpl::aboutToAbort()");
    CSecAreaResourceWrapper<CExternalClientsSocketServer> SocketServer = m_ExternalClientsSocketServer->Get();
    SocketServer->byebye();
	if (m_pExternalClientsThread != NULL)
        m_pExternalClientsThread->suspend();
    getContainerServices()->getThreadManager()->destroy(m_pExternalClientsThread);
    ACS_LOG(LM_FULL_INFO,"ExternalClientsImpl::cleanUp()",(LM_INFO,"ExternalClients::CONTROL_THREAD_TERMINATED"));
    SocketServer->cleanUp();
    ACS_LOG(LM_FULL_INFO,"ExternalClientsImpl::cleanUp()",(LM_INFO,"ExternalClients::SOCKET_SERVER_TERMINATED"));

	if (m_ExternalClientsSocketServer) {
		delete m_ExternalClientsSocketServer;
		m_ExternalClientsSocketServer = NULL;
	}		
}

char * ExternalClientsImpl::command (const char * cmd) throw (CORBA::SystemException,ManagementErrors::CommandLineErrorEx)
{
    return (char*)cmd;
}

GET_PROPERTY_REFERENCE(Management::ROTSystemStatus,m_pStatus,status);

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(ExternalClientsImpl)
