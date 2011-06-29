// $Id $

#include "MedicinaMountImpl.h"

using namespace IRA;

CMedicinaMountControlThread::CMedicinaMountControlThread(const ACE_CString& name,CSecureArea<CMedicinaMountSocket> *ACUControl,
  const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime): ACS::Thread(name,responseTime,sleepTime)
{
	AUTO_TRACE("CMedicinaMountControlThread::CMedicinaMountControlThread()");
    m_pACUControl=ACUControl;
	TLongJobData *tmp;
	tmp=new TLongJobData;
	tmp->jobID=0;
	tmp->result=NULL;
	m_pData=new CSecureArea<TLongJobData>(tmp);	
}

CMedicinaMountControlThread::~CMedicinaMountControlThread()
{ 
	AUTO_TRACE("CMedicinaMountControlThread::~CMedicinaMountControlThread()");
	if (m_pData!=NULL) delete m_pData;
}

void CMedicinaMountControlThread::onStart()
{
	AUTO_TRACE("CMedicinaMountControlThread::onStart()");
}
	
void CMedicinaMountControlThread::onStop()
{
	AUTO_TRACE("CMedicinaMountControlThread::onStop()");
}

CMedicinaMountControlThread::TLongJobResult* CMedicinaMountControlThread::setLongJob(WORD job)
{
	AUTO_TRACE("CMedicinaMountControlThread::setLongJob()");
	CSecAreaResourceWrapper<TLongJobData> newjob=m_pData->Get();
	newjob->jobID=job;
	newjob->result=new TLongJobResult;
	newjob->result->done=false;
	return newjob->result;
}

void CMedicinaMountControlThread::runLoop()
{
	WORD currentJobID;
	CSecAreaResourceWrapper<TLongJobData> currentJob=m_pData->Get();
	currentJobID=currentJob->jobID;
	currentJob.Release(); // this is important in order to avoid possible deadlock.
	CSecAreaResourceWrapper<CMedicinaMountSocket> socket=m_pACUControl->Get();
	socket->updateComponent();
	socket->detectOscillation();
	if (currentJobID!=0) {
		ACSErr::Completion_var comp;
		if (socket->updateLongJobs(currentJobID,comp.out())) {
			CSecAreaResourceWrapper<TLongJobData>finalJob=m_pData->Get();
			finalJob->result->comp=comp;
			finalJob->result->done=true;
			// no more jobs monitored
			finalJob->jobID=0;
			//sets the antenna status as ready!
			//m_pACUControl->changeAntennaStatus(Antenna::ACU_CNTD);
			socket->setStatus(Antenna::ACU_CNTD);
		}
	}
}
