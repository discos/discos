#include "PySRTMinorServoCommandLibrary.h"

using namespace MinorServo;

boost::python::object PySRTMinorServoCommandLibrary::status(const std::string servo_id)
{
    return stringToBytes(SRTMinorServoCommandLibrary::status(servo_id));
}

boost::python::object PySRTMinorServoCommandLibrary::setup(const std::string& configuration)
{
    return stringToBytes(SRTMinorServoCommandLibrary::setup(configuration));
}

boost::python::object PySRTMinorServoCommandLibrary::stow(const std::string& servo_id, unsigned int stow_position)
{
    return stringToBytes(SRTMinorServoCommandLibrary::stow(servo_id, stow_position));
}

boost::python::object PySRTMinorServoCommandLibrary::stop(const std::string& servo_id)
{
    return stringToBytes(SRTMinorServoCommandLibrary::stop(servo_id));
}

boost::python::object PySRTMinorServoCommandLibrary::preset(const std::string& servo_id, const boost::python::list& coordinates)
{
    return stringToBytes(SRTMinorServoCommandLibrary::preset(servo_id, pylist2cppvector(coordinates)));
}

boost::python::object PySRTMinorServoCommandLibrary::programTrack(const std::string& servo_id, const unsigned long& trajectory_id, const unsigned long& point_id, const boost::python::list& coordinates, double start_time)
{
    return stringToBytes(SRTMinorServoCommandLibrary::programTrack(servo_id, trajectory_id, point_id, pylist2cppvector(coordinates), start_time));
}

boost::python::object PySRTMinorServoCommandLibrary::offset(const std::string& servo_id, const boost::python::list& coordinates)
{
    return stringToBytes(SRTMinorServoCommandLibrary::offset(servo_id, pylist2cppvector(coordinates)));
}

boost::python::dict PySRTMinorServoCommandLibrary::parseAnswer(const std::string& answer)
{
    auto args = SRTMinorServoCommandLibrary::parseAnswer(answer);

    boost::python::dict dictionary;

    SRTMinorServoAnswerMap::iterator iter;

    for(iter = args.begin(); iter != args.end(); ++iter)
    {
        std::visit([dictionary, iter](const auto& var) mutable { dictionary[iter->first] = var; }, iter->second);
    }

    return dictionary;
}

std::vector<double> PySRTMinorServoCommandLibrary::pylist2cppvector(const boost::python::list& py_list)
{
    std::vector<double> cpp_vector;
    for(unsigned int i = 0; i < len(py_list); i++)
    {
        cpp_vector.push_back(boost::python::extract<double>(py_list[i]));
    }
    return cpp_vector;
}

boost::python::object PySRTMinorServoCommandLibrary::stringToBytes(const std::string& command)
{
    return boost::python::object(boost::python::handle<>(PyBytes_FromString(command.c_str())));
}
