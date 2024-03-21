#ifndef _SRTMINORSERVOUTILS_H
#define _SRTMINORSERVOUTILS_H
#include <variant>
#include <type_traits>
#include <Cplusplus11Helper.h>
#include <IRA>
#include <ComponentErrors.h>
#include <iostream>

/**
 * The following templates are useful if you want to check if a given type for the SRTMinorServoAnswerMap is accepted
 */
template <typename T>
struct is_atomic { static constexpr bool value = false; };

template <typename T>
struct is_atomic<std::atomic<T>> { static constexpr bool value = true; };

template <typename T>
inline constexpr bool is_atomic_v = is_atomic<T>::value;

template <typename T, typename... Ts>
struct is_any : public std::disjunction<std::is_same<T, Ts>...> {};

template <typename T, typename... Ts>
inline constexpr bool is_any_v = (std::is_same_v<T, Ts> || ...);

template <typename T>
struct is_string : public std::disjunction<is_any<std::decay_t<T>, char*, const char*, std::string>> {};

template <typename T>
inline constexpr bool is_string_v = is_string<T>::value;

template <typename T>
struct is_known : public std::disjunction<std::is_arithmetic<std::decay_t<T>>, is_string<std::decay_t<T>>> {};

template <typename T>
inline constexpr bool is_known_v = is_known<T>::value;

template <typename T, typename Enable = std::enable_if_t<std::disjunction_v<is_known<T>, std::is_same<T, std::vector<double>>>>>
struct DB_type
{
    using type = std::conditional_t<std::disjunction_v<is_string<T>, std::is_same<T, std::vector<double>>>, IRA::CString, std::conditional_t<std::is_integral_v<T>, long, double>>;
};

template <typename T>
T getCDBValue(maci::ContainerServices* container_services, const std::string& field, const std::string component = "")
{
    using C = typename DB_type<T>::type;

    C temp;
    if(IRA::CIRATools::getDBValue(container_services, field.c_str(), temp, "alma/", component.c_str()))
    {
        if constexpr(std::is_same_v<T, std::vector<double>>)
        {
            std::vector<double> values;
            std::istringstream iss(std::string(temp).c_str());
            std::string token;
            while(std::getline(iss, token, ','))
            {
                double value;
                try
                {
                    value = std::stod(token);
                }
                catch(std::invalid_argument& ia)
                {
                    _EXCPT(ComponentErrors::CDBAccessExImpl, ex, "SRTMinorServoUtils::getCDBValue()");
                    ex.setFieldName(field.c_str());
                    throw ex.getComponentErrorsEx();
                }
                catch(std::out_of_range& oor)
                {
                    _EXCPT(ComponentErrors::ValueOutofRangeExImpl, ex, "SRTMinorServoUtils::getCDBValue()");
                    ex.setValueName(field.c_str());
                    ex.setValueLimit(token.find('-') == std::string::npos ? std::numeric_limits<double>::max() : std::numeric_limits<double>::min());
                    throw ex.getComponentErrorsEx();
                }

                values.push_back(value);
            }
            return values;
        }
        else if constexpr(std::is_same_v<C, IRA::CString>)
        {
            return (T)std::string(temp).c_str();
        }
        else
        {
            return T(temp);
        }
    }
    else
    {
        _EXCPT(ComponentErrors::CDBAccessExImpl, ex, "SRTMinorServoUtils::getCDBValue()");
        ex.setFieldName(field.c_str());
        throw ex.getComponentErrorsEx();  // Maybe throw the plain ex
    }
}

const char* getReasonFromEx(const auto& ex)
{
    std::string reason = "Unknown reason";

    for(auto [name, value] : ex.errorTrace.data)
    {
        if(strcmp(name, "Reason") == 0)
        {
            reason = value;
            break;
        }
    }

    return reason.c_str();
}

const char* getErrorFromEx(const auto& ex)
{
    std::string error(ex.errorTrace.routine);

    for(auto [name, value] : ex.errorTrace.data)
    {
        if(strcmp(name, "Reason") == 0)
        {
            error += ": " + std::string(value);
            break;
        }
    }

    return error.c_str();
}

C11_IGNORE_WARNING_PUSH
C11_IGNORE_WARNING("-Wunused-function")

static std::ostream& operator<<(std::ostream& out, const std::variant<long, double, std::string>& value)
{
    std::visit([&out](const auto& val) { out << val; }, value);
    return out;
}

C11_IGNORE_WARNING_POP

#endif
