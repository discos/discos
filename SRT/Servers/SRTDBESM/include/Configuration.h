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
	
	/*inline ACS::ROlong addr_1() const { return m_addr_1; }
	inline long addr_2() const { return m_addr_2; }
	inline long addr_3() const { return m_addr_3; }
	inline long addr_4() const { return m_addr_4; } */
	
private:

	long m_addr_1;
	long m_addr_2;
	long m_addr_3;
	long m_addr_4;
   CString m_DBESM_IPAddress;
	WORD m_DBESM_Port;
	unsigned long m_regs_1[10];
	unsigned long m_regs_2[10];
	unsigned long m_regs_3[10];
	unsigned long m_regs_4[10];
   double m_atts_1[17];
   double m_atts_2[17];
   double m_atts_3[17];
   double m_atts_4[17];
	unsigned long m_amps_1[10];
	unsigned long m_amps_2[10];
	unsigned long m_amps_3[10];
	unsigned long m_amps_4[10];
	unsigned long m_eqs_1[10];
	unsigned long m_eqs_2[10];
	unsigned long m_eqs_3[10];
	unsigned long m_eqs_4[10];
	unsigned long m_bpfs_1[10];
	unsigned long m_bpfs_2[10];
	unsigned long m_bpfs_3[10];
	unsigned long m_bpfs_4[10];
	double m_volts_1[2];
   double m_volts_2[2];
   double m_volts_3[2];
   double m_volts_4[2];
   double m_temps_1[2];
   double m_temps_2[2];
   double m_temps_3[2];
   double m_temps_4[2];

};

#endif /*CONFIGURATION_H_*/
