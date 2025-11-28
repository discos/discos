#include "ZMQPublisher.hpp"

namespace ZMQLibrary
{
    ZMQPublisher::ZMQPublisher(const std::string& topic, const std::string address, const unsigned int port) :
        topic(topic),
        m_topic(this->topic.data(), this->topic.size()),
        m_context(std::make_shared<zmq::context_t>()),
        m_socket(create_socket(m_context))
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

    std::shared_ptr<zmq::socket_t> ZMQPublisher::create_socket(const std::shared_ptr<zmq::context_t>& context)
    {
        std::shared_ptr<zmq::socket_t> socket(new zmq::socket_t(*context, zmq::socket_type::pub));

        if(zmq_has("curve"))
        {
            try
            {
                char fqdn[HOST_NAME_MAX + 1] = {0};
                gethostname(fqdn, sizeof(fqdn) - 1);
                std::string hostname(fqdn);
                std::size_t dot = hostname.find('.');
                if(dot != std::string::npos)
                {
                    hostname = hostname.substr(0, dot);
                }
                std::string home = std::getenv("HOME");
                std::string client_key_file = home + "/.curve/discos/telemetry/client/identity.key_secret";
                std::string server_key_file = home + "/.curve/discos/telemetry/client/server.key";
                std::string curve_publickey, curve_secretkey, curve_serverkey;

                if(find_key(client_key_file, "public-key", curve_publickey) &&
                   find_key(client_key_file, "secret-key", curve_secretkey) &&
                   find_key(server_key_file, "public-key", curve_serverkey))
                {
                    socket->set(zmq::sockopt::curve_publickey, curve_publickey);
                    socket->set(zmq::sockopt::curve_secretkey, curve_secretkey);
                    socket->set(zmq::sockopt::curve_serverkey, curve_serverkey);
                }
            }
            catch(const zmq::error_t& e)
            {
                socket.reset(new zmq::socket_t(*context, zmq::socket_type::pub));
            }
        }

        return socket;
    }

    bool ZMQPublisher::find_key(const std::string& path, const std::string& key_type, std::string& key)
    {
        std::ifstream file(path.c_str());
        if(!file.is_open())
        {
            return false;
        }

        std::string line;
        while(std::getline(file, line))
        {
            if(line.find(key_type) == std::string::npos)
            {
                continue;
            }

            std::size_t start = line.find('"');
            if(start == std::string::npos)
            {
                continue;
            }

            std::size_t end = line.find('"', start + 1);
            if(end == std::string::npos)
            {
                continue;
            }

            key = line.substr(start + 1, end - (start + 1));
            if(key.size() == 40)
            {
                return true;
            }
            break;
        }

        return false;
    }
}
