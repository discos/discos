#include "ZMQTimeStamp.hpp"

namespace ZMQLibrary
{
    namespace ZMQTimeStamp
    {
        ZMQDictionary fromMJD(const double& MJD)
        {
            IRA::CDateTime clock;
            TIMEVALUE time;
            clock.setMJD(MJD);
            clock.getDateTime(time);
            double unix_time = IRA::CIRATools::ACSTime2UNIXTime(time.value().value);

            ZMQDictionary timestamp;
            timestamp["jd"] = clock.getJD();
            timestamp["mjd"] = MJD;
            timestamp["acs_time"] = time.value().value;
            timestamp["unix_time"] = unix_time;
            timestamp["iso8601"] = IRA::CIRATools::ACSTime2ISO8601(time.value().value);
            return timestamp;
        }

        ZMQDictionary fromUNIXTime(const double& unix_time)
        {
            ACS::Time acs_time = IRA::CIRATools::UNIXTime2ACSTime(unix_time);
            TIMEVALUE time(acs_time);
            IRA::CDateTime clock(time);

            ZMQDictionary timestamp;
            timestamp["unix_time"] = unix_time;
            timestamp["acs_time"] = acs_time;
            timestamp["jd"] = clock.getJD();
            timestamp["mjd"] = clock.getMJD();
            timestamp["iso8601"] = IRA::CIRATools::ACSTime2ISO8601(acs_time);
            return timestamp;
        }

        ZMQDictionary fromACSTime(const ACS::Time& acs_time)
        {
            TIMEVALUE time(acs_time);
            IRA::CDateTime clock(time);
            double unix_time = IRA::CIRATools::ACSTime2UNIXTime(acs_time);

            ZMQDictionary timestamp;
            timestamp["jd"] = clock.getJD();
            timestamp["mjd"] = clock.getMJD();
            timestamp["unix_time"] = unix_time;
            timestamp["acs_time"] = acs_time;
            timestamp["iso8601"] = IRA::CIRATools::ACSTime2ISO8601(acs_time);
            return timestamp;
        }
    }
}
