/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#include <ManagmentDefinitionsS.h>
#include <ComponentErrors.h>
#include "StatusUpdater.h"
#include <pthread.h>
#include <time.h>
#include <bitset>

const size_t MAX_HISTORY_SIZE = 32768; // 2 ** 15

StatusUpdater::StatusUpdater(
        const ACE_CString& name, 
        ThreadParameters& params,
        const ACS::TimeInterval& responseTime,
        const ACS::TimeInterval& sleepTime
        ) : ACS::Thread(name, responseTime, sleepTime), m_params(&params)
{
    AUTO_TRACE("StatusUpdater::StatusUpdater()");
    m_isError = false;
    m_isErrorPos = false;
}

StatusUpdater::~StatusUpdater() { 
    AUTO_TRACE("StatusUpdater::~StatusUpdater()"); 
}

void StatusUpdater::onStart() { AUTO_TRACE("StatusUpdater::onStart()"); }

void StatusUpdater::onStop() { AUTO_TRACE("StatusUpdater::onStop()"); }

void StatusUpdater::runLoop()
{
    try {
        bool isOk = true;
        PositionItem act_pos_item;
        CSecAreaResourceWrapper<sensorSocket> socket = (m_params->sensorLink)->Get();
        act_pos_item.exe_time = getTimeStamp();
        try {
            act_pos_item.position = socket->getPosition();
            m_isErrorPos = false;
        }
        catch(...) {
            if(!m_isErrorPos) {
                ACS_SHORT_LOG((LM_ERROR, "In StatusUpdater: cannot get the position"));
                m_isErrorPos = true;
            }
            isOk = false;
        }
        socket.Release();

        if(isOk) {
            CSecAreaResourceWrapper<vector< PositionItem> > apl = (m_params->act_pos_list)->Get();
            long diff = (*apl).size() - MAX_HISTORY_SIZE;
            if(diff > 0) {
                vector<PositionItem>::iterator ibegin = (*apl).begin();
                (*apl).erase(ibegin, ibegin + diff);
            }

            if((*apl).size() > MAX_HISTORY_SIZE) {
                ACS_SHORT_LOG((LM_ERROR, "In StatusUpdater: reached the MAX_HISTORY_SIZE"));
            }
            else {
                (*apl).push_back(act_pos_item);
            }
            apl.Release();
        }
        m_isError = false;
    }
    catch (...) {
        if(!m_isError) {
            ACS_SHORT_LOG((LM_ERROR, "unexpected error updating the position."));
            m_isError = true;
        }
    }
}
 

