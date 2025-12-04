#ifndef __SRTMINORSERVOBOSSIMPL_H__
#define __SRTMINORSERVOBOSSIMPL_H__

/**
 * SRTMinorServoBossImpl.h
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "SuppressWarnings.h"
#include <IRA>
#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <baciROdouble.h>
#include <baciROstring.h>
#include <enumpropROImpl.h>
#include <MinorServoBossS.h>
#include <SRTMinorServoS.h>
#include <SP_parser.h>
#include "MinorServoErrors.h"
#include "SRTMinorServoBossCore.h"
#include "SRTMinorServoStatusThread.h"
#include "MSDevIOs.h"
#include "SRTMinorServoCommon.h"

using namespace MinorServo;

// Forward classes definitions
class SRTMinorServoBossCore;
class SRTMinorServoStatusThread;

class SRTMinorServoBossImpl : public baci::CharacteristicComponentImpl, public virtual POA_MinorServo::SRTMinorServoBoss
{
    friend class SRTMinorServoBossCore;
public:
    /**
     * Constructor.
     * @param component_name the name of the component.
     * @param container_services the ACS container services.
     */
    SRTMinorServoBossImpl(const ACE_CString& component_name, maci::ContainerServices* container_services);

    /**
     * Destructor.
     */
    virtual ~SRTMinorServoBossImpl();

    /**
     * Called to give the component time to initialize itself. The component reads in configuration files/parameters, builds up connection.
     * Called before execute. It is implemented as a synchronous (blocking) call.
     * @throw ComponentErrors::ComponentErrorsEx when there has been a memory allocation issue with the properties pointers.
     */
    virtual void initialize();

    /**
     * Called after initialize to tell the component that it has to be ready to accept incoming functional calls any time.
     * Must be implemented as a synchronous (blocking) call. In this class the default implementation only logs the COMPSTATE_OPERATIONAL.
     */
    virtual void execute();

    /**
     * Called by the container before destroying the server in a normal situation. This function takes charge of releasing all resources.
     */
    virtual void cleanUp();

    /**
     * Called by the container in case of error or emergency situation.
     * This function tries to free all resources even though there is no warranty that the function is completely executed before the component is destroyed.
     */
    virtual void aboutToAbort();

    /**
     * Performs a setup procedure.
     * @param configuration a mnemonic code identifying the desired configuration.
     * @throw ManagementErrors::ConfigurationErrorEx when something went wrong while calling the SRTMinorServoBossCore method implementation.
     */
    virtual void setup(const char* configuration);

    /**
     * Performs a park procedure.
     * @throw ManagementErrors::ConfigurationErrorEx when something went wrong while calling the SRTMinorServoBossCore method implementation.
     */
    virtual void park();

    /**
     * Method that tells if the elevation tracking is enabled.
     * @return a CORBA::Boolean indicating if the elevation tracking is enabled or not.
     */
    virtual CORBA::Boolean isElevationTrackingEn();

    /**
     * Method that tells if the elevation is being tracked.
     * @return a CORBA::Boolean indicating if the system is currently tracking the elevation or not.
     */
    virtual CORBA::Boolean isElevationTracking();

    /**
     * Method that tells if the system is currently tracking the commanded position.
     * @return a CORBA::Boolean indicating if the system is currently tracking below the given error threshold or not.
     */
    virtual CORBA::Boolean isTracking();

    /**
     * Method that tells if the system is currently performing a setup procedure.
     * @return a CORBA::Boolean indicating if the system is currently performing a setup procedure or not.
     */
    virtual CORBA::Boolean isStarting();

    /**
     * Method that tells if the system is using AS coefficients or not.
     * @return a CORBA::Boolean indicating if the system is configured to use AS coefficients or not.
     */
    virtual CORBA::Boolean isASConfiguration();

    /**
     * Method that tells if the system is currently performing a parking procedure.
     * @return a CORBA::Boolean indicating if the system is currently performing a parking procedure or not.
     */
    virtual CORBA::Boolean isParking();

    /**
     * Method that tells if the system was configured correctly.
     * @return a CORBA::Boolean indicating if the system is ready to be moved or not.
     */
    virtual CORBA::Boolean isReady();

    /**
     * Method that tells if the system is currently performing a scan.
     * @return a CORBA::Boolean indicating if the system is currently performing a scan or not.
     */
    virtual CORBA::Boolean isScanning();

    /**
     * Method that tells if the system can currently perform a scan or not.
     * @return a CORBA::Boolean indicating if the system can perform a scan or not.
     */
    virtual CORBA::Boolean isScanActive();

    /**
     * Returns the name of the current focal configuration.
     * @return a string containing the current focal configuration name.
     */
    virtual char* getActualSetup();

    /**
     * Returns the name of the commanded focal configuration.
     * @return a string containing the commanded focal configuration name.
     */
    virtual char* getCommandedSetup();

    /**
     * Returns the central position for the axis involved in the current or last scan.
     * @return a CORBA::Double containing the central position for the current scan axis.
     * @throw MinorServoErrors::MinorServoErrorsEx when no scan has been performed yet.
     */
    virtual CORBA::Double getCentralScanPosition();

    /**
     * Clears the user defined offsets from all the minor servos.
     * @throw MinorServoErrors::MinorServoErrorsEx when a scan is waiting to be completed, when the system is not configured yet, when the given servo_name is unknown,
     *                                             when the given servo is not used in the current focal configuration or when checkLineStatus throws.
     */
    virtual void clearOffsets();

    /**
     * Resets the given servo user offsets to 0.
     * @param servo_name the name of the minor servo the user offsets will be reset to 0.
     * @throw MinorServoErrors::MinorServoErrorsEx when a scan is waiting to be completed, when the system is not configured yet, when the given servo_name is unknown,
     *                                             when the given servo is not used in the current focal configuration or when checkLineStatus throws.
     */
    virtual void clearUserOffset(const char* servo_name);

    /**
     * Sets the given axis' user offset. CORBA IDL implementation.
     * @param servo_axis_name the minor servo and axis names, connected by a _ character.
     * @param offset the desired user offset to be loaded for the given axis.
     * @param log a boolean indicating whether the call comes from the SimpleParser or from outside sources. In case it comes from the SimpleParser, we will log the action, otherwise we won't.
     * @throw MinorServoErrors::MinorServoErrorsEx when a scan is waiting to be completed, when the system is not configured yet, when the given servo or axis name are unknown,
     *                                             when the given servo is not used in the current focal configuration or when checkLineStatus throws.
     */
    virtual void setUserOffset(const char* servo_axis_name, CORBA::Double offset);

    /**
     * Sets the given axis' user offset. SimpleParser implementation.
     * @param servo_axis_name the minor servo and axis names, connected by a _ character.
     * @param offset the desired user offset to be loaded for the given axis.
     * @param log a boolean indicating whether the call comes from the SimpleParser or from outside sources. In case it comes from the SimpleParser, we will log the action, otherwise we won't.
     * @throw MinorServoErrors::MinorServoErrorsEx when a scan is waiting to be completed, when the system is not configured yet, when the given servo or axis name are unknown,
     *                                             when the given servo is not used in the current focal configuration or when checkLineStatus throws.
     */
    virtual void setUserOffset(const char* servo_axis_name, const double& offset);

    /**
     * Retrieves all the current user offsets.
     * @param acs_time the ACS::Time the user wants to retrieve all the axes offsets for.
     * @throw MinorServoErrors::MinorServoErrorsEx when the system is not configured yet.
     * @return a ACS::doubleSeq containing all the axes user offsets. The axes order is the same one retrieved with the getAxesInfo method.
     */
    virtual ACS::doubleSeq* getUserOffset(ACS::Time acs_time);

    /**
     * Resets the given servo system offsets to 0.
     * @param servo_name the name of the minor servo the system offsets will be reset to 0.
     * @throw MinorServoErrors::MinorServoErrorsEx when a scan is waiting to be completed, when the system is not configured yet, when the given servo_name is unknown,
     *                                             when the given servo is not used in the current focal configuration or when checkLineStatus throws.
     */
    virtual void clearSystemOffset(const char* servo_name);

    /**
     * Sets the given axis' system offset.
     * @param servo_axis_name the minor servo and axis names, connected by a _ character.
     * @param offset the desired system offset to be loaded for the given axis.
     * @throw MinorServoErrors::MinorServoErrorsEx when a scan is waiting to be completed, when the system is not configured yet, when the given servo or axis name are unknown,
     *                                             when the given servo is not used in the current focal configuration or when checkLineStatus throws.
     */
    virtual void setSystemOffset(const char* servo_axis_name, CORBA::Double offset);

    /**
     * Retrieves all the current system offsets.
     * @param acs_time the ACS::Time the user wants to retrieve all the axes offsets for.
     * @throw MinorServoErrors::MinorServoErrorsEx when the system is not configured yet.
     * @return a ACS::doubleSeq containing all the axes system offsets. The axes order is the same one retrieved with the getAxesInfo method.
     */
    virtual ACS::doubleSeq* getSystemOffset(ACS::Time acs_time);

    /**
     * Retrieves all the current axes names and units of measure.
     * @param axes_names_out a reference to the sequence in which the method will put all the axes names.
     * @param axes_units_out a reference to the sequence in which the method will put all the axes units of measure.
     * @throw MinorServoErrors::MinorServoErrorsEx when the system is not configured yet.
     */
    virtual void getAxesInfo(ACS::stringSeq_out axes_names, ACS::stringSeq_out axes_units);

    /**
     * Gets the axis involved in the currently or last executed scan.
     * @return a string containing the servo name and the axis name, connected by a _ character.
     */
    virtual char* getScanAxis();

    /**
     * Retrieves all the axes positions for a given epoch.
     * @param acs_time the ACS::Time the user wants to retrieve all the axes positions for.
     * @throw MinorServoErrors::MinorServoErrorsEx when the system is not configured yet.
     * @return an ACS::doubleSeq containing all the axes positions for the given time.
     */
    virtual ACS::doubleSeq* getAxesPosition(ACS::Time acs_time);

    /**
     * Enables or disables the elevation tracking.
     * @param configuration the desired elevation tracking configuration, allowed values are 'on', 'ON', 'off' and 'OFF'.
     * @throw MinorServoErrors::MinorServoErrorsEx when a scan is waiting to be completed or when the passed configuration is unknown.
     */
    virtual void setElevationTracking(const char* elevation_tracking);

    /**
     * Enables or disables the use of ASACTIVE configurations.
     * @param configuration the desired active surface configuration, allowed values are 'on', 'ON', 'off' and 'OFF'.
     * @throw MinorServoErrors::MinorServoErrorsEx when a scan is waiting to be completed or when the passed configuration is unknown.
     */
    virtual void setASConfiguration(const char* as_configuration);

    /**
     * Opens or closes the gregorian cover.
     * @param position the desired position for the gregorian cover, allowed values are 'open', 'OPEN', 'closed' or 'CLOSED'.
     * @throw MinorServoErrors::MinorServoErrorsEx when the commanded position is unknown, when the system is not parked or parking, 
     *                                             when anything goes wrong in the lower communication level or if checkLineStatus throws.
     */
    virtual void setGregorianCoverPosition(const char* position);

    /**
     * Sets the gregorian air blade status.
     * @param status the desired status of the gregorian air blade, allowed values are 'on', 'ON', 'off', 'OFF', 'auto' or 'AUTO'.
     * @throw MinorServoErrors::MinorServoErrorsEx when the commanded status is unknown or the gregorian cover is currently closed,
     *                                             when anything goes wrong in the lower communication level or if checkLineStatus throws.
     */
    virtual void setGregorianAirBladeStatus(const char* status);

    /**
     * This method checks if a scan with the given parameters is feasible.
     * @param start_time the starting ACS::Time for the requested scan.
     * @param scan_info the minor servo scan parameters for the requested scan.
     * @param antenna_info the antenna scan parameters for the requested scan.
     * @param ms_parameters a reference to the object containing the calculated parameters for the requested scan.
     * @return true if the requested scan is feasible, false otherwise.
     */
    virtual CORBA::Boolean checkScan(const ACS::Time start_time, const MinorServoScan& scan_info, const Antenna::TRunTimeParameters& antenna_info, TRunTimeParameters_out ms_parameters);

    /**
     * This method commands the start of a scan to the minor servo system.
     * @param start_time a reference to the ACS::Time object. This value will be written by this method and it will represent the earliest time the minor servo system can perform the requested scan.
     * @param scan_info the minor servo scan parameters for the requested scan.
     * @param antenna_info the antenna scan parameters for the requested scan.
     * @throw MinorServoErrors::MinorServoErrorsEx when there are issues with the line status, with the given parameters or when the scan could not be performed for any reason.
     */
    virtual void startScan(ACS::Time& start_time, const MinorServoScan& scan_info, const Antenna::TRunTimeParameters& antenna_info);

    /**
     * Requests a stop for the ongoing scan.
     * @param close_time the closing time for the ongoing scan.
     * @throw MinorServoErrors::MinorServoErrorsEx when there are issues with the line status.
     */
    virtual void closeScan(ACS::Time& close_time);

    /**
     * Configures the whole minor servo system to a desired position.
     * @param elevation the elevation to use for all the minor servo positions calculation.
     * @throw MinorServoErrors::MinorServoErrorsEx when a scan is waiting to be completed, when the system was not configured yet,
     *                                             when a single minor servo component raises an error or when checkLineStatus throws.
     */
    virtual void preset(double elevation);

    /**
     * This command clears the error status from the component.
     */
    void reset();

    /**
     * Parser method. It executes the desired command.
     */
    virtual CORBA::Boolean command(const char*, CORBA::String_out);

    /**
     * Returns a reference to the status property implementation of the IDL interface.
     * @return pointer to read-only TSystemStatus property status.
     */
    virtual Management::ROTSystemStatus_ptr status();

    /**
     * Returns a reference to the ready property implementation of the IDL interface.
     * @return pointer to read-only TBoolean property ready.
     */
    virtual Management::ROTBoolean_ptr ready();

    /**
     * Returns a reference to the actualSetup property implementation of the IDL interface.
     * @return pointer to read-only string property actualSetup.
     */
    virtual ACS::ROstring_ptr actualSetup();

    /**
     * Returns a reference to the motionInfo property implementation of the IDL interface.
     * @return pointer to read-only string property motionInfo.
     */
    virtual ACS::ROstring_ptr motionInfo();

    /**
     * Returns a reference to the starting property implementation of the IDL interface.
     * @return pointer to read-only TBoolean property starting.
     */
    virtual Management::ROTBoolean_ptr starting();

    /**
     * Returns a reference to the asConfiguration property implementation of the IDL interface.
     * @return pointer to read-only TBoolean property asConfiguration.
     */
    virtual Management::ROTBoolean_ptr asConfiguration();

    /**
     * Returns a reference to the elevationTrack property implementation of the IDL interface.
     * @return pointer to read-only TBoolean property elevationTrack.
     */
    virtual Management::ROTBoolean_ptr elevationTrack();

    /**
     * Returns a reference to the scanActive property implementation of the IDL interface.
     * @return pointer to read-only TBoolean property scanActive.
     */
    virtual Management::ROTBoolean_ptr scanActive();

    /**
     * Returns a reference to the scanning property implementation of the IDL interface.
     * @return pointer to read-only TBoolean property scanning.
     */
    virtual Management::ROTBoolean_ptr scanning();

    /**
     * Returns a reference to the tracking property implementation of the IDL interface.
     * @return pointer to read-only TBoolean property tracking.
     */
    virtual Management::ROTBoolean_ptr tracking();

    /**
     * Returns a reference to the connected property implementation of the IDL interface.
     * @return pointer to read-only TBoolean property connected.
     */
    virtual Management::ROTBoolean_ptr connected();

    /**
     * Returns a reference to the current_configuration property implementation of the IDL interface.
     * @return pointer to read-only SRTMinorServoFocalConfiguration property current_configuration.
     */
    virtual ROSRTMinorServoFocalConfiguration_ptr current_configuration();

    /**
     * Returns a reference to the simulation_enabled property implementation of the IDL interface.
     * @return pointer to read-only TBoolean property simulation_enabled.
     */
    virtual Management::ROTBoolean_ptr simulation_enabled();

    /**
     * Returns a reference to the plc_time property implementation of the IDL interface.
     * @return pointer to read-only double property plc_time.
     */
    virtual ACS::ROdouble_ptr plc_time();

    /**
     * Returns a reference to the plc_version property implementation of the IDL interface.
     * @return pointer to read-only string property plc_version.
     */
    virtual ACS::ROstring_ptr plc_version();

    /**
     * Returns a reference to the control property implementation of the IDL interface.
     * @return pointer to read-only SRTMinorServoControlStatus property control.
     */
    virtual ROSRTMinorServoControlStatus_ptr control();

    /**
     * Returns a reference to the power property implementation of the IDL interface.
     * @return pointer to read-only TBoolean property power.
     */
    virtual Management::ROTBoolean_ptr power();

    /**
     * Returns a reference to the emergency property implementation of the IDL interface.
     * @return pointer to read-only TBoolean property emergency.
     */
    virtual Management::ROTBoolean_ptr emergency();

    /**
     * Returns a reference to the gregorian_cover property implementation of the IDL interface.
     * @return pointer to read-only SRTMinorServoGregorianCoverStatus property gregorian_cover.
     */
    virtual ROSRTMinorServoGregorianCoverStatus_ptr gregorian_cover();

    /**
     * Returns a reference to the air_blade property implementation of the IDL interface.
     * @return pointer to read-only SRTMinorServoGregorianAirBladeStatus property air_blade.
     */
    virtual ROSRTMinorServoGregorianAirBladeStatus_ptr air_blade();

    /**
     * Returns a reference to the last_executed_command property implementation of the IDL interface.
     * @return pointer to read-only double property last_executed_command.
     */
    virtual ACS::ROdouble_ptr last_executed_command();

    /**
     * Returns a reference to the error_code property implementation of the IDL interface.
     * @return pointer to the read-only SRTMinorServoError property error_code.
     */
    virtual ROSRTMinorServoError_ptr error_code();

private:
    /**
     * Component name.
     */
    const std::string m_component_name;

    /**
     * SRTMinorServoBossCore object pointer. No delete is needed since it is handled by the shared_ptr logic.
     */
    const std::shared_ptr<SRTMinorServoBossCore> m_core_ptr;

    /**
     * SRTMinorServoBossCore object reference.
     */
    SRTMinorServoBossCore& m_core;

    /**
     * Command line parser object.
     */
    SimpleParser::CParser<SRTMinorServoBossImpl> m_parser;

    /**
     * Pointer to the connected property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>> m_connected_ptr;

    /**
     * Pointer to the status property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus), POA_Management::ROTSystemStatus>> m_status_ptr;

    /**
     * Pointer to the ready property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>> m_ready_ptr;

    /**
     * Pointer to the actual_setup property.
     */
    baci::SmartPropertyPointer<baci::ROstring> m_actual_setup_ptr;

    /**
     * Pointer to the motion_info property.
     */
    baci::SmartPropertyPointer<baci::ROstring> m_motion_info_ptr;

    /**
     * Pointer to the starting property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>> m_starting_ptr;

    /**
     * Pointer to the as_configuration property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>> m_as_configuration_ptr;

    /**
     * Pointer to the elevation_tracking property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>> m_elevation_tracking_ptr;

    /**
     * Pointer to the scan_active property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>> m_scan_active_ptr;

    /**
     * Pointer to the scanning property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>> m_scanning_ptr;

    /**
     * Pointer to the tracking property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>> m_tracking_ptr;

    /**
     * Pointer to the current_configuration property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(SRTMinorServoFocalConfiguration), POA_MinorServo::ROSRTMinorServoFocalConfiguration>> m_current_configuration_ptr;

    /**
     * Pointer to the simulation_enabled property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>> m_simulation_enabled_ptr;

    /**
     * Pointer to the plc_time property.
     */
    baci::SmartPropertyPointer<baci::ROdouble> m_plc_time_ptr;

    /**
     * Pointer to the plc_version property.
     */
    baci::SmartPropertyPointer<baci::ROstring> m_plc_version_ptr;

    /**
     * Pointer to the control property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(SRTMinorServoControlStatus), POA_MinorServo::ROSRTMinorServoControlStatus>> m_control_ptr;

    /**
     * Pointer to the power property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>> m_power_ptr;

    /**
     * Pointer to the emergency property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>> m_emergency_ptr;

    /**
     * Pointer to the gregorian_cover property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(SRTMinorServoGregorianCoverStatus), POA_MinorServo::ROSRTMinorServoGregorianCoverStatus>> m_gregorian_cover_ptr;

    /**
     * Pointer to the air_blade property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(SRTMinorServoGregorianAirBladeStatus), POA_MinorServo::ROSRTMinorServoGregorianAirBladeStatus>> m_air_blade_ptr;

    /**
     * Pointer to the last_executed_command property.
     */
    baci::SmartPropertyPointer<baci::ROdouble> m_last_executed_command_ptr;

    /**
     * Pointer to the error_code property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(SRTMinorServoError), POA_MinorServo::ROSRTMinorServoError>> m_error_code_ptr;
};

#endif
