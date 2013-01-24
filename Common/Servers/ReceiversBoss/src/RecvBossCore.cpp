#include "RecvBossCore.h"

using namespace IRA;

#ifdef COMPILE_TARGET_MED

//#define RB_DEBUG

#define KKC_ADDRESS "192.168.51.4" // this is the PortServer installed directly in the MF
#define KKC_PORT 2101 // first port...please notice that this works only if the port is configuread as "real Port"
#define RECV_ADDRESS "192.167.189.2"
#define RECV_PORT 2096
#define FS_ADDRESS "192.167.189.43"
#define FS_PORT 5002

// speed of light in meters per second
#define LIGHTSPEED 299792458.0

CRecvBossCore::CRecvBossCore()
{
}

CRecvBossCore::~CRecvBossCore()
{
}

void CRecvBossCore::initialize(maci::ContainerServices* services,CConfiguration *config)
{
	m_currentReceiver="";
	m_currentOperativeMode="";
	m_recvOpened=false;
	m_fsOpened=false;
	m_LO[0]=m_LO[1]=0.0;
	m_status=Management::MNG_WARNING;
	m_services=services;
	m_feeds=0;
	m_IFs=0;
	m_startFreq[0]=m_startFreq[1]=0.0;
	m_bandWidth[0]=m_bandWidth[1]=0.0;
	m_config=config;
}

void CRecvBossCore::execute() throw (ComponentErrors::IRALibraryResourceExImpl,ComponentErrors::CDBAccessExImpl)
{
    IRA::CError error;
	m_KKCFeedTable=new IRA::CDBTable(m_services,"Feed","DataBlock/KKCReceiver/Feeds");
	error.Reset();
	if (!m_KKCFeedTable->addField(error,"feedCode",IRA::CDataField::LONGLONG)) {
		error.setExtra("feedCode field not found",0);
 	}
	else if (!m_KKCFeedTable->addField(error,"xOffset",IRA::CDataField::DOUBLE)) {
		error.setExtra("xOffset field not found",0);
 	}
	else if (!m_KKCFeedTable->addField(error,"yOffset",IRA::CDataField::DOUBLE)) {
		error.setExtra("yOffset field not found",0);
 	}
	else if (!m_KKCFeedTable->addField(error,"relativePower",IRA::CDataField::DOUBLE)) {
		error.setExtra("relativePower field not found",0);
 	}
	if (!error.isNoError()) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		dummy.log(LM_DEBUG);
		throw dummy;
	}
	if (!m_KKCFeedTable->openTable(error))	{
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
		dummy.log(LM_DEBUG);
		throw dummy;
	}
}

void CRecvBossCore::cleanUp()
{
	if (m_KKCFeedTable) delete m_KKCFeedTable;
}

void CRecvBossCore::calOn() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	IRA::CError err;
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentReceiver=="KKC") {
#ifndef RB_DEBUG
		char buff [10] = {0x01,0x7F,0x7D,0x6F,0x00,0x04,0x03,0x04,0x0B,0x01 };
		if (m_kBandSocket.Send(err,(const void *)buff,10)!=10) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::calOn()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
		m_kBandSocket.Receive(err,(void *)buff,10); // read the answer but for the moment I don't care. I hope everything worked properly
#endif
	}
	else if (m_currentReceiver=="CCC") {
#ifndef RB_DEBUG
		char buffer [13] = {'s','e','t',' ','m','a','r','c','a',' ','o','n','\n' };
		if (m_recvSocket.Send(err,(const void *)buffer,13)!=13) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::calOn()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
		m_recvSocket.Receive(err,(void *)buffer,13); // read the answer but for the moment I don't care. I hope everything worked properly
#endif
	}
	else if (m_currentReceiver=="XXP") {
#ifndef RB_DEBUG
		// turn the marca on through thr FS
		IRA::CString fsBuffer("sxkl=*,on\n");
		if (m_fsSocket.Send(err,(const void *)fsBuffer,fsBuffer.GetLength())!=fsBuffer.GetLength()) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::calOn()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
#endif	
	}
	else {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::calOn()");
		impl.setReason("Receiver not configured yet");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
	ACS_LOG(LM_FULL_INFO,"CRecvBossCore::calOn()",(LM_NOTICE,"NOISE_CAL_TURNED_ON"));
}

void CRecvBossCore::calOff() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	IRA::CError err;
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentReceiver=="KKC") {
#ifndef RB_DEBUG		  
		char buff [10] = {0x01,0x7F,0x7D,0x6F,0x00,0x04,0x03,0x04,0x0B,0x00 };
		if (m_kBandSocket.Send(err,(const void *)buff,10)!=10) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::calOff()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
		m_kBandSocket.Receive(err,(void *)buff,10); // read the answer but for the moment I don't care. I hope everything worked properly
#endif
	}
	else if (m_currentReceiver=="CCC") {
#ifndef RB_DEBUG		
		char buffer [14] = {'s','e','t',' ','m','a','r','c','a',' ','o','f','f','\n' };
		if (m_recvSocket.Send(err,(const void *)buffer,14)!=14) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::calOff()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
		m_recvSocket.Receive(err,(void *)buffer,14); // read the answer but for the moment I don't care. I hope everything worked properly
#endif
	}
	else if (m_currentReceiver=="XXP") {
#ifndef RB_DEBUG
		// turn the marca on through thr FS
		IRA::CString fsBuffer("sxkl=*,off\n");
		if (m_fsSocket.Send(err,(const void *)fsBuffer,fsBuffer.GetLength())!=fsBuffer.GetLength()) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::calOff()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
#endif	
	}
	else {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::calOff()");
		impl.setReason("Receiver not configured yet");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
	ACS_LOG(LM_FULL_INFO,"CRecvBossCore::calOff()",(LM_NOTICE,"NOISE_CAL_TURNED_OFF"));
}

void CRecvBossCore::setupReceiver(const char * code) throw (ManagementErrors::ConfigurationErrorExImpl)
{
	try {
		setup(code);
	}
	catch(ACSErr::ACSbaseExImpl& ex) {
		_ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl,impl,ex,"CRecvBossCore::setupReceiver()");
		impl.setSubsystem("Receivers");
		throw impl;
	}
}

void CRecvBossCore::AUOn() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
				ComponentErrors::UnexpectedExImpl)
{
	ACS_LOG(LM_FULL_INFO,"CRecvBossCore::AUOn()",(LM_NOTICE,"ANTENNA_UNIT_ON_OPERATION_NOT_SUPPORTED"));
}

void CRecvBossCore::AUOff() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
				ComponentErrors::UnexpectedExImpl)
{
	ACS_LOG(LM_FULL_INFO,"CRecvBossCore::AUOff()",(LM_NOTICE,"ANTENNA_UNIT_OFF_OPERATION_NOT_SUPPORTED"));
}

void CRecvBossCore::setLO(const ACS::doubleSeq& lo) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	char buff [10];
	IRA::CError err;
	IRA::CString msg;
	WORD len;
	double trueValue;
	double loAmp[4]={62.444,-12.311,0.7323,-0.0118};
	baci::ThreadSyncGuard guard(&m_mutex);
	if (lo.length()==0) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::setLO()");
		impl.setReason("at least one value must be provided");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
	if (lo[0]==-1) {
		ACS_LOG(LM_FULL_INFO,"CRecvBossCore::setLO()",(LM_NOTICE,"KEEP_CURRENT_LOCAL_OSCILLATOR %lf",m_LO[0]));
		return;
	}
	//***********************************************************************/
	// range checks must be performed, Also the resulting bandwidth, according the lo value and the RF ranges must be recomputed
	//**********************************************************************/
	if (m_currentReceiver=="KKC") {
		//no difference between IFs so take just the first value
		trueValue=lo[0]-5900.0;
		m_LO[0]=m_LO[1]=lo[0];
		ACS_LOG(LM_FULL_INFO,"CRecvBossCore::setLO()",(LM_NOTICE,"LOCAL_OSCILLATOR: %lf",m_LO[0]));
	}
	else if (m_currentReceiver=="CCC") {
		//no difference between IFs so take just the first value
		trueValue=lo[0]+2300.0;
		m_LO[0]=m_LO[1]=lo[0];
		ACS_LOG(LM_FULL_INFO,"CRecvBossCore::setLO()",(LM_NOTICE,"LOCAL_OSCILLATOR: %lf",m_LO[0]));
	}
	else if (m_currentReceiver=="XXP") {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::setLO()");
		impl.setReason("Local oscillator could not be changed for this receiver");
		m_status=Management::MNG_WARNING;
		throw impl;		
	}
	else {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::setLO()");
		impl.setReason("Receiver not configured yet");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
#ifndef RB_DEBUG	
	// at the moment the settable LO is unique...so no difference from recevier to receiver
	msg.Format("set lofreq %lf\n",(double)trueValue);  
	len=msg.GetLength();
	if (m_recvSocket.Send(err,(const void *)msg,len)!=len) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
		dummy.setCode(err.getErrorCode());
		dummy.setDescription((const char*)err.getDescription());
		err.Reset();
		_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setLO()");
		m_status=Management::MNG_FAILURE;
		throw impl;
	}
	m_recvSocket.Receive(err,(void *)buff,10); // read the answer but for the moment I don't care. I hope everything worked properly
	if (m_currentReceiver=="KKC") {
		double amplitude=loAmp[0]+loAmp[1]*trueValue+loAmp[2]*trueValue*trueValue+loAmp[3]*trueValue*trueValue*trueValue;
		msg.Format("set loamp %lf\n",(double)amplitude);
		len=msg.GetLength();
		if (m_recvSocket.Send(err,(const void *)msg,len)!=len) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setLO()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
		m_recvSocket.Receive(err,(void *)buff,10); // read the answer but for the moment I don't care. I hope everything worked properly		
	}
#endif	
}

void CRecvBossCore::setup(const char * code) throw (ComponentErrors::SocketErrorExImpl,ComponentErrors::ValidationErrorExImpl)
{
	IRA::CError err;
	IRA::CString rec(code);
	IRA::CString recvIpAddr(RECV_ADDRESS);
	DWORD recvPort=RECV_PORT;
	IRA::CString fsIpAddr(FS_ADDRESS);
	DWORD fsPort=FS_PORT;	
	baci::ThreadSyncGuard guard(&m_mutex);
	if (!m_fsOpened) {
#ifndef RB_DEBUG		
		if (m_fsSocket.Create(err,IRA::CSocket::STREAM)!=IRA::CSocket::SUCCESS) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
		if (m_fsSocket.Connect(err,fsIpAddr,fsPort)==IRA::CSocket::FAIL) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription( (const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
#endif		
		IRA::CIRATools::Wait(0,500000);  //wait half a second to settle things down
		m_fsOpened=true;
	}	
	// Configure the comunication for the recevicer configuration
	if (!m_recvOpened) {
#ifndef RB_DEBUG		
		if (m_recvSocket.Create(err,IRA::CSocket::STREAM)!=IRA::CSocket::SUCCESS) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
		if (m_recvSocket.setSockMode(err,IRA::CSocket::NONBLOCKING)==IRA::CSocket::FAIL) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription( (const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}		
		if (m_recvSocket.Connect(err,recvIpAddr,recvPort)==IRA::CSocket::FAIL) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription( (const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
#endif		
		IRA::CIRATools::Wait(0,500000);  //wait half a second to settle things down
		m_recvOpened=true;
	}
	if (rec=="KKC") {
		// close in case it was opened.......
#ifndef RB_DEBUG		
		err.Reset();
		m_kBandSocket.Close(err);
		err.Reset();
		if (m_kBandSocket.Create(err,IRA::CSocket::STREAM)!=IRA::CSocket::SUCCESS) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
		if (m_kBandSocket.setSockMode(err,IRA::CSocket::NONBLOCKING)==IRA::CSocket::FAIL) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription( (const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}		
		IRA::CString ipAddr(KKC_ADDRESS);
		DWORD port=KKC_PORT;
		if (m_kBandSocket.Connect(err,ipAddr,port)==IRA::CSocket::FAIL) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription( (const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
		char buffer [9] = {'p','r','o','c',' ','k','k','c','\n' };
		if (m_recvSocket.Send(err,(const void *)buffer,9)!=9) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
		m_recvSocket.Receive(err,(void *)buffer,9); // read the answer but for the moment I don't care. I hope everything worked properly
		// now set the subreflector configuratio through the FS
		IRA::CString fsBuffer("scu=kkc\n");
		if (m_fsSocket.Send(err,(const void *)fsBuffer,fsBuffer.GetLength())!=fsBuffer.GetLength()) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
#endif		
		m_LO[0]=21964.0;
		m_LO[1]=21964.0;
		m_IFs=2;
		m_feeds=7;
		m_pols[0]=Receivers::RCV_LCP;
		m_pols[1]=Receivers::RCV_RCP;
		m_startFreq[0]=100.0;
		m_startFreq[1]=100.0;
		m_bandWidth[0]=2000.0;
		m_bandWidth[1]=2000.0;
		m_currentReceiver="KKC";
		m_currentOperativeMode="NORMAL";
	}
	else if (rec=="CCC") {
#ifndef RB_DEBUG		
		char buffer [9] = {'p','r','o','c',' ','c','c','c','\n' };
		if (m_recvSocket.Send(err,(const void *)buffer,9)!=9) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
		m_recvSocket.Receive(err,(void *)buffer,9); // read the answer but for the moment I don't care. I hope everything worked properly		
		// now set the subreflector configuration through the FS
		IRA::CString fsBuffer("scu=ccc\n");
		if (m_fsSocket.Send(err,(const void *)fsBuffer,fsBuffer.GetLength())!=fsBuffer.GetLength()) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}	
#endif		
		m_LO[0]=4600.0;
		m_LO[1]=4600.0;
		m_IFs=2;
		m_feeds=1;
		m_pols[0]=Receivers::RCV_LCP;
		m_pols[1]=Receivers::RCV_RCP;
		m_startFreq[0]=100.0;
		m_startFreq[1]=100.0;
		m_bandWidth[0]=800.0;
		m_bandWidth[1]=800.0;
		m_currentReceiver="CCC";
		m_currentOperativeMode="NORMAL";
	}
	else if (rec=="XXP") {
#ifndef RB_DEBUG		
		char buffer [9] = {'p','r','o','c',' ','x','x','p','\n' };
		if (m_recvSocket.Send(err,(const void *)buffer,9)!=9) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
		m_recvSocket.Receive(err,(void *)buffer,9); // read the answer but for the moment I don't care. I hope everything worked properly		
		// now set the subreflector configuration through the FS
		IRA::CString fsBuffer("scu=xxp\n");
		if (m_fsSocket.Send(err,(const void *)fsBuffer,fsBuffer.GetLength())!=fsBuffer.GetLength()) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}	
		 //fsBuffer=IRA::CString("sxkl=send%0068033f\n");
		fsBuffer=IRA::CString("xxpmode\n");
		if (m_fsSocket.Send(err,(const void *)fsBuffer,fsBuffer.GetLength())!=fsBuffer.GetLength()) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
#endif		
		m_LO[0]=8080.0;
		m_LO[1]=8080.0;
		m_IFs=2;
		m_feeds=1;
		m_pols[0]=Receivers::RCV_RCP;
		m_pols[1]=Receivers::RCV_LCP;
		m_startFreq[0]=100.0;
		m_startFreq[1]=100.0;
		m_bandWidth[0]=800.0;
		m_bandWidth[1]=800.0;		
		m_currentReceiver="XXP";
		m_currentOperativeMode="NORMAL";
	}
	else {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::setup()");
		impl.setReason("Receiver code is not known");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
	m_status=Management::MNG_OK;
	ACS_LOG(LM_FULL_INFO,"CRecvBossCore::setup()",(LM_NOTICE,"NEW_RECEIVER_CONFIGURED %s",(const char *)m_currentReceiver));
}

void CRecvBossCore::setMode(const char * mode) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::ModeErrorExImpl,
		ComponentErrors::UnexpectedExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,ComponentErrors::CORBAProblemExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	IRA::CString newMode(mode);
	if (m_currentReceiver=="CCC") {
		if (newMode=="NORMAL") {
			m_currentOperativeMode=newMode;
			m_bandWidth[0]=800.0;
			m_bandWidth[1]=800.0;					
		}
		else if (newMode=="NARROWBANDWIDTH") {
			m_currentOperativeMode=newMode;
			m_bandWidth[0]=150.0;
			m_bandWidth[1]=150.0;
		}
		else {
			_EXCPT(ReceiversErrors::ModeErrorExImpl,impl,"CRecvBossCore::setMode()");
			throw impl;
		}
	}
	else {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::setMode()");
		impl.setReason("current receiver does not have operative modes");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
}

void CRecvBossCore::park()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	m_LO[0]=0;
	m_LO[1]=0;
	m_currentReceiver="";
	m_currentOperativeMode="";
	m_IFs=0;
	m_feeds=0;
}

double CRecvBossCore::getTaper(const double& freq,const double& bw,const long& feed,const long& ifNumber,double& waveLen) throw (ComponentErrors::ValidationErrorExImpl,
		ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,ComponentErrors::UnexpectedExImpl)
{
	double centralFreq;
	double taper;
	double primaryFreq[5]= { 1.4, 1.65, 2.28, 8.6, 23.0 };
	double primaryTaper[5]= { -24, -29, -19, -23, -25 };
	double secondaryFreq[7]= { 4.3, 5.05, 5.8, 6.3, 18, 22, 26 };
	double secondaryTaper[7]= { -9.5, -12, -14, -9.9, -3.2, -5.3, -7.7 };
	double *ff,*tt;
	long max,p1=0,p2=0,i;
	double realFreq,realBw;

	baci::ThreadSyncGuard guard(&m_mutex);
	if ((ifNumber>=m_IFs) || (ifNumber<0)) {
		_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CRecvBossCore::getTaper()");
		impl.setValueName("IF identifier");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
	if ((feed>=m_feeds) || (feed<0)) {
		_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CRecvBossCore::getTaper()");
		impl.setValueName("feeds identifier");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
	// take the treal observed bandwidth....the correlation between detector device and the band provided by the receiver
	if (!IRA::CIRATools::skyFrequency(freq,bw,m_startFreq[ifNumber],m_bandWidth[ifNumber],realFreq,realBw)) {
		realFreq=m_startFreq[ifNumber];
		realBw=0.0;
	}
	centralFreq=m_LO[ifNumber]+realFreq+realBw/2;
	centralFreq/=1000.0; //central frequency in GHz
	if ((m_currentReceiver=="KKC") || (m_currentReceiver=="CCC")) {
		ff=secondaryFreq;
		tt=secondaryTaper;
		max=7;
	}
	else if (m_currentReceiver=="XXP") {
		ff=primaryFreq;
		tt=primaryTaper;
		max=5;
	}
	else {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::getTaper()");
		impl.setReason("Receiver code is not known");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
	waveLen=LIGHTSPEED/(centralFreq*1000000000);
	for (i=0;i<max;i++) {
		if (centralFreq<=ff[i]) {
			p2=i;
			if (i==0) {
				p1=i;
			}
			else {
				p1=i-1;
			}
			break;
		}
	}
	if (i==max) {
		p2=p1=max-1;
	}
	if (ff[p1]==ff[p2]) {
		taper=tt[p2];
	}
	else {
		taper=((tt[p2]-tt[p1])*(centralFreq-ff[p1])/(ff[p2]-ff[p1]))+tt[p1];
	}
	return taper;
}

long CRecvBossCore::getFeeds(ACS::doubleSeq& X,ACS::doubleSeq& Y,ACS::doubleSeq& power) throw (ComponentErrors::ValidationErrorExImpl,
		ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentReceiver=="KKC") {
		long id;
		unsigned len;
		m_KKCFeedTable->First();
		len=m_KKCFeedTable->recordCount();
		X.length(len);
		Y.length(len);
		power.length(len);
		for (unsigned i=0;i<len;i++) {
			id=(*m_KKCFeedTable)["feedCode"]->asLongLong();
			X[i]=(*m_KKCFeedTable)["xOffset"]->asDouble();
			Y[i]=(*m_KKCFeedTable)["yOffset"]->asDouble();
			power[i]=(*m_KKCFeedTable)["relativePower"]->asDouble();
			m_KKCFeedTable->Next();
		}		
		return m_KKCFeedTable->recordCount();
	}
	else if ((m_currentReceiver=="CCC") || (m_currentReceiver=="XXP")) {
		X.length(1);
		Y.length(1);
		power.length(1);
		X[0]=0.0;
		Y[0]=0.0;
		power[0]=1.0;
		return 1;
	}
	else {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::getFeeds()");
		impl.setReason("Receiver not configured yet");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
}

void CRecvBossCore::getCalibrationMark(ACS::doubleSeq& result,ACS::doubleSeq& resFreq,ACS::doubleSeq& resBw,const ACS::doubleSeq& freqs,const ACS::doubleSeq& bandwidths,const ACS::longSeq& feeds,
		const ACS::longSeq& ifs,double& scale) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	unsigned stdLen=freqs.length();
	if ((stdLen!=bandwidths.length()) || (stdLen!=feeds.length()) || (stdLen!=ifs.length())) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::getCalibrationMark()");
		impl.setReason("sub-bands definition is not consistent");
		m_status=Management::MNG_WARNING;
		throw impl;			
	}
	for (unsigned i=0;i<stdLen;i++) {
		if ((ifs[i]>=m_IFs) || (ifs[i]<0)) {
			_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CRecvBossCore::getCalibrationMark()");
			impl.setValueName("IF identifier");
			m_status=Management::MNG_WARNING;
			throw impl;
		}
	}
	for (unsigned i=0;i<stdLen;i++) {
		if ((feeds[i]>=m_feeds) || (feeds[i]<0)) {
			_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CRecvBossCore::getCalibrationMark()");
			impl.setValueName("feeds identifier");
			m_status=Management::MNG_WARNING;
			throw impl;
		}
	}	
	result.length(stdLen);
	resFreq.length(stdLen);
	resBw.length(stdLen);
	if (m_currentReceiver=="KKC") {
		scale=1.0;
		double LeftMarkCoeff[7][4] = { {0.2912,-21.21,506.97,-3955.5}, {0.558,-40.436,961.81,-7472.1}, {0.8963,-63.274,1469.2,-11170.0}, 
									 {0.5176,-37.47,889.81,-6895.8}, {0.662,-47.86,1136.2,-8809.9}, {0.3535,-25.63,609.53,-4727.4}, {0.2725,-19.926,478.05,-3737.9}  };
		double RightMarkCoeff[7][4] = { {0.2141,-15.771,379.96,-2976.8}, {0.523,-37.88,899.74,-6974.4}, {0.8572,-60.324,1396.3,-10581.0}, 
									 {0.5173,-37.339,884.31,-6838.1}, {0.7267,-52.213,1231.4,-9486.9}, {0.3955,-28.468,672.77,-5192.0}, {0.201,-15.761,398.52,-3237.7} };
		double f1,f2;
		double integral;
		double mark=0;
		double realFreq,realBw;
		for (unsigned i=0;i<stdLen;i++) {
			if (m_pols[ifs[i]]==Receivers::RCV_LCP) {
				// take the real observed bandwidth....the correlation between detector device and the band provided by the receiver
				if (!IRA::CIRATools::skyFrequency(freqs[i],bandwidths[i],m_startFreq[ifs[i]],m_bandWidth[ifs[i]],realFreq,realBw)) {
					realFreq=m_startFreq[ifs[i]];
					realBw=0.0;
				}
				realFreq+=m_LO[ifs[i]];
				f1=realFreq;
				f2=f1+realBw;
				f1/=1000.0; f2/=1000.0; //frequencies in giga Hertz
				integral=(LeftMarkCoeff[feeds[i]][0]/4)*(f2*f2*f2*f2-f1*f1*f1*f1)+(LeftMarkCoeff[feeds[i]][1]/3)*(f2*f2*f2-f1*f1*f1)+(LeftMarkCoeff[feeds[i]][2]/2)*(f2*f2-f1*f1)+LeftMarkCoeff[feeds[i]][3]*(f2-f1);				
				mark=integral/(f2-f1);				
			}
			else if (m_pols[ifs[i]]==Receivers::RCV_RCP) {
				// take the real observed bandwidth....the correlation between detectro device and the band provided by the receiver
				if (!IRA::CIRATools::skyFrequency(freqs[i],bandwidths[i],m_startFreq[ifs[i]],m_bandWidth[ifs[i]],realFreq,realBw)) {
					realFreq=m_startFreq[ifs[i]];
					realBw=0.0;
				}
				realFreq+=m_LO[ifs[i]];
				f1= realFreq;
				f2=f1+realBw;
				f1/=1000.0; f2/=1000.0; //frequencies in giga Hertz
				integral=(RightMarkCoeff[feeds[i]][0]/4)*(f2*f2*f2*f2-f1*f1*f1*f1)+(RightMarkCoeff[feeds[i]][1]/3)*(f2*f2*f2-f1*f1*f1)+(RightMarkCoeff[feeds[i]][2]/2)*(f2*f2-f1*f1)+RightMarkCoeff[feeds[i]][3]*(f2-f1);				
				mark=integral/(f2-f1);
			}
			result[i]=mark;
			resFreq[i]=realFreq;
			resBw[i]=realBw;
		}
	}
	else if (m_currentReceiver=="CCC") { 
		scale=1.0;
		for (unsigned i=0;i<stdLen;i++) {
			double realFreq,realBw;
			if (!IRA::CIRATools::skyFrequency(freqs[i],bandwidths[i],m_startFreq[ifs[i]],m_bandWidth[ifs[i]],realFreq,realBw)) {
				realFreq=m_startFreq[ifs[i]];
				realBw=0.0;
			}
			resFreq[i]=realFreq+m_LO[ifs[i]];
			resBw[i]=realBw;
			if (m_pols[ifs[i]]==Receivers::RCV_LCP) {
				result[i]=5.88; //left
			}
			else {
				result[i]=5.71; //right
			}
		}
	}
	else if (m_currentReceiver=="XXP") { 
		scale=1.0;
		for (unsigned i=0;i<stdLen;i++) {
			double realFreq,realBw;
			if (!IRA::CIRATools::skyFrequency(freqs[i],bandwidths[i],m_startFreq[ifs[i]],m_bandWidth[ifs[i]],realFreq,realBw)) {
				realFreq=m_startFreq[ifs[i]];
				realBw=0.0;
			}
			resFreq[i]=realFreq+m_LO[ifs[i]];
			resBw[i]=realBw;
			if (m_pols[ifs[i]]==Receivers::RCV_LCP) {
				result[i]=8.8; //left
			}
			else {
				result[i]=11.3; //right
			}
		}
	}	
	else {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::getCalibrationMark()");
		impl.setReason("Receiver not configured yet");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
}

void  CRecvBossCore::updateRecvStatus() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{

}

void CRecvBossCore::getPolarization(ACS::longSeq& pol) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	pol.length(m_IFs);
	for (long i=0;i<m_IFs;i++) pol[i]=m_pols[i];	
}

void CRecvBossCore::getLO(ACS::doubleSeq& lo) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	lo.length(m_IFs);
	for (long i=0;i<m_IFs;i++) lo[i]=m_LO[i];
}

void CRecvBossCore::getBandWidth(ACS::doubleSeq& bw)  throw  (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	bw.length(m_IFs);
	for (long i=0;i<m_IFs;i++) bw[i]=m_bandWidth[i];
}

void CRecvBossCore::getInitialFrequency(ACS::doubleSeq& iFreq)  throw  (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	iFreq.length(m_IFs);
	for (long i=0;i<m_IFs;i++) iFreq[i]=m_startFreq[i];	
}

void CRecvBossCore::getFeeds(long& feeds) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	feeds=m_feeds;
}

void CRecvBossCore::getIFs(long& ifs) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	ifs=m_IFs;
}

const IRA::CString& CRecvBossCore::getRecvCode()
{
	return m_currentReceiver;
}

const IRA::CString& CRecvBossCore::getOperativeMode() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	return m_currentOperativeMode;
}

const Management::TSystemStatus& CRecvBossCore::getStatus()
{
	return m_status;
}

#else

CRecvBossCore::CRecvBossCore()
{
}

CRecvBossCore::~CRecvBossCore()
{
}

void CRecvBossCore::initialize(maci::ContainerServices* services,CConfiguration *config)
{
	m_status=Management::MNG_OK;
	m_bossStatus=Management::MNG_OK;
	m_recvStatus=Management::MNG_WARNING;
	m_currentRecv=Receivers::Receiver::_nil();
	m_currentRecvError=false;
	m_currentRecvInstance="";
	m_currentRecvCode="";
	m_currentOperativeMode="";
	m_feeds=m_IFs=0;
	m_services=services;
	m_config=config;
	m_lastStatusChange=0;
	m_lo.length(0);
	m_startFreq.length(0);
	m_bandWidth.length(0);
	m_pol.length(0);
	m_loEpoch=m_starFreqEpoch=m_bandWidthEpoch=m_polEpoch=m_feedsEpoch=m_IFsEpoch=m_modeEpoch=m_recvStatusEpoch=0;
}

void CRecvBossCore::execute() throw (ComponentErrors::IRALibraryResourceExImpl,ComponentErrors::CDBAccessExImpl)
{
}

void CRecvBossCore::cleanUp()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	unloadReceiver();
	m_currentRecvCode="";
	m_currentRecvInstance="";
}

void CRecvBossCore::calOn() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
				ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentRecvCode=="") {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::calOn()");
		impl.setReason("Receiver not configured yet");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
	try {
		m_currentRecv->calOn();
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::calOn()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::calOn()");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (ReceiversErrors::ReceiversErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::calOn()");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::calOn()");
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
}


void CRecvBossCore::AUOn() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
				ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentRecvCode=="") {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::AUOn()");
		impl.setReason("Receiver not configured yet");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
	try {
		m_currentRecv->turnAntennaUnitOn();
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::AUOn()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::AUOn()");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (ReceiversErrors::ReceiversErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::AUOn()");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::AUOn()");
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
}

void CRecvBossCore::AUOff() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
				ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentRecvCode=="") {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::AUOff()");
		impl.setReason("Receiver not configured yet");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
	try {
		m_currentRecv->turnAntennaUnitOff();
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::AUOff()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::AUOff()");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (ReceiversErrors::ReceiversErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::AUOff()");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::AUOff()");
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
}

void CRecvBossCore::calOff() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
				ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentRecvCode=="") {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::calOff()");
		impl.setReason("Receiver not configured yet");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
	try {
		m_currentRecv->calOff();
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::calOff()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::calOf()f");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (ReceiversErrors::ReceiversErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::calOff()");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::calOn()");
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
}

void CRecvBossCore::setLO(const ACS::doubleSeq& lo) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentRecvCode=="") {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::setLO()");
		impl.setReason("Receiver not configured yet");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
	try {
		m_currentRecv->setLO(lo);
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::setLO()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::setLO()");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (ReceiversErrors::ReceiversErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::setLO()");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::setLO()");
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
}

void CRecvBossCore::setMode(const char * mode) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::ModeErrorExImpl,
		ComponentErrors::UnexpectedExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,ComponentErrors::CORBAProblemExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentRecvCode=="") {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::setMode()");
		impl.setReason("Receiver not configured yet");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
	try {
		m_currentRecv->setMode(mode);
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::setMode()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::setMode");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (ReceiversErrors::ReceiversErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::setMode");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::setMode()");
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
}

void CRecvBossCore::setupReceiver(const char * code) throw (ManagementErrors::ConfigurationErrorExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	try {
		setup(code);
	}
	catch(ACSErr::ACSbaseExImpl& ex) {
		_ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl,impl,ex,"CRecvBossCore::setupReceiver()");
		impl.setSubsystem("Receivers");
		throw impl;
	}
}

void CRecvBossCore::park()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (!CORBA::is_nil(m_currentRecv)) {
		m_currentRecv->deactivate();
	}
	unloadReceiver();
	m_currentRecvCode="";
	m_currentRecvInstance="";
	m_recvStatus=Management::MNG_WARNING;
	m_lo.length(0);
	m_startFreq.length(0);
	m_bandWidth.length(0);
	m_pol.length(0);
	m_currentOperativeMode="";
	m_feeds=m_IFs=0;
	m_loEpoch=m_starFreqEpoch=m_bandWidthEpoch=m_polEpoch=m_feedsEpoch=m_IFsEpoch=m_modeEpoch=m_recvStatusEpoch=0;
}

double CRecvBossCore::getTaper(const double& freq,const double& bw,const long& feed,const long& ifNumber,double& waveLen) throw (ComponentErrors::ValidationErrorExImpl,
		ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentRecvCode=="") {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::getTaper()");
		impl.setReason("Receiver not configured yet");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
	try {
		CORBA::Double fq,ww,wl,taper;
		CORBA::Long fe,ifn;
		fq=(CORBA::Double)freq; ww=(CORBA::Double)bw; fe=(CORBA::Long)feed; ifn=(CORBA::Long)ifNumber;
		taper=m_currentRecv->getTaper(fq,ww,fe,ifn,wl);
		waveLen=(double)wl;
		return  (double)taper;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::getTaper()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::getTaper");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (ReceiversErrors::ReceiversErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::getTaper");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::getTaper()");
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
}

long CRecvBossCore::getFeeds(ACS::doubleSeq& X,ACS::doubleSeq& Y,ACS::doubleSeq& power) throw (ComponentErrors::ValidationErrorExImpl,
		ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentRecvCode=="") {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::getFeeds()");
		impl.setReason("Receiver not configured yet");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
	try {
		CORBA::Long len;
		ACS::doubleSeq_var tempX=new ACS::doubleSeq;
		ACS::doubleSeq_var tempY=new ACS::doubleSeq;
		ACS::doubleSeq_var tempPower=new ACS::doubleSeq;
		len=m_currentRecv->getFeeds(tempX,tempY,tempPower);
		X.length(tempX->length()); Y.length(tempY->length()); power.length(tempPower->length());
		for (long i=0;i<len;i++) {
			Y[i]=tempY[i]; X[i]=tempX[i]; power[i]=tempPower[i];
		}
		return  (long)len;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::getFeeds()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::getFeeds");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (ReceiversErrors::ReceiversErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::getFeeds");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::getFeeds()");
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
}

void CRecvBossCore::getCalibrationMark(ACS::doubleSeq& result,ACS::doubleSeq& resFreq,ACS::doubleSeq& resBw,const ACS::doubleSeq& freqs,const ACS::doubleSeq& bandwidths,
		const ACS::longSeq& feeds,const ACS::longSeq& ifs,double &scale) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CORBAProblemExImpl,
				ReceiversErrors::UnavailableReceiverOperationExImpl,ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentRecvCode=="") {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::getCalibrationMark()");
		impl.setReason("Receiver not configured yet");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
	try {
		ACS::doubleSeq_var tempRes=new ACS::doubleSeq;
		ACS::doubleSeq_var tempFreq=new ACS::doubleSeq;
		ACS::doubleSeq_var tempBw=new ACS::doubleSeq;
		tempRes=m_currentRecv->getCalibrationMark(freqs,bandwidths,feeds,ifs,tempFreq.out(),tempBw.out(),scale);
		result.length(tempRes->length());
		resFreq.length(tempRes->length());
		resBw.length(tempRes->length());
		for (unsigned i=0;i<result.length();i++) {
			result[i]=tempRes[i];
			resFreq[i]=tempFreq[i];
			resBw[i]=tempBw[i];
		}
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::getCalibrationMark()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::getCalibrationMark");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (ReceiversErrors::ReceiversErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::getCalibrationMark");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::getCalibrationMark()");
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
}

void CRecvBossCore::getPolarization(ACS::longSeq& pol) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	TIMEVALUE now;
	pol.length(0);
	if (m_currentRecvCode=="") {
		return;
	}
	IRA::CIRATools::getTime(now);
	if (now.value().value>m_loEpoch+m_config->propertyUpdateTime()*10) {
		ACS::ROlongSeq_var polRef;
		ACSErr::Completion_var comp;
		ACS::longSeq_var val;
		loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
		try {
			polRef=m_currentRecv->polarization();
			val=polRef->get_sync(comp.out());
		}
		catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::getPolarization()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				changeBossStatus(Management::MNG_FAILURE);
				m_currentRecvError=true;
				throw impl;
		}
		ACSErr::CompletionImpl compImpl(comp);
		if (compImpl.isErrorFree()) {
			pol.length(val->length());
			m_pol.length(val->length());
			for (unsigned i=0;i<pol.length();i++) {
				pol[i]=val[i];
				m_pol[i]=val[i];
			}
			m_polEpoch=now.value().value;
		}
		else {
			ReceiversErrors::UnavailableReceiverAttributeExImpl ex(compImpl,__FILE__,__LINE__,"CRecvBossCore::getPolarization()");
			ex.setReceiverCode((const char *)m_currentRecvCode);
			throw ex;
		}
	}
	else {
		pol.length(m_pol.length());
		for (unsigned i=0;i<m_pol.length();i++) pol[i]=m_pol[i];
	}
}

void CRecvBossCore::getLO(ACS::doubleSeq& lo) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	TIMEVALUE now;
	lo.length(0);
	if (m_currentRecvCode=="") {
		return;
	}
	IRA::CIRATools::getTime(now);
	if (now.value().value>m_loEpoch+m_config->propertyUpdateTime()*10) {
		ACS::ROdoubleSeq_var loRef;
		ACSErr::Completion_var comp;
		ACS::doubleSeq_var val;
		loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
		try {
			loRef=m_currentRecv->LO();
			val=loRef->get_sync(comp.out());
		}
		catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::getLO()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				changeBossStatus(Management::MNG_FAILURE);
				m_currentRecvError=true;
				throw impl;
		}
		ACSErr::CompletionImpl compImpl(comp);
		if (compImpl.isErrorFree()) {
			lo.length(val->length());
			m_lo.length(val->length());
			for (unsigned i=0;i<lo.length();i++) {
				lo[i]=val[i];
				m_lo[i]=val[i];
			}
			m_loEpoch=now.value().value;
		}
		else {
			ReceiversErrors::UnavailableReceiverAttributeExImpl ex(compImpl,__FILE__,__LINE__,"CRecvBossCore::getLO()");
			ex.setReceiverCode((const char *)m_currentRecvCode);
			throw ex;
		}
	}
	else {
		lo.length(m_lo.length());
		for (unsigned i=0;i<m_lo.length();i++) lo[i]=m_lo[i];
	}
}

void CRecvBossCore::getBandWidth(ACS::doubleSeq& bw) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	TIMEVALUE now;
	bw.length(0);
	if (m_currentRecvCode=="") {
		return;
	}
	IRA::CIRATools::getTime(now);
	if (now.value().value>m_bandWidthEpoch+m_config->propertyUpdateTime()*10) {
		ACS::ROdoubleSeq_var bwRef;
		ACSErr::Completion_var comp;
		ACS::doubleSeq_var val;
		loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
		try {
			bwRef=m_currentRecv->bandWidth();
			val=bwRef->get_sync(comp.out());
		}
		catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::getBandWidth()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				changeBossStatus(Management::MNG_FAILURE);
				m_currentRecvError=true;
				throw impl;
		}
		ACSErr::CompletionImpl compImpl(comp);
		if (compImpl.isErrorFree()) {
			bw.length(val->length());
			m_bandWidth.length(val->length());
			for (unsigned i=0;i<bw.length();i++) {
				bw[i]=val[i];
				m_bandWidth[i]=val[i];
			}
			m_bandWidthEpoch=now.value().value;
		}
		else {
			ReceiversErrors::UnavailableReceiverAttributeExImpl ex(compImpl,__FILE__,__LINE__,"CRecvBossCore::getBandWidth()");
			ex.setReceiverCode((const char *)m_currentRecvCode);
			throw ex;
		}
	}
	else {
		bw.length(m_bandWidth.length());
		for (unsigned i=0;i<m_bandWidth.length();i++) bw[i]=m_bandWidth[i];
	}
}

void CRecvBossCore::getInitialFrequency(ACS::doubleSeq& iFreq) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	TIMEVALUE now;
	iFreq.length(0);
	if (m_currentRecvCode=="") {
		return;
	}
	IRA::CIRATools::getTime(now);
	if (now.value().value>m_starFreqEpoch+m_config->propertyUpdateTime()*10) {
		ACS::ROdoubleSeq_var ifreqRef;
		ACSErr::Completion_var comp;
		ACS::doubleSeq_var val;
		loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
		try {
			ifreqRef=m_currentRecv->initialFrequency();
			val=ifreqRef->get_sync(comp.out());
		}
		catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::getInitialFrequency()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				changeBossStatus(Management::MNG_FAILURE);
				m_currentRecvError=true;
				throw impl;
		}
		ACSErr::CompletionImpl compImpl(comp);
		if (compImpl.isErrorFree()) {
			iFreq.length(val->length());
			m_startFreq.length(val->length());
			for (unsigned i=0;i<iFreq.length();i++) {
				iFreq[i]=val[i];
				m_startFreq[i]=val[i];
			}
			m_starFreqEpoch=now.value().value;
		}
		else {
			ReceiversErrors::UnavailableReceiverAttributeExImpl ex(compImpl,__FILE__,__LINE__,"CRecvBossCore::getInitialFrequency()");
			ex.setReceiverCode((const char *)m_currentRecvCode);
			throw ex;
		}
	}
	else {
		iFreq.length(m_startFreq.length());
		for (unsigned i=0;i<m_startFreq.length();i++) iFreq[i]=m_startFreq[i];
	}
}

void CRecvBossCore::getFeeds(long& feeds) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	TIMEVALUE now;
	feeds=0;
	if (m_currentRecvCode=="") {
		return;
	}
	IRA::CIRATools::getTime(now);
	if (now.value().value>m_feedsEpoch+m_config->propertyUpdateTime()*10) {
		ACS::ROlong_var feedsRef;
		ACSErr::Completion_var comp;
		CORBA::Long val;
		loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
		try {
			feedsRef=m_currentRecv->feeds();
			val=feedsRef->get_sync(comp.out());
		}
		catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::getFeeds()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				changeBossStatus(Management::MNG_FAILURE);
				m_currentRecvError=true;
				throw impl;
		}
		ACSErr::CompletionImpl compImpl(comp);
		if (compImpl.isErrorFree()) {
			feeds=val;
			m_feeds=val;
			m_feedsEpoch=now.value().value;
		}
		else {
			ReceiversErrors::UnavailableReceiverAttributeExImpl ex(compImpl,__FILE__,__LINE__,"CRecvBossCore::getFeeds()");
			ex.setReceiverCode((const char *)m_currentRecvCode);
			throw ex;
		}
	}
	else {
		feeds=m_feeds;
	}
}

void CRecvBossCore::getIFs(long& ifs) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	TIMEVALUE now;
	ifs=0;
	if (m_currentRecvCode=="") {
		return;
	}
	IRA::CIRATools::getTime(now);
	if (now.value().value>m_IFsEpoch+m_config->propertyUpdateTime()*10) {
		ACS::ROlong_var ifsRef;
		ACSErr::Completion_var comp;
		CORBA::Long val;
		loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
		try {
			ifsRef=m_currentRecv->IFs();
			val=ifsRef->get_sync(comp.out());
		}
		catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::getIFs()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				changeBossStatus(Management::MNG_FAILURE);
				m_currentRecvError=true;
				throw impl;
		}
		ACSErr::CompletionImpl compImpl(comp);
		if (compImpl.isErrorFree()) {
			ifs=val;
			m_IFs=val;
			m_IFsEpoch=now.value().value;
		}
		else {
			ReceiversErrors::UnavailableReceiverAttributeExImpl ex(compImpl,__FILE__,__LINE__,"CRecvBossCore::getIFs()");
			ex.setReceiverCode((const char *)m_currentRecvCode);
			throw ex;
		}
	}
	else {
		ifs=m_IFs;
	}
}

const IRA::CString& CRecvBossCore::getRecvCode()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	return m_currentRecvCode;
}

const IRA::CString& CRecvBossCore::getOperativeMode() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	TIMEVALUE now;
	if (m_currentRecvCode=="") {
		m_currentOperativeMode="";
		return m_currentOperativeMode;
	}
	IRA::CIRATools::getTime(now);
	if (now.value().value>m_modeEpoch+m_config->propertyUpdateTime()*10) {
		ACS::ROstring_var modeRef;
		ACSErr::Completion_var comp;
		CORBA::String_var val;
		loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
		try {
			modeRef=m_currentRecv->mode();
			val=modeRef->get_sync(comp.out());
		}
		catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::getOperativeMode()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				changeBossStatus(Management::MNG_FAILURE);
				m_currentRecvError=true;
				throw impl;
		}
		ACSErr::CompletionImpl compImpl(comp);
		if (compImpl.isErrorFree()) {
			m_currentOperativeMode=IRA::CString((const char *)val);
			m_modeEpoch=now.value().value;
			return m_currentOperativeMode;
		}
		else {
			ReceiversErrors::UnavailableReceiverAttributeExImpl ex(compImpl,__FILE__,__LINE__,"CRecvBossCore::getOperativeMode()");
			ex.setReceiverCode((const char *)m_currentRecvCode);
			throw ex;
		}
	}
	else {
		return m_currentOperativeMode;
	}
}

void  CRecvBossCore::updateRecvStatus() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	TIMEVALUE now;
	if (m_currentRecvCode!="") {
		IRA::CIRATools::getTime(now);
		if (now.value().value>m_recvStatusEpoch+m_config->propertyUpdateTime()*10) {
			Management::ROTSystemStatus_var stRef;
			Management::TSystemStatus val;
			ACSErr::Completion_var comp;
			loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
			try {
				stRef=m_currentRecv->receiverStatus();
				val=stRef->get_sync(comp.out());
			}
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::getRecvStatus()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				changeBossStatus(Management::MNG_FAILURE);
				m_currentRecvError=true;
				throw impl;
			}
			ACSErr::CompletionImpl compImpl(comp);
			if (compImpl.isErrorFree()) {
				m_recvStatus=val;
				m_recvStatusEpoch=now.value().value;
			}
			else {
				ReceiversErrors::UnavailableReceiverAttributeExImpl ex(compImpl,__FILE__,__LINE__,"CRecvBossCore::getRecvStatus()");
				ex.setReceiverCode((const char *)m_currentRecvCode);
				throw ex;
			}
		}
	}
	else {  // if no receiver configured yet, it's status is supposed to be WARNING
		m_recvStatus=Management::MNG_WARNING;
	}
}

const Management::TSystemStatus& CRecvBossCore::getStatus()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	//first of all try to clear the boss status...
	changeBossStatus(Management::MNG_OK);; // it will be cleared only if the status persistence time is elapsed
	if (m_bossStatus>m_recvStatus) {
		m_status=m_bossStatus;
	}
	else {
		m_status=m_recvStatus;
	}
	return m_status;
}


//************ PRIVATE ****************************

void CRecvBossCore::setup(const char * code) throw (ComponentErrors::CORBAProblemExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl)
{
	IRA::CString component;
	if (!m_config->getReceiver(code,component)) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::setup()");
		impl.setReason("Receiver code is not known");
		throw impl;
	}
	//deactivate current receiver.....
	if (!CORBA::is_nil(m_currentRecv)) {
		m_currentRecv->deactivate();
	}
	unloadReceiver();
	m_currentRecvCode=code;
	m_currentRecvInstance=component;
	loadReceiver(); // throw ComponentErrors::CouldntGetComponentExImpl
	try {
		m_currentRecv->activate();
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CRecvBossCore::setup()");
		impl.setReason("Unable to activate receiver");
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::setup()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::setup()");
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
	changeBossStatus(Management::MNG_OK);
	ACS_LOG(LM_FULL_INFO,"CRecvBossCore::setup()",(LM_NOTICE,"NEW_RECEIVER_CONFIGURED %s",code));
}


void CRecvBossCore::loadReceiver() throw (ComponentErrors::CouldntGetComponentExImpl)
{
	if ((!CORBA::is_nil(m_currentRecv)) && (m_currentRecvError)) { // if reference was already taken, but an error was found....dispose the reference
		try {
			m_services->releaseComponent((const char*)m_currentRecvInstance);
		}
		catch (...) { //dispose silently...if an error...no matter
		}
		m_currentRecv=Receivers::Receiver::_nil();
		m_currentRecvError=false;
	}
	if (CORBA::is_nil(m_currentRecv)) {  //only if it has not been retrieved yet
		try {
			m_currentRecv=m_services->getComponent<Receivers::Receiver>((const char*)m_currentRecvInstance);
			ACS_LOG(LM_FULL_INFO,"CRecvBossCore::loadReceiver()",(LM_INFO,"RECEIVER_OBTAINED"));
			m_currentRecvError=false;
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CRecvBossCore::loadReceiver()");
			Impl.setComponentName((const char*)m_currentRecvInstance);
			m_currentRecv=Receivers::Receiver::_nil();
			changeBossStatus(Management::MNG_FAILURE);
			throw Impl;
		}
		catch (maciErrType::NoPermissionExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CRecvBossCore::loadReceiver()");
			Impl.setComponentName((const char*)m_currentRecvInstance);
			m_currentRecv=Receivers::Receiver::_nil();
			changeBossStatus(Management::MNG_FAILURE);
			throw Impl;
		}
		catch (maciErrType::NoDefaultComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CRecvBossCore::loadReceiver()");
			Impl.setComponentName((const char*)m_currentRecvInstance);
			m_currentRecv=Receivers::Receiver::_nil();
			changeBossStatus(Management::MNG_FAILURE);
			throw Impl;
		}
	}
}

void CRecvBossCore::unloadReceiver()
{
	if (!CORBA::is_nil(m_currentRecv)) {
		try {
			m_services->releaseComponent((const char*)m_currentRecvInstance);
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CRecvBossCore::unloadReceiver()");
			Impl.setComponentName((const char *)m_currentRecvInstance);
			Impl.log(LM_WARNING);
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::unloadReceiver()");
			impl.log(LM_WARNING);
		}
		m_currentRecv=Receivers::Receiver::_nil();
		m_currentRecvError=false;
	}
}

void CRecvBossCore::changeBossStatus(const Management::TSystemStatus& status)
{
	if (status>=m_bossStatus) { // if the new state has an higher priority...update the status
		TIMEVALUE now;
		m_bossStatus=status;
		IRA::CIRATools::getTime(now);
		m_lastStatusChange=now.value().value;
	}
	else { // if the priority is lower, it is  updated only if the previous error is cleared (happened more than StatusPersistenceTime ago)
		TIMEVALUE now;
		IRA::CIRATools::getTime(now);
		if ((now.value().value-m_lastStatusChange)>(m_config->getStatusPersistenceTime()*10)) {
			m_bossStatus=status;
			IRA::CIRATools::getTime(now);
			m_lastStatusChange=now.value().value;
		}
	}
}


#endif

