// $Id: Socket.cpp,v 1.3 2009-10-30 15:13:05 a.orlati Exp $

#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <sys/time.h>
#include <new>
#include <sys/poll.h>
#include "IRA"

using namespace IRA;

CSocket::CSocket() 
{
	fastInit();
}

CSocket::~CSocket()
{
	CError Tmp;
	Close(Tmp);
}

CSocket::OperationResult CSocket::Create(CError& Err,SocketType Type,WORD SocketPort,CString *IPAddr)
{
	struct sockaddr_in addr;
	if (!Err.isNoError()) return FAIL;
	// convert address from dotted, human readable format to a network byte order 32 bit integer
	if (IPAddr==NULL) {  // use the address of the local host.
		addr.sin_addr.s_addr=htonl(INADDR_ANY);
	}	
	else { 
		if (inet_aton((const char *)(*IPAddr),&(addr.sin_addr))==0) {
			_SET_ERROR(Err,CError::SocketType,CError::InvalidIPAddr,"CSocket::Create()");
			return FAIL;
		}
	}
	// store address family
	addr.sin_family=AF_INET;
	// now specify the port
	addr.sin_port=htons(SocketPort);
	memset(&(addr.sin_zero),0,8);
	// socket creation
	if (Type==STREAM) {
		m_iSocket=socket(PF_INET,SOCK_STREAM,0);
	}
	else if (Type==DGRAM) {
		m_iSocket=socket(PF_INET,SOCK_DGRAM,0);
	}
	if (m_iSocket<0) {
		_SET_SYSTEM_ERROR(Err,CError::SocketType,CError::SockCreationError,"CSocket::Create()",errno);
		return FAIL;
	}
	// this associates a local address to the socket
	if ((SocketPort!=0) || (IPAddr!=NULL)) {
		if (bind(m_iSocket,(struct sockaddr*)&addr,sizeof(struct sockaddr))<0) {
			_SET_SYSTEM_ERROR(Err,CError::SocketType,CError::SockBindError,"CSocket::Create()",errno);
			return FAIL;
		}
	}
	// Activate the Framework
	try {
		m_pHelper=(CSocketFramework *)new CSocketFramework(this);
	}
	catch (std::bad_alloc& ex) {
		m_pHelper=NULL;
		_SET_ERROR(Err,CError::SocketType,CError::SockCreationError,"CSocket::Create()");
		return FAIL;
	}
	// set socket type
	setType(Type);
	// set socket in ready state.
	setStatus(READY);
	// set socket in BLOCKING MODE
	return setSockMode(Err,BLOCKING);
}

CSocket::OperationResult CSocket::Listen(CError& Err,int Backlog)
{
	if (!Err.isNoError()) return FAIL;
	if (getStatus()==NOTCREATED) {
		_SET_ERROR(Err,CError::SocketType,CError::SocketNCreated,"CSocket::Listen()");
		return FAIL;
	}
	if (getType()==STREAM) {
		if (listen(m_iSocket,Backlog)<0) {
			_SET_SYSTEM_ERROR(Err,CError::SocketType,CError::ListenError,"CSocket::Listen()",errno);
			return FAIL;
		}
		if ((m_wAsyncFlag&E_ACCEPT)==E_ACCEPT) setStatus(ACCEPTING);
		return SUCCESS;
	}	
	else {
		_SET_ERROR(Err,CError::SocketType,CError::OperationNPermitted,"CSocket::Listen()");
		return FAIL;
	}
}

CSocket::OperationResult CSocket::Accept(CError& Err,CSocket &ConnectedSocket)
{
	struct sockaddr_in addr;
	int Res;
	if (!Err.isNoError()) return FAIL;
	if (getStatus()==NOTCREATED) {
		_SET_ERROR(Err,CError::SocketType,CError::SocketNCreated,"CSocket::Accept()");
		return FAIL;
	}
	socklen_t size=sizeof(struct sockaddr);
	if (getType()==STREAM) {
		Res=accept(this->m_iSocket,(struct sockaddr *)&addr,&size);
		if (Res<0) {  // Error 
			if (((errno==EAGAIN) || (errno==EWOULDBLOCK)) && (getMode()==NONBLOCKING)) {
				return WOULDBLOCK;
			}
			else {
				_SET_SYSTEM_ERROR(Err,CError::SocketType,CError::AcceptError,"CSocket::Accept()",errno);
				return FAIL;
			}
		}
		else {
			// Activate the Framework
			try {
				ConnectedSocket.m_pHelper=(CSocketFramework *)new CSocketFramework(&ConnectedSocket);
			}
			catch (std::bad_alloc& ex) {
				ConnectedSocket.m_pHelper=NULL;
				_SET_ERROR(Err,CError::SocketType,CError::SockCreationError,"CSocket::Accept()");
				return FAIL;
			}
			ConnectedSocket.m_iSocket=Res;
			ConnectedSocket.setStatus(READY);
			ConnectedSocket.setSockMode(Err,this->getMode());
			ConnectedSocket.setType(this->getType());
			return SUCCESS;
		}
	}
	else {
		_SET_ERROR(Err,CError::SocketType,CError::OperationNPermitted,"CSocket::Accept()");
		return FAIL;
	}
}

CSocket::OperationResult CSocket::Connect(CError& Err,CString IPAddr,WORD Port)
{
	struct sockaddr_in addr;
	if (!Err.isNoError()) return FAIL;
	if (getStatus()==NOTCREATED) {
		_SET_ERROR(Err,CError::SocketType,CError::SocketNCreated,"CSocket::Connect()");
		return FAIL;
	}
	if (getType()==STREAM) {
		addr.sin_family=AF_INET;
		addr.sin_port=htons(Port);
		if (inet_aton((const char *)(IPAddr),&(addr.sin_addr))==0) {
			_SET_ERROR(Err,CError::SocketType,CError::InvalidIPAddr,"CSocket::Connect()");
			return FAIL;
		}
		memset(&(addr.sin_zero),0,8);
		if (!connect(m_iSocket,(struct sockaddr *)&addr,sizeof(struct sockaddr))) {
			return SUCCESS;
		}
		else if ((errno==EINPROGRESS) || (errno==EALREADY) && (getMode()==NONBLOCKING)) {
			if ((m_wAsyncFlag&E_CONNECT)==E_CONNECT) setStatus(CONNECTING);
			return WOULDBLOCK;
		}
		else {
			_SET_SYSTEM_ERROR(Err,CError::SocketType,CError::ConnectError,"CSocket::Connect()",errno);
			return FAIL;
		}
	}
	else {
		m_sRemoteAddress=IPAddr;
		m_wRemotePort=Port;
		m_RemoteInfo.sin_family=AF_INET;
		m_RemoteInfo.sin_port=htons(m_wRemotePort);
		if (inet_aton((const char *)(m_sRemoteAddress),&(m_RemoteInfo.sin_addr))==0) {
			_SET_ERROR(Err,CError::SocketType,CError::InvalidIPAddr,"CSocket::Connect()");
			return FAIL;
		}
		memset(&(m_RemoteInfo.sin_zero),0,8);
		m_bDefaultRemote=true;
	}
	return SUCCESS;
}

CSocket::OperationResult CSocket::setBroadcast(CError& Err,WORD Port,CString *Addr)
{
	if (!Err.isNoError()) return FAIL;
	if (getStatus()==NOTCREATED) {
		_SET_ERROR(Err,CError::SocketType,CError::SocketNCreated,"CSocket::setBroadcast()");
		return FAIL;
	}
	if (getType()==STREAM) {
		_SET_ERROR(Err,CError::SocketType,CError::OperationNPermitted,"CSocket::setBroadcast()");
		return FAIL;
	}
	else {
		if (Addr==NULL) {    //default braodcast address 
			m_wRemotePort=Port;
			m_sRemoteAddress="255.255.255.255";
			m_RemoteInfo.sin_family=AF_INET;
			m_RemoteInfo.sin_port=htons(Port);if (m_pHelper) delete m_pHelper;
			m_RemoteInfo.sin_addr.s_addr=htonl(INADDR_BROADCAST);
			memset(&(m_RemoteInfo.sin_zero),0,8);
		}
		else { // BROADCAST
			m_sRemoteAddress=*Addr;
			m_wRemotePort=Port;
			m_RemoteInfo.sin_family=AF_INET;
			m_RemoteInfo.sin_port=htons(m_wRemotePort);
			m_RemoteInfo.sin_addr.s_addr=inet_addr((const char *)(m_sRemoteAddress));
			memset(&(m_RemoteInfo.sin_zero),0,8);
			m_bDefaultRemote=true;
		}
		// enabling broadcast transmissions.
		int Value=1;
		setSockOption(Err,SO_BROADCAST,&Value,sizeof(int));
		m_bDefaultRemote=true;
		if (!Err.isNoError()) return FAIL;
		else return SUCCESS;
	}			
}

CSocket::OperationResult CSocket::setSockMode(CError& Err,CSocket::SocketMode Mode,int recvTimeout,int sndTimeout)
{
	int opts;
	bool flagnonB=false;
	bool flagtimeO=false;
	struct timeval time;
	if (!Err.isNoError()) return FAIL;
	if (getStatus()==NOTCREATED) {
		_SET_ERROR(Err,CError::SocketType,CError::SocketNCreated,"CSocket::setSockMode()");
		return FAIL;
	}	
	if (Mode==NONBLOCKING) {
		flagnonB=true;
		flagtimeO=false;
	}
	else if (Mode==BLOCKINGTIMEO) {
		flagnonB=false;
		flagtimeO=true;
	}
	else if (Mode==BLOCKING) {
		flagnonB=false;
		flagtimeO=false;
	}
	if ((opts=fcntl(m_iSocket,F_GETFL))<0) {
		_SET_SYSTEM_ERROR(Err,CError::SocketType,CError::SockConfiguration,"CSocket::setSockMode()",errno);
		return FAIL;
	}
	else {
		if (flagnonB) {
			opts=(opts|O_NONBLOCK);
		}
		else {
			opts=(opts&~O_NONBLOCK);
		}
		if (fcntl(m_iSocket,F_SETFL,opts)<0) {
			_SET_SYSTEM_ERROR(Err,CError::SocketType,CError::SockConfiguration,"CSocket::setSockMode()",errno);
			return FAIL;
		}
	}
	if (flagtimeO) {
		time.tv_sec=(long)(recvTimeout/1000000);
		time.tv_usec=recvTimeout%1000000;
		setSockOption(Err,SO_RCVTIMEO,&time,sizeof(struct timeval));
		time.tv_sec=(long)(sndTimeout/1000000);
		time.tv_usec=sndTimeout%1000000;
		setSockOption(Err,SO_SNDTIMEO,&time,sizeof(struct timeval));
		if (!Err.isNoError()) return FAIL;
	}
	else {
		time.tv_sec=0;
		time.tv_usec=0;
		setSockOption(Err,SO_RCVTIMEO,&time,sizeof(struct timeval));
		setSockOption(Err,SO_SNDTIMEO,&time,sizeof(struct timeval));
		if (!Err.isNoError()) return FAIL;
	}
	setMode(Mode);
	return SUCCESS;
}

CSocket::OperationResult CSocket::setSockOption(CError& Err,int Opt,const void *OptVal,WORD Optlen,int Level)
{
	if (!Err.isNoError()) return FAIL;
	if (getStatus()==NOTCREATED) {
		_SET_ERROR(Err,CError::SocketType,CError::SocketNCreated,"CSocket::setSockOptions()");
		return FAIL;
	}
	if (setsockopt(m_iSocket,Level,Opt,OptVal,Optlen)<0) {
		_SET_SYSTEM_ERROR(Err,CError::SocketType,CError::SockConfiguration,"CSocket::setSockOptions()",errno);
		return FAIL;
	}
	return SUCCESS;
}

CSocket::OperationResult CSocket::getSockOption(CError& Err,int Opt,void *OptVal,WORD *Optlen,int Level)
{
	if (!Err.isNoError()) return FAIL;
	if (getStatus()==NOTCREATED) {
		_SET_ERROR(Err,CError::SocketType,CError::SocketNCreated,"CSocket::getSockOptions()");
		return FAIL;
	}
	if (getsockopt(m_iSocket,Level,Opt,OptVal,(socklen_t *)Optlen)<0) {
		_SET_SYSTEM_ERROR(Err,CError::SocketType,CError::SockConfiguration,"CSocket::getSockOptions()",errno);
		return FAIL;		
	}
	return SUCCESS;
}

int CSocket::Send(CError& Err,const void *Buff,WORD BuffLen,WORD Port,CString *Addr)
{
	int Res;
	struct sockaddr_in their_addr;
	if (!Err.isNoError()) return FAIL;
	if (getStatus()==NOTCREATED) {
		_SET_ERROR(Err,CError::SocketType,CError::SocketNCreated,"CSocket::Send()");
		return FAIL;
	}
	else if (getStatus()!=READY) {
		_SET_ERROR(Err,CError::SocketType,CError::OperationNPermitted,"CSocket::Send()");
		return FAIL;
	}
	if (getType()==STREAM) {   // stream sockets
		Res=send(m_iSocket,Buff,BuffLen,0);
	}
	else { // datagram sockets
		if ((Addr==NULL) || (Port==0)) {
			if (!m_bDefaultRemote) {
				_SET_ERROR(Err,CError::SocketType,CError::UnspecifiedHost,"CSocket::Send()");
				return FAIL;
			}
			else {
				Res=sendto(m_iSocket,Buff,BuffLen,0,(struct sockaddr *)&m_RemoteInfo,sizeof(struct sockaddr));
			}
		}
		else {
			their_addr.sin_family=AF_INET;
			their_addr.sin_port=htons(Port);
			if (inet_aton((const char *)*Addr,&(their_addr.sin_addr))==0) {
				_SET_ERROR(Err,CError::SocketType,CError::InvalidIPAddr,"CSocket::Send()");
				return FAIL;
			}
			memset(&(m_RemoteInfo.sin_zero),0,8);
			Res=sendto(m_iSocket,Buff,BuffLen,0,(struct sockaddr *)&their_addr,sizeof(struct sockaddr));
		}
	}
	if (Res<0) {
		if (((errno==EAGAIN) || (errno==EWOULDBLOCK)) && ((getMode()==NONBLOCKING) || (getMode()==BLOCKINGTIMEO))) {
			return WOULDBLOCK;
		}
		else {
			_SET_SYSTEM_ERROR(Err,CError::SocketType,CError::SendError,"CSocket::Send()",errno);
			return FAIL;
		}
	}
	return Res;
}

int CSocket::Receive(CError& Err,void *Buff,WORD BuffLen,WORD* Port,CString* Addr)
{
	int Res,addr_len;
	struct sockaddr_in their_addr;
	if (!Err.isNoError()) return FAIL;
	if (getStatus()==NOTCREATED) {
		_SET_ERROR(Err,CError::SocketType,CError::SocketNCreated,"CSocket::Receive()");
		return FAIL;
	}
	else if (getStatus()!=READY) {
		_SET_ERROR(Err,CError::SocketType,CError::OperationNPermitted,"CSocket::Receive()");
		return FAIL;
	}
	if (getType()==STREAM) {   // stream sockets
		Res=recv(m_iSocket,Buff,BuffLen,0);
	}
	else { // datagram sockets
		addr_len=sizeof(struct sockaddr);
		Res=recvfrom(m_iSocket,Buff,BuffLen,0,(struct sockaddr *)&their_addr,(socklen_t *)&addr_len);
		if (Addr!=NULL) {
			(*Addr)=CString(inet_ntoa((struct in_addr)their_addr.sin_addr));
		}
		if (Port!=NULL) {
			(*Port)=ntohs(their_addr.sin_port);
		}
	}
	if (Res<0) {
		if (((errno==EAGAIN) || (errno==EWOULDBLOCK)) && ((getMode()==NONBLOCKING) || (getMode()==BLOCKINGTIMEO))) {
			return WOULDBLOCK;
		}
		else {
			_SET_SYSTEM_ERROR(Err,CError::SocketType,CError::ReceiveError,"CSocket::Receive()",errno);
			return FAIL;
		}
	}
	return Res;
}

CSocket::OperationResult CSocket::EventSelect(CError& Err,AsyncEvent Event,bool Enabled,DDWORD Timeout)
{
	if (!Err.isNoError()) return FAIL;
	if (getStatus()==NOTCREATED) {
		_SET_ERROR(Err,CError::SocketType,CError::SocketNCreated,"CSocket::AsyncSelect()");
		return FAIL;
	}
	if (getMode()!=NONBLOCKING) {
		setSockMode(Err,NONBLOCKING);
		if (!Err.isNoError()) return FAIL;
	}
	if (Event==E_ERROR) Timeout=0; // Timeout for error events makes no sense.....
	if (!m_pHelper->Event(Event,Enabled,Timeout)) {
		_SET_ERROR(Err,CError::SocketType,CError::SockConfiguration,"CSocket::AsyncSelect()");
		return FAIL;
	}
	if (Enabled) m_wAsyncFlag|=Event;
	else m_wAsyncFlag&=~Event;
	return SUCCESS;
}

CSocket::OperationResult CSocket::Close(CError& Err)
{
	if (!Err.isNoError()) return FAIL;
	if (getStatus()==NOTCREATED) {
		_SET_ERROR(Err,CError::SocketType,CError::SocketNCreated,"CSocket::Close()");
		return FAIL;
	}
	if (m_iSocket!=0) close(m_iSocket);
	setStatus(NOTCREATED);
	if (m_pHelper) delete m_pHelper;
	fastInit();
	return SUCCESS;
}

CSocket::SocketStatus CSocket::getStatus() const
{
	return m_Status;
}

CSocket::SocketMode CSocket::getMode() const
{
	return m_Mode;
}

CSocket::SocketType CSocket::getType() const
{
	return m_Type;
}

CString CSocket::getHostNamebyAddr(CString Addr)
{
	struct hostent *h;
	struct in_addr app;
	if (inet_aton((const char*)Addr,&app)==0) {
		return CString("");
	}
	h=gethostbyaddr(&app,sizeof(app),AF_INET);
	if (h!=NULL) {
		return CString(h->h_name);
	}
	else {
		return CString("");
	}
}

CString CSocket::getAddrbyHostName(CString Host)
{
	struct hostent *h;
	char *a;
	h=gethostbyname((const char *)Host);
	if (h==NULL) {
		return CString("");
	}
	else {
		a=inet_ntoa(*((struct in_addr *)h->h_addr));
		return CString(a);
	}
}

CString CSocket::getHostName()
{
	char HostName[256];
	if (gethostname(HostName,255)<0) return CString("");
	else return CString(HostName);
}

void CSocket::getSocketName(CString &HostAddr,WORD &Port)
{
	struct sockaddr_in addr;
	char *a1;
	socklen_t Len=sizeof(struct sockaddr_in);
	if (getsockname(m_iSocket,(struct sockaddr *)&addr,&Len)<0) {
		HostAddr="";
		Port=0;
	}
	else {
		a1=inet_ntoa(addr.sin_addr);
		HostAddr=CString(a1);
		Port=ntohs(addr.sin_port);
	}
}

void CSocket::getPeerName(CString &RemoteAddr,WORD &RemotePort)
{
	struct sockaddr_in addr;
	char *a1;
	socklen_t Len=sizeof(struct sockaddr_in);
	if (getpeername(m_iSocket,(struct sockaddr*)&addr,&Len)<0) {
		RemoteAddr="";
	}
	else {
		RemotePort=ntohs(addr.sin_port);
		a1=inet_ntoa(addr.sin_addr);
		RemoteAddr=CString(a1);
	}
}

void CSocket::setSleepTime(DWORD Timebit)
{
	m_pHelper->setSleep(Timebit);
}

void CSocket::onTimeout(WORD EventMask)
{
}

void CSocket::onAccept(DWORD Counter)
{
}

void CSocket::onConnect(int ErrorCode)
{
}

void CSocket::onReceive(DWORD Counter,bool &Boost)
{
}

void CSocket::onSend(DWORD Counter,bool &Boost)
{
}

void CSocket::onError(DWORD Counter,int ErrorCode)
{
}

void CSocket::setStatus(SocketStatus Status)
{
	baci::ThreadSyncGuard guard(&m_LocalMutex);
	m_Status=Status;
}

void CSocket::setMode(SocketMode Mode)
{
	baci::ThreadSyncGuard guard(&m_LocalMutex);
	m_Mode=Mode;
}

void CSocket::setType(SocketType Type)
{
	baci::ThreadSyncGuard guard(&m_LocalMutex);
	m_Type=Type;
}

void CSocket::fastInit()
{
	m_iSocket=0;
	m_Status=NOTCREATED;
	m_Mode=BLOCKING;
	m_Type=STREAM;
	m_bDefaultRemote=false;
	m_wRemotePort=0;
	m_sRemoteAddress="";
	m_pHelper=NULL;
	m_wAsyncFlag=0;
}

//************* SocketFramework implementation ***********************

#define THREAD_NAME "SocketFrameworkWorkerThread"

CSocketFramework::CSocketFramework(CSocket *Sock)
{
	m_pThread=NULL;
	m_ThreadArea.setSocket(Sock);
}

CSocketFramework::~CSocketFramework()
{
	if (m_pThread) {
		m_pThread->stop();
		while (m_pThread->isAlive()) {
			CIRATools::Wait(0,50000);
		}
		m_threadManager.terminate(THREAD_NAME);
		//m_threadManager.terminateAll();
	}
}

bool CSocketFramework::Event(WORD Event,bool Enable,DDWORD Timeout)
{
	if (Enable) {  //asked to turn on the event
		if (m_ThreadArea.addEvent(Event,Timeout)) {
			if (m_pThread==NULL) {
				m_pThread=m_threadManager.create(THREAD_NAME,(void *)worker,static_cast<void *>(&m_ThreadArea));
				m_pThread->setSleepTime(m_ThreadArea.getSleepTime()*10);
				m_pThread->resume();
			}
			else if (m_ThreadArea.getSize()==1) {
				m_pThread->setSleepTime(m_ThreadArea.getSleepTime()*10);
				m_pThread->resume();
			}
		}
		else {
			return false;
		}
	}
	else {  // asked to turn off the event
		if (m_ThreadArea.removeEvent(Event)) {
			if (m_ThreadArea.getSize()==0) {
				m_pThread->suspend();
			}
		}	
		else {
			return false;
		}
	}
	return true;
}

void CSocketFramework::setSleep(DWORD STime)
{
	m_ThreadArea.setSleepTime(STime);
}

void CSocketFramework::worker(void *threadParam)
{
	TThreadArea *Data;
	struct pollfd Polling;
	TIMEVALUE From,To;
	int Res;
	DWORD Counter; 
	DDWORD Elapsed;
	DWORD WaitTime;
	bool boost;
	WORD Mask;
	//get access to the thread this function is being executed from
   ACS::ThreadBaseParameter *baciParameter=static_cast<ACS::ThreadBaseParameter *>(threadParam);
   ACS::ThreadBase *myself_p=baciParameter->getThreadBase();
	// Init the thread
	if (ACS::ThreadBase::InitThread!=0)  {
		ACS::ThreadBase::InitThread(THREAD_NAME);
	}
	Data=(TThreadArea*)baciParameter->getParameter();
	// save socket file descriptor.
	Polling.fd=Data->getSocket()->m_iSocket;
	Polling.events=POLLIN|POLLOUT;
	Polling.revents=0;
	while(myself_p->check()==true) {
		if(myself_p->isSuspended()==false) {
			Polling.revents=0;
			boost=false;
			WaitTime=Data->getSleepTime();
			CIRATools::getTime(From); // get time before calling Poll
			Res=poll(&Polling,1,WaitTime/1000);
			CIRATools::getTime(To); // get time after calling Poll
			if (Res==0) {  // TIMEOUT
				Mask=Data->getExpiredEvents();
				if (Mask!=0) {
					Data->getSocket()->onTimeout(Mask);
				}
				continue;
			}
			else if ((Res<0) && (errno==EINTR)) {  // a signal arrived before an event occurs
				continue;
			}
			else if (Res>0) {
				if ((Polling.revents&POLLIN)==POLLIN) {
					if (Data->getSocket()->getStatus()==CSocket::ACCEPTING) {
						if ((Counter=Data->updateEvent(CSocket::E_ACCEPT))>0) {
							Data->getSocket()->onAccept(Counter);
						}
					}
					else { // READING
						if ((Counter=Data->updateEvent(CSocket::E_RECEIVE))>0) {
							Data->getSocket()->onReceive(Counter,boost);
						}
					}
				}
				if ((Polling.revents&POLLOUT)==POLLOUT) {
					if (Data->getSocket()->getStatus()==CSocket::CONNECTING) {
						if ((Counter=Data->updateEvent(CSocket::E_CONNECT))>0) {
							int Code;
							WORD Len=sizeof(int);
							CError Err;
							Err.Reset();
							Data->getSocket()->getSockOption(Err,SO_ERROR,&Code,&Len);
							if (!Err.isNoError()) Code=-1;
							Data->getSocket()->onConnect(Code);
							Data->getSocket()->setStatus(CSocket::READY);
						}
					}
					else {  //WRITING
						if ((Counter=Data->updateEvent(CSocket::E_SEND))>0) {
							Data->getSocket()->onSend(Counter,boost);
						}
					}
				}
				if ((Polling.revents&POLLERR)==POLLERR) {
					if ((Counter=Data->updateEvent(CSocket::E_ERROR))>0) {
						int Code;
						WORD Len=sizeof(int);
						CError Err;
						Err.Reset();
						Data->getSocket()->getSockOption(Err,SO_ERROR,&Code,&Len);
						if (!Err.isNoError()) Code=-1;
						Data->getSocket()->onError(Counter,Code);
					}
				}
				Mask=Data->getExpiredEvents();
				if (Mask!=0) {
					Data->getSocket()->onTimeout(Mask);
				}
			}
			else if (Res<0) {
			}
			if (!boost) {
				Elapsed=CIRATools::timeDifference(From,To);
				if (Elapsed<WaitTime) {
					myself_p->sleep((WaitTime-Elapsed)*10);
				}
			}
		}
		myself_p->sleep();
	}
	if (ACS::ThreadBase::DoneThread!=0) {
		ACS::ThreadBase::DoneThread();
	}
	myself_p->setStopped();
	delete baciParameter;
}

CSocketFramework::TThreadArea::TThreadArea()
{
	ElCount=0;
	SleepTime=100000;  // default is one tens of second
}

CSocketFramework::TThreadArea::~TThreadArea()
{
	TEventInfo *El;
	THashTable::iterator pos;
	if (ElCount>0) {
		for (pos=Elements.begin();pos!=Elements.end();++pos) {
			El=pos->second;
			if (!El) delete El;
		}
	}
}
DWORD CSocketFramework::TThreadArea::getSleepTime()
{
	baci::ThreadSyncGuard guard(&Mutex);
	return SleepTime;
}

void CSocketFramework::TThreadArea::setSleepTime(DWORD STime)
{
	baci::ThreadSyncGuard guard(&Mutex);
	SleepTime=STime;
}

DWORD CSocketFramework::TThreadArea::updateEvent(WORD Event)
{
	baci::ThreadSyncGuard guard(&Mutex);
	THashTable::iterator pos;
	TEventInfo *El;
	TIMEVALUE Tmp;
	if (EventExist(Event,pos)) {
		CIRATools::getTime(Tmp);
		El=pos->second;
		CIRATools::timeCopy(El->LastTime,Tmp);
		El->EventCount++;
		return El->EventCount;
	}
	return 0;
}

WORD CSocketFramework::TThreadArea::getExpiredEvents()
{
	baci::ThreadSyncGuard guard(&Mutex);
	THashTable::iterator pos;
	TEventInfo *El;
	TIMEVALUE Tmp;
	WORD EventMask=0;
	if (ElCount>0) {
		// get current time !!!
		CIRATools::getTime(Tmp);
		for (pos=Elements.begin();pos!=Elements.end();++pos) {
			El=pos->second;
			if (El->Timeout!=0) {  // zero means no timeout
				if (CIRATools::timeDifference(El->LastTime,Tmp)>El->Timeout) {
					EventMask|=(pos->first);
					CIRATools::timeCopy(El->LastTime,Tmp);
				}
			}
		}
	}
	return EventMask;
}

bool CSocketFramework::TThreadArea::EventExist(WORD Event,THashTable::iterator &ff)
{
	ff=Elements.find(Event);
	if (ff!=Elements.end()) { // Event already exists
		return true;
	}
	else return false;
}

bool CSocketFramework::TThreadArea::addEvent(WORD Event,DDWORD Timeout)
{
	TIMEVALUE Tmp;
	TEventInfo *El;
	THashTable::iterator ff;
	baci::ThreadSyncGuard guard(&Mutex);
	if (EventExist(Event,ff)) { // Event already exists
		return true;
	}
	else {
		CIRATools::getTime(Tmp);
		try {
			El=(TEventInfo *)new TEventInfo;
		}
		catch (std::bad_alloc& ex) {
			return false;
		}
		El->Timeout=Timeout;
		El->EventCount=0;
		CIRATools::timeCopy(El->LastTime,Tmp);
		Elements[Event]=El;
		ElCount++;
	}
	return true;
}

bool CSocketFramework::TThreadArea::removeEvent(WORD Event)
{
	THashTable::iterator ff;
	TEventInfo *El;
	baci::ThreadSyncGuard guard(&Mutex);
	if (EventExist(Event,ff)) {   // Event exists
		El=ff->second;
		if (El) {
			delete El;
			Elements.erase(ff);
			ElCount--;
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return true;
	}
}
