/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef __MINORSERVOBOSSIMPL_H__
#define __MINORSERVOBOSSIMPL_H__

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <baciROpattern.h>
#include <baciROuLongLong.h>
#include <acsncSimpleSupplier.h>
#include <enumpropROImpl.h>
#include <ComponentErrors.h>
#include <ManagementErrors.h>
#include <WPServoImpl.h>
#include <MinorServoErrors.h>
#include <MinorServoS.h>
#include <MinorServoBossS.h>
#include "SetupThread.h"
#include "ParkThread.h"
#include "TrackingThread.h"
#include "ScanThread.h"
#include "MSBossPublisher.h"
#include "MSParameters.h"
#include "MSBossConfiguration.h"
#include <SP_parser.h>

using namespace baci;

struct VerboseStatusFlags {
    bool *is_initialized;
    bool is_parking;
};


class MinorServoBossImpl: public CharacteristicComponentImpl, public virtual POA_MinorServo::MinorServoBoss {

public:
    
    MinorServoBossImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices);

    virtual ~MinorServoBossImpl(); 

    /**
     * @throw ComponentErrors::CouldntGetComponentExImpl, ManagementErrors::ConfigurationErrorEx
     */
    virtual void initialize() throw (
            ComponentErrors::CouldntGetComponentExImpl, 
            ManagementErrors::ConfigurationErrorExImpl
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
    
    void parkImpl() throw (CORBA::SystemException, ManagementErrors::ParkingErrorExImpl);

    
    /** Return true if the elevation tracking is enabled */
    bool isTrackingEn();


    /** Return true when the system is performing a setup */
    bool isStarting();

    
    /** Return true when we are using the ASACTIVE configuration */
    bool isASConfiguration();

    
    /** Return true if the the servo position is changing by depending of the elevation */
    bool isTracking();


    /** Return true when the system is performing a park */
    bool isParking();


    /** Return true when the system is ready */
    bool isReady();


    /** Return true when the system is performing a scan */
    bool isScanning();
	
	/**
	 * This method will be used to configure the MinorServoBoss before starting an observation
	 * @param config mnemonic code of the required configuration
	 * @throw CORBA::SystemException
	 * @throw ManagementErrors::ConfigurationErrorEx
	 */
	virtual void setup(const char *config) throw (CORBA::SystemException, ManagementErrors::ConfigurationErrorEx);
    
    void setupImpl(const char *config) throw (CORBA::SystemException, ManagementErrors::ConfigurationErrorExImpl);


    /**
     * Turn the elevation tracking of minor servos on
     * @throw ManagementErrors::ConfigurationErrorEx
     */
    void turnTrackingOn() throw (ManagementErrors::ConfigurationErrorEx);


    /**
     * Turn the elevation tracking of minor servos off. After that, the system is not ready
     * @throw ManagementErrors::ConfigurationErrorEx
     */
    void turnTrackingOff() throw (ManagementErrors::ConfigurationErrorEx);


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
    

    /** 
     * Return a reference to verbose status property (ROpattern) 
     *
     * @return pointer to ROpattern verbose status property
     * @throw CORBA::SystemException
     */
     // virtual ACS::ROpattern_ptr verbose_status() throw (CORBA::SystemException);


    /** 
     * Check if the scan is achievable
     *
	 * @param starting_time the time the scan will start
     * @param range the total axis movement in mm (centered in the actual position)
	 * @param total_time the duration of axis movement
	 * @param axis the identification code of the axis
	 * @param servo the servo name
     *
     * @return true if the scan is achievable
     * @throw ManagementErrors::ConfigurationErrorEx, ManagementErrors::SubscanErrorEx
     */
     bool checkScan(
             const ACS::Time starting_time, 
             double range, 
             const ACS::Time total_time, 
             const unsigned short axis, 
             const char *servo
     ) throw (ManagementErrors::ConfigurationErrorEx, ManagementErrors::SubscanErrorEx);
     

    /** 
     * Start the scan of one axis of a MinorServo target.
     *
	 * @param starting_time the time the scan will start
     * @param range the total axis movement in mm (centered in the actual position)
	 * @param total_time the duration of axis movement
	 * @param axis the identification code of the axis
	 * @param servo the servo name
     *
     * @return pointer to ROpattern verbose status property
     * @throw ManagementErrors::ConfigurationErrorEx, ManagementErrors::SubscanErrorEx
     */
     void startScan(
             const ACS::Time starting_time, 
             const double range, 
             const ACS::Time total_time, 
             const unsigned short axis, 
             const char *servo
     ) throw (ManagementErrors::ConfigurationErrorEx, ManagementErrors::SubscanErrorEx);
     
     
     void stopScan() throw (ManagementErrors::SubscanErrorEx);
     

     /** 
      * Clear the user offset of a servo (or all servos)
      *
      * @param servo a string:
      *     * the servo name 
      *     * "ALL" to clear the user offset of all servos
      * @throw MinorServoErrors::OperationNotPermittedEx
      */
     void clearUserOffset(const char *servo) throw (MinorServoErrors::OperationNotPermittedEx);
     

     /** 
      * Set the user offset of the servo
      *
      * @param servo the servo name
      * @param doubleSeq offset sequence of user offsets to add to the position;
      * one offset for each axis
      * @throw MinorServoErrors::OperationNotPermittedEx
      */
     void setUserOffset(const char *servo, const ACS::doubleSeq &offset) throw (MinorServoErrors::OperationNotPermittedEx);
     

     /**
      * Return the user offset of a servo
      *
      * @param servo the servo name
      * @return offset the user offset of the servo
      * @throw MinorServoErrors::OperationNotPermittedEx
      */
     ACS::doubleSeq * getUserOffset(const char *servo) throw (MinorServoErrors::OperationNotPermittedEx);
 

     /** 
      * Clear the system offset of a servo (or all servos)
      *
      * @param servo a string:
      *     * the servo name 
      *     * "ALL" to clear the system offset of all servos
      * @throw MinorServoErrors::OperationNotPermittedEx
      */
     void clearSystemOffset(const char *servo) throw (MinorServoErrors::OperationNotPermittedEx);
     

     /** 
      * Set the system offset of the servo
      *
      * @param servo the servo name
      * @param doubleSeq offset sequence of system offsets to add to the position;
      * one offset for each axis
      * @throw MinorServoErrors::OperationNotPermittedEx
      */
     void setSystemOffset(const char *servo, const ACS::doubleSeq &offset) throw (MinorServoErrors::OperationNotPermittedEx);
     

     /**
      * Return the system offset of a servo
      *
      * @param servo the servo name
      * @return offset the system offset of the servo
      * @throw MinorServoErrors::OperationNotPermittedEx
      */
     ACS::doubleSeq * getSystemOffset(const char *servo) throw (MinorServoErrors::OperationNotPermittedEx);
    
 
     /** Return the active axes names and related units
      *
      * @param axes a sequence of active axes. For instance: 
      * ("SRP_XT", "SRP_YT", "SRP_ZT", "SRP_XR", "SRP_YR", "SRP_ZR", "GFR_ZR")
      * @param units a sequence of strings, each one is the unit of the corresponding axis.
      * For instance: ("mm", "mm", "mm", "degree", "degree", "degree", "mm")
      * @throw MinorServoErrors::ConfigurationErrorEx
      */
     void getAxesInfo(ACS::stringSeq_out axes, ACS::stringSeq_out units)
         throw (CORBA::SystemException, ManagementErrors::ConfigurationErrorEx);



private:

	ContainerServices *m_services;

    /** The last configuration c-string of setup method */
    CString m_config; 

	SimpleParser::CParser<MinorServoBossImpl> *m_parser;

    /** The CDB slaves attribute. Every slave is a Minor Servo to control */
    CString m_cdb_slaves;

    /** Vector of MinorServos on whith we can enable the elevation tracking */
    map<string, bool> m_tracking_list;

    /** Vector of intermediary positions needed to perform a scan */
    vector<ScanPosition> m_scan_pos;

    MSBossConfiguration * m_configuration;

    bool m_scan_active;

    bool m_scanning;

    /** Map of component references */
    map<string, MinorServo::WPServo_var> m_component_refs;
 
    /** Status property */
	SmartPropertyPointer < ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus), POA_Management::ROTSystemStatus> > m_status;
    
    /** Store the value of the property */
    Management::TSystemStatus m_status_value;

    /** Verbose status property */
    // SmartPropertyPointer<ROpattern> m_verbose_status;

    /** Struct of verbose status flags used by SubsystemVStatusDevIO (awful practice) */
    VerboseStatusFlags m_vstatus_flags;

    /** 
     * Return a vector of minor servo to control
     * @return vector of minor servo to control
     */
    vector<string> get_slaves();

    /** @var commanded configuration */
    string m_commanded_conf;

    /** @var actual configuration */
    string m_actual_conf;

    /** @var last servo scanned */
    string m_servo_scanned;

    /** @var thread parameters */
    static MSThreadParameters m_thread_params;

    TrackingThread *m_tracking_thread_ptr;

    SetupThread *m_setup_thread_ptr;

    ParkThread *m_park_thread_ptr;

    ScanThread *m_scan_thread_ptr;

    MSBossPublisher *m_publisher_thread_ptr;
	 
	/** This is the pointer to the notification channel */
	nc::SimpleSupplier *m_nchannel;

    bool slave_exists(string sname);
    
    bool isParked() throw (ManagementErrors::ConfigurationErrorEx);
    
    void clearOffset(const char *servo, string offset_type) throw (MinorServoErrors::OperationNotPermittedEx);

    void setOffset(const char *servo, const ACS::doubleSeq &offset, string offset_type)
        throw (MinorServoErrors::OperationNotPermittedEx);

    ACS::doubleSeq * getOffset(const char *servo, string offset_type) 
        throw (MinorServoErrors::OperationNotPermittedEx);

    void operator=(const MinorServoBossImpl &);
};

         
/** 
 * Return a doubleSeq of positions to set
 * @param comp_name string component name
 * @param token string that contains the polynomial coefficients
 * @return doubleSeq of positions to set
 */
ACS::doubleSeq get_positions(string comp_name, string token, const MSThreadParameters *const params)
    throw (ManagementErrors::ConfigurationErrorExImpl);


/** 
 * Return a doubleSeq of positions to set
 * @param token string that contains the component name
 * @return string the component name
 */
string get_component_name(string token);


/** Return the minimum time needed to move for range mm, with given acceleration and maximum speed */
ACS::Time get_min_time(double range, double acceleration, double max_speed);


#endif
