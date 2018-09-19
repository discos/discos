#include "MonitorThread.h"
#include <LogFilter.h>

_IRA_LOGFILTER_IMPORT;

CMonitorThread::CMonitorThread(
        const ACE_CString& name,
        SRTLPBandCore *param,
        const ACS::TimeInterval& responseTime,
        const ACS::TimeInterval& sleepTime
        ) : ACS::Thread(name, responseTime, sleepTime), m_core(param)
{
    AUTO_TRACE("CMonitorThread::CMonitorThread()");
    m_core = param;
}

CMonitorThread::~CMonitorThread()
{
    AUTO_TRACE("CMonitorThread::~CMonitorThread()");
}

void CMonitorThread::onStart()
{
    AUTO_TRACE("CMonitorThread::onStart()");
    m_currentStage = VACUUM;
}

 void CMonitorThread::onStop()
{
     AUTO_TRACE("CMonitorThread::onStop()");
}

 void CMonitorThread::setLNASamplingTime(const DDWORD& time)
{
     m_currentResponseTime = getResponseTime();
     m_currentSampling = time * 10; // uSec to 100 nanoSec
     if(m_currentResponseTime < m_currentSampling+m_currentSampling/10) {
         // Force the response time to be at least 10% more than sampling time
         m_currentResponseTime = m_currentSampling + m_currentSampling / 10; 
         ACS_LOG(
             LM_FULL_INFO,
             "SRTLPBandReceiverImpl::execute()",
             (
                  LM_WARNING, 
                  "WATCH_DOG_RESPONSE_TIME_ADJUSTED_TO_FIT_SAMPLING_TIME: %llu uSec",
                  static_cast<long long unsigned>(m_currentResponseTime/10)
             )
         );
         setResponseTime(m_currentResponseTime);
     }
}

 void CMonitorThread::runLoop()
{
     m_core->updateComponent();
     switch( m_currentStage) {
         case VACUUM: {
             m_currentStage = STATUS;
             try {
                 m_core->updateVacuum();
             }
             catch (ACSErr::ACSbaseExImpl& ex) {
                 _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl, impl, ex, "CMonitorThread::runLoop");
                 _IRA_LOGFILTER_LOG_EXCEPTION(impl, LM_ERROR);
             }
             break;
         }
        case STATUS: {
            m_currentStage = LNA_VD;
             break;
        }
        case LNA_VD: {
            try {
                 m_core->updateVdLNAControls();
            }
            catch (ACSErr::ACSbaseExImpl& ex) {
                 _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl, impl, ex, "CMonitorThread::runLoop");
                 _IRA_LOGFILTER_LOG_EXCEPTION(impl, LM_ERROR);
            }
            m_currentStage = LNA_ID;
            break;
         }
         case LNA_ID: {
            try {
                 m_core->updateIdLNAControls();
            }
            catch (ACSErr::ACSbaseExImpl& ex) {
                _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl, impl, ex, "CMonitorThread::runLoop");
                _IRA_LOGFILTER_LOG_EXCEPTION(impl, LM_ERROR);
            }
            m_currentStage = LNA_VG;
            break;
         }
         case LNA_VG: {
            try {
                 m_core->updateVgLNAControls();
            }
            catch (ACSErr::ACSbaseExImpl& ex) {
                _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl, impl, ex, "CMonitorThread::runLoop");
                _IRA_LOGFILTER_LOG_EXCEPTION(impl, LM_ERROR);
            }
            m_currentStage = CTEMPCOOLHEAD;
            break;
         }

        case CTEMPCOOLHEAD: {
            m_currentStage = CTEMPCOOLHEADW;
            try {
                m_core->updateCryoCoolHead();
            }
            catch (ACSErr::ACSbaseExImpl& ex) {
                _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl, impl, ex, "CMonitorThread::runLoop");
                _IRA_LOGFILTER_LOG_EXCEPTION(impl, LM_ERROR);
            }
            break;
         }
        case CTEMPCOOLHEADW: {
            m_currentStage = CTEMPLNA;
            try {
                m_core->updateCryoCoolHeadWin();
            }
            catch (ACSErr::ACSbaseExImpl& ex) {
                _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl, impl, ex, "CMonitorThread::runLoop");
                _IRA_LOGFILTER_LOG_EXCEPTION(impl, LM_ERROR);
            }
            break;
         }
        case CTEMPLNA: {
            m_currentStage = CTEMPLNAW;
            try {
                m_core->updateCryoLNA();
            }
            catch (ACSErr::ACSbaseExImpl& ex) {
               _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl, impl, ex, "CMonitorThread::runLoop");
               _IRA_LOGFILTER_LOG_EXCEPTION(impl, LM_ERROR);
            }
            break;
         }
        case CTEMPLNAW: {
            m_currentStage = REMOTE;
            try {
                m_core->updateCryoLNAWin();
            }
            catch (ACSErr::ACSbaseExImpl& ex) {
                _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl, impl, ex, "CMonitorThread::runLoop");
                _IRA_LOGFILTER_LOG_EXCEPTION(impl, LM_ERROR);
            }
            break;
        }
        case REMOTE: {
            m_currentStage = COOLHEAD;
            try {
                m_core->updateIsRemote();
            }
            catch (ACSErr::ACSbaseExImpl& ex) {
                _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl, impl, ex, "CMonitorThread::runLoop");
                _IRA_LOGFILTER_LOG_EXCEPTION(impl, LM_ERROR);
            }
            break;
        }
        case COOLHEAD: {
            m_currentStage = VACUUMPUMP;
            try {
                m_core->updateCoolHead();
            }
            catch (ACSErr::ACSbaseExImpl& ex) {
                _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl, impl, ex, "CMonitorThread::runLoop");
                _IRA_LOGFILTER_LOG_EXCEPTION(impl, LM_ERROR);
            }
            break;
        }
        case VACUUMPUMP: {
            m_currentStage = VACUUMVALVE;
            try {
                m_core->updateVacuumPump();
            }
            catch (ACSErr::ACSbaseExImpl& ex) {
                _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl, impl, ex, "CMonitorThread::runLoop");
                _IRA_LOGFILTER_LOG_EXCEPTION(impl, LM_ERROR);
            }
            break;
        }
        case VACUUMVALVE: {
            m_currentStage = NOISEMARK;
            try {
                m_core->updateVacuumValve();
            }
            catch (ACSErr::ACSbaseExImpl& ex) {
                _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl, impl, ex, "CMonitorThread::runLoop");
                _IRA_LOGFILTER_LOG_EXCEPTION(impl, LM_ERROR);
            }
            break;
        }
        case NOISEMARK: {
            m_currentStage = ENVTEMP;
            try {
                m_core->updateNoiseMark();
            }
            catch (ACSErr::ACSbaseExImpl& ex) {
                _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl, impl, ex, "CMonitorThread::runLoop");
                _IRA_LOGFILTER_LOG_EXCEPTION(impl, LM_ERROR);
            }
            break;
        }
        case UNLOCKED: {
            m_currentStage = ENVTEMP;
            break;
        }

        case ENVTEMP: {
            m_currentStage = VACUUM;
            try {
                m_core->updateVertexTemperature();
            }
            catch (ACSErr::ACSbaseExImpl& ex) {
                _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl, impl, ex, "CMonitorThread::runLoop");
                _IRA_LOGFILTER_LOG_EXCEPTION(impl, LM_ERROR);
            }
            break;
         }
     }
}
