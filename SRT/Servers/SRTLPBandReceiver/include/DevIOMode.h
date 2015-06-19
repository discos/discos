#ifndef _DEVIOMODE_H_
#define _DEVIOMODE_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  07/04/2010      Creation                                                  */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the by the mode  property.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */
class DevIOMode: public virtual DevIO<ACE_CString>
{
public:

	DevIOMode(CComponentCore* core): m_core(core) {
		AUTO_TRACE("DevIOMode::DevIOMode()");
	}

	~DevIOMode() {
		AUTO_TRACE("DevIOMode::~DevIOMode()");
	}

	bool initializeValue(){
		return false;
	}

	ACE_CString read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOMode::read()");
		timestamp=getTimeStamp();
		return (const char *)m_core->getActualMode();
    }

    void write(const ACE_CString& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOMode::write()");
	}

private:
	CComponentCore *m_core;
};


#endif
