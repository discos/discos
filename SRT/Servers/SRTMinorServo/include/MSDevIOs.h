#ifndef _MSDEVIOS_H
#define _MSDEVIOS_H

/**
 * MSDevIO.h
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "SRTMinorServoCommon.h"
#include "SRTMinorServoContainers.h"
#include <type_traits>
#include <baciDevIO.h>
#include <ComponentErrors.h>
#include <SRTMinorServoCommonS.h>
#include "SRTMinorServoAnswerMap.h"


namespace MinorServo
{
    /**
     * This class acts as a base class for all the SRTMinorServo DevIOs.
     */
    template <class T> class MSBaseDevIO : public DevIO<T>
    {
    public:
        /**
         * Destructor.
         */
        ~MSBaseDevIO()
        {
        }

        /**
         * Used to read the property value.
         * This is pure virtual and has to be implemented in the derived DevIOs.
         * @throw ComponentErrors::PropertyError.
         * @param timestamp epoch when the operation completes.
         */
        virtual T read(ACS::Time& timestamp);

        /**
         * @return true to initialize the property with default value from CDB.
         */
        bool initializeValue()
        {
            return false;
        }

        /**
         * It writes values into controller. Unused because all the properties are read-only.
         */
        void write(const T& value, ACS::Time& timestamp)
        {
            timestamp = getTimeStamp();
            return;
        }
    };

    /**
     * This class is used to read the status of the motion of the minor servo system.
     */
    class MSMotionInfoDevIO : public MSBaseDevIO<ACE_CString>
    {
    public:
        /**
         * Constructor.
         * @param motion_status the atomic status of the motion of the minor servo system.
         * @param answer_map a reference to the SRTMinorServoAnswerMap object containing the status of the system. It is used to read the position of the gregorian cover.
         * @param scanning a reference to the TBoolean indicating whether the system is scanning or not.
         * @param current_scan a reference to the SRTMinorServoScan object containing the parameters for the current scan. It is used to read the servo name and axis involved in the scan.
         */
        MSMotionInfoDevIO(const std::atomic<SRTMinorServoMotionStatus>& motion_status, const SRTMinorServoAnswerMap& answer_map, const std::atomic<Management::TBoolean>& scanning, const SRTMinorServoScan& current_scan) :
            m_motion_status(motion_status),
            m_answer_map(answer_map),
            m_scanning(scanning),
            m_current_scan(current_scan)
        {}

        /**
         * Returns the property value.
         * @param timestamp epoch when the operation completes.
         * @return a string containing the information about the motion status of the minor servo system.
         */
        ACE_CString read(ACS::Time& timestamp)
        {
            std::string motion_status;
            SRTMinorServoAnswerMap answer_map = m_answer_map;

            switch(m_motion_status.load())
            {
                case MOTION_STATUS_UNCONFIGURED:
                {
                    motion_status = "Unknown";
                    break;
                }
                case MOTION_STATUS_STARTING:
                {
                    motion_status = "Setup in progress...";
                    break;
                }
                case MOTION_STATUS_CONFIGURED:
                {
                    motion_status = "Elevation Track Mode Disabled";
                    break;
                }
                case MOTION_STATUS_TRACKING:
                {
                    motion_status = "Elevation Track Mode";
                    break;
                }
                case MOTION_STATUS_PARKING:
                {
                    motion_status = "Parking...";
                    break;
                }
                case MOTION_STATUS_PARKED:
                {
                    motion_status = "Parked";

                    try
                    {
                        // If I can read the status of the gregorian cover I will notify the user about it on the GUI
                        SRTMinorServoGregorianCoverStatus cover_status = SRTMinorServoGregorianCoverStatus(answer_map.get<unsigned int>("GREGORIAN_CAP"));

                        if(cover_status == COVER_STATUS_OPEN)
                        {
                            motion_status += ", gregorian cover open";
                        }
                        else if(cover_status == COVER_STATUS_CLOSED)
                        {
                            motion_status += ", gregorian cover closed";
                        }
                    }
                    catch(...)
                    {
                        // If I can't, it doesn't matter
                    }

                    break;
                }
                case MOTION_STATUS_ERROR:
                {
                    motion_status = "Error";
                    break;
                }
            }

            if(m_scanning.load() == Management::MNG_TRUE)
            {
                motion_status = "Scanning along " + m_current_scan.servo_name + " " + m_current_scan.axis_name + " axis";
            }

            return motion_status.c_str();
        }
    private:
        /**
         * Reference to the motion status object of the Boss.
         */
        const std::atomic<SRTMinorServoMotionStatus>& m_motion_status;

        /**
         * Reference to the SRTMinorServoAnswerMap object of the Boss.
         */
        const SRTMinorServoAnswerMap& m_answer_map;

        /**
         * Reference to the boolean telling if the system is scanning.
         */
        const std::atomic<Management::TBoolean>& m_scanning;

        /**
         * Reference to the SRTMinorServoScan object of the Boss.
         */
        const SRTMinorServoScan& m_current_scan;
    };

    /**
     * This class is used to read the virtual positions of a minor servo system. It is specialized this way in order to sum the plain virtual positions and the offsets.
     */
    class MSVirtualPositionsDevIO : public MSBaseDevIO<ACS::doubleSeq>
    {
    public:
        /**
         * Constructor.
         * @param servo_name the name of the minor servo system the property belongs to.
         * @param virtual_positions_fields the name of the virtual positions fields inside the SRTMinorServoAnswerMap object.
         * @param virtual_offsets_fields the name of the virtual offsets fields inside the SRTMinorServoAnswerMap object.
         * @param answer_map the SRTMinorServoAnswerMap object containing the status of the minor servo system.
         */
        MSVirtualPositionsDevIO(const std::string& servo_name, std::vector<std::string> virtual_positions_fields, std::vector<std::string> virtual_offsets_fields, const SRTMinorServoAnswerMap& answer_map) :
            m_virtual_positions_fields([&]()
            {
                std::transform(virtual_positions_fields.begin(), virtual_positions_fields.end(), virtual_positions_fields.begin(), [servo_name](const std::string& field)
                {
                    return servo_name + "_" + field;
                });
                return virtual_positions_fields;
            }()),
            m_virtual_offsets_fields([&]()
            {
                std::transform(virtual_offsets_fields.begin(), virtual_offsets_fields.end(), virtual_offsets_fields.begin(), [servo_name](const std::string& field)
                {
                    return servo_name + "_" + field;
                });
                return virtual_offsets_fields;
            }()),
            m_answer_map(answer_map)
        {
        }

        /**
         * Returns the property value.
         * @param timestamp epoch when the operation completes.
         * @return a sequence of double containing the virtual positions taking the virtual offsets into account.
         */
        ACS::doubleSeq read(ACS::Time& timestamp)
        {
            SRTMinorServoAnswerMap answer_map = m_answer_map;

            ACS::doubleSeq sequence;

            try
            {
                sequence.length(m_virtual_positions_fields.size());

                for(size_t i = 0; i < m_virtual_positions_fields.size(); i++)
                {
                    sequence[i] = answer_map.get<double>(m_virtual_positions_fields[i]) - answer_map.get<double>(m_virtual_offsets_fields[i]);
                }
            }
            catch(std::out_of_range& ex)
            {
                _EXCPT(ComponentErrors::PropertyErrorExImpl, impl, "MSVirtualPositionsDevIO::read()");
                impl.setPropertyName("virtual_positions");
                impl.setReason("Property is missing from the map!");
                throw impl;
            }
            catch(std::bad_variant_access& ex)
            {
                _EXCPT(ComponentErrors::PropertyErrorExImpl, impl, "MSVirtualPositionsDevIO::read()");
                impl.setPropertyName("virtual_positions");
                impl.setReason("Attempt to access the property with the wrong variant type!");
                throw impl;
            }
            catch(ACSErr::ACSbaseExImpl& ex)
            {
                _ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl, impl, ex, "MSVirtualPositionsDevIO::read()");
                impl.setPropertyName("virtual_positions");
                impl.setReason("Property could not be read!");
                throw impl;
            }

            timestamp = getTimeStamp(); //completion time
            return sequence;
        }

    private:
        /**
         * The virtual positions fields names.
         */
        const std::vector<std::string> m_virtual_positions_fields;

        /**
         * The virtual offsets fields names.
         */
        const std::vector<std::string> m_virtual_offsets_fields;

        /**
         * The SRTMinorServoAnswerMap containing the status of the minor servo system.
         */
        const SRTMinorServoAnswerMap& m_answer_map;
    };

    /**
     * This template class is used to retrieve values from a SRTMinorServoAnswerMap and provide them as properties.
     * The templates is specialized for the types listed right below and compilation will fail if the developer attempts to use it for an unknown MSDevIO type.
     */
    template <typename T, typename = std::enable_if<is_any_v<T, 
        Management::TBoolean,
        CORBA::Double,
        ACE_CString,
        ACS::booleanSeq,
        ACS::doubleSeq,
        SRTMinorServoFocalConfiguration,
        SRTMinorServoControlStatus,
        SRTMinorServoGregorianCoverStatus,
        SRTMinorServoCabinetStatus,
        SRTMinorServoOperativeMode
    >>>
    class MSAnswerMapDevIO : public MSBaseDevIO<T>
    {
    public:
        /**
         * Single property-field, no servo-name constructor.
         * @param property_name the name of the property, used when raising exceptions.
         * @param property_field the field name of the property as it appears inside the SRTMinorServoAnswerMap.
         * @param answer_map the reference to the SRTMinorServoAnswerMap containing the readings from the PLC.
         */
        MSAnswerMapDevIO(const std::string& property_name, const std::string& property_field, const SRTMinorServoAnswerMap& answer_map) :
            MSAnswerMapDevIO(property_name, std::vector<std::string>{ property_field }, answer_map) {}

        /**
         * Multiple property-fields, no servo-name constructor.
         * @param property_name the name of the property, used when raising exceptions.
         * @param property_fields the field names of the property as they appear inside the SRTMinorServoAnswerMap.
         * @param answer_map the reference to the SRTMinorServoAnswerMap containing the readings from the PLC.
         */
        MSAnswerMapDevIO(const std::string& property_name, const std::vector<std::string>& property_fields, const SRTMinorServoAnswerMap& answer_map) :
            m_property_name(property_name),
            m_property_fields(property_fields),
            m_answer_map(answer_map) {}

        /**
         * Single property-field, with servo-name constructor.
         * @param servo_name the name of the minor servo system the property belongs to. Used as prefix for the property_field argument.
         * @param property_name the name of the property, used when raising exceptions.
         * @param property_field the field name of the property as it appears inside the SRTMinorServoAnswerMap.
         * @param answer_map the reference to the SRTMinorServoAnswerMap containing the readings from the PLC.
         */
        MSAnswerMapDevIO(const std::string& servo_name, const std::string& property_name, const std::string& property_field, const SRTMinorServoAnswerMap& answer_map) :
            MSAnswerMapDevIO(servo_name, property_name, std::vector<std::string>{ property_field }, answer_map) {}

        /**
         * Multiple property-fields, with servo-name constructor.
         * @param servo_name the name of the minor servo system the property belongs to. Used as prefix for the property_fields argument.
         * @param property_name the name of the property, used when raising exceptions.
         * @param property_fields the field names of the property as they appear inside the SRTMinorServoAnswerMap.
         * @param answer_map the reference to the SRTMinorServoAnswerMap containing the readings from the PLC.
         */
        MSAnswerMapDevIO(const std::string& servo_name, const std::string& property_name, std::vector<std::string> property_fields, const SRTMinorServoAnswerMap& answer_map) :
            m_property_name(property_name),
            m_property_fields([&]()
            {
                std::transform(property_fields.begin(), property_fields.end(), property_fields.begin(), [servo_name](const std::string& field)
                {
                    return servo_name + "_" + field;
                });
                return property_fields;
            }()),
            m_answer_map(answer_map) {}

        /**
         * Used to read the property value.
         * @param timestamp epoch when the operation completes.
         * @throw ComponentErrors::PropertyError.
         * @return the property value as read from the SRTMinorServoAnswerMap object reference.
         */
        T read(ACS::Time& timestamp)
        {
            timestamp = getTimeStamp();

            // Copy the answer map by value. This will ensure all the elements for any sequence belong to the same reading from PLC.
            SRTMinorServoAnswerMap answer_map = m_answer_map;

            try
            {
                if constexpr(std::is_same_v<T, Management::TBoolean>)
                {
                    return (answer_map.get<unsigned int>(m_property_fields[0]) == 1) ? Management::MNG_TRUE : Management::MNG_FALSE;
                }
                else if constexpr(std::is_same_v<T, CORBA::Double>)
                {
                    unsigned int index = answer_map.index(m_property_fields[0]);

                    switch(index)
                    {
                        case 0:
                        {
                            return (double)answer_map.get<long>(m_property_fields[0]);
                        }
                        case 1:
                        {
                            return answer_map.get<double>(m_property_fields[0]);
                        }
                        default:
                        {
                            throw std::bad_variant_access();
                        }
                    }
                }
                else if constexpr(std::is_same_v<T, ACE_CString>)
                {
                    unsigned int index = answer_map.index(m_property_fields[0]);

                    switch(index)
                    {
                        case 0:
                        {
                            return std::to_string(answer_map.get<long>(m_property_fields[0])).c_str();
                        }
                        case 1:
                        {
                            return std::to_string(answer_map.get<double>(m_property_fields[0])).c_str();
                        }
                        default:
                        {
                            return answer_map.get<std::string>(m_property_fields[0]).c_str();
                        }
                    }
                }
                else if constexpr(std::is_same_v<T, ACS::booleanSeq>)
                {
                    ACS::booleanSeq value;
                    value.length(m_property_fields.size());

                    for(size_t i = 0; i < m_property_fields.size(); i++)
                    {
                        value[i] = answer_map.get<long>(m_property_fields[i]);
                    }
                    return value;
                }
                else if constexpr(std::is_same_v<T, ACS::doubleSeq>)
                {
                    ACS::doubleSeq value;
                    value.length(m_property_fields.size());

                    for(size_t i = 0; i < m_property_fields.size(); i++)
                    {
                        value[i] = answer_map.get<double>(m_property_fields[i]);
                    }
                    return value;
                }
                else if constexpr(std::is_same_v<T, SRTMinorServoFocalConfiguration>)
                {
                    return LDOConfigurationIDTable.right.at(answer_map.get<int>(m_property_fields[0]));
                }
                else if constexpr(std::is_same_v<T, SRTMinorServoControlStatus>)
                {
                    return SRTMinorServoControlStatus(answer_map.get<unsigned int>(m_property_fields[0]) - 1);
                }
                else if constexpr(std::is_same_v<T, SRTMinorServoGregorianCoverStatus>)
                {
                    return SRTMinorServoGregorianCoverStatus(answer_map.get<unsigned int>(m_property_fields[0]));
                }
                else if constexpr(std::is_same_v<T, SRTMinorServoCabinetStatus>)
                {
                    return SRTMinorServoCabinetStatus(answer_map.get<unsigned int>(m_property_fields[0]) - 1);
                }
                else if constexpr(std::is_same_v<T, SRTMinorServoOperativeMode>)
                {
                    return SRTMinorServoOperativeMode(answer_map.get<unsigned int>(m_property_fields[0]) / 10);
                }
            }
            catch(std::out_of_range& ex)
            {
                _EXCPT(ComponentErrors::PropertyErrorExImpl, impl, "MSAnswerMapDevIO::read()");
                impl.setPropertyName(m_property_name.c_str());
                impl.setReason("Property is missing from the map!");
                throw impl;
            }
            catch(std::bad_variant_access& ex)
            {
                _EXCPT(ComponentErrors::PropertyErrorExImpl, impl, "MSAnswerMapDevIO::read()");
                impl.setPropertyName(m_property_name.c_str());
                impl.setReason("Attempt to access the property with the wrong variant type!");
                throw impl;
            }
            catch(ACSErr::ACSbaseExImpl& ex)
            {
                _ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl, impl, ex, "MSAnswerMapDevIO::read()");
                impl.setPropertyName(m_property_name.c_str());
                impl.setReason("Property could not be read!");
                throw impl;
            }
        }

    private:
        /**
         * The name of the property, used when raising exceptions.
         */
        const std::string m_property_name;

        /**
         * The field names of the property as they appear on the SRTMinorServoAnswerMap.
         */
        const std::vector<std::string> m_property_fields;

        /**
         * The reference to the SRTMinorServoAnswerMap in whichthe readings from the PLC appear.
         */
        const SRTMinorServoAnswerMap& m_answer_map;
    };

    /**
     * This template class represents a generic Minor Servo DevIO.
     * It accepts 2 types, the DevIO type (the return type) and the type of the object reference which stores the original value to be returned by the read method.
     * The templates is specialized for the combinations of types listed right below and the compilation will fail if the developer attempts to use it with any other types combination.
     */
    template <typename C, typename A, typename = std::enable_if_t<
        is_any_v<C, Management::TSystemStatus, Management::TBoolean, ACE_CString, CORBA::Long> || (std::is_same_v<C, ACS::doubleSeq> && std::is_same_v<A, std::vector<double>>)
    >>
    class MSGenericDevIO : public MSBaseDevIO<C>
    {
    public:
        /**
         * Default constructor.
         * @param value a constant reference to the object from which the DevIO will read the value to be returned as property.
         */
        MSGenericDevIO(const A& value) : m_value(value) {}

        /**
         * Used to read the property value.
         * @param timestamp epoch when the operation completes.
         * @return the property value read from the original referenced object.
         */
        C read(ACS::Time& timestamp)
        {
            timestamp = getTimeStamp(); //completion time

            if constexpr(std::is_same_v<A, std::string>)
            {
                return m_value.c_str();
            }
            else if constexpr(std::is_same_v<C, ACS::doubleSeq>)
            {
                ACS::doubleSeq_var sequence = new ACS::doubleSeq;
                sequence->length(m_value.size());

                for(size_t i = 0; i < m_value.size(); i++)
                {
                    sequence[i] = m_value.operator[](i);
                }

                return sequence;
            }
            else if constexpr(is_atomic_v<A>)
            {
                return m_value.load();
            }
            else
            {
                return m_value;
            }
        }

        /**
         * The reference to the object containing the value to be returned as property.
         */
        const A& m_value;
    };
}

#endif
