#ifndef _DEVIOENABLE_H_
#define _DEVIOENABLE_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: DevIOEnable.h,v 1.3 2008-05-21 14:20:10 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  19/02/2008      Creation                                                  */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the  enabled (<i>COMPONENT</i> property and the
 * correctionEnabled (<i>CRRECTION</i>) property.. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOEnable: public virtual DevIO<Management::TBoolean>
{
public:
	
	enum TEnabling {
		COMPONENT,
		CORRECTION
	};
	
	
	DevIOEnable(IRA::CSecureArea<CBossCore>* core,const TEnabling& en): m_core(core), m_en(en) { 
		AUTO_TRACE("DevIOEnable::DevIOEnable()");
	}
	
	~DevIOEnable() {
		AUTO_TRACE("DevIOEnable::~DevIOEnable()");
	}
	
	bool initializeValue() 
	{
		return false;
	}
	
	Management::TBoolean  read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		bool val;
		CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
		AUTO_TRACE("DevIOEnable::read()");
		timestamp=getTimeStamp();
		if (m_en==COMPONENT) {
			val=resource->getEnable();
		}
		else { //CORRECTION
			val=resource->getCorrectionEnable();
		}
		if (val) {
			return Management::MNG_TRUE;
		}
		else {
			return Management::MNG_FALSE;
		}
    }
	
    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOEnable::write()");
	}
    
private:
	IRA::CSecureArea<CBossCore> *m_core;
	TEnabling m_en;
};

#endif /*DEVIOENABLE_H_*/
