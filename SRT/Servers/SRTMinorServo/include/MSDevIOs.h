#ifndef _MSDEVIOS_H
#define _MSDEVIOS_H

/**
 * MSDevIOs.h
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "SuppressWarnings.h"
#include "SRTMinorServoContainers.h"
#include <type_traits>
#include <baciDevIO.h>
#include <ComponentErrors.h>
#include "SRTMinorServoBossCore.h"


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
        MSMotionInfoDevIO(const std::atomic<SRTMinorServoMotionStatus>& motion_status, const SRTMinorServoGeneralStatus& boss_status, const std::atomic<Management::TBoolean>& scanning, const SRTMinorServoScan& current_scan) :
            m_motion_status(motion_status),
            m_boss_status(boss_status),
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
                        SRTMinorServoGregorianCoverStatus cover_position = m_boss_status.getGregorianCoverPosition();

                        if(cover_position == COVER_STATUS_OPEN)
                        {
                            motion_status += ", gregorian cover open";
                        }
                        else if(cover_position == COVER_STATUS_CLOSED)
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
         * Reference to the SRTMinorServoGeneralStatus object of the Boss.
         */
        const SRTMinorServoGeneralStatus& m_boss_status;

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
     * This template class is used to retrieve values from a SRTMinorServoAnswerMap and provide them as properties.
     * The templates is specialized for the types listed right below and compilation will fail if the developer attempts to use it for an unknown MSDevIO type.
     */
    template <typename X, typename Y, typename = std::enable_if<
        is_any_v<Y, SRTMinorServoGeneralStatus, SRTMinorServoStatus>
        && is_any_v<X,
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
        >
    >>
    class MSAnswerMapDevIO : public MSBaseDevIO<X>
    {
    public:
        /**
         * Constructor, accepting the SRTMinorServoAnswerMap derived object and a pointer to the method used to retrieve the DevIO value.
         * @param map, the SRTMinorServoAnswerMap derived object.
         * @param method, the method to call in order to retrieve the return value.
         */
        MSAnswerMapDevIO(const std::string& property_name, const Y& map, X (Y::*method)() const) : m_property_name(property_name), m_map(map), m_method(method) {}

        /**
         * Used to read the property value.
         * @param timestamp epoch when the operation completes.
         * @throw ComponentErrors::PropertyError.
         * @return the property value as read from the SRTMinorServoAnswerMap object reference.
         */
        X read(ACS::Time& timestamp)
        {
            timestamp = getTimeStamp();

            try
            {
                return (m_map.*m_method)();
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
         * The name of the property.
         */
        const std::string m_property_name;

        /**
         * The reference to the SRTMinorServoAnswerMap in which the readings from the PLC appear. This could be either a SRTMinorServoGeneralStatus or a SRTMinorServoStatus.
         */
        const Y& m_map;

        /**
         * Pointer to the method of the SRTMinorServoAnswerMap to call in order to retrieve the DevIO return value.
         */
        X (Y::*m_method)() const;
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
