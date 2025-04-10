#include "ZMQPublisher.hpp"

ZMQPublisher::ZMQPublisher(const std::string& topic, const std::string address, const unsigned int port) :
    topic(topic),
    m_topic(this->topic.data(), this->topic.size()),
    m_context(std::make_shared<zmq::context_t>()),
    m_socket(std::make_shared<zmq::socket_t>(*m_context, zmq::socket_type::pub))
{
    // Limit the outbound messages buffer to 2 (1 topic and 1 payload). This will assure we only send the last message after establishing a delayed connection.
    m_socket->set(zmq::sockopt::conflate, true);
    m_socket->connect("tcp://" + address + ":" + std::to_string(port));
}

ZMQPublisher::~ZMQPublisher()
{
    m_socket->close();
}

void ZMQPublisher::publish(const ZMQDictionary& dictionary)
{
    publish(dictionary.dump());
}

void ZMQPublisher::publish(const std::string& payload)
{
    std::string message = topic + " " + payload;
    zmq::message_t zmq_message(message.data(), message.size());
    m_socket->send(zmq_message, zmq::send_flags::none);
}
