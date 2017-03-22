void CCore::getScheduleName(IRA::CString& name)
{
	if (m_schedExecuter) name=m_schedExecuter->getScheduleName();
	else name="";
}

void CCore::getProjectCode(IRA::CString& code)
{
	if (m_schedExecuter) code=m_schedExecuter->getProjectCode();
	else code="";
}

void CCore::getScanCounter(DWORD& cc)
{
	if (m_schedExecuter) cc= m_schedExecuter->getCurrentScheduleCounter();
	else cc=0;
}

void CCore::getCurrentIdentifiers(DWORD& scanID,DWORD& subScanID)
{
	if ((m_schedExecuter) && (m_schedExecuter->isScheduleActive())) {
		m_schedExecuter->getCurrentScanIdentifers(scanID,subScanID);
	}
	else {
		scanID=m_scanID;
		subScanID=m_subScanID;
	}
}

void CCore::getCurrentBackend(IRA::CString& bck)
{
	/*Backends::GenericBackend_var backend;
	backend=m_schedExecuter->getBackendReference(); //get the reference to the currently used backend.
	baci::ThreadSyncGuard guard(&m_mutex);
	if (CORBA::is_nil(backend)) {
		bck=m_defaultBackendInstance;
	}
	else {
		bck=backend->name();
	}*/
	bck=m_defaultBackendInstance;
}

void  CCore::getCurrentDataReceiver(IRA::CString& dv)
{
	/*Management::DataReceiver_var dataWriter;
	dataWriter=m_schedExecuter->getWriterReference(); //get the reference to the currently used backend.
	baci::ThreadSyncGuard guard(&m_mutex);
	if (CORBA::is_nil(dataWriter)) {
		dv=m_defaultDataReceiverInstance;
	}
	else {
		dv=dataWriter->name();
	}*/
	dv=m_defaultDataReceiverInstance;
}

bool CCore::isTracking() const
{
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	ACS::TimeInterval diff=now.value().value-m_clearTrackingTime;
	return (m_isAntennaTracking && m_isMinorServoTracking && m_isReceiversTracking && (diff>5000000));
}

bool CCore::isOnSource() const
{
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	ACS::TimeInterval diff=now.value().value-m_clearTrackingTime;
	return (m_isAntennaTracking && (diff>5000000));
}



