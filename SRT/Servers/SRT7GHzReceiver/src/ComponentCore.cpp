#include "ComponentCore.h"

CComponentCore::CComponentCore()
{

}

CComponentCore::~CComponentCore()
{
}

void CComponentCore::initialize(maci::ContainerServices* services)
{
	m_services=services;
	m_control=NULL;
	m_localOscillatorDevice=Receivers::LocalOscillator::_nil();
	m_localOscillatorFault=false;
}

void CComponentCore::execute() throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl)
{
	m_configuration.init(m_services);  //throw (ComponentErrors::CDBAccessExImpl);
	try {

		/*m_control=new IRA::ReceiverControl((const char *)m_configuration.getDewarIPAddress(),m_configuration.getDewarPort(),
																					(const char *)m_configuration.getLNAIPAddress(),m_configuration.getLNAPort());*/
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CComponentCore::execute()");
		throw dummy;
	}
	catch (IRA::ReceiverControlEx& ex) {
		_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CComponentCore::execute()");
		throw dummy;
	}
	//members initialization
	m_startFreq.length(m_configuration.getIFs());
	m_bandwidth.length(m_configuration.getIFs());
	m_polarization.length(m_configuration.getIFs());
	for (WORD i=0;i<m_configuration.getIFs();i++) {
		m_startFreq[i]=m_configuration.getIFMin()[i];
		m_bandwidth[i]=m_configuration.getIFBandwidth()[i];
		m_polarization[i]=(long)m_configuration.getPolarizations()[i];
	}
	m_localOscillatorValue=0.0;
}

void CComponentCore::cleanup()
{
	//make sure no one is using the object
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_control!=NULL) {
		delete m_control;
	}
}

void CComponentCore::getLO(ACS::doubleSeq& lo)
{
	lo.length(m_configuration.getIFs());
	for (WORD i=0;i<m_configuration.getIFs();i++) {
		lo[i]=m_localOscillatorValue;
	}
}

void CComponentCore::setLO(const ACS::doubleSeq& lo) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl)
{
	double trueValue,amp;
	double *freq=NULL;
	double *power=NULL;
	DWORD size;
	baci::ThreadSyncGuard guard(&m_mutex);
	if (lo.length()==0) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CComponentCore::setLO");
		impl.setReason("at least one value must be provided");
		throw impl;
	}
	// in case -1 is given we keep the current value...so nothing to do
	if (lo[0]==-1) {
		ACS_LOG(LM_FULL_INFO,"CComponentCore::setLO()",(LM_NOTICE,"KEEP_CURRENT_LOCAL_OSCILLATOR %lf",m_localOscillatorValue));
		return;
	}
	// now check if the requested value match the limits
	if (lo[0]<m_configuration.getLOMin()[0]) {
		_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::setLO");
		impl.setValueName("local oscillator lower limit");
		impl.setValueLimit(m_configuration.getLOMin()[0]);
		throw impl;
	}
	else if (lo[0]>m_configuration.getLOMax()[0]) {
		_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CComponentCore::setLO");
		impl.setValueName("local oscillator upper limit");
		impl.setValueLimit(m_configuration.getLOMax()[0]);
		throw impl;
	}
	//computes the synthesizer settings
	trueValue=lo[0]+m_configuration.getFixedLO2()[0];
	size=m_configuration.getSynthesizerTable(freq,power);
	amp=round(linearFit(freq,power,size,trueValue));
	delete [] power;
	delete [] freq;
	ACS_LOG(LM_FULL_INFO,"CComponentCore::setLO()",(LM_DEBUG,"SYNTHESIZER_VALUES %lf %lf",trueValue,amp));
	/***********************************************************************/
	/****   COMMENT OUT when the local oscillator component will be available                 */
	/***********************************************************************/
	// make sure the synthesizer component is available
	/*loadLocalOscillator(); // throw (ComponentErrors::CouldntGetComponentExImpl)
	try {
		m_localOscillatorDevice->set(amp,trueValue);
	}
	catch (CORBA::SystemException) {
		m_localOscillatorFault=true;
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CScheduleExecutor::configureBackend()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		throw impl;
	}
	catch () { // AT the moment no excpt is thorown...... ASK clarification

	}*/
	// now that the local oscillator has been properly set...let's do some easy computations
	m_localOscillatorValue=lo[0];
	for (WORD i=0;i<m_configuration.getIFs();i++) {
		m_bandwidth[i]=m_configuration.getRFMax()[i]-(m_startFreq[i]+m_localOscillatorValue);
		// the if bandwidth could never be larger than the max IF bandwidth:
		if (m_bandwidth[i]>m_configuration.getIFBandwidth()[i]) m_bandwidth[i]=m_configuration.getIFBandwidth()[i];
	}
	ACS_LOG(LM_FULL_INFO,"CComponentCore::setLO()",(LM_NOTICE,"LOCAL_OSCILLATORS_NOW %lf",m_localOscillatorValue));
}

void CComponentCore::loadLocalOscillator() throw (ComponentErrors::CouldntGetComponentExImpl)
{
	if ((!CORBA::is_nil(m_localOscillatorDevice)) && (m_localOscillatorFault)) { // if reference was already taken, but an error was found....dispose the reference
		try {
			m_services->releaseComponent((const char*)m_localOscillatorDevice->name());
		}
		catch (...) { //dispose silently...if an error...no matter
		}
		m_localOscillatorDevice=Receivers::LocalOscillator::_nil();
	}
	if (CORBA::is_nil(m_localOscillatorDevice)) {  //only if it has not been retrieved yet
		try {
			m_localOscillatorDevice=m_services->getComponent<Receivers::LocalOscillator>((const char*)m_configuration.getLocalOscillatorInstance());
			ACS_LOG(LM_FULL_INFO,"CCore::loadAntennaBoss()",(LM_INFO,"LOCAL_OSCILLATOR_OBTAINED"));
			m_localOscillatorFault=false;
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CComponentCore::loadLocalOscillator()");
			Impl.setComponentName((const char*)m_configuration.getLocalOscillatorInstance());
			m_localOscillatorDevice=Receivers::LocalOscillator::_nil();
			throw Impl;
		}
		catch (maciErrType::NoPermissionExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CComponentCore::loadLocalOscillator()");
			Impl.setComponentName((const char*)m_configuration.getLocalOscillatorInstance());
			m_localOscillatorDevice=Receivers::LocalOscillator::_nil();
			throw Impl;
		}
		catch (maciErrType::NoDefaultComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CComponentCore::loadLocalOscillator()");
			Impl.setComponentName((const char*)m_configuration.getLocalOscillatorInstance());
			m_localOscillatorDevice=Receivers::LocalOscillator::_nil();
			throw Impl;
		}
	}
}

void CComponentCore::unloadLocalOscillator()
{
	if (!CORBA::is_nil(m_localOscillatorDevice)) {
		try {
			m_services->releaseComponent((const char*)m_localOscillatorDevice->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CComponentCore::unloadLocalOscillator()");
			Impl.setComponentName((const char *)m_configuration.getLocalOscillatorInstance());
			Impl.log(LM_WARNING);
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CComponentCore::unloadLocalOscillator()");
			impl.log(LM_WARNING);
		}
		m_localOscillatorDevice=Receivers::LocalOscillator::_nil();
	}
}

double CComponentCore::linearFit(double *X,double *Y,const WORD& size,double x)
{
	int low=-1,high=-1;
	for (WORD j=0;j<size;j++) {
		if (x==X[j]) {
			return Y[j];
		}
		else if (x<X[j]) {
			if (high==-1) high=j;
			else if (X[j]<X[high]) high=j;
		}
		else if (x>X[j]) { // X value is lower
			if (low==-1) low=j;
			else if (X[j]>X[low]) low=j;
		}
	}
	if ((high!=-1) && (low!=-1)) {
		double slope=X[low]-X[high];
		return ((x-X[high])/slope)*Y[low]-((x-X[low])/slope)*Y[high];
	}
	else if (high==-1) {
		return Y[low];
	}
	else if (low==-1) {
		return Y[high];
	}
	else return 0.0; //this will never happen if size!=0
}
