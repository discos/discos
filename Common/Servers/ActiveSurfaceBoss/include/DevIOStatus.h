#ifndef _ACTIVESURFACEBOSSIMPLDEVIOSTATUS_H_
#define _ACTIVESURFACEBOSSIMPLDEVIOSTATUS_H_

/* ************************************************************************************* */
/* OAC Osservatorio Astronomico di Cagliari                                              */
/* $Id: DevIOStatus.h,v 1.1 2009-05-21 15:33:19 c.migoni Exp $	 */
/*                                                                                       */
/* This code is under GNU General Public Licence (GPL).                                  */
/*                                                                                       */
/* Who                               When       What                                     */
/* Carlo Migoni (migoni@ca.astro.it) 11/05/2008 Creation                                 */
/* Giuseppe Carboni (giuseppe.carboni@inaf.it)  04/09/2025  Code generalization          */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the  status property. 
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>,
 * Osservatorio Astronomico di Cagliari, Italia<br>
 */ 
class ActiveSurfaceBossImplDevIOStatus: public virtual DevIO<Management::TSystemStatus>
{
	
public:
	
	ActiveSurfaceBossImplDevIOStatus(IRA::CSecureArea<CActiveSurfaceBossCore>* core): m_core(core) { 
		AUTO_TRACE("ActiveSurfaceBossImplDevIOStatus::ActiveSurfaceBossImplDevIOStatus()");
	}
	
	~ActiveSurfaceBossImplDevIOStatus() {
		AUTO_TRACE("ActiveSurfaceBossImplDevIOStatus::~ActiveSurfaceBossImplDevIOStatus()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	Management::TSystemStatus  read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
		AUTO_TRACE("ActiveSurfaceBossImplDevIOStatus::read()");
		timestamp=getTimeStamp();
		return resource->getStatus();

    }
	
    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("ActiveSurfaceBossImplDevIOStatus::write()");
	}
    
private:
	IRA::CSecureArea<CActiveSurfaceBossCore> *m_core;
	
};

#endif /*DEVIOSTATUS_H_*/
