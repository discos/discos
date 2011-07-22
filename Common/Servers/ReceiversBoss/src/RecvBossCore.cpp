// $Id: RecvBossCore.cpp,v 1.9 2011-03-04 16:31:58 a.orlati Exp $

#include "RecvBossCore.h"

//#define RB_DEBUG

#define KKC_ADDRESS "192.168.51.4" // this is the PortServer installed directly in the MF
#define KKC_PORT 2101 // firt port...please notice that this works only if the port is configuread as "real Port" 
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

void CRecvBossCore::initialize(maci::ContainerServices* services)
{
	m_currentReceiver="";
	m_currentOperativeMode="";
	m_recvOpened=false;
	m_fsOpened=false;
	m_LO[0]=m_LO[1]=0.0;
	m_status=Management::MNG_OK;
	m_services=services;
	m_feeds=0;
	m_IFs=0;
	m_startFreq[0]=m_startFreq[1]=0.0;
	m_bandWidth[0]=m_bandWidth[1]=0.0;
}

void CRecvBossCore::execute() throw (ComponentErrors::IRALibraryResourceExImpl,ComponentErrors::CDBAccessExImpl)
{
	CError error;
	m_KKCFeedTable=new IRA::CDBTable(m_services,"Feed","DataBlock/KKCReceiver/Feeds");
	error.Reset();
	if (!m_KKCFeedTable->addField(error,"feedCode",CDataField::LONGLONG)) {
		error.setExtra("feedCode field not found",0);
 	}
	else if (!m_KKCFeedTable->addField(error,"xOffset",CDataField::DOUBLE)) {
		error.setExtra("xOffset field not found",0);
 	}
	else if (!m_KKCFeedTable->addField(error,"yOffset",CDataField::DOUBLE)) {
		error.setExtra("yOffset field not found",0);
 	}
	else if (!m_KKCFeedTable->addField(error,"relativePower",CDataField::DOUBLE)) {
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

void CRecvBossCore::calOn() throw (ComponentErrors::SocketErrorExImpl,ComponentErrors::ValidationErrorExImpl)
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

void CRecvBossCore::calOff() throw (ComponentErrors::SocketErrorExImpl,ComponentErrors::ValidationErrorExImpl)
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

void CRecvBossCore::setLO(const ACS::doubleSeq& lo) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl)
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
	//***********************************************************************/
	// range checks must be performed, also support for lo=-1 (keep the present value must be introduced)
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
		m_pols[0]=Receivers::RCV_LEFT;
		m_pols[1]=Receivers::RCV_RIGHT;
		m_startFreq[0]=100.0;
		m_startFreq[1]=100.0;
		m_bandWidth[0]=2000.0;
		m_bandWidth[1]=2000.0;
		m_currentReceiver="KKC";
		m_currentOperativeMode="";
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
		m_pols[0]=Receivers::RCV_LEFT;
		m_pols[1]=Receivers::RCV_RIGHT;
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
#endif		
		m_LO[0]=8080.0;
		m_LO[1]=8080.0;
		m_IFs=2;
		m_feeds=1;
		m_pols[0]=Receivers::RCV_RIGHT;
		m_pols[1]=Receivers::RCV_LEFT;
		m_startFreq[0]=100.0;
		m_startFreq[1]=100.0;
		m_bandWidth[0]=800.0;
		m_bandWidth[1]=800.0;		
		m_currentReceiver="XXP";
		m_currentOperativeMode="";
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

void CRecvBossCore::setMode(const char * mode) throw (ComponentErrors::ValidationErrorExImpl,ManagementErrors::ConfigurationErrorExImpl)
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
			_EXCPT(ManagementErrors::ConfigurationErrorExImpl,impl,"CRecvBossCore::setMode()");
			impl.setReason("illegal operative mode");
			impl.setSubsystem("Receivers");
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

double CRecvBossCore::getTaper(const double& freq,const double& bw,const long& feed,const long& ifNumber,double& waveLen) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl)
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

long CRecvBossCore::getFeeds(ACS::doubleSeq& X,ACS::doubleSeq& Y,ACS::doubleSeq& power) throw (ComponentErrors::ValidationErrorExImpl)
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

void CRecvBossCore::getCalibrationMark(ACS::doubleSeq& result,const ACS::doubleSeq& freqs,const ACS::doubleSeq& bandwidths,const ACS::longSeq& feeds,const ACS::longSeq& ifs) throw (
		ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	unsigned stdLen=freqs.length();
	if ((stdLen!=bandwidths.length()) || (stdLen!=feeds.length()) || (stdLen!=ifs.length())) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::getCalibrationMark()");
		impl.setReason("sub-bands definition is not consistent");
		m_status=Management::MNG_WARNING;
		throw impl;			
	}
	result.length(stdLen);
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
	if (m_currentReceiver=="KKC") {
		double LeftMarkCoeff[7][4] = { {0.2912,-21.21,506.97,-3955.5}, {0.558,-40.436,961.81,-7472.1}, {0.8963,-63.274,1469.2,-11170.0}, 
									 {0.5176,-37.47,889.81,-6895.8}, {0.662,-47.86,1136.2,-8809.9}, {0.3535,-25.63,609.53,-4727.4}, {0.2725,-19.926,478.05,-3737.9}  };
		double RightMarkCoeff[7][4] = { {0.2141,-15.771,379.96,-2976.8}, {0.523,-37.88,899.74,-6974.4}, {0.8572,-60.324,1396.3,-10581.0}, 
									 {0.5173,-37.339,884.31,-6838.1}, {0.7267,-52.213,1231.4,-9486.9}, {0.3955,-28.468,672.77,-5192.0}, {0.201,-15.761,398.52,-3237.7} };
		double f1,f2;
		double integral;
		double mark=0;
		double realFreq,realBw;
		for (unsigned i=0;i<stdLen;i++) {
			if (m_pols[ifs[i]]==Receivers::RCV_LEFT) {
				// take the real observed bandwidth....the correlation between detector device and the band provided by the receiver
				if (!IRA::CIRATools::skyFrequency(freqs[i],bandwidths[i],m_startFreq[ifs[i]],m_bandWidth[ifs[i]],realFreq,realBw)) {
					realFreq=m_startFreq[ifs[i]];
					realBw=0.0;
				}				
				f1=realFreq+m_LO[ifs[i]]; 
				f2=f1+realBw;
				f1/=1000.0; f2/=1000.0; //frequencies in giga Hertz
				integral=(LeftMarkCoeff[feeds[i]][0]/4)*(f2*f2*f2*f2-f1*f1*f1*f1)+(LeftMarkCoeff[feeds[i]][1]/3)*(f2*f2*f2-f1*f1*f1)+(LeftMarkCoeff[feeds[i]][2]/2)*(f2*f2-f1*f1)+LeftMarkCoeff[feeds[i]][3]*(f2-f1);				
				mark=integral/(f2-f1);				
			}
			else if (m_pols[ifs[i]]==Receivers::RCV_RIGHT) {
				// take the real observed bandwidth....the correlation between detectro device and the band provided by the receiver
				if (!IRA::CIRATools::skyFrequency(freqs[i],bandwidths[i],m_startFreq[ifs[i]],m_bandWidth[ifs[i]],realFreq,realBw)) {
					realFreq=m_startFreq[ifs[i]];
					realBw=0.0;
				}								
				f1=realFreq+m_LO[ifs[i]]; 
				f2=f1+realBw;
				f1/=1000.0; f2/=1000.0; //frequencies in giga Hertz
				integral=(RightMarkCoeff[feeds[i]][0]/4)*(f2*f2*f2*f2-f1*f1*f1*f1)+(RightMarkCoeff[feeds[i]][1]/3)*(f2*f2*f2-f1*f1*f1)+(RightMarkCoeff[feeds[i]][2]/2)*(f2*f2-f1*f1)+RightMarkCoeff[feeds[i]][3]*(f2-f1);				
				mark=integral/(f2-f1);
			}
			result[i]=mark;
		}
	}
	else if (m_currentReceiver=="CCC") { 
		result.length(stdLen);
		for (unsigned i=0;i<stdLen;i++) {
			if (m_pols[ifs[i]]==Receivers::RCV_LEFT) {
				result[i]=5.88; //left
			}
			else {
				result[i]=5.71; //right
			}
		}
	}
	else if (m_currentReceiver=="XXP") { 
		result.length(stdLen);
		for (unsigned i=0;i<stdLen;i++) {
			if (m_pols[ifs[i]]==Receivers::RCV_LEFT) {
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

void CRecvBossCore::getPolarization(ACS::longSeq& pol)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	pol.length(m_IFs);
	for (long i=0;i<m_IFs;i++) pol[i]=m_pols[i];	
}

void CRecvBossCore::getLO(ACS::doubleSeq& lo)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	lo.length(m_IFs);
	for (long i=0;i<m_IFs;i++) lo[i]=m_LO[i];
}

void CRecvBossCore::getBandWidth(ACS::doubleSeq& bw)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	bw.length(m_IFs);
	for (long i=0;i<m_IFs;i++) bw[i]=m_bandWidth[i];
}

void CRecvBossCore::getInitialFrequency(ACS::doubleSeq& iFreq)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	iFreq.length(m_IFs);
	for (long i=0;i<m_IFs;i++) iFreq[i]=m_startFreq[i];	
}

void CRecvBossCore::getFeeds(long& feeds)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	feeds=m_feeds;
}

void CRecvBossCore::getIFs(long& ifs)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	ifs=m_IFs;
}


