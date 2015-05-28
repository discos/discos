#include "MedMinorServoTime.hpp"

ACS::Time 
MedMinorServoTime::servoToACSTime(uint64_t time)
{ 
    return static_cast<ACS::Time>(time + SYSTIMEBASE_K_GREGORIAN_100NSEC_OFFSET); 
}

/*ACS::Time
MedMinorServoTime::servoToACSTime(uint64_t time)
{
    uint32_t hi, low;
    hi = (uint32_t)((time >> 32) && (0x0000FFFF));
    low = (uint32_t)(time && 0x0000FFFF);
    return (ACS::Time)(((unsigned long long)4294967295) * hi + low);
}*/

uint64_t
MedMinorServoTime::ACSToServoTime(ACS::Time time)
{ 
    return time - SYSTIMEBASE_K_GREGORIAN_100NSEC_OFFSET; 
}

uint64_t
MedMinorServoTime::ACSToServoNow()
{ 
    return ACSToServoTime(IRA::CIRATools::getACSTime());
}

ACS::TimeInterval
MedMinorServoTime::deltaToACSTimeInterval(double delta)
{
     ACS::TimeInterval acstime = static_cast<unsigned long long>(delta * 10000000);
     return acstime;
}
