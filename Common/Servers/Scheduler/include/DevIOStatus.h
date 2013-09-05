#ifndef DEVIOSTATUS_H_
#define DEVIOSTATUS_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: DevIOStatus.h,v 1.1 2009-08-04 13:26:22 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            	when                    What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it) 04/06/2009      Creation                                                  */

#include <baciDevIO.h>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the by the status property. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOStatus: public virtual DevIO<Management::TSystemStatus>
{
public:
	
	DevIOStatus(CCore* core): m_core(core) { 
		AUTO_TRACE("DevIOStatus::DevIOStatus()");
	}
	
	~DevIOStatus() {
		AUTO_TRACE("DevIOStatus::~DevIOStatus()");
	}
	
	bool initializeValue() {
		return false;
	}
	
	Management::TSystemStatus read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOStatus::read()");
		timestamp=getTimeStamp();
		m_val=m_core->getStatus();
		return m_val;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOStatus::write()");
	}
    
private:
	CCore *m_core;
	Management::TSystemStatus m_val;
};





#endif /*DEVIOSTATUS_H_*/
