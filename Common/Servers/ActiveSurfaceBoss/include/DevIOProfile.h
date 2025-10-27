#ifndef _ACTIVESURFACEBOSSIMPLDEVIOPROFILE_H_
#define _ACTIVESURFACEBOSSIMPLDEVIOPROFILE_H_

/* ************************************************************************************* */
/* OAC Osservatorio Astronomico di Cagliari                                              */
/* $Id: DevIOProfile.h,v 1.1 2009-05-21 15:33:19 c.migoni Exp $	 */
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
 * This  class is derived from the template DevIO. It is used by the profile property. 
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>,
 * Osservatorio Astronomico di Cagliari, Italia<br>
 */ 
class ActiveSurfaceBossImplDevIOProfile: public virtual DevIO<ActiveSurface::TASProfile>
{
	
public:
	
	ActiveSurfaceBossImplDevIOProfile(IRA::CSecureArea<CActiveSurfaceBossCore>* core): m_core(core) { 
		AUTO_TRACE("ActiveSurfaceBossImplDevIOProfile::ActiveSurfaceBossImplDevIOProfile()");
	}
	
	~ActiveSurfaceBossImplDevIOProfile() {
		AUTO_TRACE("ActiveSurfaceBossImplDevIOProfile::~ActiveSurfaceBossImplDevIOProfile()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	ActiveSurface::TASProfile  read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
		AUTO_TRACE("ActiveSurfaceBossImplDevIOProfile::read()");
		timestamp=getTimeStamp();
		return resource->getProfile();
    }
	
    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("ActiveSurfaceBossImplDevIOProfile::write()");
	}
    
private:
	IRA::CSecureArea<CActiveSurfaceBossCore> *m_core;
};

#endif /*DEVIOPROFILE_H_*/
