/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/
#ifndef __SOCKET_LISTENER_H__
#define __SOCKET_LISTENER_H__

#include <acsThread.h>
#include <LogFilter.h>
#include <IRA>
#include "MSParameters.h"
#include "RequestDispatcher.h"

// This class accomplishes a routing of messages from MSCU to minor servos. 
class SocketListener : public ACS::Thread
{
public:

	SocketListener(
            const ACE_CString& name, 
            ThreadParameters& params,
            const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
            const ACS::TimeInterval& sleepTime=listener_sleep_time
    );

    ~SocketListener();

    /// This method is executed once when the thread starts.
    virtual void onStart();

     /// This method is executed once when the thread stops.
     virtual void onStop();

     /**
     * This method overrides the thread implementation class. The method is executed in a 
     * loop until the thread is alive. The thread can be exited by calling 
     * ACS::ThreadBase::stop or ACS::ThreadBase::exit command.
     */
     virtual void runLoop() throw (ComponentErrors::SocketErrorExImpl);

private:
     ThreadParameters *m_params;

    /** 
     * The key is a string that identify the command: "cmd_num=app_num".
     * The value is a counter of setup answers of a command with the identifier used as a key.
     */
    map<string, unsigned short> m_setup_counter;

    /** 
     * The key is a string that identify the command: "cmd_num=app_num".
     * The value is a counter of stow answers of a command with the identifier used as a key.
     */
    map<string, unsigned short> m_stow_counter;

};

#endif 

