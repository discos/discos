/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/
#ifndef __STATUS_UPDATER_H__
#define __STATUS_UPDATER_H__

#include <acsThread.h>
#include <LogFilter.h>
#include "sensorSocket.h"
#include <IRA>


struct ThreadParameters {
    CSecureArea<sensorSocket> *sensorLink;
    CSecureArea< vector<PositionItem> > *act_pos_list;
};

 
// This class updates the status property of minor servos.
class StatusUpdater : public ACS::Thread
{
public:

	StatusUpdater(
            const ACE_CString& name, 
            ThreadParameters& params,
            const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
            const ACS::TimeInterval& sleepTime=2000*10 // 2ms
    );

    ~StatusUpdater();

    /// This method is executed once when the thread starts.
    virtual void onStart();

     /// This method is executed once when the thread stops.
     virtual void onStop();

     /**
     * This method overrides the thread implementation class. The method is executed in a 
     * loop until the thread is alive. The thread can be exited by calling 
     * ACS::ThreadBase::stop or ACS::ThreadBase::exit command.
     */
     virtual void runLoop();

private:
     ThreadParameters *m_params;
     bool m_isError;
     bool m_isErrorPos;
};

#endif 

