#ifndef _SRTACTIVESURFACEBOSSIMPLDEVIOLUT_H_
#define _SRTACTIVESURFACEBOSSIMPLDEVIOLUT_H_

/****************************************************************************************/
/* OAC Osservatorio Astronomico di Cagliari                                             */
/* $Id: DevIOLUT.h,v 1.0 2025-01-13 15:42:32 G. Carboni Exp $                       	*/
/*                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                 */
/*                                                                                      */
/* Who                                          when        What                        */
/* Giuseppe Carboni (giuseppe.carboni@inaf.it)  13/01/2025  Creation                    */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. 
 * @author <a href=mailto:giuseppe.carboni@inaf.it>Giuseppe Carboni</a>,
 * Osservatorio Astronomico di Cagliari, Italia<br>
 */ 
class SRTActiveSurfaceBossImplDevIOLUT: public virtual DevIO<ACE_CString>
{
public:
	
	SRTActiveSurfaceBossImplDevIOLUT(IRA::CSecureArea<CSRTActiveSurfaceBossCore>* core): m_core(core) { 
		AUTO_TRACE("SRTActiveSurfaceBossImplDevIOLUT::SRTActiveSurfaceBossImplDevIOLUT()");
	}
	
	~SRTActiveSurfaceBossImplDevIOLUT() {
		AUTO_TRACE("SRTActiveSurfaceBossImplDevIOLUT::~SRTActiveSurfaceBossImplDevIOLUT()");
	}
	
	bool initializeValue() 
	{
		return false;
	}
	
	ACE_CString read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		CSecAreaResourceWrapper<CSRTActiveSurfaceBossCore> resource=m_core->Get();
		AUTO_TRACE("SRTActiveSurfaceBossImplDevIOLUT::read()");
		timestamp=getTimeStamp();
		return resource->getLUTfilename().c_str();
    }
	
    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("SRTActiveSurfaceBossImplDevIOLUT::write()");
	}
    
private:
	IRA::CSecureArea<CSRTActiveSurfaceBossCore> *m_core;
};

#endif /*DEVIOLUT_H_*/
