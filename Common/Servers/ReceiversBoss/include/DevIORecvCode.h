#ifndef _DEVIORECVCODE_H_
#define _DEVIORECVCODE_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: DevIORecvCode.h,v 1.1 2010-07-22 12:24:47 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                 when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  22/07/2010      Creation                                                  */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the by the Receiver Code property. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIORecvCode: public virtual DevIO<ACE_CString>
{
public:
		
	DevIORecvCode(CRecvBossCore* core): m_pCore(core) { 
		AUTO_TRACE("DevIORecvCode::DevIORecvCode()");
	}
	
	~DevIORecvCode() {
		AUTO_TRACE("DevIORecvCode::~DevIORecvCode()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	ACE_CString read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIORecvCode::read()");
		timestamp=getTimeStamp();
		return (const char *)m_pCore->getRecvCode();
    }
	
    void write(const ACE_CString& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIORecvCode::write()");
	}
    
private:
	CRecvBossCore *m_pCore;
};


#endif 
