#include "PySRTMinorServoCommandLibrary.h"

std::string PySRTMinorServoCommandLibrary::preset(std::string servo_id, boost::python::list& coordinates)
{
    return SRTMinorServoCommandLibrary::preset(servo_id, PySRTMinorServoCommandLibrary::pylist2cppvector(coordinates));
}

std::string PySRTMinorServoCommandLibrary::programTrack(std::string servo_id, unsigned int trajectory_id, unsigned int point_id, boost::python::list& coordinates, double start_time)
{
    return SRTMinorServoCommandLibrary::programTrack(servo_id, trajectory_id, point_id, PySRTMinorServoCommandLibrary::pylist2cppvector(coordinates), start_time);
}

std::string PySRTMinorServoCommandLibrary::offset(std::string servo_id, boost::python::list& coordinates)
{
    return SRTMinorServoCommandLibrary::offset(servo_id, PySRTMinorServoCommandLibrary::pylist2cppvector(coordinates));
}

std::vector<double> PySRTMinorServoCommandLibrary::pylist2cppvector(boost::python::list& py_list)
{
    std::vector<double> cpp_vector;
    for(unsigned int i = 0; i < len(py_list); i++)
    {
        cpp_vector.push_back(boost::python::extract<double>(py_list[i]));
    }
    return cpp_vector;
}

boost::python::dict PySRTMinorServoCommandLibrary::parseAnswer(std::string answer)
{
    auto args = SRTMinorServoCommandLibrary::parseAnswer(answer);

    boost::python::dict dictionary;

    typename std::map<std::string, std::variant<int, double, std::string> >::iterator iter;

    for(iter = args.begin(); iter != args.end(); ++iter)
    {
        std::visit([dictionary, iter](const auto& var) mutable { dictionary[iter->first] = var; }, iter->second);
    }

    return dictionary;
}