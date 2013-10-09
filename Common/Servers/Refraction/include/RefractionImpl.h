#ifndef REFRACTIONIMPL_H
#define REFRACTIONIMPL_H

/* ****************************************************************************** */
/* IRA Istituto di Radioastronomia                                                */
/* $Id: RefractionImpl.h,v 1.7 2009-01-29 12:37:20 c.migoni Exp $  */
/*                                                                                */
/* This code is under GNU General Public Licence (GPL).                           */
/*                                                                                */
/* Who                                when            What                        */
/* Carlo Migoni (migoni@ira.inaf.it)  01/02/2008      Creation                    */
/* Carlo Migoni (migoni@ira.inaf.it)  01/26/2009      Added thread loop           */
/* ****************************************************************************** */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/** 
 * @mainpage Refraction component documentation
 * @date 09/10/2013
 * @version 1.0.0
 * @author <a href=mailto:migoni@ira.inaf.it>Carlo Migoni</a>
 * @remarks Last compiled under ACS 8.02
*/

#include <acscomponentImpl.h>
#include <RefractionS.h>
#include <IRA>
#include <AntennaErrors.h>
#include <ComponentErrors.h>
#include <acsThread.h>
#include "RefractionCore.h"
#include <SecureArea.h>
#include "RefractionWorkingThread.h"

using namespace acscomponent;

/**
 * This class implements the Antenna::Refraction CORBA interface and the ACS Component.  
 * @author <a href=mailto:migoni@ira.inaf.it>Carlo Migoni</a>
 * Istituto di Radioastronomia, Italia
 * <br> 
 */
class RefractionImpl: public virtual acscomponent::ACSComponentImpl, public virtual POA_Antenna::Refraction
{
	public:
	
	/** 
	* Constructor.
	* @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
	* @param containerServices pointer to the class that exposes all services offered by container
	*/
	RefractionImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices);

	/**
	 * Destructor.
	*/
	virtual ~RefractionImpl(); 

	/** 
	 * Called to give the component time to initialize itself. The component reads in configuration files/parameters, builds up connection. 
	 * Called before execute. It is implemented as a synchronous (blocking) call.
	 * @throw ACSErr::ACSbaseExImpl
	 * @arg \c ComponentErrors::MemoryAllocation
	*/
	virtual void initialize() throw (ACSErr::ACSbaseExImpl);

	/**
 	 * Called after <i>initialize()</i> to tell the component that it has to be ready to accept incoming functional calls any time. 
	 * Must be implemented as a synchronous (blocking) call. In this class the default implementation only logs the COMPSTATE_OPERATIONAL
	 * @throw ACSErr::ACSbaseExImpl
	 * @arg \c ComponentErrors::CDBAccess
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
	* This method is used to calculate the corrected zenith distance (radians) due to refraction
	* @throw CORBA::SystemException
	* @param obsZenithDistance
	* @param waveLength wave length in meters
	* @param corZenithDistance
	*/
	void getCorrection (CORBA::Double obsZenithDistance,CORBA::Double waveLength, CORBA::Double_out corZenithDistance) throw (CORBA::SystemException);

	private:
    
    	IRA::CSecureArea<CRefractionCore> *m_core;
	CRefractionCore *boss;

	CRefractionWorkingThread *m_workingThread;
};

#endif /*REFRACTIONIMPL_H*/
