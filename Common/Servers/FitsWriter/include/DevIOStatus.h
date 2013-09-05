#ifndef DEVIOSTATUS_H_
#define DEVIOSTATUS_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                  When                       What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  22/01/2009      Creation                                                  */

#include <baciDevIO.h>
#include <IRA>

namespace FitsWriter_private {

/**
 * This  class is derived from the template DevIO. It is used by the  status property. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOStatus: public virtual DevIO<Management::TSystemStatus>
{
	
public:
	
	DevIOStatus(CSecureArea<FitsWriter_private::CDataCollection> * data): m_data(data) { 
		AUTO_TRACE("DevIOStatus::DevIOStatus()");
	}
	
	~DevIOStatus() {
		AUTO_TRACE("DevIOStatus::~DevIOStatus()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	Management::TSystemStatus  read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) 
	{
		AUTO_TRACE("DevIOStatus::read()");
		CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data=m_data->Get();
		timestamp=getTimeStamp();
		return data->getStatus();
    }
	
    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOStatus::write()");
	}
    
private:
	CSecureArea<FitsWriter_private::CDataCollection> *m_data;
};

};

#endif /*DEVIOSTATUS_H_*/
