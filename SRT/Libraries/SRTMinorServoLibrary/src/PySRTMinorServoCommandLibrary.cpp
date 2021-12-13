#include "PySRTMinorServoCommandLibrary.h"

std::string PySRTMinorServoCommandLibrary::preset(unsigned int servo_id, boost::python::list& coordinates)
{
    return SRTMinorServoCommandLibrary::preset(servo_id, PySRTMinorServoCommandLibrary::pylist2cppvector(coordinates));
}

std::string PySRTMinorServoCommandLibrary::programTrack(unsigned int servo_id, unsigned int trajectory_id, unsigned int point_id, boost::python::list& coordinates, double start_time)
{
    return SRTMinorServoCommandLibrary::programTrack(servo_id, trajectory_id, point_id, PySRTMinorServoCommandLibrary::pylist2cppvector(coordinates), start_time);
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
