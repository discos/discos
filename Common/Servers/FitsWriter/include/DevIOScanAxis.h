#ifndef DEVIOSCANAXIS_H_
#define DEVIOSCANAXIS_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                  When                       What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  03/11/2011      Creation                                                  */

#include <baciDevIO.h>
#include <IRA>

namespace FitsWriter_private {

/**
 * This  class is derived from the template DevIO. It is used by the  scanAxis property.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */
class DevIOScanAxis: public virtual DevIO<Management::TScanAxis>
{

public:

	DevIOScanAxis(CSecureArea<FitsWriter_private::CDataCollection> * data): m_data(data) {
		AUTO_TRACE("DevIOScanAxis::DevIOScanAxis()");
	}

	~DevIOScanAxis() {
		AUTO_TRACE("DevIOScanAxis::~DevIOScanAxis()");
	}

	bool initializeValue(){
		return false;
	}

	Management::TScanAxis  read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		AUTO_TRACE("DevIOScanAxis::read()");
		CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data=m_data->Get();
		timestamp=getTimeStamp();
		return data->getScanAxis();
    }

    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOScanAxis::write()");
	}

private:
	CSecureArea<FitsWriter_private::CDataCollection> *m_data;
};

};

#endif /*DEVIOSCANAXIS_H_*/
