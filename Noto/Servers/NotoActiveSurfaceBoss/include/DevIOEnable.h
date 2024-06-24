#ifndef _NOTOACTIVESURFACEBOSSIMPLDEVIOENABLE_H_
#define _NOTOACTIVESURFACEBOSSIMPLDEVIOENABLE_H_

/* ************************************************************************************* */
/* OAC Osservatorio Astronomico di Cagliari                                              */
/* $Id: DevIOEnable.h,v 1.2 2009-05-25 07:45:32 c.migoni Exp $                       	 */
/*                                                                                       */
/* This code is under GNU General Public Licence (GPL).                                  */
/*                                                                                       */
/* Who                                  when        What                                 */
/* Carlo Migoni (migoni@ca.astro.it)    16/03/2009  Creation                             */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. 
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>,
 * Osservatorio Astronomico di Cagliari, Italia<br>
 */ 
class NotoActiveSurfaceBossImplDevIOEnable: public virtual DevIO<Management::TBoolean>
{
public:
	
	NotoActiveSurfaceBossImplDevIOEnable(IRA::CSecureArea<CNotoActiveSurfaceBossCore>* core): m_core(core) { 
		AUTO_TRACE("NotoActiveSurfaceBossImplDevIOEnable::NotoActiveSurfaceBossImplDevIOEnable()");
	}
	
	~NotoActiveSurfaceBossImplDevIOEnable() {
		AUTO_TRACE("NotoActiveSurfaceBossImplDevIOEnable::~NotoActiveSurfaceBossImplDevIOEnable()");
	}
	
	bool initializeValue() 
	{
		return false;
	}
	
	Management::TBoolean  read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		bool val;
		CSecAreaResourceWrapper<CNotoActiveSurfaceBossCore> resource=m_core->Get();
		AUTO_TRACE("NotoActiveSurfaceBossImplDevIOEnable::read()");
		timestamp=getTimeStamp();
		val=resource->getEnable();
		if (val) {
			return Management::MNG_TRUE;
		}
		else {
			return Management::MNG_FALSE;
		}
    }
	
    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("NotoActiveSurfaceBossImplDevIOEnable::write()");
	}
    
private:
	IRA::CSecureArea<CNotoActiveSurfaceBossCore> *m_core;
};

#endif /*DEVIOENABLE_H_*/
