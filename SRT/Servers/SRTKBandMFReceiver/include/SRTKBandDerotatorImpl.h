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
#include <DerotatorErrors.h>
#include <SRTKBandDerotatorS.h>
#include <LogFilter.h>
#include "sensorSocket.h"
#include "icdSocket.h"

using namespace baci;

class SRTKBandDerotatorImpl: public CharacteristicComponentImpl,  public virtual POA_Receivers::SRTKBandDerotator {

public:
    
    SRTKBandDerotatorImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices);

    virtual ~SRTKBandDerotatorImpl(); 

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
     * Return a reference to the engine postition property implementation of IDL interface.
     *
     * @return pointer to read-only double property position 
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr enginePosition() throw (CORBA::SystemException);


    /** 
     * Return a reference to the actPosition property implemented in the IDL interface
     *
     * @return pointer to read-write double property position 
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr actPosition() throw (CORBA::SystemException);
 

    /** 
     * Return a reference to the cmdPosition property implemented in the IDL interface
     *
     * @return pointer to read-write double property position 
     * @throw CORBA::SystemException
     */
     virtual ACS::RWdouble_ptr cmdPosition() throw (CORBA::SystemException);
    

    /** 
     * Return a reference to the positionDiff property implemented in the IDL interface
     *
     * @return pointer to read-only double property position 
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr positionDiff() throw (CORBA::SystemException);

     virtual Management::ROTBoolean_ptr tracking() throw (CORBA::SystemException);
   

    /**
     * Return a reference to the ICD verbose status property implementation of IDL interface.
     *
     * @return pointer to read-only pattern property verbose status 
     * @throw CORBA::SystemException
     */
     virtual ACS::ROpattern_ptr icd_verbose_status() throw (CORBA::SystemException);


    /**
     * Return a reference to the status property implementation of IDL interface.
     *
     * @return pointer to the read-only status pattern property
     * @throw CORBA::SystemException
     */
     virtual ACS::ROpattern_ptr status() throw (CORBA::SystemException);


    /**
     * Allow the derotator to be ready to move
     *
     * @return ICD target position (double) in the user's reference system
     * @throw CORBA::SystemException
     * @throw DerotatorErrors::ConfigurationErrorEx, 
     * @throw ComponentErrors::ComponentErrorsEx);
     */
     void setup() throw (
        CORBA::SystemException,
        DerotatorErrors::ConfigurationErrorEx, 
        ComponentErrors::ComponentErrorsEx);


    /** Switch the power amplifier off
     *
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx);
     */
     void powerOff() throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);


    /**
     * Set the derotator position (value in the user's reference system)
     *
     * @ComponentErrors::ComponentErrorsEx, 
     * @throw DerotatorErrors::PositioningErrorEx,
	 * @throw DerotatorErrors::CommunicationErrorEx
     * @throw CORBA::SystemException
     */
     void setPosition(double position) throw (
         CORBA::SystemException, 
         ComponentErrors::ComponentErrorsEx, 
         DerotatorErrors::PositioningErrorEx,
	     DerotatorErrors::CommunicationErrorEx
     );


    /**
     * @return the sensor position (double) in the user's reference system
     *
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw DerotatorErrors::CommunicationErrorEx
     * @throw CORBA::SystemException
     */
     double getActPosition() throw (
            CORBA::SystemException, 
            ComponentErrors::ComponentErrorsEx, 
            DerotatorErrors::CommunicationErrorEx
     );


    /**
     * @return the last commanded position, in the user's reference system
     */
     double getCmdPosition();


    /**
     * @return the position at which the derotator was at a given time t</li>
     */
     double getPositionFromHistory(ACS::Time t);
     

     /**  
      * @ return the biggest position allowed, in the URS
      */
     double getMaxLimit();


     /**  
      * @ return the lowerst position allowed, in the URS
      */
     double getMinLimit();

     /**  
      * @ return the angle (in degrees) between two adiacent feeds
      */
     double getStep();



     /**  
      * @ return true when the derotator is tracking
      */
     bool isTracking();
     

     /**
      * @return true when the derotator is ready to move
      */
     bool isReady();


     /**
      * @return true when the derotator is moving
      */
     bool isSlewing();

     /**
      * Return the engine position (double) in the user's reference system
      *
      * @return engine position (double) in the user's reference system
      * @throw CORBA::SystemException
      */
     double getEnginePosition() ;


private:
    // CDB attributes
    CString SensorIP;
    DWORD SensorPort;
    DWORD SensorTimeout;
    long SensorZeroReference;
    double SensorConversionFactor;

    CString IP;
    DWORD Port;
    DWORD Timeout;
    double ZeroReference;
    double ConversionFactor;
    double MaxValue;
    double MinValue;
    double Step;
    double TrackingDelta;
    double PositionExpireTime;

   CSecureArea<sensorSocket> *m_sensorLink;
   CSecureArea<icdSocket> *m_icdLink;

   // Engine position property
   SmartPropertyPointer<ROdouble> m_enginePosition;
   
   /*
    * This property returns the engine encoder position in the user
    * reference system. That means its value is a degree angle,
    * obtained by subtracting a "zero reference" from the plain value
    * (in case converted as angle) got from the engine encoder.
    */
   SmartPropertyPointer<ROdouble> m_actPosition;
 
   /*
    * This property allows us to set a derotator position and to read the
    * last commanded position. The value to set must be a degree angle, 
    * in the user reference system. 
    */
   SmartPropertyPointer<RWdouble> m_cmdPosition;

   /* Difference between the actual and the commanded position */
   SmartPropertyPointer<ROdouble> m_positionDiff;

   SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean> > m_tracking;

   // ICD Verbose Status 
   SmartPropertyPointer<ROpattern> m_icd_verbose_status;
   //
   // ICD Summary Status 
   SmartPropertyPointer<ROpattern> m_status;

   void operator=(const SRTKBandDerotatorImpl&);

};

#endif
