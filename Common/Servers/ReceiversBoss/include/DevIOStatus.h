#ifndef _DEVIOSTATUS_H_
#define _DEVIOSTATUS_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                  When                       What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  22/07/2010      Creation                                                  */

#include <baciDevIO.h>
#include <IRA>

/**
 * This  class is derived from the template DevIO. It is used by the  status property. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOStatus: public virtual DevIO<Management::TSystemStatus>
{
	
public:
	
	DevIOStatus(CRecvBossCore* core): m_pCore(core) { 
		AUTO_TRACE("DevIOStatus::DevIOStatus()");
	}
	
	~DevIOStatus() {
		AUTO_TRACE("DevIOStatus::~DevIOStatus()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	Management::TSystemStatus  read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOStatus::read()");
		timestamp=getTimeStamp();
		return m_pCore->getStatus();
    }
	
    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOStatus::write()");
	}
    
private:
	CRecvBossCore *m_pCore;
};

#endif /*DEVIOSTATUS_H_*/
