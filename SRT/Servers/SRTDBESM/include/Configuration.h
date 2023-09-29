#ifndef CCONFIGURATION_H
#define CCONFIGURATION_H

#include <IRA>
#include <maciContainerServices.h>
#include <ComponentErrors.h>

using namespace IRA;

/**
 * This class implements the component configurator. The data inside this class are initialized at the startup from the
 * configuration database and then are used (read) inside the component.
  */
class CConfiguration {
public:	
	/**
	 * Constructor 
	*/
	CConfiguration();
	
	/**
	 * Destructor
	*/
	~CConfiguration();
	/**
    * This member function is used to configure component by reading the configuration parameter from the CDB.
	 * This must be the first call before using any other function of this class.
	 * @throwCDBAccess
	 * @param Services pointer to the container services object
	*/
	void init(maci::ContainerServices *Services) throw (ComponentErrors::CDBAccessExImpl);
	
	void init() throw (ComponentErrors::CDBAccessExImpl);
	
	/** Gets W band LO ip address */
	inline CString DBESMAddress() const { return m_DBESM_IPAddress; }
		
	/** Gets the socket port to which the LO is listening to commands */
	inline WORD DBESMPort() const { return m_DBESM_Port; }
	
	inline long addr_1() const { return m_addr_1; }
	inline long addr_2() const { return m_addr_2; }
	inline long addr_3() const { return m_addr_3; }
	inline long addr_4() const { return m_addr_4; }
	
	inline ACS::longSeq& regs_1() { return m_regs_1; }
	inline ACS::longSeq& regs_2() { return m_regs_2; }
	inline ACS::longSeq& regs_3() { return m_regs_3; }
	inline ACS::longSeq& regs_4() { return m_regs_4; }
		  		  
   inline ACS::doubleSeq& atts_1() { return m_atts_1; }
   inline ACS::doubleSeq& atts_2() { return m_atts_2; }
   inline ACS::doubleSeq& atts_3() { return m_atts_3; }
	inline ACS::doubleSeq& atts_4() { return m_atts_4; }
	
	inline ACS::longSeq& amps_1() { return m_amps_1; }
	inline ACS::longSeq& amps_2() { return m_amps_2; }
	inline ACS::longSeq& amps_3() { return m_amps_3; }
	inline ACS::longSeq& amps_4() { return m_amps_4; }
		  
	inline ACS::longSeq& eqs_1() { return m_eqs_1; }
	inline ACS::longSeq& eqs_2() { return m_eqs_2; }
	inline ACS::longSeq& eqs_3() { return m_eqs_3; }
	inline ACS::longSeq& eqs_4() { return m_eqs_4; }
		  
	inline ACS::longSeq& bpfs_1() { return m_bpfs_1; }
	inline ACS::longSeq& bpfs_2() { return m_bpfs_2; }	
	inline ACS::longSeq& bpfs_3() { return m_bpfs_3; }	
	inline ACS::longSeq& bpfs_4() { return m_bpfs_4; }	 	  	  
	     
   inline ACS::doubleSeq& volts_1() { return m_volts_1; }
   inline ACS::doubleSeq& volts_2() { return m_volts_2; }
   inline ACS::doubleSeq& volts_3() { return m_volts_3; }
   inline ACS::doubleSeq& volts_4() { return m_volts_4; }

	inline double temps_1() const { return m_temps_1; }
	inline double temps_2() const { return m_temps_2; }
	inline double temps_3() const { return m_temps_3; }
	inline double temps_4() const { return m_temps_4; }
	                    
private:

	long m_addr_1;
	long m_addr_2;
	long m_addr_3;
	long m_addr_4;
   CString m_DBESM_IPAddress;
	WORD m_DBESM_Port;
	
   ACS::longSeq m_regs_1;
	ACS::longSeq m_regs_2;
	ACS::longSeq m_regs_3;
	ACS::longSeq m_regs_4;
	
   ACS::doubleSeq m_atts_1;
   ACS::doubleSeq m_atts_2;
   ACS::doubleSeq m_atts_3;
   ACS::doubleSeq m_atts_4;
     
	ACS::longSeq m_amps_1;
	ACS::longSeq m_amps_2;
	ACS::longSeq m_amps_3;
	ACS::longSeq m_amps_4;
	
	ACS::longSeq m_eqs_1;
	ACS::longSeq m_eqs_2;
	ACS::longSeq m_eqs_3;
	ACS::longSeq m_eqs_4;
	
	ACS::longSeq m_bpfs_1;
	ACS::longSeq m_bpfs_2;
	ACS::longSeq m_bpfs_3;
	ACS::longSeq m_bpfs_4;
   	
	ACS::doubleSeq m_volts_1;
   ACS::doubleSeq m_volts_2;
   ACS::doubleSeq m_volts_3;
   ACS::doubleSeq m_volts_4;
   
   double m_temps_1;
   double m_temps_2;
   double m_temps_3;
   double m_temps_4;

};

#endif /*CONFIGURATION_H_*/
