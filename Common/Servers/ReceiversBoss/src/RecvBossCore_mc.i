//#define RB_DEBUG

//#define KKC_ADDRESS "192.168.51.13" // this is the PortServer installed directly in the MF
#define KKC_ADDRESS "192.167.189.102" // this is the server installed in control room PC
//#define KKC_PORT 2101 // first port...please notice that this works only if the port is configured as "real Port"
#define KKC_PORT 10000 // control room server port
#define RECV_ADDRESS "192.167.189.2"
#define RECV_PORT 2096
#define FS_ADDRESS "192.167.189.62"
#define FS_PORT 5002

// speed of light in meters per second
#define LIGHTSPEED 299792458.0

CRecvBossCore::CRecvBossCore()
{
}

CRecvBossCore::~CRecvBossCore()
{
}

void CRecvBossCore::initialize(maci::ContainerServices* services,CConfiguration *config,acscomponent::ACSComponentImpl *me)
	throw (ComponentErrors::UnexpectedExImpl)
{
	m_currentReceiver="";
	m_currentOperativeMode="";
	m_recvOpened=false;
	m_fsOpened=false;
	m_status=Management::MNG_WARNING;
	m_services=services;
	m_feeds=0;
	m_IFs=0;
	for (unsigned i=0;i<_RECVBOSSCORE_MAX_IFS;i++) {
		m_startFreq[i]=0.0;
		m_bandWidth[0]=0.0;
		m_LO[i]=0.0;
	}
	m_config=config;
	m_totalOutputs=0;
	m_notificationChannel=NULL;

	ACS_LOG(LM_FULL_INFO,"CRecvBossCore::initialize()",(LM_INFO,"OPENING_RECEIVERS_BOSS_NOTIFICATION_CHANNEL"));
	try {
		m_notificationChannel=new nc::SimpleSupplier(Receivers::RECEIVERS_DATA_CHANNEL,me);
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CRecvBossCore::initialize()");
	}
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
	if (m_notificationChannel!=NULL) {
		m_notificationChannel->disconnect();
		m_notificationChannel=NULL;
	}
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
#ifndef RB_DEBUG
	char buff [10];
	WORD len;
	double loAmp[4]={-55.24524,11.41288,-0.79437,0.01894};
#endif
	IRA::CError err;
	IRA::CString msg;
	double trueValue;

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
		for (long i=0;i<m_totalOutputs;i++) {
			m_LO[i]=lo[0];
		}
		//m_LO[0]=m_LO[1]=lo[0];
		ACS_LOG(LM_FULL_INFO,"CRecvBossCore::setLO()",(LM_NOTICE,"LOCAL_OSCILLATOR: %lf",m_LO[0]));
		//trueValue/=1000;
		//double amplitude=loAmp[0]+loAmp[1]*trueValue+loAmp[2]*trueValue*trueValue+loAmp[3]*trueValue*trueValue*trueValue;
		//printf("trueValue, amp: %lf %lf\n",trueValue,amplitude);
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
	IRA::CIRATools::Wait(0,500000);  //wait half a second to settle things down
	if (m_currentReceiver=="KKC") {
		trueValue/=1000;
		double amplitude=loAmp[0]+loAmp[1]*trueValue+loAmp[2]*trueValue*trueValue+loAmp[3]*trueValue*trueValue*trueValue;
		msg.Format("set loamp %lf\n",(double)amplitude);
		//printf("MANDO: %s\n",(const char *)msg);
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
		//printf("RICEVO: %s\n",buff);
	}
#endif
}

void CRecvBossCore::setup(const char * code) throw (ComponentErrors::SocketErrorExImpl,ComponentErrors::ValidationErrorExImpl)
{
	IRA::CError err;
	IRA::CString rec(code);
	IRA::CString recvIpAddr(RECV_ADDRESS);
#ifndef RB_DEBUG
	DWORD recvPort=RECV_PORT;
	DWORD fsPort=FS_PORT;
#endif
	IRA::CString fsIpAddr(FS_ADDRESS);
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
	// Configure the communication for the receiver configuration
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
		// now set the subreflector configuration through the FS
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
		m_IFs=2;
		m_feeds=2;
		m_totalOutputs=m_IFs*m_feeds;
		for (long i=0;i<m_totalOutputs;i++) {
			m_LO[i]=21964.0;
			m_startFreq[i]=100.0;
			m_bandWidth[i]=2000.0;
			if ((i % 2)==0) m_pols[i]=Receivers::RCV_LCP;
			else m_pols[i]=Receivers::RCV_RCP;
		}
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
		m_totalOutputs=m_IFs*m_feeds;
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
		m_totalOutputs=m_IFs*m_feeds;
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

void CRecvBossCore::park() throw (ManagementErrors::ParkingErrorExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	m_currentReceiver="";
	m_currentOperativeMode="";
	m_IFs=0;
	m_feeds=0;
	m_totalOutputs=0;
}

double CRecvBossCore::getTaper(const double& freq,const double& bw,const long& feed,const long& ifNumber,double& waveLen) throw (ComponentErrors::ValidationErrorExImpl,
		ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,ComponentErrors::UnexpectedExImpl)
{
	double centralFreq;
	double taper;
	double primaryFreq[5]= { 1.4, 1.65, 2.28, 8.6, 23.0 };
	double primaryTaper[5]= { -24, -29, -19, -23, -25 };
	double secondaryFreq[7]= { 4.3, 5.05, 5.8, 6.3, 18, 22, 26 };
	double secondaryTaper[7]= { -9.5, -12, -14, -9.9, -7.1, -12.6, -18.9};
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
	long index=(feed*m_IFs)+ifNumber;
	// take the real observed bandwidth....the correlation between detector device and the band provided by the receiver
	if (!IRA::CIRATools::skyFrequency(freq,bw,m_startFreq[index],m_bandWidth[index],realFreq,realBw)) {
		realFreq=m_startFreq[index];
		realBw=0.0;
	}
	centralFreq=m_LO[index]+realFreq+realBw/2;
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

double CRecvBossCore::getDerotatorPosition (const ACS::Time& epoch) throw (ComponentErrors::CouldntGetComponentExImpl,
		ReceiversErrors::DewarPositionerCommandErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
{
	// no need to check anything
	return 0;
}

void CRecvBossCore::getDewarParameter(Receivers::TDerotatorConfigurations& mod,double& pos) throw (
  ReceiversErrors::DewarPositionerCommandErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
{
	mod=Receivers::RCV_UNDEF_DEROTCONF;
	pos=0.0;
}

void CRecvBossCore::derotatorMode(const Receivers::TDerotatorConfigurations& mode,const Receivers::TRewindModes& rewind) throw (
		ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,ComponentErrors::ValidationErrorExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
   		ReceiversErrors::DewarPositionerSetupErrorExImpl)
{
	// baci::ThreadSyncGuard guard(&m_mutex);
	// no support in ESCS for that
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorSetup()");
	m_status=Management::MNG_WARNING;
	throw impl;
}

void CRecvBossCore::setDerotatorPosition(const double& pos) throw (ReceiversErrors::NoDewarPositioningExImpl,
  ReceiversErrors::NoDerotatorAvailableExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,
  ReceiversErrors::DewarPositionerCommandErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
{
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::setDerotatorPosition()");
	m_status=Management::MNG_WARNING;
	throw impl;
}

void CRecvBossCore::derotatorRewind(const long& steps) throw (ComponentErrors::ValidationErrorExImpl,
  ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,
  ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::DewarPositionerCommandErrorExImpl,
  ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
{
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorRewind()");
	m_status=Management::MNG_WARNING;
	throw impl;
}

void CRecvBossCore::derotatorPark() throw (ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,
    			ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::DewarPositionerParkingErrorExImpl,
    			ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
{
	// baci::ThreadSyncGuard guard(&m_mutex);
	// no support in ESCs for that
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorPark()");
	m_status=Management::MNG_WARNING;
	throw impl;
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

void CRecvBossCore::getIFOutput(const ACS::longSeq& feeds,const ACS::longSeq& ifs,ACS::doubleSeq& freqs,ACS::doubleSeq&  bw,ACS::longSeq& pols,ACS::doubleSeq& LO)  throw (
		ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	unsigned stdLen=feeds.length();
	if (stdLen!=ifs.length()) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::getIFOutput()");
		impl.setReason("sub-bands definition is not consistent");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
	for (unsigned i=0;i<stdLen;i++) {
		if ((ifs[i]>=m_IFs) || (ifs[i]<0)) {
			_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CRecvBossCore::getIFOutput()");
			impl.setValueName("IF identifier");
			m_status=Management::MNG_WARNING;
			throw impl;
		}
	}
	for (unsigned i=0;i<stdLen;i++) {
		if ((feeds[i]>=m_feeds) || (feeds[i]<0)) {
			_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CRecvBossCore::getIFOutput()");
			impl.setValueName("feeds identifier");
			m_status=Management::MNG_WARNING;
			throw impl;
		}
	}
	freqs.length(stdLen);
	bw.length(stdLen);
	pols.length(stdLen);
	LO.length(stdLen);
	for (unsigned i=0;i<stdLen;i++) {
		long index=(feeds[i]*m_IFs)+ifs[i];
		freqs[i]=m_startFreq[index];
		bw[i]=m_bandWidth[index];
		pols[i]=m_pols[index];
		LO[i]=m_LO[index];
	}
}

void CRecvBossCore::CRecvBossCore::startScan(ACS::Time& startUT,const Receivers::TReceiversParameters& param,
  const Antenna::TRunTimeParameters& antennaInfo) throw(
  ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,
  ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::DewarPositionerCommandErrorExImpl)
{

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
		/*double LeftMarkCoeff[7][4] = { {0.2912,-21.21,506.97,-3955.5}, {0.558,-40.436,961.81,-7472.1}, {0.8963,-63.274,1469.2,-11170.0},
									 {0.5176,-37.47,889.81,-6895.8}, {0.662,-47.86,1136.2,-8809.9}, {0.3535,-25.63,609.53,-4727.4}, {0.2725,-19.926,478.05,-3737.9}  };
		double RightMarkCoeff[7][4] = { {0.2141,-15.771,379.96,-2976.8}, {0.523,-37.88,899.74,-6974.4}, {0.8572,-60.324,1396.3,-10581.0},
									 {0.5173,-37.339,884.31,-6838.1}, {0.7267,-52.213,1231.4,-9486.9}, {0.3955,-28.468,672.77,-5192.0}, {0.201,-15.761,398.52,-3237.7} };*/

		double LeftMarkCoeff[2][4] = { {0.35539,-25.57141,603.90321,-4656.81229}, {0.18548,-14.13379,349.77806,-2795.54717} };
		double RightMarkCoeff[2][4] = { {0.36656,-26.16018,613.14727,-4697.37959}, {0.32624,-23.20331,	542.14083,-4141.29465} };

		double f1,f2;
		double integral;
		double mark=0;
		double realFreq,realBw;
		for (unsigned i=0;i<stdLen;i++) {
			long index=(feeds[i]*m_IFs)+ifs[i];
			if (m_pols[index]==Receivers::RCV_LCP) {
				// take the real observed bandwidth....the correlation between detector device and the band provided by the receiver
				if (!IRA::CIRATools::skyFrequency(freqs[i],bandwidths[i],m_startFreq[index],m_bandWidth[index],realFreq,realBw)) {
					realFreq=m_startFreq[index];
					realBw=0.0;
				}
				realFreq+=m_LO[index];
				f1=realFreq;
				//f1+=realBw/2.0;
				//f1/=1000; //f in GHz
				//mark=LeftMarkCoeff[ feeds[i] ][0]*f1*f1*f1+LeftMarkCoeff[ feeds[i] ][1]*f1*f1+LeftMarkCoeff[ feeds[i] ][2]*f1+LeftMarkCoeff[ feeds[i] ][3];
				//printf("LEFT realFreq %lf, bw: %lf, f1:%lf",realFreq,realBw,f1);
				f2=f1+realBw;
				f1/=1000.0; f2/=1000.0; //frequencies in giga Hertz
				integral=(LeftMarkCoeff[feeds[i]][0]/4)*(f2*f2*f2*f2-f1*f1*f1*f1)+(LeftMarkCoeff[feeds[i]][1]/3)*(f2*f2*f2-f1*f1*f1)+(LeftMarkCoeff[feeds[i]][2]/2)*(f2*f2-f1*f1)+LeftMarkCoeff[feeds[i]][3]*(f2-f1);
				mark=integral/(f2-f1);
				//printf("f1,f2,mark,realFreq: %lf %lf %lf %lf\n",f1,f2,mark,realFreq);
			}
			else if (m_pols[index]==Receivers::RCV_RCP) {
				// take the real observed bandwidth....the correlation between detector device and the band provided by the receiver
				if (!IRA::CIRATools::skyFrequency(freqs[i],bandwidths[i],m_startFreq[index],m_bandWidth[index],realFreq,realBw)) {
					realFreq=m_startFreq[index];
					realBw=0.0;
				}
				realFreq+=m_LO[index];
				f1= realFreq;
				//f1+=realBw/2.0;
				//f1/=1000; //f in GHz
				//mark=RightMarkCoeff[ feeds[i] ][0]*f1*f1*f1+RightMarkCoeff[ feeds[i] ][1]*f1*f1+RightMarkCoeff[ feeds[i] ][2]*f1+RightMarkCoeff[ feeds[i] ][3];
				//printf("RIGHT realFreq %lf, bw: %lf, f1:%lf",realFreq,realBw,f1);
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
			long index=(feeds[i]*m_IFs)+ifs[i];
			double realFreq,realBw;
			if (!IRA::CIRATools::skyFrequency(freqs[i],bandwidths[i],m_startFreq[index],m_bandWidth[index],realFreq,realBw)) {
				realFreq=m_startFreq[index];
				realBw=0.0;
			}
			resFreq[i]=realFreq+m_LO[index];
			resBw[i]=realBw;
			if (m_pols[index]==Receivers::RCV_LCP) {
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
			long index=(feeds[i]*m_IFs)+ifs[i];
			double realFreq,realBw;
			if (!IRA::CIRATools::skyFrequency(freqs[i],bandwidths[i],m_startFreq[index],m_bandWidth[index],realFreq,realBw)) {
				realFreq=m_startFreq[index];
				realBw=0.0;
			}
			resFreq[i]=realFreq+m_LO[index];
			resBw[i]=realBw;
			if (m_pols[index]==Receivers::RCV_LCP) {
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
	return;
}

void CRecvBossCore::updateDewarPositionerStatus() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::OperationErrorExImpl)
{
	return;
}

void CRecvBossCore::getPolarization(ACS::longSeq& pol) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	pol.length(m_totalOutputs);
	for (long i=0;i<m_totalOutputs;i++) pol[i]=m_pols[i];
}

void CRecvBossCore::getLO(ACS::doubleSeq& lo) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	lo.length(m_totalOutputs);
	for (long i=0;i<m_totalOutputs;i++) lo[i]=m_LO[i];
}

void CRecvBossCore::getBandWidth(ACS::doubleSeq& bw)  throw  (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	bw.length(m_totalOutputs);
	for (long i=0;i<m_totalOutputs;i++) bw[i]=m_bandWidth[i];
}

void CRecvBossCore::getInitialFrequency(ACS::doubleSeq& iFreq)  throw  (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	iFreq.length(m_totalOutputs);
	for (long i=0;i<m_totalOutputs;i++) iFreq[i]=m_startFreq[i];
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

void CRecvBossCore::publishData() throw (ComponentErrors::NotificationChannelErrorExImpl)
{
	static TIMEVALUE lastEvent(0.0L);
	Receivers::ReceiversDataBlock data;
	TIMEVALUE now;
	baci::ThreadSyncGuard guard(&m_mutex);
	IRA::CIRATools::getTime(now);
	if (CIRATools::timeDifference(lastEvent,now)>=1000000) {  //one second from last event
		data.tracking=(m_currentReceiver!="");
		data.timeMark=now.value().value;
		data.status=m_status;
		try {
			m_notificationChannel->publishData<Receivers::ReceiversDataBlock>(data);
		}
		catch (acsncErrType::PublishEventFailureExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::NotificationChannelErrorExImpl,impl,ex,"CRecvBossCore::publishData()");
			m_status=Management::MNG_WARNING;
			throw impl;
		}
		IRA::CIRATools::timeCopy(lastEvent,now);
	}
}


