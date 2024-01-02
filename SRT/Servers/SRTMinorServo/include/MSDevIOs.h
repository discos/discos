#ifndef _MSDEVIOS_H
#define _MSDEVIOS_H

/**********************************************************/
/* Giuseppe Carboni <giuseppe.carboni@inaf.it> 20/09/2023 */
/**********************************************************/

#include "Common.h"
#include <type_traits>
#include <baciDevIO.h>
#include <IRA>
#include <AntennaErrors.h>
#include <ComponentErrors.h>
#include <SRTMinorServoCommonS.h>
#include "SRTMinorServoCommandLibrary.h"


struct SRTMinorServoDevIOInfo
{
    IRA::CSecureArea<SRTMinorServoAnswerMap>* secure_area;
    std::string prefix = "";
    std::string property_name;
    std::vector<std::string> property_fields;
};


/**
 * This template class is derived from DevIO and it is used by the almost all attributes of the SRT Minor Servos components.
 * The associeted property can be selected at construction time and cannot be changed anymore.
 * @author Giuseppe Carboni <giuseppe.carboni@inaf.it>,
 */
template <class T> class MSDevIO : public DevIO<T>
{
public:
    /**
     * Constructor
     * @param data pointer to a SecureArea that protects a SRTMinorServoAnswerMap object.
     * @param property_info an std::pair containing the name of the property as shown in the Object Explorer and the fields to read from the SRTMinorServoAnswerMap
    */
    MSDevIO(SRTMinorServoDevIOInfo property_info)
    {
        // The following expression will produce an error at compile time if the T class type is not one of the following
        static_assert(
            std::disjunction<
                std::is_same<T, Management::TBoolean>,
                std::is_same<T, CORBA::Double>,
                std::is_same<T, ACE_CString>,
                std::is_same<T, ACS::booleanSeq>,
                std::is_same<T, ACS::doubleSeq>,
                std::is_same<T, MinorServo::SRTMinorServoFocalConfiguration>,
                std::is_same<T, MinorServo::SRTMinorServoControlStatus>,
                std::is_same<T, MinorServo::SRTMinorServoGregorianCoverStatus>,
                std::is_same<T, MinorServo::SRTMinorServoCabinetStatus>,
                std::is_same<T, MinorServo::SRTMinorServoOperativeMode>
            >::value,
            "Not accepted MSDevIO type!"
        );

        m_secure_area = property_info.secure_area;
        m_prefix = property_info.prefix;
        m_property_name = property_info.property_name;
        m_property_fields = property_info.property_fields;
    }

    /**
     * Destructor
    */
    ~MSDevIO()
    {
    }

    /**
     * @return true to initialize the property with default value from CDB.
    */
    bool initializeValue()
    {
        return false;
    }

    /**
     * Used to read the property value.
     * @throw ComponentErrors::PropertyError
     * @param timestamp epoch when the operation completes
    */
    T read(ACS::Time& timestamp)
    {
        try
        {
            if(m_secure_area)
            {
                IRA::CSecAreaResourceWrapper<SRTMinorServoAnswerMap> data = m_secure_area->Get();

                try
                {
                    if constexpr(std::is_same<T, Management::TBoolean>::value)
                    {
                        unsigned int value = std::get<long>(data->at(m_prefix + m_property_fields[0]));
                        m_value = (value == 1) ? Management::MNG_TRUE : Management::MNG_FALSE;
                    }
                    else if constexpr(std::is_same<T, CORBA::Double>::value)
                    {
                        auto object = data->at(m_prefix + m_property_fields[0]);
                        if(object.index() == 0)
                        {
                            m_value = double(std::get<long>(object));
                        }
                        else if(object.index() == 1)
                        {
                            m_value = std::get<double>(object);
                        }
                    }
                    else if constexpr(std::is_same<T, ACE_CString>::value)
                    {
                        auto object = data->at(m_prefix + m_property_fields[0]);
                        if(object.index() == 0)
                        {
                            m_value = std::to_string(std::get<long>(object)).c_str();
                        }
                        else if(object.index() == 1)
                        {
                            m_value = std::to_string(std::get<double>(object)).c_str();
                        }
                        else
                        {
                            m_value = std::get<std::string>(object).c_str();
                        }
                    }
                    else if constexpr(std::is_same<T, ACS::booleanSeq>::value)
                    {
                        m_value.length(m_property_fields.size());

                        for(size_t i = 0; i < m_property_fields.size(); i++)
                        {
                            m_value[i] = std::get<long>(data->at(m_prefix + m_property_fields[i]));
                        }
                    }
                    else if constexpr(std::is_same<T, ACS::doubleSeq>::value)
                    {
                        m_value.length(m_property_fields.size());

                        for(size_t i = 0; i < m_property_fields.size(); i++)
                        {
                            m_value[i] = std::get<double>(data->at(m_prefix + m_property_fields[i]));
                        }
                    }
                    else if constexpr(std::is_same<T, MinorServo::SRTMinorServoFocalConfiguration>::value)
                    {
                        int value = std::get<long>(data->at(m_prefix + m_property_fields[0]));
                        m_value = MinorServo::LDOConfigurationIDTable.right.at(value);
                    }
                    else if constexpr(std::is_same<T, MinorServo::SRTMinorServoControlStatus>::value)
                    {
                        m_value = MinorServo::SRTMinorServoControlStatus(std::get<long>(data->at(m_prefix + m_property_fields[0])) - 1);
                    }
                    else if constexpr(std::is_same<T, MinorServo::SRTMinorServoGregorianCoverStatus>::value)
                    {
                        m_value = MinorServo::SRTMinorServoGregorianCoverStatus(std::get<long>(data->at(m_prefix + m_property_fields[0])));
                    }
                    else if constexpr(std::is_same<T, MinorServo::SRTMinorServoCabinetStatus>::value)
                    {
                        m_value = MinorServo::SRTMinorServoCabinetStatus(std::get<long>(data->at(m_prefix + m_property_fields[0])) - 1);
                    }
                    else if constexpr(std::is_same<T, MinorServo::SRTMinorServoOperativeMode>::value)
                    {
                        m_value = MinorServo::SRTMinorServoOperativeMode(std::get<long>(data->at(m_prefix + m_property_fields[0])) / 10);
                    }
                    else
                    {
                        // This should never happen thanks to the static_assert in the constructor
                        _EXCPT(ComponentErrors::PropertyErrorExImpl, impl, "MSDevIO::read()");
                        impl.setPropertyName(m_property_name.c_str());
                        impl.setReason("Unknown property type!");
                        throw impl;
                    }
                }
                catch(std::out_of_range& ex)
                {
                    _EXCPT(ComponentErrors::PropertyErrorExImpl, impl, "MSDevIO::read()");
                    impl.setPropertyName(m_property_name.c_str());
                    impl.setReason("Property is missing from the map!");
                }
                catch(std::bad_variant_access& ex)
                {
                    _EXCPT(ComponentErrors::PropertyErrorExImpl, impl, "MSDevIO::read()");
                    impl.setPropertyName(m_property_name.c_str());
                    impl.setReason("Attempt to access the property with the wrong type!");
                }
            }
            else
            {
                _EXCPT(ComponentErrors::PropertyErrorExImpl, impl, "MSDevIO::read()");
                impl.setPropertyName(m_property_name.c_str());
                impl.setReason("SecureArea not ready!");
                throw impl;
            }
        }
        catch(ACSErr::ACSbaseExImpl& ex)
        {
            _ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl, impl, ex, "MSDevIO::read()");
            impl.setPropertyName(m_property_name.c_str());
            impl.setReason("Property could not be read!");
            throw impl;
        }

        timestamp = getTimeStamp(); //completion time
        return m_value;
    }

    /**
     * It writes values into controller. Unused because the properties are read-only.
     */
    void write(const T& value, ACS::Time& timestamp)
    {
        timestamp = getTimeStamp();
        return;
    }

private:
    IRA::CSecureArea<SRTMinorServoAnswerMap>* m_secure_area;
    T m_value;
    std::string m_prefix;
    std::string m_property_name;
    std::vector<std::string> m_property_fields;
};


template <class C, typename A> class MSGenericDevIO : public DevIO<C>
{
public:
    MSGenericDevIO(A* value)
    {
        // The following expression will produce an error at compile time if the T class type is not one of the following
        static_assert(
            std::disjunction<
                std::is_same<C, Management::TSystemStatus>,
                std::is_same<C, Management::TBoolean>,
                std::is_same<C, ACE_CString>,
                std::is_same<C, CORBA::Long>,
                std::is_same<C, ACS::doubleSeq>
            >::value,
            "Not accepted MSDevIO type!"
        );

        // The following expressions will produce an error at compile time if the provided A argument class type is not recognized
        static_assert(
            std::disjunction<
                std::negation<std::is_same<C, ACE_CString> >,
                std::conjunction<
                    std::is_same<C, ACE_CString>,
                    std::disjunction<
                        std::is_same<A, std::string>,
                        std::is_same<A, std::atomic<MinorServo::SRTMinorServoMotionStatus> >
                    >
                >
            >::value,
            "Not accepted argument type!"
        );

        static_assert(
            std::disjunction<
                std::negation<std::is_same<C, ACS::doubleSeq> >,
                std::conjunction<
                    std::is_same<C, ACS::doubleSeq>,
                    std::is_same<A, std::vector<double> >
                >
            >::value,
            "Not accepted argument type!"
        );

        m_value = value;
    }

    /**
     * Destructor
    */
    ~MSGenericDevIO()
    {
    }

    /**
     * @return true to initialize the property with default value from CDB.
    */
    bool initializeValue()
    {
        return false;
    }

    /**
     * Used to read the property value.
     * @param timestamp epoch when the operation completes
    */
    C read(ACS::Time& timestamp)
    {
        timestamp = getTimeStamp(); //completion time

        if constexpr(std::is_same<A, std::atomic<MinorServo::SRTMinorServoMotionStatus> >::value)  // Motion status
        {
            std::string motion_status;

            switch(*m_value)
            {
                case MinorServo::MOTION_STATUS_UNCONFIGURED:
                {
                    motion_status = "Not configured";
                    break;
                }
                case MinorServo::MOTION_STATUS_CONFIGURING:
                {
                    motion_status = "Configuring...";
                    break;
                }
                case MinorServo::MOTION_STATUS_PARK:
                {
                    motion_status = "Parked";
                    break;
                }
                case MinorServo::MOTION_STATUS_ERROR:
                {
                    motion_status = "Error";
                    break;
                }
                case MinorServo::MOTION_STATUS_CONFIGURED:
                {
                    motion_status = "Elevation Track Mode Disabled";
                    break;
                }
                case MinorServo::MOTION_STATUS_TRACKING:
                {
                    motion_status = "Elevation Track Mode";
                    break;
                }
            }

            return motion_status.c_str();
        }
        else if constexpr(std::is_same<A, std::string>::value)
        {
            return m_value->c_str();
        }
        else if constexpr(std::is_same<C, ACS::doubleSeq>::value)
        {
            ACS::doubleSeq_var sequence = new ACS::doubleSeq;
            sequence->length(m_value->size());

            for(size_t i = 0; i < m_value->size(); i++)
            {
                sequence[i] = m_value->operator[](i);
            }

            return sequence;
        }
        else
        {
            return *m_value;
        }
    }

    /**
     * It writes values into controller. Unused because the properties are read-only.
     */
    void write(const C& value, ACS::Time& timestamp)
    {
        timestamp = getTimeStamp();
        return;
    }

private:
    A* m_value;
};

#endif
