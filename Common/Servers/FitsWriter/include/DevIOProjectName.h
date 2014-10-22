#ifndef DEVIOPROJECTNAME_H_
#define DEVIOPROJECTNAME_H_

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
 * This  class is derived from the template DevIO. It is used by the by the projectName property.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */
class DevIOProjectName: public virtual DevIO<ACE_CString>
{
public:

	DevIOProjectName(FitsWriter_private::CDataCollection *data): m_data(data) {
		AUTO_TRACE("DevIOProjectName::DevIOProjectName()");
	}

	~DevIOProjectName() {
		AUTO_TRACE("DevIOProjectName::~DevIOProjectName()");
	}

	bool initializeValue(){
		return false;
	}

	ACE_CString read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		AUTO_TRACE("DevIOProjectName::read()");
		//CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data=m_data->Get();
		timestamp=getTimeStamp();
		m_val=(const char *)m_data->getProjectName();
		return m_val;
    }

    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOProjectName::write()");
	}

private:
	//CSecureArea<FitsWriter_private::CDataCollection> *m_data;
	FitsWriter_private::CDataCollection *m_data;
	ACE_CString m_val;
};

};

#endif
