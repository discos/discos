#include "ScanThread.hpp"

ScanThreadParameters::ScanThreadParameters(MedMinorServoControl_sp control,
                                             MedMinorServoStatus* status,
                                             MedMinorServoScan& scan) : 
                                             m_control(control),
                                             m_status(status),
                                             m_scan(scan)
{}

ScanThreadParameters::~ScanThreadParameters(){}

ScanThread::ScanThread(const ACE_CString& name, 
                       ScanThreadParameters& params
                      ) : ACS::Thread(name),
                         m_control(params.m_control),
                         m_status(params.m_status),
                         m_scan(params.m_scan)
{}

ScanThread::~ScanThread(){}

void
ScanThread::onStart(){
    AUTO_TRACE("ScanThread::onStart()");
}

void
ScanThread::onStop(){
    AUTO_TRACE("ScanThread::onStop()");
}

void 
ScanThread::run()
{
    TIMEVALUE now(0.0L);
    IRA::CIRATools::getTime(now);
    ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
    // COMMAND STARTING POSITION
    m_control->set_position(m_scan.getStartPosition());
    CUSTOM_LOG(LM_FULL_INFO, "ScanThread::run",
               (LM_DEBUG, "Moving to start position"));
    /**
     * Wait for tracking starting position
     */
    while(!(m_control->is_tracking())){
        sleep_ret = SLEEP_ERROR;
        sleep_ret = ACS::ThreadBase::sleep(10000);
        // Wait 1 ms
        if(sleep_ret != SLEEP_OK)
        {
            return;
        }
     }
    CUSTOM_LOG(LM_FULL_INFO, "ScanThread::run",
               (LM_DEBUG, "Reached start position"));
    CUSTOM_LOG(LM_FULL_INFO, "ScanThread::run",
               (LM_DEBUG, "Waiting for start time"));
    /**
     * Wait for star scan
     */
    IRA::CIRATools::getTime(now);
    while(now.value().value < m_scan.getStartingTime() - SCAN_DELTA) {
        sleep_ret = SLEEP_ERROR;
        sleep_ret = ACS::ThreadBase::sleep(10000);
        // Wait 1 ms
        if(sleep_ret != SLEEP_OK)
        {
            return;
        }
        IRA::CIRATools::getTime(now);
    }
    m_status->scanning = true;
    m_control->set_position_with_time(m_scan.getStopPosition(), 
                                      static_cast<double>(m_scan.getTotalTime() / 10000000));
    CUSTOM_LOG(LM_FULL_INFO, "ScanThread::run",
               (LM_DEBUG, "Begin scan movement"));
    /**
     * Wait for tracking stop position
     */
    while(!(m_control->is_tracking())){
        sleep_ret = SLEEP_ERROR;
        sleep_ret = ACS::ThreadBase::sleep(10000);
        // Wait 1 ms
        if(sleep_ret != SLEEP_OK)
        {
            return;
        }
        if(!(m_status->scanning))
        {
             CUSTOM_LOG(LM_FULL_INFO, "ScanThread::run",
               (LM_DEBUG, "Scan interrupted during execution"));
             return;
        }
     }
     //Wait other 50ms for tracking tollerance
    sleep_ret = SLEEP_ERROR;
    sleep_ret = ACS::ThreadBase::sleep(500000);
    if(sleep_ret != SLEEP_OK)
    {
        return;
    }
    m_status->scanning = false;
    CUSTOM_LOG(LM_FULL_INFO, "ScanThread::run",
               (LM_DEBUG, "Scan complete"));
}

