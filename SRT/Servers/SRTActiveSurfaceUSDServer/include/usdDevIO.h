#ifndef _DEVIOS_H
#define _DEVIOS_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who            when            What                                              */
/* G. Maccaferri  26/07/2005      Creation                                          */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif
#include <baciDevIO.h>
#define ACK 0x06

class USDImpl; // required pre-declaration

using namespace IRA;


/**
 * This template class is derived from DevIO. It is used by the usd properties 
 * component.
 * @author <a href=mailto:g.maccaferri@ira.cnr.it> GMaccaferri</a>,
 * Istituto di Radioastronomia, Italia<br> 
*/
template <class T> class USDDevIO : public DevIO<T>
{
public:
	/** 
	 * Constructor
	 * @param pusd pointer to the  USD object. 
	 * @param lan  pointer to the  USD lan. 
 	 * @param addr USD serial address. 
	 * @param cmd usd command to be used for acces this property
	 * @param wLen byte-lengh of write parameter
	 * @param rLen byte-lengh of read parameter
	
	*/
	USDDevIO(USDImpl* pusd, SRTActiveSurface::lan* plan,BYTE addr,BYTE cmd,BYTE wLen,BYTE rLen) : m_pusd(pusd),m_pLan(plan),m_addr(addr),m_cmd(cmd),m_wLen(wLen),m_rLen(rLen)   
	//USDDevIO(USDImpl* pusd,MOD_LAN::lan* plan,BYTE addr,BYTE cmd,BYTE wLen,BYTE rLen) : m_pusd(pusd),m_pLan(plan),m_addr(addr),m_cmd(cmd),m_wLen(wLen),m_rLen(rLen)   
	{
		ACS_TRACE("::USDDevIO::USDDevIO()");
	}

	/**
	 * Destructor
	*/ 
	~USDDevIO()	{ACS_TRACE("::USDDevIO::~USDDevIO()");	}

	/** 
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{
		ACS_TRACE("::USDDevIO::initializeValue()");
		return false;
	}
	
	/**
	 * Used to read values.
	*/ 
	T read( ACS::Time &timestamp)     throw (ASErrors::ASErrorsExImpl);

	/**
	 * Used to send values to USD.
	*/ 
	void write(const T& value,ACS::Time &timestamp)   throw (ASErrors::ASErrorsExImpl);

	
private:
	// devono rispettare l'ordine di iniz. del costruttore
	USDImpl* m_pusd;		// usd object pointer
	SRTActiveSurface::lan* m_pLan;   // component reference
	//MOD_LAN::lan* m_pLan;   // component reference
	BYTE m_addr;		// USD serial address
	BYTE m_cmd;			// USD command code
	BYTE m_wLen;		// param lenght in byte of parameter to write  
	BYTE m_rLen;		// param lenght in byte.of parameter to read  
	T m_value; 			// value stored for write only properties
	
};


#endif /*!_H*/
