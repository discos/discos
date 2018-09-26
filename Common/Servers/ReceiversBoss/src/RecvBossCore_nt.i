//**************************************************************************************************************
//*   THIS FILE IS DEPRECATED...NOTO is now compiled under standard target
//**************************************************************************************************************

#define FS_ADDRESS "192.167.187.17"
#define FS_PORT 5002
#define FSCAL_ADDRESS "192.167.187.12"
#define FSCAL_PORT 5002

// speed of light in meters per second
#define LIGHTSPEED 299792458.0

#define BANDAK "bandaK"
#define BANDAQ "bandaQ"
#define BANDAC "bandaC"

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
	//m_fsOpened=false;
	//m_fsCalOpened=false;
	m_fsSocketError=m_fsCalSocketError=false;
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
        m_totalPower_proxy.setComponentName("Backends/TotalPower");
        //m_totalPower_proxy.setContainerServices(m_services);
}

void CRecvBossCore::execute() throw (ComponentErrors::IRALibraryResourceExImpl,ComponentErrors::CDBAccessExImpl)
{
	m_fsCalSocketError=true;
	m_fsSocketError=true;
	reinitCal();
	reinit(); // throw (ComponentErrors::IRALibraryResourceExImpl)
}

void CRecvBossCore::cleanUp()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_notificationChannel!=NULL) {
		m_notificationChannel->disconnect();
		m_notificationChannel=NULL;
	}
	IRA::CError err;
	err.Reset();
	m_fsSocket.Close(err);
	err.Reset();
	m_fsSocketCal.Close(err);
}

void CRecvBossCore::externalCalOff() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
				ComponentErrors::UnexpectedExImpl)
{
}

void CRecvBossCore::externalCalOn() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
				ComponentErrors::UnexpectedExImpl)
{
}


void CRecvBossCore::calOn() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
        //TODO: add error management
        //try{
            m_totalPower_proxy->calOn();
        //}catch(...)
   m_cal=true;
	CUSTOM_LOG(LM_FULL_INFO,"CRecvBossCore::calOn()",(LM_NOTICE,"NOISE_CAL_TURNED_ON"));
}

void CRecvBossCore::calOff() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
        //TODO: add error management
        //try{
            m_totalPower_proxy->calOff();
        //}catch(...)
   m_cal=false;
	CUSTOM_LOG(LM_FULL_INFO,"CRecvBossCore::calOn()",(LM_NOTICE,"NOISE_CAL_TURNED_OFF"));
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
	baci::ThreadSyncGuard guard(&m_mutex);
	ACS_LOG(LM_FULL_INFO,"CRecvBossCore::setLO()",(LM_NOTICE,"LOCAL_OSCILLATOR_NOT_SUPPORTED_YET"));
	
	/*if (lo.length()==0) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::setLO()");
		impl.setReason("at least one value must be provided");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
	if (lo[0]==-1) {
		ACS_LOG(LM_FULL_INFO,"CRecvBossCore::setLO()",(LM_NOTICE,"KEEP_CURRENT_LOCAL_OSCILLATOR %lf",m_LO[0]));
		return;
	}*/
}

void CRecvBossCore::setup(const char * code) throw (ComponentErrors::SocketErrorExImpl,ComponentErrors::ValidationErrorExImpl)
{
	IRA::CError err;
	IRA::CString rec(code);
	/*IRA::CString fsIpAddr(FS_ADDRESS);
	DWORD fsPort=FS_PORT;*/
	baci::ThreadSyncGuard guard(&m_mutex);
	if ((rec!=BANDAK) && (rec!=BANDAQ) && (rec!=BANDAC)) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::setup()");
		impl.setReason("Receiver code is not known");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
	if (m_fsSocketError==true) reinit();
	/*if (m_fsSocket.Connect(err,fsIpAddr,fsPort)==IRA::CSocket::FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
		dummy.setCode(err.getErrorCode());
		dummy.setDescription( (const char*)err.getDescription());
		err.Reset();
		_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setup()");
		m_status=Management::MNG_FAILURE;
		m_fsSocketError=true;
		throw impl;
	}*/
	IRA::CIRATools::Wait(0,500000);  //wait half a second to settle things down
	//m_fsOpened=true;
	if (rec==BANDAK) {
		char buffer [10] = {'e','s','c','s','k','b','a','n','d','\n' };
		if (m_fsSocket.Send(err,(const void *)buffer,10)!=10) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			m_fsSocketError=true;
			throw impl;
		}
		m_IFs=2;
		m_feeds=1;
		m_totalOutputs=m_IFs*m_feeds;
		for (long i=0;i<m_totalOutputs;i++) {
			m_LO[i]=21869.0;
			m_startFreq[i]=100.0;
			m_bandWidth[i]=500.0;
			if ((i % 2)==0) m_pols[i]=Receivers::RCV_LCP;
			else m_pols[i]=Receivers::RCV_RCP;
		}
		m_currentReceiver=BANDAK;
		m_currentOperativeMode="NORMAL";
	}
	else if (rec==BANDAQ) {
		char buffer [10] = {'e','s','c','s','q','b','a','n','d','\n' };
		if (m_fsSocket.Send(err,(const void *)buffer,10)!=10) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			m_fsSocketError=true;
			throw impl;
		}
		m_IFs=2;
		m_feeds=1;
		m_totalOutputs=m_IFs*m_feeds;
		for (long i=0;i<m_totalOutputs;i++) {
			m_LO[i]=42850.0;
			m_startFreq[i]=100.0;
			m_bandWidth[i]=500.0;
			if ((i % 2)==0) m_pols[i]=Receivers::RCV_LCP;
			else m_pols[i]=Receivers::RCV_RCP;
		}
		m_currentReceiver=BANDAQ;
		m_currentOperativeMode="NORMAL";
	}
	else if (rec==BANDAC) {
		char buffer [10] = {'e','s','c','s','c','b','a','n','d','\n' };
		if (m_fsSocket.Send(err,(const void *)buffer,10)!=10) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
			dummy.setCode(err.getErrorCode());
			dummy.setDescription((const char*)err.getDescription());
			err.Reset();
			_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::setup()");
			m_status=Management::MNG_FAILURE;
			m_fsSocketError=true;
			throw impl;
		}
		m_IFs=2;
		m_feeds=1;
		m_totalOutputs=m_IFs*m_feeds;
		for (long i=0;i<m_totalOutputs;i++) {
			m_LO[i]=4600.0;
			m_startFreq[i]=100.0;
			m_bandWidth[i]=500.0;
			if ((i % 2)==0) m_pols[i]=Receivers::RCV_LCP;
			else m_pols[i]=Receivers::RCV_RCP;
		}
		m_currentReceiver=BANDAC;
		m_currentOperativeMode="NORMAL";
	}
	/*m_fsSocket.Close(err);
	m_fsOpened=false;*/
	m_status=Management::MNG_OK;
	m_cal=false;
	ACS_LOG(LM_FULL_INFO,"CRecvBossCore::setup()",(LM_NOTICE,"NEW_RECEIVER_CONFIGURED %s",(const char *)m_currentReceiver));
}

void CRecvBossCore::setMode(const char * mode) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::ModeErrorExImpl,
		ComponentErrors::UnexpectedExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,ComponentErrors::CORBAProblemExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	ACS_LOG(LM_FULL_INFO,"CRecvBossCore::setMode()",(LM_NOTICE,"MODE_NOT_SUPPORTED_YET"));
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
	if (m_currentReceiver==BANDAK) {  // this values are pure guesses!!!!
		taper=-12.6;
	}
	else if (m_currentReceiver==BANDAQ) {
		taper=-26.0;
	}
	else if (m_currentReceiver==BANDAC) {
		taper=-12.0;
	}
	else {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::getTaper()");
		impl.setReason("Receiver code is not known");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
	waveLen=LIGHTSPEED/(centralFreq*1000000000);
	return taper;
}

/*double CRecvBossCore::getDerotatorPosition (const ACS::Time& epoch) throw (ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,
		ComponentErrors::ValidationErrorExImpl)
double CRecvBossCore::getDerotatorPosition (const ACS::Time& epoch) throw (ComponentErrors::CouldntGetComponentExImpl,
		ReceiversErrors::DewarPositionerCommandErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	// no need to check anything
	return 0;
}*/

/*
void CRecvBossCore::derotatorSetup (const Receivers::TUpdateModes& mode,const Receivers::TRewindModes& rewind,const long& feeds) throw (
		ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,ComponentErrors::ValidationErrorExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
   		ReceiversErrors::DewarPositionerSetupErrorExImpl)
{
	// baci::ThreadSyncGuard guard(&m_mutex);
	// no support in ESCs for that
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorSetup()");
	m_status=Management::MNG_WARNING;
	throw impl;
}*/

/*
void CRecvBossCore::derotatorPark() throw (ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,
    			ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::DewarPositionerParkingErrorExImpl,
    			ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
{
	// baci::ThreadSyncGuard guard(&m_mutex);
	// no support in ESCs for that
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorPark()");
	m_status=Management::MNG_WARNING;
	throw impl;
}*/

long CRecvBossCore::getFeeds(ACS::doubleSeq& X,ACS::doubleSeq& Y,ACS::doubleSeq& power) throw (ComponentErrors::ValidationErrorExImpl,
		ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if ((m_currentReceiver!="")) { // if a receiver has been configured!
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
		const ACS::longSeq& ifs,bool& onoff,double& scale) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	double LeftMarkCoeff[1][4];
	double RightMarkCoeff[1][4];
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
	if (m_currentReceiver==BANDAK) {
		LeftMarkCoeff[0][0]=0.000001; LeftMarkCoeff[0][1]=0.823680; LeftMarkCoeff[0][2]=1825.749665; LeftMarkCoeff[0][3]=-13489491.21593;
		RightMarkCoeff[0][0]=0.000001; RightMarkCoeff[0][1]=-0.066772; RightMarkCoeff[0][2]=1472.736500; RightMarkCoeff[0][3]=-10827302.569501;
	}
	else if (m_currentReceiver==BANDAQ) {
		LeftMarkCoeff[0][0]=0.0;LeftMarkCoeff[0][1]=0.0000538;LeftMarkCoeff[0][2]=2.41322118;LeftMarkCoeff[0][3]=35949.0897503;
		RightMarkCoeff[0][0]=0.0;RightMarkCoeff[0][1]=0.0;RightMarkCoeff[0][2]=0.0;RightMarkCoeff[0][3]=16.5;
	}
	else if (m_currentReceiver==BANDAC) {	
		LeftMarkCoeff[0][0]=0.00000;LeftMarkCoeff[0][1]=-0.00116;LeftMarkCoeff[0][2]=5.81764;LeftMarkCoeff[0][3]=-9714.90615;
		RightMarkCoeff[0][0]=0.00000;RightMarkCoeff[0][1]=-0.00631;RightMarkCoeff[0][2]=30.81321;RightMarkCoeff[0][3]=-50717.24085;
	}
	else {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::getCalibrationMark()");
		impl.setReason("Receiver not configured yet");
		m_status=Management::MNG_WARNING;
		throw impl;
	}
	scale=1.0;
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
			f2=f1+realBw;
			f1/=1000.0; f2/=1000.0; //frequencies in giga Hertz
			integral=(LeftMarkCoeff[feeds[i]][0]/4)*(f2*f2*f2*f2-f1*f1*f1*f1)+(LeftMarkCoeff[feeds[i]][1]/3)*(f2*f2*f2-f1*f1*f1)+(LeftMarkCoeff[feeds[i]][2]/2)*(f2*f2-f1*f1)+LeftMarkCoeff[feeds[i]][3]*(f2-f1);
			mark=integral/(f2-f1);
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
			f1/=1000.0; f2/=1000.0; //frequencies in giga Hertz
			integral=(RightMarkCoeff[feeds[i]][0]/4)*(f2*f2*f2*f2-f1*f1*f1*f1)+(RightMarkCoeff[feeds[i]][1]/3)*(f2*f2*f2-f1*f1*f1)+(RightMarkCoeff[feeds[i]][2]/2)*(f2*f2-f1*f1)+RightMarkCoeff[feeds[i]][3]*(f2-f1);
			mark=integral/(f2-f1);
		}
		result[i]=mark;
		resFreq[i]=realFreq;
		resBw[i]=realBw;
	}
}

void  CRecvBossCore::updateRecvStatus() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl)
{
	return;
}

/*
void CRecvBossCore::updateDewarPositionerStatus() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::OperationErrorExImpl)*/
void CRecvBossCore::updateDewarPositionerStatus() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::OperationErrorExImpl,ReceiversErrors::DewarPositionerCommandErrorExImpl)
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

void CRecvBossCore::reinitCal() throw (ComponentErrors::IRALibraryResourceExImpl)
{
	IRA::CError err;
	IRA::CString fsIpAddr(FSCAL_ADDRESS);
	DWORD fsPort=FSCAL_PORT;
	err.Reset();
	m_fsSocketCal.Close(err);
	err.Reset();
	//m_fsCalOpened=false;
	if (m_fsSocketCal.Create(err,IRA::CSocket::STREAM)!=IRA::CSocket::SUCCESS) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
		dummy.setCode(err.getErrorCode());
		dummy.setDescription((const char*)err.getDescription());
		err.Reset();
		m_status=Management::MNG_FAILURE;
		throw dummy;
	}
	if (m_fsSocketCal.Connect(err,fsIpAddr,fsPort)==IRA::CSocket::FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
		dummy.setCode(err.getErrorCode());
		dummy.setDescription( (const char*)err.getDescription());
		err.Reset();
		_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::reinitCal()");
		m_status=Management::MNG_FAILURE;
		m_fsCalSocketError=true;
		throw impl;
	}
	m_fsCalSocketError=false;	
}

void CRecvBossCore::reinit() throw (ComponentErrors::IRALibraryResourceExImpl)
{
	IRA::CError err;
	IRA::CString fsIpAddr(FS_ADDRESS);
	DWORD fsPort=FS_PORT;
	err.Reset();
	m_fsSocket.Close(err);
	err.Reset();
	if (m_fsSocket.Create(err,IRA::CSocket::STREAM)!=IRA::CSocket::SUCCESS) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
		dummy.setCode(err.getErrorCode());
		dummy.setDescription((const char*)err.getDescription());
		err.Reset();
		m_status=Management::MNG_FAILURE;
		throw dummy;
	}
	if (m_fsSocket.Connect(err,fsIpAddr,fsPort)==IRA::CSocket::FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
		dummy.setCode(err.getErrorCode());
		dummy.setDescription( (const char*)err.getDescription());
		err.Reset();
		_ADD_BACKTRACE(ComponentErrors::SocketErrorExImpl,impl,dummy,"CRecvBossCore::reinit()");
		m_status=Management::MNG_FAILURE;
		m_fsSocketError=true;
		throw impl;
	}
	m_fsSocketError=false;
}

void CRecvBossCore::closeScan(ACS::Time& timeToStop) throw (ReceiversErrors::DewarPositionerCommandErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	timeToStop=0;
}

void CRecvBossCore::getDewarParameter(Receivers::TDerotatorConfigurations& mod,double& pos) throw (
  ReceiversErrors::DewarPositionerCommandErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
{
	mod=Receivers::RCV_UNDEF_DEROTCONF;
	pos=0.0;
}

void CRecvBossCore::derotatorSetConfiguration(const Receivers::TDerotatorConfigurations& conf) throw (
	ComponentErrors::ValidationErrorExImpl,ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,
	ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::DewarPositionerSetupErrorExImpl,ComponentErrors::CORBAProblemExImpl,
	ComponentErrors::UnexpectedExImpl)
{
	// no support in ESCS for that
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorSetConfiguration()");
	m_status=Management::MNG_WARNING;
	throw impl;
}

void CRecvBossCore::derotatorSetRewindingMode(const Receivers::TRewindModes& rewind) throw (
		ComponentErrors::ValidationErrorExImpl,ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::DewarPositionerSetupErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ReceiversErrors::DewarPositionerNotConfiguredExImpl)
{
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorSetRewindingMode()");
	m_status=Management::MNG_WARNING;
	throw impl;
}

void CRecvBossCore::derotatorSetAutoRewindingSteps(const long& steps) throw (
		ComponentErrors::ValidationErrorExImpl,ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::DewarPositionerSetupErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ReceiversErrors::DewarPositionerNotConfiguredExImpl)
{
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorSetAutoRewindingSteps()");
	m_status=Management::MNG_WARNING;
	throw impl;
}

void CRecvBossCore::setDerotatorPosition(const double& pos) throw (ReceiversErrors::NoDewarPositioningExImpl,
  ReceiversErrors::NoDerotatorAvailableExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,
  ReceiversErrors::DewarPositionerCommandErrorExImpl,ComponentErrors::CORBAProblemExImpl,
  ComponentErrors::UnexpectedExImpl,ReceiversErrors::DewarPositionerNotConfiguredExImpl)
{
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::setDerotatorPosition()");
	m_status=Management::MNG_WARNING;
	throw impl;
}

void CRecvBossCore::derotatorRewind(const long& steps) throw (ComponentErrors::ValidationErrorExImpl,
  ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,
  ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::DewarPositionerCommandErrorExImpl,
  ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ReceiversErrors::DewarPositionerNotConfiguredExImpl)
{
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorRewind()");
	m_status=Management::MNG_WARNING;
	throw impl;
}

double CRecvBossCore::getDerotatorPosition (const ACS::Time& epoch) throw (ComponentErrors::CouldntGetComponentExImpl,
		ReceiversErrors::DewarPositionerCommandErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	// no need to check anything
	return -9999.99;
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

