#ifndef OBSERVATORYIMPL_H
#define OBSERVATORYIMPL_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: ObservatoryImpl.h,v 1.8 2011-04-19 06:53:56 c.migoni Exp $                                                                                                 */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  16/01/2006      Creation                                         */
/* Andrea Orlati(aorlati@ira.inaf.it)  08/01/2007      Ported to ACS 5.0.4                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  30/03/2007      Ported to ACS 6.0.2                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  21/03/2009      Location information are now collected from table that match the Station.xsd schema   */
/* Andrea Orlati(aorlati@ira.inaf.it)  13/09/2010      Implmentation of the attribute observatoryName  */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/** 
 * @mainpage Observatory component documentation
 * @date 13/09/2010
 * @version 1.0.0
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>
 * @remarks Last compiled under ACS 7.0.2
 * @remarks compiler version is 4.1.2
*/
#include <ObservatoryS.h>
#include <ComponentErrors.h>
#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <baciROuLongLong.h>
#include <baciROdouble.h>
#include <baciROstring.h>
#include <enumpropROImpl.h>
#include <SecureArea.h>
#include <Site.h>
#include <DateTime.h>

using namespace baci;
using namespace maci;

/**
 * Data structure used as argument type for the DevIO.
*/
typedef struct {
	CSite site;           /*!< site information  */
	double dut1;          /*!< difference between utc and ut1 */
} T_DevDataBlock;


/**
 * This class implements the Observatory component. This componet stores the site information like latitude and longitude in addition to
 * all other parameters (dut1, polar motion) required form times/coordinates computation. All these parameter are also stored in the
 * configuration database.  
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/ 
class ObservatoryImpl: public CharacteristicComponentImpl,public virtual POA_Antenna::Observatory
{
public:
	/** 
	* Constructor.
	* @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
	* @param containerServices pointer to the class that exposes all services offered by container
	*/
	ObservatoryImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices);

	/**
	 * Destructor.
	*/
	virtual ~ObservatoryImpl(); 

	/** 
	 * Called to give the component time to initialize itself. The component reads into configuration files/parameters, builds up connection. 
	 * Called before execute. It is implemented as a synchronous (blocking) call.
	 * @throw ComponentCommonErrors::MemoryAllocationExImpl
	*/
	virtual void initialize() throw (ACSErr::ACSbaseExImpl);

	/**
 	 * Called after <i>initialize()</i> to tell the component that it has to be ready to accept incoming functional calls at any time. 
	 * Must be implemented as a synchronous (blocking) call. 	 
	 * In this class the default implementation only logs the COMPSTATE_OPERATIONAL
	*/
	virtual void execute() throw (ACSErr::ACSbaseExImpl);
	 
	/** 
	 * Called by the container before destroying the server in a normal situation. This function takes charge of releasing all resources.
	*/
	virtual void cleanUp();
	 
	/** 
	 * Called by the container in case of error or emergency situation. This function tries to free all resources even though there is no
	 * warranty that the function is completely executed before the component is destroyed.
	*/	
	virtual void aboutToAbort();	
	
	/**
     * Returns a reference to the observatoryName property implementation of IDL interface.
	 * @return pointer to read-only string property
	*/
	virtual ACS::ROstring_ptr observatoryName() throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the universalTime property implementation of IDL interface.
	 * @return pointer to read-only long long property universalTime
	*/
	virtual ACS::ROuLongLong_ptr universalTime() throw (CORBA::SystemException);

	/**
     * Returns a reference to the implementation of the julianDay property of the IDL interface.
	 * @return pointer to read-only double property julianDay
	*/
	virtual ACS::ROdouble_ptr julianDay() throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the implementation of the GAST property of the IDL interface.
	 * @return pointer to read-only long long property GST
	*/
	virtual ACS::ROuLongLong_ptr GAST() throw (CORBA::SystemException);	
	
	/**
     * Returns a reference to the implementation of the LST property of the IDL interface.
	 * @return pointer to read-only long long property LST
	*/
	virtual ACS::ROuLongLong_ptr LST() throw (CORBA::SystemException);		

	/**
     * Returns a reference to the implementation of the DUT1 property of the IDL interface.
	 * @return pointer to read-only double property DUT1
	*/
	virtual ACS::ROdouble_ptr DUT1() throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the implementation of the latitude property of the IDL interface.
	 * @return pointer to read-only double property latitude.
	*/
	virtual ACS::ROdouble_ptr latitude() throw (CORBA::SystemException);

	/**
     * Returns a reference to the implementation of the longitude property of the IDL interface.
	 * @return pointer to read-only double property longitude
	*/
	virtual ACS::ROdouble_ptr longitude() throw (CORBA::SystemException);

	/**
     * Returns a reference to the inplementation of the height property of the IDL interface.
	 * @return pointer to read-only double property height
	*/
	virtual ACS::ROdouble_ptr height() throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the inplementation of the yPolarMotion property of the IDL interface.
	 * @return pointer to read-only double property yPolarMotion
	*/
	virtual ACS::ROdouble_ptr yPolarMotion() throw (CORBA::SystemException);

	/**
     * Returns a reference to the inplementation of the height property of the IDL interface.
	 * @return pointer to read-only double property xPolarMotion
	*/
	virtual ACS::ROdouble_ptr xPolarMotion() throw (CORBA::SystemException);

	/**
     * Returns a reference to the inplementation of the xGeod property of the IDL interface.
	 * @return pointer to read-only double property xGeod
	*/
	virtual ACS::ROdouble_ptr xGeod() throw (CORBA::SystemException);

	/**
     * Returns a reference to the inplementation of the yGeod property of the IDL interface.
	 * @return pointer to read-only double property yGeod
	*/
	virtual ACS::ROdouble_ptr yGeod() throw (CORBA::SystemException);

	/**
     * Returns a reference to the inplementation of the zGeod property of the IDL interface.
	 * @return pointer to read-only double property zGeod
	*/
	virtual ACS::ROdouble_ptr zGeod() throw (CORBA::SystemException);

	/**
     * Returns a reference to the inplementation of the geodeticModel property of the IDL interface.
	 * @return pointer to read-only TGeodeticModel property geodeticModel
	*/
	virtual Antenna::ROTGeodeticModel_ptr geodeticModel() throw (CORBA::SystemException);

	IRAIDL::TSiteInformation getSiteSummary() throw (CORBA::SystemException);
	
	/** 
	 * It allows the client to load a new value for the DUT1 property.
	 * @throw ComponentCommonErrors::CDBAccessExImpl
	 * @throw ComponentCommonErrors::PropertyErrorExImpl
	 * @param val new DUT1 value (fraction of day)
	 * @param save if true the new value is saved into the Configuration Database
   	*/
	void setDUT1(CORBA::Double val,CORBA::Boolean save) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);	
	
	/** 
	 * It allows the client to load a new value for the geodeticModel property.
	 * @throw ComponentCommonErrors::CDBAccessExImpl
	 * @throw ComponentCommonErrors::PropertyErrorExImpl
	 * @param model new geodeticModel value
	 * @param save if true the new value is saved into the Configuration Database
   	*/
	void setGeodeticModel(Antenna::TGeodeticModel model,CORBA::Boolean save) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);
	
	/**
	 * It allows the caller to load new values for the xPolarMotion and yPolarMotion properties.
	 * @throw ComponentCommonErrors::CDBAccessExImpl
	 * @throw ComponentCommonErrors::PropertyErrorExImpl
	 * @param xP new value for the xPolarMotion property
	 * @param yP new value for the yPolarMotion property
	 * @param save if true the new value is saved into the Configuration Database
	*/
	void setPoleMotion(CORBA::Double xP,CORBA::Double yP,CORBA::Boolean save)  throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

	/**
	 * It converts from the local rappresentation of the geoid model to the remote one
	 * @param model local rapresentation
	 * @return the remote rapresentation of the model
	*/
	static Antenna::TGeodeticModel GeodeticModel2IDL(const CSite::TGeodeticEllipsoid& model);

	/**
	 * It converts from the remote rappresentation of the geoid model to the local one
	 * @param model remote rapresentation (IDL file)
	 * @return the local rapresentation of the model
	*/	
	static CSite::TGeodeticEllipsoid IDL2GeodeticModel(const Antenna::TGeodeticModel& model); 


private:
	/** universalTime property */
	SmartPropertyPointer<ROstring> m_observatoryName;	
	/** universalTime property */
	SmartPropertyPointer<ROuLongLong> m_puniversalTime;
	/** julianDay property */
	SmartPropertyPointer<ROdouble> m_pjulianDay;
	/** apparentGST property */
	SmartPropertyPointer<ROuLongLong> m_pGAST;
	/** LST property */
	SmartPropertyPointer<ROuLongLong> m_pLST;	
	/** DUT1 property */
	SmartPropertyPointer<ROdouble> m_pDUT1;
	/** latitude property */
	SmartPropertyPointer<ROdouble> m_platitude;
	/** longitude property */
	SmartPropertyPointer<ROdouble> m_plongitude;
	/** height property */
	SmartPropertyPointer<ROdouble> m_pheight;
	/** XGeod property */
	SmartPropertyPointer<ROdouble> m_pxGeod;
	/** YGeod property */	
	SmartPropertyPointer<ROdouble> m_pyGeod;
	/** ZGeod property */	
	SmartPropertyPointer<ROdouble> m_pzGeod;
	/** yPolarMotion property */
	SmartPropertyPointer<ROdouble> m_pyPolarMotion;
	/** xPolarMotion property */
	SmartPropertyPointer<ROdouble> m_pxPolarMotion;
	/** geodeticModel property */
	SmartPropertyPointer < ROEnumImpl<ACS_ENUM_T(Antenna::TGeodeticModel),POA_Antenna::ROTGeodeticModel> > m_pgeodeticModel;
	/** Data structure, made thread safe */
	CSecureArea<T_DevDataBlock>* m_data;
};

#endif
