#ifndef _cmdPosDEVIOS_H
#define _cmdPosDEVIOS_H

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
template <class T> class cmdPosDevIO : public DevIO<T>
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
	cmdPosDevIO(USDImpl* pusd,MOD_LAN::lan* plan,BYTE addr,BYTE cmd,BYTE wLen,BYTE rLen) : m_pusd(pusd),m_pLan(plan),m_addr(addr),m_cmd(cmd),m_wLen(wLen),m_rLen(rLen)   
	{
		ACS_TRACE("::cmdPosDevIO::cmdPosDevIO()");
	}

	/**
	 * Destructor
	*/ 
	~cmdPosDevIO()	{ACS_TRACE("::cmdPosDevIO::~cmdPosDevIO()");	}

	/** 
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{
		ACS_TRACE("::cmdPosDevIO::initializeValue()")
		return false;
	}
	
	/**
	 * Used to read values.
	*/ 
	T read( unsigned long long& timestamp)     throw (ACSErr::ACSbaseExImpl)
	{
		ACS_TRACE("::cmdPosDevIO::read()");
		
		CORBA::Long value;       
		
		if( m_rLen==0) return m_value;		// no hardware read for RW properties

		if (m_pusd->m_available) {	
			try {
				m_pLan->recUSDPar(m_cmd,m_addr,m_rLen,value);
			}
			catch (ASErrors::ASErrorsEx& ex) {
				m_pusd->exCheck(ex);
				throw ASErrors::USDErrorExImpl(ex,__FILE__,__LINE__,"cmdPosDevIO::read()");
			}        
		
			timestamp=getTimeStamp();
			m_value=value;
			return m_value;
		} else  {
			 throw ASErrors::USDUnavailableExImpl(__FILE__,__LINE__,"cmdPosDevIO::read()");
		}
	
	}
	/**
	 * Used to send values to USD.
	*/ 
	void write(const T& value,unsigned long long& timestamp)   throw (ACSErr::ACSbaseExImpl)
	{
		ACS_DEBUG("::cmdPosDevIO::write()","write invoked for %x command!");
		
		if( m_wLen==0) {
			m_value=value;		// no hardware write for RO properties
			return;
		}
		
		if (m_pusd->m_available) {	
			CompletionImpl comp(m_pLan->sendUSDCmd(m_cmd,m_addr,value,m_wLen));          
		
			if(m_pusd->compCheck(comp))	{
				throw ASErrors::USDErrorExImpl(comp,__FILE__,__LINE__,"cmdPosDevIO::write()");  
			}
			timestamp=getTimeStamp();
			m_value=value;
        } else {
			throw ASErrors::USDUnavailableExImpl(__FILE__,__LINE__,"cmdPosDevIO::write()");
		}
	}

	
private:
	// devono rispettare l'ordine di iniz. del costruttore
	USDImpl* m_pusd;		// usd object pointer
	MOD_LAN::lan* m_pLan;   // component reference
	BYTE m_addr;		// USD serial address
	BYTE m_cmd;			// USD command code
	BYTE m_wLen;		// param lenght in byte of parameter to write  
	BYTE m_rLen;		// param lenght in byte.of parameter to read  
	T m_value; 			// value stored for write only properties
	
};


#endif /*!_H*/
