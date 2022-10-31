#ifndef _PYSRTMINORSERVOCOMMANDLIBRARY_H
#define _PYSRTMINORSERVOCOMMANDLIBRARY_H

/**
 * PySRTMinorServoCommandLibrary.h
 * 2021/12/13
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include <boost/python.hpp>
#include "SRTMinorServoCommandLibrary.h"


/**
 * SRT Minor Servo Command Library Python Wrapper
 *
 * This class wraps the SRTMinorServoCommandLibrary with boost_python in order to grant access to its static functions via Python
 */
class PySRTMinorServoCommandLibrary : public SRTMinorServoCommandLibrary
{
public:
    /*
     * Builds the command used to move a single servo to a given set of coordinates
     * This is an overload of the original SRTMinorServoCommandLibrary::preset function
     * An overload was needed in order to handle the coordinates parameter as a Python list instead of a C++ std::vector
     * @param servo_id the ID of the single servo to be moved
     * @param coordinates a Python list containing the N coordinates to be sent to the servo
     * @return the composed message
     */
    static std::string preset(std::string servo_id, boost::python::list& coordinates);

    /*
     * Builds the command used to provide a single tracking set of coordinates to a single servo
     * This is an overload of the original SRTMinorServoCommandLibrary::programTrack function
     * An overload was needed in order to handle the coordinates parameter as a Python list instead of a C++ std::vector
     * @param servo_id the ID of the single servo to send the command to
     * @param trajectory_id the ID number of the trajectory the given set of coordinates belongs to
     * @param point_id the ID number of the given set of coordinates inside the trajectory
     * @param coordinates a Python list containing the N coordinates the servo have to move to at the given time
     * @param start_time only mandatory for the first point in the trajectory, a double representing the UNIX epoch of the starting instant of the trajectory
     * @return the composed message
     */
    static std::string programTrack(std::string servo_id, unsigned int trajectory_id, unsigned int point_id, boost::python::list& coordinates, double start_time=-1);

    /*
     * Builds the command used to provide a set of offsets to a given servo
     * This is an overload of the original SRTMinorServoCommandLibrary::offset function
     * An overload was needed in order to handle the coordinates parameter as a Python list instead of a C++ std::vector
     * @param servo_id the ID of the single servo to be moved
     * @param coordinates a Python list containing the N coordinates to be sent to the servo
     * @return the composed message
     */
    static std::string offset(std::string servo_id, boost::python::list& coordinates);

    /*
     * Parses the received answer by splitting it and synamically populating a std::map
     * This is an overload of the original SRTMinorServoCommandLibrary::parseAnswer function
     * An overload was needed in order to replace the original std::map object with a boost::python::dict
     * @param answer the string containing the answer received from the VBrain proxy
     * @return a Python dictionary containing the answer splitted into keys and values. The keys are always strings, the values can either be int, double or strings.
     */
    static boost::python::dict parseAnswer(std::string answer);

private:
    /*
     * Converts the given Python list into a C++ std::vector object
     * @param py_list the given Python list to be converted
     * @return the composed C++ std::vector containing doubles
     */
    static std::vector<double> pylist2cppvector(boost::python::list& py_list);
};

/*
 * The following 3 lines of code allow the overloaded functions to ignore the optional parameter and use the default one defined in the original SRTMinorServoCommandLibrary header file
 */
BOOST_PYTHON_FUNCTION_OVERLOADS(status, PySRTMinorServoCommandLibrary::status, 0, 1)
BOOST_PYTHON_FUNCTION_OVERLOADS(stow, PySRTMinorServoCommandLibrary::stow, 1, 2)
BOOST_PYTHON_FUNCTION_OVERLOADS(programTrack, PySRTMinorServoCommandLibrary::programTrack, 4, 5)

/*
 * Python module definition. Since the original SRTMinorServoCommandLibrary only contains static functions, we write the Python module with static functions only, omitting the class
 */
BOOST_PYTHON_MODULE(libPySRTMinorServoCommandLibrary)
{
    using namespace boost::python;
    def("status", &PySRTMinorServoCommandLibrary::status, status(arg("servo_id") = ""));
    def("setup", &PySRTMinorServoCommandLibrary::setup);
    def("stow", &PySRTMinorServoCommandLibrary::stow, stow(arg("stow_position") = 1));
    def("stop", &PySRTMinorServoCommandLibrary::stop);
    def("preset", &PySRTMinorServoCommandLibrary::preset);
    def("programTrack", &PySRTMinorServoCommandLibrary::programTrack, programTrack(arg("start_time") = -1));
    def("offset", &PySRTMinorServoCommandLibrary::offset);
    def("parseAnswer", &PySRTMinorServoCommandLibrary::parseAnswer);
}
#endif
