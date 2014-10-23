
int CCore::status() const
{
	if (isTracking()) {
		return 0;
	}
	else {
		return 1;
	}
}

bool CCore::isStreamStarted() const
{
	return m_streamStarted;
}

void CCore::clearTracking()
{
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	m_clearTrackingTime=now.value().value;
}

void CCore::changeSchedulerStatus(const Management::TSystemStatus& status)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (status>=m_schedulerStatus) m_schedulerStatus=status;
}

