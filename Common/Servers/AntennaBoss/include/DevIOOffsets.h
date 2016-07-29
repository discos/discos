#ifndef DEVIOOFFSETS_H_
#define DEVIOOFFSETS_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: DevIOOffsets.h,v 1.1 2010-09-14 08:41:13 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                 when             What                                                     */
/* Andrea Orlati(aorlati@ira.inaf.it)  19/05/2009     Creation                                                   */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the by the user offsets properties. The associeted property can be selected 
 * at construction time and cannot be changed anymore.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOOffsets: public virtual DevIO<CORBA::Double>
{
public:
	
	enum TOffsets {
		SSLONOFF,
		SSLATOFF,
		SAZOFF,
		SELOFF
	};
	
	DevIOOffsets(IRA::CSecureArea<CBossCore>* core,const TOffsets& e): m_core(core), m_corr(e) { 
		AUTO_TRACE("DevIOOffsets::DevIOOffsets()");
	}
	
	~DevIOOffsets() {
		AUTO_TRACE("DevIOOffsets::~DevIOOffsets()");
	}
	
	bool initializeValue() {
		return false;
	}
	
	CORBA::Double read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
		AUTO_TRACE("DevIOOffsets::read()");
		double val=0.0;
		if (m_corr==SSLONOFF) {
			val=resource->getSubScanLonOff();
		}
		else if (m_corr==SSLATOFF) {
			val=resource->getSubScanLatOff();
		}
		else if (m_corr==SAZOFF) {
			val=resource->getSystemAzOff();
		}
		else if (m_corr==SELOFF) {
			val=resource->getSystemElOff();
		}
		/*else if (m_corr==LONGITUDEOFF) {
			val=resource->getLongitudeOffset();
		}
		else if (m_corr==LATITUDEOFF) {
			val=resource->getLatitudeOffset();
		}*/
		timestamp=getTimeStamp();
		return (CORBA::Double)val;
    }
	
    void write(const CORBA::Double& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOffsets::write()");
	}
    
private:
	IRA::CSecureArea<CBossCore> *m_core;
	TOffsets m_corr;
};











#endif /*DEVIOOFFSETS_H_*/
