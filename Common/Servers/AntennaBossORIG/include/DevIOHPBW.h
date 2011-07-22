#ifndef _DEVIOHPBW_H_
#define _DEVIOHPBW_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: DevIOHPBW.h,v 1.2 2010-09-14 08:41:13 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                   When                   What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)    21/05/2007      Creation                                                  */


#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the  status property. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOHPBW: public virtual DevIO<CORBA::Double>
{
	
public:
	
	DevIOHPBW(IRA::CSecureArea<CBossCore>* core): m_core(core) { 
		AUTO_TRACE("DevIOHPBW::DevIOHPBW()");
	}
	
	~DevIOHPBW() {
		AUTO_TRACE("DevIOHPBW::~DevIOHPBW()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	CORBA::Double  read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
		AUTO_TRACE("DevIOHPBW::read()");
		timestamp=getTimeStamp();
		return resource->getBWHM();
    }
	
    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOHPBW::write()");
	}
    
private:
	IRA::CSecureArea<CBossCore> *m_core;
	
};

#endif /*DEVIOHPBW_H_*/
