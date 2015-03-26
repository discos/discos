/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Bartolini
 *  E-mail:       bartolini@ira.inaf.it
\*******************************************************************************/

#ifndef __MINORSERVOBOSSIMPL_H__
#define __MINORSERVOBOSSIMPL_H__

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <boost/thread.hpp>

#include <new>
#include <acsutil.h>
#include <baciDB.h>
#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <baciROpattern.h>
#include <baciROuLongLong.h>
#include <baciROstring.h>
#include <acsncSimpleSupplier.h>
#include <enumpropROImpl.h>

#include <MinorServoDefinitionsC.h>
#include <AntennaDefinitionsC.h>
#include <ManagmentDefinitionsS.h>
#include <ComponentErrors.h>
#include <ManagementErrors.h>
//#include <WPServoImpl.h>
#include <MinorServoErrors.h>
#include <MinorServoS.h>
#include <MinorServoBossS.h>
#include <MinorServoBossC.h>
#include <MedMinorServoBossS.h>
#include <SP_parser.h>
#include <cstring>
#include <time.h>
#include <acsThread.h>
#include "acstime.h"
#include "macros.def"


//#include "ParkThread.h"
//#include "TrackingThread.h"
#include <IRA>
#include "utils.h"
#include "MSBossPublisher.h"
#include "MSBossTracker.hpp"
#include "PositionMonitoringThread.hpp"
#include "SetupThread.hpp"
#include "ScanThread.hpp"
#include "MedMinorServoParameters.hpp"
#include "MedMinorServoControl.hpp"
#include "MedMinorServoGeometry.hpp"
#include "MedMinorServoOffset.hpp"
#include "MedMinorServoStatus.hpp"
#include "MedMinorServoScan.hpp"
#include "DevIOActualSetup.h"
#include "DevIOASConfiguration.h"
#include "DevIOElevationTrack.h"
//#include "DevIOParking.h"
#include "DevIOReady.h"
#include "DevIOScanActive.h"
#include "DevIOScanning.h"
#include "DevIOStarting.h"
#include "DevIOStatus.h"
//#include "MSBossConfiguration.h"

#define PUBLISHER_THREAD_NAME "MinorServoPublisherThread"
#define TRACKING_THREAD_NAME "MinorServoTrackingThread"
#define POSITION_MONITORING_THREAD_NAME "MinorServoPositionMonitoringThread"
#define SETUP_THREAD_NAME "MinorServoSetupThread"
#define SCAN_THREAD_NAME "MinorServoScanThread"

using namespace baci;
using namespace std;
using namespace MinorServo;

struct VerboseStatusFlags {
    bool *is_initialized;
    bool is_parking;
};


class MinorServoBossImpl: public CharacteristicComponentImpl, 
                          public virtual POA_MinorServo::MedMinorServoBoss {

public:
    
    MinorServoBossImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices);

    virtual ~MinorServoBossImpl(); 

    /**
     * @throw ComponentErrors::CouldntGetComponentExImpl, ManagementErrors::ConfigurationErrorEx
     */
    virtual void initialize() throw (
            ComponentErrors::CouldntGetComponentExImpl
    );

    virtual void execute() throw (ComponentErrors::MemoryAllocationExImpl);
    
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
	 * This method implements the command line interpreter. The interpreter allows to ask for services or to issue commands
	 * to the sub-system by human readable command lines.
	 * @param cmd string that contains the command line
	 * @param answer string containing the answer to the command
	 * @return the result of the command : true if successful
	 * @throw CORBA::SystemException
	 */
    virtual CORBA::Boolean command(const char *cmd, CORBA::String_out answer) throw (CORBA::SystemException);

	/**
	 * This method is used to stow the minor servo. Only for a configured system.
	 * @throw CORBA::SystemExcpetion
	 * @throw ManagementErrors::ParkingErrorEx
	 */
	virtual void park() throw (CORBA::SystemException, ManagementErrors::ParkingErrorEx);
    
    void parkImpl() throw (ManagementErrors::ParkingErrorExImpl);

    /** Return true if the elevation tracking is enabled */
    bool isElevationTrackingEn();
    
    /** Return true if the the servo position is changing by depending of the elevation */
    bool isElevationTracking();

    /** Is the system tracking the commanded position? */
    bool isTracking();

    /** Return true when the system is performing a setup */
    bool isStarting();
    
    /** Return true when we are using the ASACTIVE configuration */
    bool isASConfiguration();
    
    /** Return true when the system is performing a park */
    bool isParking();

    /** Return true when the system is ready */
    bool isReady();

    /** Return true when the system is performing a scan */
    bool isScanning();

    /** Return true if a scan is active. To get the system in tracking, perform
     * a closeScan() */
    bool isScanActive();
	
	/**
	 * This method will be used to configure the MinorServoBoss before starting an observation
	 * @param config mnemonic code of the required configuration
	 * @throw CORBA::SystemException
	 * @throw ManagementErrors::ConfigurationErrorEx
	 */
	virtual void setup(const char *config) throw (
        CORBA::SystemException, 
        MinorServoErrors::SetupErrorEx);

    void setupImpl(const char *config) throw (MinorServoErrors::SetupErrorExImpl);

    /**
     * Turn the elevation tracking of minor servos on
     * @throw ManagementErrors::ConfigurationErrorEx
     */
    void turnTrackingOn() throw (MinorServoErrors::MinorServoErrorsEx);

    /**
     * Turn the elevation tracking of minor servos off. After that, the system is not ready
     * @throw ManagementErrors::ConfigurationErrorEx
     */
    void turnTrackingOff() throw (MinorServoErrors::MinorServoErrorsEx);

    /** Return the actual configuration */
    char * getActualSetup();

    /** Return the commanded configuration */
    char * getCommandedSetup();

    /** 
     * Return a reference to status property (ROpattern) 
     *
     * @return pointer to ROpattern status property
     * @throw CORBA::SystemException
     */
	 virtual Management::ROTSystemStatus_ptr status() throw (CORBA::SystemException);
     virtual Management::ROTBoolean_ptr ready() throw (CORBA::SystemException);
     virtual ACS::ROstring_ptr actualSetup() throw (CORBA::SystemException);
     virtual Management::ROTBoolean_ptr starting() throw (CORBA::SystemException);
     virtual Management::ROTBoolean_ptr asConfiguration() throw (CORBA::SystemException);
     virtual Management::ROTBoolean_ptr elevationTrack() throw (CORBA::SystemException);
     virtual Management::ROTBoolean_ptr scanActive() throw (CORBA::SystemException);
     virtual Management::ROTBoolean_ptr scanning() throw (CORBA::SystemException);

    /** 
     * Return a reference to verbose status property (ROpattern) 
     *
     * @return pointer to ROpattern verbose status property
     * @throw CORBA::SystemException
     */
     // virtual ACS::ROpattern_ptr verbose_status() throw (CORBA::SystemException);

    /** 
     * Check if the focus scan is achievable
     *
	 * @param starting_time the time the scan will start
     * @param range the total axis movement in mm (centered in the actual position)
	 * @param total_time the duration of axis movement
     *
     * @return true if the scan is achievable
     * @throw ManagementErrors::ConfigurationErrorEx, ManagementErrors::SubscanErrorEx
     */
     bool checkFocusScan(
             const ACS::Time starting_time, 
             const double range, 
             const ACS::Time total_time
     ) throw (MinorServoErrors::StatusErrorEx, MinorServoErrors::ScanErrorEx);


    /** 
     * Check if the scan is achievable (IDL interface)
     *
	 * @param starting_time the time the scan will start
     * @param range the total axis movement in mm (centered in the actual position)
	 * @param total_time the duration of axis movement
	 * @param axis_code the identification code of the axis
     *
     * @return true if the scan is achievable
     */
     bool checkScan(
             ACS::Time starting_time, 
             const MinorServo::MinorServoScan& scan_parameters,
             const Antenna::TRunTimeParameters& antenna_parameters,
             TRunTimeParameters_out minor_servo_parameters
     ) throw (MinorServoErrors::MinorServoErrorsEx,
         ComponentErrors::ComponentErrorsEx);

    /** 
     * Check if the scan is achievable (implementation)
     *
	 * @param starting_time the time the scan will start
     * @param range the total axis movement in mm (centered in the actual position)
	 * @param total_time the duration of axis movement
	 * @param axis_code the identification code of the axis
     * @param azimuth the antenna azimuth at scan start
     * @param elevation the antenna elevation at scan start
     *
     * @return true if the scan is achievable
     * @throw ManagementErrors::ConfigurationErrorEx, ManagementErrors::SubscanErrorEx
     */
     bool checkScanImpl(
             ACS::Time starting_time, 
             const MinorServo::MinorServoScan& scan_parameters,
             const Antenna::TRunTimeParameters& antenna_parameters,
             TRunTimeParameters_out minor_servo_parameters
     ) throw (MinorServoErrors::MinorServoErrorsEx,
              ComponentErrors::ComponentErrorsEx);

    /** 
     * Start the scan of one axis of a MinorServo target.
     *
	 * @param starting_time the time the scan will start
     * @param range the total axis movement in mm (centered in the actual position)
	 * @param total_time the duration of axis movement
	 * @param axis_code the code of the axis (SRP_TZ, GFR_TZ, ecc.)
     *
     * @throw ManagementErrors::ConfigurationErrorEx, ManagementErrors::SubscanErrorEx
     */
     void startScan(
             ACS::Time & starting_time, 
             const MinorServo::MinorServoScan& scan_parameters,
             const Antenna::TRunTimeParameters& antenna_parameters
     ) throw (MinorServoErrors::MinorServoErrorsEx,
              ComponentErrors::ComponentErrorsEx);
     
     void closeScan(ACS::Time &timeToStop) throw (MinorServoErrors::MinorServoErrorsEx,
                             ComponentErrors::ComponentErrorsEx);
     
     void startScanImpl(
             ACS::Time & starting_time, 
             const MinorServo::MinorServoScan& scan_parameters,
             const Antenna::TRunTimeParameters& antenna_parameters
     ) throw (MinorServoErrors::MinorServoErrorsEx);
    
    /** Return the central position of the axis involved in the scan */
    CORBA::Double getCentralScanPosition()
          throw (MinorServoErrors::MinorServoErrorsEx,
                 ComponentErrors::ComponentErrorsEx);

    /** Return the code of the axis involved in the scan */
    char * getScanAxis();

    /** 
     * Start the scan of the Z axis of the MinorServo active in the primary focus.
     *
	 * @param starting_time the time the scan will start
     * @param range the total axis movement in mm (centered in the actual position)
	 * @param total_time the duration of axis movement
     *
     * @throw ManagementErrors::ConfigurationErrorEx, ManagementErrors::SubscanErrorEx
     */
     void startFocusScan(
             ACS::Time & starting_time, 
             const double range, 
             const ACS::Time total_time
     ) throw (MinorServoErrors::MinorServoErrorsEx,
              ComponentErrors::ComponentErrorsEx);
     
     /** 
      * Clear the user offset of a servo (or all servos)
      *
      * @param servo a string:
      *     * the servo name 
      *     * "ALL" to clear the user offset of all servos
      * @throw MinorServoErrors::OperationNotPermittedEx
      */
     void clearUserOffset(const char *servo="ALL")
          throw (MinorServoErrors::MinorServoErrorsEx,
                 ComponentErrors::ComponentErrorsEx);
     
     /** Clear all the offsets. This method is called when the user gives a clearOffsets from the operator input */
     void clearOffsetsFromOI() throw (MinorServoErrors::OperationNotPermittedExImpl);
      
     /** 
      * Set the user offset of the servo
      *
      * @param axis_code the axis code (for instance: SRP_TZ, GRF_TZ, ecc.) 
      * @param double the offset
      * @throw MinorServoErrors::OperationNotPermittedEx
      * @throw ManagementErrors::ConfigurationErrorEx
      */
     void setUserOffset(const char * axis_code, const double offset) 
          throw (MinorServoErrors::MinorServoErrorsEx,
                 ComponentErrors::ComponentErrorsEx);

     /** Set the user offset. This method is called when the user gives a clearOffsets from the operator input */
     void setUserOffsetFromOI(const char * axis_code, const double & offset) 
         throw (MinorServoErrors::OperationNotPermittedExImpl);
     
     /**
      * Return the user offset of the system, in the same order of getAxesInfo()
      *
      * @return offset the user offset of the system, in the same order of getAxesInfo()
      * @throw ManagementErrors::ConfigurationErrorEx
      * @throw MinorServoErrors::OperationNotPermittedEx
      */
     ACS::doubleSeq * getUserOffset()
          throw (MinorServoErrors::MinorServoErrorsEx,
                 ComponentErrors::ComponentErrorsEx);

     vector<double> getOffsetImpl(string offset_type)
         throw (MinorServoErrors::MinorServoErrorsEx);

     /** 
      * Clear the system offset of a servo (or all servos)
      *
      * @param servo a string:
      *     * the servo name 
      *     * "ALL" to clear the system offset of all servos
      * @throw MinorServoErrors::OperationNotPermittedEx
      */
     void clearSystemOffset(const char *servo = "ALL")
          throw (MinorServoErrors::MinorServoErrorsEx,
                 ComponentErrors::ComponentErrorsEx);
      
     /** 
      * Set the system offset of the servo
      *
      * @param axis_code the axis code (for instance: SRP_TZ, GRF_TZ, ecc.) 
      * @param double the offset
      * @throw MinorServoErrors::OperationNotPermittedEx
      * @throw ManagementErrors::ConfigurationErrorEx
      */
     void setSystemOffset(const char * axis_code, const double offset) 
          throw (MinorServoErrors::MinorServoErrorsEx,
                 ComponentErrors::ComponentErrorsEx);

     /**
      * Return the system offset, in the same order of getAxesInfo()
      *
      * @return offset the system offset
      * @throw MinorServoErrors::OperationNotPermittedEx
      * @throw ManagementErrors::ConfigurationErrorEx
      */
     ACS::doubleSeq * getSystemOffset()
          throw (MinorServoErrors::MinorServoErrorsEx,
                 ComponentErrors::ComponentErrorsEx);
    
     /** Return the active axes names and related units
      *
      * @param axes a sequence of active axes. For instance: 
      * ("SRP_XT", "SRP_YT", "SRP_ZT", "SRP_XR", "SRP_YR", "SRP_ZR", "GFR_ZR")
      * @param units a sequence of strings, each one is the unit of the corresponding axis.
      * For instance: ("mm", "mm", "mm", "degree", "degree", "degree", "mm")
      * @throw MinorServoErrors::ConfigurationErrorEx
      */
     void getAxesInfo(ACS::stringSeq_out axes, ACS::stringSeq_out units)
          throw (MinorServoErrors::MinorServoErrorsEx,
                 ComponentErrors::ComponentErrorsEx);
 
     /** Return the positions of the active axes
      *  
      * @param time the time related to the positin we want to retrieve
      * @return a sequence of positions, in the same order of the axes parameter of getAxesInfo()
      * @throw MinorServoErrors::ConfigurationErrorEx if the system is not configured
      * @throw ComponentErrors::UnexpectedEx
      */
     ACS::doubleSeq * getAxesPosition(ACS::Time = 0) 
          throw (MinorServoErrors::MinorServoErrorsEx,
                 ComponentErrors::ComponentErrorsEx);

     /** Set the elevation tracking flag to "ON" or "OFF"
      * 
      * @param value "ON" or "OFF"
      * @throw ManagementErrors::ConfigurationErrorEx if the input is different from "ON" or "OFF"
      */
     void setElevationTracking(const char * value) 
          throw (MinorServoErrors::MinorServoErrorsEx,
                 ComponentErrors::ComponentErrorsEx);
     
     void setElevationTrackingImpl(const char * value)
          throw (MinorServoErrors::MinorServoErrorsExImpl);

     void setASConfiguration(const char * value)
          throw (MinorServoErrors::MinorServoErrorsEx,
                 ComponentErrors::ComponentErrorsEx);

     void setASConfigurationImpl(const char * value);

     /******************************************************
      * MedMinroServoBoss interface specific implementation
      ******************************************************/ 

     /**
      * Disconnects from the minor servo server and release control lock
      */
     void connect() throw (MinorServoErrors::CommunicationErrorExImpl);
     void disconnect() throw (MinorServoErrors::CommunicationErrorExImpl);
     void reset() throw (MinorServoErrors::CommunicationErrorExImpl);
     void getServoTime(ACS::Time &servoTime) throw (MinorServoErrors::CommunicationErrorExImpl);
private:
	maci::ContainerServices *m_services;
    //Antenna::AntennaBoss_var m_antenna_boss;
    //bool  m_antenna_boss_error;

    /** The last configuration c-string of setup method */
    MedMinorServoConfiguration m_config;
    MedMinorServoParameters *m_actual_config;
    MedMinorServoStatus m_servo_status;
    MedMinorServoOffset m_offset;
    MedMinorServoScan m_scan;
    boost::recursive_mutex _scan_guard;

    IRA::CString m_server_ip;
    IRA::CString m_antennaBossInterface;
    string m_commanded_conf;
    string m_actual_conf;
    string m_servo_scanned;

    /**
     * The effective servo control
     */
    MedMinorServoControl_sp m_control;

	SimpleParser::CParser<MinorServoBossImpl> *m_parser;
 
    /** Status property */
    baci::SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus), POA_Management::ROTSystemStatus> > m_status;
    baci::SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean> > m_ready;
	baci::SmartPropertyPointer<baci::ROstring> m_actualSetup;
    baci::SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean> > m_starting;
    baci::SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean> > m_asConfiguration;
    baci::SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean> > m_elevationTrack;
    baci::SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean> > m_scanActive;
    baci::SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean> > m_scanning;
    
    /** Verbose status property */
    // SmartPropertyPointer<ROpattern> m_verbose_status;

    /** Struct of verbose status flags used by SubsystemVStatusDevIO (awful practice) */
    VerboseStatusFlags m_vstatus_flags;

    /** @var thread parameters */
    //static MSThreadParameters m_thread_params;

    //TrackingThread *m_tracking_thread_ptr;

    //ScanThread *m_scan_thread_ptr;

    MSBossPublisher *m_publisher_thread_ptr;
    MSBossTracker *m_tracking_thread_ptr;
    PositionMonitoringThread *m_position_monitoring_thread_ptr;
    SetupThread *m_setup_thread_ptr;
    ScanThread *m_scan_thread_ptr;
	 
	/** This is the pointer to the notification channel */
	nc::SimpleSupplier *m_nchannel;

    bool isParked() throw (MinorServoErrors::ConfigurationErrorEx);
    
    /**
     * If not tracking elevation, sets the corerct position according to the
     * actual configuration and actual offsets.
     */
    void setCorrectPosition();
   /** 
    * Set the offset (Implementation)
    *
    * @param comp_name the component name
    * @param doubleSeq offset sequence of user offsets to add to the position; one offset for each axis
    * @throw MinorServoErrors::OperationNotPermittedExImpl
    * @throw MinorServoErrors::ConfigurationErrorExImpl
    */
    void setOffsetImpl(string comp_name, double offset, string offset_type)
        throw (MinorServoErrors::MinorServoErrorsEx);

    /** Return the minumun starting time **/
    ACS::Time getMinScanStartingTime(double range, 
                                     const string axis_code, 
                                     double & acceleration, 
                                     double & max_speed)
        throw (MinorServoErrors::ConfigurationErrorExImpl, MinorServoErrors::ScanErrorEx);

    void operator=(const MinorServoBossImpl &);
};

/** 
 * Return a doubleSeq of positions to set
 * @param token string that contains the component name
 * @return string the component name
 */
//string get_component_name(string token);

/** Return the minimum time needed to move for range mm, with given acceleration and maximum speed */
ACS::Time get_min_time(double range, double acceleration, double max_speed);

#endif
