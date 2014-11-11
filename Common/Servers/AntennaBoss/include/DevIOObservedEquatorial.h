#ifndef _DEVIOOBSERVEDEQUATORIAL_H_
#define _DEVIOOBSERVEDEQUATORIAL_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: DevIOObservedEquatorial.h,v 1.2 2008-02-22 16:42:58 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  07/02/2008      Creation                                                  */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the by the Observed Equatorial  coordinates. The associeted property can be selected 
 * at construction time and cannot be changed anymore.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOObservedEquatorial: public virtual DevIO<CORBA::Double>
{
public:
	
	enum TEquatorial {
		RIGHTASCENSION,
		DECLINATION
	};
	
	DevIOObservedEquatorial(IRA::CSecureArea<CBossCore>* core,const TEquatorial& e): m_core(core), m_eq(e) { 
		AUTO_TRACE("DevIOObservedEquatorial::DevIOObservedEquatorial()");
	}
	
	~DevIOObservedEquatorial() {
		AUTO_TRACE("DevIOObservedEquatorial::~DevIOObservedEquatorial()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	CORBA::Double read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
		AUTO_TRACE("DevIOObservedEquatorial::read()");
		double val=0.0;
		if (m_eq==RIGHTASCENSION) {
			val=resource->getObservedEquatorialRightAscension();
		}
		else if (m_eq==DECLINATION) {
			val=resource->getObservedEquatorialDeclination();
		}
		timestamp=getTimeStamp();
		return (CORBA::Double)val;
    }
	
    void write(const CORBA::Double& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOObservedEquatorial::write()");
	}
    
private:
	IRA::CSecureArea<CBossCore> *m_core;
	TEquatorial m_eq;
};


#endif /*DEVIOOBSERVEDEQUATORIAL_H_*/
