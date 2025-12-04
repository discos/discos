/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef __DEROTATORIMPL__H
#define __DEROTATORIMPL__H

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <baciRWdouble.h>
#include <baciROdouble.h>
#include <baciROlong.h>
#include <baciROpattern.h>
#include <baciROuLongLong.h>
#include <acsncSimpleSupplier.h>
#include <enumpropROImpl.h>
#include <ComponentErrors.h>
#include <LogFilter.h>
#include "sensorSocket.h"
#include "icdSocket.h"
#include <MinorServoS.h>

using namespace baci;

class DerotatorImpl: public CharacteristicComponentImpl,  public virtual POA_MinorServo::Derotator {

public:
    
    DerotatorImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices);

    virtual ~DerotatorImpl(); 

    /**
     * Get the parameter from CDB and create a sensorSocket and a CSecureArea. 
     * Initialize the socket calling its Init method.
     *
     * @throw ComponentErrors::CDBAccessExImpl
     * @throw ACSErr::ACSbaseExImpl 
     */
    virtual void initialize() throw (ComponentErrors::CDBAccessExImpl, ACSErr::ACSbaseExImpl);


    /**
     * @throw ACSErr::ACSbaseExImpl
     */
    virtual void execute() throw (ACSErr::ACSbaseExImpl);
    

    /** 
     * Called by the container before destroying the server in a normal situation. 
     * This function takes charge of releasing all resources.
     */
     virtual void cleanUp();
    

    /** 
     * Called by the container in case of error or emergency situation. 
     * This function tries to free all resources even though there is no warranty that the function 
     * is completely executed before the component is destroyed.
     */ 
    virtual void aboutToAbort();
    
    
    /** 
     * Return a reference to the sensor postition property implementation of IDL interface.
     *
     * @return pointer to read-only double property position 
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr sensor_position() throw (CORBA::SystemException);


    /** 
     * Return a reference to the icd position property (physical position)implementation 
     * of IDL interface.
     *
     * @return pointer to read-write double property position 
     * @throw CORBA::SystemException
     */
     virtual ACS::RWdouble_ptr icd_position() throw (CORBA::SystemException);
    

    /**
     * Return a reference to the ICD verbose status property implementation of IDL interface.
     *
     * @return pointer to read-only pattern property verbose status 
     * @throw CORBA::SystemException
     */
     virtual ACS::ROpattern_ptr icd_verbose_status() throw (CORBA::SystemException);


    /**
     * Return a reference to the ICD summary status property implementation of IDL interface.
     *
     * @return pointer to read-only pattern property summary status 
     * @throw CORBA::SystemException
     */
     virtual ACS::ROpattern_ptr icd_summary_status() throw (CORBA::SystemException);


    /**
     * Return the ICD target position (double) in the user's reference system
     *
     * @return ICD target position (double) in the user's reference system
     * @throw CORBA::SystemException
     */
	 void getIcdTargetPosition(CORBA::Double_out target_position) ;


    /**
     * Return the ICD position (double) in the user's reference system
     *
     * @return ICD position (double) in the user's reference system
     * @throw CORBA::SystemException
     */
	 void getIcdURSPosition(CORBA::Double_out icd_urs_position) ;


    /**
     * Return the sensor position (double) in the user's reference system
     *
     * @return sensor position (double) in the user's reference system
     * @throw CORBA::SystemException
     */
	 void getSensorURSPosition(CORBA::Double_out sensor_urs_position) ;

    
private:

   CSecureArea<sensorSocket> *m_sensorLink;
   CSecureArea<icdSocket> *m_icdLink;

   // Sensor position property
   SmartPropertyPointer<ROdouble> m_sensor_position;
   
   // ICD position property
   SmartPropertyPointer<RWdouble> m_icd_position;

   // ICD Verbose Status 
   SmartPropertyPointer<ROpattern> m_icd_verbose_status;
   //
   // ICD Summary Status 
   SmartPropertyPointer<ROpattern> m_icd_summary_status;

   void operator=(const DerotatorImpl&);

};

#endif
