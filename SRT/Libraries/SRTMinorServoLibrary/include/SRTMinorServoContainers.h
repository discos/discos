#ifndef __SRTMINORSERVOCONTAINERS_H__
#define __SRTMINORSERVOCONTAINERS_H__

/**
 * SRTMinorServoContainers.h
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include <map>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <IRA>
#include <boost/bimap.hpp>
#include <boost/assign.hpp>
#include <SRTMinorServoCommonS.h>
#include "SRTMinorServoUtils.h"
#include "SRTMinorServoPositionsQueue.h"
#include "SRTMinorServoOffsetsQueue.h"


namespace MinorServo
{
    /**
     * Object used to store some info regarding a scan.
     */
    struct SRTMinorServoScan
    {
        /**
         * Name of the servo involved in the scan.
         */
        std::string servo_name = "";

        /**
         * Name of the axis involved in the scan.
         */
        std::string axis_name = "";

        /**
         * Index of the axis involved in the scan.
         */
        size_t axis_index = 0;

        /**
         * Range of the scan.
         */
        double scan_range = 0;

        /**
         * Starting time of the scan.
         */
        ACS::Time start_time = 0;

        /**
         * Closing time of the scan.
         */
        ACS::Time close_time = 0;

        /**
         * Duration of the scan.
         */
        ACS::TimeInterval scan_duration = 0;

        /**
         * Central position of the scan axis.
         */
        double central_position = 0;

        /**
         * Starting elevation for the scan.
         */
        double starting_elevation = 0;
    };

    /**
     * This dictionary contains the Leonardo focal configurations DISCOS enumerations, alongside their name inside the Leonardo minor servo system.
     */
    using LDOConfigurationNameTableType = boost::bimap<SRTMinorServoFocalConfiguration, std::string>;
    const LDOConfigurationNameTableType LDOConfigurationNameTable = boost::assign::list_of<LDOConfigurationNameTableType::relation>
        (CONFIGURATION_PRIMARY,     "Primario")
        (CONFIGURATION_GREGORIAN1,  "Gregoriano1")
        (CONFIGURATION_GREGORIAN2,  "Gregoriano2")
        (CONFIGURATION_GREGORIAN3,  "Gregoriano3")
        (CONFIGURATION_GREGORIAN4,  "Gregoriano4")
        (CONFIGURATION_GREGORIAN5,  "Gregoriano5")
        (CONFIGURATION_GREGORIAN6,  "Gregoriano6")
        (CONFIGURATION_GREGORIAN7,  "Gregoriano7")
        (CONFIGURATION_GREGORIAN8,  "Gregoriano8")
        (CONFIGURATION_BWG1,        "BWG1")
        (CONFIGURATION_BWG2,        "BWG2")
        (CONFIGURATION_BWG3,        "BWG3")
        (CONFIGURATION_BWG4,        "BWG4");

    /**
     * This dictionary containes the Leonardo focal configurations DISCOS enumerations, alongside their ID counterpart as read from the Leonardo minor servo system proxy.
     */
    using LDOConfigurationIDTableType = boost::bimap<SRTMinorServoFocalConfiguration, unsigned int>;
    const LDOConfigurationIDTableType LDOConfigurationIDTable = boost::assign::list_of<LDOConfigurationIDTableType::relation>
        (CONFIGURATION_UNKNOWN,     0)
        (CONFIGURATION_PRIMARY,     1)
        (CONFIGURATION_GREGORIAN1,  11)
        (CONFIGURATION_GREGORIAN2,  12)
        (CONFIGURATION_GREGORIAN3,  13)
        (CONFIGURATION_GREGORIAN4,  14)
        (CONFIGURATION_GREGORIAN5,  15)
        (CONFIGURATION_GREGORIAN6,  16)
        (CONFIGURATION_GREGORIAN7,  17)
        (CONFIGURATION_GREGORIAN8,  18)
        (CONFIGURATION_BWG1,        21)
        (CONFIGURATION_BWG2,        22)
        (CONFIGURATION_BWG3,        23)
        (CONFIGURATION_BWG4,        24);

    using SRTMinorServoCoefficientsTable = std::map<std::string, std::vector<double>>;


    class SRTMinorServoAnswerMap : private std::map<std::string, std::variant<long, double, std::string>>
    {
        /**
         * This class privately extends the type std::map<std::string, std::variant<long, double, std::string>>.
         * It is therefore an std::map which can hold different types of values, such as long, double and str::string, in the same container.
         * It is used to store the answers received from the SRTMinorServo Leonardo system.
         * This design was critical since all the received values have heterogeneous keys and values.
         * With this object, the SRTMinorServoSocket can correctly retrieve and store all the received values without having to know the keys or types a priori.
         */

        /*
         * Declare this class as friend since it will have to iterate through the inner map
         */
        friend class PySRTMinorServoCommandLibrary;
    public:
        /**
         * Use the same clear method of the parent class
         */
        using std::map<std::string, std::variant<long, double, std::string>>::clear;

        /**
         * Default constructor, initialize the std::map object and the synchronization mutex
         */
        SRTMinorServoAnswerMap() : std::map<std::string, std::variant<long, double, std::string>>(), m_mutex() {}

        /**
         * Initialize the std::map with the content of another SRTMinorServoAnswerMap, initialize the mutex, lock both objects
         * @param other the SRTMinorServoAnswerMap with which the content of the current object will be initialized
         */
        SRTMinorServoAnswerMap(const SRTMinorServoAnswerMap& other) : m_mutex()
        {
            std::unique_lock<std::shared_mutex> lockThis(m_mutex, std::defer_lock);
            std::shared_lock<std::shared_mutex> lockOther(other.m_mutex, std::defer_lock);
            std::lock(lockThis, lockOther);
            static_cast<std::map <std::string, std::variant<long, double, std::string>>&>(*this) = static_cast<const std::map<std::string, std::variant<long, double, std::string>>&>(other);
        }

        /**
         * Assignment operator. It lock both the current object and the assigned one's mutexes
         * @param other the SRTMinorServoAnswerMap which values have to be stored in the current object
         */
        SRTMinorServoAnswerMap& operator=(const SRTMinorServoAnswerMap& other)
        {
            if(this != &other)
            {
                std::unique_lock<std::shared_mutex> lockThis(m_mutex, std::defer_lock);
                std::shared_lock<std::shared_mutex> lockOther(other.m_mutex, std::defer_lock);
                std::lock(lockThis, lockOther);
                static_cast<std::map <std::string, std::variant<long, double, std::string>>&>(*this) = static_cast<const std::map<std::string, std::variant<long, double, std::string>>&>(other);
            }

            return *this;
        }

        /**
         * Update operator. It merges the current object with the elements of another object
         * @param other the SRTMinorServoAnswerMap which values have to be copied inside the current object
         */
        SRTMinorServoAnswerMap& operator+=(const SRTMinorServoAnswerMap& other)
        {
            std::unique_lock<std::shared_mutex> lockThis(m_mutex, std::defer_lock);
            std::shared_lock<std::shared_mutex> lockOther(other.m_mutex, std::defer_lock);
            std::lock(lockThis, lockOther);
            for(const auto& entry : other)
            {
                this->operator[](entry.first) = entry.second;
            }

            return *this;
        }

        /**
         * Equality operator, only check the std::map and avoid comparing the mutexes, which will obviously be different
         * @param other the SRTMinorServoAnswerMap object to compare the current object with
         */
        bool operator==(const SRTMinorServoAnswerMap& other) const
        {
            return static_cast<const std::map<std::string, std::variant<long, double, std::string>>&>(*this) == static_cast<const std::map<std::string, std::variant<long, double, std::string>>&>(other);
        }

        /**
         * get method. It must be used with a template parameter, in order for the SRTMinorServoAnswerMap to be able to retrieve the correct type of object for the given key.
         * The method will automatically convert the retrieved long, double or std::string to the given template type.
         * @param T the type (i.e.: int, long, double, char*) of the object to be retrieved. It can be anything derived from integral, floating point or string values.
         * @param key the key assigned to the value you want to retrieve
         * @return the value associated to given key 'key', returned as template type 'T', if possible. Be aware that some casts (i.e.: long to int, double to float) will lose precision and/or overflow
         * @throw std::bad_variant_access when retrieving the stored value by asking the wrong type (i.e.: stored type is a double but T is char*)
         * @throw std::runtime_error when attempting to retrieve a value with a type which cannot be stored in the container (anything not integral, not floating point and not similar to std::string)
         */
        template <typename T>
        T get(const std::string& key) const
        {
            if constexpr(std::negation_v<is_known<T>>)
            {
                throw std::runtime_error("Unsupported type.");
            }

            std::shared_lock<std::shared_mutex> lock(m_mutex);

            try
            {
                if constexpr(std::is_integral_v<T>)
                {
                    return (T)std::get<long>(this->at(key));
                }
                else if constexpr(std::is_floating_point_v<T>)
                {
                    return (T)std::get<double>(this->at(key));
                }
                else if constexpr(is_string_v<T>)
                {
                    return (T)std::get<std::string>(this->at(key)).c_str();
                }
            }
            catch(std::out_of_range& ex)
            {
                std::cout << "PLAIN_COMMAND: " << this->getPlainCommand();
                std::cout << "PLAIN_ANSWER:" << this->getPlainAnswer();
                throw ex;
            }
        }

        /**
         * put method. The template parameter is automatically deducted from the 'value' argument. Stores the given 'value' associated with key 'key'
         * @param key the key associated to the stored value 'value'
         * @param value the value we are storing with the given key 'key'
         * @throw std::runtime_error when attempting to store a value with a type which cannot be stored in the container (anything not integral, not floating point and not similar to std::string)
         */
        template <typename T>
        void put(const std::string& key, const T& value)
        {
            if constexpr(std::negation_v<is_known<T>>)
            {
                throw std::runtime_error("Unsupported type.");
            }

            std::unique_lock<std::shared_mutex> lock(m_mutex);

            if constexpr(std::is_integral_v<T>)
            {
                this->operator[](key) = long(value);
            }
            else if constexpr(std::is_floating_point_v<T>)
            {
                this->operator[](key) = double(value);
            }
            else if constexpr(is_string_v<T>)
            {
                this->operator[](key) = std::string(value);
            }
        }

        /**
         * This method checks whether the container holds a value for the given key 'key'
         * @param key the key for the value we want to check if it's present in the container
         * @return true if the value is present in the container, false otherwise
         */
        bool contains(const std::string& key) const
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            return this->find(key) != this->end();
        }

        /**
         * This methods returns the std::variant type index for the value associated to the given key 'key'
         * @param key the key for the value we want to retrieve the type index
         * @throw std::out_of_range if the key is not found in the object
         * @return 0 for long, 1 for double, 2 for std::string
         */
        unsigned int index(const std::string& key) const
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            return this->at(key).index();
        }

        /**
         * This method checks whether the contained answer to a command sent to the SRTMinorServo system was positive or not
         * @return true if the command was correctly accepted, false if the command was not accepted or the 'OUTPUT' key was not found (unlikely scenario)
         */
        const bool checkOutput() const
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            try
            {
                if(this->get<std::string>("OUTPUT") == "GOOD")
                {
                    return true;
                }
            }
            catch(std::out_of_range& ex)
            {
                // Key not found
            }

            return false;
        }

        /**
         * This method retrieves the ACS::Time associated with the received answer map. It converts the value from UNIX Epoch (double) to ACS::Time
         * @return the ACS::Time associated with the answer map
         */
        const ACS::Time getTimestamp() const
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            return IRA::CIRATools::UNIXEpoch2ACSTime(this->get<double>("TIMESTAMP"));
        }

        /**
         * This method returns the plain command sent using the socket. Useful for log purposes.
         * @return a std::string containing the plain command sent using the socket.
         */
        const std::string getPlainCommand() const
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            return this->count("PLAIN_COMMAND") > 0 ? this->get<std::string>("PLAIN_COMMAND") : "";
        }

        /**
         * This method returns the plain answer received from the socket. Useful for log purposes.
         * @return a std::string containing the plain answer received from the socket.
         */
        const std::string getPlainAnswer() const
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            return this->count("PLAIN_ANSWER") > 0 ? this->get<std::string>("PLAIN_ANSWER") : "";
        }

    protected:
        /**
         * Shared mutex to control read and write accesses. Multiple reading access are permitted and will only block writing access. Writing access will block all accesses
         */
        mutable std::shared_mutex m_mutex;
    };

    /**
     * This class is a specialization of the SRTMinorServoAnswerMap for the general Leonardo Minor Servo System status.
     */
    class SRTMinorServoGeneralStatus : public SRTMinorServoAnswerMap
    {
    public:
        /**
         * Retrieves the current configuration from the map.
         * @return the current SRTMinorServoFocalConfiguration.
         */
        SRTMinorServoFocalConfiguration getFocalConfiguration() const
        {
            return LDOConfigurationIDTable.right.at(this->get<unsigned int>("CURRENT_CONFIG"));
        }

        /**
         * Retrieves a boolean indicating whether the simulation is enabled or not.
         * @returns a boolean indicating whether the simulation is enabled or not.
         */
        Management::TBoolean isSimulationEnabled() const
        {
            return this->get<unsigned int>("SIMULATION_ENABLED") == 1 ? Management::MNG_TRUE : Management::MNG_FALSE;
        }

        /**
         * Returns the PLC time of the reading.
         * @return a double indicating the PLC time, expressed as UNIX Epoch.
         */
        double getPLCTime() const
        {
            return this->get<double>("PLC_TIME");
        }

        /**
         * Returns the firmware version present on the PLC.
         * @return a string containing the firmware version.
         */
        ACE_CString getPLCVersion() const
        {
            return this->get<std::string>("PLC_VERSION").c_str();
        }

        /**
         * Returns who is controlling the Leonardo Minor Servo System.
         * @return an enum indicating who is controlling the system.
         */
        SRTMinorServoControlStatus getControl() const
        {
            return SRTMinorServoControlStatus(this->get<unsigned int>("CONTROL") - 1);
        }

        /**
         * Returns a boolean indicating whether the system is powered on or not.
         * @return a boolean indicating whether the system is powered on or not.
         */
        Management::TBoolean hasPower() const
        {
            return this->get<unsigned int>("POWER") == 1 ? Management::MNG_TRUE : Management::MNG_FALSE;
        }

        /**
         * Is the emergency stop pressed somewhere? Is there an emergency situation?
         * @return a boolean indicating if an emergency is present or not.
         */
        Management::TBoolean emergencyPressed() const
        {
            return this->get<unsigned int>("EMERGENCY") == 1 ? Management::MNG_TRUE : Management::MNG_FALSE;
        }

        /**
         * Returns the position of the gregorian cover.
         * @return an enum indicating the position of the gregorian cover.
         */
        SRTMinorServoGregorianCoverStatus getGregorianCoverPosition() const
        {
            return SRTMinorServoGregorianCoverStatus(std::min(this->get<unsigned int>("GREGORIAN_CAP"), (unsigned int)COVER_STATUS_OPEN));
        }

        /**
         * Returns the status of the gregorian air blade.
         * @return an enum indicating the status of the gregorian air blade.
         */
        SRTMinorServoGregorianAirBladeStatus getGregorianAirBladeStatus() const
        {
            unsigned int status = this->get<unsigned int>("GREGORIAN_CAP");
            if(status <= COVER_STATUS_CLOSED)
            {
                status = AIR_BLADE_STATUS_OFF;
            }
            else
            {
                status -= 2;
            }
            return SRTMinorServoGregorianAirBladeStatus(status);
        }

        /**
         * Returns the UNIX Epoch of the last executed command.
         * @return a double containing the UNIX Epoch of the last executed command.
         */
        double getLastExecutedCommand() const
        {
            return this->get<double>("LAST_EXECUTED_COMMAND");
        }
    };

    /**
     * This class is a specialization of the SRTMinorServoAnswerMap for a single servo status of the Leonardo Minor Servo System.
     */
    class SRTMinorServoStatus : public SRTMinorServoAnswerMap
    {
    public:
        /**
         * Constructor. Accepts some lists of labels in order to correctly retrieve the values from the map.
         * @param servo_name the servo name used as a prefix when retrieving the values.
         * @param physical_axes_enabled the labels used to retrieve the status of each physical axis.
         * @param physical_positions the labels used to retrieve the position of each physical axis.
         * @param virtual_positions the labels used to retrieve the position of each virtual axis.
         * @param virtual_offsets the labels used to retrieve the offset of each virtual axis.
         */
        SRTMinorServoStatus(
                const std::string& servo_name,
                const std::vector<std::string>& physical_axes_enabled,
                const std::vector<std::string>& physical_positions,
                const std::vector<std::string>& virtual_positions,
                const std::vector<std::string>& virtual_offsets
        ) :
            SRTMinorServoAnswerMap(),
            m_servo_name(servo_name),
            m_physical_axes_enabled(physical_axes_enabled),
            m_physical_positions(physical_positions),
            m_virtual_positions(virtual_positions),
            m_virtual_offsets(virtual_offsets)
        {}

        /**
         * Returns a boolean indicating whether the servo is enabled.
         * @returns true if enabled, false otherwise.
         */
        Management::TBoolean isEnabled() const
        {
            return this->get<unsigned int>(m_servo_name + "_ENABLED") == 1 ? Management::MNG_TRUE : Management::MNG_FALSE;
        }

        /**
         * Returns the status of the servo drive cabinet.
         * @returns an enum indicating the status of the servo drive cabinet.
         */
        SRTMinorServoCabinetStatus getDriveCabinetStatus() const
        {
            return SRTMinorServoCabinetStatus(this->get<unsigned int>(m_servo_name + "_STATUS") - 1);
        }

        /**
         * Returns a boolean indicating whether the servo is blocked or not.
         * @return true if the servo is blocked, false otherwise.
         */
        Management::TBoolean isBlocked() const
        {
            return this->get<unsigned int>(m_servo_name + "_BLOCK") == 1 ? Management::MNG_TRUE : Management::MNG_FALSE;
        }

        /**
         * Returns the operative mode of the servo.
         * @return an enum indicating the operative mode of the servo.
         */
        SRTMinorServoOperativeMode getOperativeMode() const
        {
            return SRTMinorServoOperativeMode(this->get<unsigned int>(m_servo_name + "_OPERATIVE_MODE") / 10);
        }

        /**
         * Returns the status of each physical axis.
         * @return a boolean sequence, true if the axis is enabled, false otherwise.
         */
        ACS::booleanSeq getPhysicalAxesEnabled() const
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            return getSequence<ACS::booleanSeq>(m_physical_axes_enabled);
        }

        /**
         * Returns the position of each physical axis.
         * @return a double sequence containing the position of each physical axis.
         */
        ACS::doubleSeq getPhysicalPositions() const
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            return getSequence<ACS::doubleSeq>(m_physical_positions);
        }

        /**
         * Returns the plain position of each virtual axis.
         * @return a double sequence containing the plain position of each virtual axis.
         */
        ACS::doubleSeq getPlainVirtualPositions() const
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            return getSequence<ACS::doubleSeq>(m_virtual_positions);
        }

        /**
         * Returns the actual position of each virtual axis, minus the offset.
         * @return a double sequence containing the position of each virtual axis, minus the offset.
         */
        ACS::doubleSeq getVirtualPositions() const
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            ACS::doubleSeq virtual_positions = getPlainVirtualPositions();
            ACS::doubleSeq virtual_offsets = getVirtualOffsets();
            for(size_t i = 0; i < virtual_positions.length(); i++)
            {
                virtual_positions[i] -= virtual_offsets[i];
            }

            return virtual_positions;
        }

        /**
         * Returns the offset of each virtual axis.
         * @return a double sequence containing the offset of each virtual axis.
         */
        ACS::doubleSeq getVirtualOffsets() const
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            return getSequence<ACS::doubleSeq>(m_virtual_offsets);
        }

    private:
        /**
         * This method extracts a sequence, either boolean or double, from the map, and returns it.
         * @param labels a vector of strings containing the labels to use in order to extract the corresponding values from the map.
         * @return the composed sequence of booleans or doubles.
         */
        template <typename T, typename = std::enable_if<is_any_v<T, ACS::booleanSeq, ACS::doubleSeq>>>
        T getSequence(const std::vector<std::string>& labels) const
        {
            T sequence;
            sequence.length(labels.size());

            for(size_t i = 0; i < labels.size(); i++)
            {
                if constexpr(std::is_same_v<T, ACS::booleanSeq>)
                {
                    sequence[i] = (bool)this->get<unsigned int>(m_servo_name + "_" + labels[i]);
                }
                else if constexpr(std::is_same_v<T, ACS::doubleSeq>)
                {
                    sequence[i] = this->get<double>(m_servo_name + "_" + labels[i]);
                }
            }

            return sequence;
        }

        /**
         * The name of the servo, this is used as prefix when retrieving values from the map.
         */
        const std::string m_servo_name;

        /**
         * The labels for the enabled value of each physical axis.
         */
        const std::vector<std::string> m_physical_axes_enabled;

        /**
         * The labels for the positions of each physical axis.
         */
        const std::vector<std::string> m_physical_positions;

        /**
         * The labels for the positions of each virtual axis.
         */
        const std::vector<std::string> m_virtual_positions;

        /**
         * The labels for the offsets of each virtual axis.
         */
        const std::vector<std::string> m_virtual_offsets;
    };

    /**
     * This class is a specialization of the SRTMinorServoStatus for a derotator of the Leonardo Minor Servo System.
     */
    class SRTDerotatorStatus : public SRTMinorServoStatus
    {
    public:
        using SRTMinorServoStatus::SRTMinorServoStatus;

        /**
         * This method returns the current derotator position. There is a sign inversion since the LDO derotator sign is opposite to the required one.
         * @return the current derotator position
         */
        double getActualPosition() const
        {
            double position = getVirtualPositions()[0];
            return position == 0 ? position : -position;
        }
    };
}

#endif
