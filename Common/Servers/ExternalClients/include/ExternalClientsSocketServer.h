#ifndef EXTERNALCLIENTSSOCKETSERVER_H
#define EXTERNALCLIENTSSOCKETSERVER_H

/* ************************************************************************ */
/* OAC Osservatorio Astronomico di Cagliari                                 */
/* $Id: ExternalClientsSocketServer.h,v 1.3 2011-03-11 12:28:47 c.migoni Exp $*/
/*                                                                          */
/* This code is under GNU General Public Licence (GPL).                     */
/*                                                                          */
/* Who              when            What                                    */
/* Carlo Migoni     26/11/2009      Creation                                */

#include <IRA>
#include <ExternalClientsS.h>
#include "acstime.h"
#include <ComponentErrors.h>
#include <ManagementErrors.h>
#include "Configuration.h"
#include <SchedulerC.h>
#include <AntennaBossC.h>
#include <ReceiversBossC.h>
#include <MountC.h>

#define BUFFERSIZE 1000
#define GAVINO "MANAGEMENT/Gavino"
#define PALMIRO "MANAGEMENT/Palmiro"
#define DUCEZIO "MANAGEMENT/Ducezio"

using namespace IRA;
using namespace maci;

/**
 * This class is inherited from the IRA::CSocket class. It takes charge of getting data from the ACU and parsing 
 * its responses. New data are requested to the ACU only if the data are not up to date any more. if the remote side
 * disconnects or a problem affects the comunication line, this class try to reconnect to the ACU.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CExternalClientsSocketServer: public CSocket
{
public:
	/**
	 * Constructor.
	*/
	CExternalClientsSocketServer(ContainerServices *service);
	/**
	 * Destructor
	*/
	virtual ~CExternalClientsSocketServer();
	/**
     * This member function is used to enstablish and configure the communication channel. 
	 * This must be the first call before using any other function of this class.
	 * @param config pointer to the component configuration data structure
	 * @throw ComponentErrors::SocketError
	 * @arg \c IRALibraryResource>
	*/
	void initialize(CConfiguration *config) throw (ComponentErrors::SocketErrorExImpl);

	void execute() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl);

	void cleanUp();

	void cmdToScheduler();

	/** This is used to describe the status of connection to the External Client */
	enum ExternalClientSocketStatus { 
		ExternalClientSocketStatus_NOTCNTD,   	         /*!< Socket is not connected, no operation available  */
		ExternalClientSocketStatus_CNTDING,             /*!< Connection is in progress, no operation available */
		ExternalClientSocketStatus_CNTD,  		         /*!< Socket is connected and ready, operation available*/
		ExternalClientSocketStatus_BSY			         /*!< Socket is busy, no operation available */		
	};

    	/** 
	* Sets the external client socket status
	* if status is set to ExternalClientSocketStatus::ExternalClientSocketStatus_BSY then the m_bBusy flag is also set,
	* this flag remains on until the status is set ExternalClientSocketStatus::ExternalClientSocketStatus_CNTD esplicitally.
	* @param status the new external socket status status.
	*/
	void setExternalClientSocketStatus(ExternalClientSocketStatus status);

    	/** 
	* Gets the external socket status 
	* @return the present external socket status
	*/
	ExternalClientSocketStatus getStatus() const;

    	/** 
	* This function is used to check if the socket is busy with some stuff.
	* @return true if the antenna is not ready to accept new commands.
	*/
	bool isBusy();

    	bool m_byebye;

    	void byebye() {m_byebye=true;};
	
private:
    	ContainerServices* m_services;

	/** It contains error information */	
	CError m_Error;
	/** Component configuration data */
	CConfiguration *m_configuration;
    	/** */
	bool m_bTimedout;
	/** This flag is true if the socket is busy (turned on when the status is BSY, turned off when the status is CNTD again)*/
	bool m_bBusy;
    	/** Connection status */
	ExternalClientSocketStatus m_Status;
    	// New Socket created by Accept function
    	CSocket newExternalClientsSocketServer;
    	/** This is the reference to the scheduler component */
    	Management::Scheduler_var m_Scheduler;
    	/** This is the reference to the antenna boss component */
    	Antenna::AntennaBoss_var m_antennaBoss;
    	/** This is the reference to the receiver boss component */
    	Receivers::ReceiversBoss_var m_receiversBoss;
    	/** This is the reference to the mount component */
    	Antenna::Mount_var m_mount;

    	int receiveBuffer(BYTE *Msg,WORD Len);
    	void printBuffer(BYTE *Buff,WORD Len);
    	OperationResult sendBuffer(BYTE *Msg,WORD Len);
    	bool m_accept;
    	char superVisorName[20];
};

#endif
