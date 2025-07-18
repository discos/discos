#include "MonitorThread.h"
#include <LogFilter.h>

_IRA_LOGFILTER_IMPORT;

CMonitorThread::CMonitorThread(const ACE_CString& name, CComponentCore *param, const ACS::TimeInterval& responseTime, const ACS::TimeInterval& sleepTime) :
    ACS::Thread(name, responseTime, 0),
    m_core(param),
    m_currentResponseTime(responseTime),
    m_sleepTime(sleepTime)
{
    AUTO_TRACE("CMonitorThread::CMonitorThread()");
}

CMonitorThread::~CMonitorThread()
{
    AUTO_TRACE("CMonitorThread::~CMonitorThread()");
}

void CMonitorThread::onStart()
{
    AUTO_TRACE("CMonitorThread::onStart()");
    m_currentStage = VACUUM;
    m_nextTime = getTimeStamp();
}

void CMonitorThread::onStop()
{
    AUTO_TRACE("CMonitorThread::onStop()");
}

void CMonitorThread::setLNASamplingTime(const DDWORD& time)
{
    m_currentResponseTime = getResponseTime();
    m_currentSampling = time * 10; // uSec to 100 nanoSec
    if (m_currentResponseTime < m_currentSampling + m_currentSampling / 10) {
        // force the response time to be at least 10% more than sampling time
        m_currentResponseTime = m_currentSampling + m_currentSampling / 10;
        ACS_LOG(
            LM_FULL_INFO,
            "SRT7GHzImpl::execute()",
            (
                LM_WARNING,
                "WATCH_DOG_RESPONSE_TIME_ADJUSTED_TO_FIT_SAMPLING_TIME: %llu uSec",
                static_cast<long long unsigned int>(m_currentResponseTime) / 10
            )
        );
        setResponseTime(m_currentResponseTime);
    }
}

void CMonitorThread::runLoop()
{
    switch (m_currentStage) {
        case VACUUM: {
            // Avoid sleeping in between stages, we only sleep m_sleepTime after properties publication
            setSleepTime(0);
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
            m_currentStage = LNA;
            break;
        }
        case LNA: {
            m_currentStage = CTEMPCOOLHEAD;
            try {
                m_core->updateLNAControls();
            }
            catch (ACSErr::ACSbaseExImpl& ex) {
                _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl, impl, ex, "CMonitorThread::runLoop");
                _IRA_LOGFILTER_LOG_EXCEPTION(impl, LM_ERROR);
            }
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
            m_currentStage = ENVTEMP;
            try {
                m_core->updateCryoLNAWin();
            }
            catch (ACSErr::ACSbaseExImpl& ex) {
                _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl, impl, ex, "CMonitorThread::runLoop");
                _IRA_LOGFILTER_LOG_EXCEPTION(impl, LM_ERROR);
            }
            break;
        }
        case ENVTEMP: {
            m_currentStage = REMOTE;
            try {
                m_core->updateEnvironmentTemperature();
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
            m_currentStage = UNLOCKED;
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
            m_currentStage = COMPONENT;
            try {
                m_core->checkLocalOscillator();
            }
            catch (ACSErr::ACSbaseExImpl& ex) {
                _ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl, impl, ex, "CMonitorThread::runLoop");
                _IRA_LOGFILTER_LOG_EXCEPTION(impl, LM_ERROR);
            }
            break;
        }
        case COMPONENT: {
            m_currentStage = PUBLISH;
            m_core->updateComponent();
            break;
        }
        case PUBLISH: {
            m_currentStage = VACUUM;
            m_core->publishZMQData();
            m_nextTime += m_sleepTime;
            // Now sleep for m_sleepTime
            setSleepTime(ACS::TimeInterval(std::max(long(0), long(m_nextTime - getTimeStamp()))));
            break;
        }
    }
}
