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
#include <baciROstring.h>
#include "CommandSocket.h"
#include <SRTDBESMS.h>
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
	virtual void execute(); //throw (ACSErr::ACSbaseExImpl);
	
	/** 
	 * Called by the container before destroying the server in a normal situation. This function takes charge of releasing all resources.
	*/
	virtual void cleanUp();
	
	/** 
	 * Called by the container in case of error or emergency situation. This function tries to free all resources even though there is no
	 * warranty that the function is completely executed before the component is destroyed.
	*/	
	virtual void aboutToAbort();
   
   virtual void set_all(const char * cfg_name); //throw (BackendsErrors::BackendsErrorsEx);
   
   virtual void set_mode(short b_addr, const char * cfg_name); //throw (BackendsErrors::BackendsErrorsEx);
       
   virtual void set_att(short b_addr, short out_ch, double att_val); //throw (BackendsErrors::BackendsErrorsEx);
   
   virtual void store_allmode(const char * cfg_name); //throw (BackendsErrors::BackendsErrorsEx);
   
   virtual void delete_file(const char * cfg_name); //throw (BackendsErrors::BackendsErrorsEx);
 	
 	virtual void get_status(short b_addr); //throw (BackendsErrors::BackendsErrorsEx);
 	
 	virtual void get_comp(short b_addr); //throw (BackendsErrors::BackendsErrorsEx);
 	
 	virtual void get_diag(short b_addr); //throw (BackendsErrors::BackendsErrorsEx);
 	
 	virtual void get_cfg();
 	
 	virtual void set_dbeatt(const char * out_dbe, const char * att_val); //throw (BackendsErrors::BackendsErrorsEx);
 	
 	virtual void get_dbeatt(const char * out_dbe); //throw (BackendsErrors::BackendsErrorsEx);
 	
 	virtual void get_firm(short b_addr);
 	
 	virtual void handle_addr(short * b_addr);

	/** 
	 * Returns a reference to addr_1.
	 * @return pointer to long property addr_1
	 * @throw (CORBA::SystemException);
	*/	
	virtual ACS::ROlong_ptr addr_1(), addr_2(), addr_3(), addr_4();
	virtual ACS::ROlongSeq_ptr regs_1(), regs_2(), regs_3(), regs_4();
	virtual ACS::ROdoubleSeq_ptr atts_1(), atts_2(), atts_3(), atts_4();
	virtual ACS::ROlongSeq_ptr amps_1(), amps_2(), amps_3(), amps_4();
	virtual ACS::ROlongSeq_ptr eqs_1(), eqs_2(), eqs_3(), eqs_4();
	virtual ACS::ROlongSeq_ptr bpfs_1(), bpfs_2(), bpfs_3(), bpfs_4();
   virtual ACS::ROdoubleSeq_ptr volts_1(), volts_2(), volts_3(), volts_4();
   virtual ACS::ROdouble_ptr temps_1(), temps_2(), temps_3(), temps_4();
   virtual ACS::ROstring_ptr cfg_1();
   virtual ACS::ROstring_ptr cfg_2();
   virtual ACS::ROstring_ptr cfg_3();
   virtual ACS::ROstring_ptr cfg_4();

private:
		
	/** Component configuration data */
	CConfiguration m_compConfiguration;
	/**
	 *  socket used to issue commands to the ACU
	 */
	CCommandSocket m_commandSocket;
	
	SmartPropertyPointer<ROlong> m_paddr_1, m_paddr_2, m_paddr_3, m_paddr_4;
	SmartPropertyPointer<ROlongSeq> m_pregs_1, m_pregs_2, m_pregs_3, m_pregs_4;
	SmartPropertyPointer<ROdoubleSeq> m_patts_1, m_patts_2, m_patts_3, m_patts_4;
	SmartPropertyPointer<ROlongSeq> m_pamps_1, m_pamps_2, m_pamps_3, m_pamps_4;
	SmartPropertyPointer<ROlongSeq> m_peqs_1, m_peqs_2, m_peqs_3, m_peqs_4;
	SmartPropertyPointer<ROlongSeq> m_pbpfs_1, m_pbpfs_2, m_pbpfs_3, m_pbpfs_4;
	SmartPropertyPointer<ROdoubleSeq> m_pvolts_1, m_pvolts_2, m_pvolts_3, m_pvolts_4;
	SmartPropertyPointer<ROdouble> m_ptemps_1, m_ptemps_2, m_ptemps_3, m_ptemps_4;
	SmartPropertyPointer<ROstring> m_pcfg_1;
	SmartPropertyPointer<ROstring> m_pcfg_2, m_pcfg_3, m_pcfg_4;
	
};

#endif /*!_H*/
