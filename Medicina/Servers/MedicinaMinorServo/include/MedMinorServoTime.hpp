#ifndef MEDMINORSERVOTIME_HPP
#define MEDMINORSERVOTIME_HPP

#include <stdint.h>

#include <acstime.h>

#include "IRA"

#define SYSTIMEBASE_K_DAY_TO_HOUR               ((uint64_t)24)
#define SYSTIMEBASE_K_HOUR_TO_MIN               ((uint64_t)60)
#define SYSTIMEBASE_K_MIN_TO_SEC                ((uint64_t)60)
#define SYSTIMEBASE_K_SEC_TO_MSEC               ((uint64_t)1000)
#define SYSTIMEBASE_K_MSEC_TO_USEC              ((uint64_t)1000)
#define SYSTIMEBASE_K_USEC_TO_NSEC              ((uint64_t)1000)
#define SYSTIMEBASE_K_USEC_TO_100NSEC           ((uint64_t)10)
#define SYSTIMEBASE_K_100NSEC                   ((uint64_t)100)

//#define SYSTIMEBASE_K_MSEC_TO_100NSEC ((uint64_t)((SYSTIMEBASE_K_MSEC_TO_USEC * SYSTIMEBASE_K_USEC_TO_NSEC) / SYSTIMEBASE_K_100NSEC))
#define SYSTIMEBASE_K_MSEC_TO_100NSEC ((uint64_t)((SYSTIMEBASE_K_MSEC_TO_USEC * SYSTIMEBASE_K_USEC_TO_100NSEC)))
#define SYSTIMEBASE_K_SEC_TO_100NSEC            ((uint64_t)(SYSTIMEBASE_K_SEC_TO_MSEC * SYSTIMEBASE_K_MSEC_TO_100NSEC))
#define SYSTIMEBASE_K_DAY_TO_100NSEC            ((uint64_t)(SYSTIMEBASE_K_DAY_TO_HOUR * SYSTIMEBASE_K_HOUR_TO_MIN * SYSTIMEBASE_K_MIN_TO_SEC * SYSTIMEBASE_K_SEC_TO_100NSEC))

// For MicroSoft, Gregorian Calendar starts from 00:00:00 of 01/01/1601.
// Really,        Gregorian Calendar starts from 00:00:00 of 15/10/1582.
// Difference in days = 6653 days. (MicroFot(00:00:00_01/01/1601) -
// Gregorian(00:00:00_15/10/1582))
// time_t : number of seconds elapsed since 00:00:00 of 01/01/1970 UTC.
#define SYSTIMEBASE_K_GREGORIAN_DAY_OFFSET      ((uint64_t)6653)
#define SYSTIMEBASE_K_GREGORIAN_100NSEC_OFFSET ((uint64_t)(SYSTIMEBASE_K_DAY_TO_100NSEC * SYSTIMEBASE_K_GREGORIAN_DAY_OFFSET))

class MedMinorServoTime
{
    public:
        static 
        ACS::Time 
        servoToACSTime(uint64_t time);

        static 
        uint64_t
        ACSToServoTime(ACS::Time time);

        static
        uint64_t
        ACSToServoNow();

        static
        ACS::TimeInterval
        deltaToACSTimeInterval(double delta);
};

#endif

