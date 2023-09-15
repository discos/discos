#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: WatchDog.h,v 1.1 2023-08-01 10:07:55 lorenzo.monti Exp $										   */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                     when             What                                                 */
/* Lorenzo Monti(lorenzo.monti@inaf.it)  01/08/2023      Creation                                                */

#include <acsThread.h>
#include <IRA>

/**
 * This class implements a watching thread. This thread is in charge of checking the component functionality and in case to try to recover from error situations  
*/
class CWatchDog : public ACS::Thread
{
public:
	typedef struct {
		CCALSocket * calSocket;
		CLOSocket * loSocket;
          CSwMatrixSocket * swMatrixSocket;
	} TThreadParameter;
	/**
     * Constructor().
     * @param name thread name
     * @param parameter pointer to the thread parameters structure
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
    */
	CWatchDog(const ACE_CString& name,TThreadParameter *parameter, 
			const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

	/**
	 * Destructor.
    */
    ~CWatchDog();

     /**
     * This method is executed once when the thread starts.
    */
    virtual void onStart();

    /**
     * This method is executed once when the thread stops.
     */
     virtual void onStop();

     /**
      * This method overrides the thread implementation class. The method is executed in a loop until the thread is alive.
      * The thread can be exited by calling ACS::ThreadBase::stop or ACS::ThreadBase::exit command.
     */
     virtual void runLoop();
private:
	TThreadParameter * m_param;
};

#endif


