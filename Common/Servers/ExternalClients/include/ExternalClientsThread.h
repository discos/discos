#ifndef EXTERNALCLIENTSTHREAD_H
#define EXTERNALCLIENTSTHREAD_H

/* ******************************************************************************************* */
/* OAC - Osservatorio Astronomico di Cagliari                                                  */
/* $Id: ExternalClientsThread.h,v 1.1 2010-01-20 10:48:11 c.migoni Exp $		           */
/*                                                                                             */
/* This code is under GNU General Public Licence (GPL).                                        */
/*                                                                                             */
/* Who                                when            What                                     */
/* Carlo Migoni                       16/12/2009      Creation                                 */

#include <acsThread.h>
#include <IRA>
#include "ExternalClientsSocketServer.h"

using ACS::ThreadBase;

/**
 * This class inheriths from the ACS thread base class.
 * It allows to run separate thread that performs a continuos listening of commands
 * coming from external clients.
 * @author <a href=mailto:c.migoni@ca.astro.it>Carlo Migoni</a>,
 * Osservatorio Astronomico di Cagliari, Italia
 * <br> 
 */
class CExternalClientsThread : public ACS::Thread
{
public:
	/**
	 * Constructor().
	 * @param name thread name
	 * @param ACUControl pointer to the MedicinaMount component implementation 
	 * @param responseTime thread's heartbeat response time in 100ns unit. Default value is 1s.
     * @param sleepTime	thread's sleep time in 100ns unit. Default value is 100ms.
	*/
	CExternalClientsThread(const ACE_CString& name, IRA::CSecureArea<CExternalClientsSocketServer> *param,
                        const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
                        const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime); 

	/**
	 * Destructor. 
	*/
    ~CExternalClientsThread();
	
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

    IRA::CSecureArea<CExternalClientsSocketServer> *m_param;
};

	



#endif
