#ifndef _DEVIORAWHORIZONTAL_H_
#define _DEVIORAWHORIZONTAL_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: DevIORawHorizontal.h,v 1.2 2008-05-21 14:20:10 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                             when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  081/04/2008      Creation                                                  */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the by the Raw Horizontal coordinates. The associeted property can be selected 
 * at construction time and cannot be changed anymore.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIORawHorizontal: public virtual DevIO<CORBA::Double>
{
public:
	
	enum THorizontal {
		AZIMUTH,
		ELEVATION
	};
	
	DevIORawHorizontal(IRA::CSecureArea<CBossCore>* core,const THorizontal& h): m_core(core), m_horiz(h) { 
		AUTO_TRACE("DevIORawHorizontal::DevIORawHorizontal()");
	}
	
	~DevIORawHorizontal() {
		AUTO_TRACE("DevIORawHorizontal::~DevIORawHorizontal()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	CORBA::Double read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
		AUTO_TRACE("DevIORawHorizontal::read()");
		double az=0.0;
		double el=0.0;
		CORBA::Double val=0.0;
		TIMEVALUE now;
		IRA::CIRATools::getTime(now);
		resource->getRawHorizontal(now,az,el);
		timestamp=getTimeStamp();
		if (m_horiz==AZIMUTH) {
			val= (CORBA::Double)az;
		}
		else if (m_horiz==ELEVATION) {
			val=(CORBA::Double)el;
		}
		return val;
    }
	
    void write(const CORBA::Double& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIORawHorizontal::write()");
	}
    
private:
	IRA::CSecureArea<CBossCore> *m_core;
	THorizontal m_horiz;
};



#endif /*DEVIORAWHORIZONTAL_H_*/
