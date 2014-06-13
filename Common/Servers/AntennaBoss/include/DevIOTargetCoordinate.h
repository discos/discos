#ifndef _DEVIOTARGETCOORDINATE_H_
#define _DEVIOTARGETCOORDINATE_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: DevIOTargetCoordinate.h,v 1.2 2010-09-14 08:41:13 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  07/04/2010      Creation                                                  */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the by the target equatorial coordinates. The associeted property can be selected 
 * at construction time and cannot be changed anymore.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOTargetCoordinate: public virtual DevIO<CORBA::Double>
{
public:
	
	enum TLinkedAttribute {
		RIGHTASCENSION,
		DECLINATION,
		VRAD,
		FLUX
	};
	
	DevIOTargetCoordinate(IRA::CSecureArea<CBossCore>* core,const TLinkedAttribute& e): m_core(core), m_eq(e) { 
		AUTO_TRACE("DevIOTargetCoordinate::DevIOTargetCoordinate()");
	}
	
	~DevIOTargetCoordinate() {
		AUTO_TRACE("DevIOTargetCoordinate::~DevIOTargetCoordinate()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	CORBA::Double read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
		AUTO_TRACE("DevIOTargetCoordinate::read()");
		double val=0.0;
		if (m_eq==RIGHTASCENSION) {
			val=resource->getTargetRightAscension();
		}
		else if (m_eq==DECLINATION) {
			val=resource->getTargetDeclination();
		}
		else if (m_eq==VRAD) {
			val=resource->getTargetVrad();
		}
		else if (m_eq==FLUX) {
			val=resource->getTargetFlux();
		}
		timestamp=getTimeStamp();
		return (CORBA::Double)val;
    }
	
    void write(const CORBA::Double& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOTargetCoordinate::write()");
	}
    
private:
	IRA::CSecureArea<CBossCore> *m_core;
	TLinkedAttribute m_eq;
};


#endif 
