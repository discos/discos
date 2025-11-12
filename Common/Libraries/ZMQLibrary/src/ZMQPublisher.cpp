#include "ZMQPublisher.hpp"

namespace ZMQLibrary
{
    ZMQPublisher::ZMQPublisher(const std::string& topic, const std::string address, const unsigned int port) :
        topic(topic),
        m_topic(this->topic.data(), this->topic.size()),
        m_context(std::make_shared<zmq::context_t>()),
        m_socket(std::make_shared<zmq::socket_t>(*m_context, zmq::socket_type::pub))
    {
        m_socket->set(zmq::sockopt::linger, 0);
        m_socket->set(zmq::sockopt::immediate, true);
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

    void ZMQPublisher::publish(const std::string& payload_str)
    {
        zmq::const_buffer payload(payload_str.data(), payload_str.size());
        std::array<zmq::const_buffer, 2> message = { m_topic, payload };
        zmq::send_multipart(*m_socket, message);
    }
}
