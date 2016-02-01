#ifndef DEVIOPROJECTCODE_H_
#define DEVIOPROJECTCODE_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            	when                    What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it) 09/01/2013      Creation                                                  */

#include <baciDevIO.h>


/**
 * This  class is derived from the template DevIO. It is used by the by the scheduleName property. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOProjectCode: public virtual DevIO<ACE_CString>
{
public:
	
	DevIOProjectCode(CCore* core): m_core(core) {
		AUTO_TRACE("DevIOProjectCode::DevIOProjectCode()");
	}
	
	~DevIOProjectCode() {
		AUTO_TRACE("DevIOProjectCode::~DevIOProjectCode()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	ACE_CString read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOProjectCode::read()");
		IRA::CString code;
		timestamp=getTimeStamp();
		m_core->getProjectCode(code);
		m_val=(const char *)code;
		return m_val;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOProjectCode::write()");
	}
    
private:
	CCore *m_core;
	ACE_CString m_val;
};



#endif /*DEVIOPROJECTCODE_H_*/
