#ifndef _DEVIOCOMPONENTSTATUS_H_
#define _DEVIOCOMPONENTSTATUS_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                  When                       What                          */
/* Andrea Orlati(andrea.orlati@inaf.it) 10/08/2023      Creation                                         */

#include <baciDevIO.h>
#include <IRA>

using namespace IRA;

/**
 * This class is derived from template DevIO and it is used by the componentStatus property
 * @author <a href=mailto:andrea.orlati@inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
*/
class DevIOComponentStatus: public virtual DevIO<Management::TSystemStatus>
{

public:

	DevIOComponentStatus(CComponentCore* core): m_pCore(core) {
		AUTO_TRACE("DevIOComponentStatus::DevIOComponentStatus()");
	}

	~DevIOComponentStatus() {
		AUTO_TRACE("DevIOComponentStatus::~DevIOComponentStatus()");
	}

	bool initializeValue(){
		return false;
	}
	
	/*
	 * @throw ACSErr::ACSbaseExImpl
   */
	Management::TSystemStatus read(ACS::Time& timestamp) {
		m_val=m_pCore->getComponentStatus();
		timestamp=getTimeStamp();  //Completion time
		return m_val;
    }
    
	/*
	 * @throw ACSErr::ACSbaseExImpl
   */
    void write(const CORBA::Long& value, ACS::Time& timestamp) {
		timestamp=getTimeStamp();
		return;
	}

private:
	CComponentCore* m_pCore;
	Management::TSystemStatus m_val;
};

#endif
