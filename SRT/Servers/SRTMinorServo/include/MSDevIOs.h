#ifndef _MSDEVIOS_H
#define _MSDEVIOS_H

/**********************************************************/
/* Giuseppe Carboni <giuseppe.carboni@inaf.it> 20/09/2023 */
/**********************************************************/

#include "SuppressWarnings.h"
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
i * This template class is derived from DevIO and it is used by the almost all attributes of the SRT Minor Servos components.
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
                std::is_same<T, CORBA::Boolean>,
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
     *          @arg \c ComponentErrors::Timeout</a>
     *          @arg \c AntennaErrors::Connection</a>
     *          @arg \c ComponentErrors::SocketError
     * @param timestamp epoch when the operation completes
    */
    T read(ACS::Time& timestamp)
    {
        if(m_secure_area)
        {
            IRA::CSecAreaResourceWrapper<SRTMinorServoAnswerMap> data = m_secure_area->Get();

            try
            {
                if constexpr(std::is_same<T, CORBA::Boolean>::value)
                {
                    unsigned int value = std::get<long>(data->at(m_prefix + m_property_fields[0]));
                    m_value = (value == 1) ? true : false;
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
                    if(value > 20)
                    {
                        //BWG
                        value -= 11;
                    }
                    else if(value > 10)
                    {
                        //Gregorian
                        value -= 9;
                    }

                    m_value = MinorServo::SRTMinorServoFocalConfiguration(value+1);
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
                    std::cout << "Unknown type" << std::endl;
                }
            }
            catch(ACSErr::ACSbaseExImpl& ex)
            {
                _ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl, dummy, ex, "MSDevIO::read()");
                dummy.setPropertyName(m_property_name.c_str());
                dummy.setReason("Property could not be read");
                throw dummy;
            }
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

#endif
