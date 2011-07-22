// $Id: Core_Extra.i,v 1.4 2010-10-14 12:15:09 a.orlati Exp $

void CCore::wait(const double& seconds) const
{
	int sec;
	long micro;
	sec=(int)seconds;
	micro=(long)((seconds-(double)sec)*1000000.0);
	IRA::CIRATools::Wait(sec,micro);
}

void CCore::nop() const
{
	return;
}

void CCore::waitOnSource() const
{
	while (!isTracking()) {
		wait(0.1);
	}
}

int CCore::status() const
{
	if (isTracking()) {
		return 0;
	}
	else {
		return 1;
	}
}
