#include "MedMinorServoTime.hpp"

ACS::Time 
MedMinorServoTime::servoToACSTime(uint64_t time)
{ 
    return time + SYSTIMEBASE_K_GREGORIAN_100NSEC_OFFSET; 
}

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
