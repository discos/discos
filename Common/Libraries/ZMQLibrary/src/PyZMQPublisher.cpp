#include "PyZMQPublisher.hpp"

void PyZMQPublisher::publish(const std::string& payload)
{
    ZMQ::ZMQPublisher::publish(payload);
}
