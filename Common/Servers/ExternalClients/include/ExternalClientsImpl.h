#ifndef EXTERNALCLIENTSIMPL_H
#define EXTERNALCLIENTSIMPL_H

/* **************************************************************************/
/* OAC - Osservatorio Astronomico di Cagliari                               */
/* $Id: ExternalClientsImpl.h,v 1.1 2010-01-20 10:48:11 c.migoni Exp $     */
/*                                                                          */
/* This code is under GNU General Public Licence (GPL).                     */
/*                                                                          */
/* Who                                when            What                  */
/* Carlo Migoni (migoni@ca.astro.it)  14/12/2009      Creation              */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <ComponentErrors.h>
#include <acsThread.h>
#include <enumpropROImpl.h>
#include <ExternalClientsS.h>
#include "ExternalClientsSocketServer.h"
#include "ExternalClientsThread.h"

/** 
 * @mainpage External Clients component documentation
 * @date 14/12/2009
 * @version 1.0.0
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>
 * @remarks Last compiled under ACS 7.0.2
*/

using namespace baci;

/**
 * This class implements the Management::ExternalClients CORBA interface and the ACS CharacteristicComponent.  
 * All exception that comes from the run-time interation with clients are at logged with LM_DEBUG priority.
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>
 * Osservatorio Astronomico di Cagliari, Italia
 * <br> 
 */
class ExternalClientsImpl: public CharacteristicComponentImpl,
				   public virtual POA_Management::ExternalClients
{
    public:
	
	/** 
	* Constructor.
	* @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
	* @param containerServices pointer to the class that exposes all services offered by container
	*/
	ExternalClientsImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices);

	/**
	 * Destructor.
	*/
	virtual ~ExternalClientsImpl(); 

	/** 
	 * Called to give the component time to initialize itself. The component reads in configuration files/parameters, builds up connection. 
	 * Called before execute. It is implemented as a synchronous (blocking) call.
	 * @throw ACSErr::ACSbaseExImpl
	 * 		  @arg \c ComponentErrors::MemoryAllocation
	*/
	virtual void initialize() throw (ACSErr::ACSbaseExImpl);

	/**
 	 * Called after <i>initialize()</i> to tell the component that it has to be ready to accept incoming functional calls any time. 
	 * Must be implemented as a synchronous (blocking) call. In this class the default implementation only logs the COMPSTATE_OPERATIONAL
	 * @throw ACSErr::ACSbaseExImpl
	 *    @arg \c ComponentErrors::CDBAccess
	 *    @arg \c ComponentErrors::SocketError
	*/
	virtual void execute() throw (ACSErr::ACSbaseExImpl);
	
	/** 
	 * Called by the container before destroying the server in a normal situation. This function takes charge of releasing all resources.
	*/
	virtual void cleanUp();
	
	/** 
	 * Called by the container in case of error or emergency situation. This function tries to free all resources even though there is no
	 * warranty that the function is completely executed before the component is destroyed.
	*/	
	virtual void aboutToAbort();
	
	/** 
	 * Returns a reference to the status property implementation of IDL interface.
	 * @return pointer to read-only TStatus property status
	*/	
	virtual Management::ROTSystemStatus_ptr status() throw (CORBA::SystemException);

    /**
	 * This method implements the command line interpreter. The interpreter allows to ask for services or to issue commands
	 * to the control system by human readable command lines.
	 * @throw CORBA::SystemException
	 * @throw ManagementErrors::CommandLineErrorEx
	 * @param cmd string that contains the command line
	 * @return the string that contains the answer to the command.
	 */
	virtual char * command(const char *cmd) throw (CORBA::SystemException,ManagementErrors::CommandLineErrorEx);

private:
	
	/** status property */
    SmartPropertyPointer < ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus), POA_Management::ROTSystemStatus> > m_pStatus;
	/** Server socket container */
	CSecureArea<CExternalClientsSocketServer>* m_ExternalClientsSocketServer;
	/** Control loop thread */
	CExternalClientsThread *m_pExternalClientsThread;
	/** Component configuration data */
	CConfiguration m_CompConfiguration;
    /** Control Thread Period */
    DWORD m_ControlThreadPeriod;
};

#endif /*!_H*/
