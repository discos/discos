#ifndef _DEVIOLAMBDA_H_
#define _DEVIOLAMBDA_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                   When                   What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)    06/11/2013      Creation                                                  */


#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the  waveLength property.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOLambda: public virtual DevIO<CORBA::Double>
{
	
public:
	
	DevIOLambda(IRA::CSecureArea<CBossCore>* core): m_core(core) {
		AUTO_TRACE("DevIOLambda::DevIOLambda()");
	}
	
	~DevIOLambda() {
		AUTO_TRACE("DevIOLambda::~DevIOLambda()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	CORBA::Double  read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
		AUTO_TRACE("DevIOLambda::read()");
		timestamp=getTimeStamp();
		return resource->getWaveLength();
    }
	
    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOLambda::write()");
	}
    
private:
	IRA::CSecureArea<CBossCore> *m_core;
	
};

#endif /*_DEVIOLAMBDA_H_*/
