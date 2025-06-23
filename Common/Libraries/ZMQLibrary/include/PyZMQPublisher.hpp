#ifndef __PYZMQPUBLISHER_HPP__
#define __PYZMQPUBLISHER_HPP__

#include "ZMQPublisher.hpp"
#include <boost/python.hpp>

namespace py = boost::python;
namespace ZMQ = ZMQLibrary;

/**
 * Wrapper for the above ZMQPublisher class in order to be used with boost_python.
 */
class PyZMQPublisher : public ZMQ::ZMQPublisher
{
public:
    using ZMQPublisher::ZMQPublisher;

    /**
     * Re-definition of the base protected publish method as public.
     * We cannot use "using" because it won't work with boost_python.
     * @param payload, a json-ified string.
     */
    void publish(const std::string& payload) override;
};


/**
 * Python module definition.
 */
BOOST_PYTHON_MODULE(libPyZMQPublisher)
{
    /**
     * Expose DEFAULT_ADDRESS and DEFAULT_PORT to the Python module.
     */
    py::object scope = py::scope();
    scope.attr("DEFAULT_ADDRESS") = DEFAULT_ADDRESS;
    scope.attr("DEFAULT_PORT") = DEFAULT_PORT;

    /**
     * Python PyZMQPublisher class definition. It is a wrapper of the PyZMQPublisher class defined above.
     * Unlike the PyZMQPublisher class, the "publish" method is exposed as a private method "__publish".
     * The actual "publish" method is defined inside the actual python module in order to convert a python dict() to a json-ified string.
     * Take a look there for more info.
     */
    py::class_<PyZMQPublisher>("PyZMQPublisher", py::init<const std::string, const std::string, const unsigned int>())
        .def("__publish", &PyZMQPublisher::publish)
        .def_readonly("topic", &ZMQ::ZMQPublisher::topic);
}

#endif /*__PYZMQPUBLISHER_HPP__*/
