#include "Schedule.h"

using namespace Schedule;

CScanBinder::CScanBinder(CConfiguration* config,bool dispose): m_scanConf(NULL),m_own(dispose),m_config(config)
{
}

CScanBinder::~CScanBinder()
{
	if (m_own) {
		dispose();
	}
}

void CScanBinder::dispose()
{
	if (m_scanConf) delete m_scanConf;
}

void CScanBinder::startScan(DWORD id)
{
	TIMEVALUE now;
	if (m_scanConf) {
		IRA::CIRATools::getTime(now);
		m_scanConf->scanID=id;
		m_scanConf->timeStamp=now.value().value;
		m_scanConf->subScansNumber=0;
	}
}

void CScanBinder::nextScan()
{
	TIMEVALUE now;		
	if (m_scanConf) {
		IRA::CIRATools::getTime(now);
		m_scanConf->scanID++;
		m_scanConf->timeStamp=now.value().value;
		m_scanConf->subScansNumber=0;
	}
}

void CScanBinder::init(Management::TScanConfiguration * const scanConf)
{
	if (scanConf) {
		m_scanConf=scanConf;
		m_scanConf->scanID=0;
		m_scanConf->timeStamp=0;
		m_scanConf->subScansNumber=0;		
	}
}

void CScanBinder::addSubScan(Antenna::TTrackingParameters *prim, Antenna::TTrackingParameters *sec,
		MinorServo::MinorServoScan *servo,Receivers::TReceiversParameters *reciv)
{
	if (m_scanConf) {
		m_scanConf->subScansNumber++;	
	}
}



