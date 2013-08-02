/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#include <ManagmentDefinitionsS.h>
#include <ComponentErrors.h>
#include "ScanThread.h"
#include <pthread.h>
#include <time.h>
#include "MinorServoBossImpl.h"
#include <MSBossConfiguration.h> // TODO: remove

ScanThread::ScanThread(
        const ACE_CString& name, 
        MSBossConfiguration * configuration,
        const ACS::TimeInterval& responseTime,
        const ACS::TimeInterval& sleepTime,
        const bool del
        ) : ACS::Thread(name, responseTime, sleepTime, del), m_configuration(configuration)
{
    AUTO_TRACE("ScanThread::ScanThread()");
}

ScanThread::~ScanThread() { AUTO_TRACE("ScanThread::~ScanThread()"); }

void ScanThread::onStart() { AUTO_TRACE("ScanThread::onStart()"); }

void ScanThread::onStop() {AUTO_TRACE("ScanThread::onStop()");}

void ScanThread::run()
{
    ACS_SHORT_LOG((LM_ERROR, "ScanThread::run()"));
    TIMEVALUE now(0.0L);
    TIMEVALUE final_time((m_configuration->m_scanning).starting_time + (m_configuration->m_scanning).total_time);
    while(true) {
        IRA::CIRATools::getTime(now);
        if(CIRATools::timeSubtract(now, final_time) > 30000000) // delta of 3 seconds
            break;

        ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
        sleep_ret = ACS::ThreadBase::sleep(SCAN_SLEEP_TIME); // 1 second
        if(sleep_ret != SLEEP_OK) 
            break;
    }

    m_configuration->m_isScanning = false;
    ACS_SHORT_LOG((LM_INFO, ("ScanThread::run(): scanning done.")));
    // A new setup?
    return;
}

