//#define RB_DEBUG

#define RECV_ADDRESS "192.167.189.2"
#define RECV_PORT 2096
#define FS_ADDRESS "192.167.189.62"
#define FS_PORT 5002
#define SXKL_ADDRESS "192.167.189.2"
#define SXKL_PORT 9876


#include "sxklLine.i"

// speed of light in meters per second
#define LIGHTSPEED 299792458.0

void CRecvBossCore::initialize(maci::ContainerServices* services,CConfiguration *config,acscomponent::ACSComponentImpl *me)
	throw (ComponentErrors::UnexpectedExImpl)
{
	m_currentRecvCode="";
	m_currentRecvInstance="";
	m_currentRecvError=false;
	m_currentRecv=Receivers::Receiver::_nil();
	m_currentOperativeMode="";
	//m_recvOpened=false;
	//m_fsOpened=false;
	m_status=Management::MNG_WARNING;
	m_services=services;
	m_feeds=0;
	m_IFs=0;
	m_lastStatusChange=0;
	m_bossStatus=Management::MNG_OK;
	for (unsigned i=0;i<_RECVBOSSCORE_MAX_IFS;i++) {
		m_startFreq[i]=0.0;
		m_bandWidth[0]=0.0;
		m_LO[i]=0.0;
	}
	m_config=config;
	m_totalOutputs=0;
	m_notificationChannel=NULL;
	m_loEpoch=m_starFreqEpoch=m_bandWidthEpoch=m_polEpoch=m_feedsEpoch=m_IFsEpoch=m_modeEpoch=0;

	try {
		m_notificationChannel=new nc::SimpleSupplier(Receivers::RECEIVERS_DATA_CHANNEL,me);
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CRecvBossCore::initialize()");
	}
	ACS_LOG(LM_FULL_INFO,"CRecvBossCore::initialize()",(LM_INFO,"ReceiversBoss notification channel open"));
}

void CRecvBossCore::execute() throw (ComponentErrors::IRALibraryResourceExImpl,ComponentErrors::CDBAccessExImpl,ComponentErrors::SocketErrorExImpl)
{
}

void CRecvBossCore::cleanUp()
{
	//if (m_KKCFeedTable) delete m_KKCFeedTable;
	if (m_notificationChannel!=NULL) {
		m_notificationChannel->disconnect();
		m_notificationChannel=NULL;
	}
	unloadReceiver();
}

void CRecvBossCore::calOn() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	//IRA::CError err;
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentRecvCode=="KKC") {
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
	else if ((m_currentRecvCode=="CCC") || (m_currentRecvCode=="CHC")){
		char buffer [14] = {'s','e','t',' ','m','a','r','c','a','c',' ','o','n','\n' };
		if (!sendToRecvControl((const void *)buffer,14)) {
			_EXCPT(ComponentErrors::SocketErrorExImpl,impl,"CRecvBossCore::calOn()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
	}
	else if (m_currentRecvCode=="XXP") {
		// turn the marca on through the FS
		IRA::CString fsBuffer("sxkl=*,on\n");
		if (!sendToFS((const void *)fsBuffer,fsBuffer.GetLength())) {
			_EXCPT(ComponentErrors::SocketErrorExImpl,impl,"CRecvBossCore::calOn()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
	}
	else {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::calOn()");
		impl.setReason("Receiver not configured yet");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
	m_cal=true;
	ACS_LOG(LM_FULL_INFO,"CRecvBossCore::calOn()",(LM_NOTICE,"Noise mark is turned on"));
}

void CRecvBossCore::calOff() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	//IRA::CError err;
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentRecvCode=="KKC") {
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
			_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::calOff()");
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
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::calOff()");
			changeBossStatus(Management::MNG_FAILURE);
			m_currentRecvError=true;
			throw impl;
		}
	}
	else if ((m_currentRecvCode=="CCC") || (m_currentRecvCode=="CHC")) {
		char buffer [15] = {'s','e','t',' ','m','a','r','c','a','c',' ','o','f','f','\n' };
		if (!sendToRecvControl((const void *)buffer,15)) {
			_EXCPT(ComponentErrors::SocketErrorExImpl,impl,"CRecvBossCore::calOff()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
	}
	else if (m_currentRecvCode=="XXP") {
		// turn the marca on through thr FS
		IRA::CString fsBuffer("sxkl=*,off\n");
		if (!sendToFS((const void *)fsBuffer,fsBuffer.GetLength())) {
			_EXCPT(ComponentErrors::SocketErrorExImpl,impl,"CRecvBossCore::calOff()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}				
	}
	else {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::calOff()");
		impl.setReason("Receiver not configured yet");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
	m_cal=false;
	ACS_LOG(LM_FULL_INFO,"CRecvBossCore::calOff()",(LM_NOTICE,"Noise mark is turned off"));
}

void CRecvBossCore::setLO(const ACS::doubleSeq& lo) throw (ComponentErrors::ValidationErrorExImpl,
	ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,
	ReceiversErrors::UnavailableReceiverOperationExImpl,
	ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	WORD len;
	//double loAmp[4]={-55.24524,11.41288,-0.79437,0.01894};
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
		CUSTOM_LOG(LM_FULL_INFO,"CRecvBossCore::setLO()",(LM_NOTICE,"Keeping local oscillator value: %lf",m_LO[0]));
		return;
	}
	//***********************************************************************/
	// range checks must be performed, Also the resulting bandwidth, according the lo value and the RF ranges must be recomputed
	//**********************************************************************/
	if (m_currentRecvCode=="KKC") {
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
		return;	
	}
	else if (m_currentRecvCode=="CCC") {
		//no difference between IFs so take just the first value
		trueValue=lo[0]+2300.0;
		m_LO[0]=m_LO[1]=lo[0];
		CUSTOM_LOG(LM_FULL_INFO,"CRecvBossCore::setLO()",(LM_NOTICE,"New local oscillator value: %lf",m_LO[0]));
	}
	else if (m_currentRecvCode=="CHC") {
		//no difference between IFs so take just the first value
		trueValue=lo[0]+2300.0;
		m_LO[0]=m_LO[1]=lo[0];
		CUSTOM_LOG(LM_FULL_INFO,"CRecvBossCore::setLO()",(LM_NOTICE,"New local oscillator value: %lf",m_LO[0]));
	}
	else if (m_currentRecvCode=="XXP") {
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
	// at the moment the settable LO is unique...so no difference from receiver to receiver
	msg.Format("set lofreq %lf\n",(double)trueValue);
	len=msg.GetLength();
	if (!sendToRecvControl((const void *)msg,len)) {
		_EXCPT(ComponentErrors::SocketErrorExImpl,impl,"CRecvBossCore::setLO()");
		m_status=Management::MNG_FAILURE;
		throw impl;
	}	
	IRA::CIRATools::Wait(0,500000);  //wait half a second to settle things down
}

void CRecvBossCore::setup(const char * code) throw (ComponentErrors::SocketErrorExImpl,ComponentErrors::ValidationErrorExImpl)
{
	IRA::CError err;
	IRA::CString rec(code);
	IRA::CString recvIpAddr(RECV_ADDRESS);
	IRA::CString fsIpAddr(FS_ADDRESS);
	baci::ThreadSyncGuard guard(&m_mutex);
	if (rec=="KKC") {
		IRA::CString component;
		bool derotator;
		if (!m_config->getReceiver(code,component,derotator)) {
			_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::setup()");
			impl.setReason("Receiver code is not known");
			throw impl;
		}
		//deactivate current receiver.....
		if (!CORBA::is_nil(m_currentRecv)) {
			try {
				m_currentRecv->deactivate();
			}
			catch (...) {
				ACS_LOG(LM_FULL_INFO,"CRecvBossCore::setup()",(LM_WARNING,"Current Receiver could not be deactivated"));
			}
		}
		unloadReceiver();
		m_currentRecvCode=code;
		m_currentRecvInstance=component;
		loadReceiver(); // throw ComponentErrors::CouldntGetComponentExImpl
		try {
			m_currentRecv->activate(code);
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

		m_currentRecvCode="KKC";
		m_currentOperativeMode="NORMAL";
	}
	else if (rec=="CCC") {
		char buffer [9] = {'p','r','o','c',' ','c','c','c','\n' };
		if (!sendToRecvControl((const void *)buffer,9)) {
			_EXCPT(ComponentErrors::SocketErrorExImpl,impl,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}
		// now set the subreflector configuration through the FS
		IRA::CString fsBuffer("scu=ccc\n");
		if (!sendToFS((const void *)fsBuffer,fsBuffer.GetLength())) {
			_EXCPT(ComponentErrors::SocketErrorExImpl,impl,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}	
		m_LO[0]=4600.0;
		m_LO[1]=4600.0;
		m_IFs=2;
		m_feeds=1;
		m_totalOutputs=m_IFs*m_feeds;
		m_pols[0]=Receivers::RCV_LCP;
		m_pols[1]=Receivers::RCV_RCP;
		m_startFreq[0]=100.0;
		m_startFreq[1]=100.0;
		m_bandWidth[0]=400.0;
		m_bandWidth[1]=400.0;
		m_currentRecvCode="CCC";
		m_currentOperativeMode="NORMAL";
	}
	else if (rec=="CHC") {
		char buffer [9] = {'p','r','o','c',' ','c','h','c','\n' };
		if (!sendToRecvControl((const void *)buffer,9)) {
			_EXCPT(ComponentErrors::SocketErrorExImpl,impl,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}		
		// now set the subreflector configuration through the FS
		IRA::CString fsBuffer("scu=chc\n");
		if (!sendToFS((const void *)fsBuffer,fsBuffer.GetLength())) {
			_EXCPT(ComponentErrors::SocketErrorExImpl,impl,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}		
		m_LO[0]=6400.0;
		m_LO[1]=6400.0;
		m_IFs=2;
		m_feeds=1;
		m_totalOutputs=m_IFs*m_feeds;
		m_pols[0]=Receivers::RCV_LCP;
		m_pols[1]=Receivers::RCV_RCP;
		m_startFreq[0]=100.0;
		m_startFreq[1]=100.0;
		m_bandWidth[0]=400.0;
		m_bandWidth[1]=400.0;
		m_currentRecvCode="CHC";
		m_currentOperativeMode="NORMAL";
	}
	else if (rec=="XXP") {
		char buffer [9] = {'p','r','o','c',' ','x','x','p','\n' };
		if (!sendToRecvControl((const void *)buffer,9)) {
			_EXCPT(ComponentErrors::SocketErrorExImpl,impl,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}		
		// now set the subreflector configuration through the FS
		IRA::CString fsBuffer("scu=xxp\n");
		if (!sendToFS((const void *)fsBuffer,fsBuffer.GetLength())) {
			_EXCPT(ComponentErrors::SocketErrorExImpl,impl,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}			
		 //fsBuffer=IRA::CString("sxkl=send%0068033f\n");
		fsBuffer=IRA::CString("xxpmode\n");
		// now set the subreflector configuration through the FS
		if (!sendToFS((const void *)fsBuffer,fsBuffer.GetLength())) {
			_EXCPT(ComponentErrors::SocketErrorExImpl,impl,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			throw impl;
		}					
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
		m_currentRecvCode="XXP";
		m_currentOperativeMode="NORMAL";
	}
	else {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::setup()");
		impl.setReason("Receiver code is not known");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
	m_status=Management::MNG_OK;
	m_cal=false;
	CUSTOM_LOG(LM_FULL_INFO,"CRecvBossCore::setup()",(LM_NOTICE,"New receiver configured %s",(const char *)m_currentRecvCode));
}

void CRecvBossCore::setMode(const char * mode) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::ModeErrorExImpl,
		ComponentErrors::UnexpectedExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	//*********************************************
	//The KKC receiver does not support for modes, in principle I should have added the code
	// to call the KKC component setMode method.
	//***********************************************
	IRA::CString newMode(mode);
	if (m_currentRecvCode=="CCC") {
		if (newMode=="NORMAL") {
			m_bandWidth[0]=400.0;
			m_bandWidth[1]=400.0;
		}
		else if (newMode=="NARROWBANDWIDTH") {
			m_bandWidth[0]=150.0;
			m_bandWidth[1]=150.0;
		}
		else {
			_EXCPT(ReceiversErrors::ModeErrorExImpl,impl,"CRecvBossCore::setMode()");
			throw impl;
		}
		m_currentOperativeMode=newMode;
		CUSTOM_LOG(LM_FULL_INFO,"CRecvBossCore::setMode()",(LM_NOTICE,"New receiver mode %s",(const char *)m_currentOperativeMode));
	}
	else if (m_currentRecvCode=="CHC") {
		if (newMode=="NORMAL") {
			m_bandWidth[0]=400.0;
			m_bandWidth[1]=400.0;
		}
		else if (newMode=="NARROWBANDWIDTH") {
			m_bandWidth[0]=150.0;
			m_bandWidth[1]=150.0;
		}
		else {
			_EXCPT(ReceiversErrors::ModeErrorExImpl,impl,"CRecvBossCore::setMode()");
			throw impl;
		}
		m_currentOperativeMode=newMode;
		CUSTOM_LOG(LM_FULL_INFO,"CRecvBossCore::setMode()",(LM_NOTICE,"New receiver mode %s",(const char *)m_currentOperativeMode));
	}
	else {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::setMode()");
		impl.setReason("current receiver does not support operative modes");
		_ADD_USER_MESSAGE(impl,"The configured receiver does not support operative modes");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
}

void CRecvBossCore::park() throw (ManagementErrors::ParkingErrorExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	m_currentRecvCode="";
	m_currentOperativeMode="";
	m_IFs=0;
	m_feeds=0;
	m_totalOutputs=0;
	if (!CORBA::is_nil(m_currentRecv)) {
		try {
			m_currentRecv->deactivate();
		}
		catch (...) {
			ACS_LOG(LM_FULL_INFO,"CRecvBossCore::park()",(LM_WARNING,"Current receiver could not be deactivated"));
		}
	}	
	unloadReceiver();
}

double CRecvBossCore::getTaper(const double& freq,const double& bw,const long& feed,const long& ifNumber,double& waveLen) throw (ComponentErrors::ValidationErrorExImpl,
		ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CORBAProblemExImpl,
		ReceiversErrors::UnavailableReceiverOperationExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::CouldntGetComponentExImpl)
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
	
	if (m_currentRecvCode=="KKC") {
		loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
		try {
			CORBA::Double fq,ww,wl,taper;
			CORBA::Long fe,ifn;
			fq=(CORBA::Double)freq; ww=(CORBA::Double)bw; fe=(CORBA::Long)feed; ifn=(CORBA::Long)ifNumber;
			taper=m_currentRecv->getTaper(fq,ww,fe,ifn,wl);
			waveLen=(double)wl;
			return (double)taper;
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
	if ((m_currentRecvCode=="CCC") || (m_currentRecvCode=="CHC")) {
		ff=secondaryFreq;
		tt=secondaryTaper;
		max=7;
	}
	else if (m_currentRecvCode=="XXP") {
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
		ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentRecvCode=="KKC") {
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
			return (long)len;
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
	else if ((m_currentRecvCode=="CCC") || (m_currentRecvCode=="XXP") || (m_currentRecvCode=="CHC")) {
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
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentRecvCode=="KKC") {
		loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
		try {
			ACS::doubleSeq_var tempFreqs=new ACS::doubleSeq;
			ACS::doubleSeq_var tempBw=new ACS::doubleSeq;
			ACS::longSeq_var tempPols=new ACS::longSeq;
			ACS::doubleSeq_var tempLO=new ACS::doubleSeq;
			unsigned len;
			m_currentRecv->getIFOutput(feeds,ifs,tempFreqs.out(),tempBw.out(),tempPols.out(),tempLO.out());
			len=tempFreqs->length();
			freqs.length(len);
			bw.length(len);
			pols.length(len);
			LO.length(len);
			for (unsigned i=0;i<len;i++) {
				freqs[i]=tempFreqs[i];
				bw[i]=tempBw[i];
				pols[i]=tempPols[i];
				LO[i]=tempLO[i];
			}
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::getIFOutput()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			changeBossStatus(Management::MNG_FAILURE);
			m_currentRecvError=true;
			throw impl;
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::getIFOutput");
			impl.setReceiverCode((const char *)m_currentRecvCode);
			changeBossStatus(Management::MNG_FAILURE);
			throw impl;
		}
		catch (ReceiversErrors::ReceiversErrorsEx& ex) {
			_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::getIFOutput");
			impl.setReceiverCode((const char *)m_currentRecvCode);
			changeBossStatus(Management::MNG_FAILURE);
			throw impl;
		}	
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::getIFOutput()");
			changeBossStatus(Management::MNG_FAILURE);
			m_currentRecvError=true;
			throw impl;
		}
		return;
	}	
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

bool CRecvBossCore::checkScan(ACS::Time& startUT,const Receivers::TReceiversParameters& param,
		const Antenna::TRunTimeParameters& antennaInfo,const Management::TScanConfiguration& scanConf,Receivers::TRunTimeParameters& runTime) throw(
		ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl,
		ReceiversErrors::DewarPositionerCommandErrorExImpl)
{
	runTime.onTheFly=true;
	runTime.timeToStop=0;
	runTime.startEpoch=startUT;
	return true;
}

void CRecvBossCore::CRecvBossCore::startScan(ACS::Time& startUT,const Receivers::TReceiversParameters& param,
  const Antenna::TRunTimeParameters& antennaInfo,const Management::TScanConfiguration& scanConf) throw(
  ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,
  ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::DewarPositionerCommandErrorExImpl)
{

}

void CRecvBossCore::closeScan(ACS::Time& timeToStop) throw (ReceiversErrors::DewarPositionerCommandErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	timeToStop=0;
}

void CRecvBossCore::getCalibrationMark(ACS::doubleSeq& result,ACS::doubleSeq& resFreq,ACS::doubleSeq& resBw,const ACS::doubleSeq& freqs,const ACS::doubleSeq& bandwidths,const ACS::longSeq& feeds,
		const ACS::longSeq& ifs,bool& onoff,double& scale) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
		ComponentErrors::UnexpectedExImpl, ComponentErrors::CouldntGetComponentExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentRecvCode=="KKC") {
		loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
		try {
			ACS::doubleSeq_var tempRes=new ACS::doubleSeq;
			ACS::doubleSeq_var tempFreq=new ACS::doubleSeq;
			ACS::doubleSeq_var tempBw=new ACS::doubleSeq;
			CORBA::Boolean onoff_out;
			tempRes=m_currentRecv->getCalibrationMark(freqs,bandwidths,feeds,ifs,tempFreq.out(),tempBw.out(),onoff_out,scale);
			result.length(tempRes->length());
			resFreq.length(tempRes->length());
			resBw.length(tempRes->length());
			onoff=onoff_out;
			for (unsigned i=0;i<result.length();i++) {
				result[i]=tempRes[i];
				resFreq[i]=tempFreq[i];
				resBw[i]=tempBw[i];
			}
			return;
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
	onoff=m_cal;

	if (m_currentRecvCode=="CHC") {
		double LeftM[2] = { -0.0023, 31.593  };
		double RightM[2] = { -0.0023, 32.764 };
		double Freq;
		double mark=0;
		double realFreq,realBw;
		double f1,f2;
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
				f2=f1+realBw;
				Freq=(f1+f2)/2.0;
				mark=(LeftM[0]*Freq)+LeftM[1];
			}
			else if (m_pols[index]==Receivers::RCV_RCP) {
				// take the real observed bandwidth....the correlation between detector device and the band provided by the receiver
				if (!IRA::CIRATools::skyFrequency(freqs[i],bandwidths[i],m_startFreq[index],m_bandWidth[index],realFreq,realBw)) {
					realFreq=m_startFreq[index];
					realBw=0.0;
				}
				realFreq+=m_LO[index];
				f1=realFreq;
				f2=f1+realBw;
				Freq=(f1+f2)/2.0;
				mark=(RightM[0]*Freq)+RightM[1];
			}
			result[i]=mark;
			resFreq[i]=realFreq;
			resBw[i]=realBw;
			scale=1.0;
		}
	}
	else if (m_currentRecvCode=="CCC") {
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
	else if (m_currentRecvCode=="XXP") {
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

void CRecvBossCore::updateRecvStatus() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	return;
}

void CRecvBossCore::updateDewarPositionerStatus() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::OperationErrorExImpl,ReceiversErrors::DewarPositionerCommandErrorExImpl)
{
	return;
}

void CRecvBossCore::getPolarization(ACS::longSeq& pol) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{	
	baci::ThreadSyncGuard guard(&m_mutex);
	TIMEVALUE now;
	pol.length(0);
	if (m_currentRecvCode=="") {
		return;
	}
	if (m_currentRecvCode=="KKC") {
		IRA::CIRATools::getTime(now);
		if (now.value().value>m_polEpoch+m_config->propertyUpdateTime()*10) {
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
	else {
		pol.length(m_totalOutputs);
		for (long i=0;i<m_totalOutputs;i++) pol[i]=m_pols[i];
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
	if (m_currentRecvCode=="KKC") {
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
	else {
		lo.length(m_totalOutputs);
		for (long i=0;i<m_totalOutputs;i++) lo[i]=m_LO[i];
	}
}

void CRecvBossCore::getBandWidth(ACS::doubleSeq& bw)  throw  (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	TIMEVALUE now;
	bw.length(0);
	if (m_currentRecvCode=="") {
		return;
	}
	if (m_currentRecvCode=="KKC") {
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
				m_bandWidth_.length(val->length());
				for (unsigned i=0;i<bw.length();i++) {
					bw[i]=val[i];
					m_bandWidth_[i]=val[i];
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
			bw.length(m_bandWidth_.length());
			for (unsigned i=0;i<m_bandWidth_.length();i++) bw[i]=m_bandWidth_[i];
		}
	}	
	else {
		bw.length(m_totalOutputs);
		for (long i=0;i<m_totalOutputs;i++) bw[i]=m_bandWidth[i];
	}
}

void CRecvBossCore::getInitialFrequency(ACS::doubleSeq& iFreq)  throw  (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	TIMEVALUE now;
	iFreq.length(0);
	if (m_currentRecvCode=="") {
		return;
	}
	if (m_currentRecvCode=="KKC") {
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
				m_startFreq_.length(val->length());
				for (unsigned i=0;i<iFreq.length();i++) {
					iFreq[i]=val[i];
					m_startFreq_[i]=val[i];
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
			iFreq.length(m_startFreq_.length());
			for (unsigned i=0;i<m_startFreq_.length();i++) iFreq[i]=m_startFreq_[i];
		}	
	}
	else {	
		iFreq.length(m_totalOutputs);
		for (long i=0;i<m_totalOutputs;i++) iFreq[i]=m_startFreq[i];
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
	if (m_currentRecvCode=="KKC") {
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
	if (m_currentRecvCode=="KKC") {
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
	else {
		ifs=m_IFs;
	}
}

const IRA::CString& CRecvBossCore::getRecvCode()
{
	return m_currentRecvCode;
}

const IRA::CString& CRecvBossCore::getOperativeMode() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	// to be adjust for the KKCband component
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
		data.tracking=(m_currentRecvCode!="");
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

bool CRecvBossCore::sendToRecvControl(const void *buffer,unsigned size)
{
	IRA::CSocket sock;
	IRA::CError err;
	char readout[128];
	IRA::CString recvIpAddr(RECV_ADDRESS);
	DWORD recvPort=RECV_PORT;
	try {
		if (sock.Create(err,IRA::CSocket::STREAM)!=IRA::CSocket::SUCCESS) {
			return false;
		}
		if (sock.setSockMode(err,IRA::CSocket::NONBLOCKING)==IRA::CSocket::FAIL) {
			return false;
		}
		if (sock.Connect(err,recvIpAddr,recvPort)==IRA::CSocket::FAIL) {
			return false;
		}
		if (sock.Send(err,buffer,size)!=(int)size) {
			return false;
		}
		if (sock.Receive(err,(void *)readout,128)==IRA::CSocket::FAIL) {
			return false;
		}
		if (strcmp(readout,"ACK\n")!=0) {
			return false;
		} 		
		sock.Close(err);
	}
	catch (...) {
		return false;		
	}
	return true;
}

bool CRecvBossCore::sendToFS(const void *buffer,unsigned size)
{
	IRA::CSocket sock;
	IRA::CError err;
	IRA::CString fsAddr(FS_ADDRESS);
	DWORD fsPort=FS_PORT;
	try {
		if (sock.Create(err,IRA::CSocket::STREAM)!=IRA::CSocket::SUCCESS) {
			return false;
		}
		if (sock.Connect(err,fsAddr,fsPort)==IRA::CSocket::FAIL) {
			return false;
		}
		if (sock.Send(err,buffer,size)!=(int)size) {
			return false;
		}	
		sock.Close(err);
	}
	catch (...) {
		return false;		
	}
	return true;
}


