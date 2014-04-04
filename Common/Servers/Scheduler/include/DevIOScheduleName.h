#ifndef DEVIOSCHEDULENAME_H_
#define DEVIOSCHEDULENAME_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: DevIOScheduleName.h,v 1.1 2009-01-16 17:44:57 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            	when                    What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it) 12/01/2009      Creation                                                  */

#include <baciDevIO.h>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the by the scheduleName property. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOScheduleName: public virtual DevIO<ACE_CString>
{
public:
	
	DevIOScheduleName(CCore* core): m_core(core) { 
		AUTO_TRACE("DevIOScheduleName::DevIOScheduleName()");
	}
	
	~DevIOScheduleName() {
		AUTO_TRACE("DevIOScheduleName::~DevIOScheduleName()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	ACE_CString read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOScheduleName::read()");
		IRA::CString name;
		timestamp=getTimeStamp();
		m_core->getScheduleName(name);
		m_val=(const char *)name;
		return m_val;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOScanNumber::write()");
	}
    
private:
	CCore *m_core;
	ACE_CString m_val;
};



#endif /*DEVIOSCHEDULENAME_H_*/
