/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef __BOSSCONFIGURATION_H__
#define __BOSSCONFIGURATION_H__


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <IRA>
#include <ComponentErrors.h>
#include <vector>
#include <string>
#include <map>
#include <baciROdoubleSeq.h>
#include <MinorServoS.h>
#include <WPServoImpl.h>
#include <ManagementErrors.h>
#include <ManagmentDefinitionsS.h>
#include <macros.def>
#include <SP_parser.h>
#include <SRTActiveSurfaceBossS.h>
#include <acsncSimpleSupplier.h>
#include <AntennaBossC.h>

struct MinorServoBossImpl;

const std::string actions_separator = "@";
const std::string items_separator = ":";
const std::string pos_separator = ",";
const std::string park_token = "park";
const std::string boundary_tokens = "()";
const std::string slaves_separator = ",";
const std::string coeffs_separator = ";";
const std::string coeffs_id = "=";

const double MIN_ELEVATION = 5; // In decimals

struct InfoAxisCode {
    size_t axis_id;
    std::string comp_name;
    size_t numberOfAxes;
};


/**
 * This class stores the actual minor servo system configuration. The data inside this class are 
 * initialized at the startup from the configuration database and then are used (read) inside the component.
 * @author <a href=mailto:mbuttu@oa-cagliari.inaf.it>Marco Buttu</a>,
 * INAF Osservatorio Astronomico di Cagliari, Italia
 * <br> 
  */
class MSBossConfiguration {

public:

    /** Default constructor */
    MSBossConfiguration(maci::ContainerServices *Services, MinorServoBossImpl *bossImpl);

    /** Destructor */
    ~MSBossConfiguration();
    
    /** if keepSetup is true, then the system is not performig a real setup, but it is just changing the configuration */
    void init(std::string commandedSetup, bool keepSetup=false) throw (ManagementErrors::ConfigurationErrorExImpl);

    inline bool isConfigured() { return m_isConfigured; }

    bool isTracking();

    inline bool isStarting() { return m_isStarting; }

    inline bool isParking() { return m_isParking; }

    inline bool isElevationTracking() { return m_isElevationTracking; }

    inline bool isElevationTrackingEn() { return m_isElevationTrackingEn; }
    
    inline bool isScanning() { return m_isScanning; }

    inline bool isScanActive() { return m_isScanActive; }

    inline bool isValidCDBConfiguration() { return m_isValidCDBConfiguration; }

    inline std::string getActualSetup() { return m_actualSetup; }

    std::string motionInfo();

    inline std::string getCommandedSetup() { return m_commandedSetup; }

    /** Return true when we are using the ASACTIVE cofiguration */
    inline bool isASConfiguration() { return m_isASConfiguration; }

    inline std::vector<std::string> getServosToPark() { return m_servosToPark; }

    inline std::vector<std::string> getServosToMove() { return m_servosToMove; }

    inline std::vector<std::string> getDynamicComponents() { return m_dynamic_comps; }

    inline std::vector<std::string> getAxes() { return m_axes; }

    inline std::vector<std::string> getUnits() { return m_units; }

    // Return the name of the servo active in the primary focus
    inline std::string getActivePFocusServo() { return m_active_pfocus_servo; }

    void setElevationTracking(IRA::CString value) throw (ManagementErrors::ConfigurationErrorExImpl);
 
    void setASConfiguration(IRA::CString flag) throw (ManagementErrors::ConfigurationErrorExImpl);

    InfoAxisCode getInfoFromAxisCode(string axis_code) throw (ManagementErrors::ConfigurationErrorExImpl);

    struct Scan {
        ACS::Time starting_time;
        ACS::Time total_time;
        ACS::Time delta_time;
        double range;
        std::string comp_name;
        unsigned short axis_index;
        std::string axis_code;
        ACS::doubleSeq actPos;
        ACS::doubleSeq centralPos;
        ACS::doubleSeq plainCentralPos;
        bool wasElevationTrackingEn;
    };

    /** 
     * Return a doubleSeq of positions to set
     * @param comp_name string component name
     * @param elevation the elevation (radians) of the antenna at the beginning of the scan
     * @return doubleSeq of positions to set
     */
    ACS::doubleSeq getPositionFromElevation(string comp_name, double elevation) 
        throw (ManagementErrors::ConfigurationErrorExImpl);

    /** 
     * Return a doubleSeq of positions to set
     * @param comp_name string component name
     * @param time the time we want to get the elevation the antenna will have at the time `time` 
     * @return doubleSeq of positions to set
     */
    ACS::doubleSeq getPosition(string comp_name, ACS::Time time)
        throw (ManagementErrors::ConfigurationErrorExImpl);

    void setScan(
            const ACS::Time starting_time, 
            const ACS::Time total_time, 
            const ACS::Time delta_time,
            const double range, 
            const std::string comp_name, 
            const unsigned short axis_index, 
            const std::string axis_code,
            const ACS::doubleSeq actPos,
            const ACS::doubleSeq centralPos,
            const ACS::doubleSeq plainCentralPos,
            bool wasElevationTrackingEn
    );

    inline Scan getScanInfo() { return m_scan; }

    std::string m_actualSetup;
    std::string m_commandedSetup;
    std::string m_baseSetup;
    std::string m_active_pfocus_servo;
    bool m_isConfigured;
    bool m_isStarting;
    bool m_isScanLocked;
    bool m_isScanning;
    bool m_isScanActive;
    bool m_isParking;
    bool m_isValidCDBConfiguration;
    bool m_isElevationTracking; // Is it tracking?
    bool m_isElevationTrackingEn; // Is the tracking enabled?
    bool m_isTracking;
    bool m_trackingError; // Set from an TrackingThread;
    bool m_isASConfiguration;

    Scan m_scan;

    std::vector<std::string> m_servosToPark;

    std::vector<std::string> m_servosToMove;

    std::vector<std::string> m_servosToPowerOffEncoder;

    /** Components with number of coefficients greather than 1 */
    std::vector<std::string> m_dynamic_comps;

    /** Active axes */
    std::vector<std::string> m_axes;
    /** Axes units */
    std::vector<std::string> m_units;

    std::map<std::string, std::vector<std::vector<double> > > m_servosCoefficients;

    Management::TSystemStatus m_status;

    /** Map of component references */
    map<string, MinorServo::WPServo_var> m_component_refs;

    /** Active surface boss references */
    ActiveSurface::SRTActiveSurfaceBoss_var m_as_ref;

	/** This is the pointer to the notification channel */
	nc::SimpleSupplier *m_nchannel;

    maci::ContainerServices * m_services;
    Antenna::AntennaBoss_var m_antennaBoss;
    bool m_antennaBossError;

    MinorServoBossImpl * m_bossImpl_ptr;
};


#endif
