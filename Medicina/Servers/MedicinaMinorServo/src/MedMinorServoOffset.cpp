#include "MedMinorServoOffset.hpp"

MedMinorServoOffset::MedMinorServoOffset():
    _initialized(false)
{}

MedMinorServoOffset::~MedMinorServoOffset()
{}

void 
MedMinorServoOffset::initialize(bool primary_focus)
{
    boost::recursive_mutex::scoped_lock lock(_offset_guard);
    _primary_focus = primary_focus;
    _initialized = true;
    clearOffset();
}

void 
MedMinorServoOffset::setUserOffset(std::vector<double> offset)
{
    if(!_initialized)
        throw MinorServoOffsetError("Offset has not been initialized");
    if((_primary_focus) &&
       (offset.size() != 2))
        throw MinorServoOffsetError("Worng offset size");
    if((!_primary_focus) &&
       (offset.size() != 5))
        throw MinorServoOffsetError("Worng offset size");
    boost::recursive_mutex::scoped_lock lock(_offset_guard);
    _user_offset = offset;
}

void 
MedMinorServoOffset::setUserOffset(int axis, double value)
{
    if(!_initialized)
        throw MinorServoOffsetError("Offset has not been initialized");
    if((_primary_focus) &&
       (axis >= 2))
        throw MinorServoOffsetError("Wrong axis value");
    if((!_primary_focus) &&
       (axis >= 5))
        throw MinorServoOffsetError("Wrong axis value");
    boost::recursive_mutex::scoped_lock lock(_offset_guard);
    _user_offset[axis] = value;
}

void 
MedMinorServoOffset::setSystemOffset(std::vector<double> offset)
{
    if(!_initialized)
        throw MinorServoOffsetError("Offset has not been initialized");
    if((_primary_focus) &&
       (offset.size() != 2))
        throw MinorServoOffsetError("Worng offset size");
    if((!_primary_focus) &&
       (offset.size() != 5))
        throw MinorServoOffsetError("Worng offset size");
    boost::recursive_mutex::scoped_lock lock(_offset_guard);
    _system_offset = offset;
}

void 
MedMinorServoOffset::setSystemOffset(int axis, double value)
{
    if(!_initialized)
        throw MinorServoOffsetError("Offset has not been initialized");
    if((_primary_focus) &&
       (axis >= 2))
        throw MinorServoOffsetError("Wrong axis value");
    if((!_primary_focus) &&
       (axis >= 5))
        throw MinorServoOffsetError("Wrong axis value");
    boost::recursive_mutex::scoped_lock lock(_offset_guard);
    _system_offset[axis] = value;
}

void
MedMinorServoOffset::clearUserOffset()
{
    if(!_initialized)
        throw MinorServoOffsetError("Offset has not been initialized");
    boost::recursive_mutex::scoped_lock lock(_offset_guard);
    if(_primary_focus){
        _user_offset = std::vector<double>(2, 0.0);
    }else{
        _user_offset = std::vector<double>(5, 0.0);
    }
}

void
MedMinorServoOffset::clearSystemOffset()
{
    if(!_initialized)
        throw MinorServoOffsetError("Offset has not been initialized");
    boost::recursive_mutex::scoped_lock lock(_offset_guard);
    if(_primary_focus){
        _system_offset = std::vector<double>(2, 0.0);
    }else{
        _system_offset = std::vector<double>(5, 0.0);
    }
}

void
MedMinorServoOffset::clearOffset()
{
    clearUserOffset();
    clearSystemOffset();
}

MedMinorServoPosition
MedMinorServoOffset::getSystemOffsetPosition()
{
    MedMinorServoPosition position;
    position.mode = MED_MINOR_SERVO_OFFSET;
    if(!_initialized)
        throw MinorServoOffsetError("Offset has not been initialized");
    boost::recursive_mutex::scoped_lock lock(_offset_guard);
    if(_primary_focus){
        position.y = _system_offset[0];
        position.z = _system_offset[1];
    }else{
        position.x = _system_offset[0];
        position.y = _system_offset[1];
        position.z = _system_offset[2];
        position.theta_x = _system_offset[3];
        position.theta_y = _system_offset[4];
    }
    return position;
}

MedMinorServoPosition
MedMinorServoOffset::getUserOffsetPosition()
{
    MedMinorServoPosition position;
    position.mode = MED_MINOR_SERVO_OFFSET;
    if(!_initialized)
        throw MinorServoOffsetError("Offset has not been initialized");
    boost::recursive_mutex::scoped_lock lock(_offset_guard);
    if(_primary_focus){
        position.y = _user_offset[0];
        position.z = _user_offset[1];
    }else{
        position.x = _user_offset[0];
        position.y = _user_offset[1];
        position.z = _user_offset[2];
        position.theta_x = _user_offset[3];
        position.theta_y = _user_offset[4];
    }
    return position;
}

MedMinorServoPosition
MedMinorServoOffset::getOffsetPosition()
{
    MedMinorServoPosition position;
    position.mode = MED_MINOR_SERVO_OFFSET;
    if(!_initialized)
        throw MinorServoOffsetError("Offset has not been initialized");
    boost::recursive_mutex::scoped_lock lock(_offset_guard);
    if(_primary_focus){
        position.y = _user_offset[0] + _system_offset[0];
        position.z = _user_offset[1] + _system_offset[1];
    }else{
        position.x = _user_offset[0] + _system_offset[0];
        position.y = _user_offset[1] + _system_offset[1];
        position.z = _user_offset[2] + _system_offset[2];
        position.theta_x = _user_offset[3] + _system_offset[3];
        position.theta_y = _user_offset[4] + _system_offset[4];
    }
    return position;
}

