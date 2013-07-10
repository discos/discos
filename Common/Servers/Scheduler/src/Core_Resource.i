
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

/*void CCore::loadActiveSurfaceBoss(SRTActiveSurface::SRTActiveSurfaceBoss_var& ref,bool& errorDetected) const throw (ComponentErrors::CouldntGetComponentExImpl)
{
	if ((!CORBA::is_nil(ref)) && (errorDetected)) { // if reference was already taken, but an error was found....dispose the reference
		try {
			m_services->releaseComponent((const char*)ref->name());
		}
		catch (...) { //dispose silently...if an error...no matter
		}
		ref=SRTActiveSurface::SRTActiveSurfaceBoss::_nil();
	}
	if (CORBA::is_nil(ref)) {  //only if it has not been retrieved yet
		try {
			if (m_config->getActiveSurfaceBossComponent()!="") {
				ref=m_services->getDefaultComponent<SRTActiveSurface::SRTActiveSurfaceBoss>((const char*)m_config->getActiveSurfaceBossComponent());
				ACS_LOG(LM_FULL_INFO,"CCore::loadActiveSurfaceBoss()",(LM_INFO,"ACTIVE_SURFACE_BOSS_LOCATED"));
				errorDetected=false;
			}
			else {
				ACS_LOG(LM_FULL_INFO,"CCore::loadActiveSurfaceBoss()",(LM_WARNING,"ACTIVE_SURFACE_UNAVAILABLE"));
			}
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadActiveSurfaceBoss()");
			Impl.setComponentName((const char*)m_config->getActiveSurfaceBossComponent());
			ref=SRTActiveSurface::SRTActiveSurfaceBoss::_nil();
			throw Impl;
		}
		catch (maciErrType::NoPermissionExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadActiveSurfaceBoss()");
			Impl.setComponentName((const char*)m_config->getActiveSurfaceBossComponent());
			ref=SRTActiveSurface::SRTActiveSurfaceBoss::_nil();
			throw Impl;
		}
		catch (maciErrType::NoDefaultComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadActiveSurfaceBoss()");
			Impl.setComponentName((const char*)m_config->getActiveSurfaceBossComponent());
			ref=SRTActiveSurface::SRTActiveSurfaceBoss::_nil();
			throw Impl;
		}
	}
}

void CCore::unloadActiveSurfaceBoss(SRTActiveSurface::SRTActiveSurfaceBoss_var& ref) const
{
	if (!CORBA::is_nil(ref)) {
		try {
			m_services->releaseComponent((const char*)ref->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CCore::unloadActiveSurfaceBoss()");
			Impl.setComponentName((const char *)m_config->getActiveSurfaceBossComponent());
			Impl.log(LM_WARNING);
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::unloadActiveSurfaceBoss())");
			impl.log(LM_WARNING);
		}
		ref=SRTActiveSurface::SRTActiveSurfaceBoss::_nil();
	}
}*/

void CCore::loadMinorServoBoss(MinorServo::MinorServoBoss_var& ref,bool& errorDetected) const throw (ComponentErrors::CouldntGetComponentExImpl)
{
	if ((!CORBA::is_nil(ref)) && (errorDetected)) { // if reference was already taken, but an error was found....dispose the reference
		try {
			m_services->releaseComponent((const char*)ref->name());
		}
		catch (...) { //dispose silently...if an error...no matter
		}
		ref=MinorServo::MinorServoBoss::_nil();
	}
	if (CORBA::is_nil(ref)) {  //only if it has not been retrieved yet
		try {
			if (m_config->getMinorServoBossComponent()!="") {
				ref=m_services->getDefaultComponent<MinorServo::MinorServoBoss>((const char*)m_config->getMinorServoBossComponent());
				ACS_LOG(LM_FULL_INFO,"CCore::loadMinorServoBoss()",(LM_INFO,"MINORSERVOBOSS_LOCATED"));
				errorDetected=false;
			}
			else {
				ACS_LOG(LM_FULL_INFO,"CCore::loadMinorServoBoss()",(LM_WARNING,"MINOR_SERVO_UNAVAILABLE"));
			}
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadMinorServoBoss()");
			Impl.setComponentName((const char*)m_config->getMinorServoBossComponent());
			ref=MinorServo::MinorServoBoss::_nil();
			throw Impl;
		}
		catch (maciErrType::NoPermissionExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadMinorServoBoss()");
			Impl.setComponentName((const char*)m_config->getMinorServoBossComponent());
			ref=MinorServo::MinorServoBoss::_nil();
			throw Impl;
		}
		catch (maciErrType::NoDefaultComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadMinorServoBoss()");
			Impl.setComponentName((const char*)m_config->getMinorServoBossComponent());
			ref=MinorServo::MinorServoBoss::_nil();
			throw Impl;
		}
	}
}

void CCore::unloadMinorServoBoss(MinorServo::MinorServoBoss_var& ref) const
{
	if (!CORBA::is_nil(ref)) {
		try {
			m_services->releaseComponent((const char*)ref->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CCore::unloadMinorServoBoss()");
			Impl.setComponentName((const char *)m_config->getMinorServoBossComponent());
			Impl.log(LM_WARNING);
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::unloadMinorServoBoss())");
			impl.log(LM_WARNING);
		}
		ref=MinorServo::MinorServoBoss::_nil();
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

void CCore::unloadReceiversBoss(Receivers::ReceiversBoss_var& ref) const
{
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

void CCore::loadCustomLogger(Management::CustomLogger_var& ref,bool& errorDetected) const throw (ComponentErrors::CouldntGetComponentExImpl)
{
	if ((!CORBA::is_nil(ref)) && (errorDetected)) { // if reference was already taken, but an error was found....dispose the reference
		try {
			m_services->releaseComponent((const char*)ref->name());
		}
		catch (...) { //dispose silently...if an error...no matter
		}
		ref=Management::CustomLogger::_nil();
	}
	if (CORBA::is_nil(ref)) {  //only if it has not been retrieved yet
		try {
			ref=m_services->getDefaultComponent<Management::CustomLogger>((const char*)m_config->getCustomLoggerComponent());
			errorDetected=false;
			ACS_LOG(LM_FULL_INFO,"CCore::loadCustomLogger()",(LM_INFO,"CUSTOMLOGGER_LOCATED"));
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadCustomLogger()");
			Impl.setComponentName((const char*)m_config->getCustomLoggerComponent());
			throw Impl;
		}
		catch (maciErrType::NoPermissionExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadCustomLogger()");
			Impl.setComponentName((const char*)m_config->getCustomLoggerComponent());
			throw Impl;
		}
		catch (maciErrType::NoDefaultComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadCustomLogger()");
			Impl.setComponentName((const char*)m_config->getCustomLoggerComponent());
			throw Impl;
		}
	}
}

void CCore::unloadCustomLogger(Management::CustomLogger_var& ref) const
{
	if (!CORBA::is_nil(ref)) {
		try {
			m_services->releaseComponent((const char*)ref->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CCore::unloadCustomLogger()");
			Impl.setComponentName((const char *)m_config->getCustomLoggerComponent());
			Impl.log(LM_WARNING);
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::unloadCustomLogger())");
			impl.log(LM_WARNING);
		}
		ref=Management::CustomLogger::_nil();
	}
}

void CCore::loadWeatherStation(Weather::GenericWeatherStation_var& ref,bool& errorDetected) const throw (ComponentErrors::CouldntGetComponentExImpl)
{
	if ((!CORBA::is_nil(ref)) && (errorDetected)) { // if reference was already taken, but an error was found....dispose the reference
		try {
			m_services->releaseComponent((const char*)ref->name());
		}
		catch (...) { //dispose silently...if an error...no matter
		}
		ref=Weather::GenericWeatherStation::_nil();
	}
	if (CORBA::is_nil(ref)) {  //only if it has not been retrieved yet
		try {
			ref=m_services->getDefaultComponent<Weather::GenericWeatherStation>((const char*)m_config->getWeatherStationComponent());
			errorDetected=false;
			ACS_LOG(LM_FULL_INFO,"CCore::loadCustomLogger()",(LM_INFO,"CUSTOMLOGGER_LOCATED"));
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::getWeatherStationComponent()");
			Impl.setComponentName((const char*)m_config->getWeatherStationComponent());
			throw Impl;
		}
		catch (maciErrType::NoPermissionExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::getWeatherStationComponent()");
			Impl.setComponentName((const char*)m_config->getWeatherStationComponent());
			throw Impl;
		}
		catch (maciErrType::NoDefaultComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::getWeatherStationComponent()");
			Impl.setComponentName((const char*)m_config->getWeatherStationComponent());
			throw Impl;
		}
	}
}

void CCore::unloadWeatherStation(Weather::GenericWeatherStation_var& ref) const
{
	if (!CORBA::is_nil(ref)) {
		try {
			m_services->releaseComponent((const char*)ref->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CCore::unloadWeatherStation()");
			Impl.setComponentName((const char *)m_config->getWeatherStationComponent());
			Impl.log(LM_WARNING);
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::unloadWeatherStation())");
			impl.log(LM_WARNING);
		}
		ref=Weather::GenericWeatherStation::_nil();
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


void CCore::loadDefaultDataReceiver() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl)
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
			m_defaultDataReceiver->reset();
			m_defaultDataReceiverError=false;
			ACS_LOG(LM_FULL_INFO,"CCore::loadDefaultDataReceiver()",(LM_INFO,"DATARECEIVER_LOCATED"));
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CCore::loadDefaultDataReceiver()");
			Impl.setComponentName((const char*)m_defaultDataReceiverInstance);
			changeSchedulerStatus(Management::MNG_FAILURE);
			throw Impl;		
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCore::loadDefaultDataReceiver())");
			throw impl;
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

void CCore::loadProcedures(const IRA::CString& proceduresFile) throw (ManagementErrors::ProcedureFileLoadingErrorExImpl)
{
	//baci::ThreadSyncGuard guard(&m_mutex); not required....the parser is thread safe.
	ACS::stringSeq names;
	ACS::longSeq args;
	ACS::stringSeq *commands=NULL;
	try {
		m_config->readProcedures(m_services,proceduresFile,names,args,commands);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		_ADD_BACKTRACE(ManagementErrors::ProcedureFileLoadingErrorExImpl,impl,ex,"CCore::loadProcedures()");
		impl.setFileName((const char *)proceduresFile);
		throw impl;
	}
	for (unsigned i=0;i<names.length();i++) {
		IRA::CString name(names[i]);
		m_parser->add(name,proceduresFile,commands[i],args[i]);
	}
	if (commands) delete[] commands;
	m_currentProceduresFile=proceduresFile;
	ACS_LOG(LM_FULL_INFO,"CCore::loadProcedures()",(LM_NOTICE,"PROCEDURES_LOADED: %s",(const char *)proceduresFile));
}

void CCore::loadProceduresFile(const IRA::CString path,const IRA::CString file) throw (ManagementErrors::ProcedureFileLoadingErrorExImpl)
{
	//baci::ThreadSyncGuard guard(&m_mutex); not required....the parser is thread safe.
	IRA::CString full=path+file;
	Schedule::CProcedureList loader(path,file);
	if (loader.readAll(true)) {
		_EXCPT(ManagementErrors::ProcedureFileLoadingErrorExImpl,impl,"CCore::loadProceduresFile()");
		impl.setFileName((const char *)full);
		impl.setReason((const char *)loader.getLastError());
	}
	m_parser->clearExtraProcedures();
	WORD pos=0;
	WORD args;
	ACS::stringSeq proc;
	IRA::CString name;
	while(loader.getProcedure(pos,name,proc,args)) {
		m_parser->addExtraProcedure(name,full,proc,args);
		pos++;
	}
}

void CCore::loadProceduresFile(Schedule::CProcedureList *loader)
{
	//baci::ThreadSyncGuard guard(&m_mutex); not required....the parser is thread safe.
	m_parser->clearExtraProcedures();
	WORD pos=0;
	WORD args;
	ACS::stringSeq proc;
	IRA::CString name;
	while(loader->getProcedure(pos,name,proc,args)) {
		m_parser->addExtraProcedure(name,loader->getFileName(),proc,args);
		pos++;
	}
}

void CCore::executeProcedure(const IRA::CString& name,_SP_CALLBACK(callBack),const void *param) throw (ManagementErrors::ScheduleProcedureErrorExImpl)
{
	// no need to protect this because the parser is already thread-safe
	try {
		//m_parser->inject(name,proc,callBack,param);
		m_parser->runAsync(name,callBack,param);
	}
	catch (ParserErrors::ParserErrorsExImpl& ex) {
		_ADD_BACKTRACE(ManagementErrors::ScheduleProcedureErrorExImpl,impl,ex,"CCore::executeProcedure()");
		impl.setProcedure((const char *)name);
		throw impl;
	}
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


