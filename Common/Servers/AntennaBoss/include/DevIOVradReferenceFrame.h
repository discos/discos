#ifndef DEVIOVRADREFERENCEFRAME_H_
#define DEVIOVRADREFERENCEFRAME_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                  When                       What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  14/04/2014      Creation                                                  */

#include <baciDevIO.h>

/**
 * This  class is derived from the template DevIO. It is used by the VradReferenceFrame property.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */
class DevIOVradReferenceFrame: public virtual DevIO<Antenna::TReferenceFrame>
{

public:

	DevIOVradReferenceFrame(IRA::CSecureArea<CBossCore>* core): m_core(core) {
		AUTO_TRACE("DevIOVradReferenceFrame::DevIOVradReferenceFrame()");
	}

	~DevIOVradReferenceFrame() {
		AUTO_TRACE("DevIOVradReferenceFrame::~DevIOVradReferenceFrame()");
	}

	bool initializeValue(){
		return false;
	}

	Antenna::TReferenceFrame  read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
		AUTO_TRACE("DevIOVradReferenceFrame::read()");
		timestamp=getTimeStamp();
		return resource->getReferenceFrame();
    }

    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOVradReferenceFrame::write()");
	}

private:
	IRA::CSecureArea<CBossCore> *m_core;

};

#endif /*DEVIOVRADREFERENCEFRAME_H_*/

