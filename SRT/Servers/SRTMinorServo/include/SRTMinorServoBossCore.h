#ifndef __SRTMINORSERVOBOSSCORE_H__
#define __SRTMINORSERVOBOSSCORE_H__

/**
 * SRTMinorServoBossCore.h
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "SuppressWarnings.h"
#include <IRA>
#include <baciSmartPropertyPointer.h>
#include <baciROboolean.h>
#include <baciROdouble.h>
#include <baciROstring.h>
#include <enumpropROImpl.h>
#include <SRTMinorServoBossS.h>
#include <SRTMinorServoS.h>
#include <SRTDerotatorS.h>
#include <AntennaProxy.h>
#include <ManagmentDefinitionsS.h>
#include <thread>
#include <chrono>
#include <LogFilter.h>
#include <slamac.h>
#include <MinorServoErrors.h>
#include <ManagementErrors.h>
#include <ComponentErrors.h>
#include "SRTMinorServoSocket.h"
#include "MSDevIOs.h"
#include "SRTMinorServoBossImpl.h"
#include "SRTMinorServoStatusThread.h"
#include "SRTMinorServoSetupThread.h"
#include "SRTMinorServoParkThread.h"
#include "SRTMinorServoTrackingThread.h"
#include "SRTMinorServoScanThread.h"
#include "SRTMinorServoContainers.h"


_IRA_LOGFILTER_IMPORT;

using namespace MinorServo;

class SRTMinorServoBossImpl;
class SRTMinorServoStatusThread;
class SRTMinorServoSetupThread;
class SRTMinorServoParkThread;
class SRTMinorServoTrackingThread;
class SRTMinorServoScanThread;


/**
 * This class implements the core functionalities for the SRTMinorServoBoss. It is constructed during the SRTMinorServoBossImpl component's construction.
 * It handles all the threads and the procedures necessary for the minor servo system to work properly.
 */
class SRTMinorServoBossCore
{
    /**
     * These classes needs full access to the SRTMinorServoBossCore object methods and attributes in order for the system to work properly.
     */
    friend class SRTMinorServoBossImpl;
    friend class SRTMinorServoStatusThread;
    friend class SRTMinorServoSetupThread;
    friend class SRTMinorServoParkThread;
    friend class SRTMinorServoTrackingThread;
    friend class SRTMinorServoScanThread;

public:
    /**
     * Constructor.
     * @param component a reference to the component object. Used in order to access the properties.
     * @throw ComponentErrors::ComponentErrorsEx when reading configurations from the CDB.
     */
    SRTMinorServoBossCore(SRTMinorServoBossImpl& component);

    /**
     * Destructor.
     */
    virtual ~SRTMinorServoBossCore();

private:
    /**
     * Reads the overall status from the hardware.
     * @return true when the status is OK, false otherwise.
     */
    bool status();

    /**
     * Performs a setup procedure.
     * @param configuration a mnemonic code identifying the desired configuration.
     * @throw ManagementErrors::ConfigurationErrorEx when something went wrong while performing the setup procedure or if checkLineStatus throws.
     */
    void setup(std::string configuration);

    /**
     * Performs a park procedure.
     * @throw ManagementErrors::ParkingErrorEx when something went wrong while performing the park procedure or if checkLineStatus throws.
     */
    void park();

    /**
     * Enables or disables the elevation tracking.
     * @param configuration the desired elevation tracking configuration, allowed values are 'on', 'ON', 'off' and 'OFF'.
     * @throw MinorServoErrors::MinorServoErrorsEx when a scan is waiting to be completed or when the passed configuration is unknown.
     */
    void setElevationTracking(std::string configuration);

    /**
     * Enables or disables the use of ASACTIVE configurations.
     * @param configuration the desired active surface configuration, allowed values are 'on', 'ON', 'off' and 'OFF'.
     * @throw MinorServoErrors::MinorServoErrorsEx when a scan is waiting to be completed or when the passed configuration is unknown.
     */
    void setASConfiguration(std::string configuration);

    /**
     * Opens or closes the gregorian cover.
     * @param position the desired position for the gregorian cover, allowed values are 'open', 'OPEN', 'closed' or 'CLOSED'.
     * @throw MinorServoErrors::MinorServoErrorsEx when the commanded position is unknown, when the system is not parked or parking, 
     *                                             when anything goes wrong in the lower communication level or if checkLineStatus throws.
     */
    void setGregorianCoverPosition(std::string position);

    /**
     * Sets the gregorian air blade status.
     * @param status the desired status for the gregorian air blade, allowed value are 'on', 'ON', 'off', 'OFF', 'auto' or 'AUTO'.
     * @throw MinorServoErrors::MinorServoErrorsEx when the commanded status is unknown or the gregorian cover is currently closed,
     *                                             when anything goes wrong in the lower communication level or if checkLineStatus throws.
     */
    void setGregorianAirBladeStatus(std::string status);

    /**
     * Configures the whole minor servo system to a desired position.
     * @param elevation the elevation to use for all the minor servo positions calculation.
     * @throw MinorServoErrors::MinorServoErrorsEx when a scan is waiting to be completed, when the system was not configured yet,
     *                                             when a single minor servo component raises an error or when checkLineStatus throws.
     */
    void preset(double elevation);

    /**
     * Resets the given servo user offsets to 0.
     * @param servo_name the name of the minor servo the user offsets will be reset to 0.
     * @throw MinorServoErrors::MinorServoErrorsEx when a scan is waiting to be completed, when the system is not configured yet, when the given servo_name is unknown,
     *                                             when the given servo is not used in the current focal configuration or when checkLineStatus throws.
     */
    void clearUserOffsets(std::string servo_name);

    /**
     * Sets the given axis' user offset.
     * @param servo_axis_name the minor servo and axis names, connected by a _ character.
     * @param offset the desired user offset to be loaded for the given axis.
     * @param log a boolean indicating whether the call comes from the SimpleParser or from outside sources. In case it comes from the SimpleParser, we will log the action, otherwise we won't.
     * @throw MinorServoErrors::MinorServoErrorsEx when a scan is waiting to be completed, when the system is not configured yet, when the given servo or axis name are unknown,
     *                                             when the given servo is not used in the current focal configuration or when checkLineStatus throws.
     */
    void setUserOffset(std::string servo_axis_name, double offset, bool log = false);

    /**
     * Retrieves all the current user offsets.
     * @throw MinorServoErrors::MinorServoErrorsEx when the system is not configured yet.
     * @return a ACS::doubleSeq containing all the axes user offsets. The axes order is the same one retrieved with the getAxesInfo method.
     */
    ACS::doubleSeq* getUserOffsets();

    /**
     * Resets the given servo system offsets to 0.
     * @param servo_name the name of the minor servo the system offsets will be reset to 0.
     * @throw MinorServoErrors::MinorServoErrorsEx when a scan is waiting to be completed, when the system is not configured yet, when the given servo_name is unknown,
     *                                             when the given servo is not used in the current focal configuration or when checkLineStatus throws.
     */
    void clearSystemOffsets(std::string servo_name);

    /**
     * Sets the given axis' system offset.
     * @param servo_axis_name the minor servo and axis names, connected by a _ character.
     * @param offset the desired system offset to be loaded for the given axis.
     * @throw MinorServoErrors::MinorServoErrorsEx when a scan is waiting to be completed, when the system is not configured yet, when the given servo or axis name are unknown,
     *                                             when the given servo is not used in the current focal configuration or when checkLineStatus throws.
     */
    void setSystemOffset(std::string servo_axis_name, double offset);

    /**
     * Retrieves all the current system offsets.
     * @throw MinorServoErrors::MinorServoErrorsEx when the system is not configured yet.
     * @return a ACS::doubleSeq containing all the axes system offsets. The axes order is the same one retrieved with the getAxesInfo method.
     */
    ACS::doubleSeq* getSystemOffsets();

    /**
     * Retrieves all the current axes names and units of measure.
     * @param axes_names_out a reference to the sequence in which the method will put all the axes names.
     * @param axes_units_out a reference to the sequence in which the method will put all the axes units of measure.
     * @throw MinorServoErrors::MinorServoErrorsEx when the system is not configured yet.
     */
    void getAxesInfo(ACS::stringSeq_out axes_names_out, ACS::stringSeq_out axes_units_out);

    /**
     * Retrieves all the axes positions for a given epoch.
     * @param acs_time the ACS::Time the user wants to retrieve all the axes positions for.
     * @throw MinorServoErrors::MinorServoErrorsEx when the system is not configured yet.
     * @return an ACS::doubleSeq containing all the axes positions for the given time.
     */
    ACS::doubleSeq* getAxesPositions(ACS::Time acs_time);

    /**
     * This method performs the calculations necessary to check if a scan can be performed starting from the given parameters.
     * @param start_time the starting ACS::Time for the requested scan.
     * @param scan_info the minor servo scan parameters for the requested scan.
     * @param antenna_info the antenna scan parameters for the requested scan.
     * @throw MinorServoErrors::MinorServoErrorsEx when there are issues with the given parameters or when the scan could not be performed for any reason.
     * @return a SRTMinorServoScan object containing the parameters for a feasible scan.
     */
    SRTMinorServoScan checkScanFeasibility(const ACS::Time& start_time, const MinorServoScan& scan_info, const Antenna::TRunTimeParameters& antenna_info);

    /**
     * This method checks if a scan with the given parameters is feasible by calling the checkScanFeasibility method.
     * @param start_time the starting ACS::Time for the requested scan.
     * @param scan_info the minor servo scan parameters for the requested scan.
     * @param antenna_info the antenna scan parameters for the requested scan.
     * @param ms_parameters a reference to the object containing the calculated parameters for the requested scan.
     * @return true if the requested scan is feasible, false otherwise.
     */
    bool checkScan(const ACS::Time start_time, const MinorServoScan& scan_info, const Antenna::TRunTimeParameters& antenna_info, TRunTimeParameters_out ms_parameters);

    /**
     * This method commands the start of a scan to the minor servo system. It calls the checkScanFeasibility method again to be sure the scan can actually be performed, updating the starting time.
     * @param start_time a reference to the ACS::Time object. This value will be written by this method and it will represent the earliest time the minor servo system can perform the requested scan.
     * @param scan_info the minor servo scan parameters for the requested scan.
     * @param antenna_info the antenna scan parameters for the requested scan.
     * @throw MinorServoErrors::MinorServoErrorsEx when there are issues with the line status, with the given parameters or when the scan could not be performed for any reason.
     */
    void startScan(ACS::Time& start_time, const MinorServoScan& scan_info, const Antenna::TRunTimeParameters& antenna_info);

    /**
     * Requests a stop for the ongoing scan.
     * @param close_time the closing time for the ongoing scan.
     * @throw MinorServoErrors::MinorServoErrorsEx when there are issues with the line status.
     */
    void closeScan(ACS::Time& close_time);

    /**
     * Retrieve the antenna elevation from the AntennaBoss component for the given ACS::Time.
     * @param acs_time the ACS::Time we want to know the antenna elevation for.
     * @throw ComponentErrors::ComponentErrorsEx when the AntennaBoss component could not be retrieved.
     * @return the antenna elevation for the given time, expressed in degrees.
     */
    double getElevation(const ACS::Time& acs_time);

    /**
     * Checks if the socket is connected, if the Leonardo minor servo system is currently controlled by DISCOS and if it is not in emergency status.
     * @throw MinorServoErrors::MinorServoErrorsEx when any of the above is false.
     */
    void checkLineStatus();

    /**
     * Method that creates and starts an ACS thread object.
     * It is a template specialized on the SRTMinorServoStatusThread, SRTMinorServoSetupThread, SRTMinorServoParkThread, SRTMinorServoTrackingThread and SRTMinorServoScanThread classes.
     * @param thread pointer to the ACS thread object to be eventually created and started.
     * @param sleep_time the optional thread sleep time, defaults to 0, meaning that this value will be ignored and internally the thread sleep time will be set to the default sleep time.
     */
    template <typename T, typename = std::enable_if<is_any_v<T, SRTMinorServoStatusThread, SRTMinorServoSetupThread, SRTMinorServoParkThread, SRTMinorServoTrackingThread, SRTMinorServoScanThread>>>
    void startThread(T*& thread, const ACS::TimeInterval& sleep_time = 0);

    /**
     * Method that stops an ACS thread object.
     * It is a template specialized on the SRTMinorServoStatusThread, SRTMinorServoSetupThread, SRTMinorServoParkThread, SRTMinorServoTrackingThread and SRTMinorServoScanThread classes.
     * @param thread pointer to the ACS thread object to be stopped.
     */
    template <typename T, typename = std::enable_if<is_any_v<T, SRTMinorServoStatusThread, SRTMinorServoSetupThread, SRTMinorServoParkThread, SRTMinorServoTrackingThread, SRTMinorServoScanThread>>>
    void stopThread(T*& thread);

    /**
     * Method that destroys an ACS Thread object.
     * It is a template specialized on the SRTMinorServoStatusThread, SRTMinorServoSetupThread, SRTMinorServoParkThread, SRTMinorServoTrackingThread and SRTMinorServoScanThread classes.
     * @param thread pointer to the ACS Thread object to be destroyed.
     */
    template <typename T, typename = std::enable_if<is_any_v<T, SRTMinorServoStatusThread, SRTMinorServoSetupThread, SRTMinorServoParkThread, SRTMinorServoTrackingThread, SRTMinorServoScanThread>>>
    void destroyThread(T*& thread);

    /**
     * Method that sets all the necessary variable to signal a failure of the minor servo system.
     * @param error the type of error the system should display
     */
    void setError(SRTMinorServoError error);

    /**
     * Method that clears the error status from the system.
     * This will show the system cleared the error status and is ready to be configured again.
     */
    void reset(bool force=false);

    /**
     * Method used to retrieve a configuration value from the CDB.
     * @param configuration the name of the value to be read from the CDB.
     */
    Management::TBoolean getCDBConfiguration(std::string configuration);

    /**
     * Reference to the component object.
     */
    SRTMinorServoBossImpl& m_component;

    /**
     * Pointer to the status thread.
     */
    SRTMinorServoStatusThread* m_status_thread;

    /**
     * Pointer to the setup thread.
     */
    SRTMinorServoSetupThread* m_setup_thread;

    /**
     * Pointer to the park thread.
     */
    SRTMinorServoParkThread* m_park_thread;

    /**
     * Pointer to the tracking thread.
     */
    SRTMinorServoTrackingThread* m_tracking_thread;

    /**
     * Pointer to the scan thread.
     */
    SRTMinorServoScanThread* m_scan_thread;

    /**
     * Pointer to the AntennaBoss component.
     */
    Antenna::AntennaBoss_proxy m_antennaBoss;

    /**
     * SRTMinorServoGeneralStatus object containing the status read from the PLC.
     */
    SRTMinorServoGeneralStatus m_status;

    /**
     * Enumeration indicating the status of the motion of the minor servo system.
     */
    std::atomic<SRTMinorServoMotionStatus> m_motion_status;

    /**
     * String containing the commanded focal configuration.
     */
    std::string m_commanded_setup;

    /**
     * Enumeration containing the commanded focal configuration.
     */
    std::atomic<SRTMinorServoFocalConfiguration> m_commanded_configuration;

    /**
     * Enumeration containing the status of the subsystem.
     */
    std::atomic<Management::TSystemStatus> m_subsystem_status;

    /**
     * String containing the current focal configuration name.
     */
    std::string m_actual_setup;

    /**
     * Boolean indicating whether the system is ready or not.
     */
    std::atomic<Management::TBoolean> m_ready;

    /**
     * Boolean indicating whether the system is performing a setup procedure or not.
     */
    std::atomic<Management::TBoolean> m_starting;

    /**
     * Boolean indicating whether the system is using ASACTIVE configurations or not.
     */
    std::atomic<Management::TBoolean> m_as_configuration;

    /**
     * Boolean indicating whether the system is currently tracking the elevation.
     */
    std::atomic<Management::TBoolean> m_elevation_tracking;

    /**
     * Boolean indicating whether the tracking of the elevation is enabled.
     */
    std::atomic<Management::TBoolean> m_elevation_tracking_enabled;

    /**
     * Boolean indicating whether the system can perform scans or not.
     */
    std::atomic<Management::TBoolean> m_scan_active;

    /**
     * Boolean indicating whether the system is scanning or not.
     */
    std::atomic<Management::TBoolean> m_scanning;

    /**
     * Boolean indicating whether the system is tracking or not.
     */
    std::atomic<Management::TBoolean> m_tracking;

    /**
     * Enumerator containing the error code.
     */
    std::atomic<SRTMinorServoError> m_error_code;

    /**
     * This boolean will be set to true every time the socket connects.
     * When true it will trigger a procedure that will check if the minor servos offsets need to be reloaded because of a discrepancy between the DISCOS offsets (user + system) and the Leonardo offsets.
     */
    bool m_reload_servo_offsets;

    /**
     * Configuration of the socket object.
     */
    const SRTMinorServoSocketConfiguration& m_socket_configuration;

    /**
     * Socket object.
     */
    SRTMinorServoSocket& m_socket;

    /**
     * Boolean indicating whether the socket is connected or not.
     */
    std::atomic<Management::TBoolean> m_socket_connected;

    /**
     * Map containing all the servos in the minor servo system.
     */
    const std::map<std::string, SRTBaseMinorServo_ptr> m_servos;

    /**
     * Map containing all the tracking servos in the minor servo system.
     */
    const std::map<std::string, SRTProgramTrackMinorServo_ptr> m_tracking_servos;

    /**
     * Map containing all the derotators. It is only used to update the derotators statuses.
     */
    const std::map<std::string, SRTDerotator_ptr> m_derotators;

    /**
     * Map that will dynamically be updated containing the current configuration's minor servos.
     */
    std::map<std::string, SRTBaseMinorServo_ptr> m_current_servos;

    /**
     * Map that will dynamically be updated containing the current configuration's tracking minor servos.
     */
    std::map<std::string, SRTProgramTrackMinorServo_ptr> m_current_tracking_servos;

    /**
     * Current scan parameters.
     */
    SRTMinorServoScan m_current_scan;

    /**
     * Last scan parameters.
     */
    SRTMinorServoScan m_last_scan;
};

#endif
