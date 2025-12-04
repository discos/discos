#ifndef _NOTOACTIVESURFACEBOSSIMPLDEVIOSTATUS_H_
#define _NOTOACTIVESURFACEBOSSIMPLDEVIOSTATUS_H_

/* ************************************************************************************* */
/* OAC Osservatorio Astronomico di Cagliari                                              */
/* $Id: DevIOStatus.h,v 1.1 2009-05-21 15:33:19 c.migoni Exp $	 */
/*                                                                                       */
/* This code is under GNU General Public Licence (GPL).                                  */
/*                                                                                       */
/* Who                               When       What                                     */
/* Carlo Migoni (migoni@ca.astro.it) 11/05/2008 Creation                                 */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the  status property. 
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>,
 * Osservatorio Astronomico di Cagliari, Italia<br>
 */ 
class NotoActiveSurfaceBossImplDevIOStatus: public virtual DevIO<Management::TSystemStatus>
{
	
public:
	
	NotoActiveSurfaceBossImplDevIOStatus(IRA::CSecureArea<CNotoActiveSurfaceBossCore>* core): m_core(core) { 
		AUTO_TRACE("NotoActiveSurfaceBossImplDevIOStatus::NotoActiveSurfaceBossImplDevIOStatus()");
	}
	
	~NotoActiveSurfaceBossImplDevIOStatus() {
		AUTO_TRACE("NotoActiveSurfaceBossImplDevIOStatus::~NotoActiveSurfaceBossImplDevIOStatus()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	Management::TSystemStatus  read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
		AUTO_TRACE("NotoActiveSurfaceBossImplDevIOStatus::read()");
		timestamp=getTimeStamp();
		return resource->getStatus();

    }
	
    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("NotoActiveSurfaceBossImplDevIOStatus::write()");
	}
    
private:
	IRA::CSecureArea<CNotoActiveSurfaceBossCore> *m_core;
	
};

#endif /*DEVIOSTATUS_H_*/
