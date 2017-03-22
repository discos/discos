#ifndef DEVIOFILENAME_H_
#define DEVIOFILENAME_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: DevIOFileName.h,v 1.2 2010/06/21 11:04:51 bliliana Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                         	when                    What                                                       */
/* Liliana Branciforti(bliliana@arcetri.astro.it) 04/08/2009 		Creation								*/

#include <baciDevIO.h>
#include <IRA>
#include "CommandLine.h"

using namespace IRA;

/**
 * This  class is derived from the template DevIO. It is used by the by the fileName property. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOFileName: public virtual DevIO<ACE_CString>
{
public:
	
	DevIOFileName(CSecureArea<CCommandLine>*data): m_data(data) { 
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
		CSecAreaResourceWrapper<CCommandLine> data=m_data->Get();
		timestamp=getTimeStamp();
		m_val=(const char *)data->getFileName();
		return m_val;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOFileName::write()");
	}
    
private:
	CSecureArea<CCommandLine> *m_data;
	ACE_CString m_val;
};


#endif /*DEVIOFILENAME_H_*/
