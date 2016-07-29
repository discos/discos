#ifndef DEVIOOFFFRAME_H_
#define DEVIOOFFFRAME_H_


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
class DevIOOffFrame: public virtual DevIO<Antenna::TCoordinateFrame>
{

public:

	DevIOOffFrame(IRA::CSecureArea<CBossCore>* core): m_core(core) {
		AUTO_TRACE("DevIOOffFrame::DevIOOffFrame()");
	}

	~DevIOOffFrame() {
		AUTO_TRACE("DevIOOffFrame::~DevIOOffFrame()");
	}

	bool initializeValue(){
		return false;
	}

	Antenna::TCoordinateFrame read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
		AUTO_TRACE("DevIOOffFrame::read()");
		timestamp=getTimeStamp();
		return resource->getSubScanOffFrame();
    }

    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOOffFrame::write()");
	}

private:
	IRA::CSecureArea<CBossCore> *m_core;

};


#endif /*DEVIOOFFFRAME_H_*/
