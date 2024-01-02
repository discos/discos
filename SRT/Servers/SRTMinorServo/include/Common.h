#ifndef __SRTMINORSERVOCOMMON_H__
#define __SRTMINORSERVOCOMMON_H__

#include "SuppressWarnings.h"
#include <SRTMinorServoCommonS.h>
#include <boost/bimap.hpp>
#include <boost/assign.hpp>


namespace MinorServo
{
    // The second value of the pair indicates whether the configuration has a ASACTIVE double configuration
    using DiscosConfigurationNameTableType = std::map<std::string, std::pair<MinorServo::SRTMinorServoFocalConfiguration, bool> >;
    const DiscosConfigurationNameTableType DiscosConfigurationNameTable =
    {
        {"LLP",     std::make_pair(MinorServo::CONFIGURATION_PRIMARY,    false)},
        {"PPP",     std::make_pair(MinorServo::CONFIGURATION_PRIMARY,    false)},
        {"PLP",     std::make_pair(MinorServo::CONFIGURATION_PRIMARY,    false)},
        {"HHP",     std::make_pair(MinorServo::CONFIGURATION_PRIMARY,    false)},
        {"XKP",     std::make_pair(MinorServo::CONFIGURATION_PRIMARY,    false)},
        {"CCG",     std::make_pair(MinorServo::CONFIGURATION_GREGORIAN1, true )},
        {"KKG",     std::make_pair(MinorServo::CONFIGURATION_GREGORIAN2, true )},
        {"WWG",     std::make_pair(MinorServo::CONFIGURATION_GREGORIAN3, true )},
        {"QQG",     std::make_pair(MinorServo::CONFIGURATION_GREGORIAN4, true )},
        {"TRI",     std::make_pair(MinorServo::CONFIGURATION_GREGORIAN5, true )},
        {"MISTRAL", std::make_pair(MinorServo::CONFIGURATION_GREGORIAN6, true )},
        {"CCB",     std::make_pair(MinorServo::CONFIGURATION_BWG1,       true )},
        {"XB",      std::make_pair(MinorServo::CONFIGURATION_BWG3,       true )},
    };

    using LDOConfigurationNameTableType = boost::bimap<MinorServo::SRTMinorServoFocalConfiguration, std::string>;
    const LDOConfigurationNameTableType LDOConfigurationNameTable = boost::assign::list_of< LDOConfigurationNameTableType::relation >
        (MinorServo::CONFIGURATION_PRIMARY,     "Primario")
        (MinorServo::CONFIGURATION_GREGORIAN1,  "Gregoriano 1")
        (MinorServo::CONFIGURATION_GREGORIAN2,  "Gregoriano 2")
        (MinorServo::CONFIGURATION_GREGORIAN3,  "Gregoriano 3")
        (MinorServo::CONFIGURATION_GREGORIAN4,  "Gregoriano 4")
        (MinorServo::CONFIGURATION_GREGORIAN5,  "Gregoriano 5")
        (MinorServo::CONFIGURATION_GREGORIAN6,  "Gregoriano 6")
        (MinorServo::CONFIGURATION_GREGORIAN7,  "Gregoriano 7")
        (MinorServo::CONFIGURATION_GREGORIAN8,  "Gregoriano 8")
        (MinorServo::CONFIGURATION_BWG1,        "BWG1")
        (MinorServo::CONFIGURATION_BWG2,        "BWG2")
        (MinorServo::CONFIGURATION_BWG3,        "BWG3")
        (MinorServo::CONFIGURATION_BWG4,        "BWG4");

    using LDOConfigurationIDTableType = boost::bimap<MinorServo::SRTMinorServoFocalConfiguration, unsigned int>;
    const LDOConfigurationIDTableType LDOConfigurationIDTable = boost::assign::list_of< LDOConfigurationIDTableType::relation >
        (MinorServo::CONFIGURATION_UNKNOWN,     0)
        (MinorServo::CONFIGURATION_PRIMARY,     1)
        (MinorServo::CONFIGURATION_GREGORIAN1,  11)
        (MinorServo::CONFIGURATION_GREGORIAN2,  12)
        (MinorServo::CONFIGURATION_GREGORIAN3,  13)
        (MinorServo::CONFIGURATION_GREGORIAN4,  14)
        (MinorServo::CONFIGURATION_GREGORIAN5,  15)
        (MinorServo::CONFIGURATION_GREGORIAN6,  16)
        (MinorServo::CONFIGURATION_GREGORIAN7,  17)
        (MinorServo::CONFIGURATION_GREGORIAN8,  18)
        (MinorServo::CONFIGURATION_BWG1,        21)
        (MinorServo::CONFIGURATION_BWG2,        22)
        (MinorServo::CONFIGURATION_BWG3,        23)
        (MinorServo::CONFIGURATION_BWG4,        24);

    const std::vector<std::string> ServoOrder = 
    {
        "PFP",
        "SRP",
        "GFR",
        "M3R",
    };
}

#endif
