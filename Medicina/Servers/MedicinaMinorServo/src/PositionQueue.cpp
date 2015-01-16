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
    return _queue.front();
}

MedMinorServoPosition
PositionQueue::get_position(ACS::Time time)
{
    boost::mutex::scoped_lock lock(_queue_guard);
    if(_size == 0)
        throw EmptyPositionQueueError("no position stored");
    if(_size == 1)
        return _queue.front();
    if(time >= _newest_time)
        return _queue.front();
    if(time <= _oldest_time)
        return _queue.back();
    std::deque<MedMinorServoPosition>::iterator it = _queue.begin();
    MedMinorServoPosition before_position, after_position;
    do{
        after_position = *it;
        it++;
        before_position = *it;
    }while((before_position.time > time) && (it != _queue.end()));
    return MedMinorServoGeometry::interpolate(before_position, after_position, time);
}

