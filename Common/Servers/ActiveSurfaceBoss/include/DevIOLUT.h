#ifndef _ACTIVESURFACEBOSSIMPLDEVIOLUT_H_
#define _ACTIVESURFACEBOSSIMPLDEVIOLUT_H_

/****************************************************************************************/
/* OAC Osservatorio Astronomico di Cagliari                                             */
/* $Id: DevIOLUT.h,v 1.0 2025-01-13 15:42:32 G. Carboni Exp $                       	*/
/*                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                 */
/*                                                                                      */
/* Who                                          when        What                        */
/* Giuseppe Carboni (giuseppe.carboni@inaf.it)  13/01/2025  Creation                    */
/* Giuseppe Carboni (giuseppe.carboni@inaf.it)  04/09/2025  Code generalization          */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

/**
 * This  class is derived from the template DevIO. 
 * @author <a href=mailto:giuseppe.carboni@inaf.it>Giuseppe Carboni</a>,
 * Osservatorio Astronomico di Cagliari, Italia<br>
 */ 
class ActiveSurfaceBossImplDevIOLUT: public virtual DevIO<ACE_CString>
{
public:
	
	ActiveSurfaceBossImplDevIOLUT(IRA::CSecureArea<CActiveSurfaceBossCore>* core): m_core(core) { 
		AUTO_TRACE("ActiveSurfaceBossImplDevIOLUT::ActiveSurfaceBossImplDevIOLUT()");
	}
	
	~ActiveSurfaceBossImplDevIOLUT() {
		AUTO_TRACE("ActiveSurfaceBossImplDevIOLUT::~ActiveSurfaceBossImplDevIOLUT()");
	}
	
	bool initializeValue() 
	{
		return false;
	}
	
	ACE_CString read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		CSecAreaResourceWrapper<CActiveSurfaceBossCore> resource=m_core->Get();
		AUTO_TRACE("ActiveSurfaceBossImplDevIOLUT::read()");
		timestamp=getTimeStamp();
		return resource->getLUTfilename().c_str();
    }
	
    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("ActiveSurfaceBossImplDevIOLUT::write()");
	}
    
private:
	IRA::CSecureArea<CActiveSurfaceBossCore> *m_core;
};

#endif /*DEVIOLUT_H_*/
