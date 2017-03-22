#ifndef _DEVIOOBSERVEDHORIZONTAL_H_
#define _DEVIOOBSERVEDHORIZONTAL_H_


/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: DevIOObservedHorizontal.h,v 1.2 2008-02-22 16:42:59 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  01/02/2008      Creation                                                  */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the by the Observed Horizontal coordinates. The associeted property can be selected 
 * at construction time and cannot be changed anymore.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOObservedHorizontal: public virtual DevIO<CORBA::Double>
{
public:
	
	enum THorizontal {
		AZIMUTH,
		ELEVATION
	};
	
	DevIOObservedHorizontal(IRA::CSecureArea<CBossCore>* core,const THorizontal& h): m_core(core), m_horiz(h) { 
		AUTO_TRACE("DevIOObservedHorizontal::DevIOObservedHorizontal()");
	}
	
	~DevIOObservedHorizontal() {
		AUTO_TRACE("DevIOObservedHorizontal::~DevIOObservedHorizontal()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	CORBA::Double read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
		AUTO_TRACE("DevIOObservedHorizontal::read()");
		double val=0.0;
		if (m_horiz==AZIMUTH) {
			val=resource->getObservedHorizontalAzimuth();
		}
		else if (m_horiz==ELEVATION) {
			val=resource->getObservedHorizontalElevation();
		}
		timestamp=getTimeStamp();
		return (CORBA::Double)val;
    }
	
    void write(const CORBA::Double& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOObservedHorizontal::write()");
	}
    
private:
	IRA::CSecureArea<CBossCore> *m_core;
	THorizontal m_horiz;
};

#endif /*DEVIOOBSERVEDHORIZONTAL_H_*/
