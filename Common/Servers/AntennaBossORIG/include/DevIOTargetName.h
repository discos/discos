#ifndef _DEVIOTARGETNAME_H_
#define _DEVIOTARGETNAME_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: DevIOTargetName.h,v 1.1 2010-04-08 13:10:36 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  07/04/2010      Creation                                                  */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the by the target property. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOTargetName: public virtual DevIO<ACE_CString>
{
public:
		
	DevIOTargetName(IRA::CSecureArea<CBossCore>* core): m_core(core) { 
		AUTO_TRACE("DevIOTargetName::DevIOTargetName()");
	}
	
	~DevIOTargetName() {
		AUTO_TRACE("DevIOTargetName::~DevIOTargetName()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	ACE_CString read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
		AUTO_TRACE("DevIOTargetName::read()");
		timestamp=getTimeStamp();
		return (const char *)resource->getTargetName();
    }
	
    void write(const ACE_CString& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOTargetName::write()");
	}
    
private:
	IRA::CSecureArea<CBossCore> *m_core;
};


#endif 
