#ifndef __SRTDEROTATORIMPL_H__
#define __SRTDEROTATORIMPL_H__

/**
 * SRTDerotatorImpl.h
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "SuppressWarnings.h"
//#include <regex>
#include <IRA>
#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <baciROlong.h>
#include <baciRObooleanSeq.h>
#include <baciROdoubleSeq.h>
#include <baciROstring.h>
#include <enumpropROImpl.h>
#include <SRTMinorServoS.h>
#include <maciACSComponentDefines.h>
#include <ManagmentDefinitionsS.h>
#include "ManagementErrors.h"
#include "MinorServoErrors.h"
#include "SRTMinorServoSocket.h"
#include "MSDevIOs.h"
//#include "SRTMinorServoContainers.h"
//#include "SRTMinorServoCommon.h"
#include <SRTDerotatorS.h>


using namespace MinorServo;

/**
 * This class implements the base ACS::CharacteristicComponent CORBA interface for a SRTDerotator component.
 * It is inherited by the other classes declared below.
 */
class SRTDerotatorImpl : public baci::CharacteristicComponentImpl, public virtual POA_MinorServo::SRTDerotator
{
public:
    /**
     * Constructor.
     * @param component_name component's name. This is also the name that will be used to find the configuration data for the component in the Configuration DataBase.
     * @param container_services pointer to the class that exposes all services offered by container.
     * @throw ComponentErrors::ComponentErrorsEx when there has been an issue reading some value from the CDB.
     */
    SRTDerotatorImpl(const ACE_CString& component_name, maci::ContainerServices* container_services);

    /**
     * Destructor.
     */
    virtual ~SRTDerotatorImpl();

    /**
     * Called to give the component time to initialize itself. The component reads in configuration files/parameters, builds up connection.
     * Called before execute. It is implemented as a synchronous (blocking) call.
     * @throw ComponentErrors::ComponentErrorsEx when there has been a memory allocation issue with the properties pointers.
     */
    void initialize();

    /**
     * Called after initialize to tell the component that it has to be ready to accept incoming functional calls any time.
     * Must be implemented as a synchronous (blocking) call. In this class the default implementation only logs the COMPSTATE_OPERATIONAL.
     */
    void execute();

    /**
     * Called by the container before destroying the server in a normal situation. This function takes charge of releasing all resources.
     */
    void cleanUp();

    /**
     * Called by the container in case of error or emergency situation.
     * This function tries to free all resources even though there is no warranty that the function is completely executed before the component is destroyed.
     */
    void aboutToAbort();

    /**
     * Asks the servo system to get ready.
     * @throw DerotatorErrors::DerotatorErrorsEx
     * @throw ComponentErrors::ComponentErrorsEx
     */
    void setup();

    // TODO: this should be a simple preset command
    void setPosition(CORBA::Double position);

    // TODO easy implementation, return last commanded preset or programTrack position
    double getCmdPosition() { return 0.0; };

    // TODO return current position, just read the devio or the AnswerMap
    double getActPosition() { return getPositionFromHistory(0); };

    /**
     * Returns the rotation the derotator was at the given time acs_time.
     * @param acs_time the epoch we want to retrieve the derotator position.
     * @throw MinorServoErrors::MinorServoErrorsEx when the position history is empty.
     * @throw ComponentErrors::ComponentErrorsEx when the position history is empty
     * @return a double object containing the derotator position at the given epoch.
     */
    double getPositionFromHistory(ACS::Time acs_time);

    // TODO: easy implementation, return the min and max values read from the CDB
    double getMinLimit();
    double getMaxLimit();

    // TODO: degrees between 2 lateral feeds. How to implement this with new receivers?
    double getStep() {return 0.0;};

    // TODO: easy implementation
    bool isReady();
    bool isSlewing();

    /**
     * Returns the tracking status of the minor servo.
     * @return true if the minor servo is tracking within the tracking error, false otherwise
     */
    bool isTracking() { return m_tracking.load() == Management::MNG_TRUE; }

    /**
     * Loads a set of tracking coordinates to the minor servo system.
     * @param trajectory_id the ID of the trajectory. This defaults to UNIX Epoch * 1000 (milliseconds precision) of the starting time of the trajectory.
     * @param point_id the ID of the point inside the current trajectory.
     * @param point_time the timestamp associated with the given set of tracking coordinates.
     * @coordinates the given set of tracking coordinates.
     * @throw MinorServoErrors::MinorServoErrorsEx if the length of the coordinates sequence doesn't match the number of virtual axes of the servo,
     *        if the resulting position summing the offsets would go outside the accepted range of the servo,
     *        if there has been a communication error or if the command was not accepted.
     */
    void programTrack(CORBA::Long trajectory_id, CORBA::Long point_id, ACS::Time point_time, const ACS::doubleSeq& coordinates);

    /**
     * Asks the component to calculate the servo system position starting from the given elevation.
     * @param elevation the elevation we want to use to calculate and retrieve the servo system coordinates, expressed in degrees.
     * @throw MinorServoErrors::MinorServoErrorsEx when the servo has not been configured yet and has not loaded any coefficient for the position calculation.
     * @return a pointer to the double sequence object containing the calculated coordinates of the servo system.
     */
    //ACS::doubleSeq* calcCoordinates(CORBA::Double elevation);

    /**
     * Asks the component the virtual axes user offsets.
     * @return a pointer to the double sequence object containing the current virtual axes user offsets of the servo system.
     */
    //ACS::doubleSeq* getUserOffsets();

    /**
     * Load a single virtual axis user offset to the component and to the servo system.
     * @param axis_name the name of the axis to load the given offset to.
     * @param offset the desired user offset, expressed in millimeters or degrees, depending if the axis is a translation axis or a rotation one.
     * @throw MinorServoErrors::MinorServoErrorsEx when the given axis_name is unknown, when the sum of user and system offsets for the given axis are out of range,
     *        when there has been a communication error or when the command was not accepted.
     */
    //void setUserOffset(const char* axis_name, CORBA::Double offset);

    /**
     * Resets the virtual axes user offsets to 0.
     * @throw MinorServoErrors::MinorServoErrorsEx when there has been a communication error or when the command was not accepted.
     */
    //void clearUserOffsets();

    /**
     * Asks the component the virtual axes system offsets.
     * @return a pointer to the double sequence object containing the current virtual axes system offsets of the servo system.
     */
    //ACS::doubleSeq* getSystemOffsets();

    /**
     * Load a single virtual axis system offset to the component and to the servo system.
     * @param axis_name the name of the axis to load the given offset to.
     * @param offset the desired system offset, expressed in millimeters or degrees, depending if the axis is a translation axis or a rotation one.
     * @throw MinorServoErrors::MinorServoErrorsEx when the given axis is unknown, when the sum of user and system offsets for the given axis are out of range,
     *        when there has been a communication error or when the command was not accepted.
     */
    //void setSystemOffset(const char* axis_name, CORBA::Double offset);

    /**
     * Resets the virtual axes system offsets to 0.
     * @throw MinorServoErrors::MinorServoErrorsEx when there has been a communication error or when the command was not accepted.
     */
    //void clearSystemOffsets();

    /**
     * Reload the user and the system offsets to the minor servo when the Leonardo offsets do not correspond to the sum of the DISCOS user and system offsets.
     * @throw MinorServoErrors::MinorServoErrorsEx when there has been a communication error or when the command was not accepted.
     */
    //void reloadOffsets();

    /**
     * Returns the maximum travel time to get from a starting position to a destination position.
     * @param start a double sequence containing the starting position. If the provided sequence is empty, the current axes positions are used for the calculation.
     * @param dest a double sequence containing the destination position.
     * @throw MinorServoErrors::MinorServoErrorsEx when receiving a starting position sequence of length different from zero or the number of virtual axes of the servo,
     *        when receiving a destination position sequence of lenght different from the number of virtual axes of the servo
     * @return an ACS::TimeInterval object representing the total duration of the movement from the starting position to the destination position
     */
    //ACS::TimeInterval getTravelTime(const ACS::doubleSeq& start, const ACS::doubleSeq& dest);

    /**
     * Returns the minimum and maximum range of the virtual axes of the servo system, as reference arguments.
     * @param min_ranges_out a double sequence object containing the minimum ranges of the virtual axes of the servo system.
     * @param max_ranges_out a double sequence object containing the maximum ranges of the virtual axes of the servo system.
     */
    //void getAxesRanges(ACS::doubleSeq_out min_ranges_out, ACS::doubleSeq_out max_ranges_out);

    /**
     * Returns a reference to the enabled property implementation of the IDL interface.
     * @return pointer to the read-only boolean property enabled.
     */
    virtual Management::ROTBoolean_ptr enabled();

    /**
     * Returns a reference to the drive_cabinet_status property implementation of the IDL interface.
     * @return pointer to the read-only SRTMinorServoCabinerStatus (enumeration) property drive_cabinet_status.
     */
    virtual ROSRTMinorServoCabinetStatus_ptr drive_cabinet_status();

    /**
     * Returns a reference to the block property implementation of the IDL interface.
     * @return pointer to the read-only boolean property block.
     */
    virtual Management::ROTBoolean_ptr block();

    /**
     * Returns a reference to the operative_mode property implementation of the IDL interface.
     * @return pointer to the read-only SRTMinorServoOperativeMode (enumeration) property operative_mode.
     */
    virtual ROSRTMinorServoOperativeMode_ptr operative_mode();

    /**
     * Returns a reference to the physical_axes_enabled property implementation of the IDL interface.
     * @return pointer to the read-only boolean sequence property physical_axes_enabled.
     */
    virtual ACS::RObooleanSeq_ptr physical_axes_enabled();

    /**
     * Returns a reference to the physical_positions property implementation of the IDL interface.
     * @return pointer to the read-only boolean sequence property physical_positions.
     */
    virtual ACS::ROdoubleSeq_ptr physical_positions();

    /**
     * Returns a reference to the virtual_axes property implementation of the IDL interface.
     * @return pointer to the read-only long property virtual_axes.
     */
    virtual ACS::ROlong_ptr virtual_axes();

    /**
     * Returns a reference to the plain_virtual_positions property implementation of the IDL interface.
     * @return pointer to the read-only double sequence property plain_virtual_positions.
     */
    virtual ACS::ROdoubleSeq_ptr plain_virtual_positions();

    /**
     * Returns a reference to the virtual_positions property implementation of the IDL interface.
     * @return pointer to the read-only double sequence property virtual_positions.
     */
    virtual ACS::ROdoubleSeq_ptr virtual_positions();

    /**
     * Returns a reference to the virtual_offsets property implementation of the IDL interface.
     * @return pointer to the read-only double sequence property virtual_offsets.
     */
    virtual ACS::ROdoubleSeq_ptr virtual_offsets();

    /**
     * Returns a reference to the tracking property implementation of the IDL interface.
     * @return pointer to the read-only boolean property tracking.
     */
    virtual Management::ROTBoolean_ptr tracking();

    /**
     * Returns a reference to the trajectory_id property implementation of the IDL interface.
     * @return pointer to the read-only long property trajectory_id.
     */
    virtual ACS::ROlong_ptr trajectory_id();

    /**
     * Returns a reference to the total_trajectory_points property implementation of the IDL interface.
     * @return pointer to the read-only long property total_trajectory_points.
     */
    virtual ACS::ROlong_ptr total_trajectory_points();

    /**
     * Returns a reference to the remaining_trajectory_points property implementation of the IDL interface.
     * @return pointer to the read-only long property remaining_trajectory_points.
     */
    virtual ACS::ROlong_ptr remaining_trajectory_points();

    virtual ACS::RWdouble_ptr cmdPosition();

    virtual ACS::ROdouble_ptr actPosition();

    /**
     * Returns a reference to the positionDiff property implementation of the IDL interface.
     * @return pointer to the read-only double sequence property positionDiff.
     */
    virtual ACS::ROdouble_ptr positionDiff();

    virtual ACS::ROpattern_ptr status();

private:
    /**
     * Checks if the socket is connected and if the minor servo system is in a good state.
     * @throw MinorServoErrors::MinorServoErrorsEx when the socket is not connected or when the minor servo system is blocked or the drive cabinet is in error state.
     */
    void checkLineStatus();

    /**
     * Static function used to retrieve some constants from the component CDB xml schema.
     * @param object the instance of this class, used inside the function logic.
     * @param constant the name of the constants we want to retrieve from the CDB.
     * @throw ComponentErrors::ComponentErrorsEx when the requested value cannot be read from the CDB or when it has a non meaningful value.
     * @return a vector of doubles containing the retrieved constants, its length is the same as the number of virtual axes of the servo system.
     */
    //static std::vector<double> getMotionConstant(SRTBaseMinorServoImpl& object, const std::string& constant);

    /**
     * Static function used to retrieve a table from the CDB DataBlock directory. Used inside the initialization list.
     * @param object the instance of this class, used inside the function logic.
     * @param properties_name the name of the block to retrieve as written inside the DataBlock file.
     * @throw ComponentErrors::ComponentErrorsEx when the requested value cannot be read from the CDB.
     * @return a vector of strings containing the retrieved table fields.
     */
    static std::vector<std::string> getPropertiesTable(SRTDerotatorImpl& object, const std::string& properties_name);

    /**
     * Attributes.
     * Keep the same order for the initialization list.
     */

    /**
     * Name of the component.
     */
    const std::string m_component_name;

    /**
     * Name of the servo system.
     */
    const std::string m_servo_name;

    /**
     * Number of virtual axes of the servo system.
     */
    /*const*/ size_t m_virtual_axes;

    /**
     * Number of physical axes of the servo system.
     */
    /*const*/ size_t m_physical_axes;

    /**
     * Name of the virtual axes of the servo system.
     */
    /*const*/ std::vector<std::string> m_virtual_axes_names;

    /**
     * Units of the virtual axes of the servo system.
     */
    /*const*/ std::vector<std::string> m_virtual_axes_units;

    /**
     * Dictionary containing the last status retrieved form the servo system.
     */
    //SRTMinorServoStatus m_status;

    /**
     * Commanded user offsets for each axis of the servo system.
     */
    std::vector<double> m_user_offsets;

    /**
     * Commanded system offsets for each axis of the servo system.
     */
    std::vector<double> m_system_offsets;

    /**
     * Queue of positions assumed by the servo system in time.
     */
    SRTMinorServoPositionsQueue m_positions_queue;

    /**
     * Minimum ranges of the axes of the servo system.
     */
    /*const*/ double m_min;

    /**
     * Maximum ranges of the axes of the servo system.
     */
    /*const*/ double m_max;

    /**
     * Queue of positions to be assumed by the servo system when tracking a trajectory.
     */
    SRTMinorServoPositionsQueue m_tracking_queue;

    /**
     * Tracking delta values for all minor servo system virtual axes.
     */
    /*const*/ std::vector<double> m_tracking_delta;

    /**
     * Tracking error values for all minor servo system virtual axes.
     */
    std::vector<double> m_tracking_error;

    /**
     * Indicates if the servo system is tracking or not. It is tracking when the position error is lower than the tracking delta for all the virtual axes.
     */
    std::atomic<Management::TBoolean> m_tracking;

    /**
     * Current trajectory ID.
     */
    std::atomic<unsigned int> m_trajectory_id;

    /**
     * Total trajectory points of the current trajectory.
     */
    std::atomic<unsigned int> m_total_trajectory_points;

    /**
     * Remaining trajectory points of the current trajectory.
     */
    std::atomic<unsigned int> m_remaining_trajectory_points;

    /**
     * Pointer to the enabled property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>> m_enabled_ptr;

    /**
     * Pointer to the drive_cabinet_status property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(SRTMinorServoCabinetStatus), POA_MinorServo::ROSRTMinorServoCabinetStatus>> m_drive_cabinet_status_ptr;

    /**
     * Pointer to the block property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>> m_block_ptr;

    /**
     * Pointer to the operative_mode property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(SRTMinorServoOperativeMode), POA_MinorServo::ROSRTMinorServoOperativeMode>> m_operative_mode_ptr;

    /**
     * Pointer to the physical_axes_enabled property.
     */
    baci::SmartPropertyPointer<baci::RObooleanSeq> m_physical_axes_enabled_ptr;

    /**
     * Pointer to the physical_positions property.
     */
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_physical_positions_ptr;

    /**
     * Pointer to the virtual_axes property.
     */
    baci::SmartPropertyPointer<baci::ROlong> m_virtual_axes_ptr;

    /**
     * Pointer to the plain_virtual_positions property.
     */
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_plain_virtual_positions_ptr;

    /**
     * Pointer to the virtual_positions property.
     */
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_virtual_positions_ptr;

    /**
     * Pointer to the virtual_offsets property.
     */
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_virtual_offsets_ptr;

    /**
     * Pointer to the tracking property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>> m_tracking_ptr;

    /**
     * Pointer to the trajectory_id property.
     */
    baci::SmartPropertyPointer<baci::ROlong> m_trajectory_id_ptr;

    /**
     * Pointer to the total_trajectory_points property.
     */
    baci::SmartPropertyPointer<baci::ROlong> m_total_trajectory_points_ptr;

    /**
     * Pointer to the remaining_trajectory_points property.
     */
    baci::SmartPropertyPointer<baci::ROlong> m_remaining_trajectory_points_ptr;

    baci::SmartPropertyPointer<baci::ROdouble> m_actual_position_ptr;
    baci::SmartPropertyPointer<baci::ROdouble> m_commanded_position_ptr;
    baci::SmartPropertyPointer<baci::ROdouble> m_position_difference_ptr;
    baci::SmartPropertyPointer<baci::ROpattern> m_status_ptr;

    /**
     * Configuration of the socket object.
     */
    //const SRTMinorServoSocketConfiguration& m_socket_configuration;

    /**
     * Socket object.
     */
    //SRTMinorServoSocket& m_socket;
};

#endif
