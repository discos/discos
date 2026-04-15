#ifndef __SRTMINORSERVOCOMMON_H__
#define __SRTMINORSERVOCOMMON_H__

/**
 * SRTMinorServoCommon.h
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

/**
 * programTrack constants. The first indicates the time between two consecutive points, the second indicates the time we send each point in advance.
 */
#define PROGRAM_TRACK_TIMEGAP        2000000    //200 milliseconds, time between each programTrack point
#define PROGRAM_TRACK_FUTURE_TIME    26000000   //2.6 seconds, we send points this amount of time before their actual timestamp

/**
 * Macro used to link the properties pointers to their methods.
 */
#define GET_PROPERTY_REFERENCE(TYPE, CLASSNAME, PROPERTY, PROPERTYNAME) TYPE##_ptr CLASSNAME::PROPERTYNAME() \
{ \
    if (PROPERTY==0) return TYPE::_nil(); \
    TYPE##_var tmp=TYPE::_narrow(PROPERTY->getCORBAReference()); \
    return tmp._retn(); \
}

#endif
