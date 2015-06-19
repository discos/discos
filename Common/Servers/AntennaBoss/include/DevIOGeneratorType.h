#ifndef _DEVIOGENERATORTYPE_
#define _DEVIOGENERATORTYPE_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: DevIOGeneratorType.h,v 1.1 2008-02-22 16:42:58 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  18/02/2008      Creation                                                  */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the generatorType property to report its value. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOGeneratorType: public virtual DevIO<Antenna::TGeneratorType>
{
public:
	
	DevIOGeneratorType(IRA::CSecureArea<CBossCore>* core): m_core(core) { 
		AUTO_TRACE("DevIOGeneratorType::DevIOGeneratorType()");
	}
	
	~DevIOGeneratorType() {
		AUTO_TRACE("DevIOGeneratorType::~DevIOGeneratorType()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	Antenna::TGeneratorType read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
		AUTO_TRACE("DevIOGeneratorType::read()");
		timestamp=getTimeStamp();
		return resource->getGeneratorType();
    }
	
    void write(const Antenna::TGeneratorType& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOGeneratorType::write()");
	}
    
private:
	IRA::CSecureArea<CBossCore> *m_core;
};


#endif /*DEVIOGENERATORTYPE_*/
