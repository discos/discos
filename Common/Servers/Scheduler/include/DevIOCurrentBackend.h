#ifndef DEVIOCURRENTBACKNED_H_
#define DEVIOCURRENTBACKNED_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            	when                    What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it) 28/08/2013      Creation                                                  */

#include <baciDevIO.h>


/**
 * This  class is derived from the template DevIO. It is used by the by the currentBackend property.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOCurrentBackend: public virtual DevIO<ACE_CString>
{
public:
	
	DevIOCurrentBackend(CCore* core): m_core(core) {
		AUTO_TRACE("DevIOCurrentBackend::DevIOCurrentBackend()");
	}
	
	~DevIOCurrentBackend() {
		AUTO_TRACE("DevIOCurrentBackend::~DevIOCurrentBackend()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	ACE_CString read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOCurrentBackend::read()");
		IRA::CString bck;
		timestamp=getTimeStamp();
		m_core->getCurrentBackend(bck);
		m_val=(const char *)bck;
		return m_val;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOCurrentBackend::write()");
	}
    
private:
	CCore *m_core;
	ACE_CString m_val;
};



#endif /*DEVIOCURRENTBACKNED_H_*/
