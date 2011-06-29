#include <lanImpl.h>
//#include <maciContainerImpl.h>

/*******************************************************************************
*
* "@(#) $Id: lanImpl.cpp,v 1.1 2011-03-24 09:16:42 c.migoni Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* GMM       jul 2005   creation				   
*/

lanImpl::lanImpl(const ACE_CString& _name,maci::ContainerServices* containerServices) : CharacteristicComponentImpl(_name,containerServices),
 m_delay_sp(new RWdouble(_name+":delay", getComponent()),this),
 m_status_sp(new ROlong(_name+":status", getComponent()),this)

{
	ACS_TRACE("::lanImpl::lanImpl: constructor;Constructor!");
}

void lanImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{	
	ACS_TRACE("::lanImpl::lanImpl: initialize()");
	
	DWORD dtemp;
	IRA::CString IP;
	WORD port=0;
	int err=0;
	
	
	if (CIRATools::getDBValue(getContainerServices(),"IPAddress",IP)) {
		if (CIRATools::getDBValue(getContainerServices(),"port",dtemp)) {
			port=dtemp;
		}
	} else {
	    ASErrors::CDBAccessErrorExImpl exImpl(__FILE__,__LINE__,"lanImpl::initialize()");
	    throw acsErrTypeLifeCycle::LifeCycleExImpl(exImpl,__FILE__,__LINE__,"lanImpl::initialize()");
	    
	}
	ACS_DEBUG_PARAM("::lanImpl::lanImpl","IP Address: %s",(const char*)IP);
	m_psock = new lanSocket();
	err = m_psock->Init(IP, port);
	if (err) {
		ACS_SHORT_LOG((LM_ERROR,"Error opening socket on %s port %d",(const char*)IP,port));
		ASErrors::LibrarySocketErrorExImpl exImpl(__FILE__,__LINE__,"lanImpl::initialize()");
		throw acsErrTypeLifeCycle::LifeCycleExImpl(exImpl,__FILE__,__LINE__,"lanImpl::initialize()");
	}
	ACS_DEBUG("::lanImpl::lanImpl","new socket opened and connected!");
    m_sock = new CSecureArea<lanSocket>(m_psock);
	m_sock->Init(m_psock);	// initialize the protector
}

void lanImpl::execute() throw (ACSErr::ACSbaseExImpl)
{	
	ACS_TRACE("::lanImpl::lanImpl: execute()");
}

void lanImpl::cleanUp()
{
    if (m_sock) {
        delete m_sock;
        m_sock = NULL;
    }
}

void lanImpl::aboutToAbort()
{
    if (m_sock) {
        delete m_sock;
        m_sock = NULL;
    }
}

lanImpl::~lanImpl()
{
	ACS_TRACE("::lanImpl::~lanImpl");
	ACS_DEBUG_PARAM("::lanImpl::~lanImpl", "Destroying %s...", name());
}


ACSErr::Completion*  lanImpl::sendUSDCmd(CORBA::Long cmd,CORBA::Long addr,CORBA::Long param,CORBA::Long nBytes)   throw (CORBA::SystemException)
{
	ACS_TRACE("::lanImpl::sendUSDCmd()");
	int resp;
	CompletionImpl* comp;

	CSecAreaResourceWrapper<lanSocket> psock = m_sock->Get();
		
	if (psock->m_soStat == lanSocket::READY) {

		resp = psock->sendCmd(cmd,addr,param,nBytes);
		ACS_DEBUG_PARAM("::lanImpl::sendUSDCmd()","resp:%d",resp);
		if (resp==0 ) {  	// unrecoverable socket error
			comp= new ASErrors::SocketReconnCompletion(__FILE__,__LINE__,"::lanImpl::sendUSDCmd");
		} else if(resp==-1) {   	 // USD tout
		    comp=new  ASErrors::USDTimeoutCompletion(__FILE__,__LINE__,"::lanImpl::sendUSDCmd");
		} else if(resp==-2) {   	 // socket tout
		    comp=new  ASErrors::SocketTOutCompletion(__FILE__,__LINE__,"::lanImpl::sendUSDCmd");
		} else if(resp==-3) {  		//unrecoverable socket problems
			comp=new  ASErrors::SocketFailCompletion(__FILE__,__LINE__,"::lanImpl::sendUSDCmd");
		} else if(resp==-4) {  		//incomplete reply
			comp=new  ASErrors::InvalidResponseCompletion(__FILE__,__LINE__,"::lanImpl::sendUSDCmd");
		} else if(resp==NAK) {  	//NAK
			comp=new  ASErrors::NakCompletion(__FILE__,__LINE__,"::lanImpl::sendUSDCmd");
		} else if(resp==ACK) {
			comp=new  ACSErrTypeOK::ACSErrOKCompletion();
		} else  comp=new  ASErrors::InvalidResponseCompletion(__FILE__,__LINE__,"::lanImpl::sendUSDCmd");
		
	} else {        //  socket notready
	    comp=new  ASErrors::SocketNotRdyCompletion(__FILE__,__LINE__,"::lanImpl::sendUSDCmd");
	}
	return comp->returnCompletion();
}

void lanImpl::recUSDPar(CORBA::Long cmd,CORBA::Long addr,CORBA::Long nBytes,CORBA::Long& param)  throw(CORBA::SystemException, ASErrors::ASErrorsEx)
{
	ACS_TRACE("::lanImpl::recUSDPar()");
	
	int resp,rlen=nBytes+3;
	BYTE buff[BUFFERSIZE];
	param=0;

	CSecAreaResourceWrapper<lanSocket> psock = m_sock->Get();	
    
	if (psock->m_soStat == lanSocket::READY) {
		
		resp = psock->sendCmd(cmd,addr);
	
		if(resp ==0 ) {     //socket problems. reconnected
			throw ASErrors::SocketReconnExImpl(__FILE__,__LINE__,"::lanImpl::recUSDPar").getASErrorsEx();
		} else if(resp==-1) {    // USD tout
		    throw ASErrors::USDTimeoutExImpl(__FILE__,__LINE__,"::lanImpl::recUSDPar").getASErrorsEx();
		} else if(resp==-2) {   // socket tout
		    throw ASErrors::SocketTOutExImpl(__FILE__,__LINE__,"::lanImpl::recUSDPar").getASErrorsEx();
		} else if(resp==-3) {  // unrecoverable socket problems
			throw ASErrors::SocketFailExImpl(__FILE__,__LINE__,"::lanImpl::recUSDPar").getASErrorsEx();
		} else if(resp==-4) {  		//inavild reply
			throw ASErrors::InvalidResponseExImpl(__FILE__,__LINE__,"::lanImpl::recUSDPar").getASErrorsEx();
		} else if(resp==NAK) {  		
			throw ASErrors::NakExImpl(__FILE__,__LINE__,"::lanImpl::recUSDPar").getASErrorsEx();
		} else if(resp==ACK) {
			
			resp = psock->receiveBuffer(buff,rlen);
			if(resp == 0) {
				throw ASErrors::SocketReconnExImpl(__FILE__,__LINE__,"::lanImpl::recUSDPar").getASErrorsEx();
			} else if(resp == -1) {
				throw ASErrors::USDTimeoutExImpl(__FILE__,__LINE__,"::lanImpl::recUSDPar").getASErrorsEx();
			} else if(resp == -2) {
				throw ASErrors::SocketTOutExImpl(__FILE__,__LINE__,"::lanImpl::recUSDPar").getASErrorsEx();
			} else if(resp == -3) {
				throw ASErrors::SocketFailExImpl(__FILE__,__LINE__,"::lanImpl::recUSDPar").getASErrorsEx();
			} else if(resp == -4) {
				throw ASErrors::IncompleteExImpl(__FILE__,__LINE__,"::lanImpl::recUSDPar").getASErrorsEx();
			} else {
				BYTE* pvalue=(BYTE*)&param;
				for(int i=nBytes+1; i > 1;i--) *pvalue++=buff[i]; // reverse the reply  
			}  
		} else {       // NAK or others
			throw ASErrors::sendCmdErrExImpl(__FILE__,__LINE__,"::lanImpl::recUSDPar").getASErrorsEx();
		}
	} else {
	  	throw ASErrors::SocketNotRdyExImpl(__FILE__,__LINE__,"::lanImpl::recUSDPar").getASErrorsEx();
	}
}


/* --------------------- [ CORBA stuff ] ----------------------*/

ACS::RWdouble_ptr lanImpl::delay() throw (CORBA::SystemException)
{
	if(m_delay_sp==0) return ACS::RWdouble::_nil();
	ACS::RWdouble_var prop=ACS::RWdouble::_narrow(m_delay_sp->getCORBAReference());
	return prop._retn();
}



ACS::ROlong_ptr lanImpl::status() throw (CORBA::SystemException)
{
	if(m_status_sp==0) return ACS::ROlong::_nil();
	ACS::ROlong_var prop=ACS::ROlong::_narrow(m_status_sp->getCORBAReference());
	return prop._retn();
}


/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(lanImpl)
/* ----------------------------------------------------------------*/


/*___oOo___*/


