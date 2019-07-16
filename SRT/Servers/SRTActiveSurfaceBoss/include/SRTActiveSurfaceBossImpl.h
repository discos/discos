#ifndef SRTACTIVESURFACEBOSSIMPL_H
#define SRTACTIVESURFACEBOSSIMPL_H

/* ****************************************************************************** */
/* OAC Osservatorio Astronomico di Cagliari                                       */
/* $Id: SRTActiveSurfaceBossImpl.h,v 1.4 2011-03-11 12:30:53 c.migoni Exp $       */
/*                                                                                */
/* This code is under GNU General Public Licence (GPL).                           */
/*                                                                                */
/* Who                                when            What                        */
/* Carlo Migoni (migoni@ca.astro.it)  25/02/2009      Creation                    */
/* ****************************************************************************** */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <baciROdouble.h>
#include <baciROstring.h>
#include <enumpropROImpl.h>
#include <Definitions.h>
#include <SRTActiveSurfaceBossS.h>
#include <IRA>
#include <acsThread.h>
#include <SecureArea.h>
#include <ComponentErrors.h>
#include <ASErrors.h>
#include <ManagementErrors.h>
#include "SRTActiveSurfaceBossCore.h"
#include "SRTActiveSurfaceBossWorkingThread.h"
#include "SRTActiveSurfaceBossSectorThread.h"
#include <SP_parser.h>

#define LOOPSTATUSTIME 10000000 // 1.0 second
#define LOOPWORKINGTIME 5000000 // 0.5 seconds
#define SECTORTIME 1000000 // 0.1 seconds

#define _SET_CDB(PROP,LVAL,ROUTINE) {	\
	maci::ContainerServices* cs=getContainerServices();\
		if (!CIRATools::setDBValue(cs,#PROP,(const long&) LVAL)) \
		{ ASErrors::CDBAccessErrorExImpl exImpl(__FILE__,__LINE__,ROUTINE); \
			exImpl.setFieldName(#PROP); throw exImpl; \
		} \
}
/*
#define _GET_CDB(PROP,LVAL,ROUTINE) {	\
	maci::ContainerServices* cs=getContainerServices();\
		if (!CIRATools::getDBValue(cs,#PROP,(long&) LVAL)) \
		{ ASErrors::CDBAccessErrorExImpl exImpl(__FILE__,__LINE__,ROUTINE); \
			exImpl.setFieldName(#PROP); throw exImpl; \
		} \
}
*/
using namespace baci;
using namespace maci;
using namespace ASErrors;
using namespace ComponentErrors;

/**
 * This class implements the ActiveSurface::SRTActiveSurfaceBoss CORBA interface and the ACS Component.  
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>
 * Osservatorio Astronomico di Cagliari, Italia
 * <br> 
 */
class SRTActiveSurfaceBossImpl: public virtual CharacteristicComponentImpl, public virtual POA_ActiveSurface::SRTActiveSurfaceBoss
{
	public:
	
	/** 
	* Constructor.
	* @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
	* @param containerServices pointer to the class that exposes all services offered by container
	*/
	SRTActiveSurfaceBossImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices);

	/**
	 * Destructor.
	*/
	virtual ~SRTActiveSurfaceBossImpl(); 

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
	 * Returns a reference to the status property Implementation of IDL interface.
	 * @return pointer to read-only ROTSystemStatus property status
	*/
	virtual Management::ROTSystemStatus_ptr status() throw (CORBA::SystemException);
	
	/**
	 * Returns a reference to the enable property implementation of IDL interface.
	 * @return pointer to read-only ROTBoolean  property enabled
	*/
	virtual Management::ROTBoolean_ptr enabled() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the enable property implementation of IDL interface.
	 * @return pointer to read-only ROTBoolean  property enabled
	*/
	virtual ActiveSurface::ROTASProfile_ptr pprofile() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the tracking property implementation of IDL interface.
	 * @return pointer to read-only ROTBoolean  property enabled
	*/
	virtual Management::ROTBoolean_ptr tracking() throw (CORBA::SystemException);

	/**
	 *  This method can be called in order to disable the automatic update of the surface.
	 * @throw CORBA::SystemException 
	*/	
	void asOff() throw (CORBA::SystemException);
		
	/**
	 *  This method can be called in order to enable the automatic update of the surface.
	 * @throw CORBA::SystemException 
	*/		
	void asOn() throw (CORBA::SystemException);

	/**
	 * This is the command line interpreter for the sub-system. All the attributes and all the methods exposed by the boss can be
	 * called. That means a full set of operation for standard observation, but not full control of the system.
	 * @param the string that contains the command line to be parsed
	 * @return the string that contains the answer to the command issued by the input parameter. The caller is resposible to
	 * free the returned string (@sa CORBA::string_free).
	 * @todo provide e full description of the syntax and protocol (to be decided yet)
	*/
	virtual CORBA::Boolean command(const char *cmd,CORBA::String_out answer) throw (CORBA::SystemException);
	//virtual char * command(const char *cmd) throw (CORBA::SystemException, ManagementErrors::CommandLineErrorEx);

	/**
	 * This method is used to park (i.e. reference position) the active surface).
	 * @throw CORBA::SystemExcpetion
	 * @throw ManagementErrors::ParkingErrorEx  
	 */
	void park() throw (CORBA::SystemException, ManagementErrors::ParkingErrorEx);

	/**
	 * This method will be used to configure the MinorServoBoss before starting an observation
	 * @param config mnemonic code of the required configuration
	 * @throw CORBA::SystemException
	 * @throw ManagementErrors::ConfigurationErrorEx
	 */
	void setup(const char *config) throw (CORBA::SystemException, ManagementErrors::ConfigurationErrorEx);

	void stop ( CORBA::Long circle,  CORBA::Long actuator,  CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

	//void setup ( CORBA::Long circle,  CORBA::Long actuator,  CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

	void stow ( CORBA::Long circle,  CORBA::Long actuator,  CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

	void refPos ( CORBA::Long circle,  CORBA::Long actuator,  CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

	void update ( CORBA::Double elevation) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

	void move ( CORBA::Long circle,  CORBA::Long actuator,  CORBA::Long radius, CORBA::Long incr) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

	void correction ( CORBA::Long circle,  CORBA::Long actuator,  CORBA::Long radius, CORBA::Double correction) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

	void setProfile ( ActiveSurface::TASProfile profile) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

	void usdStatus4GUIClient( CORBA::Long circle,  CORBA::Long actuator, CORBA::Long_out status) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

	void setActuator (CORBA::Long circle, CORBA::Long actuator, CORBA::Long_out actPos, CORBA::Long_out cmdPos, CORBA::Long_out Fmin, CORBA::Long_out Fmax, CORBA::Long_out acc, CORBA::Long_out delay) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

	void up ( CORBA::Long circle,  CORBA::Long actuator,  CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

	void down ( CORBA::Long circle,  CORBA::Long actuator,  CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

	void bottom ( CORBA::Long circle,  CORBA::Long actuator,  CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

	void top ( CORBA::Long circle,  CORBA::Long actuator,  CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

	void reset ( CORBA::Long circle,  CORBA::Long actuator,  CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

	void calibrate ( CORBA::Long circle,  CORBA::Long actuator,  CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

	void calVer ( CORBA::Long circle,  CORBA::Long actuator,  CORBA::Long radius) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

	void recoverUSD( CORBA::Long circle,  CORBA::Long actuator) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx );

	private:
	/**
	* pointer to Container Services
	*/
	ContainerServices* cs;

	CSRTActiveSurfaceBossWorkingThread *m_workingThread;

	std::vector<CSRTActiveSurfaceBossSectorThread*> m_sectorThread;

	SimpleParser::CParser<CSRTActiveSurfaceBossCore> *m_parser;

	SmartPropertyPointer < ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus), POA_Management::ROTSystemStatus> > m_pstatus;

	SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>  > m_penabled;
	SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(ActiveSurface::TASProfile), POA_ActiveSurface::ROTASProfile> > m_pprofile;
	SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean> > m_ptracking;
	IRA::CSecureArea<CSRTActiveSurfaceBossCore> *m_core;

	/* *
	* Active Surface profile
	*/
	ActiveSurface::TASProfile m_profile;

	CSRTActiveSurfaceBossCore *boss;
};

#endif /*SRTACTIVESURFACEBOSSIMPL_H*/
