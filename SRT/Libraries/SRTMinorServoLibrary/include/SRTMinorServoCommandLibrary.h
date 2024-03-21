#ifndef _SRTMINORSERVOCOMMANDLIBRARY_H
#define _SRTMINORSERVOCOMMANDLIBRARY_H

/**
 * SRTMinorServoCommandLibrary.h
 * 2021/12/07
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include "SRTMinorServoContainers.h"
#include <sstream>
#include <vector>
#include <map>
#include <variant>


#define CLOSER std::string("\r\n")


namespace MinorServo
{
    /**
     * SRT Minor Servo Command Library
     *
     * This class features static functions used to build commands to be sent to the PON minor servo control unit of the Sardinia Radio Telescope
     */
    class SRTMinorServoCommandLibrary
    {
    public:
        /**
         * Builds the command used to ask the status of the MSCU or, eventually, a single servo
         * @param servo_id the ID string of the eventual single servo to retrieve the status. Send no servo_id argument to retrieve the general status of the system
         * @return the composed message
         */
        static std::string status(const std::string servo_id = "");

        /**
         * Builds the command used to configure the telescope for a specific focal path
         * @param configuration the desired focal path to command to the minor servo systems
         * @return the composed message
         */
        static std::string setup(const std::string& configuration);

        /*
         * Builds the command used to stow a single servo system to a given stow position
         * @param servo_id the ID string of the single servo to be stowed
         * @param stow_position the position to which the servo have to stow to
         * @return the composed message
         */
        static std::string stow(const std::string& servo_id, const unsigned int stow_position = 1);

        /*
         * Builds the command used to stop a single servo system
         * @param servo_id the ID string of the single servo to be stopped
         * @return the composed message
         */
        static std::string stop(const std::string& servo_id);

        /*
         * Builds the command used to move a single servo to a given set of virtual coordinates
         * @param servo_id the ID string of the single servo to be moved
         * @param coordinates a vector containing the N coordinates to be sent to the servo
         * @return the composed message
         */
        static std::string preset(const std::string& servo_id, const std::vector<double>& coordinates);

        /*
         * Builds the command used to provide a single tracking point of virtual coordinates to a single servo
         * @param servo_id the ID string of the single servo to send the command to
         * @param trajectory_id the ID number of the trajectory the given set of coordinates belongs to
         * @param point_id the ID number of the given set of coordinates inside the trajectory
         * @param coordinates a vector containing the N coordinates the servo have to move to at the given time
         * @param start_time only mandatory for the first point in the trajectory, a double representing the UNIX Epoch of the starting instant of the trajectory
         * @return the composed message
         */
        static std::string programTrack(const std::string& servo_id, const unsigned long& trajectory_id, const unsigned long& point_id, const std::vector<double>& coordinates, const double start_time = 0);

        /*
         * Builds the command used to send a set of virtual offsets to a single servo
         * @param servo_id the ID string of the single servo to send the offsets to
         * @param coordinates a vector containing the N offsets to be added the servo coordinates
         * @return the composed message
         */
        static std::string offset(const std::string& servo_id, const std::vector<double>& coordinates);

        /*
         * Parses the received answer by splitting it and dynamically populating a SRTMinorServoAnswerMap object
         * @param original_answer the string containing the answer received from the VBrain proxy
         * @return a SRTMinorServoAnswerMap dictionary containing the answer splitted into keys and values. The keys are always std::string, the values can either be long, double or std::string.
         */
        static SRTMinorServoAnswerMap parseAnswer(const std::string& original_answer);
    };
}

#endif
