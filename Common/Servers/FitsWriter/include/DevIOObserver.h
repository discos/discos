#ifndef DEVIOOBSERVER_H_
#define DEVIOOBSERVER_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            	when                    What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it) 03/11/2011      Creation                                                  */

#include <baciDevIO.h>

namespace FitsWriter_private {

/**
 * This  class is derived from the template DevIO. It is used by the by the observer property.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */
class DevIOObserver: public virtual DevIO<ACE_CString>
{
public:

	DevIOObserver(CSecureArea<FitsWriter_private::CDataCollection>*data): m_data(data) {
		AUTO_TRACE("DevIOObserver::DevIOObserver()");
	}

	~DevIOObserver() {
		AUTO_TRACE("DevIOObserver::~DevIOObserver()");
	}

	bool initializeValue(){
		return false;
	}

	ACE_CString read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		AUTO_TRACE("DevIOObserver::read()");
		CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data=m_data->Get();
		timestamp=getTimeStamp();
		m_val=(const char *)data->getObserverName();
		return m_val;
    }

    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOObserver::write()");
	}

private:
	CSecureArea<FitsWriter_private::CDataCollection> *m_data;
	ACE_CString m_val;
};

};

#endif /*DEVIOOBSERVER_H_*/
