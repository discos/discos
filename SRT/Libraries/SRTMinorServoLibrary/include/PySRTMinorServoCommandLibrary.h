#ifndef _PYSRTMINORSERVOCOMMANDLIBRARY_H
#define _PYSRTMINORSERVOCOMMANDLIBRARY_H

/**
 * PySRTMinorServoCommandLibrary.h
 * 2021/12/13
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include <boost/python.hpp>
#include <Python.h>
#include "SRTMinorServoCommandLibrary.h"


namespace MinorServo
{
    /**
     * SRT Minor Servo Command Library Python Wrapper
     *
     * This class wraps the SRTMinorServoCommandLibrary with boost_python in order to grant access to its static functions via Python
     */
    class PySRTMinorServoCommandLibrary : public SRTMinorServoCommandLibrary
    {
    public:
        /**
         * Builds the command used to ask the general status of the minor servos or, eventually, a single servo
         * @param servo_id the ID string of the eventual single servo to retrieve the status. Send no servo_id argument to retrieve the general status of the system
         * @return the composed message
         */
        static boost::python::object status(const std::string servo_id = "");

        /**
         * Builds the command used to configure the telescope for a specific focal path
         * @param configuration the desired focal path to command to the minor servo systems
         * @return the composed message
         */
        static boost::python::object setup(const std::string& configuration);

        /*
         * Builds the command used to stow a single servo system to a given stow position
         * @param servo_id the ID string of the single servo to be stowed
         * @param stow_position the position to which the servo have to stow to
         * @return the composed message
         */
        static boost::python::object stow(const std::string& servo_id, unsigned int stow_position = 1);

        /*
         * Builds the command used to stop a single servo system
         * @param servo_id the ID string of the single servo to be stopped
         * @return the composed message
         */
        static boost::python::object stop(const std::string& servo_id);

        /*
         * Builds the command used to move a single servo to a given set of coordinates
         * This is an overload of the original SRTMinorServoCommandLibrary::preset function
         * @param servo_id the ID of the single servo to be moved
         * @param coordinates a Python list containing the N coordinates to be sent to the servo
         * @return the composed message
         */
        static boost::python::object preset(const std::string& servo_id, const boost::python::list& coordinates);

        /*
         * Builds the command used to provide a single tracking set of coordinates to a single servo
         * This is an overload of the original SRTMinorServoCommandLibrary::programTrack function
         * @param servo_id the ID of the single servo to send the command to
         * @param trajectory_id the ID number of the trajectory the given set of coordinates belongs to
         * @param point_id the ID number of the given set of coordinates inside the trajectory
         * @param coordinates a Python list containing the N coordinates the servo have to move to at the given time
         * @param start_time only mandatory for the first point in the trajectory, a double representing the UNIX epoch of the starting instant of the trajectory
         * @return the composed message
         */
        static boost::python::object programTrack(const std::string& servo_id, const unsigned long& trajectory_id, const unsigned long& point_id, const boost::python::list& coordinates, double start_time=-1);

        /*
         * Builds the command used to provide a set of offsets to a given servo
         * This is an overload of the original SRTMinorServoCommandLibrary::offset function
         * @param servo_id the ID of the single servo to be moved
         * @param coordinates a Python list containing the N coordinates to be sent to the servo
         * @return the composed message
         */
        static boost::python::object offset(const std::string& servo_id, const boost::python::list& coordinates);

        /*
         * Parses the received answer by splitting it and synamically populating a std::map
         * This is an overload of the original SRTMinorServoCommandLibrary::parseAnswer function
         * @param answer the string containing the answer received from the VBrain proxy
         * @return a Python dictionary containing the answer splitted into keys and values. The keys are always strings, the values can either be int, double or strings.
         */
        static boost::python::dict parseAnswer(const std::string& answer);
    private:
        /*
         * Converts the given Python list into a C++ std::vector object
         * @param py_list the given Python list to be converted
         * @return the composed C++ std::vector containing doubles
         */
        static std::vector<double> pylist2cppvector(const boost::python::list& py_list);

        /**
         * Converts the given std::string to a Python bytestring
         * @param command a reference to the given command string
         * @return the bytestring containing the given command string
         */
        static boost::python::object stringToBytes(const std::string& command);
    };
}

/*
 * The following 3 lines of code allow the overloaded functions to ignore the optional parameter and use the default one defined in the original SRTMinorServoCommandLibrary header file
 */
BOOST_PYTHON_FUNCTION_OVERLOADS(status, MinorServo::PySRTMinorServoCommandLibrary::status, 0, 1)
BOOST_PYTHON_FUNCTION_OVERLOADS(stow, MinorServo::PySRTMinorServoCommandLibrary::stow, 1, 2)
BOOST_PYTHON_FUNCTION_OVERLOADS(programTrack, MinorServo::PySRTMinorServoCommandLibrary::programTrack, 4, 5)

/*
 * Python module definition. Since the original SRTMinorServoCommandLibrary only contains static functions, we write the Python module with static functions only, omitting the class
 */
BOOST_PYTHON_MODULE(libPySRTMinorServoCommandLibrary)
{
    using namespace boost::python;
    def("status", &MinorServo::PySRTMinorServoCommandLibrary::status, status(arg("servo_id") = ""));
    def("setup", &MinorServo::PySRTMinorServoCommandLibrary::setup);
    def("stow", &MinorServo::PySRTMinorServoCommandLibrary::stow, stow(arg("stow_position") = 1));
    def("stop", &MinorServo::PySRTMinorServoCommandLibrary::stop);
    def("preset", &MinorServo::PySRTMinorServoCommandLibrary::preset);
    def("programTrack", &MinorServo::PySRTMinorServoCommandLibrary::programTrack, programTrack(arg("start_time") = -1));
    def("offset", &MinorServo::PySRTMinorServoCommandLibrary::offset);
    def("parseAnswer", &MinorServo::PySRTMinorServoCommandLibrary::parseAnswer);
}
#endif
