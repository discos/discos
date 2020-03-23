#include "RecvBossCore.h"

#ifdef COMPILE_TARGET_MED

#elif COMPILE_TARGET_NT

#else

#include <ReceiversModule.h>
#include <DewarPositionerDefinitionsC.h>
#include "slamac.h"

#endif

using namespace IRA;

CRecvBossCore::CRecvBossCore() 
{
}

CRecvBossCore::~CRecvBossCore()
{
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
			if (CORBA::is_nil(m_currentRecv)) {
				_EXCPT(ComponentErrors::CouldntGetComponentExImpl,Impl,"CRecvBossCore::loadReceiver()");
				Impl.setComponentName((const char*)m_currentRecvInstance);
				m_currentRecv=Receivers::Receiver::_nil();
				changeBossStatus(Management::MNG_FAILURE);
				throw Impl;
			}
			else {
				ACS_LOG(LM_FULL_INFO,"CRecvBossCore::loadReceiver()",(LM_INFO,"RECEIVER_OBTAINED"));
				m_currentRecvError=false;
			}
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
		catch (...) {
			_EXCPT(ComponentErrors::CouldntGetComponentExImpl,Impl,"CRecvBossCore::loadReceiver()");
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
			m_currentRecv=Receivers::Receiver::_nil();
			m_currentRecvError=false;
			Impl.log(LM_WARNING);
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::unloadReceiver()");
			m_currentRecv=Receivers::Receiver::_nil();
			m_currentRecvError=false;
			impl.log(LM_WARNING);
		}
		m_currentRecv=Receivers::Receiver::_nil();
		m_currentRecvError=false;
	}
}

void CRecvBossCore::AUOn() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
				ComponentErrors::UnexpectedExImpl)
{
#ifdef COMPILE_TARGET_MED
	CUSTOM_LOG(LM_FULL_INFO,"CRecvBossCore::AUOn()",(LM_NOTICE,"Antenna Unit not supported"));
	return; 
#elif COMPILE_TARGET_NT
	CUSTOM_LOG(LM_FULL_INFO,"CRecvBossCore::AUOn()",(LM_NOTICE,"Antenna Unit not supported"));
	return; 
#else	
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
#endif
}

void CRecvBossCore::AUOff() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
				ComponentErrors::UnexpectedExImpl)
{
#ifdef COMPILE_TARGET_MED
	CUSTOM_LOG(LM_FULL_INFO,"CRecvBossCore::AUOff()",(LM_NOTICE,"Antenna Unit not supported"));
	return;
#elif COMPILE_TARGET_NT
	CUSTOM_LOG(LM_FULL_INFO,"CRecvBossCore::AUOff()",(LM_NOTICE,"Antenna Unit not supported"));
	return;
#else	
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
#endif
}

void CRecvBossCore::externalCalOn() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
				ComponentErrors::UnexpectedExImpl)
{
#ifdef COMPILE_TARGET_MED
	CUSTOM_LOG(LM_FULL_INFO,"CRecvBossCore::externalCalOn()",(LM_NOTICE,"External diode control not supported"));
	return;
#elif COMPILE_TARGET_NT
	CUSTOM_LOG(LM_FULL_INFO,"CRecvBossCore::externalCalOn()",(LM_NOTICE,"External diode control not supported"));
	return;
#else
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentRecvCode=="") {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::externalCalOn()");
		impl.setReason("Receiver not configured yet");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
	try {
		m_currentRecv->externalCalOn();
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::externalCalOn()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::externalCalOn()");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (ReceiversErrors::ReceiversErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::externalCalOn()");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::externalCalOn()");
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
#endif
}

void CRecvBossCore::externalCalOff() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
				ComponentErrors::UnexpectedExImpl)
{
#ifdef COMPILE_TARGET_MED
	CUSTOM_LOG(LM_FULL_INFO,"CRecvBossCore::externalCalOff()",(LM_NOTICE,"External diode control not supported"));
	return;
#elif COMPILE_TARGET_NT
	CUSTOM_LOG(LM_FULL_INFO,"CRecvBossCore::externalCalOff()",(LM_NOTICE,"External diode control not supported"));
	return;
#else	
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentRecvCode=="") {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::externalCalOff()");
		impl.setReason("Receiver not configured yet");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	loadReceiver(); //  ComponentErrors::CouldntGetComponentExImpl
	try {
		m_currentRecv->externalCalOff();
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::externalCalOff()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::externalCalOff()");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (ReceiversErrors::ReceiversErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::UnavailableReceiverOperationExImpl,impl,ex,"CRecvBossCore::externalCalOff()");
		impl.setReceiverCode((const char *)m_currentRecvCode);
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::externalCalOff()");
		changeBossStatus(Management::MNG_FAILURE);
		m_currentRecvError=true;
		throw impl;
	}
#endif
}

void CRecvBossCore::getDewarParameter(Receivers::TDerotatorConfigurations& mod,double& pos) throw (
  ReceiversErrors::DewarPositionerCommandErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
{
#ifdef COMPILE_TARGET_MED
	mod=Receivers::RCV_UNDEF_DEROTCONF;
	pos=0.0;
#elif COMPILE_TARGET_NT
	mod=Receivers::RCV_UNDEF_DEROTCONF;
	pos=0.0;
#else		
	baci::ThreadSyncGuard guard(&m_mutex);
	mod=m_updateMode;
	if (m_updateMode==Receivers::RCV_UNDEF_DEROTCONF) {
		pos=0.0;
		return;
	}
	else {
		//No checks....if the current configuration is properly setup, that means the derotator is available.
		try {
			Receivers::TDewarPositionerScanInfo info=m_dewarPositioner->getScanInfo();
			pos=info.iStaticPos;
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ReceiversErrors::DewarPositionerCommandErrorExImpl,impl,ex,"CRecvBossCore::setDerotatorPosition()");
			impl.setCommand("getScanInfo()");
			changeBossStatus(Management::MNG_FAILURE);
			throw impl;
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::getDewarParameter()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			changeBossStatus(Management::MNG_WARNING);
			m_dewarPositionerError=true;
			throw impl;
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::getDewarParameter()");
			changeBossStatus(Management::MNG_WARNING);
			m_dewarPositionerError=true;
			throw impl;
		}
	}
#endif
}

double CRecvBossCore::getDerotatorPosition(const ACS::Time& epoch) throw (ComponentErrors::CouldntGetComponentExImpl,
		ReceiversErrors::DewarPositionerCommandErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
{
#ifdef COMPILE_TARGET_MED
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::getDerotatorPosition()");
	m_status=Management::MNG_WARNING;
	throw impl;
#elif COMPILE_TARGET_NT
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::getDerotatorPosition()");
	m_status=Management::MNG_WARNING;
	throw impl;
#else		
	IRA::CString component;
	bool derotator;
	if (m_currentRecvCode=="") {
		return -9999.99;
	}
	if (!m_config->getReceiver(m_currentRecvCode,component,derotator)) {
		return -9999.99;
	}
	if (m_config->dewarPositionerInterface()=="") {
	}
	if (!derotator) {
		return -9999.99;
	}
	if (m_updateMode==Receivers::RCV_UNDEF_DEROTCONF) {
		return -9999.99;
	}
	baci::ThreadSyncGuard guard(&m_mutex);
	loadDewarPositioner(); // ComponentErrors::CouldntGetComponentExImpl
	try {
		return m_dewarPositioner->getPositionFromHistory(epoch)*DD2R;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::DewarPositionerCommandErrorExImpl,impl,ex,"CRecvBossCore::getDerotatorPosition()");
		impl.setCommand("setPosition()");
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::getDerotatorPosition()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE);
		m_dewarPositionerError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::getDerotatorPosition()");
		changeBossStatus(Management::MNG_FAILURE);
		m_dewarPositionerError=true;
		throw impl;
	}
#endif
}

void CRecvBossCore::setDerotatorPosition(const double& pos) throw (ReceiversErrors::NoDewarPositioningExImpl,
		ReceiversErrors::NoDerotatorAvailableExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,
		ReceiversErrors::DewarPositionerCommandErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
		ReceiversErrors::DewarPositionerNotConfiguredExImpl)
{
#ifdef COMPILE_TARGET_MED
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::setDerotatorPosition()");
	m_status=Management::MNG_WARNING;
	throw impl;
#elif COMPILE_TARGET_NT
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::setDerotatorPosition()");
	m_status=Management::MNG_WARNING;
	throw impl;
#else		
	IRA::CString component;
	bool derotator;
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentRecvCode=="") {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::setDerotatorPosition()");
		impl.setReason("Receiver not configured yet");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	// the receiver code should be validated....
	if (!m_config->getReceiver(m_currentRecvCode,component,derotator)) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::setDerotatorPosition()");
		impl.setReason("Receiver code is not known");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	// a dewar positioner must be available.....
	if (m_config->dewarPositionerInterface()=="") {
		_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::setDerotatorPosition()");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	// receiver must be equipped with a derotator
	if (!derotator) {
		_EXCPT(ReceiversErrors::NoDerotatorAvailableExImpl,impl,"CRecvBossCore::setDerotatorPosition()");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	if (m_updateMode==Receivers::RCV_UNDEF_DEROTCONF) {
		_EXCPT(ReceiversErrors::DewarPositionerNotConfiguredExImpl,impl,"CRecvBossCore::setDerotatorPosition()");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	loadDewarPositioner(); // ComponentErrors::CouldntGetComponentExImpl
	try {
		m_dewarPositioner->setPosition(pos);
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::DewarPositionerCommandErrorExImpl,impl,ex,"CRecvBossCore::setDerotatorPosition()");
		impl.setCommand("setPosition()");
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::setDerotatorPosition()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE);
		m_dewarPositionerError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::setDerotatorPosition()");
		changeBossStatus(Management::MNG_FAILURE);
		m_dewarPositionerError=true;
		throw impl;
	}
#endif
}

void CRecvBossCore::derotatorRewind(const long& steps) throw (ComponentErrors::ValidationErrorExImpl,
  ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,
  ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::DewarPositionerCommandErrorExImpl,
  ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
  ReceiversErrors::DewarPositionerNotConfiguredExImpl)
{
#ifdef COMPILE_TARGET_MED
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorRewind()");
	m_status=Management::MNG_WARNING;
	throw impl;
#elif COMPILE_TARGET_NT
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorRewind()");
	m_status=Management::MNG_WARNING;
	throw impl;
#else		
	baci::ThreadSyncGuard guard(&m_mutex);
	IRA::CString component;
	bool derotator;
	// receiver must be yet configured.....
	if (m_currentRecvCode=="") {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::derotatorRewind()");
		impl.setReason("Receiver not configured yet");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	// the receiver code should be validated....
	if (!m_config->getReceiver(m_currentRecvCode,component,derotator)) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::derotatorRewind()");
		impl.setReason("Receiver code is not known");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	// a dewar positioner must be available.....
	if (m_config->dewarPositionerInterface()=="") {
		_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorRewind()");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	// receiver must be equipped with a derotator
	if (!derotator) {
		_EXCPT(ReceiversErrors::NoDerotatorAvailableExImpl,impl,"CRecvBossCore::derotatorRewind()");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	if (m_updateMode==Receivers::RCV_UNDEF_DEROTCONF) {
		_EXCPT(ReceiversErrors::DewarPositionerNotConfiguredExImpl,impl,"CRecvBossCore::derotatorRewind()");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	loadDewarPositioner(); // ComponentErrors::CouldntGetComponentExImpl
	try {
		m_dewarPositioner->rewind(steps);
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::DewarPositionerCommandErrorExImpl,impl,ex,"CRecvBossCore::derotatorRewind()");
		impl.setCommand("rewind()");
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::derotatorRewind()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE);
		m_dewarPositionerError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::derotatorRewind()");
		changeBossStatus(Management::MNG_FAILURE);
		m_dewarPositionerError=true;
		throw impl;
	}
#endif
}

void CRecvBossCore::derotatorSetConfiguration(const Receivers::TDerotatorConfigurations& conf) throw (
		ComponentErrors::ValidationErrorExImpl,ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::DewarPositionerSetupErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl)
{
#ifdef COMPILE_TARGET_MED
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorSetConfiguration()");
	m_status=Management::MNG_WARNING;
	throw impl;
#elif COMPILE_TARGET_NT
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorSetConfiguration()");
	m_status=Management::MNG_WARNING;
	throw impl;
#else		
	baci::ThreadSyncGuard guard(&m_mutex);
	IRA::CString component;
	bool derotator;

	// manage default parameter '*'
	if (conf==Receivers::RCV_UNDEF_DEROTCONF) {
		return;
	}
	// receiver must be yet configured.....
	if (m_currentRecvCode=="") {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::derotatorSetConfiguration()");
		impl.setReason("Receiver not configured yet");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	// the receiver code should be validated....
	if (!m_config->getReceiver(m_currentRecvCode,component,derotator)) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::derotatorSetConfiguration()");
		impl.setReason("Receiver code is not known");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	// a dewar positioner must be available.....
	if (m_config->dewarPositionerInterface()=="") {
		_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorSetConfiguration()");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	// receiver must be equipped with a derotator
	if (!derotator) {
		_EXCPT(ReceiversErrors::NoDerotatorAvailableExImpl,impl,"CRecvBossCore::derotatorSetConfiguration()");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	loadDewarPositioner(); // ComponentErrors::CouldntGetComponentExImpl
	//setup the proper dewar for derotation
	try {
		if (m_updateMode!=conf) {
			if (m_dewarPositioner->getActualSetup()!=m_currentRecvCode) {
				m_dewarPositioner->setup(m_currentRecvCode);
			}
			m_rewindMode=Receivers::RCV_AUTO_REWIND;
			m_autoRewindSteps=1;
			m_dewarPositioner->setConfiguration(Receivers::Definitions::map(conf));
			m_updateMode=conf;
		}
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::DewarPositionerSetupErrorExImpl,impl,ex,"CRecvBossCore::derotatorSetConfiguration()");
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::derotatorSetConfiguration()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE);
		m_dewarPositionerError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::derotatorSetConfiguration()");
		changeBossStatus(Management::MNG_FAILURE);
		m_dewarPositionerError=true;
		throw impl;
	}
#endif
}

void CRecvBossCore::derotatorSetRewindingMode(const Receivers::TRewindModes& rewind) throw (
		ComponentErrors::ValidationErrorExImpl,ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::DewarPositionerSetupErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ReceiversErrors::DewarPositionerNotConfiguredExImpl)
{
#ifdef COMPILE_TARGET_MED
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorSetRewindingMode()");
	m_status=Management::MNG_WARNING;
	throw impl;
#elif COMPILE_TARGET_NT
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorSetRewindingMode()");
	m_status=Management::MNG_WARNING;
	throw impl;
#else		
	
	baci::ThreadSyncGuard guard(&m_mutex);
	IRA::CString component;
	bool derotator;

	// manage default parameter '*'
	if (rewind==Receivers::RCV_UNDEF_REWIND) {
		return;
	}
	// receiver must be yet configured.....
	if (m_currentRecvCode=="") {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::derotatorSetRewindingMode()");
		impl.setReason("Receiver not configured yet");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	// the receiver code should be validated....
	if (!m_config->getReceiver(m_currentRecvCode,component,derotator)) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::derotatorSetRewindingMode()");
		impl.setReason("Receiver code is not known");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	// a dewar positioner must be available.....
	if (m_config->dewarPositionerInterface()=="") {
		_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorSetRewindingMode()");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	// receiver must be equipped with a derotator
	if (!derotator) {
		_EXCPT(ReceiversErrors::NoDerotatorAvailableExImpl,impl,"CRecvBossCore::derotatorSetRewindingMode()");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	if (m_updateMode==Receivers::RCV_UNDEF_DEROTCONF) {
		_EXCPT(ReceiversErrors::DewarPositionerNotConfiguredExImpl,impl,"CRecvBossCore::derotatorSetRewindingMode()");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	loadDewarPositioner(); // ComponentErrors::CouldntGetComponentExImpl
	try {
		if (m_rewindMode!=rewind) {
			m_dewarPositioner->setRewindingMode(Receivers::Definitions::map(rewind));
			m_rewindMode=rewind;
		}
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::DewarPositionerSetupErrorExImpl,impl,ex,"CRecvBossCore::derotatorSetRewindingMode()");
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::derotatorSetRewindingMode()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE);
		m_dewarPositionerError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::derotatorSetRewindingMode()");
		changeBossStatus(Management::MNG_FAILURE);
		m_dewarPositionerError=true;
		throw impl;
	}
#endif
}

void CRecvBossCore::derotatorSetAutoRewindingSteps(const long& steps) throw (
		ComponentErrors::ValidationErrorExImpl,ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::DewarPositionerSetupErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ReceiversErrors::DewarPositionerNotConfiguredExImpl)
{
#ifdef COMPILE_TARGET_MED
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorSetAutoRewindingSteps()");
	m_status=Management::MNG_WARNING;
	throw impl;
#elif COMPILE_TARGET_NT
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorSetAutoRewindingSteps()");
	m_status=Management::MNG_WARNING;
	throw impl;
#else		
	baci::ThreadSyncGuard guard(&m_mutex);
	IRA::CString component;
	bool derotator;

	// manage default parameter '*'
	if (steps==-1) {
		return;
	}
	// receiver must be yet configured.....
	if (m_currentRecvCode=="") {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::derotatorSetAutoRewindingSteps()");
		impl.setReason("Receiver not configured yet");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	// the receiver code should be validated....
	if (!m_config->getReceiver(m_currentRecvCode,component,derotator)) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::derotatorSetAutoRewindingSteps()");
		impl.setReason("Receiver code is not known");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	// a dewar positioner must be available.....
	if (m_config->dewarPositionerInterface()=="") {
		_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorSetAutoRewindingSteps()");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	// receiver must be equipped with a derotator
	if (!derotator) {
		_EXCPT(ReceiversErrors::NoDerotatorAvailableExImpl,impl,"CRecvBossCore::derotatorSetAutoRewindingSteps()");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	if (m_updateMode==Receivers::RCV_UNDEF_DEROTCONF) {
		_EXCPT(ReceiversErrors::DewarPositionerNotConfiguredExImpl,impl,"CRecvBossCore::derotatorSetAutoRewindingSteps()");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	loadDewarPositioner(); // ComponentErrors::CouldntGetComponentExImpl
	try {
		if (m_autoRewindSteps!=steps) {
			m_dewarPositioner->setAutoRewindingSteps(steps);
			m_autoRewindSteps=steps;
		}
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::DewarPositionerSetupErrorExImpl,impl,ex,"CRecvBossCore::derotatorSetAutoRewindingSteps()");
		changeBossStatus(Management::MNG_FAILURE);
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::derotatorSetAutoRewindingSteps()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE);
		m_dewarPositionerError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::derotatorSetAutoRewindingSteps()");
		changeBossStatus(Management::MNG_FAILURE);
		m_dewarPositionerError=true;
		throw impl;
	}
#endif
}

void CRecvBossCore::derotatorPark() throw (ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,
		ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::DewarPositionerParkingErrorExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
{
#ifdef COMPILE_TARGET_MED
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorPark()");
	m_status=Management::MNG_WARNING;
	throw impl;
#elif COMPILE_TARGET_NT
	_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorPark()");
	m_status=Management::MNG_WARNING;
	throw impl;
#else		
	baci::ThreadSyncGuard guard(&m_mutex);
	IRA::CString component;
	bool derotator;
	if (m_updateMode==Receivers::RCV_UNDEF_DEROTCONF) {
		return;
	}
	// receiver must be yet configured.....
	if (m_currentRecvCode=="") {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::derotatorPark()");
		impl.setReason("Receiver not configured yet");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	// the receiver code should be validated....
	if (!m_config->getReceiver(m_currentRecvCode,component,derotator)) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::derotatorPark()");
		impl.setReason("Receiver code is not known");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	// a dewar positioner must be available.....
	if (m_config->dewarPositionerInterface()=="") {
		_EXCPT(ReceiversErrors::NoDewarPositioningExImpl,impl,"CRecvBossCore::derotatorPark()");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	// receiver must be equipped with a derotator
	if (!derotator) {
		_EXCPT(ReceiversErrors::NoDerotatorAvailableExImpl,impl,"CRecvBossCore::derotatorPark()");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	m_updateMode=Receivers::RCV_UNDEF_DEROTCONF;
	m_rewindMode=Receivers::RCV_UNDEF_REWIND;
	m_autoRewindSteps=-1;
	m_dewarIsMoving=false;
	loadDewarPositioner(); // ComponentErrors::CouldntGetComponentExImpl
	try {
		// According to IDL documentation this should also:
		// 1) resets offsets
		// 2) put the derotator to park position
		// 3) set rewind mode to UNKNOWN
		// 4) set the configuration to UNKNOWN
		m_dewarPositioner->park();
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ReceiversErrors::DewarPositionerParkingErrorExImpl,impl,ex,"CRecvBossCore::derotatorPark()");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::derotatorPark()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_WARNING);
		m_dewarPositionerError=true;
		throw impl;
	}
	catch (...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::derotatorPark()");
		changeBossStatus(Management::MNG_WARNING);
		m_dewarPositionerError=true;
		throw impl;
	}
#endif
}

#ifdef COMPILE_TARGET_MED

#include "RecvBossCore_mc.i"

#elif COMPILE_TARGET_NT

#include "RecvBossCore_nt.i"

#else

void CRecvBossCore::initialize(maci::ContainerServices* services,CConfiguration *config,acscomponent::ACSComponentImpl *me) throw (
		ComponentErrors::UnexpectedExImpl)
{
	m_status=Management::MNG_OK;
	m_bossStatus=Management::MNG_OK;
	m_recvStatus=Management::MNG_WARNING;
	m_dewarStatus=Management::MNG_WARNING;
	m_dewarTracking=false;
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
	m_updateMode=Receivers::RCV_UNDEF_DEROTCONF;
	m_rewindMode=Receivers::RCV_UNDEF_REWIND;
	m_autoRewindSteps=-1;
	m_dewarIsMoving=false;
	m_notificationChannel=NULL;
	m_lastScanTime=0;

	ACS_LOG(LM_FULL_INFO,"CRecvBossCore::initialize()",(LM_INFO,"OPENING_RECEIVERS_BOSS_NOTIFICATION_CHANNEL"));
	try {
		m_notificationChannel=new nc::SimpleSupplier(Receivers::RECEIVERS_DATA_CHANNEL,me);
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CRecvBossCore::initialize()");
	}
	m_dewarPositioner=Receivers::DewarPositioner::_nil();
	m_dewarPositionerError=false;
	m_loEpoch=m_starFreqEpoch=m_bandWidthEpoch=m_polEpoch=m_feedsEpoch=m_IFsEpoch=m_modeEpoch=m_recvStatusEpoch=m_dewarStatusEpoch=0;
}

void CRecvBossCore::execute() throw (ComponentErrors::IRALibraryResourceExImpl,ComponentErrors::CDBAccessExImpl,ComponentErrors::SocketErrorExImpl)
{
}

void CRecvBossCore::cleanUp()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_notificationChannel!=NULL) {
		m_notificationChannel->disconnect();
		m_notificationChannel=NULL;
	}
	unloadReceiver();
	unloadDewarPositioner();
	m_currentRecvCode="";
	m_currentRecvInstance="";
}

long CRecvBossCore::getFeeds(ACS::doubleSeq& X,ACS::doubleSeq& Y,ACS::doubleSeq& power) throw (ComponentErrors::ValidationErrorExImpl,
		ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
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

void CRecvBossCore::calOn() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
				ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
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

void CRecvBossCore::calOff() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
				ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
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

void CRecvBossCore::setLO(const ACS::doubleSeq& lo) throw (ComponentErrors::ValidationErrorExImpl,
	ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,
	ReceiversErrors::UnavailableReceiverOperationExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
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
		ComponentErrors::UnexpectedExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntGetComponentExImpl)
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

void CRecvBossCore::park() throw (ManagementErrors::ParkingErrorExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (!CORBA::is_nil(m_currentRecv)) {
		try {
			m_currentRecv->deactivate();
		}
		catch (...) {
			ACS_LOG(LM_FULL_INFO,"CRecvBossCore::park()",(LM_WARNING,"COULD_NOT_DEACTIVATE_CURRENT_RECEIVER"));
		}
	}
	unloadReceiver();
	m_currentRecvCode="";
	m_currentRecvInstance="";
	m_recvStatus=Management::MNG_WARNING;
	m_dewarStatus=Management::MNG_WARNING;
	m_dewarTracking=false;
	m_lo.length(0);
	m_startFreq.length(0);
	m_bandWidth.length(0);
	m_pol.length(0);
	m_currentOperativeMode="";
	m_feeds=m_IFs=0;
	m_loEpoch=m_starFreqEpoch=m_bandWidthEpoch=m_polEpoch=m_feedsEpoch=m_IFsEpoch=m_modeEpoch=m_recvStatusEpoch=m_dewarStatusEpoch=0;
	try {
		if (m_updateMode==Receivers::RCV_UNDEF_DEROTCONF) {
			derotatorPark();
		}
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		_ADD_BACKTRACE(ManagementErrors::ParkingErrorExImpl,impl,ex,"CRecvBossCore::park()");
		impl.setSubsystem("Receivers");
		impl.setReason("could not park derotator");
		throw impl;
	}
	unloadDewarPositioner();
}

double CRecvBossCore::getTaper(const double& freq,const double& bw,const long& feed,const long& ifNumber,double& waveLen) throw (ComponentErrors::ValidationErrorExImpl,
		ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::CouldntGetComponentExImpl)
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

void CRecvBossCore::getIFOutput(const ACS::longSeq& feeds,const ACS::longSeq& ifs,ACS::doubleSeq& freqs,ACS::doubleSeq&  bw,ACS::longSeq& pols,ACS::doubleSeq& LO)  throw (
		ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_currentRecvCode=="") {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::getIFOutput()");
		impl.setReason("Receiver not configured yet");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
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
}

void CRecvBossCore::getCalibrationMark(ACS::doubleSeq& result,ACS::doubleSeq& resFreq,ACS::doubleSeq& resBw,const ACS::doubleSeq& freqs,const ACS::doubleSeq& bandwidths,
		const ACS::longSeq& feeds,const ACS::longSeq& ifs,bool& onoff,double &scale) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CORBAProblemExImpl,
				ReceiversErrors::UnavailableReceiverOperationExImpl,ComponentErrors::UnexpectedExImpl,
				ComponentErrors::CouldntGetComponentExImpl)
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

void CRecvBossCore::closeScan(ACS::Time& timeToStop) throw (ReceiversErrors::DewarPositionerCommandErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	timeToStop=0;
	baci::ThreadSyncGuard guard(&m_mutex);
	if (m_dewarIsMoving) {
		try {
			m_dewarPositioner->stopUpdating();
			m_dewarIsMoving=false;
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ReceiversErrors::DewarPositionerCommandErrorExImpl,impl,ex,"CRecvBossCore::closeScan()");
			impl.setCommand("stopUpdating()");
			changeBossStatus(Management::MNG_FAILURE);
			throw impl;
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::closeScan()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			changeBossStatus(Management::MNG_WARNING);
			m_dewarPositionerError=true;
			throw impl;
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::closeScan()");
			changeBossStatus(Management::MNG_WARNING);
			m_dewarPositionerError=true;
			throw impl;
		}
	}
}

bool CRecvBossCore::checkScan(ACS::Time& startUT,const Receivers::TReceiversParameters& param,const Antenna::TRunTimeParameters& antennaInfo,
     const Management::TScanConfiguration& scanConf,Receivers::TRunTimeParameters& runTime) throw(
     ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,
     ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl,
     ReceiversErrors::DewarPositionerCommandErrorExImpl)
{
	IRA::CString component;
	ACS::Time estimatedTime;
	bool derotator,res,newTarget;
	res=false;
	baci::ThreadSyncGuard guard(&m_mutex);
	if (!m_config->getReceiver(m_currentRecvCode,component,derotator)) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::checkScan()");
		impl.setReason("Receiver code is not known");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	runTime.onTheFly=false;
	if (m_lastScanTime!=scanConf.timeStamp) {
		newTarget=true;
	}
	else {
		newTarget=false;
	}
	if ((m_config->dewarPositionerInterface()!="") && (derotator) && (m_updateMode!=Receivers::RCV_UNDEF_DEROTCONF))  {
		loadDewarPositioner(); // ComponentErrors::CouldntGetComponentExImpl
		try {
			res=m_dewarPositioner->checkUpdating(startUT,antennaInfo.axis,antennaInfo.section,
			  antennaInfo.azimuth,antennaInfo.elevation,antennaInfo.rightAscension,antennaInfo.declination,newTarget,
			  estimatedTime);
			  runTime.onTheFly=true;
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ReceiversErrors::DewarPositionerCommandErrorExImpl,impl,ex,"CRecvBossCore::checkScan()");
			impl.setCommand("checkScan()");
			changeBossStatus(Management::MNG_FAILURE);
			throw impl;
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::checkScan()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			changeBossStatus(Management::MNG_WARNING);
			m_dewarPositionerError=true;
			throw impl;
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::checkScan()");
			changeBossStatus(Management::MNG_WARNING);
			m_dewarPositionerError=true;
			throw impl;
		}
	}
	runTime.timeToStop=0;
	if (res) {
		if (startUT<=0) runTime.startEpoch=estimatedTime;
		else runTime.startEpoch=startUT;
	}	
	return res;
}

void CRecvBossCore::startScan(ACS::Time& startUT,const Receivers::TReceiversParameters& param,const Antenna::TRunTimeParameters & antennaInfo,
  const Management::TScanConfiguration& scanConf) throw (ComponentErrors::ValidationErrorExImpl,
  ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl,
  ReceiversErrors::DewarPositionerCommandErrorExImpl)
{

	IRA::CString component;
	bool derotator,newTarget;
	baci::ThreadSyncGuard guard(&m_mutex);
	//*********************************************************
	// At the moment no scans are included in for receivers boss. so:
	// I comment out the controls and the error thrown if the receiver is not configured.
	// They should be restored when the check (is_emptyscan=false) could be done.
	//*********************************************************
	if (m_currentRecvCode=="") {
		//_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::startScan()");
		//impl.setReason("Receiver not configured yet");
		//changeBossStatus(Management::MNG_WARNING);
		//throw impl;
	}
	// the receiver code should be validated....
	if (!m_config->getReceiver(m_currentRecvCode,component,derotator)) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CRecvBossCore::startScan()");
		impl.setReason("Receiver code is not known");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	// now check if the start scan have to deal also with the derotator
	if ((m_config->dewarPositionerInterface()!="") && (derotator) && (m_updateMode!=Receivers::RCV_UNDEF_DEROTCONF))  {
		loadDewarPositioner(); // ComponentErrors::CouldntGetComponentExImpl
		if (m_lastScanTime!=scanConf.timeStamp) {
			newTarget=true;
			m_lastScanTime=scanConf.timeStamp;
		}
		else {
			newTarget=false;
		}
		try {
			m_dewarPositioner->startUpdating(antennaInfo.axis,antennaInfo.section,antennaInfo.azimuth,
					antennaInfo.elevation,antennaInfo.rightAscension,antennaInfo.declination,newTarget);
			m_dewarIsMoving=true;
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ReceiversErrors::DewarPositionerCommandErrorExImpl,impl,ex,"CRecvBossCore::startScan()");
			impl.setCommand("setPosition()");
			changeBossStatus(Management::MNG_FAILURE);
			throw impl;
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::startScan()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			changeBossStatus(Management::MNG_WARNING);
			m_dewarPositionerError=true;
			throw impl;
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::startScan()");
			changeBossStatus(Management::MNG_WARNING);
			m_dewarPositionerError=true;
			throw impl;
		}
	}
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

void CRecvBossCore::updateDewarPositionerStatus() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::OperationErrorExImpl,ReceiversErrors::DewarPositionerCommandErrorExImpl)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	IRA::CString component;
	bool derotator;
	TIMEVALUE now;
	if (m_currentRecvCode=="") {
		m_dewarStatus=Management::MNG_WARNING;
		return;
	}
	if (!m_config->getReceiver(m_currentRecvCode,component,derotator)) {
		m_dewarStatus=Management::MNG_WARNING;
		return;
	}
	if (m_updateMode!=Receivers::RCV_UNDEF_DEROTCONF) {
	//if ((derotator) &&  (m_config->dewarPositionerInterface()!="")) {
		IRA::CIRATools::getTime(now);
		if (now.value().value>m_dewarStatusEpoch+m_config->propertyUpdateTime()*10) {
			loadDewarPositioner(); // ComponentErrors::CouldntGetComponentExImpl
			try {
				m_dewarTracking=m_dewarPositioner->isTracking();
			}
			catch (ComponentErrors::ComponentErrorsEx& ex) {
				_ADD_BACKTRACE(ReceiversErrors::DewarPositionerCommandErrorExImpl,impl,ex,"CRecvBossCore::updateDewarPositionerStatus");
				impl.setCommand("isTracking()");
				changeBossStatus(Management::MNG_FAILURE);
				throw impl;
			}
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::updateDewarPositionerStatus()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				changeBossStatus(Management::MNG_WARNING);
				m_dewarPositionerError=true;
				throw impl;
			}
			catch (...) {
				_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::updateDewarPositionerStatus()");
				changeBossStatus(Management::MNG_WARNING);
				m_dewarPositionerError=true;
				throw impl;
			}
			try {
				m_dewarStatus=m_dewarPositioner->getManagementStatus();
			}
			catch (ComponentErrors::ComponentErrorsEx& ex) {
				_ADD_BACKTRACE(ReceiversErrors::DewarPositionerCommandErrorExImpl,impl,ex,"CRecvBossCore::updateDewarPositionerStatus");
				impl.setCommand("getManagementStatus()");
				changeBossStatus(Management::MNG_FAILURE);
				throw impl;
			}
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CRecvBossCore::updateDewarPositionerStatus()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				changeBossStatus(Management::MNG_WARNING);
				m_dewarPositionerError=true;
				throw impl;
			}
			catch (...) {
				_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::updateDewarPositionerStatus()");
				changeBossStatus(Management::MNG_WARNING);
				m_dewarPositionerError=true;
				throw impl;
			}
			m_dewarStatusEpoch=now.value().value;
		}
	}
	else { // case the derotator is not used or supported
		m_dewarStatus=Management::MNG_OK;
		m_dewarTracking=true;
		m_dewarStatusEpoch=now.value().value;
	}
}

const Management::TSystemStatus& CRecvBossCore::getStatus()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	//first of all try to clear the boss status...
	changeBossStatus(Management::MNG_OK); // it will be cleared only if the status persistence time is elapsed
	if (m_recvStatus>m_dewarStatus) {
		m_status=m_recvStatus;
	}
	else {
		m_status=m_dewarStatus;
	}
	if (m_bossStatus>m_status) {
		m_status=m_bossStatus;
	}
	return m_status;
}

void CRecvBossCore::publishData() throw (ComponentErrors::NotificationChannelErrorExImpl)
{
	bool sendData;
	static TIMEVALUE lastEvent(0.0L);
	static Receivers::ReceiversDataBlock prvData={0,false,Management::MNG_OK};
	TIMEVALUE now;
	baci::ThreadSyncGuard guard(&m_mutex);
	IRA::CIRATools::getTime(now);
	if (CIRATools::timeDifference(lastEvent,now)>=1000000) {  //one second from last event
		sendData=true;
	}
	else if ((prvData.tracking!=m_dewarTracking) || (prvData.status!=m_status)) {
		sendData=true;
	}
	else {
		sendData=false;
	}
	if (sendData) {
		prvData.tracking=m_dewarTracking;
		prvData.timeMark=now.value().value;
		prvData.status=m_status;
		try {
			m_notificationChannel->publishData<Receivers::ReceiversDataBlock>(prvData);
		}
		catch (acsncErrType::PublishEventFailureExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::NotificationChannelErrorExImpl,impl,ex,"CRecvBossCore::publishData()");
			changeBossStatus(Management::MNG_WARNING);
			throw impl;
		}
		IRA::CIRATools::timeCopy(lastEvent,now);
	}
}

//************ PRIVATE ****************************

void CRecvBossCore::setup(const char * code) throw (ComponentErrors::CORBAProblemExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl)
{
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
			ACS_LOG(LM_FULL_INFO,"CRecvBossCore::setup()",(LM_WARNING,"COULD_NOT_DEACTIVATE_CURRENT_RECEIVER"));
		}
	}
	unloadReceiver();
	try {
		derotatorPark();
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		ACS_LOG(LM_FULL_INFO,"CRecvBossCore::setup()",(LM_WARNING,"COULD_NOT_PARK_THE_DEROTATOR"));

		changeBossStatus(Management::MNG_WARNING);
		// anyway go on....
	}
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
	ACS_LOG(LM_FULL_INFO,"CRecvBossCore::setup()",(LM_NOTICE,"NEW_RECEIVER_CONFIGURED %s",code));
}

void CRecvBossCore::loadDewarPositioner()  throw (ComponentErrors::CouldntGetComponentExImpl)
{
	if ((!CORBA::is_nil(m_dewarPositioner)) && (m_dewarPositionerError)) { // if reference was already taken, but an error was found....dispose the reference
		try {
			m_services->releaseComponent((const char*)m_dewarPositioner->name());
		}
		catch (...) { //dispose silently...if an error...no matter
		}
		m_dewarPositioner=Receivers::DewarPositioner::_nil();
		m_dewarPositionerError=false;
	}
	if (CORBA::is_nil(m_dewarPositioner)) {  //only if it has not been retrieved yet
		try {
			m_dewarPositioner=m_services->getDefaultComponent<Receivers::DewarPositioner>((const char*)m_config->dewarPositionerInterface());
			ACS_LOG(LM_FULL_INFO,"CRecvBossCore::loadDewarPostioner",(LM_INFO,"DEWAR_POSITIONER_LOCATED"));
			m_dewarPositionerError=false;
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CRecvBossCore::loadDewarPostioner");
			Impl.setComponentName((const char*)m_config->dewarPositionerInterface());
			m_dewarPositioner=Receivers::DewarPositioner::_nil();
			changeBossStatus(Management::MNG_FAILURE);
			throw Impl;
		}
		catch (maciErrType::NoPermissionExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CRecvBossCore::loadDewarPostioner");
			Impl.setComponentName((const char*)m_config->dewarPositionerInterface());
			m_dewarPositioner=Receivers::DewarPositioner::_nil();
			changeBossStatus(Management::MNG_FAILURE);
			throw Impl;
		}
		catch (maciErrType::NoDefaultComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CRecvBossCore::loadDewarPostioner");
			Impl.setComponentName((const char*)m_config->dewarPositionerInterface());
			m_dewarPositioner=Receivers::DewarPositioner::_nil();
			changeBossStatus(Management::MNG_FAILURE);
			throw Impl;
		}
	}
}

void CRecvBossCore::unloadDewarPositioner()
{
	if (!CORBA::is_nil(m_dewarPositioner)) {
		try {
			m_services->releaseComponent((const char*)m_dewarPositioner->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CRecvBossCore::unloadDewarPositioner()");
			Impl.setComponentName((const char *)m_config->dewarPositionerInterface());
			Impl.log(LM_WARNING);
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CRecvBossCore::unloadDewarPositioner()");
			impl.log(LM_WARNING);
		}
		m_dewarPositioner=Receivers::DewarPositioner::_nil();
		m_dewarPositionerError=false;
	}
}

#endif

