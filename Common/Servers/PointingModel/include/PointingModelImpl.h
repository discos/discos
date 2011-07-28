#ifndef POINTINGMODELIMPL_H
#define POINTINGMODELIMPL_H

/* ****************************************************************************** */
/* IRA Istituto di Radioastronomia                                                */
/* $Id: PointingModelImpl.h,v 1.2 2008-02-01 09:38:14 c.migoni Exp $  */
/*                                                                                */
/* This code is under GNU General Public Licence (GPL).                           */
/*                                                                                */
/* Who                                when            What                        */
/* Carlo Migoni (migoni@ira.inaf.it)  10/05/2007      Creation                    */
/* ****************************************************************************** */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/** 
 * @mainpage Pointing Model component documentation
 * @date 10/05/2007
 * @version 1.0.0
 * @author <a href=mailto:migoni@ira.inaf.it>Carlo Migoni</a>
 * @remarks Last compiled under ACS 6.0.2
*/

#include <acscomponentImpl.h>
#include <PointingModelS.h>
#include <IRA>
#include <AntennaErrors.h>
#include <ComponentErrors.h>
#include <DBTable.h>

#define MAX_MODEL_PARAM 30

using namespace acscomponent;
using namespace IRA;

/**
 * This class implements the Antenna::PointingModel CORBA interface and the ACS Component.  
 * @author <a href=mailto:migoni@ira.inaf.it>Carlo Migoni</a>
 * Istituto di Radioastronomia, Italia
 * <br> 
 */
class PointingModelImpl: public virtual acscomponent::ACSComponentImpl, public virtual POA_Antenna::PointingModel
{
	public:
	
	/** 
	* Constructor.
	* @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
	* @param containerServices pointer to the class that exposes all services offered by container
	*/
	PointingModelImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices);

	/**
	 * Destructor.
	*/
	virtual ~PointingModelImpl(); 

	/** 
	 * Called to give the component time to initialize itself. The component reads in configuration files/parameters, builds up connection. 
	 * Called before execute. It is implemented as a synchronous (blocking) call.
	 * @throw ACSErr::ACSbaseExImpl
	 * @arg \c ComponentErrors::MemoryAllocation
	*/
	virtual void initialize() throw (ACSErr::ACSbaseExImpl);

	/**
 	 * Called after <i>initialize()</i> to tell the component that it has to be ready to accept incoming functional calls any time. 
	 * Must be implemented as a synchronous (blocking) call. In this class the default implementation only logs the COMPSTATE_OPERATIONAL
	 * @throw ACSErr::ACSbaseExImpl
	 * @arg \c ComponentErrors::CDBAccess
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
	* This method is used to calculate the azimuth and elevation offsets (radians)
	* @throw CORBA::SystemException
	* @throw AntennaErrors::ReceiverNotFoundEx
	* @param azimuth
	* @param elevation
	* @param azOffset
	* @param elOffset
	*/
	void getAzElOffsets (CORBA::Double azimuth, CORBA::Double elevation, CORBA::Double_out azOffset, CORBA::Double_out elOffset) throw (CORBA::SystemException, AntennaErrors::AntennaErrorsEx);

	/**
	* This method is used to set the receiver for the observation
	* @throw CORBA::SystemException
	* @param receiverCode
	*/
	void setReceiver(const char* receiverCode) throw (CORBA::SystemException, AntennaErrors::AntennaErrorsEx); 
	
	private:
	/**
	* Copy constructor is not allowed following the ACS desgin rules.
	*/
	PointingModelImpl(const PointingModelImpl&);
	/**
	* Assignment operator is not allowed due to ACS design rules.
	*/
	void operator=(const PointingModelImpl&);

	/**
	 * This will contains the code of the receiver
	*/
	CString m_receiverCode;
	/**
	 * This will contain the name of the component......
	 */
	CString m_componentName;
	/**
	 * This is the table used to access the CDB in order to read the receiver table
	*/
	CDBTable* m_receiverTable;
	/**
	* Struct containing receiver pointing model parameters
	*/
	struct PointingModel {
		double phi;
		double coefficientVal[MAX_MODEL_PARAM];
		int coefficientNum[MAX_MODEL_PARAM];
	};
	PointingModel PM;
	/**
	* Internally used to calculate azimuth offset
	*/
	void getAzOff (double az, double el, double &azOff);
	/**
	* Internally used to calculate elevation offset
	*/
	void getElOff (double az, double el, double &elOff);
	/**
	* This parameter indicates if a receiver is active or not
	*/
	bool receiverActivated;
};
#endif /*POINTINGMODELIMPL_H*/
