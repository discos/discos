#include "ExternalClientsSocketServer.h"

using namespace ComponentErrors;
using namespace ManagementErrors;

CExternalClientsSocketServer::CExternalClientsSocketServer(ContainerServices *service) : CSocket(), 
	m_services(service)
{
	AUTO_TRACE("CExternalClientsSocketServer::CExternalClientsSocketServer()");
	setExternalClientSocketStatus(ExternalClientSocketStatus_NOTCNTD);
    m_byebye = false;
    m_accept = false;
}

CExternalClientsSocketServer::~CExternalClientsSocketServer()
{
	AUTO_TRACE("CExternalClientsSocketServer::~CExternalClientsSocketServer()");
	setExternalClientSocketStatus(ExternalClientSocketStatus_NOTCNTD);
	Close(m_Error);
}

void CExternalClientsSocketServer::initialize(CConfiguration *config) throw (SocketErrorExImpl)
{
	AUTO_TRACE("CExternalClientsSocketServer::Init()");
	m_configuration = config;
	// Create server socket in blocking mode
	if (Create(m_Error,STREAM,m_configuration->Port(),&m_configuration->ipAddress()) == FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CExternalClientsSocketServer::Init()");
	}
    // Listen
	if (Listen(m_Error) == FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CExternalClientsSocketServer::Init()");
	}	
	else {
		setExternalClientSocketStatus(ExternalClientSocketStatus_CNTD);
	}
	// TO BE VERIFIED
	int Val=1000;
	if (setSockOption(m_Error,SO_RCVBUF,&Val,sizeof(int)) == FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CExternalClientsSocketServer::Init()");
	}
}

void CExternalClientsSocketServer::execute() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl)
{
    if (getStatus() == ExternalClientSocketStatus_CNTD) {
        //m_Scheduler = Management::Scheduler::_nil();
        CString sVisor = m_configuration->superVisor();
        if (sVisor.Compare("Gavino") == 0)
            strcpy (superVisorName, GAVINO);
        if (sVisor.Compare("Palmiro") == 0)
            strcpy (superVisorName, PALMIRO);

        try {
		    //m_Scheduler = m_services->getComponent<Management::Scheduler>(superVisorName);
	    }
        catch (maciErrType::CannotGetComponentExImpl& ex) {
		    _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CExternalClientsSocketServer::execute()");
		    Impl.setComponentName("MANAGEMENT/Gavino");
		    throw Impl;
	    }
	    ACS_LOG(LM_FULL_INFO,"CExternalClientsSocketServer::execute()",(LM_INFO,"CExternalClientsSocketServer::SCHEDULER_LOCATED"));
    }
}

void CExternalClientsSocketServer::cleanUp()
{
    try {
        //m_services->releaseComponent((const char*)m_Scheduler->name());
    }
    catch (maciErrType::CannotReleaseComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CExternalClientsSocketServer::cleanUp()");
		Impl.setComponentName((const char *)m_Scheduler->name());
		Impl.log(LM_DEBUG);
	}
}

void CExternalClientsSocketServer::cmdToScheduler()
{
    BYTE inBuffer[BUFFERSIZE];
    BYTE outBuffer[BUFFERSIZE];
    char * ret_val;
    IRA::CString out;
    int rBytes;
    WORD Len;
    OperationResult Res;

    if (m_accept == false) {
    // Accept
    OperationResult Res;
    Res = Accept(m_Error, newExternalClientsSocketServer);
    if (Res == FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
        _ADD_BACKTRACE(SocketErrorExImpl,_dummy,dummy,"CExternalClientsSocketServer::cmdToScheduler()");
        _dummy.log(LM_ERROR);
	}
    if (Res == WOULDBLOCK)
        setSockMode(m_Error,BLOCKING);
    }
    m_accept = true;

    rBytes = receiveBuffer(inBuffer,BUFFERSIZE);
    if (rBytes > 0 ) {
        try {
            //ret_val = m_Scheduler->command((const char*)inBuffer);
        }
        catch (ManagementErrors::CommandLineErrorExImpl& eX) {
            _EXCPT(ComponentErrors::OperationErrorExImpl,impl,"CExternalClientsSocketServer::cmdToScheduler()");
            impl.log(LM_ERROR);
            ret_val = "Command Error";
        }
        catch (CORBA::SystemException& ex) {
            _EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CExternalClientsSocketServer::cmdToScheduler()");
		    impl.setName(ex._name());
		    impl.setMinor(ex.minor());
		    impl.log(LM_ERROR);
            ret_val = "System Error";
        }
        catch (...) {
		    _EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CExternalClientsSocketServer::cmdToScheduler()");
		    impl.log(LM_ERROR);
            ret_val = "System Error";
	    }
        inBuffer[rBytes] = '\0';
        printf("Command received = %s\n", inBuffer);

        if (strncmp ((const char*)inBuffer, "Closing communication", 21) == 0)
            ret_val = "bye bye";

        out = IRA::CString(ret_val);
        Len = out.GetLength();
        int i;
        for (i = 0; i < Len; i++) {
            outBuffer[i] = out.CharAt(i);
        }
        outBuffer[Len]='\n';
        printf("Command returned = %s", outBuffer);
        Res = sendBuffer(outBuffer,Len+1);
        if (Res == WOULDBLOCK || Res == FAIL) {
            _EXCPT(SocketErrorExImpl,impl,"CExternalClientsSocketServer::cmdToScheduler()");
            impl.log(LM_ERROR);
        }
    }
    else { 
        newExternalClientsSocketServer.Close(m_Error);
        m_accept = false;
    }
}

// private methods

CExternalClientsSocketServer::OperationResult CExternalClientsSocketServer::sendBuffer(BYTE *Msg,WORD Len)
{
	int NWrite;
	int BytesSent;
	BytesSent=0;
	while (BytesSent<Len) {
		if ((NWrite = newExternalClientsSocketServer.Send(m_Error,(const void *)(Msg+BytesSent),Len-BytesSent))<0) {
			if (NWrite==WOULDBLOCK) {
				setExternalClientSocketStatus(ExternalClientSocketStatus_NOTCNTD);
				return WOULDBLOCK;
			}
			else {
				setExternalClientSocketStatus(ExternalClientSocketStatus_NOTCNTD);
				return FAIL;
			}
		}
		else { // success
			BytesSent+=NWrite;
		}
	}
	if (BytesSent==Len) {
		return SUCCESS;
   }
	else {
		_SET_ERROR(m_Error,CError::SocketType,CError::SendError,"CExternalClientsSocketServer::SendBuffer()");
		return FAIL;
	}
}

int CExternalClientsSocketServer::receiveBuffer(BYTE *Msg,WORD Len)
{
	int nRead;
	TIMEVALUE Now;
	TIMEVALUE Start;
	CIRATools::getTime(Start);
	while (true) {
		nRead = newExternalClientsSocketServer.Receive(m_Error,(void *)Msg,Len);
		if (nRead == WOULDBLOCK) {
			CIRATools::getTime(Now);
			if (CIRATools::timeDifference(Start,Now)>m_configuration->receiveTimeout()) {
				return WOULDBLOCK;
			}
			else {
				CIRATools::Wait(0,5000);
				continue;
			}
		}
		else if (nRead == FAIL) { 
			return nRead;
		}
		else if (nRead == 0) {
			return nRead;
		}
		else
            return nRead;
	}
}

void CExternalClientsSocketServer::setExternalClientSocketStatus(ExternalClientSocketStatus status)
{ 
	m_Status = status;
	if (m_Status == ExternalClientSocketStatus_BSY) m_bBusy=true;
	else if (m_Status == ExternalClientSocketStatus_CNTD) m_bBusy=false;
}

bool CExternalClientsSocketServer::isBusy() 
{
	return (m_bBusy);
}

CExternalClientsSocketServer::ExternalClientSocketStatus CExternalClientsSocketServer:: getStatus() const
{
    return m_Status;
}
