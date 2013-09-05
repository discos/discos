#ifndef DEVIOFILENAME_H_
#define DEVIOFILENAME_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            	when                    What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it) 22/01/2009      Creation                                                  */

#include <baciDevIO.h>

namespace FitsWriter_private {

/**
 * This  class is derived from the template DevIO. It is used by the by the fileName property. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOFileName: public virtual DevIO<ACE_CString>
{
public:
	
	DevIOFileName(CSecureArea<FitsWriter_private::CDataCollection>*data): m_data(data) { 
		AUTO_TRACE("DevIOFileName::DevIOFileName()");
	}
	
	~DevIOFileName() {
		AUTO_TRACE("DevIOFileName::~DevIOFileName()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	ACE_CString read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		AUTO_TRACE("DevIOFileName::read()");
		CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data=m_data->Get();
		timestamp=getTimeStamp();
		m_val=(const char *)data->getFileName();
		return m_val;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOFileName::write()");
	}
    
private:
	CSecureArea<FitsWriter_private::CDataCollection> *m_data;
	ACE_CString m_val;
};

};

#endif /*DEVIOFILENAME_H_*/
