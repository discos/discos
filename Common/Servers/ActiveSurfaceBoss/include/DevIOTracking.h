#ifndef _ACTIVESURFACEBOSSIMPLDEVIOTRACKING_H_
#define _ACTIVESURFACEBOSSIMPLDEVIOTRACKING_H_

/* ************************************************************************************* */
/* OAC Osservatorio Astronomico di Cagliari                                              */
/* $Id: DevIOTracking.h,v 1.2 2009-05-25 07:45:32 c.migoni Exp $                       	 */
/*                                                                                       */
/* This code is under GNU General Public Licence (GPL).                                  */
/*                                                                                       */
/* Who                                  when        What                                 */
/* Carlo Migoni (migoni@ca.astro.it)    16/03/2009  Creation                             */
/* Giuseppe Carboni (giuseppe.carboni@inaf.it)  04/09/2025  Code generalization          */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. 
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>,
 * Osservatorio Astronomico di Cagliari, Italia<br>
 */ 
class ActiveSurfaceBossImplDevIOTracking: public virtual DevIO<Management::TBoolean>
{
public:
	
	ActiveSurfaceBossImplDevIOTracking(IRA::CSecureArea<CActiveSurfaceBossCore>* core): m_core(core) { 
		AUTO_TRACE("ActiveSurfaceBossImplDevIOTracking::ActiveSurfaceBossImplDevIOTracking()");
	}
	
	~ActiveSurfaceBossImplDevIOTracking() {
		AUTO_TRACE("ActiveSurfaceBossImplDevIOTracking::~ActiveSurfaceBossImplDevIOTracking()");
	}
	
	bool initializeValue() 
	{
		return false;
	}
	
	Management::TBoolean  read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		bool val;
		CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
		AUTO_TRACE("ActiveSurfaceBossImplDevIOTracking::read()");
		timestamp=getTimeStamp();
		val=resource->getTracking();
		if (val) {
			return Management::MNG_TRUE;
		}
		else {
			return Management::MNG_FALSE;
		}
    }
	
    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("ActiveSurfaceBossImplDevIOTracking::write()");
	}
    
private:
	IRA::CSecureArea<CActiveSurfaceBossCore> *m_core;
};

#endif /*DEVIOTRACKING_H_*/
