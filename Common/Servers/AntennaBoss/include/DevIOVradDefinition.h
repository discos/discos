#ifndef DEVIOVRADDEFINITION_H_
#define DEVIOVRADDEFINITION_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                  When                       What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  14/04/2014      Creation                                                  */

#include <baciDevIO.h>

/**
 * This  class is derived from the template DevIO. It is used by the  VradDefinition property.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */
class DevIOVradDefinition: public virtual DevIO<Antenna::TVradDefinition>
{

public:

	DevIOVradDefinition(IRA::CSecureArea<CBossCore>* core): m_core(core) {
		AUTO_TRACE("DevIOVradDefinition::DevIOVradDefinition()");
	}

	~DevIOVradDefinition() {
		AUTO_TRACE("DevIOVradDefinition::~DevIOVradDefinition()");
	}

	bool initializeValue(){
		return false;
	}

	Antenna::TVradDefinition  read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
		AUTO_TRACE("DevIOVradDefinition::read()");
		timestamp=getTimeStamp();
		return resource->getVradDefinition();
    }

    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOVradDefinition::write()");
	}

private:
	IRA::CSecureArea<CBossCore> *m_core;

};

#endif /*DEVIOVRADDEFINITION_H_*/
