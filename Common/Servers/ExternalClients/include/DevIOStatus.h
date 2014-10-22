#ifndef DEVIOSTATUS_H_
#define DEVIOSTATUS_H_

/** **************************************************************************/
/* OAC - Osservatorio Astronomico di Cagliari                                */
/* $Id: DevIOStatus.h,v 1.1 2010-01-20 10:48:11 c.migoni Exp $               */
/*                                                                           */
/* This code is under GNU General Public Licence (GPL).                      */
/*                                                                           */
/* Who              when            What                                     */
/* Carlo Migoni     21/12/2009      Creation                                 */

#include <baciDevIO.h>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the by the status property. 
 * @author <a href=mailto:c.migoni@ca.astro.it>Carlo Migoni</a>,
 * Osservatorio Astronomico di Cagliari, Italia<br>
 */ 
class DevIOStatus: public virtual DevIO<Management::TSystemStatus>
{
public:
	
	DevIOStatus(CExternalClientsSocketServer* core): m_core(core) { 
		AUTO_TRACE("DevIOStatus::DevIOStatus()");
	}
	
	~DevIOStatus() {
		AUTO_TRACE("DevIOStatus::~DevIOStatus()");
	}
	
	bool initializeValue() {
		return false;
	}
	
	Management::TSystemStatus read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOStatus::read()");
		timestamp=getTimeStamp();
		//m_val=m_core->getStatus();
        m_status=m_core->getStatus();
        if (m_status == CExternalClientsSocketServer::ExternalClientSocketStatus_NOTCNTD)
            m_val = Management::MNG_FAILURE;
        else if (m_status == CExternalClientsSocketServer::ExternalClientSocketStatus_CNTDING || m_status == CExternalClientsSocketServer::ExternalClientSocketStatus_BSY)
            m_val = Management::MNG_WARNING;
        else
            m_val = Management::MNG_OK;

		return m_val;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOStatus::write()");
	}
    
private:
	CExternalClientsSocketServer *m_core;
	Management::TSystemStatus m_val;
    CExternalClientsSocketServer::ExternalClientSocketStatus m_status;
};





#endif /*DEVIOSTATUS_H_*/
