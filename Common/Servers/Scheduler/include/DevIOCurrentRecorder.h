#ifndef DEVIOCURRENTRECORDER_H_
#define DEVIOCURRENTRECORDER_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            	when                    What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it) 09/01/2013      Creation                                                  */

#include <baciDevIO.h>


/**
 * This  class is derived from the template DevIO. It is used by the by the currentRecorder property.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOCurrentRecorder: public virtual DevIO<ACE_CString>
{
public:
	
	DevIOCurrentRecorder(CCore* core): m_core(core) {
		AUTO_TRACE("DevIOCurrentRecorder::DevIOCurrentRecorder()");
	}
	
	~DevIOCurrentRecorder() {
		AUTO_TRACE("DevIOCurrentRecorder::~DevIOCurrentRecorder()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	ACE_CString read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOCurrentRecorder::read()");
		IRA::CString dr;
		timestamp=getTimeStamp();
		m_core->getCurrentDataReceiver(dr);
		m_val=(const char *)dr;
		return m_val;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOCurrentRecorder::write()");
	}
    
private:
	CCore *m_core;
	ACE_CString m_val;
};



#endif /*DEVIOCURRENTRECORDER_H_*/
