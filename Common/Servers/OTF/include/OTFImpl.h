/* ***************************************************************** */
/* IRA Istituto di Radioastronomia                                   */
/* $Id: OTFImpl.h,v 1.12 2010-09-07 11:58:51 s.righini Exp $                                                                 */
/*                                                                   */
/* This code is under GNU General Public Licence (GPL)               */
/*                                                                   */
/* Who                      When            What   				     */
/* Simona Righini			19/03/2008      created      			 */
/* Simona Righini			27/05/2008		modified	             */
/* Simona Righini			10/06/2008		bug fixing & CDB param. loading */
/* Simona Righini			06/09/2010      offsets handling added*/
/* Simona Righini           06/09/2010      0-azimuth bug fixing*/


#ifndef OTFIMPL_H_
#define OTFIMPL_H_

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/** 
 * @mainpage Header file of "OTF" module
 * @date 08/04/2013
 * @version 0.15.0
 * @author <a href=mailto:righin_s@ira.inaf.it>Simona Righini</a>, 
 * @remarks Last compiled under ACS 8.2.0
 */

#include <baci.h>
#include <OTFS.h>
#include <AntennaErrors.h>
#include <ComponentErrors.h>
#include <acscomponentImpl.h>
#include "SubScan.h"
#include <String.h>
#include <Site.h>


using namespace baci;
using namespace IRA;

/**
 * Component returns the sequence of Az,El pointings to move along the subscan, 
 * given the start/stop positions (or the central position plus the subscan span)
 * and the subscan duration.
 * Available reference frames: Equatorial (J2000), Galactic and Horizontal.
 * Available scan geometries: along Great Circle, along longitude or latitude 
 * of the selected subscan reference frame.
 * @author <a href=mailto:righin_s@ira.inaf.it>Simona Righini</a>,
 * Istituto di Radioastronomia, Italy
 */

class OTFImpl : acscomponent::ACSComponentImpl,
virtual public POA_Antenna::OTF 
{
public:
	OTFImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices);
	virtual ~OTFImpl();
	
	virtual void initialize() throw (ACSErr::ACSbaseExImpl);
	virtual void execute() throw (ACSErr::ACSbaseExImpl);
	virtual void cleanup();
	virtual void aboutToAbort();
	
	
	/* This method gets only azimuth and elevation computed by 
	 * the "SubScan" class */
	virtual void getHorizontalCoordinate (
	        ACS::Time time,
	        CORBA::Double_out az,
	        CORBA::Double_out el
	      ) throw (CORBA::SystemException);
	
	/* This method retrieves the whole list of attributes */
	virtual void getAttributes (Antenna::OTFAttributes_out att)
	throw (CORBA::SystemException);
	
	virtual void getAllCoordinates(ACS::Time time,CORBA::Double_out az,CORBA::Double_out el,CORBA::Double_out ra,CORBA::Double_out dec,CORBA::Double_out jepoch,CORBA::Double_out lon,
			CORBA::Double_out lat) throw (CORBA::SystemException);

	/* Sets all the parameters which define the subscan */
	virtual Antenna::TSections setSubScan(
			const char * targetName,
			CORBA::Double initAz,
			Antenna::TSections initSteering,
			CORBA::Double initEl,
			ACS::Time initTime,
			CORBA::Double lon1,
	        CORBA::Double lat1,
	        CORBA::Double lon2,
	        CORBA::Double lat2,
	        Antenna::TCoordinateFrame coordFrame,
	        Antenna::TsubScanGeometry geometry,
	        Antenna::TCoordinateFrame subScanFrame,
	        Antenna::TsubScanDescription description,
	        Antenna::TsubScanDirection direction,
	        ACS::Time startUT,
	        ACS::TimeInterval subScanDuration
	 ) throw (CORBA::SystemException,AntennaErrors::AntennaErrorsEx,
	    		ComponentErrors::ComponentErrorsEx);

	/* Checks if actual pointing coincides with the predicted one, within 0.1*HPBW */ 
	virtual CORBA::Boolean checkTracking(ACS::Time time, CORBA::Double az, CORBA::Double el, CORBA::Double HPBW
			 ) throw (CORBA::SystemException);
	
	/* Checks if the antenna is slewing, running along a ramp or running along the 
	 * constant speed path*/
	virtual CORBA::Long checkPath(ACS::Time time, CORBA::Double az, CORBA::Double el 
			 ) throw (CORBA::SystemException);


	virtual void setOffsets(CORBA::Double lon, CORBA::Double lat, Antenna::TCoordinateFrame frame)
	throw(CORBA::SystemException, AntennaErrors::AntennaErrorsEx);

    /**
     * This method is supposed to compute the  flux of the current target. For this component it is considered to be a place holder.
     * @throw CORBA::SystemException
     * @param freq frequency observed
     * @param fwhm fwhm
     * @param flux computed flux
     */ 
    void computeFlux(CORBA::Double freq, CORBA::Double fwhm, CORBA::Double_out flux) throw (CORBA::SystemException);

	
private:
	SubScan m_subScan;
	IRA::CString m_componentName;
	IRA::CSite m_site;
	double m_dut1;
	BACIMutex m_mutex;
	IRA::CString m_targetName;
};

#endif /*OTFIMPL_H_*/
