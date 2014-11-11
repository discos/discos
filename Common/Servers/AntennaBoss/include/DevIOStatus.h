#ifndef _DEVIOSTATUS_H_
#define _DEVIOSTATUS_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: DevIOStatus.h,v 1.1 2008-05-14 12:39:33 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                  When                       What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  11/05/2008      Creation                                                  */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the  status property. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOStatus: public virtual DevIO<Management::TSystemStatus>
{
	
public:
	
	DevIOStatus(IRA::CSecureArea<CBossCore>* core): m_core(core) { 
		AUTO_TRACE("DevIOStatus::DevIOStatus()");
	}
	
	~DevIOStatus() {
		AUTO_TRACE("DevIOStatus::~DevIOStatus()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	Management::TSystemStatus  read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
		AUTO_TRACE("DevIOStatus::read()");
		timestamp=getTimeStamp();
		return resource->getStatus();

    }
	
    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOStatus::write()");
	}
    
private:
	IRA::CSecureArea<CBossCore> *m_core;
	
};

#endif /*DEVIOSTATUS_H_*/
