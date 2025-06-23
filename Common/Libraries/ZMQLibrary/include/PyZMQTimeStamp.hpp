#ifndef __PYZMQTIMESTAMP_HPP__
#define __PYZMQTIMESTAMP_HPP__

#include "ZMQTimeStamp.hpp"
#include <boost/python.hpp>

namespace py = boost::python;
namespace ZMQ = ZMQLibrary;

struct JsonToPyString {
    static PyObject* convert(const ZMQ::ZMQDictionary& dict) {
        std::string s = dict.dump();
        py::object json_mod = py::import("json");
        py::object py_obj = json_mod.attr("loads")(s);
        return py::incref(py_obj.ptr());
    }
};

BOOST_PYTHON_MODULE(libPyZMQTimeStamp)
{
    py::to_python_converter<nlohmann::json, JsonToPyString>();

    py::def("fromMJD", ZMQ::ZMQTimeStamp::fromMJD);
    py::def("fromUNIXTime", ZMQ::ZMQTimeStamp::fromUNIXTime);
    py::def("fromACSTime", ZMQ::ZMQTimeStamp::fromACSTime);
}

#endif /*__PYZMQTIMESTAMP_HPP__*/
