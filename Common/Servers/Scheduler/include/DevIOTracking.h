#ifndef DEVIOTRACKING_H_
#define DEVIOTRACKING_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: DevIOTracking.h,v 1.1 2010-09-14 10:25:17 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            	when                    What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it) 31/05/2010      Creation                                                  */

#include <baciDevIO.h>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the by the tracking property. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOTracking: public virtual DevIO<Management::TBoolean>
{
public:
	
	DevIOTracking(CCore* core): m_core(core) { 
		AUTO_TRACE("DevIOTracking::DevIOTracking()");
	}
	
	~DevIOTracking() {
		AUTO_TRACE("DevIOTracking::~DevIOTracking()");
	}
	
	bool initializeValue() {
		return false;
	}
	
	Management::TBoolean read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOTracking::read()");
		timestamp=getTimeStamp();
		if (m_core->isTracking()) {
			m_val=Management::MNG_TRUE;
		}
		else {
			m_val=Management::MNG_FALSE;
		}
		return m_val;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOTracking::write()");
	}
    
private:
	CCore *m_core;
	Management::TBoolean m_val;
};







#endif /*DEVIOTRACKING_H_*/
