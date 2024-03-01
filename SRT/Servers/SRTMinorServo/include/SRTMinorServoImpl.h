#ifndef __SRTMINORSERVOIMPL_H__
#define __SRTMINORSERVOIMPL_H__

/**
 * SRTMinorServoImpl.h
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "SuppressWarnings.h"
#include <regex>
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
#include "SRTMinorServoContainers.h"
#include "SRTMinorServoCommon.h"


using namespace MinorServo;

/**
 * This class implements the base ACS::CharacteristicComponent CORBA interface for a SRTMinorServo component.
 * It is inherited by the other classes declared below.
 */
class SRTBaseMinorServoImpl : public baci::CharacteristicComponentImpl
{
public:
    /**
     * Constructor.
     * @param component_name component's name. This is also the name that will be used to find the configuration data for the component in the Configuration DataBase.
     * @param container_services pointer to the class that exposes all services offered by container.
     * @throw ComponentErrors::ComponentErrorsEx when there has been an issue reading some value from the CDB.
     */
    SRTBaseMinorServoImpl(const ACE_CString& component_name, maci::ContainerServices* container_services);

    /**
     * Destructor.
     */
    virtual ~SRTBaseMinorServoImpl();

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
     * Asks the hardware the status of the servo system and updates the component properties.
     * @return true if the communication succeeded, false otherwise.
     * @throw MinorServoErrors::MinorServoErrorsEx when trying to reset the offsets when they don't match the ones loaded into the hardware.
     */
    bool status();

    /**
     * Asks the servo system to perform a STOW operation.
     * @param stow_position the position to get stowed into.
     * @throw MinorServoErrors::MinorServoErrorsEx if there has been a communication error or if the command was not accepted.
     */
    void stow(CORBA::Long stow_position = 1);

    /**
     * Asks the servo system to perform a STOP operation.
     * @throw MinorServoErrors::MinorServoErrorsEx if there has been a communication error or if the command was not accepted.
     */
    void stop();

    /**
     * Asks the servo system to perform a PRESET operation.
     * @param coordinates the sequence of coordinates to get into position to, the sequence length must be equal to the number of virtual axes of the servo system.
     * @throw MinorServoErrors::MinorServoErrorsEx if the length of the coordinates sequence doesn't match the number of virtual axes of the servo,
     *        if the resulting position summing the offsets would go outside the accepted range of the servo,
     *        if there has been a communication error or if the command was not accepted.
     */
    void preset(const ACS::doubleSeq& coordinates);

    /**
     * Asks the servo system to load the commanded configuration table.
     * @param configuration_name the configuration the servo system should assume.
     * @throw ComponentErrors::ComponentErrorsEx when there is an error while trying to load the table for the given configuration.
     * @return true if the servo is in use with the current configuration, false otherwise
     */
    bool setup(const char* configuration_name = "");

    /**
     * Asks the component to calculate the servo system position starting from the given elevation.
     * @param elevation the elevation we want to use to calculate and retrieve the servo system coordinates, expressed in degrees.
     * @throw MinorServoErrors::MinorServoErrorsEx when the servo has not been configured yet and has not loaded any coefficient for the position calculation.
     * @return a pointer to the double sequence object containing the calculated coordinates of the servo system.
     */
    ACS::doubleSeq* calcCoordinates(CORBA::Double elevation);

    /**
     * Asks the component the virtual axes user offsets.
     * @return a pointer to the double sequence object containing the current virtual axes user offsets of the servo system.
     */
    ACS::doubleSeq* getUserOffsets();

    /**
     * Load a single virtual axis user offset to the component and to the servo system.
     * @param axis_name the name of the axis to load the given offset to.
     * @param offset the desired user offset, expressed in millimeters or degrees, depending if the axis is a translation axis or a rotation one.
     * @throw MinorServoErrors::MinorServoErrorsEx when the given axis_name is unknown, when the sum of user and system offsets for the given axis are out of range,
     *        when there has been a communication error or when the command was not accepted.
     */
    void setUserOffset(const char* axis_name, CORBA::Double offset);

    /**
     * Resets the virtual axes user offsets to 0.
     * @throw MinorServoErrors::MinorServoErrorsEx when there has been a communication error or when the command was not accepted.
     */
    void clearUserOffsets();

    /**
     * Asks the component the virtual axes system offsets.
     * @return a pointer to the double sequence object containing the current virtual axes system offsets of the servo system.
     */
    ACS::doubleSeq* getSystemOffsets();

    /**
     * Load a single virtual axis system offset to the component and to the servo system.
     * @param axis_name the name of the axis to load the given offset to.
     * @param offset the desired system offset, expressed in millimeters or degrees, depending if the axis is a translation axis or a rotation one.
     * @throw MinorServoErrors::MinorServoErrorsEx when the given axis is unknown, when the sum of user and system offsets for the given axis are out of range,
     *        when there has been a communication error or when the command was not accepted.
     */
    void setSystemOffset(const char* axis_name, CORBA::Double offset);

    /**
     * Resets the virtual axes system offsets to 0.
     * @throw MinorServoErrors::MinorServoErrorsEx when there has been a communication error or when the command was not accepted.
     */
    void clearSystemOffsets();

    /**
     * Reload the user and the system offsets to the minor servo when the Leonardo offsets do not correspond to the sum of the DISCOS user and system offsets.
     * @throw MinorServoErrors::MinorServoErrorsEx when there has been a communication error or when the command was not accepted.
     */
    void reloadOffsets();

    /**
     * Returns the name and the unit of each virtual axes of the servo system, as reference arguments.
     * @param axes_names_out a string sequence object containing the names of the virtual axes of the servo system.
     * @param axes_units_out a string sequence object containing the units of the virtual axes of the servo system.
     */
    void getAxesInfo(ACS::stringSeq_out axes_names_out, ACS::stringSeq_out axes_units_out);

    /**
     * Returns the virtual axes positions to where the servo system was at the given time acs_time.
     * @param acs_time the epoch we want to retrieve the axes virtual positions of the servo system.
     * @throw MinorServoErrors::MinorServoErrorsEx when the position history is empty.
     * @return a pointer to the double sequence object containing the virtual axes positions at the given epoch.
     */
    ACS::doubleSeq* getAxesPositions(ACS::Time acs_time);

    /**
     * Returns the maximum travel time to get from a starting position to a destination position.
     * @param start a double sequence containing the starting position. If the provided sequence is empty, the current axes positions are used for the calculation.
     * @param dest a double sequence containing the destination position.
     * @throw MinorServoErrors::MinorServoErrorsEx when receiving a starting position sequence of length different from zero or the number of virtual axes of the servo,
     *        when receiving a destination position sequence of lenght different from the number of virtual axes of the servo
     * @return an ACS::TimeInterval object representing the total duration of the movement from the starting position to the destination position
     */
    ACS::TimeInterval getTravelTime(const ACS::doubleSeq& start, const ACS::doubleSeq& dest);

    /**
     * Returns the minimum and maximum range of the virtual axes of the servo system, as reference arguments.
     * @param min_ranges_out a double sequence object containing the minimum ranges of the virtual axes of the servo system.
     * @param max_ranges_out a double sequence object containing the maximum ranges of the virtual axes of the servo system.
     */
    void getAxesRanges(ACS::doubleSeq_out min_ranges_out, ACS::doubleSeq_out max_ranges_out);

    /**
     * Returns a reference to the enabled property implementation of the IDL interface.
     * @return pointer to read-only boolean property enabled.
     */
    virtual Management::ROTBoolean_ptr enabled();

    /**
     * Returns a reference to the drive_cabinet_status property implementation of the IDL interface.
     * @return pointer to read-only SRTMinorServoCabinerStatus (enumeration) property drive_cabinet_status.
     */
    virtual ROSRTMinorServoCabinetStatus_ptr drive_cabinet_status();

    /**
     * Returns a reference to the block property implementation of the IDL interface.
     * @return pointer to read-only boolean property block.
     */
    virtual Management::ROTBoolean_ptr block();

    /**
     * Returns a reference to the operative_mode property implementation of the IDL interface.
     * @return pointer to read-only SRTMinorServoOperativeMode (enumeration) property operative_mode.
     */
    virtual ROSRTMinorServoOperativeMode_ptr operative_mode();

    /**
     * Returns a reference to the physical_axes_enabled property implementation of the IDL interface.
     * @return pointer to read-only boolean sequence property physical_axes_enabled.
     */
    virtual ACS::RObooleanSeq_ptr physical_axes_enabled();

    /**
     * Returns a reference to the physical_positions property implementation of the IDL interface.
     * @return pointer to read-only boolean sequence property physical_positions.
     */
    virtual ACS::ROdoubleSeq_ptr physical_positions();

    /**
     * Returns a reference to the virtual_axes property implementation of the IDL interface.
     * @return pointer to read-only long property virtual_axes.
     */
    virtual ACS::ROlong_ptr virtual_axes();

    /**
     * Returns a reference to the plain_virtual_positions property implementation of the IDL interface.
     * @return pointer to read-only double sequence property plain_virtual_positions.
     */
    virtual ACS::ROdoubleSeq_ptr plain_virtual_positions();

    /**
     * Returns a reference to the virtual_positions property implementation of the IDL interface.
     * @return pointer to read-only double sequence property virtual_positions.
     */
    virtual ACS::ROdoubleSeq_ptr virtual_positions();

    /**
     * Returns a reference to the virtual_offsets property implementation of the IDL interface.
     * @return pointer to read-only double sequence property virtual_offsets.
     */
    virtual ACS::ROdoubleSeq_ptr virtual_offsets();

    /**
     * Returns a reference to the virtual_user_offsets property implementation of the IDL interface.
     * @return pointer to read-only double sequence property virtual_user_offsets.
     */
    virtual ACS::ROdoubleSeq_ptr virtual_user_offsets();

    /**
     * Returns a reference to the virtual_system_offsets property implementation of the IDL interface.
     * @return pointer to read-only double sequence property virtual_system_offsets.
     */
    virtual ACS::ROdoubleSeq_ptr virtual_system_offsets();

    /**
     * Returns a reference to the in_use property implementation of the IDL interface.
     * @return pointer to read-only boolean property in_use.
     */
    virtual Management::ROTBoolean_ptr in_use();

    /**
     * Returns a reference to the current_setup property implementation of the IDL interface.
     * @return pointer to read-only string property current_setup.
     */
    virtual ACS::ROstring_ptr current_setup();

protected:
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
    static std::vector<double> getMotionConstant(SRTBaseMinorServoImpl& object, const std::string& constant);

private:
    /**
     * Static function used to retrieve a table from the CDB DataBlock directory. Used inside the initialization list.
     * @param object the instance of this class, used inside the function logic.
     * @param properties_name the name of the block to retrieve as written inside the DataBlock file.
     * @throw ComponentErrors::ComponentErrorsEx when the requested value cannot be read from the CDB.
     * @return a vector of strings containing the retrieved table fields.
     */
    static std::vector<std::string> getPropertiesTable(SRTBaseMinorServoImpl& object, const std::string& properties_name);

    /**
     * Attributes.
     * Keep the same order for the initialization list.
     */
protected:
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
    const size_t m_virtual_axes;
private:
    /**
     * Number of physical axes of the servo system.
     */
    const size_t m_physical_axes;

    /**
     * Name of the virtual axes of the servo system.
     */
    const std::vector<std::string> m_virtual_axes_names;

    /**
     * Units of the virtual axes of the servo system.
     */
    const std::vector<std::string> m_virtual_axes_units;
protected:
    /**
     * Dictionary containing the last status retrieved form the servo system.
     */
    SRTMinorServoStatus m_status;

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
    const std::vector<double> m_min;

    /**
     * Maximum ranges of the axes of the servo system.
     */
    const std::vector<double> m_max;
private:
    /**
     * Maximum speeds of the axes of the servo system.
     */
    const std::vector<double> m_m_s;

    /**
     * Accelerations of the axes of the servo system.
     */
    const std::vector<double> m_a;

    /**
     * Times to perform a full acceleration ramp from 0 to maximum speed, for each axis.
     */
    const std::vector<double> m_r_t;

    /**
     * Distances covered by a full acceleration ramp from 0 to maximum speed, for each axis.
     */
    const std::vector<double> m_r_d;

    /**
     * Current speed of the axes of the servo system.
     */
    std::vector<double> m_c_s;

    /**
     * Boolean indicating whether the servo system is used in the current focal configuration.
     */
    std::atomic<Management::TBoolean> m_in_use;

    /**
     * Current active setup name.
     */
    std::string m_current_setup;

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
     * Pointer to the virtual_user_offsets property.
     */
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_virtual_user_offsets_ptr;

    /**
     * Pointer to the virtual_system_offsets property.
     */
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_virtual_system_offsets_ptr;

    /**
     * Pointer to the in_use property.
     */
    baci::SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>> m_in_use_ptr;

    /**
     * Pointer to the current_setup property.
     */
    baci::SmartPropertyPointer<baci::ROstring> m_current_setup_ptr;

    /**
     * Table containing the coefficients for the positions calculations.
     */
    SRTMinorServoLookupTable m_current_lookup_table;

    /**
     * Configuration of the socket object.
     */
    const SRTMinorServoSocketConfiguration& m_socket_configuration;
protected:
    /**
     * Socket object.
     */
    SRTMinorServoSocket& m_socket;
};

/**
 * MACRO definition of child classes methods.
 * This simplifies the declaration since these methods' implementations are the same for both SRTGenericMinorServo and SRTProgramTrackMinorServo classes.
 * This was necessary since these are pure virtual methods in POA_MinorServo::SRTGenericMinorServo and POA_MinorServo::SRTProgramTrackMinorServo,
 * and even if they are inherited from the SRTBaseMinorServo class they are not seen by the compiler and they must be declared inside the respective classes.
 * Take a look to the SRTBaseMinorServoImpl class for more information for each of these methods.
 */
#define METHODS_DECLARATION                                                                                                                                              \
    void stow(CORBA::Long stow_position = 1)                                                    { SRTBaseMinorServoImpl::stow(stow_position);                           }\
    void stop()                                                                                 { SRTBaseMinorServoImpl::stop();                                        }\
    void preset(const ACS::doubleSeq& coordinates)                                              { SRTBaseMinorServoImpl::preset(coordinates);                           }\
    ACS::doubleSeq* calcCoordinates(CORBA::Double elevation)                                    { return SRTBaseMinorServoImpl::calcCoordinates(elevation);             }\
    ACS::doubleSeq* getUserOffsets()                                                            { return SRTBaseMinorServoImpl::getUserOffsets();                       }\
    void setUserOffset(const char* axis_name, CORBA::Double offset)                             { SRTBaseMinorServoImpl::setUserOffset(axis_name, offset);              }\
    void clearUserOffsets()                                                                     { SRTBaseMinorServoImpl::clearUserOffsets();                            }\
    ACS::doubleSeq* getSystemOffsets()                                                          { return SRTBaseMinorServoImpl::getSystemOffsets();                     }\
    void setSystemOffset(const char* axis_name, CORBA::Double offset)                           { SRTBaseMinorServoImpl::setSystemOffset(axis_name, offset);            }\
    void clearSystemOffsets()                                                                   { SRTBaseMinorServoImpl::clearSystemOffsets();                          }\
    void reloadOffsets()                                                                        { SRTBaseMinorServoImpl::reloadOffsets();                               }\
    void getAxesInfo(ACS::stringSeq_out axes_names_out, ACS::stringSeq_out axes_units_out)      { SRTBaseMinorServoImpl::getAxesInfo(axes_names_out, axes_units_out);   }\
    ACS::doubleSeq* getAxesPositions(ACS::Time acs_time)                                        { return SRTBaseMinorServoImpl::getAxesPositions(acs_time);             }\
    long getTravelTime(const ACS::doubleSeq& start, const ACS::doubleSeq& dest)                 { return SRTBaseMinorServoImpl::getTravelTime(start, dest);             }\
    void getAxesRanges(ACS::doubleSeq_out min_ranges_out, ACS::doubleSeq_out max_ranges_out)    { SRTBaseMinorServoImpl::getAxesRanges(min_ranges_out, max_ranges_out); }

/**
 * MACRO definition of child classes properties methods.
 * This simplifies the declaration since these properties' methods implementations are the same for both SRTGenericMinorServo and SRTProgramTrackMinorServo classes.
 * This was necessary since these are pure virtual methods in POA_MinorServo::SRTGenericMinorServo and POA_MinorServo::SRTProgramTrackMinorServo,
 * and even if they are inherited from the SRTBaseMinorServo class they are not seen by the compiler and they must be declared inside the respective classes.
 * Take a look to the SRTBaseMinorServoImpl class for more information for each of these properties methods.
 */
#define PROPERTIES_DECLARATION                                                                                                                                           \
    virtual Management::ROTBoolean_ptr enabled()                                                { return SRTBaseMinorServoImpl::enabled();                              }\
    virtual ROSRTMinorServoCabinetStatus_ptr drive_cabinet_status()                             { return SRTBaseMinorServoImpl::drive_cabinet_status();                 }\
    virtual Management::ROTBoolean_ptr block()                                                  { return SRTBaseMinorServoImpl::block();                                }\
    virtual ROSRTMinorServoOperativeMode_ptr operative_mode()                                   { return SRTBaseMinorServoImpl::operative_mode();                       }\
    virtual ACS::RObooleanSeq_ptr physical_axes_enabled()                                       { return SRTBaseMinorServoImpl::physical_axes_enabled();                }\
    virtual ACS::ROdoubleSeq_ptr physical_positions()                                           { return SRTBaseMinorServoImpl::physical_positions();                   }\
    virtual ACS::ROlong_ptr virtual_axes()                                                      { return SRTBaseMinorServoImpl::virtual_axes();                         }\
    virtual ACS::ROdoubleSeq_ptr plain_virtual_positions()                                      { return SRTBaseMinorServoImpl::plain_virtual_positions();              }\
    virtual ACS::ROdoubleSeq_ptr virtual_positions()                                            { return SRTBaseMinorServoImpl::virtual_positions();                    }\
    virtual ACS::ROdoubleSeq_ptr virtual_offsets()                                              { return SRTBaseMinorServoImpl::virtual_offsets();                      }\
    virtual ACS::ROdoubleSeq_ptr virtual_user_offsets()                                         { return SRTBaseMinorServoImpl::virtual_user_offsets();                 }\
    virtual ACS::ROdoubleSeq_ptr virtual_system_offsets()                                       { return SRTBaseMinorServoImpl::virtual_system_offsets();               }\
    virtual Management::ROTBoolean_ptr in_use()                                                 { return SRTBaseMinorServoImpl::in_use();                               }\
    virtual ACS::ROstring_ptr current_setup()                                                   { return SRTBaseMinorServoImpl::current_setup();                        }

/**
 * This class implements the SRTGenericMinorServoImpl CORBA interface for a generic SRTMinorServo component.
 * It inherits from the SRTBaseMinorServoImpl class.
 * A SRTGenericMinorServo component does not need to implement any tracking feature.
 */
class SRTGenericMinorServoImpl: public SRTBaseMinorServoImpl, public virtual POA_MinorServo::SRTGenericMinorServo
{
public:
    /**
     * Constructor
     * @param component_name component's name. This is also the name that will be used to find the configuration data for the component in the Configuration DataBase.
     * @param container_services pointer to the class that exposes all services offered by container.
     */
    SRTGenericMinorServoImpl(const ACE_CString &component_name, maci::ContainerServices *container_services);

    /**
     * Destructor.
     */
    ~SRTGenericMinorServoImpl();

    /**
     * Status method definition. It simply calls and returns the SRTBaseMinorServoImpl method.
     */
    bool status()                                                                               { return SRTBaseMinorServoImpl::status();                               }

    /**
     * Setup method definition. It simply calls the SRTBaseMinorServoImpl method.
     */
    bool setup(const char* configuration_name = "")                                             { return SRTBaseMinorServoImpl::setup(configuration_name);              }

    /**
     * Declaration of all the other inherited methods.
     */
    METHODS_DECLARATION;

    /**
     * Declaration of all the other inherited properties methods.
     */
    PROPERTIES_DECLARATION;
};

/**
 * This class implements the SRTProgramTrackMinorServoImpl CORBA interface for a tracking-capable SRTMinorServo component.
 * It inherits from the SRTBaseMinorServoImpl class.
 * A SRTProgramTrackMinorServo component is capable of commanding a trajectory to track to its related minor servo system.
 */
class SRTProgramTrackMinorServoImpl: public SRTBaseMinorServoImpl, public virtual POA_MinorServo::SRTProgramTrackMinorServo
{
public:
    /**
     * Constructor.
     * @param component_name component's name. This is also the name that will be used to find the configuration data for the component in the Configuration DataBase.
     * @param container_services pointer to the class that exposes all services offered by container.
     * @throw ComponentErrors::ComponentErrorsEx when there has been an issue reading some value from the CDB.
     */
    SRTProgramTrackMinorServoImpl(const ACE_CString &component_name, maci::ContainerServices *container_services);

    /**
     * Destructor.
     */
    ~SRTProgramTrackMinorServoImpl();

    /**
     * Overloaded initialize method. It calls the SRTBaseMinorServoImpl initialize method and performs some other initialization routines.
     * @throw ComponentErrors::ComponentErrorsEx when there has been a memory allocation issue with the properties pointers.
     */
    void initialize();

    /**
     * Overloaded status method. It calls the SRTBaseMinorServoImpl status method and performs some other routines.
     * @throw MinorServoErrors::MinorServoErrorsEx when trying to reset the offsets when they don't match the ones loaded into the hardware.
     */
    bool status();

    /**
     * Overloaded setup method. It calls the SRTBaseMinorServoImpl status method and performs some other routines.
     * @param configuration_name the configuration the servo system should assume.
     * @throw ComponentErrors::ComponentErrorsEx when there is an error while trying to load the table for the given configuration.
     */
    bool setup(const char* configuration_name = "");

    /**
     * Declaration of all the other inherited methods.
     */
    METHODS_DECLARATION;

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
     * Returns the tracking status of the minor servo.
     * @return true if the minor servo is tracking within the tracking error, false otherwise
     */
    bool isTracking() { return m_tracking.load() == Management::MNG_TRUE; }

    /**
     * Declaration of all the other inherited properties methods.
     */
    PROPERTIES_DECLARATION;

    /**
     * Returns a reference to the tracking property implementation of the IDL interface.
     * @return pointer to read-only boolean property tracking.
     */
    virtual Management::ROTBoolean_ptr tracking();

    /**
     * Returns a reference to the trajectory_id property implementation of the IDL interface.
     * @return pointer to read-only long property trajectory_id.
     */
    virtual ACS::ROlong_ptr trajectory_id();

    /**
     * Returns a reference to the total_trajectory_points property implementation of the IDL interface.
     * @return pointer to read-only long property total_trajectory_points.
     */
    virtual ACS::ROlong_ptr total_trajectory_points();

    /**
     * Returns a reference to the remaining_trajectory_points property implementation of the IDL interface.
     * @return pointer to read-only long property remaining_trajectory_points.
     */
    virtual ACS::ROlong_ptr remaining_trajectory_points();

private:
    /**
     * Queue of positions to be assumed by the servo system when tracking a trajectory.
     */
    SRTMinorServoPositionsQueue m_tracking_queue;

    /**
     * Tracking delta values for all minor servo system virtual axes.
     */
    const std::vector<double> m_tracking_delta;

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
};

#endif
