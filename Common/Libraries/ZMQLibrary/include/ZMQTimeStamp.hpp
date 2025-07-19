#ifndef __ZMQTIMESTAMP_HPP__
#define __ZMQTIMESTAMP_HPP__

#include <IRA>
#include "ZMQDictionary.hpp"

namespace ZMQLibrary
{
    namespace ZMQTimeStamp
    {
        ZMQDictionary now();
        ZMQDictionary fromMJD(const double& MJD);
        ZMQDictionary fromUNIXTime(const double& unix_epoch);
        ZMQDictionary fromACSTime(const ACS::Time& acs_time);
    }
}

#endif /*__ZMQTIMESTAMP_HPP__*/
