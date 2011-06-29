/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/
#ifndef __REQUEST_SCHEDULER_H__
#define __REQUEST_SCHEDULER_H__

#include <acsThread.h>
#include <LogFilter.h>
#include <IRA>
#include "MSParameters.h"
#include "WPServoTalker.h"
#include "WPServoSocket.h"

struct ThreadParameters {
    map<int, WPServoTalker *> *map_of_talkers_ptr;
    WPServoSocket *socket_ptr;
    pthread_mutex_t *listener_mutex;
    pthread_mutex_t *scheduler_mutex;
    pthread_mutex_t *status_mutex;
    ExpireTime *expire_time; 
    CSecureArea< map<int, vector<PositionItem> > > *cmd_pos_list;
    map<int, bool> *status_thread_en;
    double tracking_delta;
    bool *is_setup_exe_ptr;
    bool *is_stow_exe_ptr;
};

// This class takes care of getting requests from minor servos and send them. 
class RequestScheduler : public ACS::Thread
{
public:

	RequestScheduler(
            const ACE_CString& name, 
            ThreadParameters& params,
            const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
            const ACS::TimeInterval& sleepTime=scheduler_sleep_time
    );

    ~RequestScheduler();

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

};

#endif 

