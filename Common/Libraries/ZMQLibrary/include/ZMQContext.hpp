#ifndef __ZMQCONTEXT_HPP__
#define __ZMQCONTEXT_HPP__

#include <zmq.hpp>
#include <memory>

namespace ZMQLibrary
{
    class ZMQContext
    {
    public:
        static std::shared_ptr<zmq::context_t> getInstance()
        {
            static std::shared_ptr<zmq::context_t> instance = std::make_shared<zmq::context_t>();
            return instance;
        }

    private:
        ZMQContext() = default;
        ~ZMQContext() = default;

        ZMQContext(const ZMQContext&) = delete;
        ZMQContext& operator=(const ZMQContext&) = delete;
    };
}
    
#endif /* __ZMQCONTEXT_HPP__ */
