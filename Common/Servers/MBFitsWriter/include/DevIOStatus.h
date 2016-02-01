#ifndef DEVIOSTATUS_H_
#define DEVIOSTATUS_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: DevIOStatus.h,v 1.3 2009-01-29 21:20:27 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                  When                       What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  22/01/2009      Creation                                                  */

#include <baciDevIO.h>
#include <IRA>

namespace MBFitsWriter_private {

/**
 * This  class is derived from the template DevIO. It is used by the  status property.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */
class DevIOStatus: public virtual DevIO<Management::TSystemStatus>
{

public:

	DevIOStatus(CSecureArea<MBFitsWriter_private::CDataCollection> * data): m_data(data) {
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
		CSecAreaResourceWrapper<MBFitsWriter_private::CDataCollection> data=m_data->Get();
		timestamp=getTimeStamp();
		return data->getStatus();
    }

    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOStatus::write()");
	}

private:
	CSecureArea<MBFitsWriter_private::CDataCollection> *m_data;
};

};

#endif /*DEVIOSTATUS_H_*/
