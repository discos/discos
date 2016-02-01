#ifndef _DEVIOOBSERVEDGALACTIC_H_
#define _DEVIOOBSERVEDGALACTIC_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: DevIOObservedGalactic.h,v 1.2 2008-02-22 16:42:58 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  15/02/2008      Creation                                                  */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the by the Observed Galactic coordinates. The associeted property can be selected 
 * at construction time and cannot be changed anymore.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOObservedGalactic: public virtual DevIO<CORBA::Double>
{
public:
	
	enum TGalactic {
		LONGITUDE,
		LATITUDE
	};
	
	DevIOObservedGalactic(IRA::CSecureArea<CBossCore>* core,const TGalactic& e): m_core(core), m_gal(e) { 
		AUTO_TRACE("DevIOObservedGalactic::DevIOObservedGalactic()");
	}
	
	~DevIOObservedGalactic() {
		AUTO_TRACE("DevIOObservedGalactic::~DevIOObservedGalactic()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	CORBA::Double read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOObservedGalactic::read()");
		CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
		double val=0.0;
		if (m_gal==LONGITUDE) {
			val=resource->getObservedGalacticLongitude();
		}
		else if (m_gal==LATITUDE) {
			val=resource->getObservedGalacticLatitude();
		}
		timestamp=getTimeStamp();
		return (CORBA::Double)val;
    }
	
    void write(const CORBA::Double& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOObservedGalactic::write()");
	}
    
private:
	IRA::CSecureArea<CBossCore> *m_core;
	TGalactic m_gal;
};



#endif /*DEVIOOBSERVEDGALACTIC_H_*/
