#ifndef POSITIONQUEUE_HPP
#define POSITIONQUEUE_HPP

#include <deque>
#include <stdexcept>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include "MedMinorServoGeometry.hpp"

class EmptyPositionQueueError: public std::runtime_error
{
    public:
        EmptyPositionQueueError(const char *msg): std::runtime_error(std::string(msg)){};
};

class PositionQueue
{
    public:
        PositionQueue(int max_size);
        virtual ~PositionQueue(){};
        MedMinorServoPosition get_position(); // throw EmpyPositionQueueError
        MedMinorServoPosition get_position(ACS::Time time); // th  EmptyPositionQueueError
        /**
         * insert a new position as the head of the queue. Postion time must be
         * previous than current queue head in order to be inserted.
         * @return : true if position if actually inserted in the queue, false
         * otherwise
         */
        bool push(const MedMinorServoPosition& position);
        bool empty();
        void clear();
        int size(){ return _size;};
        int max_size(){ return _max_size;};
    private:
        boost::mutex _queue_guard;
        std::deque<MedMinorServoPosition> _queue;
        int _max_size;
        int _size;
        ACS::Time _oldest_time, _newest_time;
};

#endif 

