#ifndef _DEVIOCORRECTIONOFFSETS_H_
#define _DEVIOCORRECTIONOFFSETS_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: DevIOCorrectionOffsets.h,v 1.1 2008-04-14 08:39:09 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  07/04/2008     Creation                                                      */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the by the correction offsets property. The associeted property can be selected 
 * at construction time and cannot be changed anymore.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOCorrectionOffsets: public virtual DevIO<CORBA::Double>
{
public:
	
	enum TCorrection {
		POINTINGAZIMUTHOFFSET,
		POINTINGELEVATIONOFFSET,
		REFRACTIONOFFSET
	};
	
	DevIOCorrectionOffsets(IRA::CSecureArea<CBossCore>* core,const TCorrection& e): m_core(core), m_corr(e) { 
		AUTO_TRACE("DevIOCorrectionOffsets::DevIOCorrectionOffsets()");
	}
	
	~DevIOCorrectionOffsets() {
		AUTO_TRACE("DevIOCorrectionOffsets::~DevIOCorrectionOffsets()");
	}
	
	bool initializeValue() {
		return false;
	}
	
	CORBA::Double read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
		AUTO_TRACE("DevIOCorrectionOffsets::read()");
		double val=0.0;
		if (m_corr==POINTINGAZIMUTHOFFSET) {
			val=resource->getPointingAzOffset();
		}
		else if (m_corr==POINTINGELEVATIONOFFSET) {
			val=resource->getPointingElOffset();
		}
		else if (m_corr==REFRACTIONOFFSET) {
			val=resource->getRefractionOffset();
		}
		timestamp=getTimeStamp();
		return (CORBA::Double)val;
    }
	
    void write(const CORBA::Double& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOCorrectionOffsets::write()");
	}
    
private:
	IRA::CSecureArea<CBossCore> *m_core;
	TCorrection m_corr;
};

#endif /*DEVIOCORRECTIONOFFSETS_H_*/
