// $Id: Core_Resource.i,v 1.9 2011-05-20 16:53:09 a.orlati Exp $


void CCore::loadAntennaBoss(Antenna::AntennaBoss_var& ref,bool& errorDetected) const throw (ComponentErrors::CouldntGetComponentExImpl)
{
	if ((!CORBA::is_nil(ref)) && (errorDetected)) { // if reference was already taken, but an error was found....dispose the reference
		try {
			m_services->releaseComponent((const char*)ref->name());
		}
		catch (...) { //dispose silently...if an error...no matter
		}
		ref=Antenna::AntennaBoss::_nil();
	}
	if (CORBA::is_nil(ref)) {  //only if it has not been retrieved yet
		try {  
			ref=m_services->getDefaultComponent<Antenna::AntennaBoss>((const char*)m_config->getAntennaBossComponent());
			ACS_LOG(LM_FULL_INFO,"CCore::loadAntennaBoss()",(LM_INFO,"ANTENNABOSS_LOCATED"));
			errorDetected=false;
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadAntennaBoss()");
			Impl.setComponentName((const char*)m_config->getAntennaBossComponent());
			ref=Antenna::AntennaBoss::_nil();
			throw Impl;		
		}
		catch (maciErrType::NoPermissionExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadAntennaBoss()");
			Impl.setComponentName((const char*)m_config->getAntennaBossComponent());
			ref=Antenna::AntennaBoss::_nil();
			throw Impl;		
		}
		catch (maciErrType::NoDefaultComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadAntennaBoss()");
			Impl.setComponentName((const char*)m_config->getAntennaBossComponent());
			ref=Antenna::AntennaBoss::_nil();
			throw Impl;				
		}
	}
}

void CCore::unloadAntennaBoss(Antenna::AntennaBoss_var& ref) const
{
	if (!CORBA::is_nil(ref)) {
		try {
			m_services->releaseComponent((const char*)ref->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CCore::unloadAntennaBoss()");
			Impl.setComponentName((const char *)m_config->getAntennaBossComponent());
			Impl.log(LM_WARNING);
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::unloadAntennaBoss())");
			impl.log(LM_WARNING);
		}
		ref=Antenna::AntennaBoss::_nil();
	}
}

void CCore::loadReceiversBoss(Receivers::ReceiversBoss_var& ref,bool& errorDetected) const throw (ComponentErrors::CouldntGetComponentExImpl)
{
	if ((!CORBA::is_nil(ref)) && (errorDetected)) { // if reference was already taken, but an error was found....dispose the reference
		try {
			m_services->releaseComponent((const char*)ref->name());
		}
		catch (...) { //dispose silently...if an error...no matter
		}
		ref=Receivers::ReceiversBoss::_nil();
	}	
	if (CORBA::is_nil(ref)) {  //only if it has not been retrieved yet
		try {  
			ref=m_services->getDefaultComponent<Receivers::ReceiversBoss>((const char*)m_config->getReceiversBossComponent());
			errorDetected=false;
			ACS_LOG(LM_FULL_INFO,"CCore::loadReceiversBoss()",(LM_INFO,"RECEIVERSBOSS_LOCATED"));
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadReceiversBoss()");
			Impl.setComponentName((const char*)m_config->getReceiversBossComponent());
			throw Impl;		
		}
		catch (maciErrType::NoPermissionExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadReceiversBoss()");
			Impl.setComponentName((const char*)m_config->getReceiversBossComponent());
			throw Impl;		
		}
		catch (maciErrType::NoDefaultComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadReceiversBoss()");
			Impl.setComponentName((const char*)m_config->getReceiversBossComponent());
			throw Impl;				
		}
	}
}

void CCore::unloadReceiversBoss(Receivers::ReceiversBoss_var& ref) const {
	if (!CORBA::is_nil(ref)) {
		try {
			m_services->releaseComponent((const char*)ref->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CCore::unloadReceiversBoss()");
			Impl.setComponentName((const char *)m_config->getReceiversBossComponent());
			Impl.log(LM_WARNING);
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::unloadReceiversBoss())");
			impl.log(LM_WARNING);
		}
		ref=Receivers::ReceiversBoss::_nil();
	}
}

void CCore::loadDefaultBackend() throw (ComponentErrors::CouldntGetComponentExImpl)
{
	if ((!CORBA::is_nil(m_defaultBackend)) && (m_defaultBackendError)) { // if reference was already taken, but an error was found....dispose the reference
		try {
			m_services->releaseComponent((const char*)m_defaultBackend->name());
		}
		catch (...) { //dispose silently...if an error...no matter
		}
		m_defaultBackend=Backends::GenericBackend::_nil();
	}		
	if (CORBA::is_nil(m_defaultBackend)) {  //only if it has not been retrieved yet
		try {  	
			m_defaultBackend=m_services->getComponent<Backends::GenericBackend>((const char *)m_defaultBackendInstance);
			m_defaultBackendError=false;
			ACS_LOG(LM_FULL_INFO,"CCore::loadDefaultBackend()",(LM_INFO,"DEFAULTBACKEND_LOCATED"));
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadDefaultBackend()");
			Impl.setComponentName((const char*)m_defaultBackendInstance);
			changeSchedulerStatus(Management::MNG_FAILURE);
			throw Impl;		
		}
	}
}

void CCore::unloadDefaultBackend()
{
	if (!CORBA::is_nil(m_defaultBackend)) {
		try {
			m_services->releaseComponent((const char*)m_defaultBackend->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CCore::unloadDefaultBackend()");
			Impl.setComponentName((const char *)m_defaultBackendInstance);
			Impl.log(LM_WARNING);
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::unloadDefaultBackend())");
			impl.log(LM_WARNING);
		}
		m_defaultBackend=Backends::GenericBackend::_nil();
	}	
}


void CCore::loadDefaultDataReceiver() throw (ComponentErrors::CouldntGetComponentExImpl)
{
	if ((!CORBA::is_nil(m_defaultDataReceiver)) && (m_defaultDataReceiverError)) { // if reference was already taken, but an error was found....dispose the reference
		try {
			m_services->releaseComponent((const char*)m_defaultDataReceiver->name());
		}
		catch (...) { //dispose silently...if an error...no matter
		}
		m_defaultDataReceiver=Management::DataReceiver::_nil();
	}		
	if (CORBA::is_nil(m_defaultDataReceiver)) {  //only if it has not been retrieved yet
		try {  	
			m_defaultDataReceiver=m_services->getComponent<Management::DataReceiver>((const char *)m_defaultDataReceiverInstance);
			m_defaultDataReceiverError=false;
			ACS_LOG(LM_FULL_INFO,"CCore::loadDefaultDataReceiver()",(LM_INFO,"DATARECEIVER_LOCATED"));
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadDefaultDataReceiver()");
			Impl.setComponentName((const char*)m_defaultDataReceiverInstance);
			changeSchedulerStatus(Management::MNG_FAILURE);
			throw Impl;		
		}
	}	
}

void CCore::unloadDefaultDataReceiver()
{
	if (!CORBA::is_nil(m_defaultDataReceiver)) {
		try {
			m_services->releaseComponent((const char*)m_defaultDataReceiver->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CCore::unloadDefaultDataReceiver()");
			Impl.setComponentName((const char *)m_defaultDataReceiverInstance);
			Impl.log(LM_WARNING);
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::unloadDefaultDataReceiver())");
			impl.log(LM_WARNING);
		}
		m_defaultDataReceiver=Management::DataReceiver::_nil();
	}	
}


void CCore::injectProcedure(const IRA::CString& name,const ACS::stringSeq& proc,_SP_CALLBACK(callBack),const void *param) throw (ManagementErrors::ProcedureErrorExImpl)
{
	// no need to protect this beacuse the parser is already thread-safe
	try {
		m_parser->inject(name,proc,callBack,param);
	}
	catch (ParserErrors::ProcedureErrorExImpl& ex) {
		_ADD_BACKTRACE(ManagementErrors::ProcedureErrorExImpl,impl,ex,"CCore::injectProcedure()");
		impl.setCommand(ex.getCommand());
		impl.setProcedure((const char *)name);
		throw impl;
		//ex.log(LM_WARNING);
		//return false;
	}
	//return true;
}


bool CCore::addTimerEvent(const ACS::Time& time,IRA::CScheduleTimer::TCallBack handler,void *parameter)
{
	baci::ThreadSyncGuard guard(&m_timerMutex); // user specific mutex for timer resource...this is shared between Core and Executor
	if (!m_timer.schedule(handler,time,0,parameter)) {
		return false;
	}
	return true;
}

void CCore::cancelTimerEvent(ACS::Time& scheduledTime)
{
	baci::ThreadSyncGuard guard(&m_timerMutex); // user specific mutex for timer resource...this is shared between Core and Executor
	if (scheduledTime!=0) {
		m_timer.cancel(scheduledTime);
		scheduledTime=0;
	}
}

void CCore::cancelAllTimerEvents()
{
	baci::ThreadSyncGuard guard(&m_timerMutex); // user specific mutex for timer resource...this is shared between Core and Executor
	m_timer.cancelAll();
}


