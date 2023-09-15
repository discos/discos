#ifndef SRTDBESMIMPL_H
#define SRTDBESMIMPL_H

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <LogFilter.h>
#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <baciROdouble.h>
#include <baciROlong.h>
#include <baciROdoubleSeq.h>
#include <baciROlongSeq.h>
#include "CommandSocket.h"
#include <SRTDBESMS.h>
//#include <baciDevIO.h>
#include <IRA>

using namespace baci;

class SRTDBESMImpl: public CharacteristicComponentImpl, public virtual POA_Backends::SRTDBESM
{

public:
	
	/** 
	* Constructor.
	* @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
	* @param containerServices pointer to the class that exposes all services offered by container
	*/
	SRTDBESMImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices);

	/**
	 * Destructor.
	*/
	virtual ~SRTDBESMImpl(); 

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
   
   virtual void set_all(const char * cfg_name) throw (BackendsErrors::BackendsErrorsEx);
   
   virtual void set_mode(short b_addr, const char * cfg_name) throw (BackendsErrors::BackendsErrorsEx);
       
   virtual void set_att(short b_addr, short out_ch, double att_val) throw (BackendsErrors::BackendsErrorsEx);
   
   virtual void store_allmode(const char * cfg_name) throw (BackendsErrors::BackendsErrorsEx);
   
   virtual void clr_mode(const char * cfg_name) throw (BackendsErrors::BackendsErrorsEx);
   
	/** 
	 * Returns a reference to addr_1.
	 * @return pointer to long property addr_1
	 * @throw (CORBA::SystemException);
	*/	
	virtual ACS::ROlong_ptr addr_1();


private:
		
	/** Component configuration data */
	CConfiguration m_compConfiguration;
	/**
	 *  socket used to issue commands to the ACU
	 */
	CCommandSocket m_commandSocket;
	
	SmartPropertyPointer<ROlong> m_paddr_1;	
	
};

#endif /*!_H*/
