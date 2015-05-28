void CCore::receiversNCHandler(Receivers::ReceiversDataBlock receiversData,void *handlerParam)
{
	CCore *me=static_cast<CCore *>(handlerParam);
	if (receiversData.status!=Management::MNG_OK) {
		me->m_isReceiversTracking=false;
	}
	else {
		me->m_isReceiversTracking=receiversData.tracking;
	}
}

void CCore::antennaNCHandler(Antenna::AntennaDataBlock antennaData,void *handlerParam)
{
	CCore *me=static_cast<CCore *>(handlerParam);
	if (antennaData.status!=Management::MNG_OK) {
		me->m_isAntennaTracking=false;
	}
	else {
		me->m_isAntennaTracking=antennaData.tracking;
	}
}

void CCore::minorServoNCHandler(MinorServo::MinorServoDataBlock servoData,void *handlerParam)
{
	CCore *me=static_cast<CCore *>(handlerParam);
	if (servoData.status!=Management::MNG_OK) {
		me->m_isMinorServoTracking=false;
	}
	else {
		me->m_isMinorServoTracking=servoData.tracking;
	}
}

bool CCore::remoteCall(const IRA::CString& command,const IRA::CString& package,const long& par,IRA::CString& out) throw (ParserErrors::PackageErrorExImpl,ManagementErrors::UnsupportedOperationExImpl)
{
	char * ret_val;
	CORBA::Boolean res;
	switch (par) {
		case 1: { //antenna package
			try {
				baci::ThreadSyncGuard guard(&m_mutex);  //loadAntennaBoss works on class data so it is safe to sync
				loadAntennaBoss(m_antennaBoss,m_antennaBossError);
			}
			catch (ComponentErrors::CouldntGetComponentExImpl& err) { //this exception can be thrown by the loadAntennaBoss()
				_ADD_BACKTRACE(ParserErrors::PackageErrorExImpl,impl,err,"CCore::remoteCall()");
				impl.setPackageName((const char *)package);
				throw impl;
			}
			try {
				res=m_antennaBoss->command((const char *)command,ret_val); // throw CORBA::SystemException
				out=IRA::CString(ret_val);
				CORBA::string_free(ret_val);
				return res;
			}
			catch (CORBA::SystemException& err) {
				_EXCPT(ParserErrors::PackageErrorExImpl,impl,"CCore::command()");
				impl.setPackageName((const char *)package);
				m_antennaBossError=true;
				throw impl;
			}
			break;
		}
		case 2: { //receivers package
			try {
				baci::ThreadSyncGuard guard(&m_mutex);  //loadReceiversBoss works on class data so it is safe to sync
				loadReceiversBoss(m_receiversBoss,m_receiversBossError);
			}
			catch (ComponentErrors::CouldntGetComponentExImpl& err) { //this exception can be thrown by the loadReceiversBoss()
				_ADD_BACKTRACE(ParserErrors::PackageErrorExImpl,impl,err,"CCore::remoteCall()");
				impl.setPackageName((const char *)package);
				throw impl;
			}
			try {
				res=m_receiversBoss->command((const char *)command,ret_val); // throw CORBA::SystemException
				out=IRA::CString(ret_val);
				CORBA::string_free(ret_val);
				return res;
			}
			catch (CORBA::SystemException& err) {
				_EXCPT(ParserErrors::PackageErrorExImpl,impl,"CCore::command()");
				impl.setPackageName((const char *)package);
				m_receiversBossError=true;
				throw impl;
			}
			break;
		}
		case 3: { // default backend..
			Backends::GenericBackend_var backend;
			//backend=m_schedExecuter->getBackendReference(); //get the reference to the currently used backend.
			try {
				/*if (CORBA::is_nil(backend)) {*/
				baci::ThreadSyncGuard guard(&m_mutex);
				loadDefaultBackend(); // throw ComponentErrors::CouldntGetComponentExImpl& err)
				backend=m_defaultBackend;
				/*}*/
			}
			catch (ComponentErrors::CouldntGetComponentExImpl& err) { //this exception can be thrown by the loadDefaultBackend()
				_ADD_BACKTRACE(ParserErrors::PackageErrorExImpl,impl,err,"CCore::remoteCall()");
				impl.setPackageName((const char *)package);
				throw impl;
			}
			try {
				res=backend->command((const char *)command,ret_val); // throw CORBA::SystemException
				out=IRA::CString(ret_val);
				CORBA::string_free(ret_val);
				return res;
			}
			catch (CORBA::SystemException& err) {
				_EXCPT(ParserErrors::PackageErrorExImpl,impl,"CCore::command()");
				impl.setPackageName((const char *)package);
				m_defaultBackendError=true;
				throw impl;
			}
			break;
		}
		case 4: { //minor servo package
			try {
				baci::ThreadSyncGuard guard(&m_mutex);
				loadMinorServoBoss(m_minorServoBoss,m_minorServoBossError);
				if (CORBA::is_nil(m_minorServoBoss)) {
					_EXCPT(ManagementErrors::UnsupportedOperationExImpl,impl,"CCore::remoteCall()");
					throw impl;
				}
			}
			catch (ComponentErrors::CouldntGetComponentExImpl& err) {
				_ADD_BACKTRACE(ParserErrors::PackageErrorExImpl,impl,err,"CCore::remoteCall()");
				impl.setPackageName((const char *)package);
				throw impl;
			}
			try {
				res=m_minorServoBoss->command((const char *)command,ret_val); // throw CORBA::SystemException
				out=IRA::CString(ret_val);
				CORBA::string_free(ret_val);
				return res;
			}
			catch (CORBA::SystemException& err) {
				_EXCPT(ParserErrors::PackageErrorExImpl,impl,"CCore::command()");
				impl.setPackageName((const char *)package);
				m_minorServoBossError=true;
				throw impl;
			}
			break;
		}
		case 5: { //active surface package
				try {
					baci::ThreadSyncGuard guard(&m_mutex);
					loadActiveSurfaceBoss(m_activeSurfaceBoss,m_activeSurfaceBossError);
					if (CORBA::is_nil(m_activeSurfaceBoss)) {
						_EXCPT(ManagementErrors::UnsupportedOperationExImpl,impl,"CCore::remoteCall()");
						throw impl;
					}
				}
				catch (ComponentErrors::CouldntGetComponentExImpl& err) {
					_ADD_BACKTRACE(ParserErrors::PackageErrorExImpl,impl,err,"CCore::remoteCall()");
					impl.setPackageName((const char *)package);
					throw impl;
				}
				try {
					res=m_activeSurfaceBoss->command((const char *)command,ret_val); // throw CORBA::SystemException
					out=IRA::CString(ret_val);
					CORBA::string_free(ret_val);
					return res;
				}
				catch (CORBA::SystemException& err) {
					_EXCPT(ParserErrors::PackageErrorExImpl,impl,"CCore::command()");
					impl.setPackageName((const char *)package);
					m_activeSurfaceBossError=true;
					throw impl;
				}
				break;
			}
		default: {
			_EXCPT(ParserErrors::PackageErrorExImpl,impl,"CCore::remoteCall()");
			impl.setPackageName((const char *)package);
			throw impl;
		}
	}
}

void CCore::waitUntilHandler(const ACS::Time& time,const void *par)
{
	long *done;
	done=static_cast<long *>(const_cast<void *>(par));
	*done=1;
}


void CCore::waitUntilHandlerCleanup(const void *par)
{
	long *done=(long *)(par);
	if (done!=NULL) {
		delete done;
	}
}

