#ifndef CURRENTDEVICE_H_
#define CURRENTDEVICE_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  04/11/2011      Creation                                                  */

#include <baciDevIO.h>

/**
 * This  class is derived from the template DevIO. It is used by the by the currentDevice  property.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */
class DevIOCurrentDevice: public virtual DevIO<CORBA::Long>
{
public:

	DevIOCurrentDevice(CCore* core): m_core(core) {
		AUTO_TRACE("DevIOScanNumber::DevIOScanNumber()");
	}

	~DevIOCurrentDevice() {
		AUTO_TRACE("DevIOCurrentDevice::~DevIOCurrentDevice()");
	}

	bool initializeValue(){
		return false;
	}

	CORBA::Long read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOCurrentDevice::read()");
		m_val=m_core->getCurrentDevice();
		timestamp=getTimeStamp();
		return m_val;
    }

    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOCurrentDevice::write()");
	}

private:
	CCore *m_core;
	CORBA::Long m_val;
};

#endif /*CURRENTDEVICE_H_*/
