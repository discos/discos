#ifndef __SRTMINORSERVOCOMMON_H__
#define __SRTMINORSERVOCOMMON_H__

/**
 * SRTMinorServoCommon.h
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "SuppressWarnings.h"
#include <SRTMinorServoCommonS.h>
#include <map>
#include <vector>
#include <boost/bimap.hpp>
#include <boost/assign.hpp>

/**
 * programTrack constants. The first indicates the time between two consecutive points, the second indicates the time we send each point in advance.
 */
#define PROGRAM_TRACK_TIMEGAP        2000000    //200 milliseconds, time between each programTrack point
#define PROGRAM_TRACK_FUTURE_TIME    26000000   //2.6 seconds, we send points this amount of time before their actual timestamp

/**
 * Macro used to link the properties pointers to their methods.
 */
#define GET_PROPERTY_REFERENCE(TYPE, CLASSNAME, PROPERTY, PROPERTYNAME) TYPE##_ptr CLASSNAME::PROPERTYNAME()\
{ \
	if (PROPERTY==0) return TYPE::_nil(); \
	TYPE##_var tmp=TYPE::_narrow(PROPERTY->getCORBAReference()); \
	return tmp._retn(); \
}


namespace MinorServo
{
    /**
     * This dictionary contains information regarding the possibile focal configurations.
     * The key indicates the configuration as known by DISCOS.
     * The value is a pair, the first element of the pair is the DISCOS enumeration for the relative Leonardo configuration,
     * the second element of the pair indicates whether the configuration has a ASACTIVE twin configuraiton.
     */
    using DiscosConfigurationNameTableType = std::map<std::string, std::pair<SRTMinorServoFocalConfiguration, bool>>;
    const DiscosConfigurationNameTableType DiscosConfigurationNameTable =
    {
        {"LLP",     std::make_pair(CONFIGURATION_PRIMARY,    false)},
        {"PPP",     std::make_pair(CONFIGURATION_PRIMARY,    false)},
        {"PLP",     std::make_pair(CONFIGURATION_PRIMARY,    false)},
        {"HHP",     std::make_pair(CONFIGURATION_PRIMARY,    false)},
        {"XKP",     std::make_pair(CONFIGURATION_PRIMARY,    false)},
        {"CCG",     std::make_pair(CONFIGURATION_GREGORIAN1, true )},
        {"KKG",     std::make_pair(CONFIGURATION_GREGORIAN2, true )},
        {"WWG",     std::make_pair(CONFIGURATION_GREGORIAN3, true )},
        {"QQG",     std::make_pair(CONFIGURATION_GREGORIAN4, true )},
        {"TRI",     std::make_pair(CONFIGURATION_GREGORIAN5, true )},
        {"MISTRAL", std::make_pair(CONFIGURATION_GREGORIAN6, true )},
        {"CCB",     std::make_pair(CONFIGURATION_BWG1,       true )},
        {"XB",      std::make_pair(CONFIGURATION_BWG3,       true )},
    };

    /**
     * This dictionary contains the Leonardo focal configurations DISCOS enumerations, alongside their name inside the Leonardo minor servo system.
     */
    using LDOConfigurationNameTableType = boost::bimap<SRTMinorServoFocalConfiguration, std::string>;
    const LDOConfigurationNameTableType LDOConfigurationNameTable = boost::assign::list_of<LDOConfigurationNameTableType::relation>
        (CONFIGURATION_PRIMARY,     "Primario")
        (CONFIGURATION_GREGORIAN1,  "Gregoriano 1")
        (CONFIGURATION_GREGORIAN2,  "Gregoriano 2")
        (CONFIGURATION_GREGORIAN3,  "Gregoriano 3")
        (CONFIGURATION_GREGORIAN4,  "Gregoriano 4")
        (CONFIGURATION_GREGORIAN5,  "Gregoriano 5")
        (CONFIGURATION_GREGORIAN6,  "Gregoriano 6")
        (CONFIGURATION_GREGORIAN7,  "Gregoriano 7")
        (CONFIGURATION_GREGORIAN8,  "Gregoriano 8")
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

    /**
     * This is a simple list indicating the order of the minor servo components.
     */
    const std::vector<std::string> ServoOrder =
    {
        //"PFP",
        "SRP",
        //"GFR",
        //"M3R"
    };
}

#endif
