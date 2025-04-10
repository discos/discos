#include "PyZMQPublisher.hpp"

void PyZMQPublisher::publish(const std::string& payload)
{
    ZMQPublisher::publish(payload);
}
