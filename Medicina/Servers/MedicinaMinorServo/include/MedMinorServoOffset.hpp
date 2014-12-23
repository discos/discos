#ifndef MEDMINORSERVOOFFSET_HPP
#define MEDMINORSERVOOFFSET_HPP

#include <string.h>
#include <vector>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include "MedMinorServoGeometry.hpp"

class MinorServoOffsetError: public std::runtime_error
{
    public:
        MinorServoOffsetError(const char *msg): std::runtime_error(std::string(msg)){};
};

class MedMinorServoOffset
{
    public:
        MedMinorServoOffset();
        virtual ~MedMinorServoOffset();
        void setUserOffset(std::vector<double>);
        void setUserOffset(int axis, double value);
        void setSystemOffset(std::vector<double>);
        void setSystemOffset(int axis, double value);
        std::vector<double> getUserOffset(){ return _user_offset;};
        std::vector<double> getSystemOffset(){ return _system_offset;};
        void clearUserOffset();
        void clearSystemOffset();
        void clearOffset();
        MedMinorServoPosition getOffsetPosition();
        void initialize(bool primary_focus);
    private:
        std::vector<double> _user_offset, _system_offset;
        bool _primary_focus, _initialized;
        boost::recursive_mutex _offset_guard;
};

#endif
