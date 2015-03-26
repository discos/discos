#include "PositionQueue.hpp"

PositionQueue::PositionQueue(int max_size) :
    _max_size(max_size),
    _size(0),
    _oldest_time(0),
    _newest_time(0)
{}

bool
PositionQueue::empty()
{
    boost::mutex::scoped_lock lock(_queue_guard);
    return _queue.empty();
}

void 
PositionQueue::clear()
{
    boost::mutex::scoped_lock lock(_queue_guard);
    if(_size > 0){
        _queue.clear();
        _size = 0;
    }
}

bool
PositionQueue::push(const MedMinorServoPosition& position)
{
    /*CUSTOM_LOG(LM_FULL_INFO, 
               "MinorServo::PositionQueue::push()",
               (LM_DEBUG, 
               "pushing position at time: %s",
               (const char*)time2str(position.time)));*/
    boost::mutex::scoped_lock lock(_queue_guard);
    if(position.time <= _newest_time)
        return false;
    if(_size >= _max_size){
        _queue.pop_back();
        _oldest_time = _queue.back().time;
        --_size;
    }
    _queue.push_front(position);
    _newest_time = position.time;
    if(_size == 0)
    {
        _oldest_time = position.time;
    }
    ++_size;
    return true;
}

MedMinorServoPosition
PositionQueue::get_position()
{
    boost::mutex::scoped_lock lock(_queue_guard);
    if(_size == 0)
        throw EmptyPositionQueueError("no position stored");
    MedMinorServoPosition result = _queue.front();
    /*CUSTOM_LOG(LM_FULL_INFO, 
               "MinorServo::PositionQueue::get_position(time)",
               (LM_DEBUG, 
               "you requested for a position at most recent time: %s",
               (const char*)time2str(result.time)));*/
    return result;
}

MedMinorServoPosition
PositionQueue::get_position(ACS::Time time)
{
    boost::mutex::scoped_lock lock(_queue_guard);
    if(_size == 0)
        throw EmptyPositionQueueError("no position stored");
    if(_size == 1)
    {
        /*CUSTOM_LOG(LM_FULL_INFO, 
                   "MinorServo::PositionQueue::get_position(time)",
                   (LM_DEBUG, 
                   "you requested for a position at %llu but we have only one",
                   time));*/
        return _queue.front();
    }
    if(time >= _newest_time)
    {
        /*CUSTOM_LOG(LM_FULL_INFO, 
                   "MinorServo::PositionQueue::get_position(time)",
                   (LM_DEBUG, 
                   "you requested for a position at %llu that is in the future",
                   time));*/
        return _queue.front();
    }
    if(time <= _oldest_time)
    {
        /*CUSTOM_LOG(LM_FULL_INFO, 
                   "MinorServo::PositionQueue::get_position(time)",
                   (LM_DEBUG, 
                   "you requested for a position at %llu that is too past",
                   time));*/
        return _queue.back();
    }
    std::deque<MedMinorServoPosition>::iterator it = _queue.begin();
    MedMinorServoPosition before_position, after_position;
    do{
        after_position = *it;
        it++;
        before_position = *it;
    }while((before_position.time > time) && (it != _queue.end()));
    /*CUSTOM_LOG(LM_FULL_INFO, 
               "MinorServo::PositionQueue::get_position(time)",
               (LM_DEBUG, 
               "you requested for a position at %llu, found between %llu and %llu",
               time,
               before_position.time,
               after_position.time));*/
    return MedMinorServoGeometry::interpolate(before_position, after_position, time);
}

IRA::CString
PositionQueue::time2str(ACS::Time time)
{
    IRA::CString cstring;
    IRA::CIRATools::timeToStr(time, cstring);
    return cstring;
}

