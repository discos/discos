#ifndef __ZMQPUBLISHER_HPP__
#define __ZMQPUBLISHER_HPP__

#include <unistd.h>
#include <limits.h>
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include "ZMQDictionary.hpp"

#define DEFAULT_ADDRESS std::string("127.0.0.1")
#define DEFAULT_PORT    16001

namespace ZMQLibrary
{
    /**
     * This class implements a publisher object over a ZeroMQ socket, single topic only.
     * It exposes a publish method which sends a dictionary of key, value format over the ZMQ socket.
     */
    class ZMQPublisher
    {
    public:
        /**
         * Constructors. Initializes the ZMQPublisher object with the given topic, address and port.
         */
        ZMQPublisher(const std::string& topic, const std::string address, const unsigned int port);
        ZMQPublisher(const std::string& topic) : ZMQPublisher(topic, DEFAULT_ADDRESS, DEFAULT_PORT) {};
        ZMQPublisher(const std::string& topic, const std::string address) : ZMQPublisher(topic, address, DEFAULT_PORT) {};
        ZMQPublisher(const std::string& topic, const unsigned int port) : ZMQPublisher(topic, DEFAULT_ADDRESS, port) {};

        /**
         * Destructor.
         */
        ~ZMQPublisher();

        /**
         * Class cannot be copied
         */
        ZMQPublisher(const ZMQPublisher&) = delete;
        ZMQPublisher& operator=(const ZMQPublisher&) = delete;

        /**
         * Public publisher method. This method accepts a ZMQDictionary object reference,
         * it converts it to a json-ified string and calls the protected publisher method.
         * @param dictionary, a json-like dictionary containing tuples of key, value format.
         */
        virtual void publish(const ZMQDictionary& dictionary);

        /**
         * Name of the topic on which the messages will be sent.
         */
        const std::string topic;

    protected:
        /**
         * Protected publisher method. This method sends a tuple (topic, payload) over the ZMQ socket.
         * @param payload, a json-ified string.
         */
        virtual void publish(const std::string& payload);

    private:
        /**
         * Function that creates and configures the socket
         */
        static std::shared_ptr<zmq::socket_t> create_socket(const std::shared_ptr<zmq::context_t>& ctx);

        /**
         * Function that retrieves a desired curve key from a file
         */
        static bool find_key(const std::string& file, const std::string& key_type, std::string& key);

        /**
         * ZMQ constant buffer which references the topic name.
         */
        const zmq::const_buffer m_topic;

        /**
         * ZMQ context shared pointer. We use a pointer since the context must not be destroyed for communications to work properly.
         */
        std::shared_ptr<zmq::context_t> m_context;

        /**
         * ZMQ socket shared pointer. We use a pointer since the socket must not be destroyed for communications to work properly.
         */
        std::shared_ptr<zmq::socket_t> m_socket;
    };
}

#endif /*__ZMQPUBLISHER_HPP__*/
