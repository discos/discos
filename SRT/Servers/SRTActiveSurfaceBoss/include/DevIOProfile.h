#ifndef _SRTACTIVESURFACEBOSSIMPLDEVIOPROFILE_H_
#define _SRTACTIVESURFACEBOSSIMPLDEVIOPROFILE_H_

/* ************************************************************************************* */
/* OAC Osservatorio Astronomico di Cagliari                                              */
/* $Id: DevIOProfile.h,v 1.1 2009-05-21 15:33:19 c.migoni Exp $	 */
/*                                                                                       */
/* This code is under GNU General Public Licence (GPL).                                  */
/*                                                                                       */
/* Who                               When       What                                     */
/* Carlo Migoni (migoni@ca.astro.it) 11/05/2008 Creation                                 */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the profile property. 
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>,
 * Osservatorio Astronomico di Cagliari, Italia<br>
 */ 
class SRTActiveSurfaceBossImplDevIOProfile: public virtual DevIO<ActiveSurface::TASProfile>
{
	
public:
	
	SRTActiveSurfaceBossImplDevIOProfile(IRA::CSecureArea<CSRTActiveSurfaceBossCore>* core): m_core(core) { 
		AUTO_TRACE("SRTActiveSurfaceBossImplDevIOProfile::SRTActiveSurfaceBossImplDevIOProfile()");
	}
	
	~SRTActiveSurfaceBossImplDevIOProfile() {
		AUTO_TRACE("SRTActiveSurfaceBossImplDevIOProfile::~SRTActiveSurfaceBossImplDevIOProfile()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	ActiveSurface::TASProfile  read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
		AUTO_TRACE("SRTActiveSurfaceBossImplDevIOProfile::read()");
		timestamp=getTimeStamp();
		return resource->getProfile();
    }
	
    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("SRTActiveSurfaceBossImplDevIOProfile::write()");
	}
    
private:
	IRA::CSecureArea<CSRTActiveSurfaceBossCore> *m_core;
};

#endif /*DEVIOPROFILE_H_*/
