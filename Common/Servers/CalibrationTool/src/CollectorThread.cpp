#include "CollectorThread.h"
#include <LogFilter.h>
#include <Definitions.h>
#include <ComponentErrors.h>
#include <ManagmentDefinitionsC.h>
#include "CommonTools.h"

using namespace IRA;
using namespace CalibrationTool_private;

/*
 * At the moment the thread has been disabled as the collection on tracking information from the scheduler is not used...see class Impl, the thread is no resumed
 */

_IRA_LOGFILTER_IMPORT;

CCollectorThread::CCollectorThread(const ACE_CString& name,CDataCollection *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : 
				ACS::Thread(name,responseTime,sleepTime),m_data(param)
{
	AUTO_TRACE("CCollectorThread::CCollectorThread()");
	m_trackingDuty=200000; // 0.2 second
	m_trackingTime=0; 
	m_scheduler=Management::Scheduler::_nil();
	m_schedulerError=false;
	m_schedulerTracking=Management::ROTBoolean::_nil();
}

CCollectorThread::~CCollectorThread()
{
	AUTO_TRACE("CCollectorThread::~CCollectorThread()");
	try {
		CCommonTools::unloadScheduler(m_scheduler,m_services);
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		ex.log(LM_WARNING);
	}
}

void CCollectorThread::onStart()
{
	AUTO_TRACE("CCollectorThread::onStart()");
}

void CCollectorThread::onStop()
{
	 AUTO_TRACE("CCollectorThread::onStop()");
}

void CCollectorThread::runLoop()
{
	if (!checkReadyFlag()) {  //it does not make sense to run this thread when the data transfer is not set up yet.
		return;
	}
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);

	if ((now.value().value-m_trackingTime)>m_trackingDuty) {
		Management::TBoolean track;
		ACSErr::Completion_var comp;
		m_trackingTime=now.value().value;
		try {
			CCommonTools::getScheduler(m_scheduler,m_services,m_config->getSchedulerComponent(),m_schedulerError);
		}
		catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
			/* TO BE REVIEWED */
			/*_IRA_LOGFILTER_LOG_EXCEPTION(ex,LM_ERROR);
			m_data->setStatus(Management::MNG_WARNING);		
			m_schedulerError=true;*/
		}
		if (!CORBA::is_nil(m_scheduler)) {
			if (CORBA::is_nil(m_schedulerTracking)) {
				try {
					m_schedulerTracking=m_scheduler->tracking();
				}
				catch (CORBA::SystemException& ex) {
					/* TO BE REVIEWED */
					/*_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCollectorThread::runLoop()");
					impl.setName(ex._name());
					impl.setMinor(ex.minor());
					_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
					m_schedulerTracking=Management::ROTBoolean::_nil();
					m_schedulerError=true;
					m_data->setStatus(Management::MNG_WARNING);*/
				}
			}
			else {
				try {
					track=m_schedulerTracking->get_sync(comp.out()); // throw CORBA::SystemException
					ACSErr::CompletionImpl compImpl(comp);
					if (compImpl.isErrorFree()) {
						/*m_data->setTelescopeTracking(track,m_trackingTime);*/
					}
					else {
						/* TO BE REVIEWED */
						/*_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,compImpl,"CCollectorThread::runLoop()");
						impl.setAttributeName("tracking");
						impl.setComponentName((const char *)m_config->getSchedulerComponent());
						_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
						m_data->setStatus(Management::MNG_WARNING);*/
					}					
				}
				catch (CORBA::SystemException& ex) {
					/* TO BE REVIEWED */
					/*_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCollectorThread::runLoop()");
					impl.setName(ex._name());
					impl.setMinor(ex.minor());
					_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
					m_schedulerTracking=Management::ROTBoolean::_nil();
					m_data->setStatus(Management::MNG_WARNING);*/
				}
			}
		}
	}
}

bool CCollectorThread::checkReadyFlag()
{
	//CSecAreaResourceWrapper<CDataCollection> data=m_dataWrapper->Get();
	return m_data->isReady(); 
}
