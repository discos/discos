#ifndef _SRTMINORSERVOANSWERMAP_H
#define _SRTMINORSERVOANSWERMAP_H

#include <sstream>
#include <map>
#include <variant>
#include <type_traits>
#include <mutex>
#include <shared_mutex>
#include <IRA>

#include <Cplusplus11Helper.h>
C11_IGNORE_WARNING_PUSH
C11_IGNORE_WARNING("-Wunused-function")

static std::ostream& operator<<(std::ostream& out, const std::variant<long, double, std::string>& value)
{
    std::visit([&out](const auto& val) { out << val; }, value);
    return out;
}

C11_IGNORE_WARNING_POP


namespace MinorServo
{
    /**
     * The following templates are useful if you want to check if a given type for the SRTMinorServoAnswerMap is accepted
     */
    template <typename T>
    struct is_string : public std::disjunction<std::is_same<char*, std::decay_t<T>>, std::is_same<const char*, std::decay_t<T>>, std::is_same<std::string, std::decay_t<T>>> {};

    template <typename T>
    inline constexpr bool is_string_v = is_string<T>::value;

    template <typename T>
    struct is_known : public std::disjunction<std::is_arithmetic<std::decay_t<T>>, is_string<std::decay_t<T>>> {};

    template <typename T>
    inline constexpr bool is_known_v = is_known<T>::value;

    class SRTMinorServoAnswerMap : private  std::map<std::string, std::variant<long, double, std::string>>
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
            std::unique_lock<std::shared_mutex> lockOther(other.m_mutex, std::defer_lock);
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
                std::unique_lock<std::shared_mutex> lockOther(other.m_mutex, std::defer_lock);
                std::lock(lockThis, lockOther);
                static_cast<std::map <std::string, std::variant<long, double, std::string>>&>(*this) = static_cast<const std::map<std::string, std::variant<long, double, std::string>>&>(other);
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
            if constexpr(std::negation_v<MinorServo::is_known<T>>)
            {
                throw std::runtime_error("Unsupported type.");
            }

            std::shared_lock<std::shared_mutex> lock(m_mutex);

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

        /**
         * put method. The template parameter is automatically deducted from the 'value' argument. Stores the given 'value' associated with key 'key'
         * @param key the key associated to the stored value 'value'
         * @param value the value we are storing with the given key 'key'
         * @throw std::runtime_error when attempting to store a value with a type which cannot be stored in the container (anything not integral, not floating point and not similar to std::string)
         */
        template <typename T>
        void put(const std::string& key, const T& value)
        {
            if constexpr(std::negation_v<MinorServo::is_known<T>>)
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

    private:
        /**
         * Shared mutex to control read and write accesses. Multiple reading access are permitted and will only block writing access. Writing access will block all accesses
         */
        mutable std::shared_mutex m_mutex;
    };
}


#endif
