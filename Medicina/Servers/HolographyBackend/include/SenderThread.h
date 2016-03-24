#ifndef _SENDERTHREAD_H_
#define _SENDERTHREAD_H_
/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: SenderThread.h,v 1.1 2010-09-17 17:11:51 spoppi Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  21/11/2008      Creation                                                  */
/* Sergio Poppi (spoppi@oa-cagliari.inaf.it) 01/04/2010  Adapted for holography Backend							 */
#include "HolographyImpl.h"
#include <maciSimpleClient.h>
#include <AntennaBossC.h>

#include <acsThread.h>
#include <IRA>

class HolographyImpl;

/**
 * This class implements a working thread. This thread is in charge of reading the data from the backend, doing a pre processing and then
 * publishing them into the ACS bulk data channel. 
*/
class CSenderThread : public ACS::Thread
{
public:
//		TotalPowerImpl *sender;


         struct TSenderParameter {
		
		DXC::DigitalXCorrelator_var dxc_correlator;
		HolographyImpl *sender;
		Antenna::AntennaBoss_var  antennaBoss;
	}; // the thread parameter contains
           //  reference to the thread CSender 



	/**
     * Constructor().
     * @param name thread name
     * @param par thread parameter 
     * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime thread's sleep time in 100ns unit. Default value is 100ms.
    */
	CSenderThread(const ACE_CString& name,TSenderParameter *param, const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
			const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime);

	/**
	 * Destructor.
    */
    ~CSenderThread();

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
     
     /**
      * This method is used to inform the thread that should not send data anymore
      * @param stop true the thread will not send dsendata any more
      */
     
     void setStop(bool stop) { m_stop=stop; }


     void resumeTransfer() { m_go=true; }  
     void suspendTransfer() { m_stop=true; }


private:
	TSenderParameter *threadPar;
	Antenna::AntennaBoss_var m_antenna_boss;
	DXC::DigitalXCorrelator_var m_dxc_correlator;
	HolographyImpl *m_sender;
	bool m_stop;
	bool m_go;
	bool m_sending;

};

#endif /*_SENDERTHREAD_H_*/
