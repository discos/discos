// $Id: CollectorThread.cpp,v 1.3 2011-04-18 13:42:37 a.orlati Exp $

#include "CollectorThread.h"
#include <LogFilter.h>
#include <Definitions.h>
#include <ComponentErrors.h>
#include <ManagmentDefinitionsC.h>
#include "CommonTools.h"

#include "MBFitsManager.h"

using namespace IRA;

using MBFitsWriter_private::CCollectorThread;

//_IRA_LOGFILTER_IMPORT;

CCollectorThread::CCollectorThread( const ACE_CString& name_,
                                    CSecureArea<CDataCollection>* dataWrapper_p_,
                                    const ACS::TimeInterval& responseTime_,
                                    const ACS::TimeInterval& sleepTime_) : ACS::Thread(name_, responseTime_, sleepTime_),
                                                                           m_dataWrapper_p(dataWrapper_p_),
                                                                           m_configuration_p(NULL),
                                                                           m_containerServices_p(NULL),
                                                                           m_mbFitsManager_p(NULL)
{
	AUTO_TRACE("CCollectorThread::CCollectorThread()");
	m_meteoDuty=10000000; //one second
	m_meteoTime=0;
	m_trackingDuty=200000; // 0.2 second
	m_trackingTime=0;
	m_meteoData=Weather::GenericWeatherStation::_nil();
	m_scheduler=Management::Scheduler::_nil();
	m_schedulerError=false;
	m_meteoError=false;
	m_schedulerTracking=Management::ROTBoolean::_nil();
}

CCollectorThread::~CCollectorThread()
{
	m_mbFitsManager_p = NULL;

	AUTO_TRACE("CCollectorThread::~CCollectorThread()");
	try {
		CCommonTools::unloadScheduler(m_scheduler, m_containerServices_p);
		CCommonTools::unloadMeteoComponent(m_meteoData, m_containerServices_p);
	} catch( ACSErr::ACSbaseExImpl& exception_ ) {
		exception_.log(LM_WARNING);
	}

	m_scheduler = Management::Scheduler::_nil();
	m_meteoData	= Weather::GenericWeatherStation::_nil();

	m_containerServices_p	= NULL;
	m_configuration_p			= NULL;
}

void CCollectorThread::onStart()
{
	AUTO_TRACE("CCollectorThread::onStart()");
}

void CCollectorThread::onStop()
{
	 AUTO_TRACE("CCollectorThread::onStop()");
}

void CCollectorThread::runLoop() {
	// it does not make sense to run this thread when the data transfer is not set up yet.
	if ( !checkReadyFlag() ) {
		return;
	}

//ACS_LOG(LM_FULL_INFO,"CCollectorThread::runLoop()",(LM_INFO, "ctrl 01"));

	TIMEVALUE now;
	IRA::CIRATools::getTime(now);

	bool processMeteo			= false;
	bool processTracking	= false;

	if ( (now.value().value - m_meteoTime		) > m_meteoDuty			) {
		collectMeteo();
		processMeteo		= true;
	}

	if ( (now.value().value - m_trackingTime) > m_trackingDuty	) {
		collectTracking();
		processTracking	= true;
	}

	if ( processMeteo || processTracking ) {
		processData();
	}
}

bool CCollectorThread::checkReadyFlag() {
	CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();
	return data_p->isReady();
}

void CCollectorThread::setConfiguration( CConfiguration* const configuration_p_ ) {
	m_configuration_p			= configuration_p_;
}

void CCollectorThread::setServices( maci::ContainerServices* const containerServices_p_ ) {
	m_containerServices_p	= containerServices_p_;
}

void CCollectorThread::setMBFitsManager( MBFitsManager* const mbFitsManager_p_ ) {
	m_mbFitsManager_p = mbFitsManager_p_;
}

void CCollectorThread::processData() {
	// it does not make sense to run this thread when the data transfer is not set up yet.
	if ( !checkReadyFlag() ) {
		return;
	}

	if ( !m_mbFitsManager_p ) {
		return;
	}

	CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();

	double					humidity		= 0.0;
	double					temperature	= 0.0;
	double					pressure		= 0.0;
	data_p->getMeteo(humidity, temperature, pressure);

	double mjd = EpochHelper(m_meteoTime).toMJDseconds() / (24 * 60 *60);

	MBFitsManager::Double_v_t	tAmb_p_humid;
	tAmb_p_humid.push_back(temperature);
	tAmb_p_humid.push_back(pressure);
	tAmb_p_humid.push_back(humidity);

	m_mbFitsManager_p->monitor(mjd,
														 string("TAMB_P_HUMID"),
														 tAmb_p_humid);
}

void CCollectorThread::collectMeteo() {
	// it does not make sense to run this thread when the data transfer is not set up yet.
	if ( !checkReadyFlag() ) {
		return;
	}

	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	m_meteoTime	= now.value().value;

	double humidity			= 0.0;
	double temperature	= 0.0;
	double pressure			= 0.0;

	try {
		CCommonTools::getMeteoComponent(m_meteoData, m_containerServices_p, m_configuration_p->getMeteoInstance(), m_meteoError);
	} catch ( ComponentErrors::CouldntGetComponentExImpl& exception_ ) {
//			_IRA_LOGFILTER_LOG_EXCEPTION(ex,LM_ERROR);
		CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();
		data_p->setStatus(Management::MNG_WARNING);
		m_meteoError = true;
	}

	if ( !CORBA::is_nil(m_meteoData) ) {
		try {
			Weather::parameters parameters;
			parameters	= m_meteoData->getData();
			humidity		= parameters.humidity;
			temperature	= parameters.temperature;
			pressure		= parameters.pressure;

			// now take the lock in order to store the retrived data.....
			CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();
			data_p->setMeteo(humidity, temperature, pressure);
		} catch ( CORBA::SystemException& exception_ ) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CCollectorThread::runLoop()");
			impl.setName(exception_._name());
			impl.setMinor(exception_.minor());
//				_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
			m_meteoError = true;
			CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();
			data_p->setStatus(Management::MNG_WARNING);
		} catch ( ... ) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CCollectorThread::runLoop()");
//				_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
			CSecAreaResourceWrapper<CDataCollection> dataAgain_p = m_dataWrapper_p->Get();
			dataAgain_p->setStatus(Management::MNG_WARNING);
		}
	}
}

void CCollectorThread::collectTracking() {
	// it does not make sense to run this thread when the data transfer is not set up yet.
	if ( !checkReadyFlag() ) {
		return;
	}

	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	m_trackingTime = now.value().value;

	Management::TBoolean track;
	ACSErr::Completion_var comp;
	try {
		CCommonTools::getScheduler(m_scheduler, m_containerServices_p, m_configuration_p->getSchedulerComponent(), m_schedulerError);
	} catch ( ComponentErrors::CouldntGetComponentExImpl& exception_ ) {
//			_IRA_LOGFILTER_LOG_EXCEPTION(ex,LM_ERROR);
		CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();
		data_p->setStatus(Management::MNG_WARNING);
		m_schedulerError = true;
	}

	if ( !CORBA::is_nil(m_scheduler) ) {
		if ( CORBA::is_nil(m_schedulerTracking) ) {
			try {
				m_schedulerTracking = m_scheduler->tracking();
			} catch ( CORBA::SystemException& exception_ ) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl, impl, "CCollectorThread::runLoop()");
				impl.setName(exception_._name());
				impl.setMinor(exception_.minor());
//					_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
				m_schedulerTracking	= Management::ROTBoolean::_nil();
				m_schedulerError		= true;
				CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();
				data_p->setStatus(Management::MNG_WARNING);
			}
		} else {
			try {
				track = m_schedulerTracking->get_sync(comp.out()); // throw CORBA::SystemException
				ACSErr::CompletionImpl compImpl(comp);
				if (compImpl.isErrorFree()) {
					CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();
					data_p->setTelescopeTracking(track, m_trackingTime);
				} else {
					_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl, impl, compImpl, "CCollectorThread::runLoop()");
					impl.setAttributeName("tracking");
					impl.setComponentName((const char *)m_configuration_p->getSchedulerComponent());
//						_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
					CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();
					data_p->setStatus(Management::MNG_WARNING);
				}
			} catch ( CORBA::SystemException& exception_ ) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl, impl, "CCollectorThread::runLoop()");
				impl.setName(exception_._name());
				impl.setMinor(exception_.minor());
//					_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
				m_schedulerTracking=Management::ROTBoolean::_nil();
				CSecAreaResourceWrapper<CDataCollection> data_p = m_dataWrapper_p->Get();
				data_p->setStatus(Management::MNG_WARNING);
			}
		}
	}
}
